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


Abstract: This is a stub class definition of CTelemetryChunkData

*/

#include "libmcdata_telemetrychunkdata.hpp"
#include "libmcdata_interfaceexception.hpp"

// Include custom headers here.

using namespace LibMCData::Impl;

/*************************************************************************************************************************
 Class definition of CTelemetryChunkData 
**************************************************************************************************************************/


CTelemetryChunkData::CTelemetryChunkData(uint64_t nChunkIndex)
    : m_nChunkIndex(nChunkIndex),
    m_nStartTimeStamp(0),
    m_nEndTimeStamp(0)
{

}


CTelemetryChunkData::~CTelemetryChunkData()
{

}

LibMCData_uint64 CTelemetryChunkData::GetChunkIndex()
{
    return m_nChunkIndex;
}

LibMCData_uint64 CTelemetryChunkData::GetStartTimeStamp()
{
    return m_nStartTimeStamp;
}

LibMCData_uint64 CTelemetryChunkData::GetEndTimeStamp()
{
    return m_nEndTimeStamp;
}

LibMCData_uint64 CTelemetryChunkData::GetEntryCount()
{
    return m_Entries.size();
}

void CTelemetryChunkData::GetEntries(LibMCData_uint64 nTelemetryEntriesBufferSize, LibMCData_uint64* pTelemetryEntriesNeededCount, LibMCData::sTelemetryChunkEntry* pTelemetryEntriesBuffer)
{
    if (pTelemetryEntriesNeededCount != nullptr)
        *pTelemetryEntriesNeededCount = m_Entries.size();

    if (pTelemetryEntriesBuffer != nullptr) {
        if (nTelemetryEntriesBufferSize < m_Entries.size())
            throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_BUFFERTOOSMALL);

        LibMCData::sTelemetryChunkEntry* pTarget = pTelemetryEntriesBuffer;
        for (auto& entry : m_Entries) {
            *pTarget = entry;
            pTarget++;
        }
    }
}

std::vector<LibMCData::sTelemetryChunkEntry>& CTelemetryChunkData::getEntriesInternal()
{
    return m_Entries;
}

void CTelemetryChunkData::setTimeInterval(uint64_t nStartTimeStamp, uint64_t nEndTimeStamp)
{
    m_nStartTimeStamp = nStartTimeStamp;
    m_nEndTimeStamp = nEndTimeStamp;
}
