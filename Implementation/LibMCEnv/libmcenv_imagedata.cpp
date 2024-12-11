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
#include "libmcenv_jpegimagedata.hpp"
#include "libmcenv_pngimagestoreoptions.hpp"
#include "libmcenv_jpegimagestoreoptions.hpp"

// Include custom headers here.
#include "Libraries/LodePNG/lodepng.h"

#include <cmath>
#include <turbojpeg.h>

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

	case eImagePixelFormat::RGB16bit:
	{
		std::unique_ptr <std::vector<uint8_t>> convertedPixelBuffer(new std::vector<uint8_t>());
		convertedPixelBuffer->resize(pixelCount * 2);
		uint8_t* pSource = &pixelBuffer->at(0);
		uint16_t* pTarget = (uint16_t*) &convertedPixelBuffer->at(0);
		for (size_t nIndex = 0; nIndex < pixelCount; nIndex++) {
			*pTarget = ((pSource[2] & 0xF8) << 8) | ((pSource[1] & 0xFC) << 3) | (pSource[0] >> 3);

			pTarget++;
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

CImageData* CImageData::createFromJPEG(const uint8_t* pBuffer, uint64_t nBufferSize, const double dDPIValueX, const double dDPIValueY, eImagePixelFormat pixelFormat)
{
	if ((pBuffer == nullptr) || (nBufferSize == 0))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDJPEGDATA);
	if (nBufferSize > IMAGEDATA_MAXJPEGSIZE)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDJPEGBUFFERSIZE);

	bool bStopOnWarning = false;
	bool bFastUpsample = false;
	bool bFastDCT = false;
	
	auto tjInstance = tj3Init(TJINIT_DECOMPRESS);
	if (tjInstance == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNOTINITIALIZEJPEGLIBRARY);

	if (tj3Set(tjInstance, TJPARAM_STOPONWARNING, (int) bStopOnWarning) < 0)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNOTSETJPEGSTOPONWARNING);
	if (tj3Set(tjInstance, TJPARAM_FASTUPSAMPLE, (int)bFastUpsample) < 0)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNOTSETFASTUPSAMPLE);
	if (tj3Set(tjInstance, TJPARAM_FASTDCT, (int)bFastDCT) < 0)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNOTSETFASTDCT);
	// Maybe TODO: TJPARAM_SCANLIMIT / TJPARAM_MAXMEMORY	

	if (tj3DecompressHeader (tjInstance, pBuffer, nBufferSize) < 0)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNOTREADJPEGHEADER);

	int nSubSamppling = tj3Get(tjInstance, TJPARAM_SUBSAMP);
	int nWidth = tj3Get(tjInstance, TJPARAM_JPEGWIDTH);
	int nHeight = tj3Get(tjInstance, TJPARAM_JPEGHEIGHT);
	int nPrecision = tj3Get(tjInstance, TJPARAM_PRECISION);

	if ((nWidth <= 0) || (nHeight <= 0) || (nWidth >= IMAGEDATA_MAXPIXELCOUNT) || (nHeight >= IMAGEDATA_MAXPIXELCOUNT))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDJPEGIMAGESIZE, "invalid JPEG image size: " + std::to_string (nWidth) + "x" + std::to_string (nHeight));

	if (nPrecision > 8)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_JPEGCOLORPRECISIONTOOHIGH, "JPEG color precisition is too high");
	
	size_t nPixelCount = (size_t)nWidth * (size_t)nHeight;

	switch (pixelFormat) {
		case eImagePixelFormat::GreyScale8bit: {
			std::unique_ptr <std::vector<uint8_t>> pixelBuffer(new std::vector<uint8_t>());
			pixelBuffer->resize(nPixelCount);

			if (tj3Decompress8(tjInstance, pBuffer, nBufferSize, (unsigned char*)pixelBuffer.get (), 0, TJPF_GRAY) < 0)
				throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNOTDECOMPRESSJPEG, "could not decompress JPEG");

			return new CImageData(pixelBuffer.release(), nWidth, nHeight, dDPIValueX, dDPIValueY, eImagePixelFormat::GreyScale8bit, false);

			break;
		}

		case eImagePixelFormat::RGB24bit: {
			std::unique_ptr <std::vector<uint8_t>> pixelBuffer(new std::vector<uint8_t>());
			pixelBuffer->resize(nPixelCount * 3);

			if (tj3Decompress8(tjInstance, pBuffer, nBufferSize, (unsigned char*)pixelBuffer.get(), 0, TJPF_RGB) < 0)
				throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNOTDECOMPRESSJPEG, "could not decompress JPEG");

			return new CImageData(pixelBuffer.release(), nWidth, nHeight, dDPIValueX, dDPIValueY, eImagePixelFormat::RGB24bit, false);

			break;
		}

		case eImagePixelFormat::RGBA32bit: {
			std::unique_ptr <std::vector<uint8_t>> pixelBuffer(new std::vector<uint8_t>());
			pixelBuffer->resize(nPixelCount * 4);

			if (tj3Decompress8(tjInstance, pBuffer, nBufferSize, (unsigned char*)pixelBuffer.get(), 0, TJPF_RGBX) < 0)
				throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNOTDECOMPRESSJPEG, "could not decompress JPEG");

			uint8_t* pTarget = &pixelBuffer->at(3);
			for (size_t nPixelIndex = 0; nPixelIndex < nPixelCount; nPixelIndex++) {
				*pTarget = 255;
				pTarget += 4;
			}

			return new CImageData(pixelBuffer.release(), nWidth, nHeight, dDPIValueX, dDPIValueY, eImagePixelFormat::RGBA32bit, false);

			break;
		}

		default:
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELFORMAT, "Invalid pixel format");
	}

}



CImageData* CImageData::createFromRGB24(const uint8_t* pBuffer, uint64_t nBufferSize, const uint32_t nPixelCountX, const uint32_t nPixelCountY, const double dDPIValueX, const double dDPIValueY, eImagePixelFormat pixelFormat)
{
	std::unique_ptr <std::vector<uint8_t>> pixelBuffer(new std::vector<uint8_t>());
	switch (pixelFormat)
	{
	case eImagePixelFormat::GreyScale8bit: pixelBuffer->resize((size_t)nPixelCountX * (size_t)nPixelCountY); break;
	case eImagePixelFormat::RGB16bit: pixelBuffer->resize((size_t)nPixelCountX * (size_t)nPixelCountY * 2); break;
	case eImagePixelFormat::RGB24bit: pixelBuffer->resize((size_t)nPixelCountX * (size_t)nPixelCountY * 3); break;
	case eImagePixelFormat::RGBA32bit: pixelBuffer->resize((size_t)nPixelCountX * (size_t)nPixelCountY * 4); break;
	default:
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELFORMAT);
	}

	auto pImageData = std::make_unique<CImageData> (pixelBuffer.release(), nPixelCountX, nPixelCountY, dDPIValueX, dDPIValueY, pixelFormat, false);

	pImageData->readFromRawMemoryEx_RGB24bit (0, 0, nPixelCountX, nPixelCountY, pBuffer, nPixelCountX * 3);

	return pImageData.release();
}

