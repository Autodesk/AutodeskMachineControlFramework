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


Abstract: This is a stub class definition of CImageData

*/

#include "libmcenv_imagedata.hpp"
#include "libmcenv_interfaceexception.hpp"
#include "libmcenv_pngimagedata.hpp"

// Include custom headers here.
#include "Libraries/LodePNG/lodepng.h"

using namespace LibMCEnv::Impl;

#define IMAGEDATA_MAXPIXELCOUNT (1024ULL * 1024ULL * 32ULL)
#define IMAGEDATA_MAXPIXELSIZE (1024ULL * 1024ULL)

/*************************************************************************************************************************
 Class definition of CImageData 
**************************************************************************************************************************/

CImageData* CImageData::createFromPNG(const uint8_t* pBuffer, uint64_t nBufferSize, const double dDPIValueX, const double dDPIValueY, eImagePixelFormat pixelFormat)
{
	if (pBuffer == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
	if (nBufferSize == 0)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_EMPTYPNGSTREAM);

	std::unique_ptr <std::vector<uint8_t>> pixelBuffer(new std::vector<uint8_t>());
	

	unsigned int width = 0;
	unsigned int height = 0;
	lodepng::State state;
	unsigned int errorCode = lodepng::decode (*pixelBuffer, width, height, pBuffer, nBufferSize);
	if (errorCode)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNOTLOADPNGIMAGE, "could not load png image (error #" + std::to_string (errorCode));

	if ((width == 0) || (width > IMAGEDATA_MAXPIXELSIZE) || (height == 0) || (height > IMAGEDATA_MAXPIXELSIZE))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELSIZE);

	size_t pixelCount = (size_t)width * (size_t)height;

	switch (pixelFormat) {
	case eImagePixelFormat::RGBA32bit:
		return new CImageData(pixelBuffer.release(), width, height, dDPIValueX, dDPIValueY, eImagePixelFormat::RGBA32bit, false);

	case eImagePixelFormat::RGB24bit:
	{
		std::unique_ptr <std::vector<uint8_t>> convertedPixelBuffer(new std::vector<uint8_t>());
		convertedPixelBuffer->resize(pixelCount * 3);
		uint8_t* pSource = &pixelBuffer->at (0);
		uint8_t* pTarget = &convertedPixelBuffer->at(0);
		for (size_t nIndex = 0; nIndex < pixelCount; nIndex++) {
			pTarget[0] = pSource[0];
			pTarget[1] = pSource[1];
			pTarget[2] = pSource[2];
			pTarget += 3;
			pSource += 4; // Skip Alpha
		}

		return new CImageData(convertedPixelBuffer.release(), width, height, dDPIValueX, dDPIValueY, eImagePixelFormat::RGB24bit, false);
	}

	case eImagePixelFormat::GreyScale8bit:
	{
		std::unique_ptr <std::vector<uint8_t>> convertedPixelBuffer(new std::vector<uint8_t>());
		convertedPixelBuffer->resize(pixelCount);
		uint8_t* pSource = &pixelBuffer->at(0);
		uint8_t* pTarget = &convertedPixelBuffer->at(0);
		for (size_t nIndex = 0; nIndex < pixelCount; nIndex++) {
			*pTarget = ((uint32_t)pSource[0] + (uint32_t)pSource[1] + (uint32_t)pSource[2]) / 3;
			pTarget ++;
			pSource += 4; // Skip Alpha
		}

		return new CImageData(convertedPixelBuffer.release(), width, height, dDPIValueX, dDPIValueY, eImagePixelFormat::GreyScale8bit, false);
	}

	default:
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELFORMAT);

	}

}

CImageData* CImageData::createEmpty(const uint32_t nPixelCountX, const uint32_t nPixelCountY, const double dDPIValueX, const double dDPIValueY, eImagePixelFormat pixelFormat)
{
	std::unique_ptr <std::vector<uint8_t>> pixelBuffer(new std::vector<uint8_t>());
	switch (pixelFormat)
	{
	case eImagePixelFormat::GreyScale8bit: pixelBuffer->resize((size_t)nPixelCountX * (size_t)nPixelCountY); break;
	case eImagePixelFormat::RGB24bit: pixelBuffer->resize((size_t)nPixelCountX * (size_t)nPixelCountY * 3); break;
	case eImagePixelFormat::RGBA32bit: pixelBuffer->resize((size_t)nPixelCountX * (size_t)nPixelCountY * 4); break;
	default:
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELFORMAT);
	}

	return new CImageData(pixelBuffer.release(), nPixelCountX, nPixelCountY, dDPIValueX, dDPIValueY, pixelFormat, true);

}

CImageData::CImageData(std::vector<uint8_t>* pPixelData, const uint32_t nPixelCountX, const uint32_t nPixelCountY, const double dDPIValueX, const double dDPIValueY, eImagePixelFormat pixelFormat, bool bDoClear)
	: m_PixelFormat (pixelFormat), m_nPixelCountX(nPixelCountX), m_nPixelCountY(nPixelCountY), m_dDPIValueX(dDPIValueX), m_dDPIValueY(dDPIValueY), m_PixelData (pPixelData),
	m_dPositionX(0.0), m_dPositionY(0.0)
{
	if (pPixelData == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDIMAGEBUFFER);
	if (nPixelCountX <= 0)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELCOUNT);
	if (nPixelCountY <= 0)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELCOUNT);
	if (dDPIValueX <= 0.0)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDDPIVALUE);
	if (dDPIValueY <= 0.0)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDDPIVALUE);


	switch (pixelFormat) {
		case eImagePixelFormat::GreyScale8bit: 
			if (m_PixelData->size () != (size_t)nPixelCountX * (size_t)nPixelCountY)
				throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELMEMORYSIZE);

			if (bDoClear)
				Clear(128); 
			break;

		case eImagePixelFormat::RGBA32bit: 
			if (m_PixelData->size() != (size_t)nPixelCountX * (size_t)nPixelCountY * 4)
				throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELMEMORYSIZE);
			if (bDoClear)
				Clear(0xffffffffUL);
			break;

		case eImagePixelFormat::RGB24bit: 
			if (m_PixelData->size() != (size_t)nPixelCountX * (size_t)nPixelCountY * 3)
				throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELMEMORYSIZE);

			if (bDoClear)
				Clear(0x00ffffffUL);
			break;

		default: 
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELFORMAT);

	}
	
}

