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


Abstract: This is a stub class definition of CDateTime

*/

#include "libmcenv_datetime.hpp"
#include "libmcenv_interfaceexception.hpp"

#include "common_chrono.hpp"

using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CDateTime 
**************************************************************************************************************************/

CDateTime* CDateTime::makefromUTC(const std::string& sUTCTimeString)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}


CDateTime::CDateTime(const uint64_t nMicrosecondsSince1970)
	: m_nMicrosecondsSince1970 (nMicrosecondsSince1970)
{

}

CDateTime::~CDateTime()
{

}

LibMCEnv_uint64 CDateTime::ToMicrosecondsSince1970()
{
	return m_nMicrosecondsSince1970;
}

LibMCEnv_uint64 CDateTime::ToUnixTimestamp()
{
	return m_nMicrosecondsSince1970 / 1000000;
}

std::string CDateTime::ToUTCDateTime()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

std::string CDateTime::ToUTCDateTimeInMilliseconds()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

std::string CDateTime::ToUTCDateTimeInMicroseconds()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CDateTime::GetDate(LibMCEnv_uint32 & nYear, LibMCEnv_uint32 & nMonth, LibMCEnv_uint32 & nDay)
{
	
	uint32_t nDayOfTheWeek = 0;
	AMCCommon::CChrono::parseDateFromMicrosecondsSince1970(m_nMicrosecondsSince1970, nYear, nMonth, nDay, nDayOfTheWeek);
}

void CDateTime::GetTime(LibMCEnv_uint32 & nHour, LibMCEnv_uint32 & nMinute, LibMCEnv_uint32 & nSecond, LibMCEnv_uint32 & nMicrosecond)
{
	uint64_t nMicroSecondsOfTheDay = (m_nMicrosecondsSince1970 % (24ULL * 3600ULL * 1000000ULL));
	uint64_t nSecondsOfTheDay = nMicroSecondsOfTheDay / 1000000ULL;
	uint64_t nMinutesOfTheDay = nSecondsOfTheDay / 60;
	uint32_t nHour = (uint32_t) (nMinutesOfTheDay / 60);
	nMinute = (uint32_t)(nMinutesOfTheDay % 60);
	nSecond = (uint32_t)(nSecondsOfTheDay % 60);
	nMicrosecond = (uint32_t)(nMicroSecondsOfTheDay % 1000000ULL);
}

IDateTime * CDateTime::Duplicate()
{
	return new CDateTime(m_nMicrosecondsSince1970);
}

bool CDateTime::IsLeapYear()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

bool CDateTime::IsLaterThan(IDateTime* pOtherTimeStamp)
{
	if (pOtherTimeStamp == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

	return (m_nMicrosecondsSince1970 > pOtherTimeStamp->ToMicrosecondsSince1970());
}

bool CDateTime::IsEarlierThan(IDateTime* pOtherTimeStamp)
{
	if (pOtherTimeStamp == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

	return (m_nMicrosecondsSince1970 < pOtherTimeStamp->ToMicrosecondsSince1970());
}

bool CDateTime::IsEqualTo(IDateTime* pOtherTimeStamp)
{
	if (pOtherTimeStamp == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

	return (m_nMicrosecondsSince1970 == pOtherTimeStamp->ToMicrosecondsSince1970());
}

IDateTimeDifference* CDateTime::GetTimeDifference(IDateTime* pOtherTimeStamp)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CDateTime::AddDuration(IDateTimeDifference* pDuration)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CDateTime::SubtractDuration(IDateTimeDifference* pDuration)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CDateTime::ShiftByYears(const LibMCEnv_int64 nDeltaYears)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CDateTime::ShiftByDays(const LibMCEnv_int64 nDeltaDays)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CDateTime::ShiftByHours(const LibMCEnv_int64 nDeltaHours)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CDateTime::ShiftByMinutes(const LibMCEnv_int64 nDeltaMinutes)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CDateTime::ShiftBySeconds(const LibMCEnv_int64 nDeltaSeconds)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CDateTime::ShiftByMilliseconds(const LibMCEnv_int64 nDeltaMilliseconds)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CDateTime::ShiftByMicroseconds(const LibMCEnv_int64 nDeltaMicroseconds)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CDateTime::RoundDownToYear()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CDateTime::RoundDownToMonth()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CDateTime::RoundDownToDay()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CDateTime::RoundDownToHour()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CDateTime::RoundDownToMinute()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CDateTime::RoundDownToSeconds()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CDateTime::RoundDownToMilliseconds()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CDateTime::RoundUpToYear()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CDateTime::RoundUpToMonth()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CDateTime::RoundUpToDay()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CDateTime::RoundUpToHour()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CDateTime::RoundUpToMinute()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CDateTime::RoundUpToSeconds()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CDateTime::RoundUpToMilliseconds()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