CImageData* CImageData::createFromRGBA32(const uint8_t* pBuffer, uint64_t nBufferSize, const uint32_t nPixelCountX, const uint32_t nPixelCountY, const double dDPIValueX, const double dDPIValueY, eImagePixelFormat pixelFormat)
{
	std::unique_ptr <std::vector<uint8_t>> pixelBuffer(new std::vector<uint8_t>());
	switch (pixelFormat)
	{
	case eImagePixelFormat::GreyScale8bit: pixelBuffer->resize((size_t)nPixelCountX * (size_t)nPixelCountY); break;
	case eImagePixelFormat::RGB16bit: pixelBuffer->resize((size_t)nPixelCountX * (size_t)nPixelCountY * 2); break;
	case eImagePixelFormat::RGB24bit: pixelBuffer->resize((size_t)nPixelCountX * (size_t)nPixelCountY * 3); break;
	case eImagePixelFormat::RGBA32bit: pixelBuffer->resize((size_t)nPixelCountX * (size_t)nPixelCountY * 4); break;
	default:
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELFORMAT);
	}

	auto pImageData = std::make_unique<CImageData>(pixelBuffer.release(), nPixelCountX, nPixelCountY, dDPIValueX, dDPIValueY, pixelFormat, false);

	pImageData->readFromRawMemoryEx_RGBA32bit(0, 0, nPixelCountX, nPixelCountY, pBuffer, nPixelCountX * 4);

	return pImageData.release();

}

CImageData* CImageData::createFromYUY2(const uint8_t* pBuffer, uint64_t nBufferSize, const uint32_t nPixelCountX, const uint32_t nPixelCountY, const double dDPIValueX, const double dDPIValueY, eImagePixelFormat pixelFormat)
{
	std::unique_ptr <std::vector<uint8_t>> pixelBuffer(new std::vector<uint8_t>());
	switch (pixelFormat)
	{
	case eImagePixelFormat::GreyScale8bit: pixelBuffer->resize((size_t)nPixelCountX * (size_t)nPixelCountY); break;
	case eImagePixelFormat::RGB16bit: pixelBuffer->resize((size_t)nPixelCountX * (size_t)nPixelCountY * 2); break;
	case eImagePixelFormat::RGB24bit: pixelBuffer->resize((size_t)nPixelCountX * (size_t)nPixelCountY * 3); break;
	case eImagePixelFormat::RGBA32bit: pixelBuffer->resize((size_t)nPixelCountX * (size_t)nPixelCountY * 4); break;
	default:
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELFORMAT);
	}

	auto pImageData = std::make_unique<CImageData>(pixelBuffer.release(), nPixelCountX, nPixelCountY, dDPIValueX, dDPIValueY, pixelFormat, false);

	pImageData->SetPixelsFromRawYUY2Data (nBufferSize, pBuffer);

	return pImageData.release();

}



