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


Abstract: This is the class declaration of CDataSeries

*/


#ifndef __LIBMCENV_DATASERIES
#define __LIBMCENV_DATASERIES

#include "libmcenv_interfaces.hpp"

// Parent classes
#include "libmcenv_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "amc_dataseries.hpp"

namespace LibMCEnv {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CDataSeries 
**************************************************************************************************************************/

class CDataSeries : public virtual IDataSeries, public virtual CBase {
private:
	AMC::PDataSeries m_pDataSeries;

public:

	CDataSeries(AMC::PDataSeries pDataSeries);

	virtual ~CDataSeries();

	std::string GetName() override;

	std::string GetUUID() override;

	void Clear() override;

	bool IsEmpty() override;

	LibMCEnv_uint64 GetMinimum() override;

	LibMCEnv_uint64 GetMaximum() override;

	void GetAllEntries(LibMCEnv_uint64 nEntryArrayBufferSize, LibMCEnv_uint64* pEntryArrayNeededCount, LibMCEnv::sTimeStreamEntry * pEntryArrayBuffer) override;

	void SetAllEntries(const LibMCEnv_uint64 nEntryArrayBufferSize, const LibMCEnv::sTimeStreamEntry * pEntryArrayBuffer) override;

	void SampleJournalVariable(IJournalVariable* pJournalVariable, const LibMCEnv_uint64 nStartTimeStamp, const LibMCEnv_uint64 nEndTimeStamp, const LibMCEnv_uint32 nNumberOfSamples) override;
	
	LibMCEnv_uint32 GetVersion() override;

	void IncreaseVersion() override;

};

} // namespace Impl
} // namespace LibMCEnv

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCENV_DATASERIES
