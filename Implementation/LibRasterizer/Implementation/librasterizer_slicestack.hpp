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


Abstract: This is the class declaration of CSliceStack

*/


#ifndef __LIBRASTERIZER_SLICESTACK
#define __LIBRASTERIZER_SLICESTACK

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
 Class declaration of CSliceStack 
**************************************************************************************************************************/

class CSliceStack : public virtual ISliceStack, public virtual CBase {
private:

    std::vector<PLayerDataObject> m_Layers;
    double m_dLayerThickness;
    double m_dBottomZ;

protected:


public:

    CSliceStack(uint32_t nLayerCount, double dLayerThickness, double dBottomZ);

    virtual ~CSliceStack();
	
	LibRasterizer_uint32 GetLayerCount() override;

	LibRasterizer_double GetLayerThickness() override;

	LibRasterizer_double GetBottomZ() override;

	LibRasterizer_double GetTopZ() override;

	ILayerObject * GetLayer(const LibRasterizer_uint32 nLayerIndex) override;

};

} // namespace Impl
} // namespace LibRasterizer

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBRASTERIZER_SLICESTACK