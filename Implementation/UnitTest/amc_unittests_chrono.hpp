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

#ifndef __AMCTEST_UNITTEST_CHRONO
#define __AMCTEST_UNITTEST_CHRONO


#include "amc_unittests.hpp"
#include "common_chrono.hpp"
#include <cstdlib>
#include <ctime>


// Portable helpers to set/unset TZ and apply changes
namespace portable_tz {

    inline const char* get_tz() {
        return std::getenv("TZ");
    }

#if defined(_WIN32)
    inline void set_tz(const char* value) {
        _putenv_s("TZ", value ? value : "");
        _tzset();
    }
    inline void unset_tz() {
        _putenv_s("TZ", "");
        _tzset();
    }
#else
    inline void set_tz(const char* value) {
        if (value) setenv("TZ", value, 1);
        else unsetenv("TZ");
        tzset();
    }
    inline void unset_tz() {
        unsetenv("TZ");
        tzset();
    }
#endif

} // namespace portable_tz

namespace AMCUnitTest {

    class CUnitTestGroup_Chrono : public CUnitTestGroup {
    public:


        virtual std::string getTestGroupName() override {
            return "Chrono";
        }

        virtual void registerTests() override {
            registerTest("ISO8601RoundTrip", "ISO 8601 UTC string should round-trip correctly through parse/convert cycle", eUnitTestCategory::utMandatoryPass,
                std::bind(&CUnitTestGroup_Chrono::testISO8601RoundTrip, this));

            registerTest("TimezoneIndependence", "UTC parsing should be independent of system timezone", eUnitTestCategory::utMandatoryPass,
                std::bind(&CUnitTestGroup_Chrono::testTimezoneIndependence, this));

            registerTest("MidnightBoundary", "Midnight timestamps should round-trip correctly", eUnitTestCategory::utMandatoryPass,
                std::bind(&CUnitTestGroup_Chrono::testMidnightBoundary, this));

            registerTest("LeapYear", "Leap year date should round-trip correctly", eUnitTestCategory::utOptionalPass,
                std::bind(&CUnitTestGroup_Chrono::testLeapYear, this));

            registerTest("ChronoInstanceAndConversions", "CChrono instance usage and full conversion helpers", eUnitTestCategory::utMandatoryPass,
                std::bind(&CUnitTestGroup_Chrono::testChronoInstanceAndConversions, this));
        }

        virtual void initializeTests() override {
            // Optional setup logic
        }

    private:

        void testISO8601RoundTrip() {
            // Test multiple different timestamps to ensure fix works across different dates/times

            std::vector<std::string> testCases = {
                "2025-01-01T00:00:00.000000Z",
                "2025-06-15T12:30:45.123456Z",
                "2025-12-31T23:59:59.999999Z",
                "2025-10-01T15:24:29.729232Z", // original bug detected date
                "2024-02-29T18:45:30.500000Z"  // Leap year
            };

            for (const auto& original : testCases) {
                uint64_t microseconds = AMCCommon::CChrono::parseISO8601TimeUTC(original);
                std::string converted = AMCCommon::CChrono::convertToISO8601TimeUTC(microseconds);

                assertTrue(original == converted, "Timestamp '" + original + "' should round-trip correctly, got '" + converted + "'");
            }
        }





        void testTimezoneIndependence() {
            // Save original TZ (may be null)
            const char* originalTZ = portable_tz::get_tz();
            std::string originalCopy = originalTZ ? std::string(originalTZ) : std::string();

            // We’ll use POSIX TZ strings so it also works on Windows:
            // "GMT0"      -> UTC+0
            // "EET-2"     -> UTC+2  (note the minus means +2)
            // "EST+5"     -> UTC-5  (plus means -5)
            // The exact offsets don’t matter for this test; we just want different locals.
            portable_tz::set_tz("GMT0");
            uint64_t utcResult = AMCCommon::CChrono::parseISO8601TimeUTC("2025-10-01T15:24:29Z");

            portable_tz::set_tz("EET-2"); // "UTC+2"
            uint64_t plus2Result = AMCCommon::CChrono::parseISO8601TimeUTC("2025-10-01T15:24:29Z");

            portable_tz::set_tz("EST+5"); // "UTC-5"
            uint64_t minus5Result = AMCCommon::CChrono::parseISO8601TimeUTC("2025-10-01T15:24:29Z");

            // Restore original TZ
            if (!originalCopy.empty()) {
                portable_tz::set_tz(originalCopy.c_str());
            }
            else {
                portable_tz::unset_tz();
            }

            // All results should be identical since they represent the same UTC time
            assertTrue(utcResult == plus2Result, "UTC parsing should be timezone-independent (UTC vs UTC+2)");
            assertTrue(utcResult == minus5Result, "UTC parsing should be timezone-independent (UTC vs UTC-5)");
        }

