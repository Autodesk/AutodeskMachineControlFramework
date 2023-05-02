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

#define __AMCIMPL_UI_MODULE
#define __AMCIMPL_API_CONSTANTS

#include "amc_ui_module_customitem.hpp"
#include "libmc_interfaceexception.hpp"
#include "amc_ui_expression.hpp"
#include "amc_ui_module.hpp"

#include "amc_api_constants.hpp"
#include "Common/common_utils.hpp"
#include "amc_parameterhandler.hpp"
#include "libmc_exceptiontypes.hpp"


#include "libmcdata_dynamic.hpp"

using namespace AMC;


CUIModuleCustomItem_Properties::CUIModuleCustomItem_Properties(const std::string& sUUID, const std::string& sModulePath, PUIModuleEnvironment pUIModuleEnvironment)
	: CUIModuleItem (sModulePath), m_sUUID (AMCCommon::CUtils::normalizeUUIDString (sUUID)), m_pUIModuleEnvironment (pUIModuleEnvironment)
{
	LibMCAssertNotNull(pUIModuleEnvironment.get());

}

CUIModuleCustomItem_Properties::~CUIModuleCustomItem_Properties()
{

}

std::string CUIModuleCustomItem_Properties::getUUID()
{
	return m_sUUID;
}


void CUIModuleCustomItem_Properties::addContentToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler, uint32_t nStateID)
{
	auto pGroup = pClientVariableHandler->findGroup(getItemPath(), true);

	auto pStateMachineData = m_pUIModuleEnvironment->stateMachineData();

	for (auto iIter : m_Properties) {
		std::string sName = iIter.first;
		if (iIter.second.needsSync())
			pGroup->setParameterValueByName(sName, iIter.second.evaluateStringValue(pStateMachineData));

		object.addString(sName, pGroup->getParameterValueByName (sName));
	}

}

std::list <std::string> CUIModuleCustomItem_Properties::getReferenceUUIDs()
{
	std::list <std::string> resultList;
	resultList.push_back(m_sUUID);

	return resultList;
}


void CUIModuleCustomItem_Properties::populateClientVariables(CParameterHandler* pParameterHandler)
{
	LibMCAssertNotNull(pParameterHandler);
	auto pStateMachineData = m_pUIModuleEnvironment->stateMachineData();

	auto pGroup = pParameterHandler->findGroup(getItemPath(), true);
	for (auto iIter : m_Properties) {
		pGroup->addNewStringParameter(iIter.first, iIter.first, iIter.second.evaluateStringValue(pStateMachineData));
	}
	
}

void CUIModuleCustomItem_Properties::registerProperty(const std::string& sName, const std::string& sTypeString, CUIExpression valueExpression)
{
	m_Properties.insert (std::make_pair (sName, valueExpression));
}


std::string CUIModuleCustomItem_Properties::findElementPathByUUID(const std::string& sUUID)
{
	if (sUUID == getUUID())
		return m_sItemPath;

	return "";
}

CUIModuleCustomItem_EventParameter::CUIModuleCustomItem_EventParameter(const std::string& sParameterName, CUIExpression expression)
	: m_sParameterName (sParameterName), m_Expression (expression), m_sUUID (AMCCommon::CUtils::createUUID ())

{

}

CUIModuleCustomItem_EventParameter::~CUIModuleCustomItem_EventParameter()
{

}

std::string CUIModuleCustomItem_EventParameter::getParameterName()
{
	return m_sParameterName;
}

std::string CUIModuleCustomItem_EventParameter::getUUID()
{
	return m_sUUID;
}


CUIExpression& CUIModuleCustomItem_EventParameter::getExpression()
{
	return m_Expression;
}


CUIModuleCustomItem_Event::CUIModuleCustomItem_Event(const std::string& sEventName, const std::string& sModulePath, PUIModuleEnvironment pUIModuleEnvironment)
	: CUIModuleItem(sModulePath + "." + sEventName), m_sUUID(AMCCommon::CUtils::createUUID()), m_pUIModuleEnvironment(pUIModuleEnvironment),
	m_sEventName (sEventName)
{
	LibMCAssertNotNull(pUIModuleEnvironment.get());
	if (sEventName.empty())
		throw ELibMCCustomException(LIBMC_ERROR_EMTPYEVENTNAME, sModulePath);

	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sEventName))
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDEVENTNAME, sModulePath + "." + sEventName);

	if (!AMCCommon::CUtils::stringIsValidAlphanumericPathString(sEventName))
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDEVENTPATH, sModulePath + "." + sEventName);

}

