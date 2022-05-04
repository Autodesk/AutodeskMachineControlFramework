/*++

Copyright (C) 2022 Autodesk Inc.

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

#include "libmcdriver_rasterizer_imageobject.hpp"
#include "libmcdriver_rasterizer_interfaceexception.hpp"

// Include custom headers here.
#include <cmath>


using namespace LibMCDriver_Rasterizer::Impl;

/*************************************************************************************************************************
 Class definition of CImageObject 
**************************************************************************************************************************/

CImageObject::CImageObject(const uint32_t nPixelCountX, const uint32_t nPixelCountY, const double dDPIValueX, const double dDPIValueY)
	: m_nPixelCountX(nPixelCountX), m_nPixelCountY(nPixelCountY), m_dDPIValueX(dDPIValueX), m_dDPIValueY(dDPIValueY),
	m_dPositionX(0.0), m_dPositionY(0.0), m_nBlockCountX(0), m_nBlockCountY(0), m_nUnitsPerSubPixel(0),
	m_nSubSamplingX (0), m_nSubSamplingY (0), m_dUnitsX (0.0), m_dUnitsY (0.0), m_nPixelsPerBlock (0)
{
	if (nPixelCountX <= 0)
		throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_INVALIDPIXELCOUNT);
	if (nPixelCountY <= 0)
		throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_INVALIDPIXELCOUNT);
	if (dDPIValueX <= 0.0)
		throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_INVALIDDPIVALUE);
	if (dDPIValueY <= 0.0)
		throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_INVALIDDPIVALUE);

	m_PixelData.resize((size_t)m_nPixelCountX * (size_t)m_nPixelCountY);

	clear(0);

}

CImageObject::~CImageObject()
{

}



void CImageObject::clear(uint8_t nValue)
{
	for (auto iter = m_PixelData.begin(); iter != m_PixelData.end(); iter++)
		*iter = nValue;
}

void CImageObject::setPixel(const LibMCDriver_Rasterizer_uint32 nX, const LibMCDriver_Rasterizer_uint32 nY, const LibMCDriver_Rasterizer_uint8 nValue)
{
	if (nX >= m_nPixelCountX)
		return;
	if (nY >= m_nPixelCountY)
		return;

	size_t nAddress = (size_t)nX + (size_t)nY * (size_t)m_nPixelCountX;
	m_PixelData[nAddress] = nValue;
}


void CImageObject::drawLayerObject(CLayerDataObject * pLayerDataObject, uint8_t nValue)
{
	if (pLayerDataObject == nullptr)
		throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_INVALIDPARAM);

	size_t nEntityCount = pLayerDataObject->getEntityCount();
	for (size_t nEntityIndex = 0; nEntityIndex < nEntityCount; nEntityIndex++) {
		drawLayerEntity(&pLayerDataObject->getEntity(nEntityIndex), nValue);
	}
}

void CImageObject::drawLayerEntity(CLayerDataEntity * pLayerEntity, uint8_t nValue)
{
	if (pLayerEntity == nullptr)
		throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_INVALIDPARAM);

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
		if ((intX1 >= 0) && (intY1 >= 0) && (intX1 < (int64_t)m_nPixelCountX) && (intY1 < (int64_t)m_nPixelCountY)) {
			setPixel(intX1, intY1, nValue);
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

std::vector<uint8_t>& CImageObject::getBuffer()
{
	return m_PixelData;
}


void CImageObject::initRasterizationAlgorithms(uint32_t nUnitsPerSubPixel, uint32_t nPixelsPerBlock, uint32_t nSubSamplingX, uint32_t nSubSamplingY)
{
	if (nSubSamplingX < RASTERER_MINSUBSAMPLING)
		throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_INVALIDSUBSAMPLING);
	if (nSubSamplingY < RASTERER_MINSUBSAMPLING)
		throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_INVALIDSUBSAMPLING);
	if (nSubSamplingX > RASTERER_MAXSUBSAMPLING)
		throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_INVALIDSUBSAMPLING);
	if (nSubSamplingY > RASTERER_MAXSUBSAMPLING)
		throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_INVALIDSUBSAMPLING);

	m_nBlockCountX = (m_nPixelCountX + nPixelsPerBlock - 1) / nPixelsPerBlock;
	m_nBlockCountY = (m_nPixelCountY + nPixelsPerBlock - 1) / nPixelsPerBlock;
	m_nUnitsPerSubPixel = nUnitsPerSubPixel;
	m_nPixelsPerBlock = nPixelsPerBlock;

	m_dUnitsX = (25.4 / m_dDPIValueX) / (double)(nSubSamplingX * m_nUnitsPerSubPixel);
	m_dUnitsY = (25.4 / m_dDPIValueY) / (double)(nSubSamplingY * m_nUnitsPerSubPixel);

	m_nSubSamplingX = nSubSamplingX;
	m_nSubSamplingY = nSubSamplingY;

	m_Algorithms.clear();

}

