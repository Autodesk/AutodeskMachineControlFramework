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

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
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

#include "libmcdata_datamodel.hpp"
#include "libmcdata_interfaceexception.hpp"

// Include custom headers here.
#include "libmcdata_storage.hpp"
#include "libmcdata_logsession.hpp"
#include "libmcdata_journalsession.hpp"
#include "libmcdata_journalreader.hpp"
#include "libmcdata_alertsession.hpp"
#include "libmcdata_telemetrysession.hpp"
#include "libmcdata_telemetryreader.hpp"
#include "libmcdata_buildjobhandler.hpp"
#include "libmcdata_loginhandler.hpp"
#include "libmcdata_persistencyhandler.hpp"
#include "libmcdata_installationinformation.hpp"
#include "libmcdata_machineconfigurationtype.hpp"

#include "amcdata_databasemigrator.hpp"
#include "amcdata_sqlhandler_sqlite.hpp"

#include "amcdata_databasemigrator_storage.hpp"
#include "amcdata_databasemigrator_buildjobs.hpp"
#include "amcdata_databasemigrator_users.hpp"
#include "amcdata_databasemigrator_persistentparameters.hpp"
#include "amcdata_databasemigrator_journals.hpp"
#include "amcdata_databasemigrator_machineconfiguration.hpp"

#include "common_utils.hpp"
#include "common_chrono.hpp"

#include <sstream>
#include <iomanip>

using namespace LibMCData::Impl;

#define __STRINGIZE(x) #x
#define __STRINGIZE_VALUE_OF(x) __STRINGIZE(x)

/*************************************************************************************************************************
 Class definition of CDataModel 
**************************************************************************************************************************/

CDataModel::CDataModel()
    : m_eDataBaseType(eDataBaseType::Unknown), m_pLogCallback (nullptr), m_pLogUserData (nullptr)
{
    m_sSessionUUID = AMCCommon::CUtils::createUUID();

    AMCCommon::CChrono chrono;
    uint32_t nYear = 0;
    uint32_t nMonth = 0;
    uint32_t nDay = 0;
    uint32_t nHour = 0;
    uint32_t nMinute = 0;
    uint32_t nSecond = 0;
    uint32_t nMicrosecond = 0;
    uint64_t nTimeStamp = chrono.getUTCTimeStampInMicrosecondsSince1970();

    AMCCommon::CChrono::parseDateTimeFromMicrosecondsSince1970(nTimeStamp, nYear, nMonth, nDay, nHour, nMinute, nSecond, nMicrosecond);

    std::stringstream sstream;
    sstream << std::setfill('0')
        << std::setw(4) << nYear
        << std::setw(2) << nMonth
        << std::setw(2) << nDay << "_"
        << std::setw(2) << nHour
        << std::setw(2) << nMinute
        << std::setw(2) << nSecond;

    m_sTimeFileName = sstream.str();
    m_sStartTime = AMCCommon::CChrono::convertToISO8601TimeUTC (nTimeStamp, AMCCommon::eUTCStringAccuracy::Seconds);

}

CDataModel::~CDataModel()
{

}

void CDataModel::InitialiseDatabase(const std::string & sDataDirectory, const LibMCData::eDataBaseType dataBaseType, const std::string & sConnectionString)
    