CImageData::~CImageData()
{

}


LibMCEnv::eImagePixelFormat CImageData::GetPixelFormat()
{
	return m_PixelFormat;
}

void CImageData::ChangePixelFormat(const LibMCEnv::eImagePixelFormat ePixelFormat)
{
	if (ePixelFormat == m_PixelFormat)
		return;

	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CImageData::GetDPI(LibMCEnv_double & dDPIValueX, LibMCEnv_double & dDPIValueY)
{
	dDPIValueX = m_dDPIValueX;
	dDPIValueY = m_dDPIValueY;
}

void CImageData::SetDPI(const LibMCEnv_double dDPIValueX, const LibMCEnv_double dDPIValueY)
{
	m_dDPIValueX = dDPIValueX;
	m_dDPIValueY = dDPIValueY;
}

void CImageData::GetSizeInMM(LibMCEnv_double & dSizeX, LibMCEnv_double & dSizeY)
{
	// 1 inch is 25.4 mm
	double dMMperPixelX = 25.4 / m_dDPIValueX;
	double dMMperPixelY = 25.4 / m_dDPIValueY;
	dSizeX = m_nPixelCountX * dMMperPixelX;
	dSizeY = m_nPixelCountY * dMMperPixelY;

}

void CImageData::GetSizeInPixels(LibMCEnv_uint32 & nPixelSizeX, LibMCEnv_uint32 & nPixelSizeY)
{
	nPixelSizeX = m_nPixelCountX;
	nPixelSizeY = m_nPixelCountY;
}

void CImageData::ResizeImage(LibMCEnv_uint32 & nPixelSizeX, LibMCEnv_uint32 & nPixelSizeY)
{
	if (m_PixelData.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDIMAGEBUFFER);

	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

IPNGImageData* CImageData::CreatePNGImage(IPNGImageStoreOptions* pPNGStorageOptions)
{
	unsigned int error = 0;

	if (m_PixelData.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDIMAGEBUFFER);

	std::unique_ptr<CPNGImageData> pResult (new CPNGImageData (m_nPixelCountX, m_nPixelCountY));

	switch (m_PixelFormat) {
	case eImagePixelFormat::GreyScale8bit:
		error = lodepng::encode(pResult->getPNGStreamBuffer (), *m_PixelData, m_nPixelCountX, m_nPixelCountY, LCT_GREY, 8);
		break;
	case eImagePixelFormat::RGB24bit:
		error = lodepng::encode(pResult->getPNGStreamBuffer(), *m_PixelData, m_nPixelCountX, m_nPixelCountY, LCT_RGB, 8);
		break;
	case eImagePixelFormat::RGBA32bit:
		error = lodepng::encode(pResult->getPNGStreamBuffer(), *m_PixelData, m_nPixelCountX, m_nPixelCountY, LCT_RGBA, 8);
		break;

	default:
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELFORMAT);

	}

	if (error)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNOTCOMPRESSPNGIMAGE);

	if (pResult->getPNGStreamBuffer().empty())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNOTSTOREPNGIMAGE);

	return pResult.release();
}


void CImageData::EncodePNG()
{

	m_EncodedPNGDataCache.reset(CreatePNGImage (nullptr));

}

void CImageData::GetEncodedPNGData(LibMCEnv_uint64 nPNGDataBufferSize, LibMCEnv_uint64* pPNGDataNeededCount, LibMCEnv_uint8 * pPNGDataBuffer)
{

	if (m_EncodedPNGDataCache.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_EMPTYPNGIMAGEDATA);

	m_EncodedPNGDataCache->GetPNGDataStream(nPNGDataBufferSize, pPNGDataNeededCount, pPNGDataBuffer);

}

void CImageData::ClearEncodedPNGData()
{
	m_EncodedPNGDataCache.reset ();
}

void CImageData::Clear(const LibMCEnv_uint32 nValue)
{
	if (m_PixelData.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDIMAGEBUFFER);

	switch (m_PixelFormat) {
		case eImagePixelFormat::GreyScale8bit: {
			if (nValue > 0x000000ff)
				throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDDEFAULTIMAGEVALUE);

			for (auto iter = m_PixelData->begin(); iter != m_PixelData->end();) {
				*iter = nValue;
				iter++;
			}

			break;
		}

		case eImagePixelFormat::RGBA32bit: {
			uint8_t nRed = (nValue & 0xff);
			uint8_t nGreen = ((nValue >> 8) & 0xff);
			uint8_t nBlue = ((nValue >> 16) & 0xff);
			uint8_t nAlpha = ((nValue >> 24) & 0xff);

			for (auto iter = m_PixelData->begin(); iter != m_PixelData->end();) {
				*iter = nRed; iter++;
				*iter = nGreen; iter++;
				*iter = nBlue; iter++;
				*iter = nAlpha; iter++;
			}
			break;
		}

		case eImagePixelFormat::RGB24bit: {
			uint8_t nRed = (nValue & 0xff);
			uint8_t nGreen = ((nValue >> 8) & 0xff);
			uint8_t nBlue = ((nValue >> 16) & 0xff);

			for (auto iter = m_PixelData->begin(); iter != m_PixelData->end();) {
				*iter = nRed; iter++;
				*iter = nGreen; iter++;
				*iter = nBlue; iter++;
			}
			break;
		}

	}



}

LibMCEnv_uint32 CImageData::GetPixel(const LibMCEnv_uint32 nX, const LibMCEnv_uint32 nY)
{
	if (m_PixelData.get() == nullptr)
		return 0;

	if (nX >= m_nPixelCountX)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDXCOORDINATE);
	if (nY >= m_nPixelCountY)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDYCOORDINATE);

	size_t nAddress = (size_t)nX + (size_t)nY * (size_t)m_nPixelCountX;

	switch (m_PixelFormat) {

	case eImagePixelFormat::GreyScale8bit: {
		return m_PixelData->at(nAddress);
		break;
	}

	case eImagePixelFormat::RGB24bit: {
		uint32_t nRed = m_PixelData->at(nAddress * 3);
		uint32_t nGreen = m_PixelData->at(nAddress * 3 + 1);
		uint32_t nBlue = m_PixelData->at(nAddress * 3 + 2);
		return (nRed | (nGreen << 8) | (nBlue << 16));
		break;
	}

	case eImagePixelFormat::RGBA32bit: {
		uint32_t nRed = m_PixelData->at(nAddress * 4);
		uint32_t nGreen = m_PixelData->at(nAddress * 4 + 1);
		uint32_t nBlue = m_PixelData->at(nAddress * 4 + 2);
		uint32_t nAlpha = m_PixelData->at(nAddress * 4 + 3);
		return (nRed | (nGreen << 8) | (nBlue << 16) | (nAlpha << 24));
		break;
	}

	default:
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELFORMAT);

	}

	
}

