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

#include "amc_ui_module_contentitem_parameterlist.hpp"
#include "amc_ui_expression.hpp"
#include "libmc_interfaceexception.hpp"

#include "amc_api_constants.hpp"
#include "amc_ui_frontendstate.hpp"
#include "Common/common_utils.hpp"
#include "amc_parameterhandler.hpp"
#include "amc_parametertype.hpp"
#include "amc_statemachinedata.hpp"
#include "amc_ui_module.hpp"

#include "libmcdata_dynamic.hpp"
#include "libmc_exceptiontypes.hpp"

#include <sstream>
#include <vector>

using namespace AMC;




// Maps an internal parameter data type to a stable frontend type string.
static std::string parameterDataTypeToString(AMC::eParameterDataType eType)
{
	switch (eType) {
		case AMC::eParameterDataType::String: return "string";
		case AMC::eParameterDataType::UUID: return "uuid";
		case AMC::eParameterDataType::Integer: return "integer";
		case AMC::eParameterDataType::Double: return "double";
		case AMC::eParameterDataType::Bool: return "bool";
		default: return "unknown";
	}
}


CUIModule_ContentParameterListEntry::CUIModule_ContentParameterListEntry(const std::string& sInstance, const std::string& sParameterGroup, const std::string& sParameter, bool bEditable, const std::string& sMin, const std::string& sMax, const std::string& sStep)
	: m_sInstance(sInstance), m_sParameterGroup(sParameterGroup), m_sParameter(sParameter), m_bEditable(bEditable), m_sMin(sMin), m_sMax(sMax), m_sStep(sStep)
{

}

CUIModule_ContentParameterListEntry::~CUIModule_ContentParameterListEntry()
{

}

std::string CUIModule_ContentParameterListEntry::getInstance()
{
	return m_sInstance;
}

std::string CUIModule_ContentParameterListEntry::getParameterGroup()
{
	return m_sParameterGroup;
}

std::string CUIModule_ContentParameterListEntry::getParameter()
{
	return m_sParameter;
}

bool CUIModule_ContentParameterListEntry::isEditable()
{
	return m_bEditable;
}

std::string CUIModule_ContentParameterListEntry::getMin()
{
	return m_sMin;
}

std::string CUIModule_ContentParameterListEntry::getMax()
{
	return m_sMax;
}

std::string CUIModule_ContentParameterListEntry::getStep()
{
	return m_sStep;
}

bool CUIModule_ContentParameterListEntry::isFullGroup()
{
	return m_sParameter.empty ();
}

bool CUIModule_ContentParameterListEntry::isFullInstance()
{
	return m_sParameterGroup.empty();
}


PUIModule_ContentParameterList CUIModule_ContentParameterList::makeFromXML(const pugi::xml_node& xmlNode, const std::string& sItemName, const std::string& sModulePath, PUIModuleEnvironment pUIModuleEnvironment)
{
	LibMCAssertNotNull(pUIModuleEnvironment);
	auto loadingtextAttrib = xmlNode.attribute("loadingtext");
	auto entriesperpageAttrib = xmlNode.attribute("entriesperpage");
	std::string sLoadingText = loadingtextAttrib.as_string();

	int nEntriesPerPage;
	if (!entriesperpageAttrib.empty()) {
		nEntriesPerPage = entriesperpageAttrib.as_int();
		if (nEntriesPerPage < AMC_API_KEY_UI_ITEM_MINENTRIESPERPAGE)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDENTRIESPERPAGE);
		if (nEntriesPerPage > AMC_API_KEY_UI_ITEM_MAXENTRIESPERPAGE)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDENTRIESPERPAGE);
	}
	else {
		nEntriesPerPage = AMC_API_KEY_UI_ITEM_DEFAULTENTRIESPERPAGE;
	}

	auto pParameterList = std::make_shared <CUIModule_ContentParameterList>(sLoadingText, nEntriesPerPage, pUIModuleEnvironment->stateMachineData(), sItemName, sModulePath);

	pParameterList->loadFromXML(xmlNode);

	return pParameterList;

}




