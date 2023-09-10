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

#include "amc_api_handler_auth.hpp"
#include "amc_api_jsonrequest.hpp"
#include "amc_ui_handler.hpp"

#include "libmc_interfaceexception.hpp"
#include "libmcdata_dynamic.hpp"

#include "amc_userinformation.hpp"
#include "common_utils.hpp"

#include <vector>
#include <memory>
#include <string>
#include <iostream>

using namespace AMC;

CAPIHandler_Auth::CAPIHandler_Auth(PAPISessionHandler pSessionHandler, LibMCData::PLoginHandler pLoginHandler, const std::string& sInstallationSecret, const std::string& sGitHash, const std::string& sClientHash, PAccessControl pAccessControl)
	: CAPIHandler (sClientHash), m_pSessionHandler(pSessionHandler), m_pLoginHandler (pLoginHandler), m_sInstallationSecret(sInstallationSecret), m_sGitHash (sGitHash), m_pAccessControl (pAccessControl)
{

	if (pAccessControl.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
	if (pSessionHandler.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
	if (pLoginHandler.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

}

CAPIHandler_Auth::~CAPIHandler_Auth()
{
	
}
				
std::string CAPIHandler_Auth::getBaseURI ()
{
	return "api/auth";
}

bool CAPIHandler_Auth::expectsRawBody(const std::string& sURI, const eAPIRequestType requestType)
{
	return true;
}

APIHandler_AuthType CAPIHandler_Auth::parseRequest(const std::string& sURI, const eAPIRequestType requestType, std::string& sSessionUUID)
{
	// Leave away base URI
	auto sParameterString = AMCCommon::CUtils::toLowerString (sURI.substr(getBaseURI ().length ()));
	sSessionUUID = "";

	if (requestType == eAPIRequestType::rtPost) {

		if ((sParameterString == "/") || (sParameterString == "")) {
			return APIHandler_AuthType::atNewSession;
		}

		if (sParameterString.length() == 37) {
			if (sParameterString.substr(0, 1) == "/") {
				sSessionUUID = AMCCommon::CUtils::normalizeUUIDString(sParameterString.substr(1));
				return APIHandler_AuthType::atAuthorize;
			}
		}

	}

	if (requestType == eAPIRequestType::rtPost) {


	} 


	return APIHandler_AuthType::atUnknown;
}


void CAPIHandler_Auth::checkAuthorizationMode(const std::string& sURI, const eAPIRequestType requestType, bool& bNeedsToBeAuthorized, bool& bCreateNewSession)
{
	std::string sSessionUUID;
	auto authType = parseRequest(sURI, requestType, sSessionUUID);

	switch (authType) {
		case APIHandler_AuthType::atNewSession:
			bNeedsToBeAuthorized = false;
			bCreateNewSession = true;
			break;

		case APIHandler_AuthType::atAuthorize:
			bNeedsToBeAuthorized = false;
			bCreateNewSession = false;
			break;

		default:
			bNeedsToBeAuthorized = true;
			bCreateNewSession = false;		
	}

}


void CAPIHandler_Auth::handleNewSessionRequest(const uint8_t* pBodyData, const size_t nBodyDataSize, CJSONWriter& writer, PAPIAuth pAuth)
{
	CAPIJSONRequest pRequest(pBodyData, nBodyDataSize);

	if (pAuth.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	if (pAuth->userIsAuthorized ())
		throw ELibMCInterfaceException(LIBMC_ERROR_USERALREADYAUTHORIZED);

	auto sUserName = pRequest.getNameString(AMC_API_KEY_AUTH_USERNAME, LIBMC_ERROR_INVALIDUSERNAME);
	if (sUserName.empty ())
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDUSERNAME);	
	
	std::string sLoginSalt;
	if (m_pLoginHandler->UserExists (sUserName)) {
		std::string sHashedPassword;
		std::string sUserUUID;
		std::string sUserDescription;
		std::string sUserRoleIdentifier;
		std::string sUserLanguageIdentifier;

		// password hash is calculateSHA256FromString(sLoginSalt + "password"); 
		m_pLoginHandler->GetUserDetails(sUserName, sLoginSalt, sHashedPassword);
		m_pLoginHandler->GetUserProperties(sUserName, sUserUUID, sUserDescription, sUserRoleIdentifier, sUserLanguageIdentifier);
				
		if (sUserRoleIdentifier.empty())
			sUserRoleIdentifier = m_pAccessControl->getDefaultRole()->getIdentifier();

		m_pSessionHandler->setUserDetailsForSession(pAuth->getSessionUUID(), sUserName, sHashedPassword, sUserUUID, sUserDescription, sUserRoleIdentifier, sUserLanguageIdentifier);
	}
	else {
		// If user has not been found, then generate a repeatable salt to not show that the user is not existing.
		sLoginSalt = AMCCommon::CUtils::calculateSHA256FromString(m_sGitHash + m_sInstallationSecret + AMCCommon::CUtils::calculateSHA256FromString (sUserName));
	}

	writer.addString(AMC_API_KEY_AUTH_SESSIONUUID, pAuth->getSessionUUID());
	writer.addString(AMC_API_KEY_AUTH_SESSIONKEY, pAuth->getSessionKey());
	writer.addString(AMC_API_KEY_AUTH_LOGINSALT, sLoginSalt);
	writer.addString(AMC_API_KEY_AUTH_USERNAME, sUserName);
}

void CAPIHandler_Auth::handleAuthorizeRequest(const uint8_t* pBodyData, const size_t nBodyDataSize, CJSONWriter& writer, std::string& sSessionUUID, PAPIAuth pAuth)
{
	CAPIJSONRequest pRequest(pBodyData, nBodyDataSize);

	if (pAuth.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	if (pAuth->userIsAuthorized())
		throw ELibMCInterfaceException(LIBMC_ERROR_USERALREADYAUTHORIZED);

	auto pUserInformation = pAuth->getUserInformation();
	if (!m_pAccessControl->hasRole(pUserInformation->getRoleIdentifier())) {
		throw ELibMCInterfaceException(LIBMC_ERROR_USERHASUNKNOWNROLE, sSessionUUID);
	}

	auto sSaltedPassword = pRequest.getSHA256(AMC_API_KEY_AUTH_SALTEDPASSWORD, LIBMC_ERROR_INVALIDPASSWORD);
	auto sClientKey = pRequest.getSHA256(AMC_API_KEY_AUTH_CLIENTKEY, LIBMC_ERROR_INVALIDCLIENTKEY);	
	m_pSessionHandler->authorizeSession (sSessionUUID, sSaltedPassword, sClientKey);

	if (m_pSessionHandler->sessionIsAuthenticated(sSessionUUID)) {
		
		CJSONWriter tokenWriter;
		tokenWriter.addString(AMC_API_KEY_TOKEN_SESSION, sSessionUUID);
		tokenWriter.addString(AMC_API_KEY_TOKEN_TOKEN, m_pSessionHandler->getSessionToken(sSessionUUID));

		// Create Base64 token
		std::string sToken = AMCCommon::CUtils::encodeBase64(tokenWriter.saveToString(), AMCCommon::eBase64Type::URL);
		writer.addString(AMC_API_KEY_AUTH_TOKEN, sToken);
	}
	else {
		// this should not happen, but we want to be double sure here!
		throw ELibMCInterfaceException(LIBMC_ERROR_INTERNALERROR);
	}
}


PAPIResponse CAPIHandler_Auth::handleRequest(const std::string& sURI, const eAPIRequestType requestType, CAPIFormFields & pFormFields, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth)
{
	std::string sSessionUUID;
	auto authType = parseRequest(sURI, requestType, sSessionUUID);


	CJSONWriter writer;
	writeJSONHeader(writer, AMC_API_PROTOCOL_AUTH);

	switch (authType) {
	case APIHandler_AuthType::atNewSession:
		handleNewSessionRequest(pBodyData, nBodyDataSize, writer, pAuth);
		break;

	case APIHandler_AuthType::atAuthorize:
		handleAuthorizeRequest(pBodyData, nBodyDataSize, writer, sSessionUUID, pAuth);
		break;

	default:
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	}

	return std::make_shared<CAPIStringResponse>(AMC_API_HTTP_SUCCESS, AMC_API_CONTENTTYPE, writer.saveToString());
	
}



		