void CImageData::SetPixel(const LibMCEnv_uint32 nX, const LibMCEnv_uint32 nY, const LibMCEnv_uint32 nValue)
{
	if (m_PixelData.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDIMAGEBUFFER);

	if (nX >= m_nPixelCountX)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDXCOORDINATE);
	if (nY >= m_nPixelCountY)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDYCOORDINATE);

	size_t nAddress = (size_t)nX + (size_t)nY * (size_t)m_nPixelCountX;

	switch (m_PixelFormat) {
	case eImagePixelFormat::GreyScale8bit: {
		m_PixelData->at(nAddress) = nValue;
		break;
	}
	case eImagePixelFormat::RGB24bit: {
		uint32_t nRed = nValue & 0xff;
		uint32_t nGreen = (nValue >> 8) & 0xff;
		uint32_t nBlue = (nValue >> 16) & 0xff;
		m_PixelData->at(nAddress * 3) = nRed;
		m_PixelData->at(nAddress * 3 + 1) = nGreen;
		m_PixelData->at(nAddress * 3 + 2) = nBlue;
		break;
	}
	case eImagePixelFormat::RGBA32bit: {
		uint32_t nRed = nValue & 0xff;
		uint32_t nGreen = (nValue >> 8) & 0xff;
		uint32_t nBlue = (nValue >> 16) & 0xff;
		uint32_t nAlpha = (nValue >> 24) & 0xff;
		m_PixelData->at(nAddress * 4) = nRed;
		m_PixelData->at(nAddress * 4 + 1) = nGreen;
		m_PixelData->at(nAddress * 4 + 2) = nBlue;
		m_PixelData->at(nAddress * 4 + 3) = nAlpha;
		break;
	}

	default:
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELFORMAT);

	}
	
}

void CImageData::GetPixelRange(const LibMCEnv_uint32 nXMin, const LibMCEnv_uint32 nYMin, const LibMCEnv_uint32 nXMax, const LibMCEnv_uint32 nYMax, LibMCEnv_uint64 nValueBufferSize, LibMCEnv_uint64* pValueNeededCount, LibMCEnv_uint8 * pValueBuffer)
{
	if (m_PixelData.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDIMAGEBUFFER);

	if (nXMin >= m_nPixelCountX)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDXCOORDINATE);
	if (nYMin >= m_nPixelCountY)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDYCOORDINATE);
	if (nXMax >= m_nPixelCountX)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDXCOORDINATE);
	if (nYMax >= m_nPixelCountY)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDYCOORDINATE);
	if (nXMin > nXMax)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDXCOORDINATERANGE);
	if (nYMin > nYMax)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDYCOORDINATERANGE);

	size_t nSizeFactor = getBytesPerPixel();

	uint32_t nSizeX = (nXMax - nXMin) + 1;
	uint32_t nSizeY = (nYMax - nYMin) + 1;
	size_t nNeededSize = (size_t)nSizeX * (size_t)nSizeY * nSizeFactor;

	if (pValueNeededCount != nullptr)
		*pValueNeededCount = nNeededSize;

	if (pValueBuffer != nullptr) {
		if (nValueBufferSize < nNeededSize)
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_BUFFERTOOSMALL);

		auto & pPixelData = *m_PixelData.get();

		uint8_t* pTargetData = pValueBuffer;
		for (uint32_t nY = 0; nY < nSizeY; nY++) {
			const uint8_t* pSrcData = &pPixelData[(((uint64_t)nY + (uint64_t)nYMin) * m_nPixelCountX + nXMin) * nSizeFactor];

			size_t nCopyCount = nSizeX * nSizeFactor;

			for (uint32_t nX = 0; nX < nCopyCount; nX++) {
				*pTargetData = *pSrcData;
				pTargetData++;
				pSrcData++;
			}
		}

	}
}

size_t CImageData::getBytesPerPixel()
{
	switch (m_PixelFormat) {
	case eImagePixelFormat::GreyScale8bit:
		return 1;
	case eImagePixelFormat::RGB24bit: 
		return 3;
	case eImagePixelFormat::RGBA32bit: 
		return 4;

	default:
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELFORMAT);

	}

}


void CImageData::SetPixelRange(const LibMCEnv_uint32 nXMin, const LibMCEnv_uint32 nYMin, const LibMCEnv_uint32 nXMax, const LibMCEnv_uint32 nYMax, const LibMCEnv_uint64 nValueBufferSize, const LibMCEnv_uint8 * pValueBuffer)
{
	if (m_PixelData.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDIMAGEBUFFER);

	if (pValueBuffer == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
	if (nXMin >= m_nPixelCountX)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDXCOORDINATE);
	if (nYMin >= m_nPixelCountY)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDYCOORDINATE);
	if (nXMax >= m_nPixelCountX)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELCOUNT);
	if (nYMax >= m_nPixelCountY)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELCOUNT);
	if (nXMin > nXMax)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDXCOORDINATERANGE);
	if (nYMin > nYMax)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDYCOORDINATERANGE);
	
	size_t nSizeFactor = getBytesPerPixel ();

	uint32_t nSizeX = (nXMax - nXMin) + 1;
	uint32_t nSizeY = (nYMax - nYMin) + 1;
	size_t nNeededSize = (size_t)nSizeX * (size_t)nSizeY * nSizeFactor;
	if (nValueBufferSize != nNeededSize)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELDATACOUNT);

	auto& pPixelData = *m_PixelData.get();

	const uint8_t* pSrcData = pValueBuffer;
	for (uint32_t nY = 0; nY < nSizeY; nY++) {
		uint8_t* pTargetData = &pPixelData[(((uint64_t)nY + (uint64_t)nYMin) * m_nPixelCountX + nXMin) * nSizeFactor];

		size_t nCopyCount = nSizeX * nSizeFactor;

		for (uint32_t nX = 0; nX < nCopyCount; nX++) {
			*pTargetData = *pSrcData;
			pTargetData++;
			pSrcData++;
		}
	}
}


