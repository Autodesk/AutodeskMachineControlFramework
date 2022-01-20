/*++

Copyright (C) 2021 Autodesk Inc.

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


Abstract: This is a stub class definition of CImageObject

*/

#include "librasterizer_imageobject.hpp"
#include "librasterizer_interfaceexception.hpp"

// Include custom headers here.
#include "lodepng.h"
#include <fstream>
#include <cmath>

using namespace LibRasterizer::Impl;

/*************************************************************************************************************************
 Class definition of CImageObject 
**************************************************************************************************************************/
CImageObject::CImageObject(const uint32_t nPixelCountX, const uint32_t nPixelCountY, const double dDPIValueX, const double dDPIValueY)
    : m_nPixelCountX (nPixelCountX), m_nPixelCountY (nPixelCountY), m_dDPIValueX (dDPIValueX), m_dDPIValueY (dDPIValueY),
	m_dPositionX (0.0), m_dPositionY (0.0)
{
	if (nPixelCountX <= 0)
		throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_INVALIDPARAM);
	if (nPixelCountY <= 0)
		throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_INVALIDPARAM);
	if (dDPIValueX <= 0.0)
		throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_INVALIDPARAM);
	if (dDPIValueY <= 0.0)
		throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_INVALIDPARAM);

	m_PixelData.resize((size_t)m_nPixelCountX * (size_t)m_nPixelCountY);

	Clear(0);
	
}

CImageObject::~CImageObject()
{

}

void CImageObject::Clear(const LibRasterizer_uint8 nValue)
{
	for (auto iter = m_PixelData.begin(); iter != m_PixelData.end(); iter++)
		*iter = nValue;
}


void CImageObject::GetDPI(LibRasterizer_double & dDPIValueX, LibRasterizer_double & dDPIValueY)
{
	dDPIValueX = m_dDPIValueX;
	dDPIValueY = m_dDPIValueY;
}

void CImageObject::GetSize(LibRasterizer_double & dSizeX, LibRasterizer_double & dSizeY)
{
	// 1 inch is 25.4 mm
	double dMMperPixelX = 25.4 / m_dDPIValueX;
	double dMMperPixelY = 25.4 / m_dDPIValueY;
	dSizeX = m_nPixelCountX * dMMperPixelX;
	dSizeY = m_nPixelCountY * dMMperPixelY;
}

void CImageObject::GetPixelSize(LibRasterizer_uint32 & nPixelSizeX, LibRasterizer_uint32 & nPixelSizeY)
{
	nPixelSizeX = m_nPixelCountX;
	nPixelSizeY = m_nPixelCountY;
}

void CImageObject::SaveToPNG(const std::string & sFileName)
{
	std::vector<uint8_t> pngData;
	unsigned int error = lodepng::encode(pngData, m_PixelData, m_nPixelCountX, m_nPixelCountY, LCT_GREY, 8);

	if (error)
		throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_COULDNOTCOMPRESSPNGIMAGE);

	if (pngData.empty())
		throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_EMPTYPNGIMAGEDATA);

	std::ofstream stream(sFileName, std::ios::binary);
	if (!stream)
		throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_COULDNOTSTOREPNGIMAGE);

	stream.write((char*)pngData.data(), pngData.size());
	stream.close();

}

LibRasterizer_uint8 CImageObject::GetPixel(const LibRasterizer_uint32 nX, const LibRasterizer_uint32 nY)
{
	if (nX >= m_nPixelCountX)
		throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_INVALIDXCOORDINATE);
	if (nY >= m_nPixelCountY)
		throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_INVALIDYCOORDINATE);

	size_t nAddress = (size_t)nX + (size_t)nY * (size_t)m_nPixelCountX;
	return m_PixelData[nAddress];
}

void CImageObject::SetPixel(const LibRasterizer_uint32 nX, const LibRasterizer_uint32 nY, const LibRasterizer_uint8 nValue)
{
	if (nX >= m_nPixelCountX)
		throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_INVALIDXCOORDINATE);
	if (nY >= m_nPixelCountY)
		throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_INVALIDYCOORDINATE);

	size_t nAddress = (size_t)nX + (size_t)nY * (size_t)m_nPixelCountX;
	m_PixelData[nAddress] = nValue;
}

