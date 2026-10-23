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

#include "amc_api_handler_event.hpp"
#include "amc_api_jsonrequest.hpp"
#include "amc_ui_handler.hpp"
#include "amc_ui_clientaction.hpp"

#include "libmc_interfaceexception.hpp"

using namespace AMC;

CAPIHandler_Event::CAPIHandler_Event(PSystemState pSystemState)
	: CAPIHandler(pSystemState->getClientHash()), m_pSystemState(pSystemState)
{
	if (pSystemState.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
}

CAPIHandler_Event::~CAPIHandler_Event()
{
}

std::string CAPIHandler_Event::getBaseURI()
{
	return "api/event";
}

bool CAPIHandler_Event::expectsRawBody(const std::string& sURI, const eAPIRequestType requestType)
{
	sURI;
	return (requestType == eAPIRequestType::rtPost);
}

void CAPIHandler_Event::handleEventRequest(CJSONWriter& writer, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth)
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

	std::string sEventParameterJSON;
	if (apiRequest.hasValue(AMC_API_KEY_STATUSPARAMETERGROUP_PARAMETERS))
		sEventParameterJSON = apiRequest.getJSONObjectString(AMC_API_KEY_STATUSPARAMETERGROUP_PARAMETERS, LIBMC_ERROR_INVALIDFORMVALUES);

	auto pUIHandler = m_pSystemState->uiHandler();
	auto pEventResult = pUIHandler->handleEvent(sEventName, sSenderUUID, sFormValueJSON, sEventParameterJSON, pAuth);

	if (pEventResult.getErrorCode()) {
		writer.addInteger(AMC_API_KEY_UI_EVENTERRORCODE, pEventResult.getErrorCode());
		writer.addString(AMC_API_KEY_UI_EVENTERRORMESSAGE, pEventResult.getErrorMessage());
	}
	else {
		CJSONWriterArray actionsArray(writer);
		for (auto pAction : pEventResult.getClientActions()) {
			CJSONWriterObject actionObject(writer);
			pAction->writeToJSON(writer, actionObject);
			actionsArray.addObject(actionObject);
		}

		writer.addArray(AMC_API_KEY_UI_EVENTACTIONS, actionsArray);
	}
}

PAPIResponse CAPIHandler_Event::handleRequest(const std::string& sURI, const eAPIRequestType requestType, CAPIFormFields& pFormFields, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth)
{
	sURI;
	pFormFields;

	if (requestType != eAPIRequestType::rtPost)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	CJSONWriter writer;
	writeJSONHeader(writer, AMC_API_PROTOCOL_UI);
	handleEventRequest(writer, pBodyData, nBodyDataSize, pAuth);

	return std::make_shared<CAPIStringResponse>(AMC_API_HTTP_SUCCESS, AMC_API_CONTENTTYPE, writer.saveToString());
}