void CImageData::GetPixels(const LibMCEnv_uint32 nStartX, const LibMCEnv_uint32 nStartY, const LibMCEnv_uint32 nCountX, const LibMCEnv_uint32 nCountY, const LibMCEnv::eImagePixelFormat eTargetFormat, LibMCEnv_uint64 nValueBufferSize, LibMCEnv_uint64* pValueNeededCount, LibMCEnv_uint8* pValueBuffer)
{
	if (m_PixelData.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDIMAGEBUFFER);

	if (pValueBuffer == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

	if (m_PixelData.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDIMAGEBUFFER);

	if (nCountX <= 0)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELCOUNT);
	if (nCountY <= 0)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELCOUNT);

	if (nStartX >= m_nPixelCountX)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDXCOORDINATE);
	if (nStartY >= m_nPixelCountY)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDYCOORDINATE);
	if (((uint64_t) nStartX + nCountX) > m_nPixelCountX)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDXCOORDINATE);
	if (((uint64_t)nStartY + nCountY) > m_nPixelCountY)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDYCOORDINATE);

	size_t nTargetBytesPerPixel;
	switch (eTargetFormat) {
		case LibMCEnv::eImagePixelFormat::GreyScale8bit: nTargetBytesPerPixel = 1; break;
		case LibMCEnv::eImagePixelFormat::RGB16bit: nTargetBytesPerPixel = 2; break;
		case LibMCEnv::eImagePixelFormat::RGB24bit: nTargetBytesPerPixel = 3; break;
		case LibMCEnv::eImagePixelFormat::RGBA32bit: nTargetBytesPerPixel = 4; break;
		default:
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELFORMAT);
	}

	size_t nBytesToWrite = nTargetBytesPerPixel * (size_t)nCountX * (size_t)nCountY;

	if (pValueNeededCount != nullptr)
		*pValueNeededCount = nBytesToWrite;

	if (pValueBuffer != nullptr) {
		if (nValueBufferSize < nBytesToWrite)
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_BUFFERTOOSMALL);

		WriteToRawMemory(nStartX, nStartY, nCountX, nCountY, eTargetFormat, (LibMCEnv_pvoid)pValueBuffer, nTargetBytesPerPixel * (size_t)nCountX);
	}
}

void CImageData::SetPixels(const LibMCEnv_uint32 nStartX, const LibMCEnv_uint32 nStartY, const LibMCEnv_uint32 nCountX, const LibMCEnv_uint32 nCountY, const LibMCEnv::eImagePixelFormat eSourceFormat, LibMCEnv_uint64 nValueBufferSize, const LibMCEnv_uint8* pValueBuffer)
{
	if (m_PixelData.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDIMAGEBUFFER);
	if (pValueBuffer == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELFORMAT);

	if (pValueBuffer == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

	if (nCountX <= 0)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELCOUNT);
	if (nCountY <= 0)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELCOUNT);

	if (nStartX >= m_nPixelCountX)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDXCOORDINATE);
	if (nStartY >= m_nPixelCountY)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDYCOORDINATE);
	if (((uint64_t)nStartX + nCountX) > m_nPixelCountX)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDXCOORDINATE);
	if (((uint64_t)nStartY + nCountY) > m_nPixelCountY)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDYCOORDINATE);

	size_t nSourceBytesPerPixel;
	switch (eSourceFormat) {
	case LibMCEnv::eImagePixelFormat::GreyScale8bit: nSourceBytesPerPixel = 1; break;
	case LibMCEnv::eImagePixelFormat::RGB16bit: nSourceBytesPerPixel = 2; break;
	case LibMCEnv::eImagePixelFormat::RGB24bit: nSourceBytesPerPixel = 3; break;
	case LibMCEnv::eImagePixelFormat::RGBA32bit: nSourceBytesPerPixel = 4; break;
	default:
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELFORMAT);
	}

	size_t nBytesToRead = nSourceBytesPerPixel * (size_t)nCountX * (size_t)nCountY;
	if (nValueBufferSize != nBytesToRead)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELDATACOUNT);

	ReadFromRawMemory(nStartX, nStartY, nCountX, nCountY, eSourceFormat, (LibMCEnv_pvoid)pValueBuffer, nSourceBytesPerPixel * (size_t)nCountX);

}

void CImageData::WriteToRawMemory(const LibMCEnv_uint32 nStartX, const LibMCEnv_uint32 nStartY, const LibMCEnv_uint32 nCountX, const LibMCEnv_uint32 nCountY, const LibMCEnv::eImagePixelFormat eTargetFormat, const LibMCEnv_pvoid pTarget, const LibMCEnv_uint32 nYLineOffset)
{
	if (m_PixelData.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDIMAGEBUFFER);
	if (pTarget == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

	if (nCountX <= 0)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELCOUNT);
	if (nCountY <= 0)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELCOUNT);

	if (nStartX >= m_nPixelCountX)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDXCOORDINATE);
	if (nStartY >= m_nPixelCountY)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDYCOORDINATE);
	if (((uint64_t)nStartX + nCountX) > m_nPixelCountX)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDXCOORDINATE);
	if (((uint64_t)nStartY + nCountY) > m_nPixelCountY)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDYCOORDINATE);

	uint8_t* pTypedTarget = (uint8_t*)pTarget;

	switch (eTargetFormat) {
	case LibMCEnv::eImagePixelFormat::GreyScale8bit: writeToRawMemoryEx_GreyScale8bit (nStartX, nStartY, nCountX, nCountY, pTypedTarget, nYLineOffset); break;
	case LibMCEnv::eImagePixelFormat::RGB16bit: writeToRawMemoryEx_RGB16bit(nStartX, nStartY, nCountX, nCountY, pTypedTarget, nYLineOffset); break;
	case LibMCEnv::eImagePixelFormat::RGB24bit: writeToRawMemoryEx_RGB24bit(nStartX, nStartY, nCountX, nCountY, pTypedTarget, nYLineOffset); break;
	case LibMCEnv::eImagePixelFormat::RGBA32bit: writeToRawMemoryEx_RGBA24bit(nStartX, nStartY, nCountX, nCountY, pTypedTarget, nYLineOffset); break;
	default:
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELFORMAT);
	}


}

