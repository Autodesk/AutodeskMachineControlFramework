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


#include "common_chrono.hpp"

#ifdef _WIN32
#include <Windows.h>

#else
#include <time.h>

#endif


#include <thread>
#include <exception>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <ctime>

namespace AMCCommon {

	class CChrono_Impl {
	private:
#ifdef _WIN32
		LARGE_INTEGER StartingTime;
		LARGE_INTEGER Frequency;
		struct tm utc_time;
#else		
		std::tm utc_time;
		timespec startTime;
#endif
	public:

		CChrono_Impl()
		{
#ifdef _WIN32
			QueryPerformanceFrequency(&Frequency);
			QueryPerformanceCounter(&StartingTime);

			__int64 ltime;
			_time64(&ltime);

			errno_t err;
			err = _gmtime64_s(&utc_time, &ltime);
#else
			std::time_t t = std::time(nullptr);
			utc_time = *std::localtime(&t);

			clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &startTime);
#endif

		}

		uint64_t getElapsedMicroseconds()
		{
#ifdef _WIN32
			LARGE_INTEGER EndingTime, ElapsedMicroseconds;

			QueryPerformanceCounter(&EndingTime);

			ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;

			ElapsedMicroseconds.QuadPart *= 1000000ULL;
			ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;

			return ElapsedMicroseconds.QuadPart;

#else
			timespec currentTime;
			clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &currentTime);

			timespec temp;
			if ((currentTime.tv_nsec - startTime.tv_nsec) < 0) {
				temp.tv_sec = currentTime.tv_sec - startTime.tv_sec - 1;
				temp.tv_nsec = 1000000000 + currentTime.tv_nsec - startTime.tv_nsec;
			}
			else {
				temp.tv_sec = currentTime.tv_sec - startTime.tv_sec;
				temp.tv_nsec = currentTime.tv_nsec - startTime.tv_nsec;
			}

			return (temp.tv_nsec / 1000) + temp.tv_sec * 1000000;
#endif
		}

		std::string getStartTimeISO8601TimeUTC()
		{
			std::stringstream sstream;
			sstream << std::put_time(&utc_time, "%FT%TZ") << " UTC";
			return sstream.str();
		}

		std::string getStartTimeFileName()
		{
			std::stringstream sstream;
			sstream << std::put_time(&utc_time, "%Y%m%d_%H%M%S");
			return sstream.str();
		}

	};

	CChrono::CChrono()
		: m_pChronoImpl (std::make_unique <CChrono_Impl> ())
	{
	}

	CChrono::~CChrono()
	{
	}

	std::string CChrono::getStartTimeISO8601TimeUTC()
	{
		return m_pChronoImpl->getStartTimeISO8601TimeUTC();
	}

	std::string CChrono::getStartTimeFileName()
	{
		return m_pChronoImpl->getStartTimeFileName();
	}

	uint64_t CChrono::getExistenceTimeInSeconds()
	{
		return getExistenceTimeInMicroseconds() / 1000000ULL;
	}

	uint64_t CChrono::getExistenceTimeInMilliseconds()
	{
		return getExistenceTimeInMicroseconds() / 1000ULL;
	}

	uint64_t CChrono::getExistenceTimeInMicroseconds()
	{
		return m_pChronoImpl->getElapsedMicroseconds();
	}

	uint64_t CChrono::getDurationTimeInSeconds(const uint64_t nStartTimeInSeconds)
	{
		uint64_t nCurrentTimeInSeconds = getExistenceTimeInSeconds ();
		if (nCurrentTimeInSeconds < nStartTimeInSeconds)
			throw std::runtime_error("timing error");
		
		return nCurrentTimeInSeconds - nStartTimeInSeconds;
	}

	uint64_t CChrono::getDurationTimeInMilliseconds(const uint64_t nStartTimeInMilliseconds)
	{
		uint64_t nCurrentTimeInMilliseconds = getExistenceTimeInMilliseconds();
		if (nCurrentTimeInMilliseconds < nStartTimeInMilliseconds)
			throw std::runtime_error("timing error");

		return nCurrentTimeInMilliseconds - nStartTimeInMilliseconds;

	}

	uint64_t CChrono::getDurationTimeInMicroseconds(const uint64_t nStartTimeInMicroseconds)
	{
		uint64_t nCurrentTimeInMicroseconds = getExistenceTimeInMicroseconds();
		if (nCurrentTimeInMicroseconds < nStartTimeInMicroseconds)
			throw std::runtime_error("timing error");

		return nCurrentTimeInMicroseconds - nStartTimeInMicroseconds;

	}


	void CChrono::sleepSeconds(const uint64_t seconds)
	{
		sleepMicroseconds(seconds * 1000000);
	}

	void CChrono::sleepMilliseconds(const uint64_t milliseconds)
	{
		sleepMicroseconds(milliseconds * 1000);
	}

	void CChrono::sleepMicroseconds(const uint64_t microseconds)
	{
#ifdef _WIN32
		std::this_thread::sleep_for(std::chrono::microseconds(microseconds));
#else
		struct timespec tim, tim2;
		tim.tv_sec = microseconds / 1000000;
		tim.tv_nsec = (microseconds % 1000000) * 1000;

		if (nanosleep(&tim, &tim2) < 0) {
			throw std::runtime_error("sleep failed");
		}

#endif
	}

/*

	std::string CUtils::getCurrentISO8601TimeUTC() {

#ifdef _WIN32
		struct tm utc_time;

		__int64 ltime;
		_time64(&ltime);

		errno_t err;
		err = _gmtime64_s(&utc_time, &ltime);
#else
		std::time_t t = std::time(nullptr);
		std::tm utc_time = *std::localtime(&t);
#endif

		std::stringstream sstream;
		sstream << std::put_time(&utc_time, "%FT%TZ") << " UTC";
		return sstream.str();

	}

	std::string CUtils::getCurrentTimeFileName() {

#ifdef _WIN32
		struct tm utc_time;
		__int64 ltime;
		_time64(&ltime);

		errno_t err;
		err = _gmtime64_s(&utc_time, &ltime);

#else
		std::time_t t = std::time(nullptr);
		std::tm utc_time = *std::localtime(&t);

#endif

		std::stringstream sstream;
		sstream << std::put_time(&utc_time, "%Y%m%d_%H%M%S");
		return sstream.str();


	}


 */

}