        void testMidnightBoundary() {
            // Test midnight times specifically, as this is where getMicrosecondsSince1970FromDay starts

            std::string original = "2025-10-01T00:00:00.000000Z";
            uint64_t microseconds = AMCCommon::CChrono::parseISO8601TimeUTC(original);
            std::string converted = AMCCommon::CChrono::convertToISO8601TimeUTC(microseconds);

            assertTrue(original == converted, "Midnight timestamp must round-trip correctly");
        }

        void testLeapYear() {
            // Test leap year date (Feb 29, 2024)

            std::string original = "2024-02-29T12:00:00.000000Z";
            uint64_t microseconds = AMCCommon::CChrono::parseISO8601TimeUTC(original);
            std::string converted = AMCCommon::CChrono::convertToISO8601TimeUTC(microseconds);

            assertTrue(original == converted, "Leap year timestamp must round-trip correctly");
        }


        void testChronoInstanceAndConversions() {
            AMCCommon::CChrono chrono;
            uint64_t start = chrono.getStartTimeStampInMicrosecondsSince1970();
            uint64_t current = chrono.getUTCTimeStampInMicrosecondsSince1970();
            assertTrue(current >= start);

            std::string nowISO = chrono.getUTCTimeInISO8601();
            uint64_t nowParsed = AMCCommon::CChrono::parseISO8601TimeUTC(nowISO);
            assertTrue(AMCCommon::CChrono::timeStampIsWithinAMillionYears(nowParsed));

            uint64_t elapsedBefore = chrono.getElapsedMicroseconds();
            AMCCommon::CChrono::sleepMicroseconds(1000);
            uint64_t elapsedAfter = chrono.getElapsedMicroseconds();
            assertTrue(elapsedAfter >= elapsedBefore);

            AMCCommon::CChrono::sleepMilliseconds(1);
            AMCCommon::CChrono::sleepSeconds(0);

            uint64_t dayStamp = AMCCommon::CChrono::getMicrosecondsSince1970FromDay(2025, 1, 1);
            uint64_t dateTimeStamp = AMCCommon::CChrono::getMicrosecondsSince1970FromDateTime(2025, 1, 1, 0, 0, 0, 0);
            assertTrue(dayStamp == dateTimeStamp);

            uint32_t year = 0, month = 0, day = 0, weekday = 0;
            AMCCommon::CChrono::parseDateFromMicrosecondsSince1970WithWeekday(dayStamp, year, month, day, weekday);
            assertTrue(year == 2025);
            assertTrue(month == 1);
            assertTrue(day == 1);
            assertTrue((weekday >= 1) && (weekday <= 7));

            uint32_t yearOnly = 0, monthOnly = 0, dayOnly = 0;
            AMCCommon::CChrono::parseDateFromMicrosecondsSince1970(dayStamp, yearOnly, monthOnly, dayOnly);
            assertTrue(yearOnly == 2025);
            assertTrue(monthOnly == 1);
            assertTrue(dayOnly == 1);

            uint32_t hour = 0, minute = 0, second = 0, microsecond = 0;
            AMCCommon::CChrono::parseDateTimeFromMicrosecondsSince1970(dayStamp, year, month, day, hour, minute, second, microsecond);
            assertTrue(hour == 0);
            assertTrue(minute == 0);
            assertTrue(second == 0);
            assertTrue(microsecond == 0);

            uint64_t sampleStamp = AMCCommon::CChrono::getMicrosecondsSince1970FromDateTime(2025, 6, 15, 12, 30, 45, 123000);
            std::string sampleMicro = AMCCommon::CChrono::convertToISO8601TimeUTC(sampleStamp, AMCCommon::eUTCStringAccuracy::Microseconds);
            std::string sampleMilli = AMCCommon::CChrono::convertToISO8601TimeUTC(sampleStamp, AMCCommon::eUTCStringAccuracy::Milliseconds);
            std::string sampleSecond = AMCCommon::CChrono::convertToISO8601TimeUTC(sampleStamp, AMCCommon::eUTCStringAccuracy::Seconds);
            assertTrue(sampleMicro.find(".") != std::string::npos);
            assertTrue(sampleMilli.find(".") != std::string::npos);
            assertTrue(sampleSecond.find(".") == std::string::npos);

            bool thrown = false;
            try {
                AMCCommon::CChrono::convertToISO8601TimeUTC(sampleStamp, static_cast<AMCCommon::eUTCStringAccuracy>(0));
            }
            catch (...) {
                thrown = true;
            }
            assertTrue(thrown, "Expected convertToISO8601TimeUTC to throw on invalid accuracy");

            assertTrue(AMCCommon::CChrono::yearIsLeapYear(2024));
            assertFalse(AMCCommon::CChrono::yearIsLeapYear(2100));

            uint64_t tooLarge = 1000000ULL * 3600ULL * 365ULL * 1000000ULL + 1;
            assertFalse(AMCCommon::CChrono::timeStampIsWithinAMillionYears(tooLarge));
        }

    };

}

#endif // __AMCTEST_UNITTEST_CHRONO