void CImageData::ReadFromRawMemory(const LibMCEnv_uint32 nStartX, const LibMCEnv_uint32 nStartY, const LibMCEnv_uint32 nCountX, const LibMCEnv_uint32 nCountY, const LibMCEnv::eImagePixelFormat eSourceFormat, const LibMCEnv_pvoid pSource, const LibMCEnv_uint32 nYLineOffset)
{
	if (m_PixelData.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDIMAGEBUFFER);
	if (pSource == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

	if (nCountX <= 0)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELCOUNT);
	if (nCountY <= 0)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELCOUNT);

	if (nStartX >= m_nPixelCountX)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDXCOORDINATE);
	if (nStartY >= m_nPixelCountY)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDYCOORDINATE);
	if (((uint64_t)nStartX + nCountX) > m_nPixelCountX)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDXCOORDINATE);
	if (((uint64_t)nStartY + nCountY) > m_nPixelCountY)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDYCOORDINATE);

	uint8_t* pTypedSource = (uint8_t*)pSource;

	switch (eSourceFormat) {
	case LibMCEnv::eImagePixelFormat::GreyScale8bit: readFromRawMemoryEx_GreyScale8bit(nStartX, nStartY, nCountX, nCountY, pTypedSource, nYLineOffset); break;
	case LibMCEnv::eImagePixelFormat::RGB16bit: readFromRawMemoryEx_RGB16bit(nStartX, nStartY, nCountX, nCountY, pTypedSource, nYLineOffset); break;
	case LibMCEnv::eImagePixelFormat::RGB24bit: readFromRawMemoryEx_RGB24bit(nStartX, nStartY, nCountX, nCountY, pTypedSource, nYLineOffset); break;
	case LibMCEnv::eImagePixelFormat::RGBA32bit: readFromRawMemoryEx_RGBA24bit(nStartX, nStartY, nCountX, nCountY, pTypedSource, nYLineOffset); break;
	default:
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELFORMAT);
	}

}


std::vector <uint8_t>& CImageData::getPixelData()
{
	return *m_PixelData.get ();
}


void CImageData::writeToRawMemoryEx_GreyScale8bit(uint32_t nStartX, uint32_t nStartY, uint32_t nCountX, uint32_t nCountY, uint8_t* pTarget, uint32_t nYLineOffset)
{
	if ((nCountX <= 0) || (nCountY <= 0))
		return;

	if (m_PixelData.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDIMAGEBUFFER);
	if (pTarget == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDIMAGEBUFFER);

	if (nStartX >= m_nPixelCountX)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDXCOORDINATE);
	if (nStartY >= m_nPixelCountY)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDYCOORDINATE);
	if (((uint64_t)nStartX + nCountX) > m_nPixelCountX)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDXCOORDINATE);
	if (((uint64_t)nStartY + nCountY) > m_nPixelCountY)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDYCOORDINATE);

	size_t nLineAddress = (size_t)nStartX + (size_t)nStartY * (size_t)m_nPixelCountX;
	uint8_t* pLinePtr = pTarget;

	switch (m_PixelFormat) {
	case eImagePixelFormat::GreyScale8bit: {
		for (uint32_t nRow = 0; nRow < nCountY; nRow++) {

			size_t nPixelAddress = nLineAddress;
			uint8_t* pPixelPtr = pLinePtr;

			for (uint32_t nColumn = 0; nColumn < nCountX; nColumn++) {
				*pPixelPtr = m_PixelData->at(nPixelAddress);
				nPixelAddress++;
				pPixelPtr++;
			}

			pLinePtr += nYLineOffset;
			nLineAddress += m_nPixelCountX;
		}

		
		break;
	}


	case eImagePixelFormat::RGB16bit: {
		for (uint32_t nRow = 0; nRow < nCountY; nRow++) {

			size_t nPixelAddress = nLineAddress;
			uint8_t* pPixelPtr = pLinePtr;

			for (uint32_t nColumn = 0; nColumn < nCountX; nColumn++) {

				uint32_t nLow = m_PixelData->at(nPixelAddress); nPixelAddress++;
				uint32_t nHigh = m_PixelData->at(nPixelAddress); nPixelAddress++;
				uint16_t nColor = nLow | (nHigh << 8);

				uint32_t nRed = (nColor & 0x1f) << 3;
				uint32_t nGreen = ((nColor >> 5) & 0x3f) << 2;
				uint32_t nBlue = ((nColor >> 11) & 0x1f) << 3;

				uint8_t nGray = (uint8_t)((nRed + nGreen + nBlue) / 3);
				*pPixelPtr = nGray;

				pPixelPtr++;
			}

			pLinePtr += nYLineOffset;
			nLineAddress += (size_t)m_nPixelCountX * 2;
		}

		break;
	}

	case eImagePixelFormat::RGB24bit: {
		for (uint32_t nRow = 0; nRow < nCountY; nRow++) {

			size_t nPixelAddress = nLineAddress;
			uint8_t* pPixelPtr = pLinePtr;

			for (uint32_t nColumn = 0; nColumn < nCountX; nColumn++) {

				uint32_t nRed = m_PixelData->at(nPixelAddress); nPixelAddress++;
				uint32_t nGreen = m_PixelData->at(nPixelAddress); nPixelAddress++;
				uint32_t nBlue = m_PixelData->at(nPixelAddress); nPixelAddress++;

				uint8_t nGray = (uint8_t)((nRed + nGreen + nBlue) / 3);
				*pPixelPtr = nGray;

				pPixelPtr++;
			}

			pLinePtr += nYLineOffset;
			nLineAddress += (size_t) m_nPixelCountX * 3;
		}

		break;
	}

	case eImagePixelFormat::RGBA32bit: {
		size_t nPixelAddress = nLineAddress;
		uint8_t* pPixelPtr = pLinePtr;

		for (uint32_t nColumn = 0; nColumn < nCountX; nColumn++) {

			uint32_t nRed = m_PixelData->at(nPixelAddress); nPixelAddress++;
			uint32_t nGreen = m_PixelData->at(nPixelAddress); nPixelAddress++;
			uint32_t nBlue = m_PixelData->at(nPixelAddress); nPixelAddress++;
			nPixelAddress++; // Ignore alpha

			uint8_t nGray = (uint8_t)((nRed + nGreen + nBlue) / 3);
			*pPixelPtr = nGray;

			pPixelPtr++;
		}

		pLinePtr += nYLineOffset;
		nLineAddress += (size_t)m_nPixelCountX * 4;

		break;
	}

	default:
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELFORMAT);

	}


}

