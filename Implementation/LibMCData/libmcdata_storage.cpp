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


Abstract: This is a stub class definition of CStorage

*/

#include "libmcdata_storage.hpp"
#include "libmcdata_storagestream.hpp"
#include "libmcdata_interfaceexception.hpp"

// Include custom headers here.
#include "common_utils.hpp"
#include "amcdata_storagestate.hpp"

#include "common_chrono.hpp"
#include "PicoSHA2/picosha2.h"

using namespace LibMCData::Impl;

/*************************************************************************************************************************
 Class definition of CStorage 
**************************************************************************************************************************/

CStorage::CStorage(AMCData::PSQLHandler pSQLHandler, AMCData::PStorageState pStorageState)
    : m_pSQLHandler (pSQLHandler), m_pStorageState(pStorageState)
{
    if (pStorageState.get() == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);
    if (pSQLHandler.get() == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

}


void CStorage::insertDBEntry(const std::string& sUUID, const std::string& sContextUUID, const std::string& sContextIdentifier, const std::string& sName, const std::string& sMimeType, const LibMCData_uint64 nSize, const std::string& sSHA2, const std::string& sUserID)
{

    auto pTransaction = m_pSQLHandler->beginTransaction();

    std::string sParsedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);
    std::string sParsedContextUUID = AMCCommon::CUtils::normalizeUUIDString(sContextUUID);

    AMCCommon::CChrono chrono;
    std::string sTimestamp = chrono.getStartTimeISO8601TimeUTC ();

    std::string sQuery = "SELECT uuid FROM storage_streams WHERE uuid=?";
    auto pStatement = pTransaction->prepareStatement(sQuery);
    pStatement->setString(1, sParsedUUID);
    if (pStatement->nextRow())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_DUPLICATESTORAGESTREAM);
    pStatement = nullptr;

    std::string sInsertQuery = "INSERT INTO storage_streams (uuid, identifier, name, mimetype, sha2, size, userid, timestamp, status) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)";
    auto pInsertStatement = pTransaction->prepareStatement(sInsertQuery);
    pInsertStatement->setString(1, sParsedUUID);
    pInsertStatement->setString(2, sContextIdentifier);
    pInsertStatement->setString(3, sName);
    pInsertStatement->setString(4, sMimeType);
    pInsertStatement->setString(5, sSHA2);
    pInsertStatement->setInt64(6, nSize);
    pInsertStatement->setString(7, sUserID);
    pInsertStatement->setString(8, sTimestamp);
    pInsertStatement->setString(9, AMCData::CStorageState::storageStreamStatusToString (AMCData::eStorageStreamStatus::sssNew));
    pInsertStatement->execute();
    pInsertStatement = nullptr;

    std::string sContextQuery = "INSERT INTO storage_context (streamuuid, contextuuid) VALUES (?, ?)";
    auto pContextStatement = pTransaction->prepareStatement(sContextQuery);
    pContextStatement->setString(1, sParsedUUID);
    pContextStatement->setString(2, sParsedContextUUID);
    pContextStatement->execute();
    pContextStatement = nullptr;

    pTransaction->commit();
}


bool CStorage::StreamIsReady(const std::string& sUUID)
{
    std::string sParsedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);

    std::string sQuery = "SELECT uuid FROM storage_streams WHERE uuid=? AND status=?";
    auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
    pStatement->setString(1, sParsedUUID);
    pStatement->setString(2, AMCData::CStorageState::storageStreamStatusToString(AMCData::eStorageStreamStatus::sssValidated));
    return (pStatement->nextRow());
}

IStorageStream* CStorage::RetrieveStream(const std::string& sUUID)
{
    return CStorageStream::makeFromDatabase(sUUID, m_pSQLHandler, m_pStorageState);
}

void CStorage::StoreNewStream(const std::string& sUUID, const std::string& sContextUUID, const std::string& sContextIdentifier, const std::string& sName, const std::string& sMimeType, const LibMCData_uint64 nContentBufferSize, const LibMCData_uint8* pContentBuffer, const std::string& sUserID)
{
    if (nContentBufferSize == 0)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDBUFFERSIZE);
    if (pContentBuffer == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);
    if (sName.empty ())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);
    if (sMimeType.empty())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);
    if (sUserID.empty())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);


    // Calculate SHA Hash (not under mutex lock)
    auto beginIter = static_cast<const unsigned char*>(pContentBuffer);
    auto endIter = static_cast<const unsigned char*>(pContentBuffer) + nContentBufferSize;

    picosha2::hash256_one_by_one hasher;
    hasher.process(beginIter, endIter);
    hasher.finish();

    std::vector<unsigned char> hash(picosha2::k_digest_size);
    hasher.get_hash_bytes(hash.begin(), hash.end());
    std::string sSHA256 = picosha2::get_hash_hex_string(hasher);
	
    // From here, we lock storage database write access
    {       
        insertDBEntry(sUUID, sContextUUID, sContextIdentifier, sName, sMimeType, nContentBufferSize, sSHA256, sUserID);        
    }

    std::string sCalculatedSHA256, sCalculatedBlockSHA256;

    // Store data asynchroniously on disk and finalize when writing has finished
    auto pWriter = std::make_shared<AMCData::CStorageWriter>(sUUID, m_pStorageState->getStreamPath (sUUID), nContentBufferSize);
    pWriter->writeChunkAsync(pContentBuffer, nContentBufferSize, 0);
    pWriter->finalize(sSHA256, "", sCalculatedSHA256, sCalculatedBlockSHA256);
 
}

