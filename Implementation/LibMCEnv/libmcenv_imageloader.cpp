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


Abstract: This is a stub class definition of CImageLoader

*/

#include "libmcenv_imageloader.hpp"
#include "libmcenv_interfaceexception.hpp"
#include "libmcenv_imagedata.hpp"

using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CImageLoader 
**************************************************************************************************************************/

CImageLoader::CImageLoader()
{

}

CImageLoader::~CImageLoader()
{

}


IImageData * CImageLoader::LoadPNGImage(const LibMCEnv_uint64 nPNGDataBufferSize, const LibMCEnv_uint8 * pPNGDataBuffer, const LibMCEnv_double dDPIValueX, const LibMCEnv_double dDPIValueY, const LibMCEnv::eImagePixelFormat ePixelFormat)
{
    return CImageData::createFromPNG(pPNGDataBuffer, nPNGDataBufferSize, dDPIValueX, dDPIValueY, ePixelFormat);
}

IImageData * CImageLoader::LoadJPEGImage(const LibMCEnv_uint64 nJPEGDataBufferSize, const LibMCEnv_uint8 * pJPEGDataBuffer, const LibMCEnv_double dDPIValueX, const LibMCEnv_double dDPIValueY, const LibMCEnv::eImagePixelFormat ePixelFormat)
{
    return CImageData::createFromJPEG(pJPEGDataBuffer, nJPEGDataBufferSize, dDPIValueX, dDPIValueY, ePixelFormat);
}

IImageData * CImageLoader::CreateImageFromRawRGB24Data(const LibMCEnv_uint64 nRGB24DataBufferSize, const LibMCEnv_uint8 * pRGB24DataBuffer, const LibMCEnv_uint32 nPixelSizeX, const LibMCEnv_uint32 nPixelSizeY, const LibMCEnv_double dDPIValueX, const LibMCEnv_double dDPIValueY, const LibMCEnv::eImagePixelFormat ePixelFormat)
{
    return CImageData::createFromRGB24(pRGB24DataBuffer, nRGB24DataBufferSize, nPixelSizeX, nPixelSizeY, dDPIValueX, dDPIValueY, ePixelFormat);
}

IImageData * CImageLoader::CreateImageFromRawRGBA32Data(const LibMCEnv_uint64 nRGBA32DataBufferSize, const LibMCEnv_uint8 * pRGBA32DataBuffer, const LibMCEnv_uint32 nPixelSizeX, const LibMCEnv_uint32 nPixelSizeY, const LibMCEnv_double dDPIValueX, const LibMCEnv_double dDPIValueY, const LibMCEnv::eImagePixelFormat ePixelFormat)
{
    return CImageData::createFromRGBA32(pRGBA32DataBuffer, nRGBA32DataBufferSize, nPixelSizeX, nPixelSizeY, dDPIValueX, dDPIValueY, ePixelFormat);
}

IImageData * CImageLoader::CreateImageFromRawYUY2Data(const LibMCEnv_uint64 nYUY2DataBufferSize, const LibMCEnv_uint8 * pYUY2DataBuffer, const LibMCEnv_uint32 nPixelSizeX, const LibMCEnv_uint32 nPixelSizeY, const LibMCEnv_double dDPIValueX, const LibMCEnv_double dDPIValueY, const LibMCEnv::eImagePixelFormat ePixelFormat)
{
    return CImageData::createFromYUY2(pYUY2DataBuffer, nYUY2DataBufferSize, nPixelSizeX, nPixelSizeY, dDPIValueX, dDPIValueY, ePixelFormat);
}

