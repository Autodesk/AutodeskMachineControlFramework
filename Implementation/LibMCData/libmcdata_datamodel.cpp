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
#include "libmcdata_alertsession.hpp"
#include "libmcdata_buildjobhandler.hpp"
#include "libmcdata_loginhandler.hpp"
#include "libmcdata_persistencyhandler.hpp"

#include "amcdata_databasemigrator.hpp"
#include "amcdata_sqlhandler_sqlite.hpp"

#include "amcdata_databasemigrator_storage.hpp"
#include "amcdata_databasemigrator_buildjobs.hpp"
#include "amcdata_databasemigrator_users.hpp"
#include "amcdata_databasemigrator_persistentparameters.hpp"
#include "amcdata_databasemigrator_journals.hpp"

#include "common_utils.hpp"
#include "common_chrono.hpp"

using namespace LibMCData::Impl;

/*************************************************************************************************************************
 Class definition of CDataModel 
**************************************************************************************************************************/

CDataModel::CDataModel()
    : m_eDataBaseType(eDataBaseType::Unknown), m_pLogCallback (nullptr), m_pLogUserData (nullptr)
{
    m_sSessionUUID = AMCCommon::CUtils::createUUID();

    AMCCommon::CChrono chrono;
    m_sTimeFileName = chrono.getStartTimeFileName();
    m_sStartTime = chrono.getStartTimeISO8601TimeUTC();


}

CDataModel::~CDataModel()
{

}

void CDataModel::InitialiseDatabase(const std::string & sDataDirectory, const LibMCData::eDataBaseType dataBaseType, const std::string & sConnectionString)
    
{
    m_pStoragePath = std::make_shared<AMCData::CStoragePath> (sDataDirectory);
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
    migrator.migrateDatabaseSchemas(m_pSQLHandler, m_sInstallationUUID, m_sInstallationSecret);

    // Store Database type after successful initialisation
    m_eDataBaseType = dataBaseType;

    auto sJournalPath = m_pStoragePath->getJournalPath(m_sTimeFileName);
    auto sJournalDataPath = m_pStoragePath->getJournalDataPath(m_sTimeFileName);
    auto sJournalName = m_pStoragePath->getJournalFileName(m_sTimeFileName);
    auto sJournalDataName = m_pStoragePath->getJournalDataFileName(m_sTimeFileName);

    m_pJournal = std::make_shared<AMCData::CJournal>(sJournalPath, sJournalDataPath);

    auto pStatement = m_pSQLHandler->prepareStatement("INSERT INTO journals (uuid, starttime, logfilename, journalfilename, logfilepath, journalfilepath, schemaversion) VALUES (?, ?, ?, ?, ?, ?, ?)");
    pStatement->setString(1, m_sSessionUUID);
    pStatement->setString(2, m_sStartTime);
    pStatement->setString(3, sJournalName);
    pStatement->setString(4, sJournalDataName);
    pStatement->setString(5, sJournalPath);
    pStatement->setString(6, sJournalDataPath);
    pStatement->setInt(7, m_pJournal->getSchemaVersion ());
    pStatement->execute();

}

LibMCData_uint32 CDataModel::GetDataModelVersion()
{
	return AMCData::CDatabaseMigrator::getCurrentSchemaVersion ();
}

void CDataModel::GetInstallationInformation(std::string& sInstallationUUID, std::string& sInstallationSecret)
{
    if (m_eDataBaseType == eDataBaseType::Unknown)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_UNKNOWNDATABASETYPE);

    sInstallationUUID = AMCCommon::CUtils::normalizeUUIDString (m_sInstallationUUID);
    sInstallationSecret = AMCCommon::CUtils::normalizeSHA256String (m_sInstallationSecret);

}

IStorage * CDataModel::CreateStorage()
{
    if (m_eDataBaseType == eDataBaseType::Unknown)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_UNKNOWNDATABASETYPE);
    if (m_pStoragePath.get() == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDSTORAGEPATH);

	return new CStorage (m_pSQLHandler, m_pStoragePath);
}

ILogSession* CDataModel::CreateNewLogSession()
{
    if (m_pStoragePath.get() == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDSTORAGEPATH);

    if (m_pJournal.get () == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDJOURNAL);

	return new CLogSession(m_pJournal);
}

IJournalSession* CDataModel::CreateJournalSession()
{
    if (m_pStoragePath.get() == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDSTORAGEPATH);

    if (m_pJournal.get() == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDJOURNAL);

    return new CJournalSession(m_pJournal);

}

IAlertSession* CDataModel::CreateAlertSession()
{
    if (m_pStoragePath.get() == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDSTORAGEPATH);

    if (m_pJournal.get() == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDJOURNAL);

    return new CAlertSession(m_pJournal);

}


IBuildJobHandler* CDataModel::CreateBuildJobHandler()
{

    return new CBuildJobHandler(m_pSQLHandler, m_pStoragePath);
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
