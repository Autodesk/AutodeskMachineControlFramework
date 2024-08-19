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
:   CFrameBufferInstance (sIdentifier),
    m_nFBDeviceHandle (-1),
    m_nCurrentBufferIndex (0),
    m_bDoubleBufferingEnabled(false),
    m_nScanLineLength (0),
    m_pFramebufferPtr (nullptr)
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

    LibMCDriver_FrameBuffer::eFrameBufferBitDepth bitDepth;

    switch (vinfo.bits_per_pixel) {
    case 16:
        bitDepth = LibMCDriver_FrameBuffer::eFrameBufferBitDepth::RGB565;
        break;
    case 24:
        bitDepth = LibMCDriver_FrameBuffer::eFrameBufferBitDepth::RGB888;
        break;
    case 32:
        bitDepth = LibMCDriver_FrameBuffer::eFrameBufferBitDepth::RGBA8888;
        break;
    default:
        throw ELibMCDriver_FrameBufferInterfaceException(LIBMCDRIVER_FRAMEBUFFER_ERROR_UNKNOWNFRAMEBUFFERBITDEPTH);
    }


    uint32_t nScreenWidth = (uint32_t)vinfo.xres;
    uint32_t nScreenHeight = (uint32_t)vinfo.yres;
    m_nVirtualResolutionY = (uint32_t)vinfo.yres_virtual;

    setScreenResolution(nScreenWidth, nScreenHeight, bitDepth);

    m_nScanLineLength = (uint32_t)finfo.line_length;
    m_nMemorySize = m_nVirtualResolutionY * m_nScanLineLength;

    m_bDoubleBufferingEnabled = ((uint64_t) m_nVirtualResolutionY >= (2 * (uint64_t)nScreenHeight));

    m_pFramebufferPtr = (uint8_t*) mmap(0, m_nMemorySize, PROT_READ | PROT_WRITE, MAP_SHARED, m_nFBDeviceHandle, 0);
    if (((intptr_t)m_pFramebufferPtr == -1) || (m_pFramebufferPtr == nullptr)) {
        m_pFramebufferPtr = nullptr;
        close(m_nFBDeviceHandle);
        m_nFBDeviceHandle = -1;
        throw ELibMCDriver_FrameBufferInterfaceException(LIBMCDRIVER_FRAMEBUFFER_ERROR_COULDNOTMAPFRAMEBUFFERMEMORY);
    }
    
    setDrawBuffer(m_pFramebufferPtr, m_nScanLineLength);

    LibMCDriver_FrameBuffer::sColor black;
    black.m_Red = 0;
    black.m_Green = 0;
    black.m_Blue = 0;
    clearScreen(black);

    vinfo.yoffset = 0;
    ioctl(m_nFBDeviceHandle, FBIOPAN_DISPLAY, &vinfo);

    if (m_bDoubleBufferingEnabled) {
        m_nCurrentBufferIndex = 1;
        setDrawBuffer(m_pFramebufferPtr + ((uint64_t)m_nScanLineLength * nScreenHeight), m_nScanLineLength);
    }

#else
    throw ELibMCDriver_FrameBufferInterfaceException(LIBMCDRIVER_FRAMEBUFFER_ERROR_DEVICENOTSUPPORTEDONPLATFORM);
#endif
}

CFrameBufferDeviceInstance::~CFrameBufferDeviceInstance()
{
#ifdef __linux__
    if (m_pFramebufferPtr != nullptr) {
        munmap(m_pFramebufferPtr, m_nMemorySize);
        m_pFramebufferPtr = nullptr;
    }

    if (m_nFBDeviceHandle != -1) {
        close(m_nFBDeviceHandle);
        m_nFBDeviceHandle = -1;
    }
#endif
}


void CFrameBufferDeviceInstance::flip()
{

    if (m_bDoubleBufferingEnabled) {
#ifdef __linux__
        fb_var_screeninfo vinfo;
        if (ioctl(m_nFBDeviceHandle, FBIOGET_VSCREENINFO, &vinfo)) 
            throw ELibMCDriver_FrameBufferInterfaceException(LIBMCDRIVER_FRAMEBUFFER_ERROR_COULDNOTGETVARIABLESCREENINFO);
        
        uint32_t nScreenHeight = getScreenHeight();
        vinfo.yoffset = m_nCurrentBufferIndex * nScreenHeight;
        ioctl(m_nFBDeviceHandle, FBIOPAN_DISPLAY, &vinfo);

        if (m_nCurrentBufferIndex == 0) {
            m_nCurrentBufferIndex = 1;
            setDrawBuffer(m_pFramebufferPtr + ((uint64_t)m_nScanLineLength * nScreenHeight), m_nScanLineLength);
        }
        else {
            m_nCurrentBufferIndex = 0;
            setDrawBuffer(m_pFramebufferPtr, m_nScanLineLength);

        }

#endif
    }
    else {
        setDrawBuffer(m_pFramebufferPtr, m_nScanLineLength);
    }

}

bool CFrameBufferDeviceInstance::usesDoubleBuffering()
{
    return m_bDoubleBufferingEnabled;
}
