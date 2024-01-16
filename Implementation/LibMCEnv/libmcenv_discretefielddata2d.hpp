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


Abstract: This is the class declaration of CDiscreteFieldData2D

*/


#ifndef __LIBMCENV_DISCRETEFIELDDATA2D
#define __LIBMCENV_DISCRETEFIELDDATA2D

#include "libmcenv_interfaces.hpp"

// Parent classes
#include "libmcenv_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "amc_discretefielddata2d.hpp"

namespace LibMCEnv {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CDiscreteFieldData2D 
**************************************************************************************************************************/

class CDiscreteFieldData2D : public virtual IDiscreteFieldData2D, public virtual CBase {
private:

	AMC::PDiscreteFieldData2DInstance m_pDiscreteFieldDataInstance;

public:

	CDiscreteFieldData2D(AMC::PDiscreteFieldData2DInstance pDiscreteFieldDataInstance);

	virtual ~CDiscreteFieldData2D();

	AMC::PDiscreteFieldData2DInstance getInstance();

	void GetDPI(LibMCEnv_double & dDPIValueX, LibMCEnv_double & dDPIValueY) override;

	void SetDPI(const LibMCEnv_double dDPIValueX, const LibMCEnv_double dDPIValueY) override;

	void GetOriginInMM(LibMCEnv_double & dOriginX, LibMCEnv_double & dOriginY) override;

	void SetOriginInMM(const LibMCEnv_double dOriginX, const LibMCEnv_double dOriginY) override;

	void GetSizeInMM(LibMCEnv_double & dSizeX, LibMCEnv_double & dSizeY) override;

	void GetSizeInPixels(LibMCEnv_uint32 & nPixelSizeX, LibMCEnv_uint32 & nPixelSizeY) override;

	void ResizeField(LibMCEnv_uint32 nPixelSizeX, LibMCEnv_uint32 nPixelSizeY, const LibMCEnv_double dDefaultValue) override;

	void Clear(const LibMCEnv_double dValue) override;

	void Clamp(const LibMCEnv_double dMinValue, const LibMCEnv_double dMaxValue) override;

	LibMCEnv_double GetPixel(const LibMCEnv_uint32 nX, const LibMCEnv_uint32 nY) override;

	void SetPixel(const LibMCEnv_uint32 nX, const LibMCEnv_uint32 nY, const LibMCEnv_double dValue) override;

	void GetPixelRange(const LibMCEnv_uint32 nXMin, const LibMCEnv_uint32 nYMin, const LibMCEnv_uint32 nXMax, const LibMCEnv_uint32 nYMax, LibMCEnv_uint64 nValueBufferSize, LibMCEnv_uint64* pValueNeededCount, LibMCEnv_double * pValueBuffer) override;

	void SetPixelRange(const LibMCEnv_uint32 nXMin, const LibMCEnv_uint32 nYMin, const LibMCEnv_uint32 nXMax, const LibMCEnv_uint32 nYMax, const LibMCEnv_uint64 nValueBufferSize, const LibMCEnv_double * pValueBuffer) override;

	void RenderAveragePointValues(const LibMCEnv_double dDefaultValue, const LibMCEnv::eFieldSamplingMode eSamplingMode, const LibMCEnv_double dSampleSizeX, const LibMCEnv_double dSampleSizeY, const LibMCEnv_uint64 nPointValuesBufferSize, const LibMCEnv::sFieldData2DPoint * pPointValuesBuffer) override;

	IDiscreteFieldData2D * ScaleFieldDown(const LibMCEnv_uint32 nFactorX, const LibMCEnv_uint32 nFactorY) override;

	IDiscreteFieldData2D * ScaleFieldUp(const LibMCEnv_uint32 nFactorX, const LibMCEnv_uint32 nFactorY) override;

	void Discretize(const LibMCEnv_uint64 nDiscreteValuesBufferSize, const LibMCEnv_double * pDiscreteValuesBuffer) override;

	void DiscretizeWithMapping(const LibMCEnv_uint64 nDiscreteValuesBufferSize, const LibMCEnv_double * pDiscreteValuesBuffer, const LibMCEnv_uint64 nNewValuesBufferSize, const LibMCEnv_double * pNewValuesBuffer) override;

	IImageData * RenderToImageRaw(const LibMCEnv_double dMinValue, const LibMCEnv::sColorRGB MinColor, const LibMCEnv_double dMidValue, const LibMCEnv::sColorRGB MidColor, const LibMCEnv_double dMaxValue, const LibMCEnv::sColorRGB MaxColor) override;

	void TransformField(const LibMCEnv_double dScale, const LibMCEnv_double dOffset) override;

	void AddField(IDiscreteFieldData2D* pOtherField, const LibMCEnv_double dScale, const LibMCEnv_double dOffset) override;

	IDiscreteFieldData2D * Duplicate() override;

};

} // namespace Impl
} // namespace LibMCEnv

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCENV_DISCRETEFIELDDATA2D
