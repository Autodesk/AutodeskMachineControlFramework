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

#include "amc_api_handler_widget.hpp"
#include "amc_api_jsonrequest.hpp"
#include "amc_ui_handler.hpp"
#include "amc_ui_module_item.hpp"

#include "libmc_interfaceexception.hpp"
#include "common_utils.hpp"

using namespace AMC;

CAPIHandler_Widget::CAPIHandler_Widget(PSystemState pSystemState)
	: CAPIHandler(pSystemState->getClientHash()), m_pSystemState(pSystemState)
{
	if (pSystemState.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
}

CAPIHandler_Widget::~CAPIHandler_Widget()
{
}

std::string CAPIHandler_Widget::getBaseURI()
{
	return "api/widget";
}

bool CAPIHandler_Widget::expectsRawBody(const std::string& sURI, const eAPIRequestType requestType)
{
	sURI;
	return (requestType == eAPIRequestType::rtPost);
}

void CAPIHandler_Widget::handleWidgetRequest(CJSONWriter& writer, const std::string& sWidgetUUID, const std::string& sRequestType, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth)
{
	if (pAuth.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
	if (pBodyData == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	CAPIJSONRequest apiRequest(pBodyData, nBodyDataSize);
	auto pUIHandler = m_pSystemState->uiHandler();

	auto pWidget = pUIHandler->findModuleItem(AMCCommon::CUtils::normalizeUUIDString(sWidgetUUID));
	if (pWidget.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_MODULEITEMNOTFOUND, "Widget not found: " + sWidgetUUID);

	pWidget->handleCustomRequest(pAuth, sRequestType, apiRequest, writer, pUIHandler);
}

PAPIResponse CAPIHandler_Widget::handleRequest(const std::string& sURI, const eAPIRequestType requestType, CAPIFormFields& pFormFields, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth)
{
	pFormFields;

	if (requestType != eAPIRequestType::rtPost)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	auto sParameterString = AMCCommon::CUtils::toLowerString(sURI.substr(getBaseURI().length()));
	if (sParameterString.length() < 38)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDWIDGETREQUEST);
	if (sParameterString[0] != '/')
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDWIDGETREQUEST);

	std::string sWidgetUUID = AMCCommon::CUtils::normalizeUUIDString(sParameterString.substr(1, 36));
	if (sParameterString.length() < 39)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDWIDGETREQUEST);
	if (sParameterString[37] != '/')
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDWIDGETREQUEST);

	std::string sRequestType = sParameterString.substr(38);
	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sRequestType))
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDWIDGETREQUEST);

	CJSONWriter writer;
	writeJSONHeader(writer, AMC_API_PROTOCOL_UI);
	handleWidgetRequest(writer, sWidgetUUID, sRequestType, pBodyData, nBodyDataSize, pAuth);

	return std::make_shared<CAPIStringResponse>(AMC_API_HTTP_SUCCESS, AMC_API_CONTENTTYPE, writer.saveToString());
}
