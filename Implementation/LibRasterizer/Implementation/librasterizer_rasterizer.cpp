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


Abstract: This is a stub class definition of CRasterizer

*/

#include "librasterizer_rasterizer.hpp"
#include "librasterizer_interfaceexception.hpp"
#include "librasterizer_imageobject.hpp"

// Include custom headers here.


using namespace LibRasterizer::Impl;

/*************************************************************************************************************************
 Class definition of CRasterer
**************************************************************************************************************************/
CRasterizer::CRasterizer(const uint32_t nPixelCountX, const uint32_t nPixelCountY, double dDPIX, double dDPIY)
	: m_nPixelCountX(nPixelCountX), m_nPixelCountY(nPixelCountY), m_dDPIX(dDPIX), m_dDPIY(dDPIY),
	m_dPositionX(0.0), m_dPositionY(0.0), m_nSubSamplingX(1), m_nSubSamplingY(1)
{
	if (nPixelCountX <= 0)
		throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_INVALIDPARAM);
	if (nPixelCountY <= 0)
		throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_INVALIDPARAM);
	if (dDPIX <= 0.0)
		throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_INVALIDPARAM);
	if (dDPIY <= 0.0)
		throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_INVALIDPARAM);
}

CRasterizer::~CRasterizer()
{

}

void CRasterizer::GetDPI(LibRasterizer_double& dDPIValueX, LibRasterizer_double& dDPIValueY)
{
	dDPIValueX = m_dDPIX;
	dDPIValueY = m_dDPIY;
}

void CRasterizer::GetSize(LibRasterizer_double& dSizeX, LibRasterizer_double& dSizeY)
{
	// 1 inch is 25.4 mm
	double dMMperPixelX = 25.4 / m_dDPIX;
	double dMMperPixelY = 25.4 / m_dDPIY;
	dSizeX = m_nPixelCountX * dMMperPixelX;
	dSizeY = m_nPixelCountY * dMMperPixelY;
}

void CRasterizer::GetPixelSize(LibRasterizer_uint32& nPixelSizeX, LibRasterizer_uint32& nPixelSizeY)
{
	nPixelSizeX = m_nPixelCountX;
	nPixelSizeY = m_nPixelCountY;
}

void CRasterizer::GetPosition(LibRasterizer_double& dPositionX, LibRasterizer_double& dPositionY)
{
	dPositionX = m_dPositionX;
	dPositionY = m_dPositionY;
}

void CRasterizer::SetPosition(const LibRasterizer_double dPositionX, const LibRasterizer_double dPositionY)
{
	m_dPositionX = dPositionX;
	m_dPositionY = dPositionY;
}

void CRasterizer::SetSubsampling(const LibRasterizer_uint32 nSubsamplingX, const LibRasterizer_uint32 nSubsamplingY)
{
	if (nSubsamplingX < RASTERER_MINSUBSAMPLING)
		throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_INVALIDSUBSAMPLING);
	if (nSubsamplingY < RASTERER_MINSUBSAMPLING)
		throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_INVALIDSUBSAMPLING);
	if (nSubsamplingX > RASTERER_MAXSUBSAMPLING)
		throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_INVALIDSUBSAMPLING);
	if (nSubsamplingY > RASTERER_MAXSUBSAMPLING)
		throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_INVALIDSUBSAMPLING);

	m_nSubSamplingX = nSubsamplingX;
	m_nSubSamplingY = nSubsamplingY;
}

void CRasterizer::GetSubsampling(LibRasterizer_uint32& nSubsamplingX, LibRasterizer_uint32& nSubsamplingY)
{
	nSubsamplingX = m_nSubSamplingX;
	nSubsamplingY = m_nSubSamplingY;
}

void CRasterizer::AddLayer(ILayerObject* pLayerObject)
{
	if (pLayerObject == nullptr)
		throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_INVALIDPARAM);

	CLayerObject* pLayerObjectInstance = dynamic_cast <CLayerObject*> (pLayerObject);
	if (pLayerObjectInstance == nullptr)
		throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_INVALIDCAST);

	m_Layers.push_back(pLayerObjectInstance->getDataObject ());
}

IImageObject* CRasterizer::CalculateImage(const bool bAntialiased)
{
	auto pImage = std::make_unique<CImageObject>(m_nPixelCountX, m_nPixelCountY, m_dDPIX, m_dDPIY);

	for (auto pLayer : m_Layers)
		pImage->drawLayerObject(pLayer.get(), 255);

	return pImage.release();
}