CImageData* CImageData::createEmpty(const uint32_t nPixelCountX, const uint32_t nPixelCountY, const double dDPIValueX, const double dDPIValueY, eImagePixelFormat pixelFormat)
{
	std::unique_ptr <std::vector<uint8_t>> pixelBuffer(new std::vector<uint8_t>());
	switch (pixelFormat)
	{
	case eImagePixelFormat::GreyScale8bit: pixelBuffer->resize((size_t)nPixelCountX * (size_t)nPixelCountY); break;
	case eImagePixelFormat::RGB16bit: pixelBuffer->resize((size_t)nPixelCountX * (size_t)nPixelCountY * 2); break;
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


IPNGImageStoreOptions* CImageData::CreatePNGOptions()
{
	return new CPNGImageStoreOptions();
}

IPNGImageData* CImageData::CreatePNGImage(IPNGImageStoreOptions* pPNGStorageOptions)
{
	unsigned int errorCode = 0;

	if (m_PixelData.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDIMAGEBUFFER);
	if ((m_nPixelCountX == 0) || (m_nPixelCountY == 0))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDIMAGEBUFFER);

	// Retrieve optional storage format
	LibMCEnv::ePNGStorageFormat pngStorageFormat = LibMCEnv::ePNGStorageFormat::RGB24bit;
	if (pPNGStorageOptions != nullptr)
		pngStorageFormat = pPNGStorageOptions->GetStorageFormat();


	std::vector<uint8_t> convertedPixelData;
	uint64_t nTotalPixelCount = (uint64_t)m_nPixelCountX * (uint64_t)m_nPixelCountY;
	uint32_t nRowSize = 0;

	std::unique_ptr<CPNGImageData> pResult (new CPNGImageData (m_nPixelCountX, m_nPixelCountY));

	switch (m_PixelFormat) {
	case eImagePixelFormat::GreyScale8bit:

		switch (pngStorageFormat) {
			case LibMCEnv::ePNGStorageFormat::BlackWhite1bit: 
				convertedPixelData.resize(nTotalPixelCount);
				nRowSize = (m_nPixelCountX + 7) / 8;
				writeToRawMemoryEx_BlackWhite1bit(0, 0, m_nPixelCountX, m_nPixelCountY, convertedPixelData.data(), nRowSize);
				errorCode = lodepng::encode(pResult->getPNGStreamBuffer(), convertedPixelData, m_nPixelCountX, m_nPixelCountY, LCT_GREY, 1);
				break;

			case LibMCEnv::ePNGStorageFormat::GreyScale2bit:
				convertedPixelData.resize(nTotalPixelCount);
				nRowSize = (m_nPixelCountX + 3) / 4;
				writeToRawMemoryEx_GreyScale2bit(0, 0, m_nPixelCountX, m_nPixelCountY, convertedPixelData.data(), nRowSize);
				errorCode = lodepng::encode(pResult->getPNGStreamBuffer(), convertedPixelData, m_nPixelCountX, m_nPixelCountY, LCT_GREY, 2);
				break;

			case LibMCEnv::ePNGStorageFormat::GreyScale4bit:
				convertedPixelData.resize(nTotalPixelCount);
				nRowSize = (m_nPixelCountX + 1) / 2;
				writeToRawMemoryEx_GreyScale4bit(0, 0, m_nPixelCountX, m_nPixelCountY, convertedPixelData.data(), nRowSize);
				errorCode = lodepng::encode(pResult->getPNGStreamBuffer(), convertedPixelData, m_nPixelCountX, m_nPixelCountY, LCT_GREY, 4);
				break;

			case LibMCEnv::ePNGStorageFormat::GreyScale8bit:
				errorCode = lodepng::encode(pResult->getPNGStreamBuffer(), *m_PixelData, m_nPixelCountX, m_nPixelCountY, LCT_GREY, 8);
				break;

			case LibMCEnv::ePNGStorageFormat::RGB24bit:
				convertedPixelData.resize(nTotalPixelCount * 3);
				writeToRawMemoryEx_RGBA32bit(0, 0, m_nPixelCountX, m_nPixelCountY, convertedPixelData.data(), m_nPixelCountX * 3);
				errorCode = lodepng::encode(pResult->getPNGStreamBuffer(), convertedPixelData, m_nPixelCountX, m_nPixelCountY, LCT_RGB, 8);
				break;

			case LibMCEnv::ePNGStorageFormat::RGBA32bit:
				convertedPixelData.resize(nTotalPixelCount * 4);
				writeToRawMemoryEx_RGBA32bit(0, 0, m_nPixelCountX, m_nPixelCountY, convertedPixelData.data(), m_nPixelCountX * 4);
				errorCode = lodepng::encode(pResult->getPNGStreamBuffer(), convertedPixelData, m_nPixelCountX, m_nPixelCountY, LCT_RGBA, 8);
				break;

			default:
				throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPNGEXPORTFORMAT);

		}

		break;
	case eImagePixelFormat::RGB24bit:

		switch (pngStorageFormat) {
		case LibMCEnv::ePNGStorageFormat::BlackWhite1bit:
			convertedPixelData.resize(nTotalPixelCount);
			nRowSize = (m_nPixelCountX + 7) / 8;
			writeToRawMemoryEx_BlackWhite1bit(0, 0, m_nPixelCountX, m_nPixelCountY, convertedPixelData.data(), nRowSize);
			errorCode = lodepng::encode(pResult->getPNGStreamBuffer(), convertedPixelData, m_nPixelCountX, m_nPixelCountY, LCT_GREY, 1);
			break;

		case LibMCEnv::ePNGStorageFormat::GreyScale2bit:
			convertedPixelData.resize(nTotalPixelCount);
			nRowSize = (m_nPixelCountX + 3) / 4;
			writeToRawMemoryEx_GreyScale2bit(0, 0, m_nPixelCountX, m_nPixelCountY, convertedPixelData.data(), nRowSize);
			errorCode = lodepng::encode(pResult->getPNGStreamBuffer(), convertedPixelData, m_nPixelCountX, m_nPixelCountY, LCT_GREY, 2);
			break;

		case LibMCEnv::ePNGStorageFormat::GreyScale4bit:
			convertedPixelData.resize(nTotalPixelCount);
			nRowSize = (m_nPixelCountX + 1) / 2;
			writeToRawMemoryEx_GreyScale4bit(0, 0, m_nPixelCountX, m_nPixelCountY, convertedPixelData.data(), nRowSize);
			errorCode = lodepng::encode(pResult->getPNGStreamBuffer(), convertedPixelData, m_nPixelCountX, m_nPixelCountY, LCT_GREY, 4);
			break;

		case LibMCEnv::ePNGStorageFormat::GreyScale8bit:	
			convertedPixelData.resize(nTotalPixelCount);
			writeToRawMemoryEx_GreyScale8bit(0, 0, m_nPixelCountX, m_nPixelCountY, convertedPixelData.data(), m_nPixelCountX);
			errorCode = lodepng::encode(pResult->getPNGStreamBuffer(), convertedPixelData, m_nPixelCountX, m_nPixelCountY, LCT_GREY, 8);
			break;

		case LibMCEnv::ePNGStorageFormat::RGB24bit:
			errorCode = lodepng::encode(pResult->getPNGStreamBuffer(), *m_PixelData, m_nPixelCountX, m_nPixelCountY, LCT_RGB, 8);
			break;

		case LibMCEnv::ePNGStorageFormat::RGBA32bit:
			convertedPixelData.resize(nTotalPixelCount * 4);
			writeToRawMemoryEx_RGBA32bit(0, 0, m_nPixelCountX, m_nPixelCountY, convertedPixelData.data(), m_nPixelCountX * 4);
			errorCode = lodepng::encode(pResult->getPNGStreamBuffer(), convertedPixelData, m_nPixelCountX, m_nPixelCountY, LCT_RGBA, 8);
			break;

		default:
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPNGEXPORTFORMAT);

		}		
		break;

	case eImagePixelFormat::RGBA32bit:
		switch (pngStorageFormat) {
		case LibMCEnv::ePNGStorageFormat::BlackWhite1bit:
			convertedPixelData.resize(nTotalPixelCount);
			nRowSize = (m_nPixelCountX + 7) / 8;
			writeToRawMemoryEx_BlackWhite1bit(0, 0, m_nPixelCountX, m_nPixelCountY, convertedPixelData.data(), nRowSize);
			errorCode = lodepng::encode(pResult->getPNGStreamBuffer(), convertedPixelData, m_nPixelCountX, m_nPixelCountY, LCT_GREY, 1);
			break;

		case LibMCEnv::ePNGStorageFormat::GreyScale2bit:
			convertedPixelData.resize(nTotalPixelCount);
			nRowSize = (m_nPixelCountX + 3) / 4;
			writeToRawMemoryEx_GreyScale2bit(0, 0, m_nPixelCountX, m_nPixelCountY, convertedPixelData.data(), nRowSize);
			errorCode = lodepng::encode(pResult->getPNGStreamBuffer(), convertedPixelData, m_nPixelCountX, m_nPixelCountY, LCT_GREY, 2);
			break;

		case LibMCEnv::ePNGStorageFormat::GreyScale4bit:
			convertedPixelData.resize(nTotalPixelCount);
			nRowSize = (m_nPixelCountX + 1) / 2;
			writeToRawMemoryEx_GreyScale4bit(0, 0, m_nPixelCountX, m_nPixelCountY, convertedPixelData.data(), nRowSize);
			errorCode = lodepng::encode(pResult->getPNGStreamBuffer(), convertedPixelData, m_nPixelCountX, m_nPixelCountY, LCT_GREY, 4);
			break;

		case LibMCEnv::ePNGStorageFormat::GreyScale8bit:
			convertedPixelData.resize(nTotalPixelCount);
			writeToRawMemoryEx_GreyScale8bit(0, 0, m_nPixelCountX, m_nPixelCountY, convertedPixelData.data(), m_nPixelCountX);
			errorCode = lodepng::encode(pResult->getPNGStreamBuffer(), convertedPixelData, m_nPixelCountX, m_nPixelCountY, LCT_GREY, 8);
			break;

		case LibMCEnv::ePNGStorageFormat::RGB24bit:
			convertedPixelData.resize(nTotalPixelCount * 3);
			writeToRawMemoryEx_RGB24bit(0, 0, m_nPixelCountX, m_nPixelCountY, convertedPixelData.data(), m_nPixelCountX * 3);
			errorCode = lodepng::encode(pResult->getPNGStreamBuffer(), convertedPixelData, m_nPixelCountX, m_nPixelCountY, LCT_RGB, 8);
			break;

		case LibMCEnv::ePNGStorageFormat::RGBA32bit:
			errorCode = lodepng::encode(pResult->getPNGStreamBuffer(), *m_PixelData, m_nPixelCountX, m_nPixelCountY, LCT_RGBA, 8);
			break;

		default:
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPNGEXPORTFORMAT);

		}
		break;

	default:
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELFORMAT);

	}

	if (errorCode) {
		std::string sErrorMessage (lodepng_error_text(errorCode));
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNOTCOMPRESSPNGIMAGE, sErrorMessage);
	}

	if (pResult->getPNGStreamBuffer().empty())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNOTSTOREPNGIMAGE);

	return pResult.release();
}

