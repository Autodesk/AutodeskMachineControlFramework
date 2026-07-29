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
#define __AMCIMPL_API_CONSTANTS

#include "amc_ui_module_contentitem_togglepanel.hpp"
#include "amc_ui_expression.hpp"
#include "libmc_interfaceexception.hpp"

#include "amc_api_constants.hpp"
#include "amc_ui_frontendstate.hpp"
#include "Common/common_utils.hpp"
#include "amc_parameterhandler.hpp"
#include "amc_statemachinedata.hpp"
#include "amc_ui_module.hpp"

#include "libmcdata_dynamic.hpp"
#include "libmc_exceptiontypes.hpp"

using namespace AMC;


CUIModule_ContentToggle::CUIModule_ContentToggle(const std::string& sLabel, const CUIExpression& valueExpression, const std::string& sEvent, const std::string& sMode)
	: m_sUUID(AMCCommon::CUtils::createUUID()), m_sLabel(sLabel), m_ValueExpression(valueExpression), m_sEvent(sEvent), m_sMode(sMode)
{
}

CUIModule_ContentToggle::~CUIModule_ContentToggle()
{
}

std::string CUIModule_ContentToggle::getUUID()
{
	return m_sUUID;
}

std::string CUIModule_ContentToggle::getLabel()
{
	return m_sLabel;
}

std::string CUIModule_ContentToggle::getEvent()
{
	return m_sEvent;
}

std::string CUIModule_ContentToggle::getMode()
{
	return m_sMode;
}

std::string CUIModule_ContentToggle::evaluateValue(CStateMachineData* pStateMachineData)
{
	return m_ValueExpression.evaluateStringValue(pStateMachineData);
}


PUIModule_ContentTogglePanel CUIModule_ContentTogglePanel::makeFromXML(const pugi::xml_node& xmlNode, const std::string& sItemName, const std::string& sModulePath, PUIModuleEnvironment pUIModuleEnvironment)
{
	LibMCAssertNotNull(pUIModuleEnvironment);

	uint32_t nColumns = 3;
	auto columnsAttrib = xmlNode.attribute("columns");
	if (!columnsAttrib.empty()) {
		int nParsed = columnsAttrib.as_int();
		if (nParsed >= 1)
			nColumns = (uint32_t)nParsed;
	}

	auto pTogglePanel = std::make_shared<CUIModule_ContentTogglePanel>(nColumns, pUIModuleEnvironment->stateMachineData(), sItemName, sModulePath);

	pTogglePanel->loadFromXML(xmlNode);

	return pTogglePanel;
}


CUIModule_ContentTogglePanel::CUIModule_ContentTogglePanel(uint32_t nColumns, PStateMachineData pStateMachineData, const std::string& sItemName, const std::string& sModulePath)
	: CUIModule_ContentItem(AMCCommon::CUtils::createUUID(), sItemName, sModulePath), m_nColumns(nColumns), m_pStateMachineData(pStateMachineData)
{
	if (pStateMachineData.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
}

CUIModule_ContentTogglePanel::~CUIModule_ContentTogglePanel()
{
}

void CUIModule_ContentTogglePanel::loadFromXML(const pugi::xml_node& xmlNode)
{
	auto toggleNodes = xmlNode.children("toggle");
	for (auto toggleNode : toggleNodes) {

		auto labelAttrib = toggleNode.attribute("label");
		std::string sLabel = labelAttrib.as_string();

		// "source" carries the reflected state (literal or sync:source="instance.group.parameter").
		CUIExpression valueExpression(toggleNode, "source", std::string(""));

		auto eventAttrib = toggleNode.attribute("event");
		std::string sEvent = eventAttrib.as_string();

		auto modeAttrib = toggleNode.attribute("mode");
		std::string sMode = modeAttrib.as_string();
		if (sMode.empty())
			sMode = "onoff";

		addToggle(std::make_shared<CUIModule_ContentToggle>(sLabel, valueExpression, sEvent, sMode));
	}
}

void CUIModule_ContentTogglePanel::addToggle(PUIModule_ContentToggle pToggle)
{
	if (m_Toggles.size() >= AMC_CONTENT_MAXENTRYCOUNT)
		throw ELibMCInterfaceException(LIBMC_ERROR_TOOMANYCONTENTPARAMETERS);

	m_Toggles.push_back(pToggle);
}

uint32_t CUIModule_ContentTogglePanel::getToggleCount()
{
	return (uint32_t)m_Toggles.size();
}

std::list<std::string> CUIModule_ContentTogglePanel::getReferenceUUIDs()
{
	std::list<std::string> resultList;
	resultList.push_back(m_sUUID);
	for (auto pToggle : m_Toggles)
		resultList.push_back(pToggle->getUUID());

	return resultList;
}

std::string CUIModule_ContentTogglePanel::findElementPathByUUID(const std::string& sUUID)
{
	if (sUUID == m_sUUID)
		return getItemPath();

	for (auto pToggle : m_Toggles) {
		if (pToggle->getUUID() == sUUID)
			return getItemPath();
	}

	return "";
}


void CUIModule_ContentTogglePanel::writeTogglesToJSON(CJSONWriter& writer, CJSONWriterArray& toggleArray)
{
	for (auto pToggle : m_Toggles) {
		CJSONWriterObject toggleObject(writer);
		toggleObject.addString("uuid", pToggle->getUUID());
		toggleObject.addString("label", pToggle->getLabel());
		toggleObject.addString("mode", pToggle->getMode());
		toggleObject.addString("event", pToggle->getEvent());
		toggleObject.addString("value", pToggle->evaluateValue(m_pStateMachineData.get()));
		toggleArray.addObject(toggleObject);
	}
}


void CUIModule_ContentTogglePanel::addLegacyContentToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler, uint32_t nStateID)
{
	object.addString(AMC_API_KEY_UI_ITEMTYPE, "togglepanel");
	object.addString(AMC_API_KEY_UI_ITEMUUID, m_sUUID);
	object.addInteger("columns", m_nColumns);

	CJSONWriterArray toggleArray(writer);
	writeTogglesToJSON(writer, toggleArray);
	object.addArray("toggles", toggleArray);
}

std::string CUIModule_ContentTogglePanel::getItemType()
{
	return "togglepanel";
}

void CUIModule_ContentTogglePanel::registerFrontendAttributes()
{
	CUIExpression columnsExpr;
	columnsExpr.setFixedValue(std::to_string(m_nColumns));
	registerItemIntegerAttribute("columns", columnsExpr);
}

void CUIModule_ContentTogglePanel::frontendWriteItemToJSON(CJSONWriter& writer, CJSONWriterObject& itemObject, CUIFrontendState* pFrontendState, CStateMachineData* pStateMachineData)
{
	if (pFrontendState == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
	if (m_pItemModuleStore == nullptr)
		return;

	std::string sItemType = m_pItemModuleStore->getModuleType();
	if (sItemType.empty())
		return;

	itemObject.addString("moduletype", sItemType);
	itemObject.addString("uuid", m_pItemModuleStore->getUUID());

	CJSONWriterObject attributesObject(writer);
	pFrontendState->writeModuleAttributesToJSON(writer, attributesObject, m_pItemModuleStore.get(), pStateMachineData);

	// Embed the live, resolved toggle states directly into the v2 attributes.
	CJSONWriterArray toggleArray(writer);
	writeTogglesToJSON(writer, toggleArray);
	attributesObject.addArray("toggles", toggleArray);

	itemObject.addObject("attributes", attributesObject);
}
