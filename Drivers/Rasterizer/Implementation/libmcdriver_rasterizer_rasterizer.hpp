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


Abstract: This is the class declaration of CRasterizer

*/


#ifndef __LIBMCDRIVER_RASTERIZER_RASTERIZER
#define __LIBMCDRIVER_RASTERIZER_RASTERIZER

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

#define RASTERER_MINSUBSAMPLING 1
#define RASTERER_MAXSUBSAMPLING 32

/*************************************************************************************************************************
Class declaration of CRasterizerInstance
**************************************************************************************************************************/

class CRasterizerInstance  {
private:
	uint32_t m_nPixelCountX;
	uint32_t m_nPixelCountY;
	double m_dDPIX;
	double m_dDPIY;
	double m_dPositionX;
	double m_dPositionY;
	uint32_t m_nSubSamplingX;
	uint32_t m_nSubSamplingY;

	std::vector<PLayerDataObject> m_Layers;

protected:


public:
	CRasterizerInstance(const uint32_t nPixelCountX, const uint32_t nPixelCountY, double dDPIX, double dDPIY);
	virtual ~CRasterizerInstance();

	void GetDPI(double& dDPIValueX, double& dDPIValueY);

	void GetSize(double& dSizeX, double& dSizeY);

	void GetPixelSize(uint32_t& nPixelSizeX, uint32_t& nPixelSizeY);

	void GetPosition(double& dPositionX, double& dPositionY);

	void SetPosition(const double dPositionX, const double dPositionY);

	void SetSubsampling(const uint32_t nSubsamplingX, const uint32_t nSubsamplingY);

	void GetSubsampling(uint32_t& nSubsamplingX, uint32_t& nSubsamplingY);

	void AddLayer(ILayerObject* pLayerObject);

	void CalculateImage(LibMCEnv::CImageData * pImageData, const bool bAntialiased);

};

typedef std::shared_ptr<CRasterizerInstance> PRasterizerInstance;

/*************************************************************************************************************************
 Class declaration of CRasterizer 
**************************************************************************************************************************/

class CRasterizer : public virtual IRasterizer, public virtual CBase {
private:

protected:
	PRasterizerInstance m_pRasterizerInstance;

public:

	CRasterizer(PRasterizerInstance pRasterizerInstance);

	virtual ~CRasterizer();

	void GetDPI(LibMCDriver_Rasterizer_double & dDPIValueX, LibMCDriver_Rasterizer_double & dDPIValueY) override;

	void GetSize(LibMCDriver_Rasterizer_double & dSizeX, LibMCDriver_Rasterizer_double & dSizeY) override;

	void GetPixelSize(LibMCDriver_Rasterizer_uint32 & nPixelSizeX, LibMCDriver_Rasterizer_uint32 & nPixelSizeY) override;

	void GetPosition(LibMCDriver_Rasterizer_double & dPositionX, LibMCDriver_Rasterizer_double & dPositionY) override;

	void SetPosition(const LibMCDriver_Rasterizer_double dPositionX, const LibMCDriver_Rasterizer_double dPositionY) override;

	void SetSubsampling(const LibMCDriver_Rasterizer_uint32 nSubsamplingX, const LibMCDriver_Rasterizer_uint32 nSubsamplingY) override;

	void GetSubsampling(LibMCDriver_Rasterizer_uint32 & nSubsamplingX, LibMCDriver_Rasterizer_uint32 & nSubsamplingY) override;

	void AddLayer(ILayerObject* pLayerObject) override;

	void CalculateImage(LibMCEnv::PImageData pImageObject, const bool bAntialiased);

};

} // namespace Impl
} // namespace LibMCDriver_Rasterizer

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_RASTERIZER_RASTERIZER
