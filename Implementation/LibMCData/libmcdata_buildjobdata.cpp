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

CBuildJobData::CBuildJobData(const std::string& sDataUUID, const std::string& sIdentifier, const std::string& sName, const std::string& sJobUUID, LibMCData::eBuildJobDataType eDataType, std::string& sTimeStamp, std::string& sStorageStreamUUID, std::string& sUserID, std::string& sSHA2, uint64_t nStreamSize, AMCData::PSQLHandler pSQLHandler, AMCData::PStoragePath pStoragePath)
    : 
    m_sIdentifier (sIdentifier),
    m_sName(sName),
    m_eDataType(eDataType),
    m_sDataUUID(AMCCommon::CUtils::normalizeUUIDString(sDataUUID)),
    m_sTimeStamp(sTimeStamp),
    m_sJobUUID(AMCCommon::CUtils::normalizeUUIDString(sJobUUID)),
    m_sStorageStreamUUID(AMCCommon::CUtils::normalizeUUIDString(sStorageStreamUUID)),
    m_sUserID(sUserID),
    m_sSHA256(sSHA2),
    m_nStreamSize(nStreamSize),
    m_pSQLHandler(pSQLHandler),
    m_pStoragePath(pStoragePath)
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

CBuildJobData::~CBuildJobData()
{

}

CBuildJobData* CBuildJobData::make(const std::string& sDataUUID, const std::string& sIdentifier, const std::string& sName, const std::string& sJobUUID, LibMCData::eBuildJobDataType eDataType, std::string& sTimeStamp, std::string& sStorageStreamUUID, std::string& sUserID, std::string& sSHA2, uint64_t nStreamSize, AMCData::PSQLHandler pSQLHandler, AMCData::PStoragePath pStoragePath)
{
    return new CBuildJobData(sDataUUID, sIdentifier, sName, sJobUUID, eDataType, sTimeStamp, sStorageStreamUUID, sUserID, sSHA2, nStreamSize, pSQLHandler, pStoragePath);
}