CUIModule_ContentParameterList::CUIModule_ContentParameterList(const std::string& sLoadingText, const uint32_t nEntriesPerPage, PStateMachineData pStateMachineData, const std::string& sItemName, const std::string & sModulePath)
	: CUIModule_ContentItem (AMCCommon::CUtils::createUUID(), sItemName, sModulePath), m_sLoadingText (sLoadingText), m_nEntriesPerPage (nEntriesPerPage), m_pStateMachineData(pStateMachineData)
{
	if (pStateMachineData.get() == nullptr)
		throw ELibMCInterfaceException (LIBMC_ERROR_INVALIDPARAM);

	m_sParameterDescCaption = "Parameter";
	m_sParameterValueCaption = "Value";
	m_sParameterGroupCaption = "Group";
	m_sParameterSystemCaption = "System";

	// Column defaults: all visible, flexible width, not resizable.
	m_ColumnDescription = sColumnConfig();
	m_ColumnValue = sColumnConfig();
	m_ColumnGroup = sColumnConfig();
	m_ColumnSystem = sColumnConfig();

}

CUIModule_ContentParameterList::~CUIModule_ContentParameterList()
{

}



void CUIModule_ContentParameterList::collectRows(std::vector<sParameterListRow>& rows, std::string& sDefinitionSignature)
{
	for (auto entry : m_List) {
		auto pParameterHandler = m_pStateMachineData->getParameterHandler(entry->getInstance());
		std::string sSystemDescription = pParameterHandler->getDescription();

		std::vector<PParameterGroup> groups;
		if (entry->isFullInstance()) {
			uint32_t nGroupCount = pParameterHandler->getGroupCount();
			for (uint32_t nGroupIndex = 0; nGroupIndex < nGroupCount; nGroupIndex++)
				groups.push_back(pParameterHandler->getGroup(nGroupIndex));
		}
		else {
			groups.push_back(pParameterHandler->findGroup(entry->getParameterGroup(), true));
		}

		for (auto pGroup : groups) {
			sDefinitionSignature += entry->getInstance() + "/" + pGroup->getName();

			if (entry->isFullInstance() || entry->isFullGroup()) {
				uint32_t nCount = pGroup->getParameterCount();
				sDefinitionSignature += ":" + std::to_string(nCount) + ";";
				for (uint32_t nIndex = 0; nIndex < nCount; nIndex++)
					rows.push_back(sParameterListRow{ pGroup, nIndex, "", entry->getInstance(), sSystemDescription, entry.get() });
			}
			else {
				sDefinitionSignature += "." + entry->getParameter() + ";";
				rows.push_back(sParameterListRow{ pGroup, 0, entry->getParameter(), entry->getInstance(), sSystemDescription, entry.get() });
			}
		}
	}
}

std::string CUIModule_ContentParameterList::readRowValue(const sParameterListRow& row)
{
	if (row.m_sParameterName.empty())
		return row.m_pGroup->getParameterValueByIndex(row.m_nParameterIndex);

	return row.m_pGroup->getParameterValueByName(row.m_sParameterName);
}

void CUIModule_ContentParameterList::writeRowDefinitionToJSON(CJSONWriter& writer, CJSONWriterArray& entryArray, const sParameterListRow& row)
{
	std::string sParameterName = row.m_sParameterName;
	std::string sDescription;
	std::string sDefaultValue;
	std::string sType;

	if (sParameterName.empty()) {
		row.m_pGroup->getParameterInfo(row.m_nParameterIndex, sParameterName, sDescription, sDefaultValue);
		sType = parameterDataTypeToString(row.m_pGroup->getParameterDataTypeByIndex(row.m_nParameterIndex));
	}
	else {
		row.m_pGroup->getParameterInfoByName(sParameterName, sDescription, sDefaultValue);
		sType = parameterDataTypeToString(row.m_pGroup->getParameterDataTypeByName(sParameterName));
	}

	auto pEntry = row.m_pEntry;
	bool bEditable = (pEntry != nullptr) && pEntry->isEditable() && (!m_sEditEvent.empty());

	CJSONWriterObject entryObject(writer);
	entryObject.addString(AMC_API_KEY_UI_ITEMPARAMETERDESCRIPTION, sDescription);
	entryObject.addString(AMC_API_KEY_UI_ITEMPARAMETERGROUP, row.m_pGroup->getDescription());
	entryObject.addString(AMC_API_KEY_UI_ITEMPARAMETERSYSTEM, row.m_sSystemDescription);
	entryObject.addString(AMC_API_KEY_UI_ITEMPARAMETERNAME, sParameterName);
	entryObject.addString(AMC_API_KEY_UI_ITEMPARAMETERINSTANCE, row.m_sInstanceName);
	entryObject.addString(AMC_API_KEY_UI_ITEMPARAMETERGROUPNAME, row.m_pGroup->getName());
	entryObject.addString(AMC_API_KEY_UI_ITEMPARAMETERTYPE, sType);
	entryObject.addBool(AMC_API_KEY_UI_ITEMPARAMETEREDITABLE, bEditable);
	entryObject.addString(AMC_API_KEY_UI_ITEMPARAMETERMIN, (pEntry != nullptr) ? pEntry->getMin() : "");
	entryObject.addString(AMC_API_KEY_UI_ITEMPARAMETERMAX, (pEntry != nullptr) ? pEntry->getMax() : "");
	entryObject.addString(AMC_API_KEY_UI_ITEMPARAMETERSTEP, (pEntry != nullptr) ? pEntry->getStep() : "");
	entryArray.addObject(entryObject);
}

