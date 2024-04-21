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

		bool m_bHighResTimerAvailable;
		LARGE_INTEGER m_HighResStartingTicks;
		LARGE_INTEGER m_HighResFrequency;
		uint64_t m_LowResStartingMilliseconds;

#else		
		timespec startTime;
#endif

		uint64_t m_nStartTimeStampUTC;

	public:

		CChrono_Impl(bool bHighResolution, uint64_t nStartTimeStampUTC)
			: m_nStartTimeStampUTC (nStartTimeStampUTC)
		{

#ifdef _WIN32
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
						throw std::runtime_error("high resolution timer clock error");

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

			int64_t microSeconds = ((int64_t)temp.tv_nsec / 1000LL) + (int64_t)temp.tv_sec * 1000000LL;
			if (microSeconds < 0)
				throw std::runtime_error("clock time was returned negative");

			return (uint64_t)microSeconds;
#endif
		}

		uint64_t getAbsoluteMicroseconds()
		{
			return m_nStartTimeStampUTC + getElapsedMicroseconds();
		}

		uint64_t getStartTimeMicroseconds()
		{
			return m_nStartTimeStampUTC;
		}
		

	};

	CChrono::CChrono()
		: m_pChronoImpl(std::make_unique <CChrono_Impl>(true, getCurrentUTCTimeInternal()))
	{
	}

	CChrono::~CChrono()
	{
	}

	uint64_t CChrono::getUTCTimeStampInMicrosecondsSince1970()
	{
		return m_pChronoImpl->getAbsoluteMicroseconds();
	}

	uint64_t CChrono::getStartTimeStampInMicrosecondsSince1970()
	{
		return m_pChronoImpl->getStartTimeMicroseconds();
	}


	std::string CChrono::getUTCTimeInISO8601()
	{
		return convertToISO8601TimeUTC(getUTCTimeStampInMicrosecondsSince1970());
	}

	void CChrono::sleepSeconds(const uint64_t seconds)
	{
		sleepMicroseconds(seconds * 1000000ULL);
	}

	void CChrono::sleepMilliseconds(const uint64_t milliseconds)
	{
		sleepMicroseconds(milliseconds * 1000ULL);
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


	// Checks if a timestamp is within a million years
	bool CChrono::timeStampIsWithinAMillionYears(const uint64_t nMicroseconds)
	{
		return (nMicroseconds < (1000000ULL * 3600ULL * 365ULL * 1000000ULL));
	}


	const char* parseUInt32InISO8601String(const char* pStringPointer, const char primarySeparationCharacter, const char secondarySeparationCharacter, uint32_t nMaxValue, uint32_t& nParsedValue, char& nFoundSeparator)
	{
		if (pStringPointer == nullptr)
			throw std::runtime_error("invalid input string parameter");

		const char* pPtr = pStringPointer;

		nFoundSeparator = 0;
		nParsedValue = 0;

		while (*pPtr != 0) {
			char ch = *pPtr;
			pPtr++;

			if ((ch == primarySeparationCharacter) || (ch == secondarySeparationCharacter)) {
				nFoundSeparator = ch;
				return pPtr;
			}
			else if ((ch >= '0') && (ch <= '9')) {
				nParsedValue *= 10;
				nParsedValue += (ch - '0');

				if (nParsedValue > nMaxValue)
					throw std::runtime_error("ISO8601 time value larger than " + std::to_string(nMaxValue) + ": " + std::string(pStringPointer));

			}
			else {
				throw std::runtime_error("invalid character in string: " + std::string(pPtr));
			}

		}

		throw std::runtime_error("ISO8601 time string parsing error: " + std::string(pStringPointer));

	}

	uint64_t CChrono::parseISO8601TimeUTC(const std::string& sUTCString)
	{
		uint32_t nYear = 0;
		uint32_t nMonth = 0;
		uint32_t nDay = 0;
		uint32_t nHour = 0;
		uint32_t nMinute = 0;
		uint32_t nSecond = 0;
		uint32_t nMicrosecond = 0;

		char nFoundSeparator = 0;

		// Parse for the separators. Input string is supposed to look like "2024-04-08T23:59:50.166Z"
		const char* pYearPtr = sUTCString.c_str();
		const char* pMonthPtr = parseUInt32InISO8601String(pYearPtr, '-', 0, 9999, nYear, nFoundSeparator);
		const char* pDayPtr = parseUInt32InISO8601String(pMonthPtr, '-', 0, 12, nMonth, nFoundSeparator);
		const char* pHourPtr = parseUInt32InISO8601String(pDayPtr, 'T', 0, 31, nDay, nFoundSeparator);
		const char* pMinutePtr = parseUInt32InISO8601String(pHourPtr, ':', 0, 23, nHour, nFoundSeparator);
		const char* pSecondPtr = parseUInt32InISO8601String(pMinutePtr, ':', 0, 59, nMinute, nFoundSeparator);
		const char* pResidue = parseUInt32InISO8601String(pSecondPtr, '.', 'Z', 59, nSecond, nFoundSeparator);

		if (nFoundSeparator == '.') {

			// If given, parse Fractional seconds too
			uint32_t nPostCommaNumber = 0;
			pResidue = parseUInt32InISO8601String(pResidue, 'Z', 0, 99999999, nPostCommaNumber, nFoundSeparator);

			if (nPostCommaNumber > 0) {
				uint32_t nDigits = 0;
				uint32_t nDummy = nPostCommaNumber;
				while (nDummy > 0) {
					nDigits++;
					nDummy /= 10;
				}

				if (nDigits <= 6) {
					// If post comma number is less than 6 digits, then multiply up to become microseconds
					nMicrosecond = nPostCommaNumber;
					for (uint32_t nIndex = nDigits; nIndex < 6; nIndex++)
						nMicrosecond *= 10;
				}
				else if (nDigits > 6) {
					// If post comma number is larger than 6 digits, then divide down to become microseconds
					nMicrosecond = nPostCommaNumber;
					for (uint32_t nIndex = 6; nIndex < nDigits; nIndex++)
						nMicrosecond /= 10;

				}

			}
		}

		std::string sResidue(pResidue);

		// left trim residue
		sResidue.erase(sResidue.begin(), std::find_if(sResidue.begin(), sResidue.end(), [](int ch) {
			return !std::isspace(ch);
		}));

		// right trim residue
		sResidue.erase(std::find_if(sResidue.rbegin(), sResidue.rend(), [](int ch) {
			return !std::isspace(ch);
		}).base(), sResidue.end());

		if ((!sResidue.empty()) && (sResidue != "UTC"))
			throw std::runtime_error("Invalid ISO8601 UTC residue: " + sResidue);

		return getMicrosecondsSince1970FromDateTime(nYear, nMonth, nDay, nHour, nMinute, nSecond, nMicrosecond);
	}


	std::string CChrono::convertToISO8601TimeUTC(const uint64_t nMicrosecondsSince1970, eUTCStringAccuracy eAccuracy)
	{
		uint32_t nYear = 0;
		uint32_t nMonth = 0;
		uint32_t nDay = 0;
		uint32_t nHour = 0;
		uint32_t nMinute = 0;
		uint32_t nSecond = 0;
		uint32_t nMicrosecond = 0;

		if (!timeStampIsWithinAMillionYears(nMicrosecondsSince1970))
			throw std::runtime_error("timestamp is invalid: " + std::to_string (nMicrosecondsSince1970));

		parseDateTimeFromMicrosecondsSince1970(nMicrosecondsSince1970, nYear, nMonth, nDay, nHour, nMinute, nSecond, nMicrosecond);

		std::stringstream sstream;
		sstream << std::setfill('0')
			<< std::setw(4) << nYear << "-"
			<< std::setw(2) << nMonth << "-"
			<< std::setw(2) << nDay << "T"
			<< std::setw(2) << nHour << ":"
			<< std::setw(2) << nMinute << ":"
			<< std::setw(2) << nSecond;

		switch (eAccuracy) {
		case eUTCStringAccuracy::Microseconds:
			sstream << "." << std::setw(6) << nMicrosecond;
			break;
		case eUTCStringAccuracy::Milliseconds:
			sstream << "." << std::setw(3) << (nMicrosecond / 1000);
			break;
		case eUTCStringAccuracy::Seconds:
			break;
		default:
			throw std::runtime_error("invalid UTC string accuracy");
		}

		sstream << "Z";

		return sstream.str();
	}


	uint64_t CChrono::getMicrosecondsSince1970FromDay(uint32_t nYear, uint32_t nMonth, uint32_t nDay)
	{
#ifdef _WIN32
		SYSTEMTIME st;

		st.wYear = nYear;
		st.wMonth = nMonth;
		st.wDay = nDay;
		st.wHour = 0;
		st.wMinute = 0;
		st.wSecond = 0;
		st.wMilliseconds = 0;

		FILETIME ft;
		ULARGE_INTEGER ui;

		if (!SystemTimeToFileTime(&st, &ft)) {
			throw std::runtime_error("Input Date is not valid: " + std::to_string(nYear) + "/" + std::to_string(nMonth) + "/" + std::to_string(nDay));
		}

		ui.LowPart = ft.dwLowDateTime;
		ui.HighPart = ft.dwHighDateTime;

		int64_t intervalSinceFileTimeEpoch = ui.QuadPart;

		// Calculate the offset in 100-nanosecond intervals since FILETIME epoch (1601) to Unix epoch (1970)
		int64_t unixTimeStartInFileTime = 116444736000000000LL; // 1970 - 1601

		if (unixTimeStartInFileTime > intervalSinceFileTimeEpoch)
			throw std::runtime_error("Input Date is not a valid day after 1970: " + std::to_string(nYear) + "/" + std::to_string(nMonth) + "/" + std::to_string(nDay));

		int64_t nMicrosecondsSinceUnixEpoch = (intervalSinceFileTimeEpoch - unixTimeStartInFileTime) / 10;

		return (uint64_t)nMicrosecondsSinceUnixEpoch;


#else
		throw std::runtime_error("date time support not implemented");
#endif
	}


	void CChrono::parseDateFromMicrosecondsSince1970WithWeekday(const uint64_t nMicrosecondsSince1970, uint32_t& nYear, uint32_t& nMonth, uint32_t& nDay, uint32_t& nDayOfTheWeek)
	{
#ifdef _WIN32
		// Calculate the offset in 100-nanosecond intervals since FILETIME epoch (1601) to Unix epoch (1970)
		int64_t unixTimeStartInFileTime = 116444736000000000LL; // 1970 - 1601

		// Convert microseconds since 1970 epoch to 100-nanosecond intervals since 1601
		long long filetime_intervals_since_1601 = nMicrosecondsSince1970 * 10 + unixTimeStartInFileTime;

		// Convert to FILETIME
		FILETIME ft;
		ft.dwLowDateTime = static_cast<DWORD>(filetime_intervals_since_1601);
		ft.dwHighDateTime = static_cast<DWORD>(filetime_intervals_since_1601 >> 32);

		// Convert FILETIME to SYSTEMTIME
		SYSTEMTIME st;
		if (!FileTimeToSystemTime(&ft, &st)) {
			throw std::runtime_error("could not convert date time stamp properly: " + std::to_string(nMicrosecondsSince1970));
		}

		// SYSTEMTIME structure contains the date broken down to year, month, day, etc.
		nYear = st.wYear;
		nMonth = st.wMonth;
		nDay = st.wDay;

		nDayOfTheWeek = st.wDayOfWeek;
		// Handle Sunday as special case
		if (nDayOfTheWeek == 0)
			nDayOfTheWeek = 7;
#else
		throw std::runtime_error("date time support not implemented");
#endif
	}


	void CChrono::parseDateFromMicrosecondsSince1970(const uint64_t nMicrosecondsSince1970, uint32_t& nYear, uint32_t& nMonth, uint32_t& nDay)
	{
		uint32_t nWeekday = 0;
		parseDateFromMicrosecondsSince1970WithWeekday(nMicrosecondsSince1970, nYear, nMonth, nDay, nWeekday);
	}

	void CChrono::parseDateTimeFromMicrosecondsSince1970(const uint64_t nMicrosecondsSince1970, uint32_t& nYear, uint32_t& nMonth, uint32_t& nDay, uint32_t& nHour, uint32_t& nMinute, uint32_t& nSecond, uint32_t & nMicrosecond)
	{
		parseDateFromMicrosecondsSince1970(nMicrosecondsSince1970, nYear, nMonth, nDay);
		nMicrosecond = (uint32_t)(nMicrosecondsSince1970 % 1000000ULL);
		nSecond = (uint32_t)((nMicrosecondsSince1970 / 1000000ULL) % 60ULL);
		nMinute = (uint32_t)((nMicrosecondsSince1970 / (1000000ULL * 60ULL)) % 60ULL);
		nHour = (uint32_t)((nMicrosecondsSince1970 / (1000000ULL * 3600ULL)) % 24ULL);
	}

	uint64_t CChrono::getMicrosecondsSince1970FromDateTime(uint32_t nYear, uint32_t nMonth, uint32_t nDay, uint32_t nHour, uint32_t nMinute, uint32_t nSecond, uint32_t nMicrosecond)
	{

		if ((nYear < 1970) || (nMonth < 1) || (nMonth > 12) || (nDay < 1) || (nDay > 31))
			throw std::runtime_error("Invalid date: " + std::to_string(nYear) + "/" + std::to_string(nMonth) + "/" + std::to_string(nDay));

		if ((nHour > 23) || (nMinute > 59) || (nSecond > 59) || (nMicrosecond > 999999))
			throw std::runtime_error("Invalid time: " + std::to_string(nHour) + ":" + std::to_string(nMinute) + ":" + std::to_string(nSecond) + "." + std::to_string(nMicrosecond));
		uint64_t nStartOfDay = getMicrosecondsSince1970FromDay(nYear, nMonth, nDay);
		uint64_t nSecondOfDay = ((uint64_t)nHour) * 3600 + ((uint64_t)nMinute) * 60 + (uint64_t)nSecond;

		return nStartOfDay + nSecondOfDay * 1000000ULL + (uint64_t)nMicrosecond;
	}


	uint64_t CChrono::getCurrentUTCTimeInternal() {

#ifdef _WIN32
		SYSTEMTIME utcTime;

		// Retrieve the current UTC system time
		GetSystemTime(&utcTime);

		return getMicrosecondsSince1970FromDateTime(utcTime.wYear, utcTime.wMonth, utcTime.wDay, utcTime.wHour, utcTime.wMinute, utcTime.wSecond, ((uint32_t)utcTime.wMilliseconds) * 1000);
#else
		std::time_t t = std::time(nullptr);
		utc_time = *std::localtime(&t);
		throw std::runtime_error("date time support not implemented");

#endif


	}


	bool CChrono::yearIsLeapYear(uint32_t nYear)
	{
		if (nYear % 100 == 0) {
			return ((nYear % 400) == 0);
		}
		else {
			return ((nYear % 4) == 0);
		}

	}


}