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

#include "amc_api_handler_signal.hpp"
#include "amc_api_jsonrequest.hpp"

#include "libmc_interfaceexception.hpp"
#include "libmcdata_dynamic.hpp"

#include "amc_service_buildfileparsing.hpp"
#include "amc_toolpathhandler.hpp"
#include "amc_statesignalhandler.hpp"

#include "common_utils.hpp"

#include <vector>
#include <memory>
#include <string>
#include <iostream>


using namespace AMC;

CAPIHandler_Signal::CAPIHandler_Signal(PSystemState pSystemState)
	: m_pSystemState(pSystemState)
{
	if (pSystemState.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

}

CAPIHandler_Signal::~CAPIHandler_Signal()
{
	
}
				
std::string CAPIHandler_Signal::getBaseURI () 
{
	return "api/signal";
}

APIHandler_SignalType CAPIHandler_Signal::parseRequest(const std::string& sURI, const eAPIRequestType requestType, std::string& jobUUID)
{
	// Leave away base URI
	auto sParameterString = AMCCommon::CUtils::toLowerString (sURI.substr (getBaseURI().length ()));
	jobUUID = "";

	if (requestType == eAPIRequestType::rtPost) {

		if ((sParameterString == "/") || (sParameterString == "")) 
			return APIHandler_SignalType::stSendSignal;
	
	}

	return APIHandler_SignalType::stUnknown;
}


bool CAPIHandler_Signal::expectsRawBody(const std::string& sURI, const eAPIRequestType requestType)
{
	std::string jobUUID;

	switch (parseRequest(sURI, requestType, jobUUID)) {
		case APIHandler_SignalType::stSendSignal:
			return true;

		default:
			return false;
	}
}


uint32_t CAPIHandler_Signal::getFormDataFieldCount(const std::string& sURI, const eAPIRequestType requestType)
{
	return 0;	
}

CAPIFieldDetails CAPIHandler_Signal::getFormDataFieldDetails(const std::string& sURI, const eAPIRequestType requestType, const uint32_t nFieldIndex)
{
	throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDINDEX);
}

void CAPIHandler_Signal::handleSendSignalRequest(CJSONWriter& writer, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth)
{
	if (pBodyData == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
	if (pAuth.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	CAPIJSONRequest jsonRequest(pBodyData, nBodyDataSize);
	auto sInstanceName = jsonRequest.getNameString(AMC_API_KEY_SIGNAL_INSTANCENAME, LIBMC_ERROR_MISSINGINSTANCENAME);
	auto sSignalName = jsonRequest.getNameString(AMC_API_KEY_SIGNAL_SIGNALNAME, LIBMC_ERROR_MISSINGSIGNALNAME);

	auto pSignalHandler = m_pSystemState->getStateSignalHandlerInstance();

	if (!pSignalHandler->hasSignalDefinition(sInstanceName, sSignalName))
		throw ELibMCInterfaceException (LIBMC_ERROR_SIGNALNOTFOUND);

	auto pParameterGroup = std::make_shared<AMC::CParameterGroup>();

	pSignalHandler->populateParameterGroup(sInstanceName, sSignalName, pParameterGroup.get());

	auto nCount = pParameterGroup->getParameterCount();
	for (uint32_t nIndex = 0; nIndex < nCount; nIndex++) {
		std::string sParamName, sDescription, sDefaultValue;
		pParameterGroup->getParameterInfo(nIndex, sParamName, sDescription, sDefaultValue);
		auto sValue = jsonRequest.getRawString(sParamName, LIBMC_ERROR_MISSINGSIGNALPARAMETER);
		pParameterGroup->setParameterValueByIndex(nIndex, sValue);
	}

	std::string sNewSignalUUID;

	if (pSignalHandler->triggerSignal(sInstanceName, sSignalName, pParameterGroup->serializeToJSON (), sNewSignalUUID)) {
		writer.addString(AMC_API_KEY_SIGNAL_SIGNALUUID, sNewSignalUUID);
	} else
		throw ELibMCInterfaceException(LIBMC_ERROR_COULDNOTSENDSIGNAL);
	
}




PAPIResponse CAPIHandler_Signal::handleRequest(const std::string& sURI, const eAPIRequestType requestType, CAPIFormFields & pFormFields, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth)
{
	std::string jobUUID;

	auto buildType = parseRequest(sURI, requestType, jobUUID);

	CJSONWriter writer;
	writeJSONHeader(writer, AMC_API_PROTOCOL_SIGNAL);

	switch (buildType) {
	case APIHandler_SignalType::stSendSignal:
		handleSendSignalRequest(writer, pBodyData, nBodyDataSize, pAuth);
		break;

	default:
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	}

	return std::make_shared<CAPIStringResponse>(AMC_API_HTTP_SUCCESS, AMC_API_CONTENTTYPE, writer.saveToString());

	return nullptr;
}




		

