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


Abstract: This is the class declaration of CRasterizer

*/


#ifndef __LIBRASTERIZER_RASTERIZER
#define __LIBRASTERIZER_RASTERIZER

#include "librasterizer_interfaces.hpp"
#include "librasterizer_layerobject.hpp"

// Parent classes
#include "librasterizer_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.


namespace LibRasterizer {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CRasterizer 
**************************************************************************************************************************/

#define RASTERER_MINSUBSAMPLING 1
#define RASTERER_MAXSUBSAMPLING 32

/*************************************************************************************************************************
 Class declaration of CRasterer
**************************************************************************************************************************/

	class CRasterizer : public virtual IRasterizer, public virtual CBase {
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
		CRasterizer(const uint32_t nPixelCountX, const uint32_t nPixelCountY, double dDPIX, double dDPIY);
		virtual ~CRasterizer();

		void GetDPI(LibRasterizer_double& dDPIValueX, LibRasterizer_double& dDPIValueY) override;

		void GetSize(LibRasterizer_double& dSizeX, LibRasterizer_double& dSizeY) override;

		void GetPixelSize(LibRasterizer_uint32& nPixelSizeX, LibRasterizer_uint32& nPixelSizeY) override;

		void GetPosition(LibRasterizer_double& dPositionX, LibRasterizer_double& dPositionY) override;

		void SetPosition(const LibRasterizer_double dPositionX, const LibRasterizer_double dPositionY) override;

		void SetSubsampling(const LibRasterizer_uint32 nSubsamplingX, const LibRasterizer_uint32 nSubsamplingY) override;

		void GetSubsampling(LibRasterizer_uint32& nSubsamplingX, LibRasterizer_uint32& nSubsamplingY) override;

		void AddLayer(ILayerObject* pLayerObject) override;

		IImageObject* CalculateImage(const bool bAntialiased) override;

	};


} // namespace Impl
} // namespace LibRasterizer

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBRASTERIZER_RASTERIZER
