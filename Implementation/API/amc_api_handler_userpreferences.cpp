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

#include "amc_api_handler_userpreferences.hpp"
#include "amc_api_jsonrequest.hpp"

#include "libmc_interfaceexception.hpp"
#include "libmcdata_dynamic.hpp"

#include "common_utils.hpp"

#include <string>

using namespace AMC;

CAPIHandler_UserPreferences::CAPIHandler_UserPreferences(PSystemState pSystemState)
	: CAPIHandler(pSystemState->getClientHash()), m_pSystemState(pSystemState)
{
	if (pSystemState.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
}

CAPIHandler_UserPreferences::~CAPIHandler_UserPreferences()
{

}

std::string CAPIHandler_UserPreferences::getBaseURI()
{
	return "api/userpreferences";
}

bool CAPIHandler_UserPreferences::expectsRawBody(const std::string& sURI, const eAPIRequestType requestType)
{
	// The store and delete endpoints (both POST) carry a JSON payload.
	return (requestType == eAPIRequestType::rtPost);
}

APIHandler_UserPreferencesType CAPIHandler_UserPreferences::parseRequest(const std::string& sURI, const eAPIRequestType requestType)
{
	// Leave away base URI ("api/userpreferences" has 19 characters).
	auto sParameterString = AMCCommon::CUtils::toLowerString(sURI.substr(19));

	if (requestType == eAPIRequestType::rtPost) {
		if ((sParameterString == "/delete") || (sParameterString == "/delete/"))
			return APIHandler_UserPreferencesType::uptDelete;
		if ((sParameterString == "/") || (sParameterString == ""))
			return APIHandler_UserPreferencesType::uptStore;
	}

	if (requestType == eAPIRequestType::rtGet) {
		if ((sParameterString == "/") || (sParameterString == ""))
			return APIHandler_UserPreferencesType::uptRetrieve;
	}

	return APIHandler_UserPreferencesType::uptUnknown;
}

std::string CAPIHandler_UserPreferences::getScopedUserUUID(PAPIAuth pAuth)
{
	if (pAuth.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	std::string sUserUUID = pAuth->getUserUUID();
	if (sUserUUID.empty())
		return AMCCommon::CUtils::createEmptyUUID();

	return AMCCommon::CUtils::normalizeUUIDString(sUserUUID);
}

void CAPIHandler_UserPreferences::handleRetrieveRequest(CJSONWriter& writer, CAPIFormFields& pFormFields, PAPIAuth pAuth)
{
	std::string sUserUUID = getScopedUserUUID(pAuth);
	std::string sDomain = pFormFields.getRequestParameter(AMC_API_KEY_USERPREFERENCE_DOMAIN, true);
	std::string sKey = pFormFields.getRequestParameter(AMC_API_KEY_USERPREFERENCE_KEY, true);

	auto pDataModel = m_pSystemState->getDataModelInstance();
	auto pPreferenceHandler = pDataModel->CreateUserPreferenceHandler();

	bool bExists = pPreferenceHandler->HasUserPreference(sUserUUID, sDomain, sKey);
	writer.addBoolean(AMC_API_KEY_USERPREFERENCE_EXISTS, bExists);
	if (bExists)
		writer.addString(AMC_API_KEY_USERPREFERENCE_VALUE, pPreferenceHandler->RetrieveUserPreference(sUserUUID, sDomain, sKey));
	else
		writer.addString(AMC_API_KEY_USERPREFERENCE_VALUE, "");
}

void CAPIHandler_UserPreferences::handleStoreRequest(CJSONWriter& writer, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth)
{
	if (pBodyData == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	std::string sUserUUID = getScopedUserUUID(pAuth);

	CAPIJSONRequest jsonRequest(pBodyData, nBodyDataSize);
	std::string sDomain = jsonRequest.getRawString(AMC_API_KEY_USERPREFERENCE_DOMAIN, LIBMC_ERROR_INVALIDPARAM);
	std::string sKey = jsonRequest.getRawString(AMC_API_KEY_USERPREFERENCE_KEY, LIBMC_ERROR_INVALIDPARAM);
	// The value is an opaque, client-provided UTF8 payload (a stringified JSON document).
	std::string sValue = jsonRequest.getRawString(AMC_API_KEY_USERPREFERENCE_VALUE, LIBMC_ERROR_INVALIDPARAM);

	auto pGlobalChrono = m_pSystemState->globalChrono();
	auto pDataModel = m_pSystemState->getDataModelInstance();
	auto pPreferenceHandler = pDataModel->CreateUserPreferenceHandler();

	pPreferenceHandler->StoreUserPreference(sUserUUID, sDomain, sKey, sValue, pGlobalChrono->getUTCTimeStampInMicrosecondsSince1970());

	writer.addBoolean(AMC_API_KEY_USERPREFERENCE_EXISTS, true);
}

void CAPIHandler_UserPreferences::handleDeleteRequest(CJSONWriter& writer, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth)
{
	if (pBodyData == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	std::string sUserUUID = getScopedUserUUID(pAuth);

	CAPIJSONRequest jsonRequest(pBodyData, nBodyDataSize);
	std::string sDomain = jsonRequest.getRawString(AMC_API_KEY_USERPREFERENCE_DOMAIN, LIBMC_ERROR_INVALIDPARAM);
	std::string sKey = jsonRequest.getRawString(AMC_API_KEY_USERPREFERENCE_KEY, LIBMC_ERROR_INVALIDPARAM);

	auto pDataModel = m_pSystemState->getDataModelInstance();
	auto pPreferenceHandler = pDataModel->CreateUserPreferenceHandler();

	bool bExisted = pPreferenceHandler->DeleteUserPreference(sUserUUID, sDomain, sKey);
	writer.addBoolean(AMC_API_KEY_USERPREFERENCE_EXISTS, bExisted);
}

PAPIResponse CAPIHandler_UserPreferences::handleRequest(const std::string& sURI, const eAPIRequestType requestType, CAPIFormFields& pFormFields, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth)
{
	auto requestSubType = parseRequest(sURI, requestType);

	CJSONWriter writer;
	writeJSONHeader(writer, AMC_API_PROTOCOL_USERPREFERENCES);

	switch (requestSubType) {
	case APIHandler_UserPreferencesType::uptRetrieve:
		handleRetrieveRequest(writer, pFormFields, pAuth);
		break;
	case APIHandler_UserPreferencesType::uptStore:
		handleStoreRequest(writer, pBodyData, nBodyDataSize, pAuth);
		break;
	case APIHandler_UserPreferencesType::uptDelete:
		handleDeleteRequest(writer, pBodyData, nBodyDataSize, pAuth);
		break;
	default:
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
	}

	return std::make_shared<CAPIStringResponse>(AMC_API_HTTP_SUCCESS, AMC_API_CONTENTTYPE, writer.saveToString());
}