{
    m_pStorageState = std::make_shared<AMCData::CStorageState> (sDataDirectory, m_sSessionUUID);
    m_pStorageState->addAcceptedContent("application/3mf");
    m_pStorageState->addAcceptedContent("text/csv");
    m_pStorageState->addAcceptedContent("image/png");
    m_pStorageState->addAcceptedContent("image/jpeg");
    m_pStorageState->addAcceptedContent("application/amcf-discretefield2d");

    m_pStorageState->addAcceptedContent("image/png");
    m_pStorageState->addAcceptedContent("image/jpeg");

    m_pStorageState->addImageContent("image/png");
    m_pStorageState->addImageContent("image/jpeg");

    if (dataBaseType == eDataBaseType::SqLite) {
        m_pSQLHandler = std::make_shared<AMCData::CSQLHandler_SQLite>(sConnectionString);
    }
    else {
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_UNKNOWNDATABASETYPE);
    }

    // ADD DATABASE MIGRATORS HERE!
    AMCData::CDatabaseMigrator migrator;
    migrator.addMigrationClass(std::make_shared<AMCData::CDatabaseMigrationClass_Storage>());
    migrator.addMigrationClass(std::make_shared<AMCData::CDatabaseMigrationClass_BuildJobs>());
    migrator.addMigrationClass(std::make_shared<AMCData::CDatabaseMigrationClass_Users>());
    migrator.addMigrationClass(std::make_shared<AMCData::CDatabaseMigrationClass_PersistentParameters>());
    migrator.addMigrationClass(std::make_shared<AMCData::CDatabaseMigrationClass_Journals>());
    migrator.addMigrationClass(std::make_shared<AMCData::CDatabaseMigrationClass_MachineConfiguration>());
    migrator.migrateDatabaseSchemas(m_pSQLHandler, m_sInstallationUUID, m_sInstallationSecret);

    // Store Database type after successful initialisation
    m_eDataBaseType = dataBaseType;

    auto sJournalBasePath = m_pStorageState->getJournalBasePath(m_sTimeFileName);
    auto sJournalName = m_pStorageState->getJournalFileName(m_sTimeFileName);
    auto sJournalChunkBaseName = m_pStorageState->getJournalChunkBaseName(m_sTimeFileName);
    auto sTelemetryChunkBaseName = m_pStorageState->getTelemetryChunkBaseName(m_sTimeFileName);;

    m_pJournal = std::make_shared<AMCData::CJournal> (sJournalBasePath, sJournalName, sJournalChunkBaseName, sTelemetryChunkBaseName, m_sSessionUUID);

    auto pStatement = m_pSQLHandler->prepareStatement("INSERT INTO journals (uuid, starttime, logfilename, journalfilename, telemetryfilename, logfilepath, journalfilepath, schemaversion, githash) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");
    pStatement->setString(1, m_sSessionUUID);
    pStatement->setString(2, m_sStartTime);
    pStatement->setString(3, sJournalName);
    pStatement->setString(4, sJournalChunkBaseName);
    pStatement->setString(5, sTelemetryChunkBaseName);
    pStatement->setString(6, "");
    pStatement->setString(7, "");
    pStatement->setInt(8, m_pJournal->getSchemaVersion ());
    pStatement->setString(9, __STRINGIZE_VALUE_OF(__GITHASH));
    pStatement->execute();

}

LibMCData_uint32 CDataModel::GetDataModelVersion()
{
	return AMCData::CDatabaseMigrator::getCurrentSchemaVersion ();
}

void CDataModel::GetInstallationInformation(std::string& sInstallationUUID, std::string& sInstallationSecret)
{
    /* DEPRECIATED! use GetInstallationInformationObject instead! */

    if (m_eDataBaseType == eDataBaseType::Unknown)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_UNKNOWNDATABASETYPE);

    sInstallationUUID = AMCCommon::CUtils::normalizeUUIDString (m_sInstallationUUID);
    sInstallationSecret = AMCCommon::CUtils::normalizeSHA256String (m_sInstallationSecret);

}

IInstallationInformation* CDataModel::GetInstallationInformationObject()
{
    return new CInstallationInformation(m_sInstallationUUID, m_sInstallationSecret, m_sTempBasePath);
}

IStorage * CDataModel::CreateStorage()
{
    if (m_eDataBaseType == eDataBaseType::Unknown)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_UNKNOWNDATABASETYPE);
    if (m_pStorageState.get() == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDSTORAGESTATE);

	return new CStorage (m_pSQLHandler, m_pStorageState);
}

ILogSession* CDataModel::CreateNewLogSession()
{
    if (m_pJournal.get () == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDJOURNAL);

	return new CLogSession(m_pJournal);
}