std::vector<uint32_t> CUIModule_ContentParameterList::parseRowIndices(const std::string& sRowIndices, size_t nRowCount)
{
	auto parseIndex = [nRowCount](const std::string& sValue) -> uint32_t {
		if (sValue.empty() || (sValue.find_first_not_of("0123456789") != std::string::npos) || (sValue.length() > 9))
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
		uint32_t nIndex = (uint32_t)std::stoul(sValue);
		if (nIndex >= nRowCount)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDINDEX);
		return nIndex;
	};

	std::vector<uint32_t> indices;
	std::stringstream stream(sRowIndices);
	std::string sToken;
	while (std::getline(stream, sToken, ',')) {
		auto nDashPosition = sToken.find('-');
		uint32_t nFirst = parseIndex(sToken.substr(0, nDashPosition));
		uint32_t nLast = (nDashPosition == std::string::npos) ? nFirst : parseIndex(sToken.substr(nDashPosition + 1));
		if (nLast < nFirst)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

		for (uint32_t nIndex = nFirst; nIndex <= nLast; nIndex++) {
			if (indices.size() >= nRowCount)
				throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
			indices.push_back(nIndex);
		}
	}

	return indices;
}

void CUIModule_ContentParameterList::writeDefinitionToJSON(CJSONWriter& writer)
{
	std::vector<sParameterListRow> rows;
	std::string sDefinitionSignature;
	collectRows(rows, sDefinitionSignature);

	CJSONWriterArray entryArray(writer);
	CJSONWriterArray valueArray(writer);
	for (auto& row : rows) {
		writeRowDefinitionToJSON(writer, entryArray, row);
		valueArray.addString(readRowValue(row));
	}

	writer.addString(AMC_API_KEY_UI_PARAMETERLIST_DEFINITIONHASH, AMCCommon::CUtils::calculateSHA256FromString(sDefinitionSignature));
	writer.addArray(AMC_API_KEY_UI_ITEMENTRIES, entryArray);
	writer.addArray(AMC_API_KEY_UI_PARAMETERLIST_VALUES, valueArray);
}

void CUIModule_ContentParameterList::writeValuesToJSON(CJSONWriter& writer, const std::string& sKnownDefinitionHash, const std::string& sKnownValuesHash, const std::string& sRowIndices)
{
	std::vector<sParameterListRow> rows;
	std::string sDefinitionSignature;
	collectRows(rows, sDefinitionSignature);

	std::string sDefinitionHash = AMCCommon::CUtils::calculateSHA256FromString(sDefinitionSignature);
	writer.addString(AMC_API_KEY_UI_PARAMETERLIST_DEFINITIONHASH, sDefinitionHash);
	if (sDefinitionHash != sKnownDefinitionHash) {
		writer.addBoolean(AMC_API_KEY_UI_PARAMETERLIST_DEFINITIONCHANGED, true);
		return;
	}

	std::vector<std::string> values;
	if (sRowIndices.empty()) {
		values.reserve(rows.size());
		for (auto& row : rows)
			values.push_back(readRowValue(row));
	}
	else {
		auto indices = parseRowIndices(sRowIndices, rows.size());
		values.reserve(indices.size());
		for (auto nIndex : indices)
			values.push_back(readRowValue(rows.at(nIndex)));
	}

	// The requested row set is part of the hash, so a changed selection never reports stale values as unchanged.
	std::string sValuesSignature = sRowIndices;
	for (auto& sValue : values) {
		sValuesSignature += '\x1f';
		sValuesSignature += sValue;
	}
	std::string sValuesHash = AMCCommon::CUtils::calculateSHA256FromString(sValuesSignature);
	writer.addString(AMC_API_KEY_UI_PARAMETERLIST_VALUESHASH, sValuesHash);

	if (sValuesHash == sKnownValuesHash) {
		writer.addBoolean(AMC_API_KEY_UI_PARAMETERLIST_UNCHANGED, true);
		return;
	}

	CJSONWriterArray valueArray(writer);
	for (auto& sValue : values)
		valueArray.addString(sValue);
	writer.addArray(AMC_API_KEY_UI_PARAMETERLIST_VALUES, valueArray);
}


