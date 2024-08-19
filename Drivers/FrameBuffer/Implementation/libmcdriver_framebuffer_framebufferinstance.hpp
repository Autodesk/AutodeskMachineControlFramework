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


Abstract: This is the class declaration of CFrameBufferAccess

*/


#ifndef __LIBMCDRIVER_FRAMEBUFFER_FRAMEBUFFERINSTANCE
#define __LIBMCDRIVER_FRAMEBUFFER_FRAMEBUFFERINSTANCE

#include "libmcdriver_framebuffer_interfaces.hpp"

#define FRAMEBUFFER_MINSCREENSIZE 128UL
#define FRAMEBUFFER_MAXSCREENSIZE 16384UL


namespace LibMCDriver_FrameBuffer {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CFrameBufferAccess 
**************************************************************************************************************************/

class CFrameBufferInstance {
private:

	std::string m_sIdentifier;

	uint32_t m_nScreenWidth;
	uint32_t m_nScreenHeight;
	uint8_t* m_pDrawbufferPtr;
	uint32_t m_nLineLength;

	LibMCDriver_FrameBuffer::eFrameBufferBitDepth m_BitDepth;

protected:

	void setScreenResolution (uint32_t nScreenWidth, uint32_t nScreenHeight, LibMCDriver_FrameBuffer::eFrameBufferBitDepth bitDepth);
	void setDrawBuffer (uint8_t* pDrawBuffer, uint32_t nLineLength);

public:

	CFrameBufferInstance (const std::string & sIdentifier);
	
	virtual ~CFrameBufferInstance ();

	LibMCDriver_FrameBuffer_uint32 getScreenWidth();

	LibMCDriver_FrameBuffer_uint32 getScreenHeight();

	LibMCDriver_FrameBuffer::eFrameBufferBitDepth bitDepth();

	virtual bool usesDoubleBuffering() = 0;

	virtual void flip() = 0;

	void clearScreen(const LibMCDriver_FrameBuffer::sColor RGBColor);

	void setPixel(const LibMCDriver_FrameBuffer_int32 nX, const LibMCDriver_FrameBuffer_int32 nY, const LibMCDriver_FrameBuffer::sColor RGBColor);

	void fillRectangle(const LibMCDriver_FrameBuffer_int32 nX1, const LibMCDriver_FrameBuffer_int32 nY1, const LibMCDriver_FrameBuffer_int32 nX2, const LibMCDriver_FrameBuffer_int32 nY2, const LibMCDriver_FrameBuffer::sColor RGBColor);

	void drawImage(const LibMCDriver_FrameBuffer_int32 nX, const LibMCDriver_FrameBuffer_int32 nY, LibMCEnv::PImageData pImage);

};

typedef std::shared_ptr<CFrameBufferInstance> PFrameBufferInstance;

} // namespace Impl
} // namespace LibMCDriver_FrameBuffer

#endif // __LIBMCDRIVER_FRAMEBUFFER_FRAMEBUFFERINSTANCE
