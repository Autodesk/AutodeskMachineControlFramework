/*++

Copyright (C) 2026 Autodesk Inc.

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

#define __AMCIMPL_API_CONSTANTS

#include "amc_api_handler_validate.hpp"
#include "amc_api_jsonrequest.hpp"
#include "amc_ui_handler.hpp"

#include "libmc_interfaceexception.hpp"

#include "amc_api_constants.hpp"

#include "RapidJSON/document.h"

using namespace AMC;

static const std::string VALIDATION_ERROR_PREFIX = "VALIDATIONERROR:";

CAPIHandler_Validate::CAPIHandler_Validate(PSystemState pSystemState)
	: CAPIHandler(pSystemState->getClientHash()), m_pSystemState(pSystemState)
{
	if (pSystemState.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
}

CAPIHandler_Validate::~CAPIHandler_Validate()
{
}

std::string CAPIHandler_Validate::getBaseURI()
{
	return "api/validate";
}

bool CAPIHandler_Validate::expectsRawBody(const std::string& sURI, const eAPIRequestType requestType)
{
	sURI;
	return (requestType == eAPIRequestType::rtPost);
}

void CAPIHandler_Validate::handleValidateRequest(CJSONWriter& writer, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth)
{
	if (pAuth.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
	if (pBodyData == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	CAPIJSONRequest apiRequest(pBodyData, nBodyDataSize);
	auto sEventName = apiRequest.getNameString(AMC_API_KEY_UI_EVENTNAME, LIBMC_ERROR_EVENTNAMENOTFOUND);

	std::string sSenderUUID;
	if (apiRequest.hasValue(AMC_API_KEY_UI_EVENTSENDER))
		sSenderUUID = apiRequest.getUUID(AMC_API_KEY_UI_EVENTSENDER, LIBMC_ERROR_INVALIDEVENTSENDER);

	std::string sFormValueJSON;
	if (apiRequest.hasValue(AMC_API_KEY_UI_FORMVALUEJSON))
		sFormValueJSON = apiRequest.getJSONObjectString(AMC_API_KEY_UI_FORMVALUEJSON, LIBMC_ERROR_INVALIDFORMVALUES);

	auto pUIHandler = m_pSystemState->uiHandler();

	try {
		pUIHandler->handleEvent(sEventName, sSenderUUID, sFormValueJSON, "", pAuth);
		writer.addBoolean(AMC_API_KEY_UI_FORMVALID, true);
	}
	catch (ELibMCInterfaceException& ex) {
		std::string sMessage = ex.what();
		if (sMessage.size() >= VALIDATION_ERROR_PREFIX.size() &&
		    sMessage.substr(0, VALIDATION_ERROR_PREFIX.size()) == VALIDATION_ERROR_PREFIX) {

			std::string sErrorsJSON = sMessage.substr(VALIDATION_ERROR_PREFIX.size());
			writer.addBoolean(AMC_API_KEY_UI_FORMVALID, false);

			rapidjson::Document errDoc;
			errDoc.Parse(sErrorsJSON.c_str());

			if (!errDoc.HasParseError() && errDoc.IsObject()) {
				CJSONWriterObject errObject(writer);
				errObject.copyFromObject(errDoc);
				writer.addObject(AMC_API_KEY_UI_FORMERRORS, errObject);
			}
			else {
				writer.addString(AMC_API_KEY_UI_FORMERRORS, sErrorsJSON);
			}
		}
		else {
			throw;
		}
	}
}

PAPIResponse CAPIHandler_Validate::handleRequest(const std::string& sURI, const eAPIRequestType requestType, CAPIFormFields& pFormFields, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth)
{
	sURI;
	pFormFields;

	if (requestType != eAPIRequestType::rtPost)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	CJSONWriter writer;
	writeJSONHeader(writer, AMC_API_PROTOCOL_UI);
	handleValidateRequest(writer, pBodyData, nBodyDataSize, pAuth);

	return std::make_shared<CAPIStringResponse>(AMC_API_HTTP_SUCCESS, AMC_API_CONTENTTYPE, writer.saveToString());
}
