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

#include "libmcdriver_bur_journal.hpp"
#include "libmcdriver_bur_interfaceexception.hpp"

#include "RapidJSON/rapidjson.h"
#include "RapidJSON/document.h"


using namespace LibMCDriver_BuR::Impl;


#define JOURNALENTRYTYPE_INT32 0x60000000
#define JOURNALENTRYTYPE_DOUBLE 0x90000000
#define JOURNALENTRYTYPE_INT32 0x60000000
#define JOURNALENTRYTYPE_BOOL_TRUE 0xA0000000
#define JOURNALENTRYTYPE_BOOL_FALSE 0xB0000000

#define JOURNALENTRYTYPE_TYPEMASK 0xF0000000
#define JOURNALENTRYTYPE_IDMASK 0x0FFFFFFF


class CJournalStatusParser
{
private:
    const std::vector<uint8_t>& m_Buffer;
    size_t m_nPosition;

public:

    CJournalStatusParser(const std::vector<uint8_t>& buffer)
        : m_Buffer(buffer), m_nPosition(0)
    {

    }

    int32_t readInt32()
    {
        if (m_nPosition + 4 > m_Buffer.size())
            throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_BUFFERTOOSMALL);

        int32_t* pData = (int32_t*)&m_Buffer.at(m_nPosition);
        m_nPosition += 4;

        return *pData;
    }


    uint32_t readUint32()
    {
        if (m_nPosition + 4 > m_Buffer.size())
            throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_BUFFERTOOSMALL);

        uint32_t* pData = (uint32_t*)&m_Buffer.at(m_nPosition);
        m_nPosition += 4;

        return *pData;
    }

    uint32_t readUint8()
    {
        if (m_nPosition + 1 > m_Buffer.size())
            throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_BUFFERTOOSMALL);

        uint8_t* pData = (uint8_t*)&m_Buffer.at(m_nPosition);
        m_nPosition++;

        return *pData;
    }

    double readDouble()
    {
        if (m_nPosition + 8 > m_Buffer.size())
            throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_BUFFERTOOSMALL);

        double* pData = (double*)&m_Buffer.at(m_nPosition);
        m_nPosition += 8;

        return *pData;
    }

};



CDriver_BuRJournalParameterValue::CDriver_BuRJournalParameterValue(const std::string& sName, const std::string& sGroupName, const std::string& sValueName)
    : m_sName (sName), m_sGroupName (sGroupName), m_sValueName (sValueName)
{

}

CDriver_BuRJournalParameterValue::~CDriver_BuRJournalParameterValue()
{

}

std::string CDriver_BuRJournalParameterValue::getName()
{
    return m_sName;
}

std::string CDriver_BuRJournalParameterValue::getGroupName()
{
    return m_sGroupName;
}

std::string CDriver_BuRJournalParameterValue::getValueName()
{
    return m_sValueName;
}


