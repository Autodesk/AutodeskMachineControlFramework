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


#include "amc_api_handler_status.hpp"
#include "libmc_exceptiontypes.hpp"
#include "common_utils.hpp"

#include <vector>
#include <memory>
#include <string>


using namespace AMC;

CAPIHandler_Status::CAPIHandler_Status(std::vector <AMC::PStateMachineInstance>& Instances, PStateMachineData pStateMachineData, const std::string& sClientHash)
	: CAPIHandler(sClientHash), m_Instances(Instances), m_pStateMachineData (pStateMachineData)
{
	LibMCAssertNotNull(pStateMachineData.get());
	
}

CAPIHandler_Status::~CAPIHandler_Status()
{
	
}
				
std::string CAPIHandler_Status::getBaseURI () 
{
	return "api/statemachines";
}

PStateMachineInstance CAPIHandler_Status::findInstanceByName(const std::string& sInstanceName, bool bFailIfNotExisting)
{
	for (auto pInstance : m_Instances) {
		if (pInstance->getName() == sInstanceName)
			return pInstance;
	}

	if (bFailIfNotExisting)
		throw ELibMCCustomException(LIBMC_ERROR_STATEMACHINENOTFOUND, sInstanceName);

	return nullptr;
}

void CAPIHandler_Status::writeStateMachineSummaryToJSON(CJSONWriter& writer, CJSONWriterObject& instanceJSONObject, PStateMachineInstance pInstance)
{
	LibMCAssertNotNull(pInstance.get());

	auto sInstanceName = pInstance->getName();
	instanceJSONObject.addString(AMC_API_KEY_STATUSINSTANCE_NAME, sInstanceName);
	instanceJSONObject.addString(AMC_API_KEY_STATUSINSTANCE_STATE, m_pStateMachineData->getInstanceStateName(sInstanceName));
}

void CAPIHandler_Status::writeStateMachineDetailsToJSON(CJSONWriter& writer, CJSONWriterObject& instanceJSONObject, PStateMachineInstance pInstance)
{
	LibMCAssertNotNull(pInstance.get());

	writeStateMachineSummaryToJSON(writer, instanceJSONObject, pInstance);

	CJSONWriterArray parameterGroupsJSONArray(writer);
	auto pParameterHandler = pInstance->getParameterHandler();
	uint32_t nParameterGroupCount = pParameterHandler->getGroupCount();

	for (uint32_t nGroupIndex = 0; nGroupIndex < nParameterGroupCount; nGroupIndex++) {
		auto pGroup = pParameterHandler->getGroup(nGroupIndex);

		CJSONWriterObject groupJSONObject(writer);
		groupJSONObject.addString(AMC_API_KEY_STATUSPARAMETERGROUP_NAME, pGroup->getName());

		CJSONWriterArray parametersJSONArray(writer);
		uint32_t nParameterCount = pGroup->getParameterCount();
		for (uint32_t nParamIndex = 0; nParamIndex < nParameterCount; nParamIndex++) {
			CJSONWriterObject parameterJSONObject(writer);
			std::string sParamName, sParamDescription, sParamDefaultValue;
			pGroup->getParameterInfo(nParamIndex, sParamName, sParamDescription, sParamDefaultValue);
			parameterJSONObject.addString(AMC_API_KEY_STATUSPARAMETER_NAME, sParamName);
			parameterJSONObject.addString(AMC_API_KEY_STATUSPARAMETER_VALUE, pGroup->getParameterValueByIndex(nParamIndex));
			parametersJSONArray.addObject(parameterJSONObject);
		}

		groupJSONObject.addArray(AMC_API_KEY_STATUSPARAMETERGROUP_PARAMETERS, parametersJSONArray);
		parameterGroupsJSONArray.addObject(groupJSONObject);
	}

	instanceJSONObject.addArray(AMC_API_KEY_STATUSPARAMETERGROUPS, parameterGroupsJSONArray);
}
			
PAPIResponse CAPIHandler_Status::handleRequest(const std::string& sURI, const eAPIRequestType requestType, CAPIFormFields & pFormFields, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth)
{
	pFormFields;
	pBodyData;
	nBodyDataSize;
	pAuth;

	if (requestType != eAPIRequestType::rtGet)
		return nullptr;

	auto sParameterString = sURI.substr(getBaseURI().length());

	// GET /api/statemachines
	if (sParameterString.empty() || (sParameterString == "/")) {
		CJSONWriter writer;
		writeJSONHeader(writer, AMC_API_PROTOCOL_STATUS);

		CJSONWriterArray instanceJSONArray(writer);
		for (auto pInstance : m_Instances) {
			CJSONWriterObject instanceJSONObject(writer);
			writeStateMachineSummaryToJSON(writer, instanceJSONObject, pInstance);
			instanceJSONArray.addObject(instanceJSONObject);
		}

		writer.addArray(AMC_API_KEY_STATUSINSTANCES, instanceJSONArray);
		return std::make_shared<CAPIStringResponse>(AMC_API_HTTP_SUCCESS, AMC_API_CONTENTTYPE, writer.saveToString());
	}

	// GET /api/statemachines/{instancename}
	if ((sParameterString.length() >= 2) && (sParameterString[0] == '/')) {
		auto sInstanceName = sParameterString.substr(1);
		if (!sInstanceName.empty() && (sInstanceName.back() == '/'))
			sInstanceName = sInstanceName.substr(0, sInstanceName.length() - 1);

		if ((sInstanceName.empty()) || (sInstanceName.find("/") != std::string::npos))
			return nullptr;

		if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sInstanceName))
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDSTATEMACHINENAME, sInstanceName);

		auto pInstance = findInstanceByName(sInstanceName, true);

		CJSONWriter writer;
		writeJSONHeader(writer, AMC_API_PROTOCOL_STATUS);

		CJSONWriterObject instanceJSONObject(writer);
		writeStateMachineDetailsToJSON(writer, instanceJSONObject, pInstance);
		writer.addObject("instance", instanceJSONObject);

		return std::make_shared<CAPIStringResponse>(AMC_API_HTTP_SUCCESS, AMC_API_CONTENTTYPE, writer.saveToString());
	}

	return nullptr;
}




