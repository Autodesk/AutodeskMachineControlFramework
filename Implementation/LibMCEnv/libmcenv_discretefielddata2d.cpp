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


Abstract: This is a stub class definition of CDiscreteFieldData2D

*/

#include "libmcenv_discretefielddata2d.hpp"
#include "libmcenv_imagedata.hpp"
#include "libmcenv_interfaceexception.hpp"

// Include custom headers here.


using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CDiscreteFieldData2D 
**************************************************************************************************************************/
CDiscreteFieldData2D::CDiscreteFieldData2D(AMC::PDiscreteFieldData2DInstance pDiscreteFieldDataInstance)
	: m_pDiscreteFieldDataInstance (pDiscreteFieldDataInstance)
{
	if (pDiscreteFieldDataInstance.get () == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

}

CDiscreteFieldData2D::~CDiscreteFieldData2D()
{

}

AMC::PDiscreteFieldData2DInstance CDiscreteFieldData2D::getInstance()
{
	return m_pDiscreteFieldDataInstance;
}


void CDiscreteFieldData2D::GetDPI(LibMCEnv_double & dDPIValueX, LibMCEnv_double & dDPIValueY)
{
	m_pDiscreteFieldDataInstance->GetDPI(dDPIValueX, dDPIValueY);
}

void CDiscreteFieldData2D::SetDPI(const LibMCEnv_double dDPIValueX, const LibMCEnv_double dDPIValueY)
{
	m_pDiscreteFieldDataInstance->SetDPI(dDPIValueX, dDPIValueY);
}

void CDiscreteFieldData2D::GetOriginInMM(LibMCEnv_double & dOriginX, LibMCEnv_double & dOriginY)
{
	m_pDiscreteFieldDataInstance->GetOriginInMM(dOriginX, dOriginY);
}

void CDiscreteFieldData2D::SetOriginInMM(const LibMCEnv_double dOriginX, const LibMCEnv_double dOriginY)
{
	m_pDiscreteFieldDataInstance->SetOriginInMM(dOriginX, dOriginY);
}

void CDiscreteFieldData2D::GetSizeInMM(LibMCEnv_double & dSizeX, LibMCEnv_double & dSizeY)
{
	m_pDiscreteFieldDataInstance->GetSizeInMM(dSizeX, dSizeY);
}

void CDiscreteFieldData2D::GetSizeInPixels(LibMCEnv_uint32 & nPixelSizeX, LibMCEnv_uint32 & nPixelSizeY)
{
	m_pDiscreteFieldDataInstance->GetSizeInPixels(nPixelSizeX, nPixelSizeY);
}

void CDiscreteFieldData2D::ResizeField(LibMCEnv_uint32 nPixelSizeX, LibMCEnv_uint32 nPixelSizeY, const LibMCEnv_double dDefaultValue)
{
	m_pDiscreteFieldDataInstance->ResizeField(nPixelSizeX, nPixelSizeY, dDefaultValue);
}

void CDiscreteFieldData2D::Clear(const LibMCEnv_double dValue)
{
	m_pDiscreteFieldDataInstance->Clear(dValue);
}

LibMCEnv_double CDiscreteFieldData2D::GetPixel(const LibMCEnv_uint32 nX, const LibMCEnv_uint32 nY)
{
	return m_pDiscreteFieldDataInstance->GetPixel(nX, nY);
}

void CDiscreteFieldData2D::SetPixel(const LibMCEnv_uint32 nX, const LibMCEnv_uint32 nY, const LibMCEnv_double dValue)
{
	m_pDiscreteFieldDataInstance->SetPixel(nX, nY, dValue);
}

void CDiscreteFieldData2D::GetPixelRange(const LibMCEnv_uint32 nXMin, const LibMCEnv_uint32 nYMin, const LibMCEnv_uint32 nXMax, const LibMCEnv_uint32 nYMax, LibMCEnv_uint64 nValueBufferSize, LibMCEnv_uint64* pValueNeededCount, LibMCEnv_double * pValueBuffer)
{
	m_pDiscreteFieldDataInstance->GetPixelRange(nXMin, nYMin, nXMax, nYMax, nValueBufferSize, pValueNeededCount, pValueBuffer);
}

void CDiscreteFieldData2D::SetPixelRange(const LibMCEnv_uint32 nXMin, const LibMCEnv_uint32 nYMin, const LibMCEnv_uint32 nXMax, const LibMCEnv_uint32 nYMax, const LibMCEnv_uint64 nValueBufferSize, const LibMCEnv_double * pValueBuffer)
{
	m_pDiscreteFieldDataInstance->SetPixelRange(nXMin, nYMin, nXMax, nYMax, nValueBufferSize, pValueBuffer);
}

void CDiscreteFieldData2D::RenderAveragePointValues(const LibMCEnv_double dDefaultValue, const LibMCEnv::eFieldSamplingMode eSamplingMode, const LibMCEnv_double dSampleSizeX, const LibMCEnv_double dSampleSizeY, const LibMCEnv_uint64 nPointValuesBufferSize, const LibMCEnv::sFieldData2DPoint * pPointValuesBuffer)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

IDiscreteFieldData2D * CDiscreteFieldData2D::ScaleFieldDown(const LibMCEnv_uint32 nFactorX, const LibMCEnv_uint32 nFactorY)
{
	auto pNewField = m_pDiscreteFieldDataInstance->ScaleFieldDown(nFactorX, nFactorY);
	return new CDiscreteFieldData2D(pNewField);
}

IDiscreteFieldData2D * CDiscreteFieldData2D::ScaleFieldUp(const LibMCEnv_uint32 nFactorX, const LibMCEnv_uint32 nFactorY)
{
	auto pNewField = m_pDiscreteFieldDataInstance->ScaleFieldUp(nFactorX, nFactorY);
	return new CDiscreteFieldData2D(pNewField);
}

void CDiscreteFieldData2D::Discretize(const LibMCEnv_uint64 nDiscreteValuesBufferSize, const LibMCEnv_double * pDiscreteValuesBuffer)
{
	m_pDiscreteFieldDataInstance->DiscretizeWithMapping(nDiscreteValuesBufferSize, pDiscreteValuesBuffer, nDiscreteValuesBufferSize, pDiscreteValuesBuffer);
}

void CDiscreteFieldData2D::DiscretizeWithMapping(const LibMCEnv_uint64 nDiscreteValuesBufferSize, const LibMCEnv_double * pDiscreteValuesBuffer, const LibMCEnv_uint64 nNewValuesBufferSize, const LibMCEnv_double * pNewValuesBuffer)
{
	m_pDiscreteFieldDataInstance->DiscretizeWithMapping(nDiscreteValuesBufferSize, pDiscreteValuesBuffer, nNewValuesBufferSize, pNewValuesBuffer);
}

IImageData * CDiscreteFieldData2D::RenderToImageRaw(const LibMCEnv_double dMinValue, const LibMCEnv::sColorRGB MinColor, const LibMCEnv_double dMidValue, const LibMCEnv::sColorRGB MidColor, const LibMCEnv_double dMaxValue, const LibMCEnv::sColorRGB MaxColor)
{
	auto pixelData = std::make_unique<std::vector<uint8_t>>();

	uint32_t nPixelCountX, nPixelCountY;
	m_pDiscreteFieldDataInstance->GetSizeInPixels(nPixelCountX, nPixelCountY);

	double dDPIValueX, dDPIValueY;
	m_pDiscreteFieldDataInstance->GetDPI(dDPIValueX, dDPIValueY);

	m_pDiscreteFieldDataInstance->renderRGBImage(pixelData.get(), dMinValue, MinColor.m_Red, MinColor.m_Green, MinColor.m_Blue, dMidValue, MidColor.m_Red, MidColor.m_Green, MidColor.m_Blue, dMaxValue, MaxColor.m_Red, MaxColor.m_Green, MaxColor.m_Blue);

	return new CImageData(pixelData.release(), nPixelCountX, nPixelCountY, dDPIValueX, dDPIValueY, LibMCEnv::eImagePixelFormat::RGB24bit, false);

}

void CDiscreteFieldData2D::TransformField(const LibMCEnv_double dScale, const LibMCEnv_double dOffset)
{
	m_pDiscreteFieldDataInstance->TransformField(dScale, dOffset);
}

void CDiscreteFieldData2D::AddField(IDiscreteFieldData2D* pOtherField, const LibMCEnv_double dScale, const LibMCEnv_double dOffset)
{
	if (pOtherField == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

	auto pOtherFieldInstance = dynamic_cast<CDiscreteFieldData2D*> (pOtherField);
	if (pOtherFieldInstance == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDCAST);

	m_pDiscreteFieldDataInstance->AddField(pOtherFieldInstance->getInstance().get(), dScale, dOffset);
}

IDiscreteFieldData2D * CDiscreteFieldData2D::Duplicate()
{
	auto pNewField = m_pDiscreteFieldDataInstance->Duplicate();
	return new CDiscreteFieldData2D(pNewField);
}

