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


Abstract: This is a stub class definition of CLogEntryList

*/

#include "libmcdata_logentrylist.hpp"
#include "libmcdata_interfaceexception.hpp"

// Include custom headers here.


using namespace LibMCData::Impl;

/*************************************************************************************************************************
 Class definition of CLogEntryList 
**************************************************************************************************************************/

CLogEntryListEntry::CLogEntryListEntry(uint32_t nLogID, eLogLevel logLevel, const std::string& sTimeStamp, const std::string& sSubSystem, const std::string& sMessage)
    : m_nLogID (nLogID), m_logLevel (logLevel), m_sTimeStamp (sTimeStamp), m_sSubSystem (sSubSystem), m_sMessage (sMessage)
{

}

CLogEntryListEntry::~CLogEntryListEntry()
{

}

uint32_t CLogEntryListEntry::getLogID()
{
    return m_nLogID;
}

LibMCData::eLogLevel CLogEntryListEntry::getLogLevel()
{
    return m_logLevel;
}

std::string CLogEntryListEntry::getTimeStamp()
{
    return m_sTimeStamp;
}

std::string CLogEntryListEntry::getSubSystem()
{
    return m_sSubSystem;
}

std::string CLogEntryListEntry::getMessage()
{
    return m_sMessage;
}

CLogEntryList::CLogEntryList()
{

}


CLogEntryList::~CLogEntryList()
{

}


LibMCData_uint32 CLogEntryList::Count()
{
    return (uint32_t)m_Entries.size();
}

void CLogEntryList::GetEntryByIndex(const LibMCData_uint32 nIndex, LibMCData_uint32 & nID, std::string & sMessage, std::string & sSubSystem, LibMCData::eLogLevel & eLogLevel, std::string & sTimestamp)
{
    if (nIndex >= m_Entries.size())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDINDEX);

    auto pEntry = m_Entries.at(nIndex);
    nID = pEntry->getLogID();
    sMessage = pEntry->getMessage();
    sSubSystem = pEntry->getSubSystem();
    eLogLevel = pEntry->getLogLevel();
    sTimestamp = pEntry->getTimeStamp();

}

void CLogEntryList::GetEntryByID(const LibMCData_uint32 nID, std::string & sMessage, std::string & sSubSystem, LibMCData::eLogLevel & eLogLevel, std::string & sTimestamp)
{
    auto iIter = m_EntryMap.find(nID);
    if (iIter == m_EntryMap.end ())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_COULDNOTFINDLOGENTRY);

    auto pEntry = iIter->second;
    sMessage = pEntry->getMessage();
    sSubSystem = pEntry->getSubSystem();
    eLogLevel = pEntry->getLogLevel();
    sTimestamp = pEntry->getTimeStamp();

}

bool CLogEntryList::HasEntry(const LibMCData_uint32 nID)
{
    auto iIter = m_EntryMap.find(nID);
    return (iIter != m_EntryMap.end());
}

void CLogEntryList::addEntry(const LibMCData_uint32 nID, const std::string& sMessage, const std::string& sSubSystem, LibMCData::eLogLevel eLogLevel, const std::string& sTimestamp)
{
    auto pEntry = std::make_shared<CLogEntryListEntry> (nID, eLogLevel, sTimestamp, sSubSystem, sMessage);
    m_Entries.push_back(pEntry);
    m_EntryMap.insert(std::make_pair (nID, pEntry));

}