void CUIModule_ContentParameterList::writeColumnsToJSON(CJSONWriter& writer, CJSONWriterObject& object)
{
	struct sColumnDescriptor {
		const char* pIdentifier;
		const char* pValueKey;
		const std::string& sCaption;
		const sColumnConfig& config;
	};

	std::vector<sColumnDescriptor> columns = {
		{ "parameter", AMC_API_KEY_UI_ITEMPARAMETERDESCRIPTION, m_sParameterDescCaption, m_ColumnDescription },
		{ "value", AMC_API_KEY_UI_ITEMPARAMETERVALUE, m_sParameterValueCaption, m_ColumnValue },
		{ "group", AMC_API_KEY_UI_ITEMPARAMETERGROUP, m_sParameterGroupCaption, m_ColumnGroup },
		{ "system", AMC_API_KEY_UI_ITEMPARAMETERSYSTEM, m_sParameterSystemCaption, m_ColumnSystem }
	};

	CJSONWriterArray columnsArray(writer);
	for (auto& column : columns) {
		CJSONWriterObject columnObject(writer);
		columnObject.addString(AMC_API_KEY_UI_ITEMCOLUMNIDENTIFIER, column.pIdentifier);
		columnObject.addString(AMC_API_KEY_UI_ITEMVALUE, column.pValueKey);
		columnObject.addString(AMC_API_KEY_UI_ITEMTEXT, column.sCaption);
		columnObject.addBool(AMC_API_KEY_UI_ITEMCOLUMNVISIBLE, column.config.visible);
		columnObject.addString(AMC_API_KEY_UI_ITEMCOLUMNWIDTH, column.config.width);
		columnObject.addBool(AMC_API_KEY_UI_ITEMCOLUMNSIZEABLE, column.config.sizeable);
		columnsArray.addObject(columnObject);
	}

	object.addArray(AMC_API_KEY_UI_ITEMCOLUMNS, columnsArray);
}


void CUIModule_ContentParameterList::addEntry(const std::string& sInstance, const std::string& sParameterGroup, const std::string& sParameter, bool bEditable, const std::string& sMin, const std::string& sMax, const std::string& sStep)
{
	if (m_List.size() >= AMC_CONTENT_MAXENTRYCOUNT)
		throw ELibMCInterfaceException(LIBMC_ERROR_TOOMANYCONTENTPARAMETERS);

	m_List.push_back (std::make_shared <CUIModule_ContentParameterListEntry> (sInstance, sParameterGroup, sParameter, bEditable, sMin, sMax, sStep));
}


uint32_t CUIModule_ContentParameterList::getEntryCount()
{
	return (uint32_t)m_List.size();
}

CUIModule_ContentParameterListEntry* CUIModule_ContentParameterList::getEntry(const uint32_t nIndex)
{
	if (nIndex >= m_List.size())
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDINDEX);

	return m_List[nIndex].get();
}


