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


Abstract: This is a stub class definition of CBuildJobExecutionData

*/

#include "libmcdata_buildjobexecutiondata.hpp"
#include "libmcdata_interfaceexception.hpp"

// Include custom headers here.
#include "libmcdata_storagestream.hpp"
#include "common_utils.hpp"


using namespace LibMCData::Impl;

/*************************************************************************************************************************
 Class definition of CBuildJobExecutionData 
**************************************************************************************************************************/

CBuildJobExecutionData::CBuildJobExecutionData(const std::string& sDataUUID, const std::string& sIdentifier, const std::string& sName, const std::string& sExecutionUUID, LibMCData::eCustomDataType eDataType, const std::string& sTimeStamp, const std::string& sStorageStreamUUID, const std::string& sUserID, const std::string& sSHA2, uint64_t nStreamSize, AMCData::PSQLHandler pSQLHandler, AMCData::PStorageState pStorageState)
    :
    CCustomDataStream(sDataUUID, sIdentifier, sName, eDataType, sTimeStamp, sStorageStreamUUID, sUserID, sSHA2, nStreamSize),
    m_sExecutionUUID(AMCCommon::CUtils::normalizeUUIDString(sExecutionUUID)),
    m_pSQLHandler(pSQLHandler),
    m_pStorageState(pStorageState)
{
    if (pSQLHandler.get() == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);
    if (pStorageState.get() == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);
    if (sName.empty())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDJOBNAME);
    if (sTimeStamp.empty())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDJOBTIMESTAMP);
    if (sUserID.empty())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDUSERID);
}

CBuildJobExecutionData::~CBuildJobExecutionData()
{

}

CBuildJobExecutionData* CBuildJobExecutionData::make(const std::string& sDataUUID, const std::string& sIdentifier, const std::string& sName, const std::string& sExecutionUUID, LibMCData::eCustomDataType eDataType, const std::string& sTimeStamp, const std::string& sStorageStreamUUID, const std::string& sUserID, const std::string& sSHA2, uint64_t nStreamSize, AMCData::PSQLHandler pSQLHandler, AMCData::PStorageState pStorageState)
{
    return new CBuildJobExecutionData(sDataUUID, sIdentifier, sName, sExecutionUUID, eDataType, sTimeStamp, sStorageStreamUUID, sUserID, sSHA2, nStreamSize, pSQLHandler, pStorageState);
}

CBuildJobExecutionData* CBuildJobExecutionData::makeFrom(CBuildJobExecutionData* pBuildJobExecutionData)
{
    if (pBuildJobExecutionData == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

    return make(pBuildJobExecutionData->GetDataUUID(), pBuildJobExecutionData->GetIdentifier(), pBuildJobExecutionData->GetName(), pBuildJobExecutionData->GetExecutionUUID(), pBuildJobExecutionData->GetDataType(), pBuildJobExecutionData->GetTimeStamp(), pBuildJobExecutionData->GetStorageStreamUUID(), pBuildJobExecutionData->GetUserUUID(), pBuildJobExecutionData->GetStorageStreamSHA2(), pBuildJobExecutionData->GetStorageStreamSize(), pBuildJobExecutionData->m_pSQLHandler, pBuildJobExecutionData->m_pStorageState);
}

PBuildJobExecutionData CBuildJobExecutionData::makeShared(const std::string& sDataUUID, const std::string& sIdentifier, const std::string& sName, const std::string& sExecutionUUID, LibMCData::eCustomDataType eDataType, const std::string& sTimeStamp, const std::string& sStorageStreamUUID, const std::string& sUserID, const std::string& sSHA2, uint64_t nStreamSize, AMCData::PSQLHandler pSQLHandler, AMCData::PStorageState pStorageState)
{
    return std::shared_ptr<CBuildJobExecutionData>(make(sDataUUID, sIdentifier, sName, sExecutionUUID, eDataType, sTimeStamp, sStorageStreamUUID, sUserID, sSHA2, nStreamSize, pSQLHandler, pStorageState));
}

PBuildJobExecutionData CBuildJobExecutionData::makeSharedFrom(CBuildJobExecutionData* pBuildJobExecutionData)
{
    return std::shared_ptr<CBuildJobExecutionData>(makeFrom(pBuildJobExecutionData));
}

CBuildJobExecutionData* CBuildJobExecutionData::createInDatabase(const std::string& sIdentifier, const std::string & sName, const std::string& sExecutionUUID, LibMCData::eCustomDataType eDataType, const std::string & sTimeStamp, const std::string & sStorageStreamUUID, const std::string & sUserID, const std::string& sSHA2, uint64_t nStreamSize, AMCData::PSQLHandler pSQLHandler, AMCData::PStorageState pStorageState)
{
    if (pSQLHandler.get() == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

    std::string sUUID = AMCCommon::CUtils::createUUID();

    std::string sInsertQuery = "INSERT INTO buildjobexecutiondata (uuid, executionuuid, identifier, name, datatype, timestamp, storagestreamuuid, userid, active) VALUES (?, ?, ?, ?, ?, ?, ?, ?, 1)";
    auto pInsertStatement = pSQLHandler->prepareStatement(sInsertQuery);
    pInsertStatement->setString(1, sUUID);
    pInsertStatement->setString(2, AMCCommon::CUtils::normalizeUUIDString (sExecutionUUID));
    pInsertStatement->setString(3, sIdentifier);
    pInsertStatement->setString(4, sName);
    pInsertStatement->setString(5, convertCustomDataTypeToString(eDataType));
    pInsertStatement->setString(6, sTimeStamp);
    pInsertStatement->setString(7, sStorageStreamUUID);
    pInsertStatement->setString(8, sUserID);
    pInsertStatement->execute();

    return make(sUUID, sIdentifier, sName, sExecutionUUID, eDataType, sTimeStamp, sStorageStreamUUID, sUserID, sSHA2, nStreamSize, pSQLHandler, pStorageState);
}

PBuildJobExecutionData CBuildJobExecutionData::createSharedInDatabase(const std::string& sIdentifier, const std::string & sName, const std::string& sExecutionUUID, LibMCData::eCustomDataType eDataType, const std::string & sTimeStamp, const std::string & sStorageStreamUUID, const std::string & sUserID, const std::string& sSHA2, uint64_t nStreamSize, AMCData::PSQLHandler pSQLHandler, AMCData::PStorageState pStorageState)
{
    return std::shared_ptr<CBuildJobExecutionData>(createInDatabase(sIdentifier, sName, sExecutionUUID, eDataType, sTimeStamp, sStorageStreamUUID, sUserID, sSHA2, nStreamSize, pSQLHandler, pStorageState));
}



IStorageStream* CBuildJobExecutionData::GetStorageStream()
{
    return CStorageStream::makeFromDatabase(GetStorageStreamUUID(), m_pSQLHandler, m_pStorageState);
}

std::string CBuildJobExecutionData::GetExecutionUUID()
{
    return m_sExecutionUUID;
}


std::string CBuildJobExecutionData::GetMIMEType()
{
    auto pStorageStream = CStorageStream::makeSharedFromDatabase(GetStorageStreamUUID(), m_pSQLHandler, m_pStorageState);
    return pStorageStream->GetMIMEType();
}