void CStorage::BeginPartialStream(const std::string& sUUID, const std::string& sContextUUID, const std::string& sContextIdentifier, const std::string& sName, const std::string& sMimeType, const LibMCData_uint64 nSize, const std::string& sUserID)
{

    if (nSize == 0)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDBUFFERSIZE);
    if (sName.empty())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);
    if (sMimeType.empty())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);
    if (sUserID.empty())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

    {
        std::string sParsedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);
        insertDBEntry(sParsedUUID, sContextUUID, sContextIdentifier, sName, sMimeType, nSize, "", sUserID);

        auto pWriter = std::make_shared<AMCData::CStorageWriter>(sParsedUUID, m_pStorageState->getStreamPath(sUUID), nSize);
        m_pStorageState->addPartialWriter(pWriter);
    }
    
}

void CStorage::StorePartialStream(const std::string & sUUID, const LibMCData_uint64 nOffset, const LibMCData_uint64 nContentBufferSize, const LibMCData_uint8 * pContentBuffer)
{

    if (nContentBufferSize == 0)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDBUFFERSIZE);
    if (pContentBuffer == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

    std::string sParsedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);
    auto pWriter = m_pStorageState->findPartialWriter(sParsedUUID, false);
    if (pWriter == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARTIALUPLOAD, "invalid partial upload: " + sParsedUUID);

    pWriter->writeChunkAsync(pContentBuffer, nContentBufferSize, nOffset);
}

void CStorage::FinishPartialStream(const std::string& sUUID, const std::string& sSHA2)
{
    FinishPartialStreamEx(sUUID, sSHA2, "");
}

void CStorage::FinishPartialStreamBlockwiseSHA256(const std::string& sUUID, const std::string& sBlockwiseSHA2)
{
    FinishPartialStreamEx(sUUID, "", sBlockwiseSHA2);
}


void CStorage::FinishPartialStreamEx(const std::string& sUUID, const std::string& sSHA2, const std::string& sBlockwiseSHA2)
{

    std::string sParsedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);

    std::string sNormalizedSHA2;
    if (!sSHA2.empty ())
        sNormalizedSHA2 = AMCCommon::CUtils::normalizeSHA256String(sSHA2);

    std::string sNormalizedBlockwiseSHA2;
    if (!sBlockwiseSHA2.empty())
        sNormalizedBlockwiseSHA2 = AMCCommon::CUtils::normalizeSHA256String(sBlockwiseSHA2);

    auto pWriter = m_pStorageState->findPartialWriter (sParsedUUID, true);
    m_pStorageState->deletePartialWriter (sParsedUUID);

    // Write data and calculate checksums
    std::string sCalculatedSHA256, sCalculatedBlockSHA256;
    pWriter->finalize (sNormalizedSHA2, sNormalizedBlockwiseSHA2, sCalculatedSHA256, sCalculatedBlockSHA256);

    std::string sUpdateQuery = "UPDATE storage_streams SET status=?, sha2=?, sha256_block64k=? WHERE uuid=? AND status=?";
    auto pUpdateStatement = m_pSQLHandler->prepareStatement(sUpdateQuery);
    pUpdateStatement->setString(1, AMCData::CStorageState::storageStreamStatusToString(AMCData::eStorageStreamStatus::sssValidated));
    pUpdateStatement->setString(2, sCalculatedSHA256);
    pUpdateStatement->setString(3, sCalculatedBlockSHA256);
    pUpdateStatement->setString(4, sUUID);
    pUpdateStatement->setString(5, AMCData::CStorageState::storageStreamStatusToString(AMCData::eStorageStreamStatus::sssNew));
    pUpdateStatement->execute();

}


LibMCData_uint64 CStorage::GetMaxStreamSize()
{
    return 1024ULL * 1024ULL * 1024ULL * 32ULL; // 32GB Maximum stream data size
}

bool CStorage::ContentTypeIsAccepted(const std::string& sContentType)
{
    auto sLowerContentType = AMCCommon::CUtils::toLowerString(AMCCommon::CUtils::trimString(sContentType));

    return m_pStorageState->isAcceptedContent(sLowerContentType);
}

bool CStorage::StreamIsImage(const std::string& sUUID) 
{
    std::string sParsedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);

    std::string sQuery = "SELECT mimetype FROM storage_streams WHERE uuid=?";
    auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
    pStatement->setString(1, sParsedUUID);
    if (!pStatement->nextRow())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_STORAGESTREAMNOTFOUND);

    auto sMimeType = pStatement->getColumnString(1);
    auto sLowerMimeType = AMCCommon::CUtils::toLowerString(AMCCommon::CUtils::trimString(sMimeType));

    return m_pStorageState->isImageContent(sLowerMimeType);

}



