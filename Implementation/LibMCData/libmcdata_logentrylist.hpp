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


Abstract: This is the class declaration of CLogEntryList

*/


#ifndef __LIBMCDATA_LOGENTRYLIST
#define __LIBMCDATA_LOGENTRYLIST

#include "libmcdata_interfaces.hpp"

// Parent classes
#include "libmcdata_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include <vector>
#include <map>


namespace LibMCData {
namespace Impl {

/*************************************************************************************************************************
 Class declaration of CLogEntryList 
**************************************************************************************************************************/

class CLogEntryListEntry {
private:
    uint32_t m_nLogID;
    eLogLevel m_logLevel;
    std::string m_sTimeStamp;
    std::string m_sSubSystem;
    std::string m_sMessage;

public:
    CLogEntryListEntry(uint32_t nLogID, eLogLevel logLevel, const std::string& sTimeStamp, const std::string& sSubSystem, const std::string& sMessage);

    virtual ~CLogEntryListEntry();

    uint32_t getLogID();

    eLogLevel getLogLevel();

    std::string getTimeStamp();

    std::string getSubSystem();

    std::string getMessage();

};

typedef std::shared_ptr<CLogEntryListEntry> PLogEntryListEntry;

class CLogEntryList : public virtual ILogEntryList, public virtual CBase {
protected:

    std::vector<PLogEntryListEntry> m_Entries;
    std::map<uint32_t, PLogEntryListEntry> m_EntryMap;

public:

    CLogEntryList();

    virtual ~CLogEntryList();

	LibMCData_uint32 Count() override;

	void GetEntryByIndex(const LibMCData_uint32 nIndex, LibMCData_uint32 & nID, std::string & sMessage, std::string & sSubSystem, LibMCData::eLogLevel & eLogLevel, std::string & sTimestamp) override;

	void GetEntryByID(const LibMCData_uint32 nID, std::string & sMessage, std::string & sSubSystem, LibMCData::eLogLevel & eLogLevel, std::string & sTimestamp) override;

	bool HasEntry(const LibMCData_uint32 nID) override;

    void addEntry(const LibMCData_uint32 nID, const std::string& sMessage, const std::string& sSubSystem, LibMCData::eLogLevel eLogLevel, const std::string& sTimestamp);

};

} // namespace Impl
} // namespace LibMCData

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDATA_LOGENTRYLIST
