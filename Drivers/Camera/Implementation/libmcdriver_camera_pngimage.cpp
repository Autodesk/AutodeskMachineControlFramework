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


Abstract: This is a stub class definition of CPNGImage

*/

#include "libmcdriver_camera_pngimage.hpp"
#include "libmcdriver_camera_interfaceexception.hpp"

// Include custom headers here.


using namespace LibMCDriver_Camera::Impl;
using namespace LibMCDriver_Camera;

/*************************************************************************************************************************
 Class definition of CPNGImage 
**************************************************************************************************************************/
CPNGImage::CPNGImage(uint32_t nWidth, uint32_t nHeight, eImagePixelFormat ePixelFormat)
    : m_nWidth (nWidth), m_nHeight (nHeight), m_ePixelFormat (ePixelFormat)
{
    if ((nWidth == 0) || (nHeight == 0) || (ePixelFormat == eImagePixelFormat::Unknown))
        throw ELibMCDriver_CameraInterfaceException (LIBMCDRIVER_CAMERA_ERROR_INVALIDPARAM);


}


LibMCDriver_Camera_uint64 CPNGImage::GetWidth()
{
    return m_nWidth;
}

LibMCDriver_Camera_uint64 CPNGImage::GetHeight()
{
    return m_nHeight;
}

LibMCDriver_Camera::eImagePixelFormat CPNGImage::GetPixelFormat()
{
    return m_ePixelFormat;
}

void CPNGImage::GetRawData(LibMCDriver_Camera_uint64 nPNGDataBufferSize, LibMCDriver_Camera_uint64* pPNGDataNeededCount, LibMCDriver_Camera_uint8 * pPNGDataBuffer)
{

    size_t nSize = m_BinaryData.size();
    if (pPNGDataNeededCount != nullptr)
        *pPNGDataNeededCount = nSize;

    if (pPNGDataBuffer != nullptr) {
        if (nPNGDataBufferSize < nSize)
            throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_BUFFERTOOSMALL);

        if (nSize > 0) {
            const uint8_t* pSrc = m_BinaryData.data();
            uint8_t* pDst = pPNGDataBuffer;

            for (size_t nIndex = 0; nIndex < nSize; nIndex++) {
                *pDst = *pSrc;
                pSrc++;
                pDst++;
            }
        }
    }
}

std::string CPNGImage::SaveToFile()
{
	throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_NOTIMPLEMENTED);
}

std::vector <uint8_t>& CPNGImage::getBinaryData()
{
    return m_BinaryData;
}
