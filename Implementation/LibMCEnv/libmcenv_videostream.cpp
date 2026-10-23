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


Abstract: This is the class definition of CVideoStream

*/

#include "libmcenv_videostream.hpp"
#include "libmcenv_interfaceexception.hpp"
#include "libmcenv_imagedata.hpp"
#include "libmcenv_datetime.hpp"

// Include custom headers here.

using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CVideoStream 
**************************************************************************************************************************/

CVideoStream::CVideoStream(AMC::PVideoStreamInstance pInstance)
	: m_pInstance(pInstance)
{
	if (pInstance.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
}

CVideoStream::~CVideoStream()
{
}

std::string CVideoStream::GetUUID()
{
	return m_pInstance->getUUID();
}

LibMCEnv_uint32 CVideoStream::GetWidth()
{
	return m_pInstance->getWidth();
}

LibMCEnv_uint32 CVideoStream::GetHeight()
{
	return m_pInstance->getHeight();
}

void CVideoStream::GetExtents(LibMCEnv_uint32 & nWidth, LibMCEnv_uint32 & nHeight)
{
	nWidth = m_pInstance->getWidth();
	nHeight = m_pInstance->getHeight();
}

LibMCEnv_uint32 CVideoStream::GetFrameCount()
{
	return m_pInstance->getFrameCount();
}

LibMCEnv_uint32 CVideoStream::GetDroppedFrameCount()
{
	return m_pInstance->getDroppedFrameCount();
}

LibMCEnv_uint32 CVideoStream::GetDesiredFrameDuration()
{
	return m_pInstance->getDesiredFrameDurationInMicroseconds();
}

LibMCEnv_double CVideoStream::GetDesiredFramerate()
{
	return m_pInstance->getDesiredFramerate();
}

LibMCEnv_uint32 CVideoStream::GetPauseTolerance()
{
	return m_pInstance->getPauseToleranceInMicroseconds();
}

LibMCEnv_uint32 CVideoStream::GetFrameCacheDuration()
{
	return m_pInstance->getFrameCacheDurationInMicroseconds();
}

bool CVideoStream::IsActive()
{
	return m_pInstance->isActive();
}

IDateTime * CVideoStream::GetStreamStartTime()
{
	uint64_t nMicroseconds1970 = m_pInstance->getStreamStartTimeMicroseconds1970();
	return new CDateTime(nMicroseconds1970);
}

LibMCEnv_uint64 CVideoStream::GetLastSourceTime()
{
	return m_pInstance->getLastSourceTimeMicroseconds();
}

IImageData * CVideoStream::GetLastSourceFrame()
{
	auto rgb24Data = m_pInstance->getLastRGB24Frame();

	if (rgb24Data.empty())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDIMAGEBUFFER);

	uint32_t nWidth = m_pInstance->getWidth();
	uint32_t nHeight = m_pInstance->getHeight();

	return CImageData::createFromRGB24(
		rgb24Data.data(),
		rgb24Data.size(),
		nWidth,
		nHeight,
		72.0,
		72.0,
		eImagePixelFormat::RGB24bit
	);
}

void CVideoStream::PushFrame(IImageData* pSourceFrameImage)
{
	if (pSourceFrameImage == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

	uint32_t nSrcWidth, nSrcHeight;
	pSourceFrameImage->GetSizeInPixels(nSrcWidth, nSrcHeight);

	if (nSrcWidth != m_pInstance->getWidth() || nSrcHeight != m_pInstance->getHeight())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELCOUNT, "Source frame dimensions do not match the video stream dimensions.");

	if (pSourceFrameImage->GetPixelFormat() != eImagePixelFormat::RGB24bit)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELFORMAT, "Source frame must be RGB24 format.");

	// Get the raw pixel data from the image
	auto pImageData = dynamic_cast<CImageData*>(pSourceFrameImage);
	if (pImageData == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDCAST);

	auto& pixelData = pImageData->getPixelData();

	m_pInstance->pushFrame(pixelData.data(), (uint32_t)pixelData.size());
}

LibMCEnv_uint64 CVideoStream::PushFrameWithTime(IImageData* pSourceFrameImage)
{
	// For now, PushFrameWithTime behaves like PushFrame, returning the current time.
	// Timing-based frame dropping can be implemented later.
	PushFrame(pSourceFrameImage);
	return m_pInstance->getLastSourceTimeMicroseconds();
}

AMC::PVideoStreamInstance CVideoStream::getInstance()
{
	return m_pInstance;
}
