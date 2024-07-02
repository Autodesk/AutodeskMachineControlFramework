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


Abstract: This is a stub class definition of CStorageStream

*/

#include "libmcdata_storagestream.hpp"
#include "libmcdata_interfaceexception.hpp"

// Include custom headers here.
#include "common_utils.hpp"
#include "common_importstream_native.hpp"

using namespace LibMCData::Impl;

/*************************************************************************************************************************
 Class definition of CStorageStream 
**************************************************************************************************************************/

uint32_t storageStream_ReadCallback(void* pBuffer, uint64_t nSize, void* pUserData)
{
	try {
		if (pBuffer == nullptr)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);
		if (pUserData == nullptr)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

		sStorageStreamCallbackData* pCallbackRecord = (sStorageStreamCallbackData*)pUserData;
		if (pCallbackRecord->m_nStructSign != STORAGESTREAM_RECORDSIGNATURE)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDSTORAGECALLBACKSIGNATURE);
		if (pCallbackRecord->m_pStream == nullptr)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDSTORAGECALLBACKSTREAM);

		auto pImportStream = pCallbackRecord->m_pStream->getImportStream();
		pImportStream->readBuffer((uint8_t*) pBuffer, nSize, true);

		return LIBMCDATA_SUCCESS;

	}
	catch (ELibMCDataInterfaceException& E) {
		return E.getErrorCode();
	}
	catch (...) {
		return LIBMCDATA_ERROR_GENERICEXCEPTION;
	}
		
}

uint32_t storageStream_SeekCallback(uint64_t nPosition, void* pUserData)
{
	try {
		if (pUserData == nullptr)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

		sStorageStreamCallbackData* pCallbackRecord = (sStorageStreamCallbackData*)pUserData;
		if (pCallbackRecord->m_nStructSign != STORAGESTREAM_RECORDSIGNATURE)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDSTORAGECALLBACKSIGNATURE);
		if (pCallbackRecord->m_pStream == nullptr)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDSTORAGECALLBACKSTREAM);

		auto pImportStream = pCallbackRecord->m_pStream->getImportStream();
		pImportStream->seekPosition(nPosition, true);

		return LIBMCDATA_SUCCESS;
	}
	catch (ELibMCDataInterfaceException& E) {
		return E.getErrorCode();
	}
	catch (...) {
		return LIBMCDATA_ERROR_GENERICEXCEPTION;
	}

}


CStorageStream::CStorageStream(AMCCommon::PImportStream pImportStream, const std::string& sUUID, const std::string& sContextIdentifier, const std::string& sName, const uint64_t nSize, const std::string& sMIMEType, const std::string& sSHA2, const std::string& sTimeStamp, const std::string& sUserID)
	: m_pImportStream (pImportStream),
	m_sUUID (AMCCommon::CUtils::normalizeUUIDString(sUUID)),
	m_sName (sName),
	m_nSize (nSize),
	m_sContextIdentifier (sContextIdentifier),
	m_sMIMEType (sMIMEType),
	m_sSHA2 (sSHA2),
	m_sTimeStamp (sTimeStamp),
	m_sUserID (sUserID)
{
	if (pImportStream.get() == nullptr)
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

	m_sStreamCallbackData.m_nStructSign = 0x3AB82D1E1FF1982A;
	m_sStreamCallbackData.m_pStream = this;
}


CStorageStream* CStorageStream::make(AMCCommon::PImportStream pImportStream, const std::string& sUUID, const std::string& sContextIdentifier, const std::string& sName, const uint64_t nSize, const std::string& sMIMEType, const std::string& sSHA2, const std::string& sTimeStamp, const std::string& sUserID)
{
	return new CStorageStream (pImportStream, sUUID, sContextIdentifier, sName, nSize, sMIMEType, sSHA2, sTimeStamp, sUserID);
}