IJPEGImageStoreOptions* CImageData::CreateJPEGOptions()
{
	return new CJPEGImageStoreOptions();
}

IJPEGImageData* CImageData::CreateJPEGImage(IJPEGImageStoreOptions* pJPEGStorageOptions)
{
	if (m_PixelData.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDIMAGEBUFFER);

	std::unique_ptr<CJPEGImageData> pResult(new CJPEGImageData(m_nPixelCountX, m_nPixelCountY));

	switch (m_PixelFormat) {
	case eImagePixelFormat::GreyScale8bit:
		//error = lodepng::encode(pResult->getPNGStreamBuffer(), *m_PixelData, m_nPixelCountX, m_nPixelCountY, LCT_GREY, 8);
		break;
	case eImagePixelFormat::RGB24bit:
		//error = lodepng::encode(pResult->getPNGStreamBuffer(), *m_PixelData, m_nPixelCountX, m_nPixelCountY, LCT_RGB, 8);
		break;
	case eImagePixelFormat::RGBA32bit:
		//error = lodepng::encode(pResult->getPNGStreamBuffer(), *m_PixelData, m_nPixelCountX, m_nPixelCountY, LCT_RGBA, 8);
		break;

	default:
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELFORMAT);

	}	

	if (pResult->getJPEGStreamBuffer().empty())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNOTSTOREJPEGIMAGE);

	return pResult.release();
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

		WriteToRawMemory(nStartX, nStartY, nCountX, nCountY, eTargetFormat, (LibMCEnv_pvoid)pValueBuffer, (uint32_t) (nTargetBytesPerPixel * (size_t)nCountX));
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

	ReadFromRawMemory(nStartX, nStartY, nCountX, nCountY, eSourceFormat, (LibMCEnv_pvoid)pValueBuffer, (uint32_t) (nSourceBytesPerPixel * (size_t)nCountX));

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
	case LibMCEnv::eImagePixelFormat::RGBA32bit: writeToRawMemoryEx_RGBA32bit(nStartX, nStartY, nCountX, nCountY, pTypedTarget, nYLineOffset); break;
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
	case LibMCEnv::eImagePixelFormat::RGBA32bit: readFromRawMemoryEx_RGBA32bit(nStartX, nStartY, nCountX, nCountY, pTypedSource, nYLineOffset); break;
	default:
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELFORMAT);
	}

}


void CImageData::SetPixelsFromRawYUY2Data(const LibMCEnv_uint64 nYUY2DataBufferSize, const LibMCEnv_uint8* pYUY2DataBuffer)
{
	if (pYUY2DataBuffer == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

	uint64_t nTotalPixelCount = (size_t)(m_nPixelCountX) * (size_t)(m_nPixelCountY);
	if (nTotalPixelCount == 0)
		return;

	if ((m_nPixelCountX % 2 != 0) || (m_nPixelCountY % 2 != 0))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_YUY2PIXELCOUNTMUSTBEAMULTIPLEOF2);

	if (m_PixelData.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDIMAGEBUFFER);
	if (nYUY2DataBufferSize != (nTotalPixelCount * 2))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDYUY2BUFFERSIZE);

	switch (m_PixelFormat) {
	case LibMCEnv::eImagePixelFormat::GreyScale8bit: convertFromYUY2_GreyScale8bit (pYUY2DataBuffer); break;
	case LibMCEnv::eImagePixelFormat::RGB16bit: convertFromYUY2_RGB16bit(pYUY2DataBuffer); break;
	case LibMCEnv::eImagePixelFormat::RGB24bit: convertFromYUY2_RGB24bit(pYUY2DataBuffer); break;
	case LibMCEnv::eImagePixelFormat::RGBA32bit: convertFromYUY2_RGBA32bit(pYUY2DataBuffer); break;
	default:
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELFORMAT);
	}

}


