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


Abstract: This is a stub class definition of CSliceStack

*/

#include "libmcdriver_rasterizer_slicestack.hpp"
#include "libmcdriver_rasterizer_interfaceexception.hpp"

// Include custom headers here.


using namespace LibMCDriver_Rasterizer::Impl;

/*************************************************************************************************************************
 Class definition of CSliceStack 
**************************************************************************************************************************/

CSliceStack::CSliceStack(uint32_t nLayerCount, double dLayerThickness, double dBottomZ)
    : m_dLayerThickness(dLayerThickness), m_dBottomZ(dBottomZ)
{
    if (nLayerCount == 0)
        throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_INVALIDLAYERCOUNT);
    if (dLayerThickness <= 0.0)
        throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_INVALIDLAYERTHICKNESS);

    m_Layers.resize(nLayerCount);
    for (uint32_t nIndex = 0; nIndex < nLayerCount; nIndex++)
        m_Layers[nIndex] = std::make_shared<CLayerDataObject>();
}

CSliceStack::~CSliceStack()
{

}


LibMCDriver_Rasterizer_uint32 CSliceStack::GetLayerCount()
{
    return (uint32_t)m_Layers.size();
}

LibMCDriver_Rasterizer_double CSliceStack::GetLayerThickness()
{
    return m_dLayerThickness;
}

LibMCDriver_Rasterizer_double CSliceStack::GetBottomZ()
{
    return m_dBottomZ;
}

LibMCDriver_Rasterizer_double CSliceStack::GetTopZ()
{
    return m_dBottomZ + m_Layers.size() * m_dLayerThickness;
}

ILayerObject* CSliceStack::GetLayer(const LibMCDriver_Rasterizer_uint32 nLayerIndex)
{
    return new CLayerObject(m_Layers[nLayerIndex]);
}

