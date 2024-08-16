/*++

Copyright (C) 2024 Autodesk Inc.

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


Abstract: This is a stub class definition of CFrameBufferAccess

*/

#include "libmcdriver_framebuffer_framebufferdevice.hpp"
#include "libmcdriver_framebuffer_interfaceexception.hpp"

#ifdef __linux__
    #include <linux/fb.h>
    #include <sys/ioctl.h>
    #include <sys/mman.h>
    #include <fcntl.h>
    #include <unistd.h>
#endif

using namespace LibMCDriver_FrameBuffer::Impl;

/*************************************************************************************************************************
 Class definition of CFrameBufferAccess 
**************************************************************************************************************************/

CFrameBufferDeviceInstance::CFrameBufferDeviceInstance(const std::string& sIdentifier, const std::string& sDevicePath)
:   m_sIdentifier (sIdentifier),
    m_nScreenWidth (0), 
    m_nScreenHeight (0),  
    m_BitDepth (LibMCDriver_FrameBuffer::eFrameBufferBitDepth::Unknown), 
    m_bDoubleBufferingEnabled (false),
    m_nFBDeviceHandle (-1),
    m_pFramebufferPtr (nullptr),
    m_pDrawbufferPtr (nullptr),
    m_nCurrentBufferIndex (0)
{
#ifdef __linux__

    m_nFBDeviceHandle = open(sDevicePath.c_str (), O_RDWR);

    if (m_nFBDeviceHandle == -1) 
        throw ELibMCDriver_FrameBufferInterfaceException(LIBMCDRIVER_FRAMEBUFFER_ERROR_COULDNOTOPENFRAMEBUFFERDEVICE, "could not open framebuffer device: " + sDevicePath);

    fb_var_screeninfo vinfo;
    fb_fix_screeninfo finfo;

    if (ioctl(m_nFBDeviceHandle, FBIOGET_FSCREENINFO, &finfo)) {
        close(m_nFBDeviceHandle);
        m_nFBDeviceHandle = -1;
        throw ELibMCDriver_FrameBufferInterfaceException(LIBMCDRIVER_FRAMEBUFFER_ERROR_COULDNOTGETFIXEDSCREENINFO);
    }

    if (ioctl(m_nFBDeviceHandle, FBIOGET_VSCREENINFO, &vinfo)) {
        close(m_nFBDeviceHandle);
        m_nFBDeviceHandle = -1;
        throw ELibMCDriver_FrameBufferInterfaceException(LIBMCDRIVER_FRAMEBUFFER_ERROR_COULDNOTGETVARIABLESCREENINFO);
    }

    switch (vinfo.bits_per_pixel) {
    case 16:
        m_BitDepth = LibMCDriver_FrameBuffer::eFrameBufferBitDepth::RGB565;
        break;
    case 24:
        m_BitDepth = LibMCDriver_FrameBuffer::eFrameBufferBitDepth::RGB888;
        break;
    case 32:
        m_BitDepth = LibMCDriver_FrameBuffer::eFrameBufferBitDepth::RGBA8888;
        break;
    default:
        throw ELibMCDriver_FrameBufferInterfaceException(LIBMCDRIVER_FRAMEBUFFER_ERROR_UNKNOWNFRAMEBUFFERBITDEPTH);
    }


    m_nScreenHeight = (uint32_t)vinfo.yres;
    m_nVirtualResolutionY = (uint32_t)vinfo.yres_virtual;

    m_nLineLength = (uint32_t)finfo.line_length;
    m_nMemorySize = m_nVirtualResolutionY * m_nLineLength;

    m_bDoubleBufferingEnabled = (m_nVirtualResolutionY <= (2 * m_nScreenHeight));

    m_pFramebufferPtr = (uint8_t*) mmap(0, m_nMemorySize, PROT_READ | PROT_WRITE, MAP_SHARED, m_nFBDeviceHandle, 0);
    if (((intptr_t)m_pFramebufferPtr == -1) || (m_pFramebufferPtr == nullptr)) {
        m_pFramebufferPtr = nullptr;
        close(m_nFBDeviceHandle);
        m_nFBDeviceHandle = -1;
        throw ELibMCDriver_FrameBufferInterfaceException(LIBMCDRIVER_FRAMEBUFFER_ERROR_COULDNOTMAPFRAMEBUFFERMEMORY);
    }
    
    m_pDrawbufferPtr = m_pFramebufferPtr;

    LibMCDriver_FrameBuffer::sColor black;
    black.m_Red = 0;
    black.m_Green = 0;
    black.m_Blue = 0;
    clearScreen(black);

    vinfo.yoffset = 0;
    ioctl(m_nFBDeviceHandle, FBIOPAN_DISPLAY, &vinfo);

    m_nCurrentBufferIndex = 0;
    if (m_bDoubleBufferingEnabled) {
        flip();
    }

#else
    throw ELibMCDriver_FrameBufferInterfaceException(LIBMCDRIVER_FRAMEBUFFER_ERROR_DEVICENOTSUPPORTEDONPLATFORM);
#endif
}

CFrameBufferDeviceInstance::~CFrameBufferDeviceInstance()
{

}

LibMCDriver_FrameBuffer_uint32 CFrameBufferDeviceInstance::getScreenWidth()
{
    return m_nScreenWidth;
}