void CImageData::convertFromYUY2_GreyScale8bit(const uint8_t* pSource)
{
	if (pSource == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
	if (m_PixelData == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDIMAGEBUFFER);

	uint8_t* pTarget = m_PixelData->data();

	uint64_t byteIndex = 0;
	uint64_t byteCount = (uint64_t)m_nPixelCountX * (uint64_t)m_nPixelCountY * 2;
	for (uint64_t byteIndex = 0; byteIndex < byteCount; byteIndex += 4) {

		// Read the YUY2 data (2 pixels per 4 bytes)
		uint8_t Y1 = pSource[byteIndex];
		uint8_t U = pSource[byteIndex + 1];
		uint8_t Y2 = pSource[byteIndex + 2];
		uint8_t V = pSource[byteIndex + 3];

		// Convert YUV to RGB for two pixels
		for (int j = 0; j < 2; j++) {
			uint8_t Y = (j == 0) ? Y1 : Y2;

			// YUV to RGB conversion formulas
			int C = Y - 16;
			int D = U - 128;
			int E = V - 128;

			int R = (298 * C + 409 * E + 128) >> 8;
			int G = (298 * C - 100 * D - 208 * E + 128) >> 8;
			int B = (298 * C + 516 * D + 128) >> 8;

			// Clamp RGB values to [0, 255]
			R = std::min(255, std::max(0, R));
			G = std::min(255, std::max(0, G));
			B = std::min(255, std::max(0, B));

			// Average computation
			int nAverage = (R + G + B) / 3;

			*pTarget = (uint8_t)nAverage; pTarget++;
		}
	}
}

void CImageData::convertFromYUY2_RGB16bit(const uint8_t* pSource)
{
	if (pSource == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
	if (m_PixelData == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDIMAGEBUFFER);

	uint16_t* pTarget = (uint16_t *) m_PixelData->data();

	uint64_t byteIndex = 0;
	uint64_t byteCount = (uint64_t)m_nPixelCountX * (uint64_t)m_nPixelCountY * 2;
	for (uint64_t byteIndex = 0; byteIndex < byteCount; byteIndex += 4) {

		// Read the YUY2 data (2 pixels per 4 bytes)
		uint8_t Y1 = pSource[byteIndex];
		uint8_t U = pSource[byteIndex + 1];
		uint8_t Y2 = pSource[byteIndex + 2];
		uint8_t V = pSource[byteIndex + 3];

		// Convert YUV to RGB for two pixels
		for (int j = 0; j < 2; j++) {
			uint8_t Y = (j == 0) ? Y1 : Y2;

			// YUV to RGB conversion formulas
			int C = Y - 16;
			int D = U - 128;
			int E = V - 128;

			int R = (298 * C + 409 * E + 128) >> 8;
			int G = (298 * C - 100 * D - 208 * E + 128) >> 8;
			int B = (298 * C + 516 * D + 128) >> 8;

			// Clamp RGB values to [0, 255]
			R = std::min(255, std::max(0, R));
			G = std::min(255, std::max(0, G));
			B = std::min(255, std::max(0, B));

			*pTarget = ((B & 0xF8) << 8) | ((G & 0xFC) << 3) | (R >> 3);
		}
	}
}

void CImageData::convertFromYUY2_RGB24bit(const uint8_t* pSource)
{
	if (pSource == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
	if (m_PixelData == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDIMAGEBUFFER);

	uint8_t* pTarget = m_PixelData->data();

	uint64_t byteIndex = 0;
	uint64_t byteCount = (uint64_t) m_nPixelCountX * (uint64_t) m_nPixelCountY * 2;
	for (uint64_t byteIndex = 0; byteIndex < byteCount; byteIndex += 4) {

		// Read the YUY2 data (2 pixels per 4 bytes)
		uint8_t Y1 = pSource[byteIndex];
		uint8_t U = pSource[byteIndex + 1];
		uint8_t Y2 = pSource[byteIndex + 2];
		uint8_t V = pSource[byteIndex + 3];

		// Convert YUV to RGB for two pixels
		for (int j = 0; j < 2; j++) {
			uint8_t Y = (j == 0) ? Y1 : Y2;

			// YUV to RGB conversion formulas
			int C = Y - 16;
			int D = U - 128;
			int E = V - 128;

			int R = (298 * C + 409 * E + 128) >> 8;
			int G = (298 * C - 100 * D - 208 * E + 128) >> 8;
			int B = (298 * C + 516 * D + 128) >> 8;

			// Clamp RGB values to [0, 255]
			R = std::min(255, std::max(0, R));
			G = std::min(255, std::max(0, G));
			B = std::min(255, std::max(0, B));

			*pTarget = (uint8_t)R; pTarget++;
			*pTarget = (uint8_t)G; pTarget++;
			*pTarget = (uint8_t)B; pTarget++;
		}
	}
}

void CImageData::convertFromYUY2_RGBA32bit(const uint8_t* pSource)
{
	if (pSource == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
	if (m_PixelData == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDIMAGEBUFFER);

	uint8_t* pTarget = m_PixelData->data();

	uint64_t byteIndex = 0;
	uint64_t byteCount = (uint64_t)m_nPixelCountX * (uint64_t)m_nPixelCountY * 2;
	for (uint64_t byteIndex = 0; byteIndex < byteCount; byteIndex += 4) {

		// Read the YUY2 data (2 pixels per 4 bytes)
		uint8_t Y1 = pSource[byteIndex];
		uint8_t U = pSource[byteIndex + 1];
		uint8_t Y2 = pSource[byteIndex + 2];
		uint8_t V = pSource[byteIndex + 3];

		// Convert YUV to RGB for two pixels
		for (int j = 0; j < 2; j++) {
			uint8_t Y = (j == 0) ? Y1 : Y2;

			// YUV to RGB conversion formulas
			int C = Y - 16;
			int D = U - 128;
			int E = V - 128;

			int R = (298 * C + 409 * E + 128) >> 8;
			int G = (298 * C - 100 * D - 208 * E + 128) >> 8;
			int B = (298 * C + 516 * D + 128) >> 8;

			// Clamp RGB values to [0, 255]
			R = std::min(255, std::max(0, R));
			G = std::min(255, std::max(0, G));
			B = std::min(255, std::max(0, B));

			*pTarget = (uint8_t)R; pTarget++;
			*pTarget = (uint8_t)G; pTarget++;
			*pTarget = (uint8_t)B; pTarget++;
			*pTarget = 255; pTarget++;
		}
	}
}



std::vector <uint8_t>& CImageData::getPixelData()
{
	return *m_PixelData.get ();
}

void CImageData::writeToRawMemoryEx_BlackWhite1bit(uint32_t nStartX, uint32_t nStartY, uint32_t nCountX, uint32_t nCountY, uint8_t* pTarget, uint32_t nYLineOffset)
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

				uint8_t nGrayValue1bit = nGrayValue / 128;

				uint32_t nColumnMod = nColumn % 8;
				if (nColumnMod == 0) {
					*pPixelPtr = nGrayValue1bit;

				}
				else {
					*pPixelPtr |= nGrayValue1bit << nColumnMod;
				}

				if (nColumnMod == 7)
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

				uint8_t nGrayValue1bit = nGray / 128;

				uint32_t nColumnMod = nColumn % 8;
				if (nColumnMod == 0) {
					*pPixelPtr = nGrayValue1bit;

				}
				else {
					*pPixelPtr |= nGrayValue1bit << nColumnMod;
				}

				if (nColumnMod == 7)
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

				uint8_t nGrayValue1bit = nGray / 128;

				uint32_t nColumnMod = nColumn % 8;
				if (nColumnMod == 0) {
					*pPixelPtr = nGrayValue1bit;

				}
				else {
					*pPixelPtr |= nGrayValue1bit << nColumnMod;
				}

				if (nColumnMod == 7)
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
			nPixelAddress++; // Ignore alpha

			uint8_t nGray = (uint8_t)((nRed + nGreen + nBlue) / 3);
			uint8_t nGrayValue1bit = nGray / 128;

			uint32_t nColumnMod = nColumn % 8;
			if (nColumnMod == 0) {
				*pPixelPtr = nGrayValue1bit;

			}
			else {
				*pPixelPtr |= nGrayValue1bit << nColumnMod;
			}

			if (nColumnMod == 7)
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

void CImageData::writeToRawMemoryEx_GreyScale2bit(uint32_t nStartX, uint32_t nStartY, uint32_t nCountX, uint32_t nCountY, uint8_t* pTarget, uint32_t nYLineOffset)
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

				uint8_t nGrayValue2bit = nGrayValue / 64;

				uint32_t nColumnMod = nColumn % 4;
				if (nColumnMod == 0) {
					*pPixelPtr = nGrayValue2bit;

				}
				else {
					*pPixelPtr |= nGrayValue2bit << (nColumnMod * 2);
				}

				if (nColumnMod == 3)
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

				uint8_t nGrayValue2bit = nGray / 64;

				uint32_t nColumnMod = nColumn % 4;
				if (nColumnMod == 0) {
					*pPixelPtr = nGrayValue2bit;

				}
				else {
					*pPixelPtr |= nGrayValue2bit << (nColumnMod * 2);
				}

				if (nColumnMod == 3)
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

				uint8_t nGrayValue2bit = nGray / 64;

				uint32_t nColumnMod = nColumn % 4;
				if (nColumnMod == 0) {
					*pPixelPtr = nGrayValue2bit;

				}
				else {
					*pPixelPtr |= nGrayValue2bit << (nColumnMod * 2);
				}

				if (nColumnMod == 3)
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
			nPixelAddress++; // Ignore alpha

			uint8_t nGray = (uint8_t)((nRed + nGreen + nBlue) / 3);
			uint8_t nGrayValue2bit = nGray / 64;

			uint32_t nColumnMod = nColumn % 4;
			if (nColumnMod == 0) {
				*pPixelPtr = nGrayValue2bit;

			}
			else {
				*pPixelPtr |= nGrayValue2bit << (nColumnMod * 2);
			}

			if (nColumnMod == 3)
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

void CImageData::writeToRawMemoryEx_GreyScale4bit(uint32_t nStartX, uint32_t nStartY, uint32_t nCountX, uint32_t nCountY, uint8_t* pTarget, uint32_t nYLineOffset)
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
				uint8_t nGrayValue4Bit = m_PixelData->at(nPixelAddress) / 16;
				nPixelAddress++;

				if (nColumn % 2 == 0) {
					*pPixelPtr = nGrayValue4Bit;
				}
				else {
					*pPixelPtr |= (nGrayValue4Bit << 4);
					pPixelPtr++;

				}
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

				uint8_t nGrayValue4Bit = nGray / 16;
				if (nColumn % 2 == 0) {
					*pPixelPtr = nGrayValue4Bit;
				}
				else {
					*pPixelPtr |= (nGrayValue4Bit << 4);
					pPixelPtr++;
				}
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
				uint8_t nGrayValue4Bit = nGray / 16;
				if (nColumn % 2 == 0) {
					*pPixelPtr = nGrayValue4Bit;
				}
				else {
					*pPixelPtr |= (nGrayValue4Bit << 4);
					pPixelPtr++;
				}
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
			nPixelAddress++; // Ignore alpha

			uint8_t nGray = (uint8_t)((nRed + nGreen + nBlue) / 3);
			uint8_t nGrayValue4Bit = nGray / 16;
			if (nColumn % 2 == 0) {
				*pPixelPtr = nGrayValue4Bit;
			}
			else {
				*pPixelPtr |= (nGrayValue4Bit << 4);
				pPixelPtr++;
			}
		}

		pLinePtr += nYLineOffset;
		nLineAddress += (size_t)m_nPixelCountX * 4;

		break;
	}

	default:
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELFORMAT);

	}
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

	uint8_t* pLinePtr = pTarget;

	switch (m_PixelFormat) {
	case eImagePixelFormat::GreyScale8bit: {
		size_t nLineAddress = (size_t)nStartX + (size_t)nStartY * (size_t)m_nPixelCountX;

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
		size_t nLineAddress = ((size_t)nStartX + (size_t)nStartY * (size_t)m_nPixelCountX) * 2;

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
		size_t nLineAddress = ((size_t)nStartX + (size_t)nStartY * (size_t)m_nPixelCountX) * 3;

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
		size_t nLineAddress = ((size_t)nStartX + (size_t)nStartY * (size_t)m_nPixelCountX) * 4;

		for (uint32_t nRow = 0; nRow < nCountY; nRow++) {

			size_t nPixelAddress = nLineAddress;
			uint16_t* pPixelPtr = (uint16_t*)pLinePtr;

			for (uint32_t nColumn = 0; nColumn < nCountX; nColumn++) {

				uint32_t nRed = m_PixelData->at(nPixelAddress); nPixelAddress++;
				uint32_t nGreen = m_PixelData->at(nPixelAddress); nPixelAddress++;
				uint32_t nBlue = m_PixelData->at(nPixelAddress); nPixelAddress++;
				nPixelAddress++; // alpha

				*pPixelPtr = ((nBlue & 0xF8) << 8) | ((nGreen & 0xFC) << 3) | (nRed >> 3);

				pPixelPtr++;
			}

			pLinePtr += nYLineOffset;
			nLineAddress += (size_t)m_nPixelCountX * 3;
		}

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

	uint8_t* pLinePtr = pTarget;

	switch (m_PixelFormat) {
	case eImagePixelFormat::GreyScale8bit: {
		size_t nLineAddress = (size_t)nStartX + (size_t)nStartY * (size_t)m_nPixelCountX;

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
		size_t nLineAddress = ((size_t)nStartX + (size_t)nStartY * (size_t)m_nPixelCountX) * 2;
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
		size_t nLineAddress = ((size_t)nStartX + (size_t)nStartY * (size_t)m_nPixelCountX) * 3;
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
		size_t nLineAddress = ((size_t)nStartX + (size_t)nStartY * (size_t)m_nPixelCountX) * 4;
		for (uint32_t nRow = 0; nRow < nCountY; nRow++) {

			size_t nPixelAddress = nLineAddress;
			uint8_t* pPixelPtr = pLinePtr;

			for (uint32_t nColumn = 0; nColumn < nCountX; nColumn++) {

				uint32_t nRed = m_PixelData->at(nPixelAddress); nPixelAddress++;
				uint32_t nGreen = m_PixelData->at(nPixelAddress); nPixelAddress++;
				uint32_t nBlue = m_PixelData->at(nPixelAddress); nPixelAddress++;
				nPixelAddress++; // alpha

				*pPixelPtr = nRed; pPixelPtr++;
				*pPixelPtr = nGreen; pPixelPtr++;
				*pPixelPtr = nBlue; pPixelPtr++;
			}

			pLinePtr += nYLineOffset;
			nLineAddress += (size_t)m_nPixelCountX * 3;
		}

		break;
	}

	default:
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELFORMAT);

	}


}

