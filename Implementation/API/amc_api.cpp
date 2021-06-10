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

#define __AMCIMPL_API_RESPONSE

#include "amc_api.hpp"
#include "amc_api_auth.hpp"
#include "amc_api_handler.hpp"
#include "amc_api_response.hpp"
#include "amc_api_jsonrequest.hpp"
#include "amc_api_sessionhandler.hpp"

#include "libmc_interfaceexception.hpp"
#include "amc_logger.hpp"


#include "common_utils.hpp"

#include <vector>
#include <memory>
#include <string>


using namespace AMC;

CAPI::CAPI()
{
	m_pSessionHandler = std::make_shared<CAPISessionHandler>();
}

CAPI::~CAPI()
{
}


PAPIHandler CAPI::getURIMatch(const std::string& sURI)
{
	for (auto pHandler : m_ApiHandlers) {
		std::string sBaseURI = pHandler->getBaseURI();
		bool bIsMatch;

		if (sBaseURI.length() > 0) {
			if (sBaseURI.length() < sURI.length()) {
				std::string sSlashURI = sBaseURI + "/";

				bIsMatch = (sSlashURI == sURI.substr(0, sSlashURI.length()));
			}
			else {
				bIsMatch = (sBaseURI == sURI);
			}
		}
		else {
			bIsMatch = true;
		}

		if (bIsMatch) {
			return pHandler;
		}
	}

	return nullptr;
}


bool CAPI::expectsRawBody(const std::string& sURI, const eAPIRequestType requestType)
{
	auto pHandler = getURIMatch(sURI);
	if (pHandler.get() == nullptr)
		return false;

	return pHandler->expectsRawBody(sURI, requestType);
}


std::string CAPI::removeLeadingSlashFromURI(const std::string& sURI)
{
	std::string sURIWithoutLeadingSlash;
	if (sURI.length() > 0) {
		if (sURI.substr(0, 1) == "/")
			sURIWithoutLeadingSlash = sURI.substr(1);
		else
			sURIWithoutLeadingSlash = sURI;
	}

	return sURIWithoutLeadingSlash;

}

eAPIRequestType CAPI::getRequestTypeFromString(const std::string& sRequestType)
{
	std::string sLowerCaseType = AMCCommon::CUtils::trimString(AMCCommon::CUtils::toLowerString(sRequestType));

	if (sLowerCaseType == "get")
		return AMC::eAPIRequestType::rtGet;
	if (sLowerCaseType == "post")
		return AMC::eAPIRequestType::rtPost;

	throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDAPIREQUESTTYPE);
}


void CAPI::checkAuthorizationMode(const std::string& sURI, const eAPIRequestType requestType, bool& bNeedsToBeAuthorized, bool& bCreateNewSession)
{

	bNeedsToBeAuthorized = true;
	bCreateNewSession = false;

	std::string sURIWithoutLeadingSlash = removeLeadingSlashFromURI(sURI);

	auto pHandler = getURIMatch(sURIWithoutLeadingSlash);
	if (pHandler.get() == nullptr)
		return;

	pHandler->checkAuthorizationMode(sURIWithoutLeadingSlash, requestType, bNeedsToBeAuthorized, bCreateNewSession);
}




PAPIResponse CAPI::handleRequest(const std::string& sURI, const eAPIRequestType requestType, const uint8_t* pData, uint64_t nCount, CAPIFormFields & pFormFields, PAPIAuth pAuth, CLogger* pLogger)
{
	auto pHandler = getURIMatch(sURI);
	if (pHandler.get() == nullptr)
		return makeError(AMC_API_HTTP_NOTFOUND, LIBMC_ERROR_URLNOTFOUND, "url not found: " + sURI);

	bool bNeedsToBeAuthorized = true;
	bool bCreateNewSession = false;
	pHandler->checkAuthorizationMode (sURI, requestType, bNeedsToBeAuthorized, bCreateNewSession);

	if (bNeedsToBeAuthorized && (!pAuth->userIsAuthorized ()))
		return makeError(AMC_API_HTTP_FORBIDDEN, LIBMC_ERROR_SESSIONNOTAUTHORIZED, "session not authorized");


	try {
		
		auto pResponse = pHandler->handleRequest (sURI, requestType, pFormFields, pData, nCount, pAuth);

		if (pResponse.get() == nullptr)
			return makeError(AMC_API_HTTP_NOTFOUND, LIBMC_ERROR_URLNOTFOUND, "url not found: " + sURI);
		
		return pResponse;		
	}
	catch (ELibMCInterfaceException& IntfException) {
		if (pLogger != nullptr)
			pLogger->logMessage(IntfException.what(), "api", eLogLevel::Message);
		return makeError(AMC_API_HTTP_BADREQUEST, IntfException.getErrorCode(), IntfException.what());
	}
	catch (std::exception& StdException) {
		if (pLogger != nullptr)
			pLogger->logMessage(StdException.what(), "api", eLogLevel::Message);
		return makeError(AMC_API_HTTP_BADREQUEST, LIBMC_ERROR_GENERICBADREQUEST, StdException.what());
	}
}

uint32_t CAPI::getFormDataFieldCount(const std::string& sURI, const eAPIRequestType requestType)
{
	auto pHandler = getURIMatch(sURI);
	if (pHandler.get() == nullptr)
		return false;

	return pHandler->getFormDataFieldCount(sURI, requestType);

}

CAPIFieldDetails CAPI::getFormDataFieldDetails(const std::string& sURI, const eAPIRequestType requestType, const uint32_t nFieldIndex)
{
	auto pHandler = getURIMatch(sURI);
	if (pHandler.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_URLNOTFOUND);

	return pHandler->getFormDataFieldDetails(sURI, requestType, nFieldIndex);

}


void CAPI::registerHandler(PAPIHandler pAPIHandler)
{
	if (pAPIHandler.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	m_ApiHandlers.push_back(pAPIHandler);
}


PAPIResponse CAPI::makeError(uint32_t nHTTPError, LibMCResult errorCode, const std::string& sErrorString)
{
	CJSONWriter writer;
	writer.addString(AMC_API_KEY_PROTOCOL, AMC_API_PROTOCOL_ERROR);
	writer.addString(AMC_API_KEY_VERSION, AMC_API_PROTOCOL_VERSION);
	writer.addInteger(AMC_API_KEY_ERRORCODE, errorCode);
	writer.addString(AMC_API_KEY_MESSAGE, sErrorString);
	return std::make_shared<CAPIStringResponse> (nHTTPError, AMC_API_CONTENTTYPE, writer.saveToString () );
}


PAPISessionHandler CAPI::getSessionHandler()
{
	return m_pSessionHandler;
}

