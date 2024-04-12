/*++

Copyright (C) 2020 Autodesk Inc.

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


#ifndef __AMC_CHRONO
#define __AMC_CHRONO

#include <memory>
#include <string>
#include <vector>

namespace AMCCommon {

	enum class eUTCStringAccuracy : uint32_t {
		Microseconds = 1,
		Milliseconds = 2,
		Seconds = 3
	};

	class CChrono_Impl;

	class CChrono {
		private:
			std::unique_ptr<CChrono_Impl> m_pChronoImpl;
		
		public:
		
			CChrono (bool bHighResolution = false);
			~CChrono ();
			
			// Returns instance creation time in ISO8601 time format
			std::string getStartTimeISO8601TimeUTC ();			
			
			// Returns instance creation time in a filename compatible format
			std::string getStartTimeFileName();

			// Returns Duration in seconds since instance creation
			// Subsequent calls of this method will never decrease.
			uint64_t getExistenceTimeInSeconds ();

			// Returns Duration in milliseconds since instance creation
			// Subsequent calls of this method will never decrease.
			uint64_t getExistenceTimeInMilliseconds ();

			// Returns Duration in microseconds since instance creation
			// Subsequent calls of this method will never decrease.
			uint64_t getExistenceTimeInMicroseconds ();

			// Returns Duration in seconds since the time passed
			// Passed timestamp MUST have been created with getExistenceTimeInSeconds
			uint64_t getDurationTimeInSeconds(const uint64_t nStartTimeInSeconds);

			// Returns Duration in seconds since the time passed
			// Passed timestamp MUST have been created with getExistenceTimeInSeconds
			uint64_t getDurationTimeInMilliseconds(const uint64_t nStartTimeInMilliseconds);

			// Returns Duration in seconds since the time passed
			// Passed timestamp MUST have been created with getExistenceTimeInSeconds
			uint64_t getDurationTimeInMicroseconds(const uint64_t nStartTimeInMicroseconds);


			// Sleeps for some seconds
			void sleepSeconds(const uint64_t seconds);

			// Sleeps for some milliseconds
			void sleepMilliseconds(const uint64_t milliseconds);

			// Sleeps for some microseconds
			void sleepMicroseconds(const uint64_t microseconds);

			// Checks if a timestamp is within a million years
			static bool timeStampIsWithinAMillionYears(const uint64_t nMicroseconds); 

			// Parses a UTC String in ISO8601 format
			static uint64_t parseISO8601TimeUTC(const std::string& sUTCString);

			// Converts a time stamp into a UTC string with a defined accuracy
			static std::string convertToISO8601TimeUTC(const uint64_t nMicrosecondsSince1970, eUTCStringAccuracy eAccuracy = AMCCommon::eUTCStringAccuracy::Microseconds);

			// Parses the date of a date time since 1970, including the day of the week.
			// Returns the year, the month (1-12), the day of the month (1-31), and the day of the week (1-7, 1 = Monday)
			static void parseDateFromMicrosecondsSince1970WithWeekday(const uint64_t nMicrosecondsSince1970, uint32_t& nYear, uint32_t& nMonth, uint32_t& nDay, uint32_t & nDayOfTheWeek);

			// Parses the date of a date time since 1970
			// Returns the year, the month (1-12), the day of the month (1-31)
			static void parseDateFromMicrosecondsSince1970(const uint64_t nMicrosecondsSince1970, uint32_t& nYear, uint32_t& nMonth, uint32_t& nDay);

			// Parses the full date time of a timestamp since 1970
			// Returns the year, the month (1-12), the day of the month (1-31),
			// the hour (0-23), the minute (0-59), the second (0-59) and the microsecond (0-999999)
			static void parseDateTimeFromMicrosecondsSince1970(const uint64_t nMicrosecondsSince1970, uint32_t& nYear, uint32_t& nMonth, uint32_t& nDay, uint32_t & nHour, uint32_t & nMinute, uint32_t & nSecond, uint32_t nMicrosecond);

			// Gets the timestamp of midnight of any given day.
			// Inputs are the year, the month (1-12), the day of the month (1-31)
			// Fails if the Year is before 1970, or the date is otherwise invalid.
			static uint64_t getMicrosecondsSince1970FromDay (uint32_t nYear, uint32_t nMonth, uint32_t nDay);

			// Gets the timestamp for a full date time.
			// Inputs are the year, the month (1-12), the day of the month (1-31),
			// the hour (0-23), the minute (0-59), the second (0-59) and the microsecond (0-999999).
			// Fails if the Year is before 1970, or the date is otherwise invalid.
			static uint64_t getMicrosecondsSince1970FromDateTime(uint32_t nYear, uint32_t nMonth, uint32_t nDay, uint32_t nHour, uint32_t nMinute, uint32_t nSecond, uint32_t nMicrosecond);

			// Returns the current system time in UTC time zone, as microseconds since 1970
			static uint64_t getCurrentUTCTime();

			// Returns if a year is a leap year
			static bool yearIsLeapYear(uint32_t nYear);
	};

	typedef std::shared_ptr<CChrono> PChrono;
	
}


#endif //__AMC_CHRONO

