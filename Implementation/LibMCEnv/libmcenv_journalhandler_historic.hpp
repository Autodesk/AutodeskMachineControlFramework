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


Abstract: This is the class declaration of CJournalHandler

*/


#ifndef __LIBMCENV_JOURNALHANDLER_HISTORIC
#define __LIBMCENV_JOURNALHANDLER_HISTORIC

#include "libmcenv_interfaces.hpp"

// Parent classes
#include "libmcenv_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

#include "libmcdata_dynamic.hpp"
#include <mutex>

namespace LibMCEnv {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CJournalHandler_Historic 
**************************************************************************************************************************/

class CJournalHandler_Historic : public virtual IJournalHandler, public virtual CBase {
protected:

    std::mutex m_JournalReaderMutex;
    LibMCData::PJournalReader m_pJournalReader;

public:

    CJournalHandler_Historic(LibMCData::PJournalReader pJournalReader);

    virtual ~CJournalHandler_Historic();

	IJournalVariable * RetrieveJournalVariable(const std::string & sVariableName, const LibMCEnv_uint64 nTimeDeltaInMicroseconds) override;

	IJournalVariable * RetrieveJournalVariableFromTimeInterval(const std::string & sVariableName, const LibMCEnv_uint64 nStartTimeInMicroseconds, const LibMCEnv_uint64 nEndTimeInMicroseconds) override;

    IDateTime* GetStartTime() override;

	ILogEntryList* RetrieveLogEntries(const LibMCEnv_uint64 nTimeDeltaInMicroseconds, LibMCEnv::eLogLevel& eMinLogLevel) override;

	ILogEntryList* RetrieveLogEntriesFromTimeInterval(const LibMCEnv_uint64 nStartTimeInMicroseconds, const LibMCEnv_uint64 nEndTimeInMicroseconds, LibMCEnv::eLogLevel& eMinLogLevel) override;

	IAlertIterator* RetrieveAlerts(const LibMCEnv_uint64 nTimeDeltaInMicroseconds) override;

	IAlertIterator* RetrieveAlertsFromTimeInterval(const LibMCEnv_uint64 nStartTimeInMicroseconds, const LibMCEnv_uint64 nEndTimeInMicroseconds) override;
};

} // namespace Impl
} // namespace LibMCEnv

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCENV_JOURNALHANDLER_HISTORIC
