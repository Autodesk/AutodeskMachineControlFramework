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


Abstract: This is the class declaration of CDateTime

*/


#ifndef __LIBMCENV_DATETIME
#define __LIBMCENV_DATETIME

#include "libmcenv_interfaces.hpp"

// Parent classes
#include "libmcenv_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include <cstdint>
#include <memory>


namespace LibMCEnv {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CDateTime 
**************************************************************************************************************************/

class CDateTime;
typedef std::shared_ptr<CDateTime> PDateTime;

class CDateTime : public virtual IDateTime, public virtual CBase {
private:

	uint64_t m_nMicrosecondsSince1900;

public:

	static CDateTime * makefromUTC (const std::string & sUTCTime);
	static PDateTime makeSharedfromUTC(const std::string & sUTCTime);

	CDateTime(uint64_t nMicrosecondsSince1900);

	virtual ~CDateTime ();

	LibMCEnv_uint64 ToMicrosecondsSince1900() override;

	LibMCEnv_uint64 ToUnixTimestamp() override;

	std::string ToUTCDateTime() override;

	std::string ToUTCDateTimeInMilliseconds() override;

	std::string ToUTCDateTimeInMicroseconds() override;

	void GetDate(LibMCEnv_uint32 & nYear, LibMCEnv_uint32 & nMonth, LibMCEnv_uint32 & nDay) override;

	void GetTime(LibMCEnv_uint32 & nHour, LibMCEnv_uint32 & nMinute, LibMCEnv_uint32 & nSecond, LibMCEnv_uint32 & nMicrosecond) override;

	IDateTime * Duplicate() override;

	bool IsLeapYear() override;

	bool IsLaterThan(IDateTime* pOtherTimeStamp) override;

	bool IsEarlierThan(IDateTime* pOtherTimeStamp) override;

	bool IsEqualTo(IDateTime* pOtherTimeStamp) override;

	void GetTimeDifference(IDateTime* pOtherTimeStamp, IDateTimeDifference* pDifference) override;

	void AddDuration(IDateTimeDifference* pDuration) override;

	void SubtractDuration(IDateTimeDifference* pDuration) override;

	void ShiftByYears(const LibMCEnv_int64 nDeltaYears) override;

	void ShiftByDays(const LibMCEnv_int64 nDeltaDays) override;

	void ShiftByHours(const LibMCEnv_int64 nDeltaHours) override;

	void ShiftByMinutes(const LibMCEnv_int64 nDeltaMinutes) override;

	void ShiftBySeconds(const LibMCEnv_int64 nDeltaSeconds) override;

	void ShiftByMilliseconds(const LibMCEnv_int64 nDeltaMilliseconds) override;

	void ShiftByMicroseconds(const LibMCEnv_int64 nDeltaMicroseconds) override;

	void RoundDownToYear() override;

	void RoundDownToMonth() override;

	void RoundDownToDay() override;

	void RoundDownToHour() override;

	void RoundDownToMinute() override;

	void RoundDownToSeconds() override;

	void RoundDownToMilliseconds() override;

	void RoundUpToYear() override;

	void RoundUpToMonth() override;

	void RoundUpToDay() override;

	void RoundUpToHour() override;

	void RoundUpToMinute() override;

	void RoundUpToSeconds() override;

	void RoundUpToMilliseconds() override;

};

} // namespace Impl
} // namespace LibMCEnv

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCENV_DATETIME
