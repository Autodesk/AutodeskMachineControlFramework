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

#include "amc_api_handler_upload.hpp"
#include "amc_api_jsonrequest.hpp"

#include "libmc_interfaceexception.hpp"
#include "libmcdata_dynamic.hpp"

#include "common_utils.hpp"

#include <vector>
#include <memory>
#include <string>
#include <iostream>


using namespace AMC;

CAPIHandler_Upload::CAPIHandler_Upload(PSystemState pSystemState)
	: CAPIHandler(pSystemState->getClientHash()), m_pSystemState(pSystemState)
{
	if (pSystemState.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	m_StreamUploadFields.push_back(CAPIFieldDetails(AMC_API_KEY_UPLOAD_DATASIZE, false, true));
	m_StreamUploadFields.push_back(CAPIFieldDetails(AMC_API_KEY_UPLOAD_DATAOFFSET, false, false));
	m_StreamUploadFields.push_back(CAPIFieldDetails(AMC_API_KEY_UPLOAD_DATA, true, true));
}

CAPIHandler_Upload::~CAPIHandler_Upload()
{
	
}
				
std::string CAPIHandler_Upload::getBaseURI () 
{
	return "api/upload";
}

APIHandler_UploadType CAPIHandler_Upload::parseRequest(const std::string& sURI, const eAPIRequestType requestType, std::string& uploadUUID)
{
	// Leave away base URI
	auto sParameterString = AMCCommon::CUtils::toLowerString (sURI.substr(10));
	uploadUUID = "";
	if (requestType == eAPIRequestType::rtPut) {
		if ((sParameterString.substr(0, 1) == "/") && (sParameterString.length() == 37)) {
			uploadUUID = AMCCommon::CUtils::normalizeUUIDString(sParameterString.substr(1));
			return utStreamUpload;
		}
	}


	if (requestType == eAPIRequestType::rtPost) {
		if ((sParameterString == "") || (sParameterString == "/")) {
			return utInitUpload;
		}
		else {

			if (sParameterString == "/finish") {
				return utFinishUpload;
			}

			if ((sParameterString.substr(0,1) == "/") && (sParameterString.length() == 37)) {
				uploadUUID = AMCCommon::CUtils::normalizeUUIDString(sParameterString.substr(1));
				return utStreamUpload;
			}
		}
	
	}

	return utUnknown;
}


bool CAPIHandler_Upload::expectsRawBody(const std::string& sURI, const eAPIRequestType requestType)
{
	std::string uploadUUID;

	switch (parseRequest(sURI, requestType, uploadUUID)) {
		case utInitUpload:
		case utFinishUpload:
			return true;

		default:
			return false;
	}
}


uint32_t CAPIHandler_Upload::getFormDataFieldCount(const std::string& sURI, const eAPIRequestType requestType)
{
	std::string uploadUUID;
	auto uploadType = parseRequest(sURI, requestType, uploadUUID);

	switch (uploadType) {
		case utStreamUpload: return (uint32_t) m_StreamUploadFields.size();
		default: return 0;
	}
	
}

CAPIFieldDetails CAPIHandler_Upload::getFormDataFieldDetails(const std::string& sURI, const eAPIRequestType requestType, const uint32_t nFieldIndex)
{
	std::string uploadUUID;
	auto uploadType = parseRequest(sURI, requestType, uploadUUID);

	switch (uploadType) {
	case utStreamUpload: 
		if (nFieldIndex >= m_StreamUploadFields.size ())
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDINDEX);

		return m_StreamUploadFields [nFieldIndex];

	default: 
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDINDEX);
	}

	
}


