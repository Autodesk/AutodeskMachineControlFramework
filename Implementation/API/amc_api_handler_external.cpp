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

#include "amc_api_handler_external.hpp"
#include "amc_api_jsonrequest.hpp"
#include "amc_ui_handler.hpp"
#include "amc_ui_module_item.hpp"

#define __AMCIMPL_UI_DIALOG
#define __AMCIMPL_UI_PAGE
#define __AMCIMPL_UI_MODULE

#include "amc_ui_page.hpp"
#include "amc_ui_dialog.hpp"
#include "amc_ui_module.hpp"
#include "amc_ui_clientaction.hpp"
#include "amc_meshentity.hpp"
#include "amc_meshhandler.hpp"
#include "amc_dataserieshandler.hpp"

#include "libmc_interfaceexception.hpp"
#include "libmcdata_dynamic.hpp"

#include "common_utils.hpp"
#include "common_chrono.hpp"

#include <cmath>
#include <vector>
#include <memory>
#include <string>
#include <iostream>

using namespace AMC;

CAPIHandler_External::CAPIHandler_External (PSystemState pSystemState)
	: CAPIHandler(pSystemState->getClientHash()), m_pSystemState(pSystemState)
{
	if (pSystemState.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);


}

CAPIHandler_External::~CAPIHandler_External()
{
	
}
				
std::string CAPIHandler_External::getBaseURI ()
{
	return "api/ext";
}



void CAPIHandler_External::checkAuthorizationMode(const std::string& sURI, const eAPIRequestType requestType, bool& bNeedsToBeAuthorized, bool& bCreateNewSession)
{
	bNeedsToBeAuthorized = true;
	bCreateNewSession = false;
	
}

bool CAPIHandler_External::expectsRawBody(const std::string& sURI, const eAPIRequestType requestType)
{
	return true;
}


void CAPIHandler_External::handleEventRequest(CJSONWriter& writer, const std::string& sEventName, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth)
{
	if (pAuth.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
	if (pBodyData == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString (sEventName))
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDEVENTNAME, "invalid external event name: " + sEventName);

	CAPIJSONRequest apiRequest(pBodyData, nBodyDataSize);
	std::string sSenderUUID;
	if (apiRequest.hasValue (AMC_API_KEY_UI_EVENTSENDER))
		sSenderUUID = apiRequest.getUUID(AMC_API_KEY_UI_EVENTSENDER, LIBMC_ERROR_INVALIDEVENTSENDER);

	std::string sEventParameterJSON;
	if (apiRequest.hasValue(AMC_API_KEY_UI_EVENTPARAMETERS)) {
		sEventParameterJSON = apiRequest.getJSONObjectString(AMC_API_KEY_UI_FORMVALUEJSON, LIBMC_ERROR_INVALIDFORMVALUES);
	}
	
	auto pUIHandler = m_pSystemState->uiHandler();

	auto pEventResult = pUIHandler->handleEvent(sEventName, sSenderUUID, "", sEventParameterJSON, pAuth);

	CJSONWriterArray contentUpdateNode(writer);

	if (pEventResult.getErrorCode())
	{
		writer.addInteger(AMC_API_KEY_UI_EVENTERRORCODE, pEventResult.getErrorCode ());
		writer.addString(AMC_API_KEY_UI_EVENTERRORMESSAGE, pEventResult.getErrorMessage());
	} 
	else {

		auto& clientActions = pEventResult.getClientActions();
		if (clientActions.size() > 0) {
			AMC::CJSONWriterArray actionsArray(writer);
			for (auto pAction : clientActions) {
				AMC::CJSONWriterObject actionObject(writer);
				pAction->writeToJSON(writer, actionObject);
				actionsArray.addObject(actionObject);
			}

			writer.addArray(AMC_API_KEY_UI_EVENTACTIONS, actionsArray);
		}

		auto& returnValues = pEventResult.getReturnValues();
		
		AMC::CJSONWriterObject actionsArray(writer);
		for (auto & returnValueIter : returnValues) {

		}

		
	}

	if (!contentUpdateNode.isEmpty()) {
		writer.addArray(AMC_API_KEY_UI_CONTENTUPDATE, contentUpdateNode);
	}


}


PAPIResponse CAPIHandler_External::handleRequest(const std::string& sURI, const eAPIRequestType requestType, CAPIFormFields & pFormFields, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth)
{
	// Leave away base URI
	auto sParameterString = AMCCommon::CUtils::toLowerString(sURI.substr(getBaseURI().length()));

	if (sParameterString.empty())
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDEVENTNAME, "empty external event name");

	if (sParameterString.substr(0, 1) != "/")
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDAPIREQUESTTYPE, "invalid API end point: " + sURI);

	std::string sEventName = sParameterString.substr(1);

	CJSONWriter writer;
	writeJSONHeader(writer, AMC_API_PROTOCOL_EXTERNAL);

	handleEventRequest (writer, sEventName, pBodyData, nBodyDataSize, pAuth);

	return std::make_shared<CAPIStringResponse>(AMC_API_HTTP_SUCCESS, AMC_API_CONTENTTYPE, writer.saveToString());
}



		

