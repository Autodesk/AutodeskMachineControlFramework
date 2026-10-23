/*++

Copyright (C) 2026 Autodesk Inc.

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


#ifndef __AMCTEST_UNITTEST_SCANLAB_JUMPFILTER
#define __AMCTEST_UNITTEST_SCANLAB_JUMPFILTER


#include "amc_unittests.hpp"
#include "Drivers/ScanLab/Implementation/libmcdriver_scanlab_rtcjumpfilter.hpp"

#include <cmath>
#include <limits>


namespace AMCUnitTest {

	class CUnitTestGroup_ScanLab_JumpFilter : public CUnitTestGroup {
	public:

		std::string getTestGroupName() override {
			return "ScanLab_JumpFilter";
		}

		void registerTests() override {
			registerTest("NullJumps", "Jumps to the current position are never written", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_ScanLab_JumpFilter::testNullJumps, this));
			registerTest("MinimumDistance", "Jumps shorter than the minimum distance are dropped", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_ScanLab_JumpFilter::testMinimumDistance, this));
			registerTest("Validation", "Invalid minimum distances are rejected", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_ScanLab_JumpFilter::testValidation, this));
			registerTest("SegmentOverride", "A segment override replaces the context setting until cleared", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_ScanLab_JumpFilter::testSegmentOverride, this));
			registerTest("ResolveSegmentOverride", "Profile values in mm and bits are resolved", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_ScanLab_JumpFilter::testResolveSegmentOverride, this));
			registerTest("SegmentScope", "The segment scope clears the override, also on exceptions", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_ScanLab_JumpFilter::testSegmentScope, this));
		}

		void initializeTests() override {
		}

	private:

		typedef LibMCDriver_ScanLab::Impl::CRTCJumpFilter CJumpFilter;
		typedef LibMCDriver_ScanLab::Impl::eRTCJumpFilterOverride eOverride;

		static constexpr double BITSPERMM = 10000.0;

		bool nearlyEqual(const double a, const double b, const double epsilon = 1.0e-12)
		{
			return std::abs(a - b) < epsilon;
		}

		void testNullJumps()
		{
			CJumpFilter filter;
			assertTrue(nearlyEqual(filter.getMinimumJumpDistance(), 0.0), "default is disabled");

			assertFalse(filter.jumpIsNeeded(100, 200, 100, 200, BITSPERMM), "null jump without minimum distance");
			assertTrue(filter.jumpIsNeeded(100, 200, 101, 200, BITSPERMM), "one bit jump without minimum distance");

			assertTrue(filter.setMinimumJumpDistance(0.5));
			assertFalse(filter.jumpIsNeeded(-5, 7, -5, 7, BITSPERMM), "null jump with minimum distance");
		}

		void testMinimumDistance()
		{
			CJumpFilter filter;

			// 0.0002 mm at 10000 bits per mm is 2 bits.
			assertTrue(filter.setMinimumJumpDistance(0.0002));

			assertFalse(filter.jumpIsNeeded(0, 0, 1, 0, BITSPERMM), "1 bit");
			assertFalse(filter.jumpIsNeeded(0, 0, 1, 1, BITSPERMM), "1.41 bits diagonal");
			assertFalse(filter.jumpIsNeeded(0, 0, 1, -1, BITSPERMM), "1.41 bits diagonal, negative direction");
			assertTrue(filter.jumpIsNeeded(0, 0, 2, 0, BITSPERMM), "exactly the minimum distance is written");
			assertTrue(filter.jumpIsNeeded(0, 0, 1, 2, BITSPERMM), "2.24 bits");
			assertTrue(filter.jumpIsNeeded(0, 0, 1000, 0, BITSPERMM), "long jump");

			assertFalse(filter.jumpIsNeeded(0, 0, 1, 1, -BITSPERMM), "mirrored correction factor");

			assertFalse(filter.jumpIsNeeded(0, 0, 1, 1, BITSPERMM * 2.0), "1.41 bits with 4 bit minimum");
			assertFalse(filter.jumpIsNeeded(0, 0, 3, 0, BITSPERMM * 2.0), "3 bits with 4 bit minimum");
			assertTrue(filter.jumpIsNeeded(0, 0, 4, 0, BITSPERMM * 2.0), "4 bits with 4 bit minimum");

			int32_t nMin = (std::numeric_limits<int32_t>::min)();
			int32_t nMax = (std::numeric_limits<int32_t>::max)();
			assertTrue(filter.jumpIsNeeded(nMin, nMin, nMax, nMax, BITSPERMM), "full range jump does not overflow");

			assertTrue(filter.setMinimumJumpDistance(0.0));
			assertTrue(filter.jumpIsNeeded(0, 0, 1, 0, BITSPERMM), "disabled again");
		}

		void testValidation()
		{
			CJumpFilter filter;
			assertTrue(filter.setMinimumJumpDistance(0.01));

			assertFalse(filter.setMinimumJumpDistance(-0.001), "negative");
			assertFalse(filter.setMinimumJumpDistance(RTCJUMPFILTER_MAX_MINIMUMJUMPDISTANCE + 0.001), "above maximum");
			assertFalse(filter.setMinimumJumpDistance(std::numeric_limits<double>::quiet_NaN()), "NaN");
			assertFalse(filter.setMinimumJumpDistance(std::numeric_limits<double>::infinity()), "infinity");
			assertTrue(nearlyEqual(filter.getMinimumJumpDistance(), 0.01), "rejected values keep the setting");

			assertTrue(filter.setMinimumJumpDistance(RTCJUMPFILTER_MAX_MINIMUMJUMPDISTANCE), "maximum");
			assertTrue(filter.setMinimumJumpDistance(0.0), "zero");
		}

		void testSegmentOverride()
		{
			CJumpFilter filter;

			// 0.001 mm is 10 bits.
			assertTrue(filter.setMinimumJumpDistance(0.001));
			assertFalse(filter.hasSegmentOverride());
			assertFalse(filter.jumpIsNeeded(0, 0, 3, 0, BITSPERMM), "context setting");

			assertTrue(filter.setSegmentOverride(0.0));
			assertTrue(filter.hasSegmentOverride());
			assertTrue(nearlyEqual(filter.getEffectiveMinimumJumpDistance(), 0.0));
			assertTrue(filter.jumpIsNeeded(0, 0, 3, 0, BITSPERMM), "override disables the distance check");
			assertFalse(filter.jumpIsNeeded(0, 0, 0, 0, BITSPERMM), "override keeps null jumps dropped");

			assertTrue(filter.setSegmentOverride(0.0005));
			assertFalse(filter.jumpIsNeeded(0, 0, 4, 0, BITSPERMM), "override below 5 bits");
			assertTrue(filter.jumpIsNeeded(0, 0, 5, 0, BITSPERMM), "override at 5 bits");

			assertFalse(filter.setSegmentOverride(-1.0), "invalid override");
			assertTrue(nearlyEqual(filter.getEffectiveMinimumJumpDistance(), 0.0005), "invalid override keeps the override");

			assertTrue(filter.setMinimumJumpDistance(0.002));
			assertTrue(nearlyEqual(filter.getEffectiveMinimumJumpDistance(), 0.0005), "context change does not replace the override");

			filter.clearSegmentOverride();
			assertFalse(filter.hasSegmentOverride());
			assertTrue(nearlyEqual(filter.getEffectiveMinimumJumpDistance(), 0.002), "cleared override returns to the context setting");
			assertFalse(filter.jumpIsNeeded(0, 0, 10, 0, BITSPERMM), "context setting of 20 bits");
		}

		void testResolveSegmentOverride()
		{
			double dDistance = -1.0;
			double dNaN = std::numeric_limits<double>::quiet_NaN();

			assertTrue(CJumpFilter::resolveSegmentOverride(false, 0.0, false, 0.0, BITSPERMM, dDistance) == eOverride::NoOverride, "no values");
			assertTrue(nearlyEqual(dDistance, 0.0));

			assertTrue(CJumpFilter::resolveSegmentOverride(true, 0.003, false, 0.0, BITSPERMM, dDistance) == eOverride::Valid, "mm");
			assertTrue(nearlyEqual(dDistance, 0.003));

			assertTrue(CJumpFilter::resolveSegmentOverride(true, 0.0, false, 0.0, BITSPERMM, dDistance) == eOverride::Valid, "zero mm");
			assertTrue(nearlyEqual(dDistance, 0.0));

			assertTrue(CJumpFilter::resolveSegmentOverride(false, 0.0, true, 5.0, BITSPERMM, dDistance) == eOverride::Valid, "bits");
			assertTrue(nearlyEqual(dDistance, 0.0005));

			assertTrue(CJumpFilter::resolveSegmentOverride(false, 0.0, true, 5.0, -BITSPERMM, dDistance) == eOverride::Valid, "bits with mirrored correction factor");
			assertTrue(nearlyEqual(dDistance, 0.0005));

			assertTrue(CJumpFilter::resolveSegmentOverride(true, 0.001, true, 5.0, BITSPERMM, dDistance) == eOverride::ProvidedTwice, "mm and bits");

			assertTrue(CJumpFilter::resolveSegmentOverride(true, -0.001, false, 0.0, BITSPERMM, dDistance) == eOverride::Invalid, "negative mm");
			assertTrue(CJumpFilter::resolveSegmentOverride(true, 2.0, false, 0.0, BITSPERMM, dDistance) == eOverride::Invalid, "mm above maximum");
			assertTrue(CJumpFilter::resolveSegmentOverride(true, dNaN, false, 0.0, BITSPERMM, dDistance) == eOverride::Invalid, "NaN mm");
			assertTrue(CJumpFilter::resolveSegmentOverride(false, 0.0, true, -1.0, BITSPERMM, dDistance) == eOverride::Invalid, "negative bits");
			assertTrue(CJumpFilter::resolveSegmentOverride(false, 0.0, true, 20000.0, BITSPERMM, dDistance) == eOverride::Invalid, "bits above maximum");
			assertTrue(CJumpFilter::resolveSegmentOverride(false, 0.0, true, dNaN, BITSPERMM, dDistance) == eOverride::Invalid, "NaN bits");
			assertTrue(CJumpFilter::resolveSegmentOverride(false, 0.0, true, 5.0, 0.0, dDistance) == eOverride::Invalid, "bits without correction factor");
		}

		void testSegmentScope()
		{
			CJumpFilter filter;
			assertTrue(filter.setMinimumJumpDistance(0.001));

			{
				LibMCDriver_ScanLab::Impl::CRTCJumpFilterSegmentScope scope(&filter);
				assertTrue(filter.setSegmentOverride(0.0));
				assertTrue(filter.hasSegmentOverride());
			}
			assertFalse(filter.hasSegmentOverride(), "cleared at the end of the scope");

			struct ELayerFailed {};

			bool bOverrideSet = false;
			bool bCaught = false;
			try {
				LibMCDriver_ScanLab::Impl::CRTCJumpFilterSegmentScope scope(&filter);
				bOverrideSet = filter.setSegmentOverride(0.0);
				throw ELayerFailed();
			}
			catch (ELayerFailed&) {
				bCaught = true;
			}
			assertTrue(bOverrideSet);
			assertTrue(bCaught);
			assertFalse(filter.hasSegmentOverride(), "cleared when the layer fails");
			assertTrue(nearlyEqual(filter.getEffectiveMinimumJumpDistance(), 0.001));
		}

	};

}

#endif // __AMCTEST_UNITTEST_SCANLAB_JUMPFILTER
