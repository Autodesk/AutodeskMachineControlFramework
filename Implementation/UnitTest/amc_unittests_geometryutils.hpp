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

#ifndef __AMCTEST_UNITTEST_GEOMETRYUTILS
#define __AMCTEST_UNITTEST_GEOMETRYUTILS


#include "amc_unittests.hpp"
#include "amc_geometryutils.hpp"

#include <cmath>


namespace AMCUnitTest {

	class CUnitTestGroup_GeometryUtils : public CUnitTestGroup {
	public:

		std::string getTestGroupName() override {
			return "GeometryUtils";
		}

		void registerTests() override {
			registerTest("VectorMath", "Vector operations, length, and normalization", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_GeometryUtils::testVectorMath, this));
			registerTest("VectorAngles", "Vector angle calculations", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_GeometryUtils::testVectorAngles, this));
		}

		void initializeTests() override {
		}

	private:

		bool nearlyEqual(const double a, const double b, const double epsilon = 1.0e-9)
		{
			return std::abs(a - b) < epsilon;
		}

		void testVectorMath()
		{
			auto zero = AMC::CVectorUtils::vectorZero();
			assertTrue(nearlyEqual(AMC::CVectorUtils::vectorLength(zero), 0.0));

			auto vecA = AMC::CVectorUtils::vectorMake(1.0, 2.0, 3.0);
			auto vecB = AMC::CVectorUtils::vectorMake(-1.0, 4.0, 0.5);

			auto sum = AMC::CVectorUtils::vectorAdd(vecA, vecB);
			assertTrue(nearlyEqual(sum.m_dCoordinates[0], 0.0));
			assertTrue(nearlyEqual(sum.m_dCoordinates[1], 6.0));
			assertTrue(nearlyEqual(sum.m_dCoordinates[2], 3.5));

			auto diff = AMC::CVectorUtils::vectorSubtract(vecA, vecB);
			assertTrue(nearlyEqual(diff.m_dCoordinates[0], 2.0));
			assertTrue(nearlyEqual(diff.m_dCoordinates[1], -2.0));
			assertTrue(nearlyEqual(diff.m_dCoordinates[2], 2.5));

			auto scaled = AMC::CVectorUtils::vectorScale(vecA, 2.0);
			assertTrue(nearlyEqual(scaled.m_dCoordinates[0], 2.0));
			assertTrue(nearlyEqual(scaled.m_dCoordinates[1], 4.0));
			assertTrue(nearlyEqual(scaled.m_dCoordinates[2], 6.0));

			double dot = AMC::CVectorUtils::vectorDotProduct(vecA, vecB);
			assertTrue(nearlyEqual(dot, (1.0 * -1.0) + (2.0 * 4.0) + (3.0 * 0.5)));

			auto cross = AMC::CVectorUtils::vectorCrossProduct(
				AMC::CVectorUtils::vectorMake(1.0, 0.0, 0.0),
				AMC::CVectorUtils::vectorMake(0.0, 1.0, 0.0)
			);
			assertTrue(nearlyEqual(cross.m_dCoordinates[0], 0.0));
			assertTrue(nearlyEqual(cross.m_dCoordinates[1], 0.0));
			assertTrue(nearlyEqual(cross.m_dCoordinates[2], 1.0));

			auto normalized = AMC::CVectorUtils::vectorNormalize(vecA, 1.0e-9);
			assertTrue(nearlyEqual(AMC::CVectorUtils::vectorLength(normalized), 1.0));

			auto normalizedZero = AMC::CVectorUtils::vectorNormalize(zero, 1.0e-6);
			assertTrue(nearlyEqual(AMC::CVectorUtils::vectorLength(normalizedZero), 0.0));
		}

		void testVectorAngles()
		{
			auto vecX = AMC::CVectorUtils::vectorMake(1.0, 0.0, 0.0);
			auto vecY = AMC::CVectorUtils::vectorMake(0.0, 1.0, 0.0);

			double angleRad = AMC::CVectorUtils::vectorAngleInRadian(vecX, vecY, 1.0e-9);
			assertTrue(nearlyEqual(angleRad, CONST_PI / 2.0));

			double angleDeg = AMC::CVectorUtils::vectorAngleInDegree(vecX, vecY, 1.0e-9);
			assertTrue(nearlyEqual(angleDeg, 90.0));

			double angleZero = AMC::CVectorUtils::vectorAngleInRadian(vecX, AMC::CVectorUtils::vectorZero(), 1.0e-6);
			assertTrue(nearlyEqual(angleZero, 0.0));
		}
	};

}

#endif // __AMCTEST_UNITTEST_GEOMETRYUTILS
