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

#include "amc_ui_module_contentitem_statuslist.hpp"
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

#include <sstream>
#include <iomanip>

using namespace AMC;


CUIModule_ContentStatusListRow::CUIModule_ContentStatusListRow(const std::string& sLabel, const CUIExpression& valueExpression, const std::string& sDisplay, uint32_t nDecimals, const std::string& sUnit)
	: m_sLabel(sLabel), m_ValueExpression(valueExpression), m_sDisplay(sDisplay), m_nDecimals(nDecimals), m_sUnit(sUnit)
{
}

CUIModule_ContentStatusListRow::~CUIModule_ContentStatusListRow()
{
}

void CUIModule_ContentStatusListRow::addMap(const std::string& sValue, const std::string& sText, const std::string& sSeverity)
{
	sStatusListMapEntry entry;
	entry.m_sValue = sValue;
	entry.m_sText = sText;
	entry.m_sSeverity = sSeverity;
	m_Maps.push_back(entry);
}

std::string CUIModule_ContentStatusListRow::getLabel()
{
	return m_sLabel;
}

std::string CUIModule_ContentStatusListRow::getDisplay()
{
	return m_sDisplay;
}

uint32_t CUIModule_ContentStatusListRow::getDecimals()
{
	return m_nDecimals;
}

std::string CUIModule_ContentStatusListRow::getUnit()
{
	return m_sUnit;
}

const std::vector<sStatusListMapEntry>& CUIModule_ContentStatusListRow::getMaps()
{
	return m_Maps;
}

std::string CUIModule_ContentStatusListRow::evaluateValue(CStateMachineData* pStateMachineData)
{
	// For numeric displays, resolve as a number and format with the configured precision so
	// the client always receives a ready-to-render string; otherwise resolve as a string.
	if (m_sDisplay == "number") {
		double dValue = m_ValueExpression.evaluateNumberValue(pStateMachineData);
		std::stringstream sStream;
		sStream << std::fixed << std::setprecision((int)m_nDecimals) << dValue;
		return sStream.str();
	}

	return m_ValueExpression.evaluateStringValue(pStateMachineData);
}


PUIModule_ContentStatusList CUIModule_ContentStatusList::makeFromXML(const pugi::xml_node& xmlNode, const std::string& sItemName, const std::string& sModulePath, PUIModuleEnvironment pUIModuleEnvironment)
{
	LibMCAssertNotNull(pUIModuleEnvironment);

	auto titleAttrib = xmlNode.attribute("title");
	auto loadingtextAttrib = xmlNode.attribute("loadingtext");

	auto pStatusList = std::make_shared<CUIModule_ContentStatusList>(titleAttrib.as_string(), loadingtextAttrib.as_string(), pUIModuleEnvironment->stateMachineData(), sItemName, sModulePath);

	pStatusList->loadFromXML(xmlNode);

	return pStatusList;
}