void CAPIHandler_Upload::handleInitUploadRequest(CJSONWriter& writer, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth)
{
	if (pBodyData == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
	if (pAuth.get () == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	auto sUUID = AMCCommon::CUtils::createUUID();

	auto pDataModel = m_pSystemState->getDataModelInstance();
	auto pStorage = pDataModel->CreateStorage();
	auto pGlobalChrono = m_pSystemState->globalChrono();

	CAPIJSONRequest jsonRequest(pBodyData, nBodyDataSize);

	auto sContext = jsonRequest.getNameString(AMC_API_KEY_UPLOAD_CONTEXT, LIBMC_ERROR_INVALIDCONTEXTUUID);
	auto sName = jsonRequest.getRawString(AMC_API_KEY_UPLOAD_NAME, LIBMC_ERROR_INVALIDUPLOADNAME);
	auto sMimeType = jsonRequest.getNameString(AMC_API_KEY_UPLOAD_MIMETYPE, LIBMC_ERROR_INVALIDMIMETYPE);
	auto nSize = jsonRequest.getUint64(AMC_API_KEY_UPLOAD_SIZE, 1, pStorage->GetMaxStreamSize(), LIBMC_ERROR_INVALIDSTREAMSIZE);

	std::string sContextUUID;
	std::string sContextIdentifier;

	if (sContext == "build") {
		sContextUUID = createNewBuild (sName, sUUID, pAuth);
		sContextIdentifier = "builddata";
	}
	else {
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDCONTEXTUUID);
	}
		
	if (!pStorage->ContentTypeIsAccepted(sMimeType))
		throw ELibMCInterfaceException(LIBMC_ERROR_CONTENTTYPENOTACCEPTED);
	
	if (!pAuth->contextUUIDIsAuthorized(sContextUUID))
		throw ELibMCInterfaceException(LIBMC_ERROR_CONTEXTUUIDNOTACCEPTED);

	pStorage->BeginPartialStream (sUUID, sName, sMimeType, nSize, pAuth->getUserName (), pGlobalChrono->getUTCTimeStampInMicrosecondsSince1970 ());

	writer.addString(AMC_API_KEY_UPLOAD_STREAMUUID, sUUID);
	writer.addString(AMC_API_KEY_UPLOAD_CONTEXTUUID, sContextUUID);

}

void CAPIHandler_Upload::handleFinishUploadRequest(CJSONWriter& writer, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth)
{
	if (pBodyData == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
	if (pAuth.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	auto pDataModel = m_pSystemState->getDataModelInstance();
	auto pStorage = pDataModel->CreateStorage();

	CAPIJSONRequest jsonRequest(pBodyData, nBodyDataSize);

	auto sUUID = jsonRequest.getUUID(AMC_API_KEY_UPLOAD_STREAMUUID, LIBMC_ERROR_INVALIDSTREAMUUID);

	if (jsonRequest.hasValue(AMC_API_KEY_UPLOAD_SHA256)) {
		auto sSHA256 = jsonRequest.getSHA256(AMC_API_KEY_UPLOAD_SHA256, LIBMC_ERROR_INVALIDSHA256SUM);
		pStorage->FinishPartialStream(sUUID, sSHA256);
	}
	else if (jsonRequest.hasValue(AMC_API_KEY_UPLOAD_SHA256BLOCK64K)) {
		auto sSHA256Block64k = jsonRequest.getSHA256(AMC_API_KEY_UPLOAD_SHA256BLOCK64K, LIBMC_ERROR_INVALIDSHA256SUM);
		pStorage->FinishPartialStreamBlockwiseSHA256(sUUID, sSHA256Block64k);
	}
	else
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDSHA256SUM);


	auto pStreamObject = pStorage->RetrieveStream (sUUID);
	writer.addString(AMC_API_KEY_UPLOAD_UUID, pStreamObject->GetUUID());
	writer.addString(AMC_API_KEY_UPLOAD_NAME, pStreamObject->GetName());
	writer.addString(AMC_API_KEY_UPLOAD_SHA256, pStreamObject->GetSHA2 ());
	writer.addString(AMC_API_KEY_UPLOAD_MIMETYPE, pStreamObject->GetMIMEType());
	writer.addInteger(AMC_API_KEY_UPLOAD_SIZE, pStreamObject->GetSize());
	writer.addString(AMC_API_KEY_UPLOAD_TIMESTAMP, pStreamObject->GetTimeStamp());




}

void CAPIHandler_Upload::handleStreamUploadRequest(CJSONWriter& writer, CAPIFormFields & pFormFields, const std::string& sStreamUUID, PAPIAuth pAuth)
{
	if (pAuth.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
	
	auto pData = pFormFields.getDataField(AMC_API_KEY_UPLOAD_DATA);
	uint64_t nSize = pFormFields.getUint64Field(AMC_API_KEY_UPLOAD_DATASIZE);
	uint64_t nOffset = 0;
	if (pFormFields.hasStringField (AMC_API_KEY_UPLOAD_DATAOFFSET))
		nOffset = pFormFields.getUint64Field(AMC_API_KEY_UPLOAD_DATAOFFSET);

	if (nSize != (uint64_t)pData->size())
		throw ELibMCInterfaceException(LIBMC_ERROR_UPLOADSIZEMISMATCH);

	auto pDataModel = m_pSystemState->getDataModelInstance();
	auto pStorage = pDataModel->CreateStorage();
	pStorage->StorePartialStream(sStreamUUID, nOffset, *pData);

}


		
PAPIResponse CAPIHandler_Upload::handleRequest(const std::string& sURI, const eAPIRequestType requestType, CAPIFormFields & pFormFields, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth)
{


	if ((requestType == eAPIRequestType::rtPost) || (requestType == eAPIRequestType::rtPut)) {

		std::string sUploadUUID;
		auto uploadType = parseRequest(sURI, requestType, sUploadUUID);

		CJSONWriter writer;
		writeJSONHeader(writer, AMC_API_PROTOCOL_UPLOAD);

		switch (uploadType) {
			case utInitUpload:
				handleInitUploadRequest (writer, pBodyData, nBodyDataSize, pAuth);
				break;

			case utFinishUpload:
				handleFinishUploadRequest(writer, pBodyData, nBodyDataSize, pAuth);
				break;

			case utStreamUpload:
				handleStreamUploadRequest(writer, pFormFields, sUploadUUID, pAuth);
				break;

			default:
				throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

		}

		return std::make_shared<CAPIStringResponse>(AMC_API_HTTP_SUCCESS, AMC_API_CONTENTTYPE, writer.saveToString());
	}


	return nullptr;
}


std::string CAPIHandler_Upload::createNewBuild(const std::string& sName, const std::string& sStorageStreamUUID, PAPIAuth pAuth)
{
	if (pAuth.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
	std::string sBuildUUID = AMCCommon::CUtils::createUUID();

	auto pGlobalChrono = m_pSystemState->globalChrono();
	auto pDataModel = m_pSystemState->getDataModelInstance();
	auto pBuildJobHandler = pDataModel->CreateBuildJobHandler();
	pBuildJobHandler->CreateJob(sBuildUUID, sName, pAuth->getUserUUID(), sStorageStreamUUID, pGlobalChrono->getUTCTimeStampInMicrosecondsSince1970 ());

	return sBuildUUID;
}

		

