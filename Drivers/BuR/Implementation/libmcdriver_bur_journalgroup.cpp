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

*/

#include "libmcdriver_bur_journalgroup.hpp"
#include "libmcdriver_bur_interfaceexception.hpp"

using namespace LibMCDriver_BuR::Impl;

CDriver_BuRJournalGroup::CDriver_BuRJournalGroup(uint32_t nGroupID, const std::string& sGroupName)
    : m_nGroupID (nGroupID), m_sGroupName (sGroupName)
{

}

CDriver_BuRJournalGroup::~CDriver_BuRJournalGroup()
{

}

uint32_t CDriver_BuRJournalGroup::getGroupID()
{
    return m_nGroupID;
}

std::string CDriver_BuRJournalGroup::getGroupName()
{
    return m_sGroupName;
}

uint32_t CDriver_BuRJournalGroup::getValueCount()
{
    return (uint32_t)m_JournalValues.size();
}

PDriver_BuRJournalValue CDriver_BuRJournalGroup::getValueByIndex(uint32_t nIndex)
{
    if ((size_t)nIndex >= m_JournalValues.size())
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_INVALIDJOURNALVALUEINDEX);

    return m_JournalValues.at(nIndex);
}

PDriver_BuRJournalValue CDriver_BuRJournalGroup::findValueByID(uint32_t nValueID, bool bFailIfNotExisting)
{
    auto iIter = m_JournalValueMap.find(nValueID);
    if (iIter != m_JournalValueMap.end())
        return iIter->second;

    if (bFailIfNotExisting)
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALVALUEIDNOTFOUND, "journal value ID not found: " + std::to_string (nValueID));

    return nullptr;
}

PDriver_BuRJournalValue CDriver_BuRJournalGroup::findValueByName(const std::string& sName, bool bFailIfNotExisting)
{
    auto iIter = m_JournalValueNameMap.find(sName);
    if (iIter != m_JournalValueNameMap.end())
        return iIter->second;

    if (bFailIfNotExisting)
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALVALUENAMENOTFOUND, "journal value name not found: " + sName);

    return nullptr;

}

PDriver_BuRJournalValue CDriver_BuRJournalGroup::addValue(uint32_t nValueID, const std::string& sValueName, const std::string& sValueType, uint32_t nValueSize)
{
    auto iIter = m_JournalValueMap.find(nValueID);
    if (iIter != m_JournalValueMap.end())
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_DUPLICATEJOURNALVALUEID, "duplicate journal value ID: " + std::to_string(nValueID));

    auto pValue = std::make_shared<CDriver_BuRJournalValue>(nValueID, sValueName, sValueType, nValueSize);
    m_JournalValues.push_back(pValue);
    m_JournalValueMap.insert(std::make_pair (nValueID, pValue));
    m_JournalValueNameMap.insert(std::make_pair (sValueName, pValue));

    return pValue;
}

