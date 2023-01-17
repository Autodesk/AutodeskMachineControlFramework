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


Abstract: This is a stub class definition of CRasterizer

*/

#include "libmcdriver_rasterizer_rasterizer.hpp"
#include "libmcdriver_rasterizer_interfaceexception.hpp"
#include "libmcdriver_rasterizer_imageobject.hpp"
#include "libmcdriver_rasterizer_algorithm.hpp"

// Include custom headers here.


using namespace LibMCDriver_Rasterizer::Impl;

/*************************************************************************************************************************
 Class definition of CRastererInstance
**************************************************************************************************************************/
CRasterizerInstance::CRasterizerInstance(const uint32_t nPixelCountX, const uint32_t nPixelCountY, double dDPIX, double dDPIY)
	: m_nPixelCountX(nPixelCountX), m_nPixelCountY(nPixelCountY), m_dDPIX(dDPIX), m_dDPIY(dDPIY),
	m_dPositionX(0.0), 
	m_dPositionY(0.0), 
	m_nSubSamplingX(1), 
	m_nSubSamplingY(1),
	m_nPixelsPerBlock (RASTERALGORITHM_DEFAULTPIXELSPERBLOCK),
	m_nUnitsPerSubPixel (RASTERALGORITHM_DEFAULTUNITSPERSUBPIXEL)
{
	if (nPixelCountX <= 0)
		throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_INVALIDPIXELCOUNT);
	if (nPixelCountY <= 0)
		throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_INVALIDPIXELCOUNT);
	if (dDPIX <= 0.0)
		throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_INVALIDDPIVALUE);
	if (dDPIY <= 0.0)
		throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_INVALIDDPIVALUE);
}

CRasterizerInstance::~CRasterizerInstance()
{

}

void CRasterizerInstance::GetDPI(double& dDPIValueX, double& dDPIValueY)
{
	dDPIValueX = m_dDPIX;
	dDPIValueY = m_dDPIY;
}

void CRasterizerInstance::GetSize(double& dSizeX, double& dSizeY)
{
	// 1 inch is 25.4 mm
	double dMMperPixelX = 25.4 / m_dDPIX;
	double dMMperPixelY = 25.4 / m_dDPIY;
	dSizeX = m_nPixelCountX * dMMperPixelX;
	dSizeY = m_nPixelCountY * dMMperPixelY;
}

void CRasterizerInstance::GetPixelSize(uint32_t& nPixelSizeX, uint32_t& nPixelSizeY)
{
	nPixelSizeX = m_nPixelCountX;
	nPixelSizeY = m_nPixelCountY;
}

void CRasterizerInstance::GetPosition(double& dPositionX, double& dPositionY)
{
	dPositionX = m_dPositionX;
	dPositionY = m_dPositionY;
}

void CRasterizerInstance::SetPosition(const double dPositionX, const double dPositionY)
{
	m_dPositionX = dPositionX;
	m_dPositionY = dPositionY;
}

void CRasterizerInstance::SetSubsampling(const uint32_t nSubsamplingX, const uint32_t nSubsamplingY)
{
	if (nSubsamplingX < RASTERER_MINSUBSAMPLING)
		throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_INVALIDSUBSAMPLING);
	if (nSubsamplingY < RASTERER_MINSUBSAMPLING)
		throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_INVALIDSUBSAMPLING);
	if (nSubsamplingX > RASTERER_MAXSUBSAMPLING)
		throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_INVALIDSUBSAMPLING);
	if (nSubsamplingY > RASTERER_MAXSUBSAMPLING)
		throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_INVALIDSUBSAMPLING);

	m_nSubSamplingX = nSubsamplingX;
	m_nSubSamplingY = nSubsamplingY;
}

void CRasterizerInstance::GetSubsampling(uint32_t& nSubsamplingX, uint32_t& nSubsamplingY)
{
	nSubsamplingX = m_nSubSamplingX;
	nSubsamplingY = m_nSubSamplingY;
}

void CRasterizerInstance::AddLayer(ILayerObject* pLayerObject)
{
	if (pLayerObject == nullptr)
		throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_INVALIDPARAM);

	CLayerObject* pLayerObjectInstance = dynamic_cast <CLayerObject*> (pLayerObject);
	if (pLayerObjectInstance == nullptr)
		throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_INVALIDCAST);

	m_Layers.push_back(pLayerObjectInstance->getDataObject());
}

