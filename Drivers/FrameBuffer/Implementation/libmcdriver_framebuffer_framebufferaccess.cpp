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

#include "libmcdriver_framebuffer_framebufferaccess.hpp"
#include "libmcdriver_framebuffer_interfaceexception.hpp"

// Include custom headers here.



using namespace LibMCDriver_FrameBuffer::Impl;

/*************************************************************************************************************************
 Class definition of CFrameBufferAccess 
**************************************************************************************************************************/
CFrameBufferAccess::CFrameBufferAccess(PFrameBufferInstance pInstance)
	: m_pInstance(pInstance)
{
	if (pInstance.get () == nullptr)
		throw ELibMCDriver_FrameBufferInterfaceException(LIBMCDRIVER_FRAMEBUFFER_ERROR_INVALIDPARAM);


}

CFrameBufferAccess::~CFrameBufferAccess()
{

}

LibMCDriver_FrameBuffer_uint32 CFrameBufferAccess::GetScreenWidth()
{
	return m_pInstance->getScreenWidth();
}

LibMCDriver_FrameBuffer_uint32 CFrameBufferAccess::GetScreenHeight()
{
	return m_pInstance->getScreenHeight();
}

LibMCDriver_FrameBuffer::eFrameBufferBitDepth CFrameBufferAccess::BitDepth()
{
	return m_pInstance->bitDepth();
}

bool CFrameBufferAccess::UsesDoubleBuffering()
{
	return m_pInstance->usesDoubleBuffering();
}

void CFrameBufferAccess::Flip()
{
	m_pInstance->flip();
}

void CFrameBufferAccess::ClearScreen(const LibMCDriver_FrameBuffer::sColor RGBColor)
{
	m_pInstance->clearScreen(RGBColor);
}

void CFrameBufferAccess::ClearScreenRGB(const LibMCDriver_FrameBuffer_uint8 nRed, const LibMCDriver_FrameBuffer_uint8 nGreen, const LibMCDriver_FrameBuffer_uint8 nBlue)
{
	LibMCDriver_FrameBuffer::sColor RGBColor;
	RGBColor.m_Red = nRed;
	RGBColor.m_Green = nGreen;
	RGBColor.m_Blue = nBlue;
	m_pInstance->clearScreen(RGBColor);
}

void CFrameBufferAccess::SetPixel(const LibMCDriver_FrameBuffer_int32 nX, const LibMCDriver_FrameBuffer_int32 nY, const LibMCDriver_FrameBuffer::sColor RGBColor) 
{
	m_pInstance->setPixel(nX, nY, RGBColor);
}

void CFrameBufferAccess::SetPixelRGB(const LibMCDriver_FrameBuffer_int32 nX, const LibMCDriver_FrameBuffer_int32 nY, const LibMCDriver_FrameBuffer_uint8 nRed, const LibMCDriver_FrameBuffer_uint8 nGreen, const LibMCDriver_FrameBuffer_uint8 nBlue) 
{
	LibMCDriver_FrameBuffer::sColor RGBColor;
	RGBColor.m_Red = nRed;
	RGBColor.m_Green = nGreen;
	RGBColor.m_Blue = nBlue;
	m_pInstance->setPixel(nX, nY, RGBColor);
}


void CFrameBufferAccess::FillRectangle(const LibMCDriver_FrameBuffer_int32 nX1, const LibMCDriver_FrameBuffer_int32 nY1, const LibMCDriver_FrameBuffer_int32 nX2, const LibMCDriver_FrameBuffer_int32 nY2, const LibMCDriver_FrameBuffer::sColor RGBColor)
{
	m_pInstance->fillRectangle(nX1, nY1, nX2, nY2, RGBColor);
}


void CFrameBufferAccess::FillRectangleRGB(const LibMCDriver_FrameBuffer_int32 nX1, const LibMCDriver_FrameBuffer_int32 nY1, const LibMCDriver_FrameBuffer_int32 nX2, const LibMCDriver_FrameBuffer_int32 nY2, const LibMCDriver_FrameBuffer_uint8 nRed, const LibMCDriver_FrameBuffer_uint8 nGreen, const LibMCDriver_FrameBuffer_uint8 nBlue) 
{
	LibMCDriver_FrameBuffer::sColor RGBColor;
	RGBColor.m_Red = nRed;
	RGBColor.m_Green = nGreen;
	RGBColor.m_Blue = nBlue;
	m_pInstance->fillRectangle(nX1, nY1, nX2, nY2, RGBColor);
}

void CFrameBufferAccess::DrawImage(const LibMCDriver_FrameBuffer_int32 nX, const LibMCDriver_FrameBuffer_int32 nY, LibMCEnv::PImageData pImage) 
{
	m_pInstance->drawImage(nX, nY, pImage);
}
