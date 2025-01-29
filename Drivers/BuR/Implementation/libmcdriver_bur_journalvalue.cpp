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

#include "libmcdriver_bur_journalvalue.hpp"
#include "libmcdriver_bur_interfaceexception.hpp"

#define BURJOURNAL_MAXNAMELENGTH 256

using namespace LibMCDriver_BuR::Impl;

CDriver_BuRJournalValue::CDriver_BuRJournalValue(uint32_t nValueID, const std::string& sValueName, const std::string& sValueType, uint32_t nValueSize)
    : m_nValueID (nValueID), m_sValueName (sValueName), m_sValueType (sValueType), m_nValueSize (nValueSize)
{

}

CDriver_BuRJournalValue::~CDriver_BuRJournalValue()
{

}

uint32_t CDriver_BuRJournalValue::getValueID()
{
    return m_nValueID;
}

std::string CDriver_BuRJournalValue::getValueName()
{
    return m_sValueName;
}

std::string CDriver_BuRJournalValue::getValueType()
{
    return m_sValueType;
}

uint32_t CDriver_BuRJournalValue::getValueSize()
{
    return m_nValueSize;
}

bool CDriver_BuRJournalValue::nameIsValidJournalName(const std::string& sName)
{
    if (sName.empty())        
        return false;

    if (sName.length () > BURJOURNAL_MAXNAMELENGTH)

    for (auto ch : sName) {
        if (!(isalnum(ch) || (ch == '_') || (ch == '-')))
            return false;
    }

    return true;
}

std::string CDriver_BuRJournalValue::getDriverParameterName()
{
    return m_sDriverParameterName;

}

void CDriver_BuRJournalValue::setDriverParameterName(const std::string& sDriverParameterName)
{
    m_sDriverParameterName = sDriverParameterName;
}