CUIModuleCustomItem_Event::~CUIModuleCustomItem_Event()
{

}

std::string CUIModuleCustomItem_Event::getUUID()
{
	return m_sUUID;
}




void CUIModuleCustomItem_Event::addContentToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler, uint32_t nStateID)
{

	auto pGroup = pClientVariableHandler->findGroup(getItemPath(), true);

	auto pStateMachineData = m_pUIModuleEnvironment->stateMachineData();

	CJSONWriterArray parameterArray(writer);

	for (auto iIter : m_EventParameterNameMap) {
		std::string sName = iIter.first;
		auto& expression = iIter.second->getExpression ();
		if (expression.needsSync())
			pGroup->setParameterValueByName(sName, expression.evaluateStringValue(pStateMachineData));


		CJSONWriterObject parameterObject(writer);
		parameterObject.addString("name", sName);
		parameterObject.addString("uuid", iIter.second->getUUID ());
		parameterObject.addString("defaultvalue", pGroup->getParameterValueByName(sName));
		parameterArray.addObject(parameterObject);
			
	}

	object.addArray("parameters", parameterArray);

}

std::list <std::string> CUIModuleCustomItem_Event::getReferenceUUIDs()
{
	std::list <std::string> resultList;
	resultList.push_back(m_sUUID);
	for (auto iIter : m_EventParameterNameMap) {
		resultList.push_back(iIter.second->getUUID ());
	}

	return resultList;
}

std::string CUIModuleCustomItem_Event::getEventName()
{
	return m_sEventName;
}


void CUIModuleCustomItem_Event::populateClientVariables(CParameterHandler* pParameterHandler)
{
	LibMCAssertNotNull(pParameterHandler);
	auto pStateMachineData = m_pUIModuleEnvironment->stateMachineData();

	auto pGroup = pParameterHandler->addGroup(getItemPath(), "custom event parameters");
	for (auto iIter : m_EventParameterNameMap) {
		auto& expression = iIter.second->getExpression();
		pGroup->addNewStringParameter(iIter.first, iIter.first, expression.evaluateStringValue(pStateMachineData));
	}

}


void CUIModuleCustomItem_Event::registerParameter(const std::string sParameterName, CUIExpression expression)
{
	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString (sParameterName))
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDEVENTPARAMETERNAME, m_sItemPath + "/" + sParameterName);

	auto pParameter = std::make_shared<CUIModuleCustomItem_EventParameter> (sParameterName, expression);
	m_EventParameterNameMap.insert(std::make_pair (sParameterName, pParameter));
	m_EventParameterUUIDMap.insert(std::make_pair(pParameter->getUUID (), pParameter));

}



std::string CUIModuleCustomItem_Event::findElementPathByUUID(const std::string& sUUID)
{
	if (sUUID == getUUID())
		return m_sItemPath;

	auto parameterIter = m_EventParameterUUIDMap.find(sUUID);
	if (parameterIter != m_EventParameterUUIDMap.end())
		return m_sItemPath + "." + parameterIter->second->getParameterName();
	
	return "";
}

void CUIModuleCustomItem_Event::setEventPayloadValue(const std::string& sEventName, const std::string& sPayloadUUID, const std::string& sPayloadValue, CParameterHandler* pClientVariableHandler)
{
	LibMCAssertNotNull(pClientVariableHandler);
	auto parameterIter = m_EventParameterUUIDMap.find(AMCCommon::CUtils::normalizeUUIDString(sPayloadUUID));
	if (parameterIter != m_EventParameterUUIDMap.end()) {
		auto pGroup = pClientVariableHandler->findGroup(getItemPath(), true);

		pGroup->setParameterValueByName(parameterIter->second->getParameterName (), sPayloadValue);

	}
	
}