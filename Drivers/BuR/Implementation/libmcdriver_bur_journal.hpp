/*++

Copyright (C) 2024 Autodesk Inc.

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


Abstract: This is the class declaration of CPLCCommand

*/


#ifndef __LIBMCDRIVER_BUR_JOURNAL
#define __LIBMCDRIVER_BUR_JOURNAL

#include "libmcdriver_bur_journalgroup.hpp"
#include "libmcenv_dynamic.hpp"

#define BUR_JOURNALSCHEMA_MINVALUEID 1
#define BUR_JOURNALSCHEMA_MAXVALUEID 32768
#define BUR_JOURNALSCHEMA_MINGROUPID 1
#define BUR_JOURNALSCHEMA_MAXGROUPID 32768
#define BUR_JOURNALSCHEMA_MINVALUESIZE 1
#define BUR_JOURNALSCHEMA_MAXVALUESIZE 16384

namespace LibMCDriver_BuR {
namespace Impl {


    class CDriver_BuRJournalParameterValue {
    private:
        std::string m_sName;
        std::string m_sGroupName;
        std::string m_sValueName;
    public:

        CDriver_BuRJournalParameterValue(const std::string& sName, const std::string& sGroupName, const std::string& sValueName);

        virtual ~CDriver_BuRJournalParameterValue();

        std::string getName();
        std::string getGroupName();
        std::string getValueName();

    };

    typedef std::shared_ptr<CDriver_BuRJournalParameterValue> PDriver_BuRJournalParameterValue;


class CDriver_BuRJournal {
private:

    std::map<uint32_t, PDriver_BuRJournalGroup> m_JournalGroupMap;
    std::map<std::string, PDriver_BuRJournalGroup> m_JournalGroupNameMap;

public:

    CDriver_BuRJournal(const std::string & sSchemaJSON);

    virtual ~CDriver_BuRJournal();

    PDriver_BuRJournalGroup findGroupByID(uint32_t nGroupID, bool bFailIfNotExisting);

    PDriver_BuRJournalGroup findGroupByName(const std::string & sName, bool bFailIfNotExisting);

    void parseStatus (const std::vector<uint8_t> & buffer, LibMCEnv::PDriverStatusUpdateSession pDriverUpdateInstance);

    void registerParameterValue (CDriver_BuRJournalParameterValue * pParameterValue);
};

typedef std::shared_ptr<CDriver_BuRJournal> PDriver_BuRJournal;


} // namespace Impl
} // namespace LibMCDriver_BuR

#endif // __LIBMCDRIVER_BUR_JOURNAL