CDriver_BuRJournal::CDriver_BuRJournal(const std::string& sSchemaJSON)
{
    rapidjson::Document document;
    document.Parse(sSchemaJSON.c_str());

    if (!document.IsObject())
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_COULDNOTPARSEBURSCHEMAJSON);

    if (!document.HasMember("schema"))
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALSCHEMAHASNOVERSIONIDENTIFIER);

    if (!document["schema"].IsString())
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALSCHEMAHASINVALIDVERSIONIDENTIFIER);

    std::string sSchemaVersion = document["schema"].GetString();

    if (!document.HasMember("groups"))
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALSCHEMAHASNOGROUPINFORMATION);

    if (!document["groups"].IsArray())
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALSCHEMAHASINVALIDGROUPINFORMATION);

    auto groupArray = document["groups"].GetArray();
    for (auto groupIter = groupArray.Begin(); groupIter != groupArray.End(); groupIter++) {

        auto& group = *groupIter;
        if (!group.HasMember("groupname"))
            throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALSCHEMAMISSESGROUPNAME);
        if (!group.HasMember("groupid"))
            throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALSCHEMAMISSESGROUPID);
        if (!group.HasMember("values"))
            throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALSCHEMAMISSESGROUPVALUES);

        auto& groupNameMember = group["groupname"];
        auto& groupIDMember = group["groupid"];
        auto& groupValuesMember = group["values"];

        if (!(groupNameMember.IsString()))
            throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALSCHEMAHASINVALIDGROUPNAME);
        if (!(groupIDMember.IsInt()))
            throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALSCHEMAHASINVALIDGROUPID);
        if (!(groupValuesMember.IsArray()))
            throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALSCHEMAHASINVALIDGROUPVALUES);

        std::string sGroupName = groupNameMember.GetString();
        int64_t nGroupID = groupIDMember.GetInt64();
        if ((nGroupID < BUR_JOURNALSCHEMA_MINGROUPID) || (nGroupID > BUR_JOURNALSCHEMA_MAXGROUPID))
            throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALSCHEMAHASINVALIDGROUPID, "journal schema has invalid group ID: " + std::to_string(nGroupID));
        if (!CDriver_BuRJournalValue::nameIsValidJournalName(sGroupName))
            throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALSCHEMAHASINVALIDGROUPNAME, "journal schema has invalid group name: " + sGroupName);

        auto iJournalGroupIter = m_JournalGroupMap.find((uint32_t)nGroupID);
        if (iJournalGroupIter != m_JournalGroupMap.end())
            throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_DUPLICATEJOURNALGROUPID, "duplicate journal group: " + std::to_string(nGroupID));

        auto pDriverJournalGroup = std::make_shared<CDriver_BuRJournalGroup>((uint32_t)nGroupID, sGroupName);

        auto groupValues = groupValuesMember.GetArray();
        for (auto valueIter = groupValues.Begin(); valueIter != groupValues.End(); valueIter++) {
            auto& value = *valueIter;
            if (!value.HasMember("type"))
                throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALSCHEMAMISSESVALUETYPE);
            if (!value.HasMember("name"))
                throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALSCHEMAMISSESVALUENAME);
            if (!value.HasMember("id"))
                throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALSCHEMAMISSESVALUEID);
            if (!value.HasMember("size"))
                throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALSCHEMAMISSESVALUESIZE);

            auto& valueTypeMember = value["type"];
            auto& valueNameMember = value["name"];
            auto& valueIDMember = value["id"];
            auto& valueSizeMember = value["size"];

            if (!(valueTypeMember.IsString()))
                throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALSCHEMAHASINVALIDVALUETYPE);
            if (!(valueNameMember.IsString()))
                throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALSCHEMAHASINVALIDVALUENAME);
            if (!(valueIDMember.IsInt()))
                throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALSCHEMAHASINVALIDVALUEID);
            if (!(valueSizeMember.IsInt()))
                throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALSCHEMAHASINVALIDVALUESIZE);

            std::string sValueType(valueTypeMember.GetString());
            std::string sValueName(valueNameMember.GetString());
            int64_t nValueID = valueIDMember.GetInt64();
            int64_t nValueSize = valueSizeMember.GetInt64();

            if ((nValueID < BUR_JOURNALSCHEMA_MINVALUEID) || (nValueID > BUR_JOURNALSCHEMA_MAXVALUEID))
                throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALSCHEMAHASINVALIDVALUEID, "journal schema has invalid value ID: " + std::to_string(nValueID));
            if ((nValueSize < BUR_JOURNALSCHEMA_MINVALUESIZE) || (nValueSize > BUR_JOURNALSCHEMA_MAXVALUESIZE))
                throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALSCHEMAHASINVALIDVALUESIZE, "journal schema has invalid value size: " + std::to_string(nValueSize));
            if (!CDriver_BuRJournalValue::nameIsValidJournalName(sValueName))
                throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALSCHEMAHASINVALIDVALUENAME, "journal schema has invalid value name: " + sValueName);
            if (!CDriver_BuRJournalValue::nameIsValidJournalName(sValueType))
                throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALSCHEMAHASINVALIDVALUENAME, "journal schema has invalid value type: " + sValueType);

            pDriverJournalGroup->addValue((uint32_t)nValueID, sValueName, sValueType, (uint32_t)nValueSize);
        }

        m_JournalGroupMap.insert(std::make_pair((uint32_t)nGroupID, pDriverJournalGroup));
        m_JournalGroupNameMap.insert(std::make_pair(sGroupName, pDriverJournalGroup));
    }
}