void CImageData::writeToRawMemoryEx_RGBA32bit(uint32_t nStartX, uint32_t nStartY, uint32_t nCountX, uint32_t nCountY, uint8_t* pTarget, uint32_t nYLineOffset)
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

	uint8_t* pLinePtr = pTarget;

	switch (m_PixelFormat) {
	case eImagePixelFormat::GreyScale8bit: {
		size_t nLineAddress = (size_t)nStartX + (size_t)nStartY * (size_t)m_nPixelCountX;

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
		size_t nLineAddress = ((size_t)nStartX + (size_t)nStartY * (size_t)m_nPixelCountX) * 2;

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
		size_t nLineAddress = ((size_t)nStartX + (size_t)nStartY * (size_t)m_nPixelCountX) * 3;

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
		size_t nLineAddress = ((size_t)nStartX + (size_t)nStartY * (size_t)m_nPixelCountX) * 4;

		for (uint32_t nRow = 0; nRow < nCountY; nRow++) {

			size_t nPixelAddress = nLineAddress;
			uint8_t* pPixelPtr = pLinePtr;

			for (uint32_t nColumn = 0; nColumn < nCountX; nColumn++) {

				uint32_t nRed = m_PixelData->at(nPixelAddress); nPixelAddress++;
				uint32_t nGreen = m_PixelData->at(nPixelAddress); nPixelAddress++;
				uint32_t nBlue = m_PixelData->at(nPixelAddress); nPixelAddress++;
				nPixelAddress++; // alpha

				*pPixelPtr = nRed; pPixelPtr++;
				*pPixelPtr = nGreen; pPixelPtr++;
				*pPixelPtr = nBlue; pPixelPtr++;
				*pPixelPtr = 255;  pPixelPtr++;

				pPixelPtr++;
			}

			pLinePtr += nYLineOffset;
			nLineAddress += (size_t)m_nPixelCountX * 4;
		}

		break;
	}


	default:
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELFORMAT);

	}


}

