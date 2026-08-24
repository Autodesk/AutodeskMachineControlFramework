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

#include "amc_ui_module.hpp"
#include "amc_ui_modulefactory.hpp"

#include "amc_ui_module_custom.hpp"
#include "amc_ui_module_customitem.hpp"

#include "amc_api_constants.hpp"
#include "amc_resourcepackage.hpp"
#include "amc_parameterhandler.hpp"

#include "amc_ui_expression.hpp"

#include "libmc_exceptiontypes.hpp"
#include "common_utils.hpp"

using namespace AMC;


CUIModule_Custom::CUIModule_Custom(pugi::xml_node& xmlNode, const std::string& sPath, PUIModuleEnvironment pUIModuleEnvironment)
: CUIModule (getNameFromXML(xmlNode), sPath, pUIModuleEnvironment->getFrontendDefinition ())
{

	LibMCAssertNotNull(pUIModuleEnvironment.get());

	if (sPath.empty())
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDMODULEPATH, m_sName);

	m_sParentPath = sPath;

	auto pFrontendDefinition = pUIModuleEnvironment->getFrontendDefinition();

	m_pCustomItem = std::make_shared<CUIModuleCustomItem_Properties>(m_sUUID, m_sParentPath, pUIModuleEnvironment);

	pugi::xml_node propertiesNode = xmlNode.child("properties");
	if (!propertiesNode.empty()) {
		auto propertyNodes = propertiesNode.children("property");
		for (auto propertyNode : propertyNodes) {
			auto nameAttribute = propertyNode.attribute("name");
			if (nameAttribute.empty ())
				throw ELibMCCustomException(LIBMC_ERROR_CUSTOMPAGEPROPERTYNAMEMISSING, m_sName);
			std::string sName = nameAttribute.as_string();

			auto typeAttribute = propertyNode.attribute("type");
			if (typeAttribute.empty())
				throw ELibMCCustomException(LIBMC_ERROR_CUSTOMPAGEPROPERTYTYPEMISSING, m_sName);
			std::string sType = typeAttribute.as_string();

			CUIExpression valueExpression (propertyNode, "value");
			m_pCustomItem->registerProperty (sName, sType, valueExpression);
		}
	}

	// Register custom properties as v2 frontend attributes
	for (auto& propPair : m_pCustomItem->getProperties()) {
		registerStringAttribute(propPair.first, propPair.second);
	}

	pugi::xml_node eventsNode = xmlNode.child("events");
	if (!eventsNode.empty()) {
		auto eventNodes = eventsNode.children("event");

		for (auto eventNode : eventNodes) {
			auto nameAttribute = eventNode.attribute("name");
			if (nameAttribute.empty())
				throw ELibMCCustomException(LIBMC_ERROR_CUSTOMPAGEVENTNAMEMISSING, m_sName);

			std::string sName = nameAttribute.as_string();
			if (sName.empty ())
				throw ELibMCCustomException(LIBMC_ERROR_CUSTOMPAGEVENTNAMEMISSING, m_sName);

			if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString (sName))
				throw ELibMCCustomException(LIBMC_ERROR_INVALIDCUSTOMPAGEVENTNAME, m_sName + "/" + sName);

			auto iIter = m_EventItemNameMap.find(sName);
			if (iIter != m_EventItemNameMap.end ())
				throw ELibMCCustomException(LIBMC_ERROR_DUPLICATECUSTOMPAGEVENTNAME, m_sName + "/" + sName);

			auto pEventItem = std::make_shared<CUIModuleCustomItem_Event>(sName, m_sParentPath, pUIModuleEnvironment);

			m_EventItemNameMap.insert(std::make_pair (pEventItem->getEventName (), pEventItem));
			m_EventItemUUIDMap.insert(std::make_pair(pEventItem->getUUID(), pEventItem));

			auto eventParameterNodes = eventNode.children("eventparameter");
			for (auto eventParameterNode : eventParameterNodes) {
				std::string sParameterName = eventParameterNode.attribute("name").as_string();
				CUIExpression defaultValueExpression(eventParameterNode, "default");

				pEventItem->registerParameter(sParameterName, defaultValueExpression);
			}

		}

	}

}


CUIModule_Custom::~CUIModule_Custom()
{
}



std::string CUIModule_Custom::getStaticType()
{
	return "custom";
}

std::string CUIModule_Custom::getType()
{
	return getStaticType();
}

std::string CUIModule_Custom::getCaption()
{
	return "custom";
}


