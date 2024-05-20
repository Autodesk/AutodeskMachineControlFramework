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


Abstract: This is a stub class definition of CBuildJobHandler

*/

#include "libmcdata_buildjobhandler.hpp"
#include "libmcdata_interfaceexception.hpp"

// Include custom headers here.
#include "libmcdata_buildjob.hpp"
#include "libmcdata_buildjobiterator.hpp"
#include "libmcdata_buildjobexecution.hpp"
#include "libmcdata_buildjobexecutioniterator.hpp"

#include "common_utils.hpp"
#include "common_chrono.hpp"

using namespace LibMCData::Impl;

/*************************************************************************************************************************
 Class definition of CBuildJobHandler 
**************************************************************************************************************************/


CBuildJobHandler::CBuildJobHandler(AMCData::PSQLHandler pSQLHandler, AMCData::PStorageState pStorageState)
    : m_pSQLHandler (pSQLHandler), m_pStorageState(pStorageState)
{
    if (pSQLHandler.get() == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);
    if (pStorageState.get() == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

}

IBuildJob* CBuildJobHandler::CreateJob(const std::string& sJobUUID, const std::string& sName, const std::string& sUserUUID, const std::string& sStorageStreamUUID, const LibMCData_uint64 nAbsoluteTimeStamp)
{
    auto sParsedJobUUID = AMCCommon::CUtils::normalizeUUIDString(sJobUUID);

    if ((sName.length() == 0) || (sName.length() >= 255))
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);
    std::string sNormalizedUserUUID = AMCCommon::CUtils::normalizeUUIDString(sUserUUID);

    auto pTransaction = m_pSQLHandler->beginTransaction();

    std::string sQuery = "SELECT uuid FROM buildjobs WHERE uuid=?";    
    auto pStatement = pTransaction->prepareStatement(sQuery);
    pStatement->setString (1, sParsedJobUUID);
    if (pStatement->nextRow())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_DUPLICATEJOBUUID);
    pStatement = nullptr;

    std::string sUserQuery = "SELECT login FROM users WHERE uuid=?";
    auto pUserStatement = pTransaction->prepareStatement(sUserQuery);
    pUserStatement->setString(1, sNormalizedUserUUID);
    if (!pUserStatement->nextRow ())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_USERNOTFOUND, "user not found: " + sNormalizedUserUUID);

    std::string sUserName = pUserStatement->getColumnString(1);
    pUserStatement = 0;


    std::string sTimeStamp = AMCCommon::CChrono::convertToISO8601TimeUTC(nAbsoluteTimeStamp); 
    LibMCData::eBuildJobStatus eJobStatus = LibMCData::eBuildJobStatus::Created;

    std::string sInsertQuery = "INSERT INTO buildjobs (uuid, name, status, timestamp, storagestreamuuid, useruuid, userid) VALUES (?, ?, ?, ?, ?, ?, ?)";
    auto pInsertStatement = pTransaction->prepareStatement(sInsertQuery);
    pInsertStatement->setString(1, sParsedJobUUID);
    pInsertStatement->setString(2, sName);
    pInsertStatement->setString(3, CBuildJob::convertBuildJobStatusToString (eJobStatus));
    pInsertStatement->setString(4, sTimeStamp);
    pInsertStatement->setString(5, sStorageStreamUUID);
    pInsertStatement->setString(6, sNormalizedUserUUID);
    pInsertStatement->setString(7, sUserName); // This is for legacy use
    pInsertStatement->execute();

    pTransaction->commit ();

    return CBuildJob::make(sParsedJobUUID, sName, eJobStatus, sTimeStamp, sStorageStreamUUID, sNormalizedUserUUID, sUserName, 0, 0, m_pSQLHandler, m_pStorageState);
    
}

IBuildJob * CBuildJobHandler::RetrieveJob(const std::string & sJobUUID)
{
    return CBuildJob::makeFromDatabase(sJobUUID, m_pSQLHandler, m_pStorageState);
}

IBuildJobIterator* CBuildJobHandler::ListJobsByStatus(const LibMCData::eBuildJobStatus eStatus)
{

    std::unique_ptr<CBuildJobIterator> pJobIterator(new CBuildJobIterator());

    std::string sQuery = "SELECT buildjobs.uuid, buildjobs.name, buildjobs.status, buildjobs.timestamp, buildjobs.storagestreamuuid, buildjobs.layercount, buildjobs.useruuid, users.login, (SELECT count(buildjobexecutions.uuid) FROM buildjobexecutions WHERE buildjobexecutions.jobuuid=buildjobs.uuid) FROM buildjobs LEFT JOIN users On users.uuid=buildjobs.useruuid WHERE buildjobs.status=? ORDER BY buildjobs.timestamp DESC";
    auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
    pStatement->setString(1, CBuildJob::convertBuildJobStatusToString(eStatus));
    while (pStatement->nextRow()) {

        auto sUUID = pStatement->getColumnString(1);
        auto sName = pStatement->getColumnString(2);
        auto eJobStatus = CBuildJob::convertStringToBuildJobStatus(pStatement->getColumnString(3));
        auto sTimeStamp = pStatement->getColumnString(4);
        auto sStorageStreamUUID = pStatement->getColumnString(5);
        auto nLayerCount = pStatement->getColumnInt(6);
        auto sUserUUID = pStatement->getColumnString(7);

        std::string sUserName;
        if (!pStatement->columnIsNull (8))
            sUserName = pStatement->getColumnString(8);

        int32_t nExecutionCount = pStatement->getColumnInt(9);
        // This should never happen, because of the SQL Query
        if (nExecutionCount < 0)
            throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_COULDNOTDETERMINEEXECUTIONCOUNT);

        pJobIterator->AddJob (CBuildJob::makeShared (sUUID, sName, eJobStatus, sTimeStamp, sStorageStreamUUID, sUserUUID, sUserName, nLayerCount, (uint32_t) nExecutionCount, m_pSQLHandler, m_pStorageState));
    }

    return pJobIterator.release();
}