void CImageData::readFromRawMemoryEx_GreyScale8bit(uint32_t nStartX, uint32_t nStartY, uint32_t nCountX, uint32_t nCountY, const uint8_t* pSource, uint32_t nYLineOffset)
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

	const uint8_t* pLinePtr = pSource;

	switch (m_PixelFormat) {
	case eImagePixelFormat::GreyScale8bit: {
		size_t nLineAddress = (size_t)nStartX + (size_t)nStartY * (size_t)m_nPixelCountX;

		for (uint32_t nRow = 0; nRow < nCountY; nRow++) {

			size_t nPixelAddress = nLineAddress;
			const uint8_t* pPixelPtr = pLinePtr;

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
		size_t nLineAddress = ((size_t)nStartX + (size_t)nStartY * (size_t)m_nPixelCountX) * 3;

		for (uint32_t nRow = 0; nRow < nCountY; nRow++) {

			size_t nPixelAddress = nLineAddress;
			const uint8_t* pPixelPtr = pLinePtr;

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
		size_t nLineAddress = ((size_t)nStartX + (size_t)nStartY * (size_t)m_nPixelCountX) * 3;

		for (uint32_t nRow = 0; nRow < nCountY; nRow++) {

			size_t nPixelAddress = nLineAddress;
			const uint8_t* pPixelPtr = pLinePtr;

			for (uint32_t nColumn = 0; nColumn < nCountX; nColumn++) {
				uint8_t nValue = *pPixelPtr;

				m_PixelData->at(nPixelAddress) = nValue; nPixelAddress++;
				m_PixelData->at(nPixelAddress) = nValue; nPixelAddress++;
				m_PixelData->at(nPixelAddress) = nValue; nPixelAddress++;
				m_PixelData->at(nPixelAddress) = 255; nPixelAddress++;

				pPixelPtr++;
			}

			pLinePtr += nYLineOffset;
			nLineAddress += (size_t)m_nPixelCountX * 3;
		}

		break;
	}


	default:
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELFORMAT);

	}


}