void CImageObject::GetPixelRange(const LibRasterizer_uint32 nXMin, const LibRasterizer_uint32 nYMin, const LibRasterizer_uint32 nXMax, const LibRasterizer_uint32 nYMax, LibRasterizer_uint64 nValueBufferSize, LibRasterizer_uint64* pValueNeededCount, LibRasterizer_uint8* pValueBuffer)
{
	if (nXMin >= m_nPixelCountX)
		throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_INVALIDXCOORDINATE);
	if (nYMin >= m_nPixelCountY)
		throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_INVALIDYCOORDINATE);
	if (nXMax >= m_nPixelCountX)
		throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_INVALIDXCOORDINATE);
	if (nYMax >= m_nPixelCountY)
		throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_INVALIDYCOORDINATE);
	if (nXMin > nXMax)
		throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_INVALIDXCOORDINATERANGE);
	if (nYMin > nYMax)
		throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_INVALIDYCOORDINATERANGE);

	uint32_t nSizeX = nXMax - nXMin;
	uint32_t nSizeY = nYMax - nYMin;
	size_t nNeededSize = (size_t)nSizeX * (size_t)nSizeY;

	if (pValueNeededCount != nullptr)
		*pValueNeededCount = nNeededSize;

	if (pValueBuffer != nullptr) {
		if (nValueBufferSize < nNeededSize)
			throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_BUFFERTOOSMALL);

		uint8_t* pTargetData = pValueBuffer;
		for (uint32_t nY = 0; nY < nSizeY; nY++) {
			const uint8_t * pSrcData = &m_PixelData[(nY + nYMin) * m_nPixelCountX + nXMin];

			for (uint32_t nX = 0; nX < nSizeX; nX++) {
				*pTargetData = *pSrcData;
				pTargetData++;
				pSrcData++;
			}
		}

	}

}

void CImageObject::SetPixelRange(const LibRasterizer_uint32 nXMin, const LibRasterizer_uint32 nYMin, const LibRasterizer_uint32 nXMax, const LibRasterizer_uint32 nYMax, const LibRasterizer_uint64 nValueBufferSize, const LibRasterizer_uint8* pValueBuffer)
{
	if (pValueBuffer == nullptr)
		throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_INVALIDPARAM);
	if (nXMin >= m_nPixelCountX)
		throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_INVALIDXCOORDINATE);
	if (nYMin >= m_nPixelCountY)
		throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_INVALIDYCOORDINATE);
	if (nXMax >= m_nPixelCountX)
		throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_INVALIDXCOORDINATE);
	if (nYMax >= m_nPixelCountY)
		throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_INVALIDYCOORDINATE);
	if (nXMin > nXMax)
		throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_INVALIDXCOORDINATERANGE);
	if (nYMin > nYMax)
		throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_INVALIDYCOORDINATERANGE);

	uint32_t nSizeX = nXMax - nXMin;
	uint32_t nSizeY = nYMax - nYMin;
	size_t nNeededSize = (size_t)nSizeX * (size_t)nSizeY;
	if (nValueBufferSize != nNeededSize)
		throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_INVALIDPIXELDATACOUNT);

	const uint8_t* pSrcData = pValueBuffer;
	for (uint32_t nY = 0; nY < nSizeY; nY++) {
		uint8_t * pTargetData = &m_PixelData[(nY + nYMin) * m_nPixelCountX + nXMin];

		for (uint32_t nX = 0; nX < nSizeX; nX++) {
			*pTargetData = *pSrcData;
			pTargetData++;
			pSrcData++;
		}
	}

}


void CImageObject::drawLayerObject(CLayerDataObject* pLayerDataObject, uint8_t nValue)
{
	if (pLayerDataObject == nullptr)
		throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_INVALIDPARAM);

	size_t nEntityCount = pLayerDataObject->getEntityCount();
	for (size_t nEntityIndex = 0; nEntityIndex < nEntityCount; nEntityIndex++) {
		drawLayerEntity(&pLayerDataObject->getEntity(nEntityIndex), nValue);
	}
}

