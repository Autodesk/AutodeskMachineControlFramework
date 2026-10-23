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

#ifndef __AMCTEST_UNITTEST_DISCRETEFIELDDATA2D
#define __AMCTEST_UNITTEST_DISCRETEFIELDDATA2D


#include "amc_unittests.hpp"
#include "amc_discretefielddata2d.hpp"

#include <cmath>


namespace AMCUnitTest {

	class CUnitTestGroup_DiscreteFieldData2D : public CUnitTestGroup {
	public:

		std::string getTestGroupName() override {
			return "DiscreteFieldData2D";
		}

		void registerTests() override {
			registerTest("ConstructionAndBasics", "Discrete field construction and basic properties", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_DiscreteFieldData2D::testConstructionAndBasics, this));
			registerTest("ResizeClampAndPixels", "Discrete field resizing, clamping, and pixel access", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_DiscreteFieldData2D::testResizeClampAndPixels, this));
			registerTest("ScalingAndTransform", "Discrete field scaling and transform operations", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_DiscreteFieldData2D::testScalingAndTransform, this));
			registerTest("DuplicateAndAdd", "Discrete field duplication and add operations", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_DiscreteFieldData2D::testDuplicateAndAdd, this));
			registerTest("RenderAndSampling", "Discrete field rendering and sampling operations", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_DiscreteFieldData2D::testRenderAndSampling, this));
			registerTest("SerializationAndLoad", "Discrete field serialization and raw pixel loading", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_DiscreteFieldData2D::testSerializationAndLoad, this));
			registerTest("NotImplementedPaths", "Discrete field not implemented paths throw", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_DiscreteFieldData2D::testNotImplementedPaths, this));
		}

		void initializeTests() override {
		}

	private:

		bool nearlyEqual(const double a, const double b, const double epsilon = 1.0e-9)
		{
			return std::abs(a - b) < epsilon;
		}

		void testConstructionAndBasics()
		{
			bool thrown = false;
			try {
				AMC::CDiscreteFieldData2DInstance invalidField(0, 1, 1.0, 1.0, 0.0, 0.0, 0.0, true);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected constructor to reject zero pixel count");

			thrown = false;
			try {
				AMC::CDiscreteFieldData2DInstance invalidField(1, 1, 0.0, 1.0, 0.0, 0.0, 0.0, true);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected constructor to reject invalid DPI");

			thrown = false;
			try {
				AMC::CDiscreteFieldData2DInstance invalidField(1, 1, 1.0, 1.0, 1.0e10, 0.0, 0.0, true);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected constructor to reject origin out of range");

			AMC::CDiscreteFieldData2DInstance field(2, 3, 25.4, 25.4, 1.0, 2.0, 7.0, true);
			double dpiX = 0.0, dpiY = 0.0;
			field.GetDPI(dpiX, dpiY);
			assertTrue(nearlyEqual(dpiX, 25.4));
			assertTrue(nearlyEqual(dpiY, 25.4));

			double originX = 0.0, originY = 0.0;
			field.GetOriginInMM(originX, originY);
			assertTrue(nearlyEqual(originX, 1.0));
			assertTrue(nearlyEqual(originY, 2.0));

			double sizeX = 0.0, sizeY = 0.0;
			field.GetSizeInMM(sizeX, sizeY);
			assertTrue(nearlyEqual(sizeX, 2.0));
			assertTrue(nearlyEqual(sizeY, 3.0));

			uint32_t sizeXPix = 0, sizeYPix = 0;
			field.GetSizeInPixels(sizeXPix, sizeYPix);
			assertTrue(sizeXPix == 2);
			assertTrue(sizeYPix == 3);

			assertTrue(nearlyEqual(field.GetPixel(0, 0), 7.0));

			field.SetDPI(50.0, 100.0);
			field.GetDPI(dpiX, dpiY);
			assertTrue(nearlyEqual(dpiX, 50.0));
			assertTrue(nearlyEqual(dpiY, 100.0));

			field.SetOriginInMM(-1.0, -2.0);
			field.GetOriginInMM(originX, originY);
			assertTrue(nearlyEqual(originX, -1.0));
			assertTrue(nearlyEqual(originY, -2.0));

			thrown = false;
			try {
				field.SetDPI(0.0, 1.0);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected SetDPI to reject invalid values");
		}

		void testResizeClampAndPixels()
		{
			AMC::CDiscreteFieldData2DInstance field(2, 2, 25.4, 25.4, 0.0, 0.0, 0.0, true);
			field.SetPixel(0, 0, 1.0);
			field.SetPixel(1, 0, 2.0);
			field.SetPixel(0, 1, 3.0);
			field.SetPixel(1, 1, 4.0);

			assertTrue(nearlyEqual(field.GetPixel(1, 1), 4.0));

			bool thrown = false;
			try {
				field.GetPixel(2, 0);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected GetPixel to reject invalid X coordinate");

			thrown = false;
			try {
				field.SetPixel(0, 3, 1.0);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected SetPixel to reject invalid Y coordinate");

			thrown = false;
			try {
				field.Clamp(1.0, 1.0);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected Clamp to reject invalid range");

			field.Clamp(0.0, 3.0);
			assertTrue(nearlyEqual(field.GetPixel(1, 1), 3.0));

			uint32_t newX = 3;
			uint32_t newY = 1;
			field.ResizeField(newX, newY, 5.0);
			uint32_t sizeX = 0, sizeY = 0;
			field.GetSizeInPixels(sizeX, sizeY);
			assertTrue(sizeX == 3);
			assertTrue(sizeY == 1);
			assertTrue(nearlyEqual(field.GetPixel(0, 0), 1.0));
			assertTrue(nearlyEqual(field.GetPixel(1, 0), 2.0));
			assertTrue(nearlyEqual(field.GetPixel(2, 0), 5.0));
		}

		void testScalingAndTransform()
		{
			AMC::CDiscreteFieldData2DInstance field(2, 2, 25.4, 25.4, 0.0, 0.0, 0.0, true);
			field.SetPixel(0, 0, 1.0);
			field.SetPixel(1, 0, 2.0);
			field.SetPixel(0, 1, 3.0);
			field.SetPixel(1, 1, 4.0);

			bool thrown = false;
			try {
				field.ScaleFieldDown(0, 1);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected ScaleFieldDown to reject invalid factors");

			auto scaledDown = field.ScaleFieldDown(2, 2);
			uint32_t sizeX = 0, sizeY = 0;
			scaledDown->GetSizeInPixels(sizeX, sizeY);
			assertTrue(sizeX == 1);
			assertTrue(sizeY == 1);
			assertTrue(nearlyEqual(scaledDown->GetPixel(0, 0), 2.5));

			thrown = false;
			try {
				field.ScaleFieldUp(0, 1);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected ScaleFieldUp to reject invalid factors");

			auto scaledUp = field.ScaleFieldUp(2, 2);
			scaledUp->GetSizeInPixels(sizeX, sizeY);
			assertTrue(sizeX == 4);
			assertTrue(sizeY == 4);
			assertTrue(nearlyEqual(scaledUp->GetPixel(0, 0), 1.0));
			assertTrue(nearlyEqual(scaledUp->GetPixel(1, 0), 1.0));
			assertTrue(nearlyEqual(scaledUp->GetPixel(2, 0), 2.0));
			assertTrue(nearlyEqual(scaledUp->GetPixel(3, 0), 2.0));
			assertTrue(nearlyEqual(scaledUp->GetPixel(0, 1), 1.0));
			assertTrue(nearlyEqual(scaledUp->GetPixel(2, 1), 2.0));
			assertTrue(nearlyEqual(scaledUp->GetPixel(0, 2), 3.0));
			assertTrue(nearlyEqual(scaledUp->GetPixel(2, 2), 4.0));

			field.TransformField(2.0, 1.0);
			assertTrue(nearlyEqual(field.GetPixel(0, 0), 3.0));
			assertTrue(nearlyEqual(field.GetPixel(1, 1), 9.0));
		}

		void testDuplicateAndAdd()
		{
			AMC::CDiscreteFieldData2DInstance field(2, 2, 25.4, 25.4, 0.0, 0.0, 0.0, true);
			field.SetPixel(0, 0, 1.0);
			field.SetPixel(1, 0, 2.0);
			field.SetPixel(0, 1, 3.0);
			field.SetPixel(1, 1, 4.0);

			auto duplicate = field.Duplicate();
			assertTrue(duplicate.get() != nullptr);
			assertTrue(duplicate.get() != &field);
			assertTrue(nearlyEqual(duplicate->GetPixel(1, 1), 4.0));

			bool thrown = false;
			try {
				field.AddField(nullptr, 1.0, 0.0);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected AddField to reject null input");

			AMC::CDiscreteFieldData2DInstance other(3, 2, 25.4, 25.4, 0.0, 0.0, 0.0, true);
			thrown = false;
			try {
				field.AddField(&other, 1.0, 0.0);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected AddField to reject mismatched sizes");

			AMC::CDiscreteFieldData2DInstance same(2, 2, 25.4, 25.4, 0.0, 0.0, 0.0, true);
			same.SetPixel(0, 0, 10.0);
			field.AddField(&same, 0.5, 1.0);
			assertTrue(nearlyEqual(field.GetPixel(0, 0), 1.0 + 10.0 * 0.5 + 1.0));
		}

		void testRenderAndSampling()
		{
			AMC::CDiscreteFieldData2DInstance field(3, 1, 25.4, 25.4, 0.0, 0.0, 0.0, true);
			field.SetPixel(0, 0, 0.0);
			field.SetPixel(1, 0, 0.5);
			field.SetPixel(2, 0, 1.0);

			bool thrown = false;
			try {
				field.renderRGBImage(nullptr, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 2.0, 1.0, 1.0, 1.0);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected renderRGBImage to reject null output buffer");

			std::vector<uint8_t> pixels;
			field.renderRGBImage(&pixels, 0.0, 0.0, 0.0, 0.0, 0.5, 0.5, 0.5, 0.5, 1.0, 1.0, 1.0, 1.0);
			assertTrue(pixels.size() == 9);
			assertTrue(pixels.at(0) == 0);
			assertTrue(pixels.at(1) == 0);
			assertTrue(pixels.at(2) == 0);
			assertTrue(pixels.at(3) == 128);
			assertTrue(pixels.at(4) == 128);
			assertTrue(pixels.at(5) == 128);
			assertTrue(pixels.at(6) == 255);
			assertTrue(pixels.at(7) == 255);
			assertTrue(pixels.at(8) == 255);

			thrown = false;
			try {
				field.renderRGBImage(&pixels, 1.0, 0.0, 0.0, 0.0, 0.5, 0.5, 0.5, 0.5, 0.0, 1.0, 1.0, 1.0);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected renderRGBImage to reject invalid color range");

			AMC::CDiscreteFieldData2DInstance samplingField(2, 2, 25.4, 25.4, 0.0, 0.0, 0.0, true);
			LibMCEnv::sFieldData2DPoint points[3];
			points[0].m_Coordinates[0] = 0.2;
			points[0].m_Coordinates[1] = 0.2;
			points[0].m_Value = 2.0;
			points[1].m_Coordinates[0] = 0.8;
			points[1].m_Coordinates[1] = 0.8;
			points[1].m_Value = 4.0;
			points[2].m_Coordinates[0] = 1.2;
			points[2].m_Coordinates[1] = 0.2;
			points[2].m_Value = 6.0;
			samplingField.renderAveragePointValues_FloorSampling(-1.0, 3, points);
			assertTrue(nearlyEqual(samplingField.GetPixel(0, 0), 3.0));
			assertTrue(nearlyEqual(samplingField.GetPixel(1, 0), 6.0));
			assertTrue(nearlyEqual(samplingField.GetPixel(0, 1), -1.0));
			assertTrue(nearlyEqual(samplingField.GetPixel(1, 1), -1.0));

			samplingField.SetPixel(0, 0, 9.0);
			samplingField.renderAveragePointValues_FloorSampling(7.0, 0, nullptr);
			assertTrue(nearlyEqual(samplingField.GetPixel(0, 0), 7.0));

			thrown = false;
			try {
				samplingField.renderAveragePointValues_FloorSampling(0.0, 1, nullptr);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected renderAveragePointValues_FloorSampling to reject null input");
		}

		void testSerializationAndLoad()
		{
			AMC::CDiscreteFieldData2DInstance field(2, 2, 10.0, 20.0, 1.0, 2.0, 0.0, true);
			field.SetPixel(0, 0, 1.0);
			field.SetPixel(1, 0, 2.0);
			field.SetPixel(0, 1, 3.0);
			field.SetPixel(1, 1, 4.0);

			std::vector<uint8_t> buffer;
			field.saveToBuffer(buffer);
			auto loaded = AMC::CDiscreteFieldData2DInstance::createFromBuffer(buffer);
			uint32_t sizeX = 0, sizeY = 0;
			loaded->GetSizeInPixels(sizeX, sizeY);
			assertTrue(sizeX == 2);
			assertTrue(sizeY == 2);

			double dpiX = 0.0, dpiY = 0.0;
			loaded->GetDPI(dpiX, dpiY);
			assertTrue(nearlyEqual(dpiX, 10.0));
			assertTrue(nearlyEqual(dpiY, 20.0));

			double originX = 0.0, originY = 0.0;
			loaded->GetOriginInMM(originX, originY);
			assertTrue(nearlyEqual(originX, 1.0));
			assertTrue(nearlyEqual(originY, 2.0));
			assertTrue(nearlyEqual(loaded->GetPixel(1, 1), 4.0));

			bool thrown = false;
			try {
				std::vector<uint8_t> tiny;
				AMC::CDiscreteFieldData2DInstance::createFromBuffer(tiny);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected createFromBuffer to reject empty buffer");

			thrown = false;
			try {
				std::vector<uint8_t> corrupted = buffer;
				if (corrupted.size() > 0)
					corrupted[0] = 0;
				AMC::CDiscreteFieldData2DInstance::createFromBuffer(corrupted);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected createFromBuffer to reject invalid signature");

			AMC::CDiscreteFieldData2DInstance pixelField(2, 2, 25.4, 25.4, 0.0, 0.0, 0.0, true);
			std::vector<uint8_t> grayData = { 0, 255, 128, 64 };
			pixelField.loadFromRawPixelData(grayData, LibMCEnv::eImagePixelFormat::GreyScale8bit, 0.0, 1.0);
			assertTrue(nearlyEqual(pixelField.GetPixel(0, 0), 0.0));
			assertTrue(nearlyEqual(pixelField.GetPixel(1, 0), 1.0));

			std::vector<uint8_t> rgbData = {
				0, 0, 0,
				255, 255, 255,
				255, 0, 0,
				0, 255, 0
			};
			pixelField.loadFromRawPixelData(rgbData, LibMCEnv::eImagePixelFormat::RGB24bit, 0.0, 1.0);
			assertTrue(nearlyEqual(pixelField.GetPixel(0, 0), 0.0));
			assertTrue(nearlyEqual(pixelField.GetPixel(1, 0), 1.0));

			std::vector<uint8_t> rgbaData = {
				0, 0, 0, 255,
				255, 255, 255, 255,
				255, 0, 0, 255,
				0, 255, 0, 255
			};
			pixelField.loadFromRawPixelData(rgbaData, LibMCEnv::eImagePixelFormat::RGBA32bit, 0.0, 1.0);
			assertTrue(nearlyEqual(pixelField.GetPixel(0, 0), 0.0));
			assertTrue(nearlyEqual(pixelField.GetPixel(1, 0), 1.0));

			thrown = false;
			try {
				pixelField.loadFromRawPixelData(grayData, LibMCEnv::eImagePixelFormat::RGB24bit, 0.0, 1.0);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected loadFromRawPixelData to reject mismatched buffer size");

			thrown = false;
			try {
				pixelField.loadFromRawPixelData(grayData, LibMCEnv::eImagePixelFormat::Unknown, 0.0, 1.0);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected loadFromRawPixelData to reject unknown pixel format");
		}

		void testNotImplementedPaths()
		{
			AMC::CDiscreteFieldData2DInstance field(2, 2, 25.4, 25.4, 0.0, 0.0, 0.0, true);
			bool thrown = false;
			try {
				field.GetPixelRange(0, 0, 1, 1, 0, nullptr, nullptr);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected GetPixelRange to throw not implemented");

			thrown = false;
			try {
				field.SetPixelRange(0, 0, 1, 1, 0, nullptr);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected SetPixelRange to throw not implemented");

			thrown = false;
			try {
				field.DiscretizeWithMapping(0, nullptr, 0, nullptr);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected DiscretizeWithMapping to throw not implemented");
		}
	};

}

#endif // __AMCTEST_UNITTEST_DISCRETEFIELDDATA2D