void CUIModule_Custom::writeLegacyDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& moduleObject, CParameterHandler* pLegacyClientVariableHandler)
{
	moduleObject.addString(AMC_API_KEY_UI_MODULENAME, getName());
	moduleObject.addString(AMC_API_KEY_UI_MODULEUUID, getUUID());
	moduleObject.addString(AMC_API_KEY_UI_MODULETYPE, getType());

	CJSONWriterArray itemsNode(writer);
	{
		CJSONWriterObject itemObject(writer);
		itemObject.addString(AMC_API_KEY_UI_ITEMTYPE, "properties");
		itemObject.addString(AMC_API_KEY_UI_ITEMUUID, m_pCustomItem->getUUID());
		m_pCustomItem->addLegacyContentToJSON(writer, itemObject, pLegacyClientVariableHandler, 0);
		itemsNode.addObject(itemObject);
	}

	for (auto eventItemIter : m_EventItemNameMap) {

		auto eventItem = eventItemIter.second;
		CJSONWriterObject itemObject(writer);
		itemObject.addString(AMC_API_KEY_UI_ITEMTYPE, "event");
		itemObject.addString(AMC_API_KEY_UI_ITEMUUID, eventItem->getUUID());
		itemObject.addString(AMC_API_KEY_UI_ITEMNAME, eventItem->getEventName ());
		eventItem->addLegacyContentToJSON(writer, itemObject, pLegacyClientVariableHandler, 0);
		itemsNode.addObject(itemObject);

	}

	moduleObject.addArray(AMC_API_KEY_UI_ITEMS, itemsNode);

}

void CUIModule_Custom::addContentToJSON(CJSONWriter& writer, CJSONWriterObject& moduleObject, CParameterHandler* pClientVariableHandler, uint32_t nStateID)
{
}

PUIModuleItem CUIModule_Custom::findLegacyItem(const std::string& sUUID)
{
	if (m_pCustomItem->getUUID() == sUUID)
		return m_pCustomItem;

	auto iEventIter = m_EventItemUUIDMap.find(sUUID);
	if (iEventIter != m_EventItemUUIDMap.end())
		return iEventIter->second;

	return nullptr;

}

void CUIModule_Custom::populateModuleMap(std::map<std::string, PUIModule>& moduleMap)
{
	moduleMap.insert(std::make_pair(m_sUUID, std::make_shared<CUIModule_Custom>(*this)));
}

void CUIModule_Custom::populateLegacyItemMap(std::map<std::string, PUIModuleItem>& itemMap)
{
	itemMap.insert(std::make_pair(m_pCustomItem->getUUID(), m_pCustomItem));
	for (auto iEventIter : m_EventItemNameMap) {
		itemMap.insert(std::make_pair(iEventIter.second->getUUID(), iEventIter.second));
		auto referencedUUIDs = iEventIter.second->getReferenceUUIDs();
		for (auto sUUID : referencedUUIDs)
			itemMap.insert(std::make_pair(sUUID, iEventIter.second));

	}
}


void CUIModule_Custom::populateLegacyClientVariables(CParameterHandler* pParameterHandler)
{
	LibMCAssertNotNull(pParameterHandler);

	m_pCustomItem->populateClientVariables(pParameterHandler);
	for (auto iEventIter : m_EventItemNameMap) {
		iEventIter.second->populateClientVariables(pParameterHandler);
	}

}

/////////////////////////////////////////////////////////////////////////////////////
// New UI Frontend System
/////////////////////////////////////////////////////////////////////////////////////

bool CUIModule_Custom::isVersion2FrontendModule()
{
	return true;
}

void CUIModule_Custom::frontendWriteModuleStatusToJSON(CJSONWriter& writer, CJSONWriterObject& moduleObject, CUIFrontendState* pFrontendState, CStateMachineData* pStateMachineData)
{
	// Base class writes moduletype, uuid, name, and all registered attributes (including custom properties)
	CUIModule::frontendWriteModuleStatusToJSON(writer, moduleObject, pFrontendState, pStateMachineData);

	// Write events as submodules
	CJSONWriterArray submodulesArray(writer);

	for (auto& eventItemPair : m_EventItemNameMap) {
		auto pEventItem = eventItemPair.second;

		CJSONWriterObject eventObject(writer);
		eventObject.addString("moduletype", "event");
		eventObject.addString("uuid", pEventItem->getUUID());

		CJSONWriterObject attributesObject(writer);
		attributesObject.addString("eventname", pEventItem->getEventName());
		eventObject.addObject("attributes", attributesObject);

		submodulesArray.addObject(eventObject);
	}

	moduleObject.addArray("submodules", submodulesArray);
}

