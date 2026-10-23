/*++

Copyright (C) 2020 Autodesk Inc.

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Autodesk Inc. nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL AUTODESK INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


Abstract: This is a stub class definition of CStreamConnection

*/

#include "libmc_streamconnection.hpp"
#include "libmc_interfaceexception.hpp"

// Include custom headers here.
#include "common_utils.hpp"
#include "libmc_streamdata.hpp"

#define STREAMCONNECTION_MICROSECONDS_PER_MILLISECOND 1000
#define STREAMCONNECTION_DEFAULT_IDLE_DELAY_IN_MS 100
#define STREAMCONNECTION_MIN_IDLE_DELAY_IN_MS 5
#define STREAMCONNECTION_MAX_IDLE_DELAY_IN_MS 100
#define STREAMCONNECTION_IDLE_DELAY_FRAMEDURATION_DIVISOR 2
#define STREAMCONNECTION_JSONEVENT_HEARTBEAT_INTERVAL_IN_MS 15000
#define STREAMCONNECTION_JSONEVENT_MIMETYPE "text/event-stream"

using namespace LibMC::Impl;

#include <iostream>

/*************************************************************************************************************************
 Class definition of CStreamConnection 
**************************************************************************************************************************/

CStreamConnection::CStreamConnection(const std::string& sStreamUUID, AMC::PStreamInstance pStream, const std::string& sClientID)
    : m_sStreamUUID (AMCCommon::CUtils::normalizeUUIDString (sStreamUUID)),
    m_pStream (pStream),
    m_nLastFrameVersion (0),
    m_LastJSONEventTime (std::chrono::steady_clock::now ())
{
    m_JSONEventCursor.m_sClientID = sClientID;

    // Cache the downcast for video streams
    if (pStream.get() != nullptr) {
        m_pVideoStream = std::dynamic_pointer_cast<AMC::CVideoStreamInstance>(pStream);
        m_pJSONEventStream = std::dynamic_pointer_cast<AMC::CJSONEventStreamInstance>(pStream);
    }
}


CStreamConnection::~CStreamConnection()
{

}


IStreamData* CStreamConnection::getNewJSONEventContent()
{
    std::string sEvent = m_pJSONEventStream->waitForNextEvent(m_JSONEventCursor);

    auto now = std::chrono::steady_clock::now();
    if (sEvent.empty()) {
        auto nIdleTimeInMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_LastJSONEventTime).count();
        if (nIdleTimeInMs < STREAMCONNECTION_JSONEVENT_HEARTBEAT_INTERVAL_IN_MS)
            return nullptr;

        sEvent = AMC::CJSONEventStreamInstance::formatComment("keepalive");
    }

    m_LastJSONEventTime = now;

    std::unique_ptr<CStreamData> pStreamData(new CStreamData(STREAMCONNECTION_JSONEVENT_MIMETYPE));
    auto& buffer = pStreamData->getBuffer();
    buffer.assign(sEvent.begin(), sEvent.end());

    return pStreamData.release();
}

IStreamData * CStreamConnection::GetNewContent()
{
    if (m_pJSONEventStream.get() != nullptr)
        return getNewJSONEventContent();

    if (m_pVideoStream.get() == nullptr)
        return nullptr;

    std::vector<uint8_t> jpegBuffer;
    uint64_t newVersion = 0;

    if (!m_pVideoStream->getLatestJPEGFrame(jpegBuffer, m_nLastFrameVersion, newVersion))
        return nullptr;

    m_nLastFrameVersion = newVersion;

    std::unique_ptr<CStreamData> pStreamData(new CStreamData("image/jpeg"));
    auto & buffer = pStreamData->getBuffer();
    buffer = std::move(jpegBuffer);

    return pStreamData.release();
}

uint32_t CStreamConnection::GetIdleDelay()
{
    // JSON event streams block inside GetNewContent until there is something to send.
    if (m_pJSONEventStream.get() != nullptr)
        return 0;

    if (m_pVideoStream.get() == nullptr)
        return STREAMCONNECTION_DEFAULT_IDLE_DELAY_IN_MS;

    // Delay should be a fraction of the frame duration for responsive streaming
    uint32_t nFrameDurationMs = m_pVideoStream->getDesiredFrameDurationInMicroseconds() / STREAMCONNECTION_MICROSECONDS_PER_MILLISECOND;
    if (nFrameDurationMs < STREAMCONNECTION_MIN_IDLE_DELAY_IN_MS)
        return STREAMCONNECTION_MIN_IDLE_DELAY_IN_MS;
    if (nFrameDurationMs > STREAMCONNECTION_MAX_IDLE_DELAY_IN_MS)
        return STREAMCONNECTION_MAX_IDLE_DELAY_IN_MS;
    return nFrameDurationMs / STREAMCONNECTION_IDLE_DELAY_FRAMEDURATION_DIVISOR;
}

LibMC::eStreamConnectionType CStreamConnection::GetStreamType()
{
    if (m_pStream.get() == nullptr)
        return LibMC::eStreamConnectionType::Unknown;

    switch (m_pStream->getStreamType()) {
    case AMC::eStreamType::VideoStream:
        return LibMC::eStreamConnectionType::JPEGImageStream;
    case AMC::eStreamType::JSONEventStream:
        return LibMC::eStreamConnectionType::JSONEventStream;
    default:
        return LibMC::eStreamConnectionType::Unknown;
    }
}

void CStreamConnection::SetResumeEventID(const LibMC_uint64 nEventID)
{
    if (m_JSONEventCursor.m_bStarted)
        throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

    m_JSONEventCursor.m_nLastEventID = nEventID;
}

bool CStreamConnection::IsActive()
{
    if (m_pStream.get() == nullptr)
        return false;

    if (m_pJSONEventStream.get() != nullptr)
        return m_pJSONEventStream->isActive();

    // A video stream's isActive only reports whether frames are arriving; a paused video stream keeps its connections.
    return true;
}