LibMCDriver_FrameBuffer_uint32 CFrameBufferDeviceInstance::getScreenHeight()
{
    return m_nScreenHeight;

}

LibMCDriver_FrameBuffer::eFrameBufferBitDepth CFrameBufferDeviceInstance::bitDepth()
{
    return m_BitDepth;

}

bool CFrameBufferDeviceInstance::usesDoubleBuffering()
{
    return m_bDoubleBufferingEnabled;
}

void CFrameBufferDeviceInstance::flip()
{

    if (m_bDoubleBufferingEnabled) {
#ifdef __linux__
        fb_var_screeninfo vinfo;
        if (ioctl(m_nFBDeviceHandle, FBIOGET_VSCREENINFO, &vinfo)) 
            throw ELibMCDriver_FrameBufferInterfaceException(LIBMCDRIVER_FRAMEBUFFER_ERROR_COULDNOTGETVARIABLESCREENINFO);
        
        vinfo.yoffset = m_nCurrentBufferIndex * m_nScreenHeight;
        ioctl(m_nFBDeviceHandle, FBIOPAN_DISPLAY, &vinfo);

        if (m_nCurrentBufferIndex == 0) {
            m_nCurrentBufferIndex = 1;
            m_pDrawbufferPtr = m_pFramebufferPtr + (m_nLineLength * m_nScreenHeight);
        }
        else {
            m_nCurrentBufferIndex = 0;
            m_pDrawbufferPtr = m_pFramebufferPtr;

        }

#endif
    }
    else {
        m_pDrawbufferPtr = m_pFramebufferPtr;
    }

}

void CFrameBufferDeviceInstance::clearScreen(const LibMCDriver_FrameBuffer::sColor RGBColor)
{
    fillRectangle(0, 0, m_nScreenWidth - 1, m_nScreenHeight - 1, RGBColor);
}

void CFrameBufferDeviceInstance::drawLine(const LibMCDriver_FrameBuffer_uint32 nX1, const LibMCDriver_FrameBuffer_uint32 nY1, const LibMCDriver_FrameBuffer_uint32 nX2, const LibMCDriver_FrameBuffer_uint32 nY2, const LibMCDriver_FrameBuffer_double dThickness, const LibMCDriver_FrameBuffer::sColor RGBColor)
{

}

void CFrameBufferDeviceInstance::fillRectangle(const LibMCDriver_FrameBuffer_uint32 nX1, const LibMCDriver_FrameBuffer_uint32 nY1, const LibMCDriver_FrameBuffer_uint32 nX2, const LibMCDriver_FrameBuffer_uint32 nY2, const LibMCDriver_FrameBuffer::sColor RGBColor)
{

    switch (m_BitDepth) {
    case LibMCDriver_FrameBuffer::eFrameBufferBitDepth::RGB565: {

        uint32_t nRed = RGBColor.m_Red;
        uint32_t nGreen = RGBColor.m_Green;
        uint32_t nBlue = RGBColor.m_Blue;

        uint16_t rawColor = ((nBlue & 0xF8) << 8) | ((nGreen & 0xFC) << 3) | (nRed >> 3);

        for (uint32_t nY = 0; nY < m_nScreenHeight; nY++) {

            uint16_t* pPixelPtr = (uint16_t*)(m_pDrawbufferPtr + (uint64_t)m_nLineLength * nY);

            for (uint32_t nX = 0; nX < m_nScreenWidth; nX++) {
                *pPixelPtr = rawColor;
                pPixelPtr++;
            }
        }

        break;
    }

    case LibMCDriver_FrameBuffer::eFrameBufferBitDepth::RGB888: {
        for (uint32_t nY = 0; nY < m_nScreenHeight; nY++) {

            uint8_t* pPixelPtr = m_pDrawbufferPtr + (uint64_t)m_nLineLength * nY;

            for (uint32_t nX = 0; nX < m_nScreenWidth; nX++) {
                *pPixelPtr = RGBColor.m_Red;
                pPixelPtr++;
                *pPixelPtr = RGBColor.m_Green;
                pPixelPtr++;
                *pPixelPtr = RGBColor.m_Blue;
                pPixelPtr++;
            }
        }

        break;
    }

    case LibMCDriver_FrameBuffer::eFrameBufferBitDepth::RGBA8888: {
        for (uint32_t nY = 0; nY < m_nScreenHeight; nY++) {

            uint8_t* pPixelPtr = m_pDrawbufferPtr + (uint64_t)m_nLineLength * nY;

            for (uint32_t nX = 0; nX < m_nScreenWidth; nX++) {
                *pPixelPtr = RGBColor.m_Red;
                pPixelPtr++;
                *pPixelPtr = RGBColor.m_Green;
                pPixelPtr++;
                *pPixelPtr = RGBColor.m_Blue;
                pPixelPtr++;
                *pPixelPtr = 255;
                pPixelPtr++;
            }
        }

        break;
    }

    }



}

void CFrameBufferDeviceInstance::drawImage(const LibMCDriver_FrameBuffer_uint32 nX, const LibMCDriver_FrameBuffer_uint32 nY, LibMCEnv::PImageData pImage)
{

}