CDriver_BuRJournal::~CDriver_BuRJournal()
{

}

PDriver_BuRJournalGroup CDriver_BuRJournal::findGroupByID(uint32_t nGroupID, bool bFailIfNotExisting)
{
    auto iIter = m_JournalGroupMap.find(nGroupID);
    if (iIter != m_JournalGroupMap.end())
        return iIter->second;

    if (bFailIfNotExisting)
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALGROUPIDNOTFOUND, "journal group ID not found: " + std::to_string(nGroupID));

    return nullptr;

}

PDriver_BuRJournalGroup CDriver_BuRJournal::findGroupByName(const std::string& sName, bool bFailIfNotExisting)
{
    auto iIter = m_JournalGroupNameMap.find(sName);
    if (iIter != m_JournalGroupNameMap.end())
        return iIter->second;

    if (bFailIfNotExisting)
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALGROUPNAMENOTFOUND, "journal group name not found: " + sName);

    return nullptr;

}


void CDriver_BuRJournal::parseStatus(const std::vector<uint8_t>& buffer, LibMCEnv::PDriverStatusUpdateSession pDriverUpdateInstance)
{
    if (pDriverUpdateInstance.get() == nullptr)
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_INVALIDPARAM);

    CJournalStatusParser parser(buffer);

    uint32_t nGroupCount = parser.readUint32() & JOURNALENTRYTYPE_IDMASK;
    for (uint32_t nGroupIndex = 0; nGroupIndex < nGroupCount; nGroupIndex++) {
        uint32_t nGroupID = parser.readUint32() & JOURNALENTRYTYPE_IDMASK;

        auto pGroup = findGroupByID(nGroupID, true);

        uint32_t nEntryCount = parser.readUint32() & JOURNALENTRYTYPE_IDMASK;
        for (uint32_t nEntryIndex = 0; nEntryIndex < nEntryCount; nEntryIndex++) {
            uint32_t nEntryRawID = parser.readUint32();
            uint32_t nTypeID = nEntryRawID & JOURNALENTRYTYPE_TYPEMASK;
            uint32_t nEntryID = nEntryRawID & JOURNALENTRYTYPE_IDMASK;

            auto pValue = pGroup->findValueByID(nEntryID, true);
            std::string sParameterName = pValue->getDriverParameterName();

            if (nTypeID == JOURNALENTRYTYPE_INT32) {
                int32_t nValue = parser.readInt32();
                if (!sParameterName.empty())
                    pDriverUpdateInstance->SetIntegerParameter(sParameterName, nValue);
            }

            if (nTypeID == JOURNALENTRYTYPE_DOUBLE) {
                double dValue = parser.readDouble();
                if (!sParameterName.empty())
                    pDriverUpdateInstance->SetDoubleParameter(sParameterName, dValue);
            }

            if (nTypeID == JOURNALENTRYTYPE_BOOL_TRUE) {
                if (!sParameterName.empty())
                    pDriverUpdateInstance->SetBoolParameter(sParameterName, true);
            }

            if (nTypeID == JOURNALENTRYTYPE_BOOL_FALSE) {
                if (!sParameterName.empty())
                    pDriverUpdateInstance->SetBoolParameter(sParameterName, false);
            }
        }
    }

}

void CDriver_BuRJournal::registerParameterValue(CDriver_BuRJournalParameterValue* pParameterValue)
{
    if (pParameterValue == nullptr)
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_INVALIDPARAM);

    auto pGroup = findGroupByName(pParameterValue->getGroupName(), true);
    auto pValue = pGroup->findValueByName(pParameterValue->getValueName(), true);
    pValue->setDriverParameterName(pParameterValue->getName());
}

