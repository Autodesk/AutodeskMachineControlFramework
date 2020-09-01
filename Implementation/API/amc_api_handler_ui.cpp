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

#include "amc_api_handler_ui.hpp"
#include "amc_api_jsonrequest.hpp"
#include "amc_ui_handler.hpp"

#include "libmc_interfaceexception.hpp"
#include "libmcdata_dynamic.hpp"

#include "common_utils.hpp"

#include <vector>
#include <memory>
#include <string>
#include <iostream>


using namespace AMC;

CAPIHandler_UI::CAPIHandler_UI(PSystemState pSystemState)
	: m_pSystemState(pSystemState)
{
	if (pSystemState.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

}

CAPIHandler_UI::~CAPIHandler_UI()
{
	
}
				
std::string CAPIHandler_UI::getBaseURI ()
{
	return "api/ui";
}

APIHandler_UIType CAPIHandler_UI::parseRequest(const std::string& sURI, const eAPIRequestType requestType)
{
	// Leave away base URI
	auto sParameterString = AMCCommon::CUtils::toLowerString (sURI.substr(getBaseURI ().length ()));

	if (requestType == eAPIRequestType::rtGet) {

		if ((sParameterString == "/config/") || (sParameterString == "/config")) {
			return APIHandler_UIType::utConfiguration;
		}

	}

	return APIHandler_UIType::utUnknown;
}


void CAPIHandler_UI::checkAuthorizationMode(const std::string& sURI, const eAPIRequestType requestType, bool& bNeedsToBeAuthorized, bool& bCreateNewSession) 
{
	auto uiType = parseRequest(sURI, requestType);

	if (uiType == APIHandler_UIType::utConfiguration) {		

		bNeedsToBeAuthorized = false;
		bCreateNewSession = false;

	}
	else {
		bNeedsToBeAuthorized = true;
		bCreateNewSession = false;

	}
	
}


void CAPIHandler_UI::handleConfigurationRequest(CJSONWriter& writer, PAPIAuth pAuth)
{
	if (pAuth.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	writer.addString(AMC_API_KEY_UI_SESSIONID, pAuth->getSessionUUID());
	writer.addString(AMC_API_KEY_UI_SESSIONKEY, pAuth->getSessionKey());
	m_pSystemState->uiHandler()->writeToJSON(writer);

}



PAPIResponse CAPIHandler_UI::handleRequest(const std::string& sURI, const eAPIRequestType requestType, CAPIFormFields & pFormFields, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth)
{

	auto uiType = parseRequest(sURI, requestType);

	CJSONWriter writer;
	writeJSONHeader(writer, AMC_API_PROTOCOL_UI);

	switch (uiType) {
	case APIHandler_UIType::utConfiguration:
		handleConfigurationRequest(writer, pAuth);
		break;

	default:
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	}

	return std::make_shared<CAPIStringResponse>(AMC_API_HTTP_SUCCESS, AMC_API_CONTENTTYPE, writer.saveToString());

	return nullptr;
}



		

