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

#include "libmcdriver_framebuffer_framebufferinstance.hpp"
#include "libmcdriver_framebuffer_interfaceexception.hpp"

using namespace LibMCDriver_FrameBuffer::Impl;

CFrameBufferInstance::CFrameBufferInstance(const std::string& sIdentifier)
    : m_sIdentifier(sIdentifier),
    m_nScreenWidth(0),
    m_nScreenHeight(0),
    m_BitDepth(LibMCDriver_FrameBuffer::eFrameBufferBitDepth::Unknown),
    m_pDrawbufferPtr(nullptr),
    m_nLineLength (0)

{

}

CFrameBufferInstance::~CFrameBufferInstance()
{

}


LibMCDriver_FrameBuffer_uint32 CFrameBufferInstance::getScreenWidth()
{
    return m_nScreenWidth;
}

LibMCDriver_FrameBuffer_uint32 CFrameBufferInstance::getScreenHeight()
{
    return m_nScreenHeight;

}

LibMCDriver_FrameBuffer::eFrameBufferBitDepth CFrameBufferInstance::bitDepth()
{
    return m_BitDepth;

}


void CFrameBufferInstance::setPixel(const LibMCDriver_FrameBuffer_int32 nX, const LibMCDriver_FrameBuffer_int32 nY, const LibMCDriver_FrameBuffer::sColor RGBColor)
{
    if ((nX >= 0) && (nY >= 0)) {
        uint32_t nPositiveX = (uint32_t)nX;
        uint32_t nPositiveY = (uint32_t)nY;

        if ((nPositiveX < m_nScreenWidth) && (nPositiveY < m_nScreenHeight)) {
            switch (m_BitDepth) {
                case LibMCDriver_FrameBuffer::eFrameBufferBitDepth::RGB565: {

                    uint32_t nRed = RGBColor.m_Red;
                    uint32_t nGreen = RGBColor.m_Green;
                    uint32_t nBlue = RGBColor.m_Blue;

                    uint16_t rawColor = ((nBlue & 0xF8) << 8) | ((nGreen & 0xFC) << 3) | (nRed >> 3);

                    uint16_t* pPixelPtr = (uint16_t*)(m_pDrawbufferPtr + (uint64_t)m_nLineLength * ((uint64_t)nPositiveY)) + nPositiveX;
                    *pPixelPtr = rawColor;

                    break;
                }

                case LibMCDriver_FrameBuffer::eFrameBufferBitDepth::RGB888: {
                    uint8_t* pPixelPtr = m_pDrawbufferPtr + (uint64_t)m_nLineLength * ((uint64_t)nPositiveY) + (uint64_t)nPositiveX * 3;
                    *pPixelPtr = RGBColor.m_Red;
                    pPixelPtr++;
                    *pPixelPtr = RGBColor.m_Green;
                    pPixelPtr++;
                    *pPixelPtr = RGBColor.m_Blue;
                    break;
                }

                case LibMCDriver_FrameBuffer::eFrameBufferBitDepth::RGBA8888: {
                    uint8_t* pPixelPtr = m_pDrawbufferPtr + (uint64_t)m_nLineLength * ((uint64_t)nPositiveY) + (uint64_t)nPositiveX * 4;
                    *pPixelPtr = RGBColor.m_Red;
                    pPixelPtr++;
                    *pPixelPtr = RGBColor.m_Green;
                    pPixelPtr++;
                    *pPixelPtr = RGBColor.m_Blue;
                    pPixelPtr++;
                    *pPixelPtr = 255;
                    break;
                }

            }
        }
    }
}

void CFrameBufferInstance::clearScreen(const LibMCDriver_FrameBuffer::sColor RGBColor)
{
    fillRectangle(0, 0, m_nScreenWidth - 1, m_nScreenHeight - 1, RGBColor);
}


