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


Abstract: This is a stub class definition of CBuildJob

*/

#include "libmcdata_buildjob.hpp"
#include "libmcdata_interfaceexception.hpp"
#include "libmcdata_storagestream.hpp"

// Include custom headers here.
#include "common_utils.hpp"

using namespace LibMCData::Impl;

/*************************************************************************************************************************
 Class definition of CBuildJob 
**************************************************************************************************************************/

CBuildJob::CBuildJob(const std::string& sUUID, const std::string sName, LibMCData::eBuildJobStatus eJobStatus, std::string sTimeStamp, std::string sStorageStreamUUID, std::string sUserID, AMCData::PSQLHandler pSQLHandler, AMCData::PStoragePath pStoragePath)
  : m_sUUID (AMCCommon::CUtils::normalizeUUIDString(sUUID)),
    m_sName (sName), 
    m_eJobStatus (eJobStatus), 
    m_sTimeStamp (sTimeStamp), 
    m_sStorageStreamUUID (AMCCommon::CUtils::normalizeUUIDString(sStorageStreamUUID)),
    m_sUserID (sUserID),
    m_pSQLHandler(pSQLHandler),
    m_pStoragePath (pStoragePath)
{
    if (pSQLHandler.get() == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);
    if (pStoragePath.get() == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);
    if (sName.empty())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDJOBNAME);
    if (sTimeStamp.empty())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDJOBTIMESTAMP);
    if (sUserID.empty())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDUSERID);
}

CBuildJob* CBuildJob::make(const std::string& sUUID, const std::string sName, LibMCData::eBuildJobStatus eJobStatus, std::string sTimeStamp, std::string sStorageStreamUUID, std::string sUserID, AMCData::PSQLHandler pSQLHandler, AMCData::PStoragePath pStoragePath)
{
    return new CBuildJob(sUUID, sName, eJobStatus, sTimeStamp, sStorageStreamUUID, sUserID, pSQLHandler, pStoragePath);
}

CBuildJob* CBuildJob::makeFromDatabase(const std::string& sJobUUID, AMCData::PSQLHandler pSQLHandler, AMCData::PStoragePath pStoragePath)
{
    if (pSQLHandler.get() == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);
    if (pStoragePath.get() == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

    auto sParsedJobUUID = AMCCommon::CUtils::normalizeUUIDString(sJobUUID);

    std::string sQuery = "SELECT uuid, name, status, timestamp, storagestreamuuid, userid FROM buildjobs WHERE uuid=?";
    auto pStatement = pSQLHandler->prepareStatement(sQuery);
    pStatement->setString(1, sParsedJobUUID);
    if (!pStatement->nextRow())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_JOBUUIDNOTFOUND);

    auto sUUID = pStatement->getColumnString(1);
    auto sName = pStatement->getColumnString(2);
    auto eJobStatus = convertStringToBuildJobStatus(pStatement->getColumnString(3));
    auto sTimeStamp = pStatement->getColumnString(4);
    auto sStorageStreamUUID = pStatement->getColumnString(5);
    auto sUserID = pStatement->getColumnString(6);

    return make (sUUID, sName, eJobStatus, sTimeStamp, sStorageStreamUUID, sUserID, pSQLHandler, pStoragePath);
}


PBuildJob CBuildJob::makeShared(const std::string& sUUID, const std::string sName, LibMCData::eBuildJobStatus eJobStatus, std::string sTimeStamp, std::string sStorageStreamUUID, std::string sUserID, AMCData::PSQLHandler pSQLHandler, AMCData::PStoragePath pStoragePath)
{
    return std::shared_ptr<CBuildJob>(make(sUUID, sName, eJobStatus, sTimeStamp, sStorageStreamUUID, sUserID, pSQLHandler, pStoragePath));
}

PBuildJob CBuildJob::makeSharedFromDatabase(const std::string& sJobUUID, AMCData::PSQLHandler pSQLHandler, AMCData::PStoragePath pStoragePath)
{
    return std::shared_ptr<CBuildJob>(makeFromDatabase (sJobUUID, pSQLHandler, pStoragePath));
}


std::string CBuildJob::GetUUID()
{
    return m_sUUID;
}

std::string CBuildJob::GetName()
{
    return m_sName;
}

LibMCData::eBuildJobStatus CBuildJob::GetStatus()
{
    return m_eJobStatus;
}

std::string CBuildJob::GetTimeStamp()
{
	return m_sTimeStamp;
}

IStorageStream * CBuildJob::GetStorageStream()
{
    return CStorageStream::makeFromDatabase(m_sStorageStreamUUID, m_pSQLHandler, m_pStoragePath);
}

ILogSession * CBuildJob::GetBuildJobLogger()
{
	throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_NOTIMPLEMENTED);
}

std::string CBuildJob::convertBuildJobStatusToString(const LibMCData::eBuildJobStatus eStatus)
{
    switch (eStatus) {
    case LibMCData::eBuildJobStatus::Created: return "created";
    case LibMCData::eBuildJobStatus::Validating: return "validating";
    case LibMCData::eBuildJobStatus::Validated: return "validated";
    case LibMCData::eBuildJobStatus::Archived: return "archived";
    default:
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDJOBSTATUS);
    }
}

LibMCData::eBuildJobStatus CBuildJob::convertStringToBuildJobStatus(const std::string& sValue)
{
    if (sValue == "created")
        return LibMCData::eBuildJobStatus::Created;
    if (sValue == "validating")
        return LibMCData::eBuildJobStatus::Validating;
    if (sValue == "validated")
        return LibMCData::eBuildJobStatus::Validated;
    if (sValue == "archived")
        return LibMCData::eBuildJobStatus::Archived;

    throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDJOBSTATUS);
}
