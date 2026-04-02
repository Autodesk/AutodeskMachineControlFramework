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

#define __AMCIMPL_UI_MODULE

#include "amc_api_handler_module.hpp"
#include "amc_ui_handler.hpp"
#include "amc_ui_module.hpp"

#include "libmc_interfaceexception.hpp"
#include "common_utils.hpp"

using namespace AMC;

CAPIHandler_Module::CAPIHandler_Module(PSystemState pSystemState)
	: CAPIHandler(pSystemState->getClientHash()), m_pSystemState(pSystemState)
{
	if (pSystemState.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
}

CAPIHandler_Module::~CAPIHandler_Module()
{
}

std::string CAPIHandler_Module::getBaseURI()
{
	return "api/module";
}

PAPIResponse CAPIHandler_Module::handleRequest(const std::string& sURI, const eAPIRequestType requestType, CAPIFormFields& pFormFields, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth)
{
	pFormFields;
	pBodyData;
	nBodyDataSize;

	if (pAuth.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
	if (requestType != eAPIRequestType::rtGet)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	auto sParameterString = AMCCommon::CUtils::toLowerString(sURI.substr(getBaseURI().length()));
	if ((sParameterString.length() < 37) || (sParameterString[0] != '/'))
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	std::string sModuleUUID = AMCCommon::CUtils::normalizeUUIDString(sParameterString.substr(1, 36));
	uint32_t nStateID = 0;

	if (sParameterString.length() > 37) {
		if ((sParameterString.length() <= 38) || (sParameterString[37] != '/'))
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDCONTENTSTATEID);

		int64_t nParsedStateID = std::stoll(sParameterString.substr(38));
		if ((nParsedStateID < 0) || (nParsedStateID > INT32_MAX))
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDCONTENTSTATEID, "Invalid content state id: " + sParameterString.substr(38));

		nStateID = (uint32_t)nParsedStateID;
	}

	auto pModule = m_pSystemState->uiHandler()->findModule(sModuleUUID);
	if (pModule.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_MODULENOTFOUND, "Module UUID = " + sModuleUUID);

	CJSONWriter writer;
	writeJSONHeader(writer, AMC_API_PROTOCOL_UI);

	CJSONWriterObject object(writer);
	pModule->addContentToJSON(writer, object, pAuth->getLegacyParameterHandler(true), nStateID);
	writer.addString(AMC_API_KEY_UI_ITEMUUID, sModuleUUID);
	writer.addObject(AMC_API_KEY_UI_CONTENT, object);

	return std::make_shared<CAPIStringResponse>(AMC_API_HTTP_SUCCESS, AMC_API_CONTENTTYPE, writer.saveToString());
}