void CImageData::writeToRawMemoryEx_RGB16bit(uint32_t nStartX, uint32_t nStartY, uint32_t nCountX, uint32_t nCountY, uint8_t* pTarget, uint32_t nYLineOffset)
{
	if ((nCountX <= 0) || (nCountY <= 0))
		return;

	if (m_PixelData.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDIMAGEBUFFER);
	if (pTarget == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDIMAGEBUFFER);

	if (nStartX >= m_nPixelCountX)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDXCOORDINATE);
	if (nStartY >= m_nPixelCountY)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDYCOORDINATE);
	if (((uint64_t)nStartX + nCountX) > m_nPixelCountX)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDXCOORDINATE);
	if (((uint64_t)nStartY + nCountY) > m_nPixelCountY)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDYCOORDINATE);

	size_t nLineAddress = (size_t)nStartX + (size_t)nStartY * (size_t)m_nPixelCountX;
	uint8_t* pLinePtr = pTarget;

	switch (m_PixelFormat) {
	case eImagePixelFormat::GreyScale8bit: {
		for (uint32_t nRow = 0; nRow < nCountY; nRow++) {

			size_t nPixelAddress = nLineAddress;
			uint16_t* pPixelPtr = (uint16_t*)pLinePtr;

			for (uint32_t nColumn = 0; nColumn < nCountX; nColumn++) {
				uint32_t nGrayValue = m_PixelData->at(nPixelAddress);
				nPixelAddress++;

				*pPixelPtr = ((nGrayValue & 0xF8) << 8) | ((nGrayValue & 0xFC) << 3) | (nGrayValue >> 3);

				pPixelPtr++;
			}

			pLinePtr += nYLineOffset;
			nLineAddress += m_nPixelCountX;
		}


		break;
	}


	case eImagePixelFormat::RGB16bit: {
		for (uint32_t nRow = 0; nRow < nCountY; nRow++) {

			size_t nPixelAddress = nLineAddress;
			uint16_t* pPixelPtr = (uint16_t*)pLinePtr;

			for (uint32_t nColumn = 0; nColumn < nCountX; nColumn++) {

				uint32_t nLow = m_PixelData->at(nPixelAddress); nPixelAddress++;
				uint32_t nHigh = m_PixelData->at(nPixelAddress); nPixelAddress++;
				uint16_t nColor = nLow | (nHigh << 8);

				*pPixelPtr = nColor;

				pPixelPtr++;
			}

			pLinePtr += nYLineOffset;
			nLineAddress += (size_t)m_nPixelCountX * 2;
		}

		break;
	}

	case eImagePixelFormat::RGB24bit: {
		for (uint32_t nRow = 0; nRow < nCountY; nRow++) {

			size_t nPixelAddress = nLineAddress;
			uint16_t* pPixelPtr = (uint16_t*)pLinePtr;

			for (uint32_t nColumn = 0; nColumn < nCountX; nColumn++) {

				uint32_t nRed = m_PixelData->at(nPixelAddress); nPixelAddress++;
				uint32_t nGreen = m_PixelData->at(nPixelAddress); nPixelAddress++;
				uint32_t nBlue = m_PixelData->at(nPixelAddress); nPixelAddress++;

				*pPixelPtr = ((nBlue & 0xF8) << 8) | ((nGreen & 0xFC) << 3) | (nRed >> 3);

				pPixelPtr++;
			}

			pLinePtr += nYLineOffset;
			nLineAddress += (size_t)m_nPixelCountX * 3;
		}

		break;
	}

	case eImagePixelFormat::RGBA32bit: {
		size_t nPixelAddress = nLineAddress;
		uint16_t* pPixelPtr = (uint16_t*)pLinePtr;

		for (uint32_t nColumn = 0; nColumn < nCountX; nColumn++) {

			uint32_t nRed = m_PixelData->at(nPixelAddress); nPixelAddress++;
			uint32_t nGreen = m_PixelData->at(nPixelAddress); nPixelAddress++;
			uint32_t nBlue = m_PixelData->at(nPixelAddress); nPixelAddress++;
			nPixelAddress++; // Ignore alpha

			*pPixelPtr = ((nBlue & 0xF8) << 8) | ((nGreen & 0xFC) << 3) | (nRed >> 3);

			pPixelPtr++;
		}

		pLinePtr += nYLineOffset;
		nLineAddress += (size_t)m_nPixelCountX * 4;

		break;
	}

	default:
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELFORMAT);

	}


}

