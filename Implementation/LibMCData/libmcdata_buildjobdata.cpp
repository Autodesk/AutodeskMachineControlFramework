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


Abstract: This is a stub class definition of CBuildJobData

*/

#include "libmcdata_buildjobdata.hpp"
#include "libmcdata_interfaceexception.hpp"
#include "libmcdata_storagestream.hpp"

// Include custom headers here.
#include "common_utils.hpp"

using namespace LibMCData::Impl;

/*************************************************************************************************************************
 Class definition of CBuildJobData 
**************************************************************************************************************************/

CBuildJobData::CBuildJobData(const std::string& sDataUUID, const std::string& sIdentifier, const std::string& sName, const std::string& sJobUUID, LibMCData::eCustomDataType eDataType, std::string& sTimeStamp, std::string& sStorageStreamUUID, std::string& sUserID, std::string& sSHA2, uint64_t nStreamSize, AMCData::PSQLHandler pSQLHandler, AMCData::PStorageState pStorageState)
    : 
    CCustomDataStream (sDataUUID, sIdentifier, sName, eDataType, sTimeStamp, sStorageStreamUUID, sUserID, sSHA2, nStreamSize),
    m_sJobUUID (AMCCommon::CUtils::normalizeUUIDString (sJobUUID)),
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

CBuildJobData::~CBuildJobData()
{

}

CBuildJobData* CBuildJobData::make(const std::string& sDataUUID, const std::string& sIdentifier, const std::string& sName, const std::string& sJobUUID, LibMCData::eCustomDataType eDataType, std::string& sTimeStamp, std::string& sStorageStreamUUID, std::string& sUserID, std::string& sSHA2, uint64_t nStreamSize, AMCData::PSQLHandler pSQLHandler, AMCData::PStorageState pStorageState)
{
    return new CBuildJobData(sDataUUID, sIdentifier, sName, sJobUUID, eDataType, sTimeStamp, sStorageStreamUUID, sUserID, sSHA2, nStreamSize, pSQLHandler, pStorageState);
}

CBuildJobData* CBuildJobData::makeFrom(CBuildJobData* pBuildJobData)
{
    if (pBuildJobData == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

    return make(pBuildJobData->GetDataUUID (), pBuildJobData->GetIdentifier (), pBuildJobData->GetName (), pBuildJobData->GetJobUUID (), pBuildJobData->GetDataType (), pBuildJobData->GetTimeStamp (), pBuildJobData->GetStorageStreamUUID (), pBuildJobData->GetUserUUID (), pBuildJobData->GetStorageStreamSHA2 (), pBuildJobData->GetStorageStreamSize (), pBuildJobData->m_pSQLHandler, pBuildJobData->m_pStorageState);
}

PBuildJobData CBuildJobData::makeShared(const std::string& sDataUUID, const std::string& sIdentifier, const std::string& sName, const std::string& sJobUUID, LibMCData::eCustomDataType eDataType, std::string& sTimeStamp, std::string& sStorageStreamUUID, std::string& sUserID, std::string& sSHA2, uint64_t nStreamSize, AMCData::PSQLHandler pSQLHandler, AMCData::PStorageState pStorageState)
{
    return std::shared_ptr<CBuildJobData>(make(sDataUUID, sIdentifier, sName, sJobUUID, eDataType, sTimeStamp, sStorageStreamUUID, sUserID, sSHA2, nStreamSize, pSQLHandler, pStorageState));
}

PBuildJobData CBuildJobData::makeSharedFrom(CBuildJobData* pBuildJobData)
{
    return std::shared_ptr<CBuildJobData> (makeFrom (pBuildJobData));
}

CBuildJobData* CBuildJobData::createInDatabase(const std::string& sIdentifier, const std::string sName,  const std::string & sJobUUID, LibMCData::eCustomDataType eDataType, std::string sTimeStamp, std::string sStorageStreamUUID, std::string sUserID, std::string& sSHA2, uint64_t nStreamSize, AMCData::PSQLHandler pSQLHandler, AMCData::PStorageState pStorageState)
{
    if (pSQLHandler.get() == nullptr)
        throw ELibMCDataInterfaceException (LIBMCDATA_ERROR_INVALIDPARAM);

    std::string sUUID = AMCCommon::CUtils::createUUID ();

    std::string sInsertQuery = "INSERT INTO buildjobdata (uuid, jobuuid, identifier, name, datatype, timestamp, storagestreamuuid, userid, active) VALUES (?, ?, ?, ?, ?, ?, ?, ?, 1)";
    auto pInsertStatement = pSQLHandler->prepareStatement(sInsertQuery);
    pInsertStatement->setString(1, sUUID);
    pInsertStatement->setString(2, sJobUUID);
    pInsertStatement->setString(3, sIdentifier);
    pInsertStatement->setString(4, sName);
    pInsertStatement->setString(5, convertCustomDataTypeToString(eDataType));
    pInsertStatement->setString(6, sTimeStamp);
    pInsertStatement->setString(7, sStorageStreamUUID);
    pInsertStatement->setString(8, sUserID);
    pInsertStatement->execute();

    return make(sUUID, sIdentifier, sName, sJobUUID, eDataType, sTimeStamp, sStorageStreamUUID, sUserID, sSHA2, nStreamSize, pSQLHandler, pStorageState);
}

PBuildJobData CBuildJobData::createSharedInDatabase(const std::string& sIdentifier, const std::string sName, const std::string & sJobUUID, LibMCData::eCustomDataType eDataType, std::string sTimeStamp, std::string sStorageStreamUUID, std::string sUserID, std::string& sSHA2, uint64_t nStreamSize, AMCData::PSQLHandler pSQLHandler, AMCData::PStorageState pStorageState)
{
    return std::shared_ptr<CBuildJobData>(createInDatabase(sIdentifier, sName, sJobUUID, eDataType, sTimeStamp, sStorageStreamUUID, sUserID, sSHA2, nStreamSize, pSQLHandler, pStorageState));
}



IStorageStream * CBuildJobData::GetStorageStream()
{
	return CStorageStream::makeFromDatabase(GetStorageStreamUUID (), m_pSQLHandler, m_pStorageState);
}

std::string CBuildJobData::GetJobUUID()
{
    return m_sJobUUID;
}


std::string CBuildJobData::GetMIMEType()
{
    auto pStorageStream = CStorageStream::makeSharedFromDatabase(GetStorageStreamUUID (), m_pSQLHandler, m_pStorageState);
    return pStorageStream->GetMIMEType();
}