void CImageObject::drawLayerEntity(CLayerDataEntity* pLayerEntity, uint8_t nValue)
{
	if (pLayerEntity == nullptr)
		throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_INVALIDPARAM);

	auto geometryType = pLayerEntity->getGeometryType();
	auto& points = pLayerEntity->getPoints();
	size_t nPointCount = points.size();
	size_t nHatchCount;

	if (nPointCount > 0) {

		switch (geometryType) {
		case eGeometryType::SolidGeometry:
			for (size_t nPointIndex = 0; nPointIndex < nPointCount; nPointIndex++) {
				double dX1 = points[nPointIndex].m_Coordinates[0];
				double dY1 = points[nPointIndex].m_Coordinates[1];
				double dX2 = points[(nPointIndex + 1) % nPointCount].m_Coordinates[0];
				double dY2 = points[(nPointIndex + 1) % nPointCount].m_Coordinates[1];
				drawLine(dX1, dY1, dX2, dY2, nValue);
			}

			break;

		case eGeometryType::OpenPolyline:
			for (size_t nPointIndex = 1; nPointIndex < nPointCount; nPointIndex++) {
				double dX1 = points[nPointIndex - 1].m_Coordinates[0];
				double dY1 = points[nPointIndex - 1].m_Coordinates[1];
				double dX2 = points[nPointIndex].m_Coordinates[0];
				double dY2 = points[nPointIndex].m_Coordinates[1];
				drawLine(dX1, dY1, dX2, dY2, nValue);
			}
			break;

		case eGeometryType::OpenHatches:
			nHatchCount = nPointCount / 2;
			for (size_t nHatchIndex = 0; nHatchIndex < nHatchCount; nHatchIndex++) {
				double dX1 = points[nHatchIndex * 2].m_Coordinates[0];
				double dY1 = points[nHatchIndex * 2].m_Coordinates[1];
				double dX2 = points[nHatchIndex * 2 + 1].m_Coordinates[0];
				double dY2 = points[nHatchIndex * 2 + 1].m_Coordinates[1];
				drawLine(dX1, dY1, dX2, dY2, nValue);
			}
			break;

		}
	}

}

void CImageObject::drawLine(double dX1, double dY1, double dX2, double dY2, uint8_t nValue)
{
	int32_t intX1 = (int32_t)round((dX1 - m_dPositionX) * m_dDPIValueX / 25.4);
	int32_t intY1 = (int32_t)round((dY1 - m_dPositionY) * m_dDPIValueY / 25.4);
	int32_t intX2 = (int32_t)round((dX2 - m_dPositionX) * m_dDPIValueX / 25.4);
	int32_t intY2 = (int32_t)round((dY2 - m_dPositionY) * m_dDPIValueY / 25.4);

	if ((intX1 < 0) && (intX2 < 0))
		return;
	if ((intY1 < 0) && (intY2 < 0))
		return;
	if ((intX1 > (int64_t) m_nPixelCountX) && (intX2 > (int64_t)  m_nPixelCountX))
		return;
	if ((intY1 > (int64_t) m_nPixelCountY) && (intY2 > (int64_t) m_nPixelCountY))
		return;

	int dx = abs(intX2 - intX1), sx = intX1 < intX2 ? 1 : -1;
	int dy = -abs(intY2 - intY1), sy = intY1 < intY2 ? 1 : -1;
	int err = dx + dy, e2; /* error value e_xy */
	
	while (1) {
		if ((intX1 >= 0) && (intY1 >= 0) && (intX1 < (int64_t) m_nPixelCountX) && (intY1 < (int64_t)m_nPixelCountY)) {
			SetPixel(intX1, intY1, nValue);
		}

		if (intX1 == intX2 && intY1 == intY2) break;
		e2 = 2 * err;
		if (e2 > dy) { err += dy; intX1 += sx; } /* e_xy+e_x > 0 */
		if (e2 < dx) { err += dx; intY1 += sy; } /* e_xy+e_y < 0 */
	}

}

void CImageObject::setPosition(double dPositionX, double dPositionY)
{
	m_dPositionX = dPositionX;
	m_dPositionY = dPositionY;
}