CStorageStream* CStorageStream::makeFromDatabase(const std::string& sStreamUUID, AMCData::PSQLHandler pSQLHandler, AMCData::PStorageState pStorageState)
{
	if (pSQLHandler.get() == nullptr)
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);
	if (pStorageState.get() == nullptr)
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

	auto sParsedStreamUUID = AMCCommon::CUtils::normalizeUUIDString(sStreamUUID);

	std::string sQuery = "SELECT uuid, identifier, name, mimetype, sha2, size, userid, timestamp FROM storage_streams WHERE uuid=?";
	auto pStatement = pSQLHandler->prepareStatement(sQuery);
	pStatement->setString(1, sParsedStreamUUID);
	//pStatement->setString(2, AMCData::CStorageState::storageStreamStatusToString(AMCData::sssValidated));
	if (!pStatement->nextRow())
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_STORAGESTREAMNOTFOUND, "storage stream not found: " + sParsedStreamUUID);

	auto sUUID = pStatement->getColumnString(1);
	auto sContextIdentifier = pStatement->getColumnString(2);
	auto sName = pStatement->getColumnString(3);
	auto sMIMEType = pStatement->getColumnString(4);
	auto sSHA2 = pStatement->getColumnString(5);
	auto nSize = pStatement->getColumnInt64(6);
	auto sUserID = pStatement->getColumnString(7);
	auto sTimeStamp = pStatement->getColumnString(8);

	auto pImportStream = std::make_shared<AMCCommon::CImportStream_Native>(pStorageState->getStreamPath(sParsedStreamUUID));
	
	return make (pImportStream, sUUID, sContextIdentifier, sName, (uint64_t)nSize, sMIMEType, sSHA2, sTimeStamp, sUserID);
}

PStorageStream CStorageStream::makeShared(AMCCommon::PImportStream pImportStream, const std::string& sUUID, const std::string& sContextIdentifier, const std::string& sName, const uint64_t nSize, const std::string& sMIMEType, const std::string& sSHA2, const std::string& sTimeStamp, const std::string& sUserID)
{
	return std::shared_ptr<CStorageStream>(make(pImportStream, sUUID, sContextIdentifier, sName, nSize, sMIMEType, sSHA2, sTimeStamp, sUserID));
}

PStorageStream CStorageStream::makeSharedFromDatabase(const std::string& sStreamUUID, AMCData::PSQLHandler pSQLHandler, AMCData::PStorageState pStorageState)
{
	return std::shared_ptr<CStorageStream>(makeFromDatabase(sStreamUUID, pSQLHandler, pStorageState));
}


CStorageStream::~CStorageStream()
{
	m_sStreamCallbackData.m_nStructSign = 0;
	m_sStreamCallbackData.m_pStream = nullptr;
}


std::string CStorageStream::GetUUID()
{
	return m_sUUID;
}

std::string CStorageStream::GetTimeStamp()
{
	return m_sTimeStamp;
}

std::string CStorageStream::GetContextIdentifier()
{
	return m_sContextIdentifier;
}

std::string CStorageStream::GetName()
{
	return m_sName;
}

std::string CStorageStream::GetMIMEType()
{
	return m_sMIMEType;
}

std::string CStorageStream::GetSHA2()
{
	return m_sSHA2;
}

LibMCData_uint64 CStorageStream::GetSize()
{
	return m_nSize;
}

void CStorageStream::GetContent(LibMCData_uint64 nContentBufferSize, LibMCData_uint64* pContentNeededCount, LibMCData_uint8 * pContentBuffer)
{
	uint64_t nSize = m_pImportStream->retrieveSize();

	if (pContentNeededCount != nullptr) {
		*pContentNeededCount = nSize;
	}

	if (pContentBuffer != nullptr) {
		if (nContentBufferSize < nSize)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_BUFFERTOOSMALL);

		m_pImportStream->seekPosition(0, true);
		m_pImportStream->readBuffer(pContentBuffer, nSize, true);
	}

}


void CStorageStream::GetCallbacks(LibMCData_pvoid & pTheReadCallback, LibMCData_pvoid & pTheSeekCallback, LibMCData_pvoid & pStreamHandle)
{
	pTheReadCallback = (void*)&storageStream_ReadCallback;
	pTheSeekCallback = (void*)&storageStream_SeekCallback;
	pStreamHandle = (void*)&m_sStreamCallbackData;
}

AMCCommon::CImportStream* CStorageStream::getImportStream()
{
	return m_pImportStream.get();
}