void CImageData::writeToRawMemoryEx_RGB24bit(uint32_t nStartX, uint32_t nStartY, uint32_t nCountX, uint32_t nCountY, uint8_t* pTarget, uint32_t nYLineOffset)
{
	if ((nCountX <= 0) || (nCountY <= 0))
		return;

	if (m_PixelData.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDIMAGEBUFFER);
	if (pTarget == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDIMAGEBUFFER);

	if (nStartX >= m_nPixelCountX)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDXCOORDINATE);
	if (nStartY >= m_nPixelCountY)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDYCOORDINATE);
	if (((uint64_t)nStartX + nCountX) > m_nPixelCountX)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDXCOORDINATE);
	if (((uint64_t)nStartY + nCountY) > m_nPixelCountY)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDYCOORDINATE);

	size_t nLineAddress = (size_t)nStartX + (size_t)nStartY * (size_t)m_nPixelCountX;
	uint8_t* pLinePtr = pTarget;

	switch (m_PixelFormat) {
	case eImagePixelFormat::GreyScale8bit: {
		for (uint32_t nRow = 0; nRow < nCountY; nRow++) {

			size_t nPixelAddress = nLineAddress;
			uint8_t* pPixelPtr = pLinePtr;

			for (uint32_t nColumn = 0; nColumn < nCountX; nColumn++) {
				uint8_t nGrayValue = m_PixelData->at(nPixelAddress);
				nPixelAddress++;
				*pPixelPtr = nGrayValue;  pPixelPtr++;
				*pPixelPtr = nGrayValue;  pPixelPtr++;
				*pPixelPtr = nGrayValue;  pPixelPtr++;
			}

			pLinePtr += nYLineOffset;
			nLineAddress += m_nPixelCountX;
		}


		break;
	}


	case eImagePixelFormat::RGB16bit: {
		for (uint32_t nRow = 0; nRow < nCountY; nRow++) {

			size_t nPixelAddress = nLineAddress;
			uint8_t* pPixelPtr = pLinePtr;

			for (uint32_t nColumn = 0; nColumn < nCountX; nColumn++) {

				uint32_t nLow = m_PixelData->at(nPixelAddress); nPixelAddress++;
				uint32_t nHigh = m_PixelData->at(nPixelAddress); nPixelAddress++;
				uint16_t nColor = nLow | (nHigh << 8);

				uint32_t nRed = (nColor & 0x1f) << 3;
				uint32_t nGreen = ((nColor >> 5) & 0x3f) << 2;
				uint32_t nBlue = ((nColor >> 11) & 0x1f) << 3;

				*pPixelPtr = nRed; pPixelPtr++;
				*pPixelPtr = nGreen; pPixelPtr++;
				*pPixelPtr = nBlue; pPixelPtr++;
			}

			pLinePtr += nYLineOffset;
			nLineAddress += (size_t)m_nPixelCountX * 2;
		}

		break;
	}

	case eImagePixelFormat::RGB24bit: {
		for (uint32_t nRow = 0; nRow < nCountY; nRow++) {

			size_t nPixelAddress = nLineAddress;
			uint8_t* pPixelPtr = pLinePtr;

			for (uint32_t nColumn = 0; nColumn < nCountX; nColumn++) {

				uint32_t nRed = m_PixelData->at(nPixelAddress); nPixelAddress++;
				uint32_t nGreen = m_PixelData->at(nPixelAddress); nPixelAddress++;
				uint32_t nBlue = m_PixelData->at(nPixelAddress); nPixelAddress++;

				*pPixelPtr = nRed; pPixelPtr++;
				*pPixelPtr = nGreen; pPixelPtr++;
				*pPixelPtr = nBlue; pPixelPtr++;				
			}

			pLinePtr += nYLineOffset;
			nLineAddress += (size_t)m_nPixelCountX * 3;
		}

		break;
	}

	case eImagePixelFormat::RGBA32bit: {
		size_t nPixelAddress = nLineAddress;
		uint8_t* pPixelPtr = pLinePtr;

		for (uint32_t nColumn = 0; nColumn < nCountX; nColumn++) {

			uint32_t nRed = m_PixelData->at(nPixelAddress); nPixelAddress++;
			uint32_t nGreen = m_PixelData->at(nPixelAddress); nPixelAddress++;
			uint32_t nBlue = m_PixelData->at(nPixelAddress); nPixelAddress++;
			// ignore alpha
			nPixelAddress++;

			*pPixelPtr = nRed; pPixelPtr++;
			*pPixelPtr = nGreen; pPixelPtr++;
			*pPixelPtr = nBlue; pPixelPtr++;
		}

		pLinePtr += nYLineOffset;
		nLineAddress += (size_t)m_nPixelCountX * 4;

		break;
	}

	default:
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELFORMAT);

	}


}

void CImageData::writeToRawMemoryEx_RGBA24bit(uint32_t nStartX, uint32_t nStartY, uint32_t nCountX, uint32_t nCountY, uint8_t* pTarget, uint32_t nYLineOffset)
{
	if ((nCountX <= 0) || (nCountY <= 0))
		return;

	if (m_PixelData.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDIMAGEBUFFER);
	if (pTarget == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDIMAGEBUFFER);

	if (nStartX >= m_nPixelCountX)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDXCOORDINATE);
	if (nStartY >= m_nPixelCountY)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDYCOORDINATE);
	if (((uint64_t)nStartX + nCountX) > m_nPixelCountX)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDXCOORDINATE);
	if (((uint64_t)nStartY + nCountY) > m_nPixelCountY)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDYCOORDINATE);

	size_t nLineAddress = (size_t)nStartX + (size_t)nStartY * (size_t)m_nPixelCountX;
	uint8_t* pLinePtr = pTarget;

	switch (m_PixelFormat) {
	case eImagePixelFormat::GreyScale8bit: {
		for (uint32_t nRow = 0; nRow < nCountY; nRow++) {

			size_t nPixelAddress = nLineAddress;
			uint8_t* pPixelPtr = pLinePtr;

			for (uint32_t nColumn = 0; nColumn < nCountX; nColumn++) {
				uint8_t nGrayValue = m_PixelData->at(nPixelAddress);
				nPixelAddress++;
				*pPixelPtr = nGrayValue;  pPixelPtr++;
				*pPixelPtr = nGrayValue;  pPixelPtr++;
				*pPixelPtr = nGrayValue;  pPixelPtr++;
				*pPixelPtr = 255;  pPixelPtr++;
			}

			pLinePtr += nYLineOffset;
			nLineAddress += m_nPixelCountX;
		}


		break;
	}


	case eImagePixelFormat::RGB16bit: {
		for (uint32_t nRow = 0; nRow < nCountY; nRow++) {

			size_t nPixelAddress = nLineAddress;
			uint8_t* pPixelPtr = pLinePtr;

			for (uint32_t nColumn = 0; nColumn < nCountX; nColumn++) {

				uint32_t nLow = m_PixelData->at(nPixelAddress); nPixelAddress++;
				uint32_t nHigh = m_PixelData->at(nPixelAddress); nPixelAddress++;
				uint16_t nColor = nLow | (nHigh << 8);

				uint32_t nRed = (nColor & 0x1f) << 3;
				uint32_t nGreen = ((nColor >> 5) & 0x3f) << 2;
				uint32_t nBlue = ((nColor >> 11) & 0x1f) << 3;

				*pPixelPtr = nRed; pPixelPtr++;
				*pPixelPtr = nGreen; pPixelPtr++;
				*pPixelPtr = nBlue; pPixelPtr++;
				*pPixelPtr = 255;  pPixelPtr++;
			}

			pLinePtr += nYLineOffset;
			nLineAddress += (size_t)m_nPixelCountX * 2;
		}

		break;
	}

	case eImagePixelFormat::RGB24bit: {
		for (uint32_t nRow = 0; nRow < nCountY; nRow++) {

			size_t nPixelAddress = nLineAddress;
			uint8_t* pPixelPtr = pLinePtr;

			for (uint32_t nColumn = 0; nColumn < nCountX; nColumn++) {

				uint32_t nRed = m_PixelData->at(nPixelAddress); nPixelAddress++;
				uint32_t nGreen = m_PixelData->at(nPixelAddress); nPixelAddress++;
				uint32_t nBlue = m_PixelData->at(nPixelAddress); nPixelAddress++;

				*pPixelPtr = nRed; pPixelPtr++;
				*pPixelPtr = nGreen; pPixelPtr++;
				*pPixelPtr = nBlue; pPixelPtr++;
				*pPixelPtr = 255;  pPixelPtr++;

				pPixelPtr++;
			}

			pLinePtr += nYLineOffset;
			nLineAddress += (size_t)m_nPixelCountX * 3;
		}

		break;
	}

	case eImagePixelFormat::RGBA32bit: {
		size_t nPixelAddress = nLineAddress;
		uint8_t* pPixelPtr = pLinePtr;

		for (uint32_t nColumn = 0; nColumn < nCountX; nColumn++) {

			uint32_t nRed = m_PixelData->at(nPixelAddress); nPixelAddress++;
			uint32_t nGreen = m_PixelData->at(nPixelAddress); nPixelAddress++;
			uint32_t nBlue = m_PixelData->at(nPixelAddress); nPixelAddress++;
			uint32_t nAlpha = m_PixelData->at(nPixelAddress); nPixelAddress++;

			*pPixelPtr = nRed; pPixelPtr++;
			*pPixelPtr = nGreen; pPixelPtr++;
			*pPixelPtr = nBlue; pPixelPtr++;
			*pPixelPtr = nAlpha;  pPixelPtr++;
		}

		pLinePtr += nYLineOffset;
		nLineAddress += (size_t)m_nPixelCountX * 4;

		break;
	}

	default:
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELFORMAT);

	}


}

