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

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL AUTODESK INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Abstract: This is the class definition of CVideoStreamInstance.

*/

#include "amc_videostreaminstance.hpp"
#include "common_utils.hpp"
#include "common_jpeg.hpp"
#include "libmc_exceptiontypes.hpp"

#include <cstring>

using namespace AMC;

CVideoStreamInstance::CVideoStreamInstance(
	const std::string& sUUID,
	uint32_t nWidth,
	uint32_t nHeight,
	uint32_t nDesiredFrameDurationInMicroseconds,
	uint32_t nPauseToleranceInMicroseconds,
	uint32_t nFrameCacheDurationInMicroseconds)
	: CStreamInstance(sUUID),
	m_nWidth(nWidth),
	m_nHeight(nHeight),
	m_nDesiredFrameDurationInMicroseconds(nDesiredFrameDurationInMicroseconds),
	m_nPauseToleranceInMicroseconds(nPauseToleranceInMicroseconds),
	m_nFrameCacheDurationInMicroseconds(nFrameCacheDurationInMicroseconds),
	m_nFrameCount(0),
	m_nDroppedFrameCount(0),
	m_nCurrentFrameVersion(0)
{
	if (nWidth == 0 || nHeight == 0)
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDPARAM, "Video stream dimensions must be positive.");

	if (nDesiredFrameDurationInMicroseconds < 10000 || nDesiredFrameDurationInMicroseconds > 60000000)
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDPARAM, "Frame duration must be between 10000 and 60000000 microseconds.");

	if (nPauseToleranceInMicroseconds <= nDesiredFrameDurationInMicroseconds)
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDPARAM, "Pause tolerance must exceed frame duration.");

	if (nFrameCacheDurationInMicroseconds < nDesiredFrameDurationInMicroseconds)
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDPARAM, "Frame cache duration must not be smaller than frame duration.");

	if (nFrameCacheDurationInMicroseconds > 100 * nDesiredFrameDurationInMicroseconds)
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDPARAM, "Frame cache duration must not exceed 100 times frame duration.");

	m_StreamStartTime = std::chrono::steady_clock::now();
	m_LastSourceTime = m_StreamStartTime;
}

CVideoStreamInstance::~CVideoStreamInstance()
{
}

eStreamType CVideoStreamInstance::getStreamType() const
{
	return eStreamType::VideoStream;
}

uint32_t CVideoStreamInstance::getWidth() const
{
	return m_nWidth;
}

uint32_t CVideoStreamInstance::getHeight() const
{
	return m_nHeight;
}

uint32_t CVideoStreamInstance::getDesiredFrameDurationInMicroseconds() const
{
	return m_nDesiredFrameDurationInMicroseconds;
}

double CVideoStreamInstance::getDesiredFramerate() const
{
	return 1000000.0 / (double)m_nDesiredFrameDurationInMicroseconds;
}

uint32_t CVideoStreamInstance::getPauseToleranceInMicroseconds() const
{
	return m_nPauseToleranceInMicroseconds;
}

uint32_t CVideoStreamInstance::getFrameCacheDurationInMicroseconds() const
{
	return m_nFrameCacheDurationInMicroseconds;
}

uint32_t CVideoStreamInstance::getFrameCount() const
{
	std::lock_guard<std::mutex> lock(m_Mutex);
	return m_nFrameCount;
}

uint32_t CVideoStreamInstance::getDroppedFrameCount() const
{
	std::lock_guard<std::mutex> lock(m_Mutex);
	return m_nDroppedFrameCount;
}

bool CVideoStreamInstance::isActive() const
{
	std::lock_guard<std::mutex> lock(m_Mutex);

	if (m_nFrameCount == 0)
		return false;

	auto now = std::chrono::steady_clock::now();
	auto elapsedMicroseconds = std::chrono::duration_cast<std::chrono::microseconds>(now - m_LastSourceTime).count();

	return (elapsedMicroseconds <= (int64_t)m_nPauseToleranceInMicroseconds);
}

uint64_t CVideoStreamInstance::getStreamStartTimeMicroseconds1970() const
{
	auto epoch = m_StreamStartTime.time_since_epoch();
	auto systemNow = std::chrono::system_clock::now();
	auto steadyNow = std::chrono::steady_clock::now();
	auto systemEpoch = systemNow - (steadyNow - m_StreamStartTime);
	return std::chrono::duration_cast<std::chrono::microseconds>(systemEpoch.time_since_epoch()).count();
}

uint64_t CVideoStreamInstance::getLastSourceTimeMicroseconds() const
{
	std::lock_guard<std::mutex> lock(m_Mutex);
	auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(m_LastSourceTime - m_StreamStartTime);
	return (uint64_t)elapsed.count();
}

void CVideoStreamInstance::pushFrame(const uint8_t* pRGB24Data, uint32_t nDataSize)
{
	if (pRGB24Data == nullptr)
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDPARAM, "RGB24 data pointer is null.");

	uint32_t nExpectedSize = m_nWidth * m_nHeight * 3;
	if (nDataSize != nExpectedSize)
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDPARAM, "RGB24 data size mismatch. Expected " + std::to_string(nExpectedSize) + " bytes, got " + std::to_string(nDataSize) + " bytes.");

	// Encode to JPEG
	std::vector<uint8_t> jpegData;
	AMCCommon::CJPEGImageEncoder encoder(m_nWidth, m_nHeight, AMCCommon::eJPEGChannelCount::ccRGB, pRGB24Data, jpegData, false);

	if (jpegData.empty())
		throw ELibMCCustomException(LIBMC_ERROR_INTERNALERROR, "JPEG encoding produced empty output.");

	std::lock_guard<std::mutex> lock(m_Mutex);

	m_CurrentJPEGFrame = std::move(jpegData);

	// Store a copy of the RGB24 data
	m_CurrentRGB24Frame.resize(nDataSize);
	std::memcpy(m_CurrentRGB24Frame.data(), pRGB24Data, nDataSize);

	m_LastSourceTime = std::chrono::steady_clock::now();
	m_nFrameCount++;
	m_nCurrentFrameVersion++;
}

bool CVideoStreamInstance::getLatestJPEGFrame(std::vector<uint8_t>& jpegBuffer, uint64_t lastKnownVersion, uint64_t& newVersion) const
{
	std::lock_guard<std::mutex> lock(m_Mutex);

	newVersion = m_nCurrentFrameVersion;

	if (m_nCurrentFrameVersion == 0 || m_nCurrentFrameVersion == lastKnownVersion)
		return false;

	jpegBuffer = m_CurrentJPEGFrame;
	return true;
}

bool CVideoStreamInstance::getLatestJPEGFrame(std::vector<uint8_t>& jpegBuffer) const
{
	std::lock_guard<std::mutex> lock(m_Mutex);

	if (m_nCurrentFrameVersion == 0)
		return false;

	jpegBuffer = m_CurrentJPEGFrame;
	return true;
}

std::vector<uint8_t> CVideoStreamInstance::getLastRGB24Frame() const
{
	std::lock_guard<std::mutex> lock(m_Mutex);
	return m_CurrentRGB24Frame;
}

uint64_t CVideoStreamInstance::getCurrentFrameVersion() const
{
	std::lock_guard<std::mutex> lock(m_Mutex);
	return m_nCurrentFrameVersion;
}