IBuildJob* CBuildJobHandler::FindJobOfData(const std::string& sDataUUID)
{

    std::string sQuery = "SELECT jobuuid FROM buildjobdata WHERE uuid=?";
    auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
    pStatement->setString(1, sDataUUID);
    if (!pStatement->nextRow())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_BUILDJOBDATANOTFOUND);

    auto sJobUUID = pStatement->getColumnString(1);

    return RetrieveJob(sJobUUID);

}


std::string CBuildJobHandler::ConvertBuildStatusToString(const LibMCData::eBuildJobStatus eStatus)
{
    return CBuildJob::convertBuildJobStatusToString(eStatus);
}

LibMCData::eBuildJobStatus CBuildJobHandler::ConvertStringToBuildStatus(const std::string& sString)
{
    return CBuildJob::convertStringToBuildJobStatus(sString);
}  

IBuildJobExecution* CBuildJobHandler::RetrieveJobExecution(const std::string& sExecutionUUID)
{
    return CBuildJobExecution::makeFromDatabase (m_pSQLHandler, AMCCommon::CUtils::normalizeUUIDString (sExecutionUUID), m_pStorageState);

}

IBuildJobExecutionIterator* CBuildJobHandler::ListJobExecutions(const std::string& sMinTimestamp, const std::string& sMaxTimestamp, const std::string& sJournalUUIDFilter)
{
    std::string sQuery = "SELECT buildjobexecutions.uuid, buildjobexecutions.jobuuid, buildjobexecutions.journaluuid, buildjobexecutions.useruuid, buildjobexecutions.startjournaltimestamp, buildjobs.name, buildjobs.status, buildjobs.layercount FROM buildjobexecutions LEFT JOIN buildjobs ON buildjobs.uuid=buildjobexecutions.jobuuid WHERE buildjobexecutions.active=?";
    if (!sJournalUUIDFilter.empty())
        sQuery += " AND buildjobexecutions.journaluuid=?";
    if (!sMinTimestamp.empty ())
        sQuery += " AND buildjobexecutions.startjournaltimestamp>?";
    if (!sMaxTimestamp.empty())
        sQuery += " AND buildjobexecutions.startjournaltimestamp<?";
    sQuery += " ORDER BY buildjobexecutions.startjournaltimestamp";

    uint32_t nColumn = 1;

    auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
    pStatement->setInt(nColumn, 1); 
    nColumn++;

    if (!sJournalUUIDFilter.empty()) {
        pStatement->setString(nColumn, AMCCommon::CUtils::normalizeUUIDString(sJournalUUIDFilter));
        nColumn++;
    }
    
    if (!sMinTimestamp.empty())
    {
        uint64_t nMinTimeStamp = AMCCommon::CChrono::parseISO8601TimeUTC(sMinTimestamp);
        pStatement->setString(nColumn, AMCCommon::CChrono::convertToISO8601TimeUTC (nMinTimeStamp));
        nColumn++;   
    }

    if (!sMaxTimestamp.empty())
    {
        uint64_t nMaxTimeStamp = AMCCommon::CChrono::parseISO8601TimeUTC(sMaxTimestamp);
        pStatement->setString(nColumn, AMCCommon::CChrono::convertToISO8601TimeUTC(nMaxTimeStamp));
        nColumn++;
    }

    std::unique_ptr<CBuildJobExecutionIterator> buildJobIterator(new CBuildJobExecutionIterator());

    while (pStatement->nextRow()) {
        // ATTENTION: All callers of makeFromStatement MUST BE in Sync in terms of column order!!
        // Column 1: Execution UUID
        // Column 2: Job UUID
        // Column 3: Journal UUID
        // Column 4: User UUID
        // Column 5: Start TimeStamp
        // Column 6: Job Name
        // Column 7: Job Status
        // Column 8: Job Layer Count

        buildJobIterator->AddJobExecution(CBuildJobExecution::makeSharedFromStatement(m_pSQLHandler, pStatement, m_pStorageState));
    }

    return buildJobIterator.release();
}
