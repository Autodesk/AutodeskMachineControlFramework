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

#include "libmcdata_buildjobdata.hpp"
#include "libmcdata_buildjobdataiterator.hpp"

// Include custom headers here.
#include "common_utils.hpp"
#include "common_chrono.hpp"

using namespace LibMCData::Impl;

/*************************************************************************************************************************
 Class definition of CBuildJob 
**************************************************************************************************************************/

CBuildJob::CBuildJob(const std::string& sUUID, const std::string sName, LibMCData::eBuildJobStatus eJobStatus, std::string sTimeStamp, std::string sStorageStreamUUID, std::string sUserID, uint32_t nLayerCount, AMCData::PSQLHandler pSQLHandler, AMCData::PStoragePath pStoragePath)
  : m_sUUID (AMCCommon::CUtils::normalizeUUIDString(sUUID)),
    m_sName (sName), 
    m_eJobStatus (eJobStatus), 
    m_sTimeStamp (sTimeStamp), 
    m_sStorageStreamUUID (AMCCommon::CUtils::normalizeUUIDString(sStorageStreamUUID)),
    m_sUserID (sUserID),
    m_pSQLHandler(pSQLHandler),
    m_nLayerCount(nLayerCount),
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

CBuildJob* CBuildJob::make(const std::string& sUUID, const std::string sName, LibMCData::eBuildJobStatus eJobStatus, std::string sTimeStamp, std::string sStorageStreamUUID, std::string sUserID, uint32_t nLayerCount, AMCData::PSQLHandler pSQLHandler, AMCData::PStoragePath pStoragePath)
{
    return new CBuildJob(sUUID, sName, eJobStatus, sTimeStamp, sStorageStreamUUID, sUserID, nLayerCount, pSQLHandler, pStoragePath);
}

CBuildJob* CBuildJob::makeFromDatabase(const std::string& sJobUUID, AMCData::PSQLHandler pSQLHandler, AMCData::PStoragePath pStoragePath)
{
    if (pSQLHandler.get() == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);
    if (pStoragePath.get() == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

    auto sParsedJobUUID = AMCCommon::CUtils::normalizeUUIDString(sJobUUID);

    std::string sQuery = "SELECT uuid, name, status, timestamp, storagestreamuuid, layercount, userid FROM buildjobs WHERE uuid=?";
    auto pStatement = pSQLHandler->prepareStatement(sQuery);
    pStatement->setString(1, sParsedJobUUID);
    if (!pStatement->nextRow())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_JOBUUIDNOTFOUND);

    auto sUUID = pStatement->getColumnString(1);
    auto sName = pStatement->getColumnString(2);
    auto eJobStatus = convertStringToBuildJobStatus(pStatement->getColumnString(3));
    auto sTimeStamp = pStatement->getColumnString(4);
    auto sStorageStreamUUID = pStatement->getColumnString(5);
    auto nLayerCount = pStatement->getColumnInt(6);
    auto sUserID = pStatement->getColumnString(7);

    return make (sUUID, sName, eJobStatus, sTimeStamp, sStorageStreamUUID, sUserID, nLayerCount, pSQLHandler, pStoragePath);
}