void CFrameBufferInstance::fillRectangle(const LibMCDriver_FrameBuffer_int32 nX1, const LibMCDriver_FrameBuffer_int32 nY1, const LibMCDriver_FrameBuffer_int32 nX2, const LibMCDriver_FrameBuffer_int32 nY2, const LibMCDriver_FrameBuffer::sColor RGBColor)
{

    int32_t nMinX;
    int32_t nMaxX;
    int32_t nMinY;
    int32_t nMaxY;

    // Determine the minimum and maximum X coordinates of the rectangle
    if (nX1 < nX2) {
        nMinX = nX1;
        nMaxX = nX2;
    }
    else {
        nMinX = nX2;
        nMaxX = nX1;
    }

    // Determine the minimum and maximum Y coordinates of the rectangle
    if (nY1 < nY2) {
        nMinY = nY1;
        nMaxY = nY2;
    }
    else {
        nMinY = nY2;
        nMaxY = nY1;
    }

    // Early exit if the rectangle is completely outside the framebuffer
    if (nMinX >= (int32_t)m_nScreenWidth)
        return;
    if (nMinY >= (int32_t)m_nScreenHeight)
        return;
    if (nMaxX < 0)
        return;
    if (nMaxY < 0)
        return;

    // Clamp the rectangle to the framebuffer bounds
    if (nMinX < 0)
        nMinX = 0;
    if (nMinY < 0)
        nMinY = 0;

    if (nMaxX >= (int32_t)m_nScreenWidth)
        nMaxX = (int32_t)m_nScreenWidth - 1;
    if (nMaxY >= (int32_t)m_nScreenHeight)
        nMaxY = (int32_t)m_nScreenHeight - 1;

    // Calculate the width and height of the rectangle to fill
    uint32_t nCountX = (uint32_t) ((nMaxX - nMinX) + 1);
    uint32_t nCountY = (uint32_t) ((nMaxY - nMinY) + 1);

    // Fill the rectangle based on the framebuffer's bit depth
    switch (m_BitDepth) {
    case LibMCDriver_FrameBuffer::eFrameBufferBitDepth::RGB565: {

        // Convert the RGB color to RGB565 format
        uint32_t nRed = RGBColor.m_Red;
        uint32_t nGreen = RGBColor.m_Green;
        uint32_t nBlue = RGBColor.m_Blue;

        uint16_t rawColor = ((nBlue & 0xF8) << 8) | ((nGreen & 0xFC) << 3) | (nRed >> 3);

        // Fill the rectangle row by row
        for (uint32_t nY = 0; nY < nCountY; nY++) {

            uint16_t* pPixelPtr = (uint16_t*)(m_pDrawbufferPtr + (uint64_t)m_nLineLength * ((uint64_t)nY + nMinY)) + nMinX;

            for (uint32_t nX = 0; nX < nCountX; nX++) {
                *pPixelPtr = rawColor;
                pPixelPtr++;
            }
        }

        break;
    }

    case LibMCDriver_FrameBuffer::eFrameBufferBitDepth::RGB888: {

        // Fill the rectangle row by row with 24-bit RGB values
        for (uint32_t nY = 0; nY < nCountY; nY++) {

            uint8_t* pPixelPtr = m_pDrawbufferPtr + (uint64_t)m_nLineLength * ((uint64_t)nY + nMinY) + (uint64_t) nMinX * 3;

            for (uint32_t nX = 0; nX < nCountX; nX++) {
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

        // Fill the rectangle row by row with 32-bit RGBA values

        for (uint32_t nY = 0; nY < nCountY; nY++) {

            uint8_t* pPixelPtr = m_pDrawbufferPtr + (uint64_t)m_nLineLength * ((uint64_t)nY + nMinY) + (uint64_t)nMinX * 4;

            for (uint32_t nX = 0; nX < nCountX; nX++) {
                *pPixelPtr = RGBColor.m_Red;
                pPixelPtr++;
                *pPixelPtr = RGBColor.m_Green;
                pPixelPtr++;
                *pPixelPtr = RGBColor.m_Blue;
                pPixelPtr++;
                *pPixelPtr = 255; // Hardcoded alpha value (fully opaque)
                pPixelPtr++;
            }
        }

        break;
    }

    default:
        throw ELibMCDriver_FrameBufferInterfaceException(LIBMCDRIVER_FRAMEBUFFER_ERROR_INVALIDPIXELFORMAT);

    }



}

void CFrameBufferInstance::drawImage(const LibMCDriver_FrameBuffer_int32 nX, const LibMCDriver_FrameBuffer_int32 nY, LibMCEnv::PImageData pImage)
{
    // Check if the image pointer is valid
    if (pImage.get() == nullptr)
        throw ELibMCDriver_FrameBufferInterfaceException(LIBMCDRIVER_FRAMEBUFFER_ERROR_INVALIDPARAM);

    // Get the dimensions of the image in pixels
    uint32_t nImageSizeX, nImageSizeY;
    pImage->GetSizeInPixels(nImageSizeX, nImageSizeY);

    // Ensure the image has content, otherwise return without drawing
    if ((nImageSizeX == 0) || (nImageSizeY == 0))
        return;

    // Check if the entire image is outside of the framebuffer bounds and return early if so
    if ((int64_t)nX <= (-(int64_t)nImageSizeX))
        return;
    if ((int64_t)nY <= (-(int64_t)nImageSizeY))
        return;
    if ((int64_t)nX >=  ((int64_t)m_nScreenWidth))
        return;
    if ((int64_t)nY >= ((int64_t)m_nScreenHeight))
        return;

    // Initialize variables for determining the section of the image to be drawn and its position in the framebuffer
    uint32_t nFrameBufferCoordStartX, nFrameBufferCoordStartY;
    uint32_t nImageSectionStartX, nImageSectionStartY;
    uint32_t nImageSectionCountX, nImageSectionCountY;

    // Handle cases where the image starts outside the framebuffer horizontally
    if (nX >= 0) {
        nFrameBufferCoordStartX = (uint32_t) nX; // Start drawing at the specified X coordinate in the framebuffer
        nImageSectionStartX = 0; // Start at the beginning of the image
        nImageSectionCountX = nImageSizeX; // Draw the full width of the image
    }
    else {
        nFrameBufferCoordStartX = 0; // Start drawing at the left edge of the framebuffer
        nImageSectionStartX = (uint32_t)-nX; // Skip part of the image that is outside the framebuffer
        nImageSectionCountX = nImageSizeX - nImageSectionStartX; // Adjust the width to only include the visible part
    }

    // Handle cases where the image starts outside the framebuffer vertically
    if (nY >= 0) {
        nFrameBufferCoordStartY = (uint32_t)nY; // Start drawing at the specified Y coordinate in the framebuffer
        nImageSectionStartY = 0; // Start at the top of the image
        nImageSectionCountY = nImageSizeY; // Draw the full height of the image
    }
    else {
        nFrameBufferCoordStartY = 0; // Start drawing at the top edge of the framebuffer
        nImageSectionStartY = (uint32_t)-nY; // Skip part of the image that is outside the framebuffer
        nImageSectionCountY = nImageSizeY - nImageSectionStartY; // Adjust the height to only include the visible part
    }

    // Adjust the section of the image to ensure it fits within the framebuffer horizontally
    if ((nFrameBufferCoordStartX + nImageSectionCountX) > m_nScreenWidth)
        nImageSectionCountX = m_nScreenWidth - nFrameBufferCoordStartX;

    // Adjust the section of the image to ensure it fits within the framebuffer vertically
    if ((nFrameBufferCoordStartY + nImageSectionCountY) > m_nScreenHeight)
        nImageSectionCountY = m_nScreenHeight - nFrameBufferCoordStartY;

    // Determine the pixel format and bytes per pixel based on the framebuffer's bit depth
    LibMCEnv::eImagePixelFormat imagePixelFormat;
    size_t nBytesPerPixel;
    switch (m_BitDepth) {
        case LibMCDriver_FrameBuffer::eFrameBufferBitDepth::RGB565:
            imagePixelFormat = LibMCEnv::eImagePixelFormat::RGB16bit; 
            nBytesPerPixel = 2;
            break;
            
        case LibMCDriver_FrameBuffer::eFrameBufferBitDepth::RGB888:
            imagePixelFormat = LibMCEnv::eImagePixelFormat::RGB24bit; 
            nBytesPerPixel = 3;

            break;
        case LibMCDriver_FrameBuffer::eFrameBufferBitDepth::RGBA8888:
            nBytesPerPixel = 4;
            imagePixelFormat = LibMCEnv::eImagePixelFormat::RGBA32bit; 
            break;

        default:
            throw ELibMCDriver_FrameBufferInterfaceException(LIBMCDRIVER_FRAMEBUFFER_ERROR_INVALIDPIXELFORMAT);
    }

    // Calculate the starting position in the framebuffer where the image will be drawn
    uint8_t* pTargetPtr = m_pDrawbufferPtr + (size_t)m_nLineLength * nFrameBufferCoordStartY + nFrameBufferCoordStartX * nBytesPerPixel;

    // Write the relevant section of the image directly to the framebuffer memory
    pImage->WriteToRawMemory(nImageSectionStartX, nImageSectionStartY, nImageSectionCountX, nImageSectionCountY, imagePixelFormat, (LibMCEnv_pvoid)pTargetPtr, m_nLineLength);

}

void CFrameBufferInstance::setScreenResolution(uint32_t nScreenWidth, uint32_t nScreenHeight, LibMCDriver_FrameBuffer::eFrameBufferBitDepth bitDepth)
{
    if ((nScreenWidth < FRAMEBUFFER_MINSCREENSIZE) || (nScreenWidth > FRAMEBUFFER_MAXSCREENSIZE) ||
        (nScreenHeight < FRAMEBUFFER_MINSCREENSIZE) || (nScreenHeight > FRAMEBUFFER_MAXSCREENSIZE)) {

        throw ELibMCDriver_FrameBufferInterfaceException(LIBMCDRIVER_FRAMEBUFFER_ERROR_INVALIDSCREENSIZE, "invalid screen size: " + std::to_string (nScreenWidth) + "x" + std::to_string (nScreenHeight));
    }

    m_nScreenWidth = nScreenWidth;
    m_nScreenHeight = nScreenHeight;
    m_BitDepth = bitDepth;
}

void CFrameBufferInstance::setDrawBuffer(uint8_t* pDrawBuffer, uint32_t nLineLength)
{
    if (pDrawBuffer == nullptr)
        throw ELibMCDriver_FrameBufferInterfaceException(LIBMCDRIVER_FRAMEBUFFER_ERROR_INVALIDDRAWBUFFER);
    if (nLineLength < FRAMEBUFFER_MINSCREENSIZE)
        throw ELibMCDriver_FrameBufferInterfaceException(LIBMCDRIVER_FRAMEBUFFER_ERROR_INVALIDLINELENGTH);

    m_pDrawbufferPtr = pDrawBuffer;
    m_nLineLength = nLineLength;
}