void CImageData::readFromRawMemoryEx_GreyScale8bit(uint32_t nStartX, uint32_t nStartY, uint32_t nCountX, uint32_t nCountY, uint8_t* pSource, uint32_t nYLineOffset)
{
	if ((nCountX <= 0) || (nCountY <= 0))
		return;

	if (m_PixelData.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDIMAGEBUFFER);
	if (pSource == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDIMAGEBUFFER);

	if (nStartX >= m_nPixelCountX)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDXCOORDINATE);
	if (nStartY >= m_nPixelCountY)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDYCOORDINATE);
	if (((uint64_t)nStartX + nCountX) > m_nPixelCountX)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDXCOORDINATE);
	if (((uint64_t)nStartY + nCountY) > m_nPixelCountY)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDYCOORDINATE);

	size_t nLineAddress = (size_t)nStartX + (size_t)nStartY * (size_t)m_nPixelCountX;
	uint8_t* pLinePtr = pSource;

	switch (m_PixelFormat) {
	case eImagePixelFormat::GreyScale8bit: {
		for (uint32_t nRow = 0; nRow < nCountY; nRow++) {

			size_t nPixelAddress = nLineAddress;
			uint8_t* pPixelPtr = pLinePtr;

			for (uint32_t nColumn	 = 0; nColumn < nCountX; nColumn++) {
				m_PixelData->at(nPixelAddress) = *pPixelPtr;
				nPixelAddress++;
				pPixelPtr++;
			}

			pLinePtr += nYLineOffset;
			nLineAddress += m_nPixelCountX;
		}


		break;
	}
	case eImagePixelFormat::RGB24bit: {
		for (uint32_t nRow = 0; nRow < nCountY; nRow++) {

			size_t nPixelAddress = nLineAddress;
			uint8_t* pPixelPtr = pLinePtr;

			for (uint32_t nColumn = 0; nColumn < nCountX; nColumn++) {
				uint8_t nValue = *pPixelPtr;

				m_PixelData->at(nPixelAddress) = nValue; nPixelAddress++;
				m_PixelData->at(nPixelAddress) = nValue; nPixelAddress++;
				m_PixelData->at(nPixelAddress) = nValue; nPixelAddress++;

				pPixelPtr++;
			}

			pLinePtr += nYLineOffset;
			nLineAddress += (size_t)m_nPixelCountX * 3;
		}

		break;
	}
	case eImagePixelFormat::RGBA32bit: {
		size_t nPixelAddress = nLineAddress;
		uint8_t* pPixelPtr = pLinePtr;

		for (uint32_t nColumn = 0; nColumn <= nCountX; nColumn++) {
			uint8_t nValue = *pPixelPtr;

			m_PixelData->at(nPixelAddress) = nValue; nPixelAddress++;
			m_PixelData->at(nPixelAddress) = nValue; nPixelAddress++;
			m_PixelData->at(nPixelAddress) = nValue; nPixelAddress++;
			m_PixelData->at(nPixelAddress) = 255; nPixelAddress++;

			pPixelPtr++;
		}

		pLinePtr += nYLineOffset;
		nLineAddress += (size_t)m_nPixelCountX * 4;

		break;
	}

	default:
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELFORMAT);

	}


}

void CImageData::readFromRawMemoryEx_RGB16bit(uint32_t nStartX, uint32_t nStartY, uint32_t nCountX, uint32_t nCountY, uint8_t* pSource, uint32_t nYLineOffset)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CImageData::readFromRawMemoryEx_RGB24bit(uint32_t nStartX, uint32_t nStartY, uint32_t nCountX, uint32_t nCountY, uint8_t* pSource, uint32_t nYLineOffset)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CImageData::readFromRawMemoryEx_RGBA24bit(uint32_t nStartX, uint32_t nStartY, uint32_t nCountX, uint32_t nCountY, uint8_t* pSource, uint32_t nYLineOffset)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}
