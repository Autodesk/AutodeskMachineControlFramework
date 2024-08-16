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

using namespace LibMCDriver_Framebuffer::Impl;

/*************************************************************************************************************************
 Class definition of CFrameBufferAccess 
**************************************************************************************************************************/

CFrameBufferDeviceInstance::CFrameBufferDeviceInstance(const std::string& sIdentifier, const std::string& sDevicePath)
:   m_sIdentifier (sIdentifier),
    m_nScreenWidth (0), 
    m_nScreenHeight (0),  
    m_BitDepth (LibMCDriver_Framebuffer::eFrameBufferBitDepth::Unknown), 
    m_bDoubleBufferingEnabled (false),
    m_nFBDeviceHandle (-1),
    m_pFramebufferPtr (nullptr)
{
#ifdef __linux__

    m_nFBDeviceHandle = open(sDevicePath.c_str (), O_RDWR);

    if (m_nFBDeviceHandle == -1) 
        throw ELibMCDriver_FramebufferInterfaceException(LIBMCDRIVER_FRAMEBUFFER_ERROR_COULDNOTOPENFRAMEBUFFERDEVICE, "could not open framebuffer device: " + sDevicePath);

    fb_var_screeninfo vinfo;
    fb_fix_screeninfo finfo;

    if (ioctl(m_nFBDeviceHandle, FBIOGET_FSCREENINFO, &finfo)) {
        close(m_nFBDeviceHandle);
        m_nFBDeviceHandle = -1;
        throw ELibMCDriver_FramebufferInterfaceException(LIBMCDRIVER_FRAMEBUFFER_ERROR_COULDNOTGETFIXEDSCREENINFO);
    }

    if (ioctl(m_nFBDeviceHandle, FBIOGET_VSCREENINFO, &vinfo)) {
        close(m_nFBDeviceHandle);
        m_nFBDeviceHandle = -1;
        throw ELibMCDriver_FramebufferInterfaceException(LIBMCDRIVER_FRAMEBUFFER_ERROR_COULDNOTGETVARIABLESCREENINFO);
    }

    switch (vinfo.bits_per_pixel) {
    case 16:
        m_BitDepth = LibMCDriver_Framebuffer::eFrameBufferBitDepth::RGB565;
        break;
    case 24:
        m_BitDepth = LibMCDriver_Framebuffer::eFrameBufferBitDepth::RGB888;
        break;
    case 32:
        m_BitDepth = LibMCDriver_Framebuffer::eFrameBufferBitDepth::RGBA8888;
        break;
    default:
        throw ELibMCDriver_FramebufferInterfaceException(LIBMCDRIVER_FRAMEBUFFER_ERROR_UNKNOWNFRAMEBUFFERBITDEPTH);
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
        throw ELibMCDriver_FramebufferInterfaceException(LIBMCDRIVER_FRAMEBUFFER_ERROR_COULDNOTMAPFRAMEBUFFERMEMORY);

    }


#else
    throw ELibMCDriver_FramebufferInterfaceException(LIBMCDRIVER_FRAMEBUFFER_ERROR_DEVICENOTSUPPORTEDONPLATFORM);
#endif
}

CFrameBufferDeviceInstance::~CFrameBufferDeviceInstance()
{

}

LibMCDriver_Framebuffer_uint32 CFrameBufferDeviceInstance::getScreenWidth()
{
    return m_nScreenWidth;
}

LibMCDriver_Framebuffer_uint32 CFrameBufferDeviceInstance::getScreenHeight()
{
    return m_nScreenHeight;

}

LibMCDriver_Framebuffer::eFrameBufferBitDepth CFrameBufferDeviceInstance::bitDepth()
{
    return m_BitDepth;

}

bool CFrameBufferDeviceInstance::usesDoubleBuffering()
{
    return m_bDoubleBufferingEnabled;
}

void CFrameBufferDeviceInstance::flip()
{

}

void CFrameBufferDeviceInstance::clearScreen(const LibMCDriver_Framebuffer::sColor RGBColor)
{

}

void CFrameBufferDeviceInstance::drawLine(const LibMCDriver_Framebuffer_uint32 nX1, const LibMCDriver_Framebuffer_uint32 nY1, const LibMCDriver_Framebuffer_uint32 nX2, const LibMCDriver_Framebuffer_uint32 nY2, const LibMCDriver_Framebuffer_double dThickness, const LibMCDriver_Framebuffer::sColor RGBColor)
{

}

void CFrameBufferDeviceInstance::fillRectangle(const LibMCDriver_Framebuffer_uint32 nX1, const LibMCDriver_Framebuffer_uint32 nY1, const LibMCDriver_Framebuffer_uint32 nX2, const LibMCDriver_Framebuffer_uint32 nY2, const LibMCDriver_Framebuffer::sColor RGBColor)
{

}

void CFrameBufferDeviceInstance::drawImage(const LibMCDriver_Framebuffer_uint32 nX, const LibMCDriver_Framebuffer_uint32 nY, LibMCEnv::PImageData pImage)
{

}

