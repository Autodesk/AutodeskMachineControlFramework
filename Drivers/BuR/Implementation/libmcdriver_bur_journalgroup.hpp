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


#ifndef __LIBMCDRIVER_BUR_JOURNALGROUP
#define __LIBMCDRIVER_BUR_JOURNALGROUP

#include "libmcdriver_bur_journalvalue.hpp"


#include <memory>
#include <string>
#include <vector>
#include <map>


namespace LibMCDriver_BuR {
namespace Impl {

	class CDriver_BuRJournalGroup {
	private:
		uint32_t m_nGroupID;
		std::string m_sGroupName;

		std::vector<PDriver_BuRJournalValue> m_JournalValues;
		std::map<uint32_t, PDriver_BuRJournalValue> m_JournalValueMap;
		std::map<std::string, PDriver_BuRJournalValue> m_JournalValueNameMap;

	public:

		CDriver_BuRJournalGroup(uint32_t nGroupID, const std::string & sGroupName);

		virtual ~CDriver_BuRJournalGroup();

		uint32_t getGroupID ();

		std::string getGroupName ();

		uint32_t getValueCount();

		PDriver_BuRJournalValue getValueByIndex (uint32_t nIndex);

		PDriver_BuRJournalValue findValueByID(uint32_t nValueID, bool bFailIfNotExisting);

		PDriver_BuRJournalValue findValueByName(const std::string & sName, bool bFailIfNotExisting);

		PDriver_BuRJournalValue addValue(uint32_t nValueID, const std::string& sValueName, const std::string& sValueType, uint32_t nValueSize);

	};

	typedef std::shared_ptr<CDriver_BuRJournalGroup> PDriver_BuRJournalGroup;


} // namespace Impl
} // namespace LibMCDriver_BuR

#endif // __LIBMCDRIVER_BUR_JOURNALGROUP
