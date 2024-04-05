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

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL AUTODESK INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


Abstract: This is a stub class definition of CDateTimeDifference

*/

#include "libmcenv_datetimedifference.hpp"
#include "libmcenv_interfaceexception.hpp"

// Include custom headers here.
#include "common_chrono.hpp"
#include "amc_constants.hpp"


using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CDateTimeDifference 
**************************************************************************************************************************/

CDateTimeDifference::CDateTimeDifference(uint64_t nDateTimeDifferenceInMicroseconds)
	: m_nDateTimeDifferenceInMicroseconds (nDateTimeDifferenceInMicroseconds)
{
	if (!AMCCommon::CChrono::timeStampIsWithinAMillionYears(nDateTimeDifferenceInMicroseconds))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_DATETIMEDIFFERENCEISINVALID, std::to_string (nDateTimeDifferenceInMicroseconds));
}

CDateTimeDifference::~CDateTimeDifference()
{

}

LibMCEnv_uint64 CDateTimeDifference::ToMicroseconds()
{
	return m_nDateTimeDifferenceInMicroseconds;
}

LibMCEnv_uint64 CDateTimeDifference::ToMilliseconds()
{
	return m_nDateTimeDifferenceInMicroseconds / MICROSECONDS_PER_MILLISECOND;
}

LibMCEnv_uint64 CDateTimeDifference::ToSeconds()
{
	return m_nDateTimeDifferenceInMicroseconds / MICROSECONDS_PER_SECOND;
}

LibMCEnv_uint64 CDateTimeDifference::ToMinutes()
{
	return m_nDateTimeDifferenceInMicroseconds / MICROSECONDS_PER_MINUTE;
}

LibMCEnv_uint64 CDateTimeDifference::ToHours()
{
	return m_nDateTimeDifferenceInMicroseconds / MICROSECONDS_PER_HOUR;
}

LibMCEnv_uint64 CDateTimeDifference::ToDays()
{
	return m_nDateTimeDifferenceInMicroseconds / MICROSECONDS_PER_DAY;
}

void CDateTimeDifference::RoundDownToDay()
{
	m_nDateTimeDifferenceInMicroseconds = ToDays() * MICROSECONDS_PER_DAY;
}

void CDateTimeDifference::RoundDownToHour()
{
	m_nDateTimeDifferenceInMicroseconds = ToHours() * MICROSECONDS_PER_HOUR;
}

void CDateTimeDifference::RoundDownToMinute()
{
	m_nDateTimeDifferenceInMicroseconds = ToMinutes() * MICROSECONDS_PER_MINUTE;
}

void CDateTimeDifference::RoundDownToSeconds()
{
	m_nDateTimeDifferenceInMicroseconds = ToSeconds() * MICROSECONDS_PER_SECOND;
}

void CDateTimeDifference::RoundDownToMilliseconds()
{
	m_nDateTimeDifferenceInMicroseconds = ToSeconds() * MICROSECONDS_PER_MILLISECOND;
}

void CDateTimeDifference::RoundUpToDay()
{
	if ((m_nDateTimeDifferenceInMicroseconds % MICROSECONDS_PER_DAY) != 0ULL) {
		m_nDateTimeDifferenceInMicroseconds = (ToDays() + 1) * MICROSECONDS_PER_DAY;
	}
		
}

void CDateTimeDifference::RoundUpToHour()
{
	if ((m_nDateTimeDifferenceInMicroseconds % MICROSECONDS_PER_HOUR) != 0ULL) {
		m_nDateTimeDifferenceInMicroseconds = (ToHours() + 1) * MICROSECONDS_PER_HOUR;
	}
}

void CDateTimeDifference::RoundUpToMinute()
{
	if ((m_nDateTimeDifferenceInMicroseconds % MICROSECONDS_PER_MINUTE) != 0ULL) {
		m_nDateTimeDifferenceInMicroseconds = (ToMinutes() + 1) * MICROSECONDS_PER_MINUTE;
	}
}

void CDateTimeDifference::RoundUpToSeconds()
{
	if ((m_nDateTimeDifferenceInMicroseconds % MICROSECONDS_PER_SECOND) != 0ULL) {
		m_nDateTimeDifferenceInMicroseconds = (ToSeconds() + 1) * MICROSECONDS_PER_SECOND;
	}
}

void CDateTimeDifference::RoundupToMilliseconds()
{
	if ((m_nDateTimeDifferenceInMicroseconds % MICROSECONDS_PER_SECOND) != 0ULL) {
		m_nDateTimeDifferenceInMicroseconds = (ToMilliseconds() + 1) * MICROSECONDS_PER_SECOND;
	}
}

