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


Abstract: This is a stub class definition of CPNGImageData

*/

#include "libmcenv_pngimagedata.hpp" 
#include "libmcenv_interfaceexception.hpp"


using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CPNGImageData 
**************************************************************************************************************************/

CPNGImageData::CPNGImageData(uint32_t nPixelSizeX, uint32_t nPixelSizeY)
    : m_nPixelSizeX (nPixelSizeX), m_nPixelSizeY (nPixelSizeY)
{

}

CPNGImageData::~CPNGImageData()
{

}

void CPNGImageData::GetSizeInPixels(LibMCEnv_uint32 & nPixelSizeX, LibMCEnv_uint32 & nPixelSizeY)
{
    nPixelSizeX = m_nPixelSizeX;
    nPixelSizeY = m_nPixelSizeY;
}

void CPNGImageData::GetPNGDataStream(LibMCEnv_uint64 nPNGDataBufferSize, LibMCEnv_uint64* pPNGDataNeededCount, LibMCEnv_uint8 * pPNGDataBuffer)
{
    uint64_t nPNGStreamSize = m_PNGStream.size();
    
    if (pPNGDataNeededCount != nullptr)
        *pPNGDataNeededCount = nPNGStreamSize;

    if (pPNGDataBuffer != nullptr) {
        if (nPNGStreamSize > 0) {
            if (nPNGDataBufferSize < nPNGStreamSize)
                throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_BUFFERTOOSMALL);

            uint8_t* pSource = m_PNGStream.data ();
            uint8_t* pTarget = pPNGDataBuffer;
            for (uint64_t nIndex = 0; nIndex < nPNGStreamSize; nIndex++) {
                *pTarget = *pSource;
                pSource++;
                pTarget++;
            }
        }
    }
}

std::vector<uint8_t>& CPNGImageData::getPNGStreamBuffer()
{
    return m_PNGStream;
}

void CPNGImageData::WriteToStream(ITempStreamWriter* pStream)
{
    if (pStream == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

    if (m_PNGStream.size() > 0)
        pStream->WriteData(m_PNGStream.size(), m_PNGStream.data());
}