CUIModule_ContentStatusList::CUIModule_ContentStatusList(const std::string& sTitle, const std::string& sLoadingText, PStateMachineData pStateMachineData, const std::string& sItemName, const std::string& sModulePath)
	: CUIModule_ContentItem(AMCCommon::CUtils::createUUID(), sItemName, sModulePath), m_sTitle(sTitle), m_sLoadingText(sLoadingText), m_pStateMachineData(pStateMachineData)
{
	if (pStateMachineData.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
}

CUIModule_ContentStatusList::~CUIModule_ContentStatusList()
{
}

void CUIModule_ContentStatusList::loadFromXML(const pugi::xml_node& xmlNode)
{
	auto rowNodes = xmlNode.children("row");
	for (auto rowNode : rowNodes) {

		auto labelAttrib = rowNode.attribute("label");
		std::string sLabel = labelAttrib.as_string();

		// "source" carries the bound value: a literal (source="..."), or a live binding (sync:source="instance.group.parameter").
		CUIExpression valueExpression(rowNode, "source", std::string(""));

		auto displayAttrib = rowNode.attribute("display");
		std::string sDisplay = displayAttrib.as_string();
		if (sDisplay.empty())
			sDisplay = "text";

		uint32_t nDecimals = 0;
		auto decimalsAttrib = rowNode.attribute("decimals");
		if (!decimalsAttrib.empty()) {
			int nParsedDecimals = decimalsAttrib.as_int();
			if (nParsedDecimals < 0)
				nParsedDecimals = 0;
			nDecimals = (uint32_t)nParsedDecimals;
		}

		auto unitAttrib = rowNode.attribute("unit");
		std::string sUnit = unitAttrib.as_string();

		auto pRow = std::make_shared<CUIModule_ContentStatusListRow>(sLabel, valueExpression, sDisplay, nDecimals, sUnit);

		// Optional explicit value->text+severity mappings for display="map".
		auto mapNodes = rowNode.children("map");
		for (auto mapNode : mapNodes) {
			auto mapValueAttrib = mapNode.attribute("value");
			auto mapTextAttrib = mapNode.attribute("text");
			auto mapSeverityAttrib = mapNode.attribute("severity");
			std::string sSeverity = mapSeverityAttrib.as_string();
			if (sSeverity.empty())
				sSeverity = "neutral";
			pRow->addMap(mapValueAttrib.as_string(), mapTextAttrib.as_string(), sSeverity);
		}

		addRow(pRow);
	}
}

void CUIModule_ContentStatusList::addRow(PUIModule_ContentStatusListRow pRow)
{
	if (m_Rows.size() >= AMC_CONTENT_MAXENTRYCOUNT)
		throw ELibMCInterfaceException(LIBMC_ERROR_TOOMANYCONTENTPARAMETERS);

	m_Rows.push_back(pRow);
}

uint32_t CUIModule_ContentStatusList::getRowCount()
{
	return (uint32_t)m_Rows.size();
}


void CUIModule_ContentStatusList::writeRowsToJSON(CJSONWriter& writer, CJSONWriterArray& rowArray)
{
	for (auto pRow : m_Rows) {
		CJSONWriterObject rowObject(writer);
		rowObject.addString("label", pRow->getLabel());
		rowObject.addString("display", pRow->getDisplay());
		rowObject.addString("value", pRow->evaluateValue(m_pStateMachineData.get()));

		if (pRow->getDisplay() == "number") {
			rowObject.addInteger("decimals", pRow->getDecimals());
			if (!pRow->getUnit().empty())
				rowObject.addString("unit", pRow->getUnit());
		}

		const auto& maps = pRow->getMaps();
		if (!maps.empty()) {
			CJSONWriterArray mapArray(writer);
			for (const auto& mapEntry : maps) {
				CJSONWriterObject mapObject(writer);
				mapObject.addString("value", mapEntry.m_sValue);
				mapObject.addString("text", mapEntry.m_sText);
				mapObject.addString("severity", mapEntry.m_sSeverity);
				mapArray.addObject(mapObject);
			}
			rowObject.addArray("maps", mapArray);
		}

		rowArray.addObject(rowObject);
	}
}


void CUIModule_ContentStatusList::addLegacyContentToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler, uint32_t nStateID)
{
	object.addString(AMC_API_KEY_UI_ITEMTYPE, "statuslist");
	object.addString(AMC_API_KEY_UI_ITEMUUID, m_sUUID);
	object.addString("title", m_sTitle);
	object.addString(AMC_API_KEY_UI_ITEMLOADINGTEXT, m_sLoadingText);

	CJSONWriterArray rowArray(writer);
	writeRowsToJSON(writer, rowArray);
	object.addArray("rows", rowArray);
}

std::string CUIModule_ContentStatusList::getItemType()
{
	return "statuslist";
}

void CUIModule_ContentStatusList::registerFrontendAttributes()
{
	CUIExpression titleExpr;
	titleExpr.setFixedValue(m_sTitle);
	registerItemStringAttribute("title", titleExpr);

	CUIExpression loadingTextExpr;
	loadingTextExpr.setFixedValue(m_sLoadingText);
	registerItemStringAttribute("loadingtext", loadingTextExpr);
}

void CUIModule_ContentStatusList::frontendWriteItemToJSON(CJSONWriter& writer, CJSONWriterObject& itemObject, CUIFrontendState* pFrontendState, CStateMachineData* pStateMachineData)
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

	// Embed the live, resolved rows directly into the v2 attributes so the frontend
	// can render pills/values without a separate polling call.
	CJSONWriterArray rowArray(writer);
	writeRowsToJSON(writer, rowArray);
	attributesObject.addArray("rows", rowArray);

	itemObject.addObject("attributes", attributesObject);
}