void CImageData::readFromRawMemoryEx_RGB16bit(uint32_t nStartX, uint32_t nStartY, uint32_t nCountX, uint32_t nCountY, const  uint8_t* pSource, uint32_t nYLineOffset)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CImageData::readFromRawMemoryEx_RGB24bit(uint32_t nStartX, uint32_t nStartY, uint32_t nCountX, uint32_t nCountY, const uint8_t* pSource, uint32_t nYLineOffset)
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

	const uint8_t* pLinePtr = pSource;

	switch (m_PixelFormat) {
	case eImagePixelFormat::GreyScale8bit: {
		size_t nLineAddress = (size_t)nStartX + (size_t)nStartY * (size_t)m_nPixelCountX;

		for (uint32_t nRow = 0; nRow < nCountY; nRow++) {

			size_t nPixelAddress = nLineAddress;
			const uint8_t* pPixelPtr = pLinePtr;

			for (uint32_t nColumn = 0; nColumn < nCountX; nColumn++) {
				uint32_t nRed = *pPixelPtr; pPixelPtr++;
				uint32_t nGreen = *pPixelPtr; pPixelPtr++;
				uint32_t nBlue = *pPixelPtr; pPixelPtr++;
				m_PixelData->at(nPixelAddress) = (nRed + nGreen + nBlue) / 2;
				nPixelAddress++;				
			}

			pLinePtr += nYLineOffset;
			nLineAddress += m_nPixelCountX;
		}


		break;
	}
	case eImagePixelFormat::RGB24bit: {
		size_t nLineAddress = ((size_t)nStartX + (size_t)nStartY * (size_t)m_nPixelCountX) * 3;

		for (uint32_t nRow = 0; nRow < nCountY; nRow++) {

			size_t nPixelAddress = nLineAddress;
			const uint8_t* pPixelPtr = pLinePtr;

			for (uint32_t nColumn = 0; nColumn < nCountX; nColumn++) {
				uint32_t nRed = *pPixelPtr; pPixelPtr++;
				uint32_t nGreen = *pPixelPtr; pPixelPtr++;
				uint32_t nBlue = *pPixelPtr; pPixelPtr++;

				m_PixelData->at(nPixelAddress) = nRed; nPixelAddress++;
				m_PixelData->at(nPixelAddress) = nGreen; nPixelAddress++;
				m_PixelData->at(nPixelAddress) = nBlue; nPixelAddress++;
			}

			pLinePtr += nYLineOffset;
			nLineAddress += (size_t)m_nPixelCountX * 3;
		}

		break;
	}
	case eImagePixelFormat::RGBA32bit: {
		size_t nLineAddress = ((size_t)nStartX + (size_t)nStartY * (size_t)m_nPixelCountX) * 4;

		for (uint32_t nRow = 0; nRow < nCountY; nRow++) {

			size_t nPixelAddress = nLineAddress;
			const uint8_t* pPixelPtr = pLinePtr;

			for (uint32_t nColumn = 0; nColumn < nCountX; nColumn++) {
				uint32_t nRed = *pPixelPtr; pPixelPtr++;
				uint32_t nGreen = *pPixelPtr; pPixelPtr++;
				uint32_t nBlue = *pPixelPtr; pPixelPtr++;

				m_PixelData->at(nPixelAddress) = nRed; nPixelAddress++;
				m_PixelData->at(nPixelAddress) = nGreen; nPixelAddress++;
				m_PixelData->at(nPixelAddress) = nBlue; nPixelAddress++;
				m_PixelData->at(nPixelAddress) = 255; nPixelAddress++;
			}

			pLinePtr += nYLineOffset;
			nLineAddress += (size_t)m_nPixelCountX * 4;
		}

		break;
	}

	default:
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELFORMAT);

	}


}

void CImageData::readFromRawMemoryEx_RGBA32bit(uint32_t nStartX, uint32_t nStartY, uint32_t nCountX, uint32_t nCountY, const uint8_t* pSource, uint32_t nYLineOffset)
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

	const uint8_t* pLinePtr = pSource;

	switch (m_PixelFormat) {
	case eImagePixelFormat::GreyScale8bit: {
		size_t nLineAddress = (size_t)nStartX + (size_t)nStartY * (size_t)m_nPixelCountX;

		for (uint32_t nRow = 0; nRow < nCountY; nRow++) {

			size_t nPixelAddress = nLineAddress;
			const uint8_t* pPixelPtr = pLinePtr;

			for (uint32_t nColumn = 0; nColumn < nCountX; nColumn++) {
				uint32_t nRed = *pPixelPtr; pPixelPtr++;
				uint32_t nGreen = *pPixelPtr; pPixelPtr++;
				uint32_t nBlue = *pPixelPtr; pPixelPtr++;
				pPixelPtr++; // alpha
				m_PixelData->at(nPixelAddress) = (nRed + nGreen + nBlue) / 2;
				nPixelAddress++;
			}

			pLinePtr += nYLineOffset;
			nLineAddress += m_nPixelCountX;
		}


		break;
	}
	case eImagePixelFormat::RGB24bit: {
		size_t nLineAddress = ((size_t)nStartX + (size_t)nStartY * (size_t)m_nPixelCountX) * 3;

		for (uint32_t nRow = 0; nRow < nCountY; nRow++) {

			size_t nPixelAddress = nLineAddress;
			const uint8_t* pPixelPtr = pLinePtr;

			for (uint32_t nColumn = 0; nColumn < nCountX; nColumn++) {
				uint32_t nRed = *pPixelPtr; pPixelPtr++;
				uint32_t nGreen = *pPixelPtr; pPixelPtr++;
				uint32_t nBlue = *pPixelPtr; pPixelPtr++;
				pPixelPtr++; // alpha

				m_PixelData->at(nPixelAddress) = nRed; nPixelAddress++;
				m_PixelData->at(nPixelAddress) = nGreen; nPixelAddress++;
				m_PixelData->at(nPixelAddress) = nBlue; nPixelAddress++;
			}

			pLinePtr += nYLineOffset;
			nLineAddress += (size_t)m_nPixelCountX * 3;
		}

		break;
	}
	case eImagePixelFormat::RGBA32bit: {
		size_t nLineAddress = ((size_t)nStartX + (size_t)nStartY * (size_t)m_nPixelCountX) * 4;

		for (uint32_t nRow = 0; nRow < nCountY; nRow++) {

			size_t nPixelAddress = nLineAddress;
			const uint8_t* pPixelPtr = pLinePtr;

			for (uint32_t nColumn = 0; nColumn < nCountX; nColumn++) {
				uint32_t nRed = *pPixelPtr; pPixelPtr++;
				uint32_t nGreen = *pPixelPtr; pPixelPtr++;
				uint32_t nBlue = *pPixelPtr; pPixelPtr++;
				pPixelPtr++; // alpha

				m_PixelData->at(nPixelAddress) = nRed; nPixelAddress++;
				m_PixelData->at(nPixelAddress) = nGreen; nPixelAddress++;
				m_PixelData->at(nPixelAddress) = nBlue; nPixelAddress++;
				m_PixelData->at(nPixelAddress) = 255; nPixelAddress++;
			}

			pLinePtr += nYLineOffset;
			nLineAddress += (size_t)m_nPixelCountX * 4;
		}

		break;
	}

	default:
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELFORMAT);

	}


}