CBuildJobData* CBuildJobData::makeFrom(CBuildJobData* pBuildJobData)
{
    if (pBuildJobData == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

    return make(pBuildJobData->m_sDataUUID, pBuildJobData->m_sIdentifier, pBuildJobData->m_sName, pBuildJobData->m_sJobUUID, pBuildJobData->m_eDataType, pBuildJobData->m_sTimeStamp, pBuildJobData->m_sStorageStreamUUID, pBuildJobData->m_sUserID, pBuildJobData->m_sSHA256, pBuildJobData->m_nStreamSize, pBuildJobData->m_pSQLHandler, pBuildJobData->m_pStoragePath);
}

PBuildJobData CBuildJobData::makeShared(const std::string& sDataUUID, const std::string& sIdentifier, const std::string& sName, const std::string& sJobUUID, LibMCData::eBuildJobDataType eDataType, std::string& sTimeStamp, std::string& sStorageStreamUUID, std::string& sUserID, std::string& sSHA2, uint64_t nStreamSize, AMCData::PSQLHandler pSQLHandler, AMCData::PStoragePath pStoragePath)
{
    return std::shared_ptr<CBuildJobData>(make(sDataUUID, sIdentifier, sName, sJobUUID, eDataType, sTimeStamp, sStorageStreamUUID, sUserID, sSHA2, nStreamSize, pSQLHandler, pStoragePath));
}

PBuildJobData CBuildJobData::makeSharedFrom(CBuildJobData* pBuildJobData)
{
    return std::shared_ptr<CBuildJobData> (makeFrom (pBuildJobData));
}

CBuildJobData* CBuildJobData::createInDatabase(const std::string& sIdentifier, const std::string sName,  const std::string & sJobUUID, LibMCData::eBuildJobDataType eDataType, std::string sTimeStamp, std::string sStorageStreamUUID, std::string sUserID, std::string& sSHA2, uint64_t nStreamSize, AMCData::PSQLHandler pSQLHandler, AMCData::PStoragePath pStoragePath)
{
    if (pSQLHandler.get() == nullptr)
        throw ELibMCDataInterfaceException (LIBMCDATA_ERROR_INVALIDPARAM);

    std::string sUUID = AMCCommon::CUtils::createUUID ();

    std::string sInsertQuery = "INSERT INTO buildjobdata (uuid, jobuuid, name, datatype, timestamp, storagestreamuuid, userid, active) VALUES (?, ?, ?, ?, ?, ?, ?, 1)";
    auto pInsertStatement = pSQLHandler->prepareStatement(sInsertQuery);
    pInsertStatement->setString(1, sUUID);
    pInsertStatement->setString(2, sJobUUID);
    pInsertStatement->setString(3, sName);
    pInsertStatement->setString(4, CBuildJobData::convertBuildJobDataTypeToString(eDataType));
    pInsertStatement->setString(5, sTimeStamp);
    pInsertStatement->setString(6, sStorageStreamUUID);
    pInsertStatement->setString(7, sUserID);
    pInsertStatement->execute();

    return make(sUUID, sIdentifier, sName, sJobUUID, eDataType, sTimeStamp, sStorageStreamUUID, sUserID, sSHA2, nStreamSize, pSQLHandler, pStoragePath);
}

PBuildJobData CBuildJobData::createSharedInDatabase(const std::string& sIdentifier, const std::string sName, const std::string & sJobUUID, LibMCData::eBuildJobDataType eDataType, std::string sTimeStamp, std::string sStorageStreamUUID, std::string sUserID, std::string& sSHA2, uint64_t nStreamSize, AMCData::PSQLHandler pSQLHandler, AMCData::PStoragePath pStoragePath)
{
    return std::shared_ptr<CBuildJobData>(createInDatabase(sIdentifier, sName, sJobUUID, eDataType, sTimeStamp, sStorageStreamUUID, sUserID, sSHA2, nStreamSize, pSQLHandler, pStoragePath));
}


std::string CBuildJobData::GetDataUUID()
{
    return m_sDataUUID;
}

std::string CBuildJobData::GetJobUUID()
{
    return m_sJobUUID;
}

std::string CBuildJobData::GetContextIdentifier()
{
    return m_sIdentifier;
}


std::string CBuildJobData::GetName()
{
    return m_sName;
}

std::string CBuildJobData::GetTimeStamp()
{
    return m_sTimeStamp;
}

IStorageStream * CBuildJobData::GetStorageStream()
{
	return CStorageStream::makeFromDatabase(m_sStorageStreamUUID, m_pSQLHandler, m_pStoragePath);
}

std::string CBuildJobData::GetStorageStreamSHA2()
{
    return m_sSHA256;
}

LibMCData_uint64 CBuildJobData::GetStorageStreamSize()
{
    return m_nStreamSize;
}


LibMCData::eBuildJobDataType CBuildJobData::GetDataType()
{
    return m_eDataType;
}

std::string CBuildJobData::GetDataTypeAsString()
{
    return convertBuildJobDataTypeToString(m_eDataType);
}


std::string CBuildJobData::GetMIMEType()
{
    auto pStorageStream = CStorageStream::makeSharedFromDatabase(m_sStorageStreamUUID, m_pSQLHandler, m_pStoragePath);
    return pStorageStream->GetMIMEType();
}

std::string CBuildJobData::convertBuildJobDataTypeToString(const LibMCData::eBuildJobDataType dataType)
{
    switch (dataType) {
    case LibMCData::eBuildJobDataType::Unknown: return "unknown";
    case LibMCData::eBuildJobDataType::Toolpath: return "toolpath";
    case LibMCData::eBuildJobDataType::Timeline: return "timeline";
    case LibMCData::eBuildJobDataType::CustomBinaryData: return "custombinarydata";
    case LibMCData::eBuildJobDataType::PNGImage: return "pngimage";
    case LibMCData::eBuildJobDataType::JPEGImage: return "jpegimage";
    case LibMCData::eBuildJobDataType::Thumbnail: return "thumbnail";
    default:
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDDATATYPE);
    }
}

LibMCData::eBuildJobDataType CBuildJobData::convertStringToBuildJobDataType(const std::string& sValue)
{
    if (sValue == "unknown")
        return LibMCData::eBuildJobDataType::Unknown;
    if (sValue == "toolpath")
        return LibMCData::eBuildJobDataType::Toolpath;
    if (sValue == "timeline")
        return LibMCData::eBuildJobDataType::Timeline;
    if (sValue == "custombinarydata")
        return LibMCData::eBuildJobDataType::CustomBinaryData;
    if (sValue == "pngimage")
        return LibMCData::eBuildJobDataType::PNGImage;
    if (sValue == "jpegimage")
        return LibMCData::eBuildJobDataType::JPEGImage;
    if (sValue == "thumbnail")
        return LibMCData::eBuildJobDataType::Thumbnail;

    throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDDATATYPE);

}

