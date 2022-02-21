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
#include <windows.h>
#endif 

#ifdef __GNUC__
#include <time.h>
#else
#include <timeapi.h>
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
		SYSTEMTIME m_StartSytemTime;
		bool m_bHighResTimerAvailable;
		LARGE_INTEGER m_HighResStartingTicks;
		LARGE_INTEGER m_HighResFrequency;
		uint64_t m_LowResStartingMilliseconds;
		
#else		
		std::tm utc_time;
		timespec startTime;
#endif
	public:

		CChrono_Impl(bool bHighResolution)
		{
#ifdef _WIN32
			GetSystemTime(&m_StartSytemTime);
			m_LowResStartingMilliseconds = timeGetTime();
			m_bHighResTimerAvailable = false;
			m_HighResFrequency.QuadPart = 0;
			m_HighResStartingTicks.QuadPart = 0;
			if (bHighResolution) {

				if (QueryPerformanceFrequency(&m_HighResFrequency)) {
					if (QueryPerformanceCounter(&m_HighResStartingTicks)) {
						m_bHighResTimerAvailable = true;
					}
				}
			}

#else
			std::time_t t = std::time(nullptr);
			utc_time = *std::localtime(&t);

			clock_gettime(CLOCK_MONOTONIC, &startTime);
#endif

		}

		uint64_t getElapsedMicroseconds()
		{
#ifdef _WIN32

			if (m_bHighResTimerAvailable) {

				LARGE_INTEGER CurrentTicks, ElapsedMicroseconds;
				if (QueryPerformanceCounter(&CurrentTicks)) {

					if (CurrentTicks.QuadPart < m_HighResStartingTicks.QuadPart)
						throw std::runtime_error ("high resolution timer clock error");

					ElapsedMicroseconds.QuadPart = CurrentTicks.QuadPart - m_HighResStartingTicks.QuadPart;

					ElapsedMicroseconds.QuadPart *= 1000000ULL;
					ElapsedMicroseconds.QuadPart /= m_HighResFrequency.QuadPart;

					return ElapsedMicroseconds.QuadPart;
				}

				// If Performance Counter not available, fall back to low res counter
			}

			uint64_t currentMilliseconds = (uint64_t)timeGetTime();
			if (currentMilliseconds < m_LowResStartingMilliseconds)
				throw std::runtime_error("high resolution clock error");

			return ((uint64_t)(currentMilliseconds - m_LowResStartingMilliseconds)) * 1000ULL;


#else
			timespec currentTime;
			clock_gettime(CLOCK_MONOTONIC, &currentTime);

			timespec temp;
			if (currentTime.tv_nsec < startTime.tv_nsec) {
				temp.tv_sec = currentTime.tv_sec - startTime.tv_sec - 1;
				temp.tv_nsec = 1000000000 + currentTime.tv_nsec - startTime.tv_nsec;
			}
			else {
				temp.tv_sec = currentTime.tv_sec - startTime.tv_sec;
				temp.tv_nsec = currentTime.tv_nsec - startTime.tv_nsec;
			}

			int64_t microSeconds = ((int64_t)temp.tv_nsec / 1000LL) + (int64_t) temp.tv_sec * 1000000LL;
			if (microSeconds < 0)
				throw std::runtime_error ("clock time was returned negative");
			
			return (uint64_t) microSeconds;
#endif
		}

		std::string getStartTimeISO8601TimeUTC()
		{
			std::stringstream sstream;
#ifdef _WIN32
			sstream << std::setfill('0')
				<< std::setw(4) << m_StartSytemTime.wYear << "-"
				<< std::setw(2) << m_StartSytemTime.wMonth << "-"
				<< std::setw(2) << m_StartSytemTime.wDay << "T"
				<< std::setw(2) << m_StartSytemTime.wHour << ":"
				<< std::setw(2) << m_StartSytemTime.wMinute << ":"
				<< std::setw(2) << m_StartSytemTime.wSecond << "."
				<< std::setw(3) << m_StartSytemTime.wMilliseconds << "Z UTC";
#else
			sstream << std::put_time(&utc_time, "%FT%TZ") << " UTC";
#endif
			return sstream.str();
		}

		std::string getStartTimeFileName()
		{
			std::stringstream sstream;
#ifdef _WIN32
			sstream << std::setfill('0')
				<< std::setw(4) << m_StartSytemTime.wYear
				<< std::setw(2) << m_StartSytemTime.wMonth
				<< std::setw(2) << m_StartSytemTime.wDay << "_"
				<< std::setw(2) << m_StartSytemTime.wHour 
				<< std::setw(2) << m_StartSytemTime.wMinute 
				<< std::setw(2) << m_StartSytemTime.wSecond;
#else
			sstream << std::put_time(&utc_time, "%Y%m%d_%H%M%S");
#endif

			return sstream.str();
		}

	};

	CChrono::CChrono(bool bHighResolution)
		: m_pChronoImpl (std::make_unique <CChrono_Impl> (bHighResolution))
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