void CRasterizerInstance::CalculateImage(LibMCEnv::CImageData* pImageData, const bool bAntialiased, uint32_t nUnitsPerSubPixel, uint32_t nPixelsPerBlock)
{
	if (pImageData == nullptr)
		throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_INVALIDPARAM);

	if (pImageData->GetPixelFormat () != LibMCEnv::eImagePixelFormat::GreyScale8bit)
		throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_PIXELFORMATSHOULDBEGREYSCALE);

	if ((nUnitsPerSubPixel < RASTERALGORITHM_MINUNITSPERSUBPIXEL) || (nUnitsPerSubPixel > RASTERALGORITHM_MAXUNITSPERSUBPIXEL) || ((nUnitsPerSubPixel % 2) != 0))
		throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_INVALIDUNITSPERSUBPIXEL);
	if ((nPixelsPerBlock < RASTERALGORITHM_MINPIXELSPERBLOCK) || (nPixelsPerBlock > RASTERALGORITHM_MAXPIXELSPERBLOCK))
		throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_INVALIDPIXELSPERBLOCK);

	uint32_t nPixelSizeX = 0;
	uint32_t nPixelSizeY = 0;
	pImageData->GetSizeInPixels(nPixelSizeX, nPixelSizeY);

	if (nPixelSizeX != m_nPixelCountX)
		throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_PIXELSIZEMISMATCH);
	if (nPixelSizeY != m_nPixelCountY)
		throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_PIXELSIZEMISMATCH);

	pImageData->SetDPI(m_dDPIX, m_dDPIY);

	auto pImage = std::make_unique<CImageObject>(m_nPixelCountX, m_nPixelCountY, m_dDPIX, m_dDPIY);

	if (!m_Layers.empty()) {

		pImage->initRasterizationAlgorithms(nUnitsPerSubPixel, nPixelsPerBlock, m_nSubSamplingX, m_nSubSamplingY);
		for (auto pLayer : m_Layers)
			pImage->addRasterizationLayer (pLayer.get());
		pImage->calculateRasterizationImage (bAntialiased);

	}


	pImageData->SetPixelRange(0, 0, m_nPixelCountX, m_nPixelCountY, pImage->getBuffer ());
}



/*************************************************************************************************************************
 Class definition of CRasterizer 
**************************************************************************************************************************/

CRasterizer::CRasterizer(PRasterizerInstance pRasterizerInstance)
	: m_pRasterizerInstance (pRasterizerInstance),
	m_nUnitsPerSubPixel (RASTERALGORITHM_DEFAULTUNITSPERSUBPIXEL),
	m_nPixelsPerBlock (RASTERALGORITHM_DEFAULTPIXELSPERBLOCK)
{
	if (pRasterizerInstance.get() == nullptr)
		throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_INVALIDPARAM);

}

CRasterizer::~CRasterizer()
{

}

void CRasterizer::GetDPI(LibMCDriver_Rasterizer_double & dDPIValueX, LibMCDriver_Rasterizer_double & dDPIValueY)
{
	m_pRasterizerInstance->GetDPI(dDPIValueX, dDPIValueY);
}

void CRasterizer::GetSize(LibMCDriver_Rasterizer_double & dSizeX, LibMCDriver_Rasterizer_double & dSizeY)
{
	m_pRasterizerInstance->GetSize(dSizeX, dSizeY);
}

void CRasterizer::GetPixelSize(LibMCDriver_Rasterizer_uint32 & nPixelSizeX, LibMCDriver_Rasterizer_uint32 & nPixelSizeY)
{
	m_pRasterizerInstance->GetPixelSize(nPixelSizeX, nPixelSizeY);
}

void CRasterizer::GetPosition(LibMCDriver_Rasterizer_double & dPositionX, LibMCDriver_Rasterizer_double & dPositionY)
{
	m_pRasterizerInstance->GetPosition(dPositionX, dPositionY);
}

void CRasterizer::SetPosition(const LibMCDriver_Rasterizer_double dPositionX, const LibMCDriver_Rasterizer_double dPositionY)
{
	m_pRasterizerInstance->SetPosition(dPositionX, dPositionY);
}

void CRasterizer::SetSubsampling(const LibMCDriver_Rasterizer_uint32 nSubsamplingX, const LibMCDriver_Rasterizer_uint32 nSubsamplingY)
{
	m_pRasterizerInstance->SetSubsampling(nSubsamplingX, nSubsamplingY);
}

void CRasterizer::GetSubsampling(LibMCDriver_Rasterizer_uint32 & nSubsamplingX, LibMCDriver_Rasterizer_uint32 & nSubsamplingY)
{
	m_pRasterizerInstance->GetSubsampling(nSubsamplingX, nSubsamplingY);
}

void CRasterizer::AddLayer(ILayerObject* pLayerObject)
{
	m_pRasterizerInstance->AddLayer(pLayerObject);
}

void CRasterizer::CalculateImage(LibMCEnv::PImageData pImageObject, const bool bAntialiased)
{
	m_pRasterizerInstance->CalculateImage(pImageObject.get(), bAntialiased, m_nUnitsPerSubPixel, m_nPixelsPerBlock);
}

void CRasterizer::SetSamplingParameters(const LibMCDriver_Rasterizer_uint32 nUnitsPerSubpixel, const LibMCDriver_Rasterizer_uint32 nPixelsPerBlock)
{
	if ((nUnitsPerSubpixel < RASTERALGORITHM_MINUNITSPERSUBPIXEL) || (nUnitsPerSubpixel > RASTERALGORITHM_MAXUNITSPERSUBPIXEL) || ((nUnitsPerSubpixel % 2) != 0))
		throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_INVALIDUNITSPERSUBPIXEL);

	if ((nPixelsPerBlock < RASTERALGORITHM_MINPIXELSPERBLOCK) || (nPixelsPerBlock > RASTERALGORITHM_MAXPIXELSPERBLOCK))
		throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_INVALIDPIXELSPERBLOCK);

	m_nUnitsPerSubPixel = nUnitsPerSubpixel;
	m_nPixelsPerBlock = nPixelsPerBlock;
}

void CRasterizer::GetSamplingParameters(LibMCDriver_Rasterizer_uint32& nUnitsPerSubpixel, LibMCDriver_Rasterizer_uint32& nPixelsPerBlock)
{
	nUnitsPerSubpixel = m_nUnitsPerSubPixel;
	nPixelsPerBlock = m_nPixelsPerBlock;
}

