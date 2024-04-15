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

IBuildJob* CBuildJobHandler::CreateJob(const std::string& sJobUUID, const std::string& sName, const std::string& sUserID, const std::string& sStorageStreamUUID, const LibMCData_uint64 nAbsoluteTimeStamp)
{
    auto sParsedJobUUID = AMCCommon::CUtils::normalizeUUIDString(sJobUUID);

    if ((sName.length() == 0) || (sName.length() >= 255))
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);
    if ((sUserID.length() == 0) || (sUserID.length() >= 63))
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

    std::string sQuery = "SELECT uuid FROM buildjobs WHERE uuid=?";    
    auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
    pStatement->setString (1, sParsedJobUUID);
    if (pStatement->nextRow())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_DUPLICATEJOBUUID);
    pStatement = nullptr;

    std::string sTimeStamp = AMCCommon::CChrono::convertToISO8601TimeUTC(nAbsoluteTimeStamp); 
    LibMCData::eBuildJobStatus eJobStatus = LibMCData::eBuildJobStatus::Created;

    std::string sInsertQuery = "INSERT INTO buildjobs (uuid, name, status, timestamp, storagestreamuuid, userid) VALUES (?, ?, ?, ?, ?, ?)";
    auto pInsertStatement = m_pSQLHandler->prepareStatement(sInsertQuery);
    pInsertStatement->setString(1, sParsedJobUUID);
    pInsertStatement->setString(2, sName);
    pInsertStatement->setString(3, CBuildJob::convertBuildJobStatusToString (eJobStatus));
    pInsertStatement->setString(4, sTimeStamp);
    pInsertStatement->setString(5, sStorageStreamUUID);
    pInsertStatement->setString(6, sUserID);
    pInsertStatement->execute();

    return CBuildJob::make(sParsedJobUUID, sName, eJobStatus, sTimeStamp, sStorageStreamUUID, sUserID, 0, m_pSQLHandler, m_pStorageState);
    
}

IBuildJob * CBuildJobHandler::RetrieveJob(const std::string & sJobUUID)
{
    return CBuildJob::makeFromDatabase(sJobUUID, m_pSQLHandler, m_pStorageState);
}

IBuildJobIterator* CBuildJobHandler::ListJobsByStatus(const LibMCData::eBuildJobStatus eStatus)
{

    std::unique_ptr<CBuildJobIterator> pJobIterator(new CBuildJobIterator());

    std::string sQuery = "SELECT uuid, name, status, timestamp, storagestreamuuid, layercount, userid FROM buildjobs WHERE status=? ORDER BY timestamp DESC";
    auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
    pStatement->setString(1, CBuildJob::convertBuildJobStatusToString(eStatus));
    while (pStatement->nextRow()) {

        auto sUUID = pStatement->getColumnString(1);
        auto sName = pStatement->getColumnString(2);
        auto eJobStatus = CBuildJob::convertStringToBuildJobStatus(pStatement->getColumnString(3));
        auto sTimeStamp = pStatement->getColumnString(4);
        auto sStorageStreamUUID = pStatement->getColumnString(5);
        auto nLayerCount = pStatement->getColumnInt(6);
        auto sUserID = pStatement->getColumnString(7);
    
        pJobIterator->AddJob (CBuildJob::makeShared (sUUID, sName, eJobStatus, sTimeStamp, sStorageStreamUUID, sUserID, nLayerCount, m_pSQLHandler, m_pStorageState));
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