void CUIModule_ContentParameterList::loadFromXML(const pugi::xml_node& xmlNode)
{
	// Optional list-wide edit event. When present, entries flagged editable may
	// be edited inline; the frontend triggers this event on commit.
	auto editEventAttrib = xmlNode.attribute("editevent");
	m_sEditEvent = editEventAttrib.as_string();

	// Optional stable preference key override. Falls back to the config-derived
	// item path (see registerFrontendAttributes) when neither attribute is given.
	auto preferenceKeyAttrib = xmlNode.attribute("preferencekey");
	if (!preferenceKeyAttrib.empty())
		m_sPreferenceKey = preferenceKeyAttrib.as_string();
	else {
		auto nameAttrib = xmlNode.attribute("name");
		if (!nameAttrib.empty())
			m_sPreferenceKey = nameAttrib.as_string();
	}

	// Optional per-column configuration via <column> subnodes. Columns not
	// listed keep their defaults (visible, flexible width, not resizable).
	auto columnNodes = xmlNode.children("column");
	for (auto columnNode : columnNodes) {
		std::string sIdentifier = columnNode.attribute("identifier").as_string();

		sColumnConfig* pColumn = nullptr;
		if (sIdentifier == "parameter")
			pColumn = &m_ColumnDescription;
		else if (sIdentifier == "value")
			pColumn = &m_ColumnValue;
		else if (sIdentifier == "group")
			pColumn = &m_ColumnGroup;
		else if (sIdentifier == "system")
			pColumn = &m_ColumnSystem;
		else
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

		pColumn->visible = columnNode.attribute("visible").as_bool(true);
		pColumn->width = columnNode.attribute("width").as_string();
		pColumn->sizeable = columnNode.attribute("sizeable").as_bool(false);
	}

	auto entryNodes = xmlNode.children ("entry");
	for (auto entryNode : entryNodes) {

		auto stateMachineAttrib = entryNode.attribute("statemachine");
		std::string sInstanceName = stateMachineAttrib.as_string();
		if (sInstanceName.empty ())
			throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGCONTENTSTATEMACHINENAME);

		auto groupAttrib = entryNode.attribute("group");
		std::string sGroupName = groupAttrib.as_string();

		auto pStateMachineInstance = m_pStateMachineData->getParameterHandler(sInstanceName);

		// Parameter may be empty (then add full group)
		auto parameterAttrib = entryNode.attribute("parameter");
		std::string sParameterName = parameterAttrib.as_string();

		// Group may be empty (then add full instance)
		if (!sGroupName.empty()) {
			pStateMachineInstance->findGroup(sGroupName, true);
		}
		else {
			if (!sParameterName.empty ())
				throw ELibMCInterfaceException(LIBMC_ERROR_EMPTYGROUPNAMEBUTPARAMETERGIVEN);
		}

		// Optional per-entry inline-editing attributes.
		bool bEditable = entryNode.attribute("editable").as_bool(false);
		std::string sMin = entryNode.attribute("min").as_string();
		std::string sMax = entryNode.attribute("max").as_string();
		std::string sStep = entryNode.attribute("step").as_string();

		addEntry(sInstanceName, sGroupName, sParameterName, bEditable, sMin, sMax, sStep);

	}
}

std::string CUIModule_ContentParameterList::getItemType()
{
	return "parameterlist";
}

void CUIModule_ContentParameterList::registerFrontendAttributes()
{
	CUIExpression loadingTextExpr;
	loadingTextExpr.setFixedValue(m_sLoadingText);
	registerItemStringAttribute("loadingtext", loadingTextExpr);
	CUIExpression entriesExpr;
	entriesExpr.setFixedValue(std::to_string(m_nEntriesPerPage));
	registerItemIntegerAttribute("entriesperpage", entriesExpr);
	CUIExpression editEventExpr;
	editEventExpr.setFixedValue(m_sEditEvent);
	registerItemStringAttribute("editevent", editEventExpr);

	// Stable per-list identifier used by the frontend to scope persisted view
	// preferences. Prefer the authored override, otherwise use the config-derived
	// item path (restart-invariant, unlike the generated module/item UUID).
	CUIExpression preferenceKeyExpr;
	preferenceKeyExpr.setFixedValue(m_sPreferenceKey.empty() ? getItemPath() : m_sPreferenceKey);
	registerItemStringAttribute("preferencekey", preferenceKeyExpr);

	registerItemProviderAttribute(AMC_API_KEY_UI_ITEMCOLUMNS, eUIFrontendDefinitionAttributeType::atArray, eUIFrontendDefinitionAttributeScope::asGlobal,
		[this](CJSONWriter& writer, CJSONWriterObject& object, const std::string& sName, CStateMachineData* pStateMachineData, CUIFrontendState* pFrontendState) {
			writeColumnsToJSON(writer, object);
		});
}