IJournalSession* CDataModel::CreateJournalSession()
{
    if (m_pJournal.get() == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDJOURNAL);

    return new CJournalSession(m_pJournal);

}

IJournalReader* CDataModel::CreateJournalReader(const std::string& sJournalUUID)
{
    auto sJournalBasePath = m_pStorageState->getJournalBasePath(m_sTimeFileName);

    return new CJournalReader(m_pSQLHandler, sJournalUUID, sJournalBasePath);
}


IAlertSession* CDataModel::CreateAlertSession()
{
    if (m_pJournal.get() == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDJOURNAL);

    return new CAlertSession(m_pJournal);

}

ITelemetrySession* CDataModel::CreateTelemetrySession()
{
    if (m_pJournal.get() == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDJOURNAL);

    return new CTelemetrySession(m_pJournal);

}

ITelemetryReader* CDataModel::CreateTelemetryReader(const std::string& sJournalUUID)
{
    auto sJournalBasePath = m_pStorageState->getJournalBasePath(m_sTimeFileName);

    return new CTelemetryReader(m_pSQLHandler, sJournalUUID, sJournalBasePath);
}



IBuildJobHandler* CDataModel::CreateBuildJobHandler()
{

    if (m_pStorageState.get() == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDSTORAGESTATE);

    return new CBuildJobHandler(m_pSQLHandler, m_pStorageState);
}

ILoginHandler* CDataModel::CreateLoginHandler()
{
    return new CLoginHandler(m_pSQLHandler);
}

IPersistencyHandler* CDataModel::CreatePersistencyHandler()
{
    return new CPersistencyHandler(m_pSQLHandler);
}

void CDataModel::SetBaseTempDirectory(const std::string& sTempDirectory)
{
    if (!sTempDirectory.empty()) {
        if (!AMCCommon::CUtils::fileOrPathExistsOnDisk(sTempDirectory))
            AMCCommon::CUtils::createDirectoryOnDisk(sTempDirectory);

        m_sTempBasePath = AMCCommon::CUtils::getFullPathName (sTempDirectory, true);
    }
    else {
        m_sTempBasePath = "";
    }
}

std::string CDataModel::GetBaseTempDirectory()
{
    // DEPRECIATED. Use GetInstallationInformationObject instead!
    return m_sTempBasePath;
}


void CDataModel::SetLogCallback(const LibMCData::LogCallback pLogCallback, const LibMCData_pvoid pUserData)
{
    m_pLogCallback = pLogCallback;
    m_pLogUserData = pUserData;
}

void CDataModel::ClearLogCallback()
{
    m_pLogCallback = nullptr;
    m_pLogUserData = nullptr;
}

bool CDataModel::HasLogCallback()
{
    return (m_pLogCallback != nullptr);
}

void CDataModel::TriggerLogCallback(const std::string& sLogMessage, const std::string& sSubSystem, const LibMCData::eLogLevel eLogLevel, const std::string& sTimestamp)
{
    if (m_pLogCallback == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_NOLOGCALLBACK);

    m_pLogCallback(sLogMessage.c_str(), sSubSystem.c_str(), eLogLevel, sTimestamp.c_str (), m_pLogUserData);
}

IMachineConfigurationTypeIterator* CDataModel::ListRegisteredConfigurationTypes()
{
    return CMachineConfigurationType::listRegisteredConfigurationTypes(m_pSQLHandler);
}

IMachineConfigurationType* CDataModel::FindConfigurationTypeBySchema(const std::string& sSchemaType)
{
    return CMachineConfigurationType::makeBySchema (m_pSQLHandler, sSchemaType);
}

IMachineConfigurationType* CDataModel::FindConfigurationTypeByUUID(const std::string& sUUID)
{
    return CMachineConfigurationType::makeByUUID (m_pSQLHandler, sUUID);
}

IMachineConfigurationType* CDataModel::CreateConfigurationType(const std::string& sSchemaType, const std::string& sName)
{
    return CMachineConfigurationType::createNewConfigurationType(m_pSQLHandler, sSchemaType, sName);
}
