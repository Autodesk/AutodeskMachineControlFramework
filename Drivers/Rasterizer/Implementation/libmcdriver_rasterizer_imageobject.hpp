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


Abstract: This is the class declaration of CImageObject

*/


#ifndef __LIBMCDRIVER_RASTERIZER_IMAGEOBJECT
#define __LIBMCDRIVER_RASTERIZER_IMAGEOBJECT

#include "libmcdriver_rasterizer_interfaces.hpp"
#include "libmcdriver_rasterizer_layerobject.hpp"

// Parent classes
#include "libmcdriver_rasterizer_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.


namespace LibMCDriver_Rasterizer {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CImageObject 
**************************************************************************************************************************/

class CImageObject {
private:


protected:

	uint32_t m_nPixelCountX;
	uint32_t m_nPixelCountY;
	double m_dDPIValueX;
	double m_dDPIValueY;
	double m_dPositionX;
	double m_dPositionY;

	std::vector <uint8_t> m_PixelData;

	// Data for rasterization algorithm
	std::vector<PRasterizationAlgorithm> m_Algorithms;
	uint32_t m_nBlockCountX;
	uint32_t m_nBlockCountY;
	uint32_t m_nUnitsPerSubPixel;
	uint32_t m_nSubSamplingX;
	uint32_t m_nSubSamplingY;
	uint32_t m_nPixelsPerBlock;
	double m_dUnitsX;
	double m_dUnitsY;


public:

	CImageObject(const uint32_t nPixelCountX, const uint32_t nPixelCountY, const double dDPIValueX, const double dDPIValueY);

	virtual ~CImageObject();	

	void drawLayerObject(CLayerDataObject* pLayerDataObject, uint8_t nValue);

	void drawLayerEntity(CLayerDataEntity* pLayerEntity, uint8_t nValue);

	void drawLine(double dX1, double dY1, double dX2, double dY2, uint8_t nValue);

	void setPosition(double dPositionX, double dPositionY);

	void clear(uint8_t nValue);

	void setPixel(const LibMCDriver_Rasterizer_uint32 nX, const LibMCDriver_Rasterizer_uint32 nY, const LibMCDriver_Rasterizer_uint8 nValue);

	std::vector<uint8_t> & getBuffer();

	void initRasterizationAlgorithms(uint32_t nUnitsPerSubPixel, uint32_t nPixelsPerBlock, uint32_t nSubSamplingX, uint32_t nSubSamplingY);

	void addRasterizationLayer(CLayerDataObject * pLayer);

	void calculateRasterizationImage(bool bAntiAliased);

};

} // namespace Impl
} // namespace LibMCDriver_Rasterizer

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_RASTERIZER_IMAGEOBJECT
