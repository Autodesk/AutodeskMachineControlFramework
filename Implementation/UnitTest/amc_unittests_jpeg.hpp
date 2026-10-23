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

#ifndef __AMCTEST_UNITTEST_JPEG
#define __AMCTEST_UNITTEST_JPEG


#include "amc_unittests.hpp"
#include "common_jpeg.hpp"


namespace AMCUnitTest {

	class CUnitTestGroup_JPEG : public CUnitTestGroup {
	public:

		std::string getTestGroupName() override {
			return "JPEG";
		}

		void registerTests() override {
			registerTest("EncodeDecodeRGB", "Encode RGB data to JPEG and decode metadata and buffers", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_JPEG::testEncodeDecodeRGB, this));
			registerTest("EncodeGrayUnsupported", "Grayscale encode should fail for unsupported channel count", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_JPEG::testEncodeGrayUnsupported, this));
			registerTest("DecoderInvalidInput", "Decoder rejects invalid buffers", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_JPEG::testDecoderInvalidInput, this));
			registerTest("EncoderInvalidInput", "Encoder rejects invalid image input", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_JPEG::testEncoderInvalidInput, this));
		}

		void initializeTests() override {
		}

	private:

		void testEncodeDecodeRGB()
		{
			const uint32_t nWidth = 2;
			const uint32_t nHeight = 2;
			std::vector<uint8_t> imageData = {
				255, 0, 0,
				0, 255, 0,
				0, 0, 255,
				255, 255, 0
			};

			std::vector<uint8_t> jpegData;
			AMCCommon::CJPEGImageEncoder encoder(nWidth, nHeight, AMCCommon::eJPEGChannelCount::ccRGB, imageData.data(), jpegData, true);
			assertTrue(encoder.getWidth() == nWidth);
			assertTrue(encoder.getHeight() == nHeight);
			assertTrue(encoder.getChannelCount() == AMCCommon::eJPEGChannelCount::ccRGB);
			assertTrue(!jpegData.empty());

			AMCCommon::CJPEGImageDecoder decoder(jpegData.data(), jpegData.size());
			assertTrue(decoder.getWidth() == nWidth);
			assertTrue(decoder.getHeight() == nHeight);
			assertTrue(decoder.getChannelCount() == AMCCommon::eJPEGChannelCount::ccRGB);

			std::vector<uint8_t> rgbBuffer;
			decoder.writeToBufferRGB24bit(rgbBuffer);
			assertTrue(rgbBuffer.size() == (size_t)nWidth * (size_t)nHeight * 3);

			std::vector<uint8_t> grayBuffer;
			decoder.writeToBufferGreyScale8bit(grayBuffer);
			assertTrue(grayBuffer.size() == (size_t)nWidth * (size_t)nHeight);

			std::vector<uint8_t> rgbaBuffer;
			decoder.writeToBufferRGBA32bit(rgbaBuffer);
			assertTrue(rgbaBuffer.size() == (size_t)nWidth * (size_t)nHeight * 4);
		}

		void testEncodeGrayUnsupported()
		{
			const uint32_t nWidth = 2;
			const uint32_t nHeight = 2;
			std::vector<uint8_t> imageData = {
				0,
				64,
				128,
				255
			};

			bool thrown = false;
			try {
				std::vector<uint8_t> jpegData;
				AMCCommon::CJPEGImageEncoder encoder(nWidth, nHeight, AMCCommon::eJPEGChannelCount::ccGray, imageData.data(), jpegData, true);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected grayscale JPEG encode to throw on unsupported channel count");
		}

		void testDecoderInvalidInput()
		{
			bool thrown = false;
			try {
				uint8_t data = 0;
				AMCCommon::CJPEGImageDecoder decoder(&data, 0);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected decoder to throw on empty buffer size");

			thrown = false;
			try {
				AMCCommon::CJPEGImageDecoder decoder(nullptr, 1);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected decoder to throw on null buffer");

			thrown = false;
			try {
				std::vector<uint8_t> invalidData = { 0x00, 0x01, 0x02, 0x03 };
				AMCCommon::CJPEGImageDecoder decoder(invalidData.data(), invalidData.size());
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected decoder to throw on invalid JPEG data");
		}

		void testEncoderInvalidInput()
		{
			bool thrown = false;
			try {
				std::vector<uint8_t> jpegData;
				AMCCommon::CJPEGImageEncoder encoder(1, 1, AMCCommon::eJPEGChannelCount::ccRGB, nullptr, jpegData, true);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected encoder to throw on null image data");

			thrown = false;
			try {
				std::vector<uint8_t> imageData = { 0, 0, 0 };
				std::vector<uint8_t> jpegData;
				AMCCommon::CJPEGImageEncoder encoder(0, 1, AMCCommon::eJPEGChannelCount::ccRGB, imageData.data(), jpegData, true);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected encoder to throw on invalid image size");
		}
	};

}

#endif // __AMCTEST_UNITTEST_JPEG
