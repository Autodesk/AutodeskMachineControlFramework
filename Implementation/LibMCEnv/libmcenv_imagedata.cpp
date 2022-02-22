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

// Include custom headers here.
#include "Libraries/LodePNG/lodepng.h"

using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CImageData 
**************************************************************************************************************************/

CImageData* CImageData::createFromPNG(const uint8_t* pBuffer, uint64_t nBufferSize, const double dDPIValueX, const double dDPIValueY, eImagePixelFormat pixelFormat)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);

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

	return new CImageData(pixelBuffer.release(), nPixelCountX, nPixelCountY, dDPIValueX, dDPIValueY, pixelFormat);

}

CImageData::CImageData(std::vector<uint8_t>* pPixelData, const uint32_t nPixelCountX, const uint32_t nPixelCountY, const double dDPIValueX, const double dDPIValueY, eImagePixelFormat pixelFormat)
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

	
	//m_PixelData->resize();

	switch (pixelFormat) {
		case eImagePixelFormat::GreyScale8bit: Clear(128); break;
		case eImagePixelFormat::RGBA32bit: Clear(0xffffffffUL); break;
		case eImagePixelFormat::RGB24bit: Clear(0xffffffUL); break;
		default: Clear(0);
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

void CImageData::LoadPNG(LibMCEnv_uint64 nPNGDataBufferSize, LibMCEnv_uint64* pPNGDataNeededCount, LibMCEnv_uint8 * pPNGDataBuffer)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CImageData::EncodePNG()
{
	if (m_PixelData.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDIMAGEBUFFER);

	m_EncodedPNGData.clear();

	unsigned int error = lodepng::encode(m_EncodedPNGData, *m_PixelData, m_nPixelCountX, m_nPixelCountY, LCT_GREY, 8);

	if (error)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNOTCOMPRESSPNGIMAGE);

	if (m_EncodedPNGData.empty())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNOTSTOREPNGIMAGE);
}

void CImageData::GetEncodedPNGData(LibMCEnv_uint64 nPNGDataBufferSize, LibMCEnv_uint64* pPNGDataNeededCount, LibMCEnv_uint8 * pPNGDataBuffer)
{
	if (m_PixelData.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDIMAGEBUFFER);

	if (m_EncodedPNGData.empty())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_EMPTYPNGIMAGEDATA);

	size_t nPNGSize = m_EncodedPNGData.size();

	if (pPNGDataNeededCount != nullptr)
		*pPNGDataNeededCount = m_EncodedPNGData.size();

	if (pPNGDataBuffer != nullptr) {

		if (nPNGDataBufferSize < nPNGSize)
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_BUFFERTOOSMALL);

		uint8_t* pSrc = m_EncodedPNGData.data();
		uint8_t* pDst = pPNGDataBuffer;

		for (size_t nIndex = 0; nIndex < nPNGSize; nIndex++)
			*pDst = *pSrc; pDst++; pSrc++;
	}
}

void CImageData::ClearEncodedPNGData()
{
	m_EncodedPNGData.clear();
}

void CImageData::Clear(const LibMCEnv_uint32 nValue)
{
	if (m_PixelData.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDIMAGEBUFFER);

	for (auto iter = m_PixelData->begin(); iter != m_PixelData->end(); iter++)
		*iter = nValue;

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
	return m_PixelData->at(nAddress);
}

void CImageData::SetPixel(const LibMCEnv_uint32 nX, const LibMCEnv_uint32 nY, const LibMCEnv_uint32 nValue)
{
	if (m_PixelData.get() == nullptr)
		return;

	if (nX >= m_nPixelCountX)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDXCOORDINATE);
	if (nY >= m_nPixelCountY)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDYCOORDINATE);

	size_t nAddress = (size_t)nX + (size_t)nY * (size_t)m_nPixelCountX;
	m_PixelData->at(nAddress) = nValue;
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

	uint32_t nSizeX = nXMax - nXMin;
	uint32_t nSizeY = nYMax - nYMin;
	size_t nNeededSize = (size_t)nSizeX * (size_t)nSizeY;

	if (pValueNeededCount != nullptr)
		*pValueNeededCount = nNeededSize;

	if (pValueBuffer != nullptr) {
		if (nValueBufferSize < nNeededSize)
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_BUFFERTOOSMALL);

		auto & pPixelData = *m_PixelData.get();

		uint8_t* pTargetData = pValueBuffer;
		for (uint32_t nY = 0; nY < nSizeY; nY++) {
			const uint8_t* pSrcData = &pPixelData[((uint64_t)nY + (uint64_t)nYMin) * m_nPixelCountX + nXMin];

			for (uint32_t nX = 0; nX < nSizeX; nX++) {
				*pTargetData = *pSrcData;
				pTargetData++;
				pSrcData++;
			}
		}

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

	uint32_t nSizeX = nXMax - nXMin;
	uint32_t nSizeY = nYMax - nYMin;
	size_t nNeededSize = (size_t)nSizeX * (size_t)nSizeY;
	if (nValueBufferSize != nNeededSize)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPIXELDATACOUNT);

	auto& pPixelData = *m_PixelData.get();

	const uint8_t* pSrcData = pValueBuffer;
	for (uint32_t nY = 0; nY < nSizeY; nY++) {
		uint8_t* pTargetData = &pPixelData[((uint64_t)nY + (uint64_t)nYMin) * m_nPixelCountX + nXMin];

		for (uint32_t nX = 0; nX < nSizeX; nX++) {
			*pTargetData = *pSrcData;
			pTargetData++;
			pSrcData++;
		}
	}
}

