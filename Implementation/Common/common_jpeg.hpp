/*++

Copyright (C) 2025 Autodesk Inc.

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

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL AUTODESK INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/


#ifndef __AMC_JPEG
#define __AMC_JPEG

#include <cstdint>
#include <memory>
#include <vector>


namespace AMCCommon {

	enum class eJPEGChannelCount : uint32_t
	{
		ccInvalid = 0,
		ccGray = 1,
		ccGrayAlpha = 2,
		ccRGB = 3,
		ccRGBAlpha = 4
	};

	class CJPEGImageDecoder {
	private:
		uint32_t m_nWidth;
		uint32_t m_nHeight;
		uint8_t* m_pJPEGData;
		eJPEGChannelCount m_ChannelCount;

	public:

		CJPEGImageDecoder(const uint8_t* pBuffer, uint64_t nBufferSize);

		virtual ~CJPEGImageDecoder();

		uint32_t getWidth ();
		uint32_t getHeight ();
		eJPEGChannelCount getChannelCount ();

		void writeToBufferGreyScale8bit (std::vector<uint8_t> & buffer);
		void writeToBufferRGB24bit (std::vector<uint8_t>& buffer);
		void writeToBufferRGBA32bit (std::vector<uint8_t>& buffer);
	};


	class CJPEGImageEncoder {
	private:
		uint32_t m_nWidth;
		uint32_t m_nHeight;
		std::vector<uint8_t> & m_JPEGData;
		eJPEGChannelCount m_ChannelCount;

	public:

		CJPEGImageEncoder(uint32_t nWidth, uint32_t nHeight, eJPEGChannelCount channelCount, const uint8_t * pImageData, std::vector<uint8_t> & JPEGData, bool bThrowError);

		virtual ~CJPEGImageEncoder();

		uint32_t getWidth();
		uint32_t getHeight();
		eJPEGChannelCount getChannelCount();

		std::vector<uint8_t> & getJPEGData();

	};


}

#endif //__AMC_JPEG