CBuildJob* CBuildJob::makeFrom(CBuildJob* pBuildJob)
{
    if (pBuildJob == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

    return make(pBuildJob->m_sUUID, pBuildJob->m_sName, pBuildJob->m_eJobStatus, pBuildJob->m_sTimeStamp, pBuildJob->m_sStorageStreamUUID, pBuildJob->m_sUserID,
            pBuildJob->m_nLayerCount, pBuildJob->m_pSQLHandler, pBuildJob->m_pStoragePath);
}

PBuildJob CBuildJob::makeShared(const std::string& sUUID, const std::string sName, LibMCData::eBuildJobStatus eJobStatus, std::string sTimeStamp, std::string sStorageStreamUUID, std::string sUserID, uint32_t nLayerCount, AMCData::PSQLHandler pSQLHandler, AMCData::PStoragePath pStoragePath)
{
    return std::shared_ptr<CBuildJob>(make(sUUID, sName, eJobStatus, sTimeStamp, sStorageStreamUUID, sUserID, nLayerCount, pSQLHandler, pStoragePath));
}

PBuildJob CBuildJob::makeSharedFromDatabase(const std::string& sJobUUID, AMCData::PSQLHandler pSQLHandler, AMCData::PStoragePath pStoragePath)
{
    return std::shared_ptr<CBuildJob>(makeFromDatabase (sJobUUID, pSQLHandler, pStoragePath));
}

PBuildJob CBuildJob::makeSharedFrom(CBuildJob* pBuildJob)
{
    if (pBuildJob == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

    return std::shared_ptr<CBuildJob>(makeFrom (pBuildJob));
}


std::string CBuildJob::GetUUID()
{
    return m_sUUID;
}

std::string CBuildJob::GetName()
{
    return m_sName;
}

LibMCData_uint32 CBuildJob::GetLayerCount()
{
    return m_nLayerCount;
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

std::string CBuildJob::GetStorageStreamUUID()
{
    return m_sStorageStreamUUID;
}

ILogSession * CBuildJob::GetBuildJobLogger()
{
	throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_NOTIMPLEMENTED);
}

void CBuildJob::ensureUpdate(const std::string sUpdateUUID, uint32_t nErrorCode)
{
    std::string sSelectQuery = "SELECT uuid FROM buildjobs WHERE updateuuid=?";
    auto pSelectStatement = m_pSQLHandler->prepareStatement(sSelectQuery);
    pSelectStatement->setString(1, sUpdateUUID);
    if (!pSelectStatement->nextRow())
        throw ELibMCDataInterfaceException(nErrorCode);

}

void CBuildJob::StartValidating()
{

    auto updateUUID = AMCCommon::CUtils::createUUID();
    std::string sQuery = "UPDATE buildjobs SET status=?, updateuuid=? WHERE status=? AND uuid=?";
    auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
    pStatement->setString(1, convertBuildJobStatusToString(LibMCData::eBuildJobStatus::Validating));
    pStatement->setString(2, updateUUID);
    pStatement->setString(3, convertBuildJobStatusToString(LibMCData::eBuildJobStatus::Created));
    pStatement->setString(4, m_sUUID);
    pStatement->execute();

    ensureUpdate(updateUUID, LIBMCDATA_ERROR_COULDNOTUPDATEBUILDSTATUS);
}

void CBuildJob::FinishValidating(const LibMCData_uint32 nLayerCount)
{

    auto updateUUID = AMCCommon::CUtils::createUUID();
    std::string sQuery = "UPDATE buildjobs SET status=?, layercount=?, updateuuid=? WHERE status=? AND uuid=?";
    auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
    pStatement->setString(1, convertBuildJobStatusToString(LibMCData::eBuildJobStatus::Validated));
    pStatement->setInt64(2, nLayerCount);
    pStatement->setString(3, updateUUID);
    pStatement->setString(4, convertBuildJobStatusToString(LibMCData::eBuildJobStatus::Validating));
    pStatement->setString(5, m_sUUID);
    pStatement->execute();

    ensureUpdate(updateUUID, LIBMCDATA_ERROR_COULDNOTUPDATEBUILDSTATUS);
}


void CBuildJob::ArchiveJob()
{
    auto updateUUID = AMCCommon::CUtils::createUUID();
    std::string sQuery = "UPDATE buildjobs SET status=?, updateuuid=? WHERE status=? AND uuid=?";
    auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
    pStatement->setString(1, convertBuildJobStatusToString(LibMCData::eBuildJobStatus::Archived));
    pStatement->setString(2, updateUUID);
    pStatement->setString(3, convertBuildJobStatusToString(LibMCData::eBuildJobStatus::Validated));
    pStatement->setString(4, m_sUUID);
    pStatement->execute();

    ensureUpdate(updateUUID, LIBMCDATA_ERROR_COULDNOTUPDATEBUILDSTATUS);
}


void CBuildJob::UnArchiveJob()
{
    auto updateUUID = AMCCommon::CUtils::createUUID();
    std::string sQuery = "UPDATE buildjobs SET status=?, updateuuid=? WHERE status=? AND uuid=?";
    auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
    pStatement->setString(1, convertBuildJobStatusToString(LibMCData::eBuildJobStatus::Validated));
    pStatement->setString(2, updateUUID);
    pStatement->setString(3, convertBuildJobStatusToString(LibMCData::eBuildJobStatus::Archived));
    pStatement->setString(4, m_sUUID);
    pStatement->execute();

    ensureUpdate(updateUUID, LIBMCDATA_ERROR_COULDNOTUPDATEBUILDSTATUS);

}

void CBuildJob::DeleteJob()
{
    // TODO: Physically delete job from disk!

    auto updateUUID = AMCCommon::CUtils::createUUID();
    std::string sQuery = "UPDATE buildjobs SET status=?, updateuuid=? WHERE status=? AND uuid=?";
    auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
    pStatement->setString(1, convertBuildJobStatusToString(LibMCData::eBuildJobStatus::Deleted));
    pStatement->setString(2, updateUUID);
    pStatement->setString(3, convertBuildJobStatusToString(LibMCData::eBuildJobStatus::Archived));
    pStatement->setString(4, m_sUUID);
    pStatement->execute();

    ensureUpdate(updateUUID, LIBMCDATA_ERROR_COULDNOTUPDATEBUILDSTATUS);

}

bool CBuildJob::JobCanBeArchived()

{
    // TODO: Track active jobs and prevent them from archiving.
    std::string sQuery = "SELECT uuid FROM buildjobs WHERE status=? AND uuid=?";
    auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
    pStatement->setString(1, convertBuildJobStatusToString(LibMCData::eBuildJobStatus::Validated));
    pStatement->setString(2, m_sUUID);
    return pStatement->nextRow();

}

void CBuildJob::AddJobData(const std::string& sName, IStorageStream* pStream, const LibMCData::eBuildJobDataType eDataType, const std::string& sUserID)
{
    if (pStream == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

    auto sStreamUUID = pStream->GetUUID();
    auto sStreamSHA2 = pStream->GetSHA2();
    auto nStreamSize = pStream->GetSize();

    AMCCommon::CChrono chrono;
    auto sTimeStamp = chrono.getStartTimeISO8601TimeUTC();
    std::unique_ptr<CBuildJobData> buildJobData (CBuildJobData::createInDatabase (sName, m_sUUID, eDataType, sTimeStamp, sStreamUUID, sUserID, sStreamSHA2, nStreamSize, m_pSQLHandler, m_pStoragePath));
}


CBuildJobData * CBuildJob::makeJobDataEx(AMCData::CSQLStatement* pStatement)
{
    if (pStatement == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

    std::string sDataUUID = pStatement->getColumnUUID(1);
    std::string sJobUUID = pStatement->getColumnString(2);
    std::string sName = pStatement->getColumnString(3);
    LibMCData::eBuildJobDataType eDataType = CBuildJobData::convertStringToBuildJobDataType(pStatement->getColumnString(4));
    std::string sTimeStamp = pStatement->getColumnString(5);
    std::string sStorageStreamUUID = pStatement->getColumnUUID(6);
    std::string sUserID = pStatement->getColumnString(7);
    std::string sSHA2 = pStatement->getColumnString(8);
    uint64_t nStreamSize = pStatement->getColumnInt64(9);

    return CBuildJobData::make(sDataUUID, sName, sJobUUID, eDataType, sTimeStamp, sStorageStreamUUID, sUserID, sSHA2, nStreamSize, m_pSQLHandler, m_pStoragePath);
}


IBuildJobDataIterator* CBuildJob::listJobDataEx(AMCData::CSQLStatement* pStatement)
{
    if (pStatement == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

    std::unique_ptr<CBuildJobDataIterator> buildJobIterator(new CBuildJobDataIterator());

    while (pStatement->nextRow()) {
        buildJobIterator->AddJobData(std::shared_ptr<CBuildJobData> (makeJobDataEx (pStatement)));
    }

    return buildJobIterator.release();
}


IBuildJobDataIterator* CBuildJob::ListJobDataByType(const LibMCData::eBuildJobDataType eDataType)
{

    std::string sQuery = "SELECT buildjobdata.uuid, buildjobdata.jobuuid, buildjobdata.name, buildjobdata.datatype, buildjobdata.timestamp, buildjobdata.storagestreamuuid, buildjobdata.userid, storage_streams.sha2, storage_streams.size FROM buildjobdata LEFT JOIN storage_streams ON storage_streams.uuid=storagestreamuuid WHERE jobuuid=? AND active=? AND datatype=? ORDER BY buildjobdata.timestamp";
    auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
    pStatement->setString (1, m_sUUID);
    pStatement->setInt(2, 1);
    pStatement->setString(3, CBuildJobData::convertBuildJobDataTypeToString(eDataType));

    return listJobDataEx (pStatement.get());
}

IBuildJobDataIterator* CBuildJob::ListJobData()
{
    std::unique_ptr<CBuildJobDataIterator> buildJobIterator (new CBuildJobDataIterator ());

    std::string sQuery = "SELECT buildjobdata.uuid, buildjobdata.jobuuid, buildjobdata.name, buildjobdata.datatype, buildjobdata.timestamp, buildjobdata.storagestreamuuid, buildjobdata.userid, storage_streams.sha2, storage_streams.size FROM buildjobdata LEFT JOIN storage_streams ON storage_streams.uuid=storagestreamuuid WHERE jobuuid=? AND active=? ORDER BY buildjobdata.timestamp";
    auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
    pStatement->setString(1, m_sUUID);
    pStatement->setInt(2, 1);

    return listJobDataEx(pStatement.get());
}

IBuildJobData* CBuildJob::RetrieveJobData(const std::string& sDataUUID)
{
    std::unique_ptr<CBuildJobDataIterator> buildJobIterator(new CBuildJobDataIterator());

    std::string sQuery = "SELECT buildjobdata.uuid, buildjobdata.jobuuid, buildjobdata.name, buildjobdata.datatype, buildjobdata.timestamp, buildjobdata.storagestreamuuid, buildjobdata.userid, storage_streams.sha2, storage_streams.size FROM buildjobdata LEFT JOIN storage_streams ON storage_streams.uuid=storagestreamuuid WHERE jobuuid=? AND buildjobdata.uuid=? AND active=?";
    auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
    pStatement->setString(1, m_sUUID);
    pStatement->setString(2, sDataUUID);
    pStatement->setInt(3, 1);

    if (!pStatement->nextRow())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_BUILDJOBDATANOTFOUND);
    
    return makeJobDataEx(pStatement.get());
}


std::string CBuildJob::convertBuildJobStatusToString(const LibMCData::eBuildJobStatus eStatus)
{
    switch (eStatus) {
    case LibMCData::eBuildJobStatus::Created: return "created";
    case LibMCData::eBuildJobStatus::Validating: return "validating";
    case LibMCData::eBuildJobStatus::Validated: return "validated";
    case LibMCData::eBuildJobStatus::Archived: return "archived";
    case LibMCData::eBuildJobStatus::Deleted: return "deleted";
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
    if (sValue == "deleted")
        return LibMCData::eBuildJobStatus::Deleted;

    throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDJOBSTATUS);
}

