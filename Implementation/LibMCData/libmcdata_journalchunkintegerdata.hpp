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


Abstract: This is the class declaration of CJournalChunkIntegerData

*/


#ifndef __LIBMCDATA_JOURNALCHUNKINTEGERDATA
#define __LIBMCDATA_JOURNALCHUNKINTEGERDATA

#include "libmcdata_interfaces.hpp"

// Parent classes
#include "libmcdata_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.


namespace LibMCData {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CJournalChunkIntegerData 
**************************************************************************************************************************/


class CJournalChunkIntegerData : public virtual IJournalChunkIntegerData, public virtual CBase {
private:

	uint32_t m_nChunkIndex;

	uint64_t m_nStartTimeStamp;

	uint64_t m_nEndTimeStamp;

	std::vector<LibMCData::sJournalChunkVariableInfo> m_VariableInfo;

	std::vector<uint32_t> m_TimeStamps;

	std::vector<int64_t> m_ValueData;

public:

	CJournalChunkIntegerData(uint32_t nChunkIndex);

	virtual ~CJournalChunkIntegerData();

	LibMCData_uint32 GetChunkIndex() override;

	LibMCData_uint64 GetStartTimeStamp() override;

	LibMCData_uint64 GetEndTimeStamp() override;

	void GetVariableInfo(LibMCData_uint64 nVariableInfoBufferSize, LibMCData_uint64* pVariableInfoNeededCount, LibMCData::sJournalChunkVariableInfo * pVariableInfoBuffer) override;

	void GetTimeStampData(LibMCData_uint64 nTimeStampDataBufferSize, LibMCData_uint64* pTimeStampDataNeededCount, LibMCData_uint32 * pTimeStampDataBuffer) override;

	void GetValueData(LibMCData_uint64 nValueDataBufferSize, LibMCData_uint64* pValueDataNeededCount, LibMCData_int64 * pValueDataBuffer) override;

	std::vector<LibMCData::sJournalChunkVariableInfo> &getVariableInfoInternal();

	std::vector<uint32_t>& getTimeStampsInternal ();

	std::vector<int64_t>& getValueDataInternal();

	void setTimeInterval(uint64_t nStartTimeStamp, uint64_t nEndTimeStamp);

};

} // namespace Impl
} // namespace LibMCData

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDATA_JOURNALCHUNKINTEGERDATA