void CImageObject::addRasterizationLayer(CLayerDataObject* pLayer)
{
	if (pLayer == nullptr)
		throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_INVALIDPARAM);

	if ((m_nBlockCountX == 0) || (m_nBlockCountY == 0))
		throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_RASTERIZATIONNOTINITIALIZED);

	uint32_t nExpectedLineCount = (uint32_t) pLayer->calculateClosedPolygonLineCount();
	auto pAlgorithm = std::make_shared<CRasterizationAlgorithm>(m_nUnitsPerSubPixel, m_nSubSamplingX, m_nSubSamplingY, m_nPixelsPerBlock, m_nBlockCountX, m_nBlockCountY, nExpectedLineCount);

	pLayer->addClosedPolygonsToAlgorithm(pAlgorithm.get(), m_dUnitsX, m_dUnitsY);
	pAlgorithm->buildBlocks();
	for (uint32_t nBlockY = 0; nBlockY < m_nBlockCountY; nBlockY++) {
		for (uint32_t nBlockX = 0; nBlockX < m_nBlockCountX; nBlockX++) {
			pAlgorithm->buildBlockScanLines (nBlockX, nBlockY);
		}
	}
			

	m_Algorithms.push_back(pAlgorithm);

}

void CImageObject::calculateRasterizationImage(bool bAntiAliased)
{
	if ((m_nBlockCountX == 0) || (m_nBlockCountY == 0))
		throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_RASTERIZATIONNOTINITIALIZED);
	if (m_Algorithms.empty ()) 
		throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_INVALIDSUBSAMPLING);

	uint32_t nNumberOfZSamples = (uint32_t) m_Algorithms.size();
	uint32_t nValueRange = 255;
	uint32_t nValueBWThreshold = nValueRange / 2;

	std::vector<CRasterizationAlgorithm*> m_ActiveAlgorithms;
	m_ActiveAlgorithms.resize (nNumberOfZSamples);

	std::vector<uint32_t> m_BlockBuffer;
	m_BlockBuffer.resize((size_t)m_nPixelsPerBlock * (size_t)m_nPixelsPerBlock);

	for (uint32_t nBlockX = 0; nBlockX < m_nBlockCountX; nBlockX++) {
		for (uint32_t nBlockY = 0; nBlockY < m_nBlockCountY; nBlockY++) {
			
			uint32_t nBaseValue = 0;
			uint32_t nActiveLayerCount = 0;
			for (auto algorithm : m_Algorithms) {
				auto blockInfo = algorithm->getBlockInfo(nBlockX, nBlockY);
				if (blockInfo == eBlockType::btCompleteInside)
					nBaseValue += nValueRange;
				if (blockInfo == eBlockType::btBorder) {
					m_ActiveAlgorithms[nActiveLayerCount] = algorithm.get();
					nActiveLayerCount++;
				}
			}

			if (nActiveLayerCount == 0) {
				uint32_t nValueNormalized = nBaseValue / nNumberOfZSamples;
				if (!bAntiAliased) {
					if (nValueNormalized > nValueBWThreshold)
						nValueNormalized = nValueRange;
					else
						nValueNormalized = 0;
				}

				for (uint32_t dY = 0; dY < m_nPixelsPerBlock; dY++) {
					uint32_t nY = nBlockY * m_nPixelsPerBlock + dY;
					uint32_t nX = nBlockX * m_nPixelsPerBlock;
					for (uint32_t dX = 0; dX < m_nPixelsPerBlock; dX++) {
						setPixel(nX, nY, (uint8_t)nValueNormalized);
						nX++;
					}
				}
			}
			else {
				for (auto it = m_BlockBuffer.begin(); it != m_BlockBuffer.end(); it++)
					*it = nBaseValue;

				for (uint32_t nSampleIndex = 0; nSampleIndex < nActiveLayerCount; nSampleIndex++) {
					m_ActiveAlgorithms[nSampleIndex]->addBlockToBuffer (nBlockX, nBlockY, m_BlockBuffer);
				}

				for (uint32_t dY = 0; dY < m_nPixelsPerBlock; dY++) {
					uint32_t nY = nBlockY * m_nPixelsPerBlock + dY;
					uint32_t nX = nBlockX * m_nPixelsPerBlock;
					for (uint32_t dX = 0; dX < m_nPixelsPerBlock; dX++) {
						uint32_t nValueNormalized = m_BlockBuffer[(size_t)dX + (size_t)dY * m_nPixelsPerBlock] / nNumberOfZSamples;
						if (!bAntiAliased) {
							if (nValueNormalized > nValueBWThreshold)
								nValueNormalized = nValueRange;
							else
								nValueNormalized = 0;
						}

						setPixel(nX, nY, (uint8_t)nValueNormalized);
						nX++;
					}
				}

			}

		}
	}

	m_nBlockCountX = 0;
	m_nBlockCountY = 0;
	m_Algorithms.clear();

}

