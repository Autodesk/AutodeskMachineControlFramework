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

#include "amc_ui_module_contentitem_workflow.hpp"
#include "libmc_interfaceexception.hpp"
#include "libmc_exceptiontypes.hpp"

#include "amc_api_constants.hpp"
#include "amc_ui_frontendstate.hpp"
#include "Common/common_utils.hpp"
#include "amc_parameterhandler.hpp"

#include "libmcdata_dynamic.hpp"

using namespace AMC;


PUIModule_ContentWorkflow CUIModule_ContentWorkflow::makeFromXML(const pugi::xml_node& xmlNode, const std::string& sItemName, const std::string& sModulePath)
{
	auto pWorkflow = std::make_shared<CUIModule_ContentWorkflow>(sItemName, sModulePath);

	for (auto stepNode : xmlNode.children("step")) {
		SWorkflowCatalogEntry entry;

		auto idAttrib = stepNode.attribute("id");
		if (idAttrib.empty())
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDPARAM, "workflow step missing 'id' attribute");
		entry.m_sId = idAttrib.as_string();

		auto labelAttrib = stepNode.attribute("label");
		entry.m_sLabel = labelAttrib.empty() ? entry.m_sId : labelAttrib.as_string();

		auto categoryAttrib = stepNode.attribute("category");
		entry.m_sCategory = categoryAttrib.empty() ? "functionblock" : categoryAttrib.as_string();

		auto colorAttrib = stepNode.attribute("color");
		entry.m_sColor = colorAttrib.empty() ? "blue" : colorAttrib.as_string();

		for (auto paramNode : stepNode.children("param")) {
			SWorkflowStepParam param;

			auto nameAttrib = paramNode.attribute("name");
			if (nameAttrib.empty())
				throw ELibMCCustomException(LIBMC_ERROR_INVALIDPARAM, "workflow step param missing 'name' attribute");
			param.m_sName = nameAttrib.as_string();

			auto typeAttrib = paramNode.attribute("type");
			param.m_sType = typeAttrib.empty() ? "string" : typeAttrib.as_string();

			auto unitAttrib = paramNode.attribute("unit");
			param.m_sUnit = unitAttrib.as_string();

			auto defaultAttrib = paramNode.attribute("default");
			param.m_sDefault = defaultAttrib.as_string();

			entry.m_Params.push_back(param);
		}

		pWorkflow->addCatalogEntry(entry);
	}

	return pWorkflow;
}

CUIModule_ContentWorkflow::CUIModule_ContentWorkflow(const std::string& sItemName, const std::string& sModulePath)
	: CUIModule_ContentItem(AMCCommon::CUtils::createUUID(), sItemName, sModulePath)
{
}

CUIModule_ContentWorkflow::~CUIModule_ContentWorkflow()
{
}

void CUIModule_ContentWorkflow::addCatalogEntry(const SWorkflowCatalogEntry& entry)
{
	m_CatalogEntries.push_back(entry);
}

void CUIModule_ContentWorkflow::addLegacyContentToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler, uint32_t nStateID)
{
	object.addString(AMC_API_KEY_UI_ITEMTYPE, "workflow");
	object.addString(AMC_API_KEY_UI_ITEMUUID, m_sUUID);
}

std::string CUIModule_ContentWorkflow::getItemType()
{
	return "workflow";
}

void CUIModule_ContentWorkflow::registerFrontendAttributes()
{
}

void CUIModule_ContentWorkflow::frontendWriteItemToJSON(CJSONWriter& writer, CJSONWriterObject& itemObject, CUIFrontendState* pFrontendState, CStateMachineData* pStateMachineData)
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

	CJSONWriterArray catalogArray(writer);
	for (const auto& entry : m_CatalogEntries) {
		CJSONWriterObject entryObject(writer);
		entryObject.addString("id", entry.m_sId);
		entryObject.addString("label", entry.m_sLabel);
		entryObject.addString("category", entry.m_sCategory);
		entryObject.addString("color", entry.m_sColor);

		if (!entry.m_Params.empty()) {
			CJSONWriterArray paramsArray(writer);
			for (const auto& param : entry.m_Params) {
				CJSONWriterObject paramObject(writer);
				paramObject.addString("name", param.m_sName);
				paramObject.addString("type", param.m_sType);
				if (!param.m_sUnit.empty())
					paramObject.addString("unit", param.m_sUnit);
				if (!param.m_sDefault.empty())
					paramObject.addString("default", param.m_sDefault);
				paramsArray.addObject(paramObject);
			}
			entryObject.addArray("params", paramsArray);
		}

		catalogArray.addObject(entryObject);
	}

	attributesObject.addArray("catalog", catalogArray);
	itemObject.addObject("attributes", attributesObject);
}
