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


Abstract: This is a stub class definition of CJPEGImageData

*/

#include "libmcenv_jpegimagedata.hpp"
#include "libmcenv_interfaceexception.hpp"

// Include custom headers here.


using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CJPEGImageData 
**************************************************************************************************************************/

CJPEGImageData::CJPEGImageData(uint32_t nPixelSizeX, uint32_t nPixelSizeY)
    : m_nPixelSizeX(nPixelSizeX), m_nPixelSizeY(nPixelSizeY)
{

}

CJPEGImageData::~CJPEGImageData()
{

}

void CJPEGImageData::GetSizeInPixels(LibMCEnv_uint32& nPixelSizeX, LibMCEnv_uint32& nPixelSizeY)
{
    nPixelSizeX = m_nPixelSizeX;
    nPixelSizeY = m_nPixelSizeY;
}

void CJPEGImageData::GetJPEGDataStream(LibMCEnv_uint64 nJPEGDataBufferSize, LibMCEnv_uint64* pJPEGDataNeededCount, LibMCEnv_uint8* pJPEGDataBuffer) 
{
    uint64_t nJPEGStreamSize = m_JPEGStream.size();

    if (pJPEGDataNeededCount != nullptr)
        *pJPEGDataNeededCount = nJPEGStreamSize;

    if (pJPEGDataBuffer != nullptr) {
        if (nJPEGStreamSize > 0) {
            if (nJPEGDataBufferSize < nJPEGStreamSize)
                throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_BUFFERTOOSMALL);

            uint8_t* pSource = m_JPEGStream.data();
            uint8_t* pTarget = pJPEGDataBuffer;
            for (uint64_t nIndex = 0; nIndex < nJPEGStreamSize; nIndex++) {
                *pTarget = *pSource;
                pSource++;
                pTarget++;
            }
        }
    }
}

std::vector<uint8_t>& CJPEGImageData::getJPEGStreamBuffer()
{
    return m_JPEGStream;
}



void CJPEGImageData::WriteToStream(ITempStreamWriter* pStream)
{
    if (pStream == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

    if (m_JPEGStream.size() > 0)
        pStream->WriteData(m_JPEGStream.size(), m_JPEGStream.data());
}