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
#include "amc_ui_module_contentleaf.hpp"

#include "amc_ui_module_contentitem.hpp"
#include "amc_ui_module_contentitem_paragraph.hpp"
#include "amc_ui_module_contentitem_image.hpp"
#include "amc_ui_module_contentitem_chart.hpp"
#include "amc_ui_module_contentitem_upload.hpp"
#include "amc_ui_module_contentitem_buildlist.hpp"
#include "amc_ui_module_contentitem_executionlist.hpp"
#include "amc_ui_module_contentitem_alertlist.hpp"
#include "amc_ui_module_contentitem_buttongroup.hpp"
#include "amc_ui_module_contentitem_parameterlist.hpp"
#include "amc_ui_module_contentitem_configurationlist.hpp"
#include "amc_ui_module_contentitem_form.hpp"
#include "amc_ui_module_contentitem_videostream.hpp"
#include "amc_ui_module_contentitem_separator.hpp"
#include "amc_ui_module_contentitem_statusbanner.hpp"
#include "amc_ui_module_contentitem_workflow.hpp"
#include "amc_ui_module_contentitem_statemachinegraph.hpp"

#include "amc_api_constants.hpp"

#include "libmc_exceptiontypes.hpp"

using namespace AMC;

CUIModule_ContentLeaf::CUIModule_ContentLeaf(pugi::xml_node& xmlNode, const std::string& sPath, PUIModuleEnvironment pUIModuleEnvironment)
	: CUIModule(getNameFromXML(xmlNode), sPath, pUIModuleEnvironment->getFrontendDefinition()),
	  m_sModuleType(getTypeFromXML(xmlNode)),
	  m_VisibleExpression(xmlNode, "visible", std::string("1")),
	  m_pStateMachineData(pUIModuleEnvironment->stateMachineData())
{
	LibMCAssertNotNull(pUIModuleEnvironment.get());

	if (!isSupportedModuleType(m_sModuleType))
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDMODULETYPE, m_sModuleType);

	if (sPath.empty())
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDMODULEPATH, m_sName);

	auto captionAttrib = xmlNode.attribute("caption");
	if (!captionAttrib.empty())
		m_sCaption = captionAttrib.as_string();

	// Use the module name as item name, and the parent path as item parent path,
	// so that the wrapped item path equals the module path.
	if (m_sModuleType == "paragraph")
		m_pItem = CUIModule_ContentParagraph::makeFromXML(xmlNode, m_sName, sPath);
	if (m_sModuleType == "image")
		m_pItem = CUIModule_ContentImage::makeFromXML(xmlNode, m_sName, sPath, pUIModuleEnvironment);
	if (m_sModuleType == "chart")
		m_pItem = CUIModule_ContentChart::makeFromXML(xmlNode, m_sName, sPath, pUIModuleEnvironment);
	if (m_sModuleType == "upload")
		m_pItem = CUIModule_ContentUpload::makeFromXML(xmlNode, m_sName, sPath, pUIModuleEnvironment);
	if (m_sModuleType == "buildlist")
		m_pItem = CUIModule_ContentBuildList::makeFromXML(xmlNode, m_sName, sPath, pUIModuleEnvironment);
	if (m_sModuleType == "executionlist")
		m_pItem = CUIModule_ContentExecutionList::makeFromXML(xmlNode, m_sName, sPath, pUIModuleEnvironment);
	if (m_sModuleType == "alertlist")
		m_pItem = CUIModule_ContentAlertList::makeFromXML(xmlNode, m_sName, sPath, pUIModuleEnvironment);
	if (m_sModuleType == "buttongroup")
		m_pItem = CUIModule_ContentButtonGroup::makeFromXML(xmlNode, m_sName, sPath, pUIModuleEnvironment);
	if (m_sModuleType == "parameterlist")
		m_pItem = CUIModule_ContentParameterList::makeFromXML(xmlNode, m_sName, sPath, pUIModuleEnvironment);
	if (m_sModuleType == "configurationlist")
		m_pItem = CUIModule_ContentConfigurationList::makeFromXML(xmlNode, m_sName, sPath, pUIModuleEnvironment);
	if (m_sModuleType == "form")
		m_pItem = CUIModule_ContentForm::makeFromXML(xmlNode, m_sName, sPath, pUIModuleEnvironment);
	if (m_sModuleType == "videostream")
		m_pItem = CUIModule_ContentVideoStream::makeFromXML(xmlNode, m_sName, sPath, pUIModuleEnvironment);
	if (m_sModuleType == "separator")
		m_pItem = CUIModule_ContentSeparator::makeFromXML(xmlNode, m_sName, sPath);
	if (m_sModuleType == "statusbanner")
		m_pItem = CUIModule_ContentStatusBanner::makeFromXML(xmlNode, m_sName, sPath);
	if (m_sModuleType == "workflow")
		m_pItem = CUIModule_ContentWorkflow::makeFromXML(xmlNode, m_sName, sPath);
	if (m_sModuleType == "statemachinegraph")
		m_pItem = CUIModule_ContentStateMachineGraph::makeFromXML(xmlNode, m_sName, sPath, pUIModuleEnvironment);

	LibMCAssertNotNull(m_pItem.get());

	auto pFrontendDefinition = pUIModuleEnvironment->getFrontendDefinition();
	m_pItem->initFrontendModuleStore(pFrontendDefinition);

	CUIExpression nameExpr;
	nameExpr.setFixedValue(m_sName);
	registerStringAttribute("name", nameExpr);

	CUIExpression captionExpr;
	captionExpr.setFixedValue(m_sCaption);
	registerStringAttribute("caption", captionExpr);

	registerBoolAttribute("visible", m_VisibleExpression);
}

CUIModule_ContentLeaf::~CUIModule_ContentLeaf()
{
}

bool CUIModule_ContentLeaf::isSupportedModuleType(const std::string& sType)
{
	return (sType == "paragraph") || (sType == "image") || (sType == "chart") || (sType == "videostream") ||
		(sType == "upload") || (sType == "buildlist") || (sType == "executionlist") || (sType == "alertlist") ||
		(sType == "buttongroup") || (sType == "parameterlist") || (sType == "configurationlist") || (sType == "form") ||
		(sType == "separator") || (sType == "statusbanner") || (sType == "workflow") || (sType == "statemachinegraph");
}

std::string CUIModule_ContentLeaf::getType()
{
	return m_sModuleType;
}

std::string CUIModule_ContentLeaf::getCaption()
{
	return m_sCaption;
}

/////////////////////////////////////////////////////////////////////////////////////
// Legacy UI System
/////////////////////////////////////////////////////////////////////////////////////

void CUIModule_ContentLeaf::addContentToJSON(CJSONWriter& writer, CJSONWriterObject& moduleObject, CParameterHandler* pClientVariableHandler, uint32_t nStateID)
{
	moduleObject.addString(AMC_API_KEY_UI_UUID, m_sUUID);
	moduleObject.addBool(AMC_API_KEY_UI_VISIBLE, m_VisibleExpression.evaluateBoolValue(m_pStateMachineData));

	// Write the wrapped item's live state (entities, buttons, imageresource, etc.)
	// so the Vue client can refresh leaf module data on every poll cycle.
	if (m_pItem.get() != nullptr)
		m_pItem->addLegacyContentToJSON(writer, moduleObject, pClientVariableHandler, nStateID);
}

void CUIModule_ContentLeaf::writeLegacyDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& moduleObject, CParameterHandler* pClientVariableHandler)
{
	moduleObject.addString(AMC_API_KEY_UI_MODULENAME, getName());
	moduleObject.addString(AMC_API_KEY_UI_MODULETYPE, getType());
	moduleObject.addString(AMC_API_KEY_UI_MODULEUUID, getUUID());
	moduleObject.addString(AMC_API_KEY_UI_CAPTION, m_sCaption);
	moduleObject.addBool(AMC_API_KEY_UI_VISIBLE, m_VisibleExpression.evaluateBoolValue(m_pStateMachineData));

	CJSONWriterArray itemsNode(writer);
	if (m_pItem.get() != nullptr) {
		CJSONWriterObject itemObject(writer);
		m_pItem->addLegacyContentToJSON(writer, itemObject, pClientVariableHandler, 0);
		itemsNode.addObject(itemObject);
	}
	moduleObject.addArray(AMC_API_KEY_UI_ITEMS, itemsNode);
}

void CUIModule_ContentLeaf::populateModuleMap(std::map<std::string, PUIModule>& moduleMap)
{
	moduleMap.insert(std::make_pair(m_sUUID, std::make_shared<CUIModule_ContentLeaf>(*this)));
}

void CUIModule_ContentLeaf::populateLegacyItemMap(std::map<std::string, PUIModuleItem>& itemMap)
{
	if (m_pItem.get() == nullptr)
		return;

	auto referenceList = m_pItem->getReferenceUUIDs();
	for (auto sUUID : referenceList)
		itemMap.insert(std::make_pair(sUUID, m_pItem));

	itemMap.insert(std::make_pair(m_sUUID, m_pItem));
}

PUIModuleItem CUIModule_ContentLeaf::findLegacyItem(const std::string& sUUID)
{
	if (m_pItem.get() == nullptr)
		return nullptr;

	if (sUUID == m_sUUID)
		return m_pItem;

	if (!m_pItem->findElementPathByUUID(sUUID).empty())
		return m_pItem;

	return nullptr;
}

void CUIModule_ContentLeaf::configureLegacyPostLoading()
{
	if (m_pItem.get() != nullptr)
		m_pItem->configurePostLoading();
}

void CUIModule_ContentLeaf::populateLegacyClientVariables(CParameterHandler* pParameterHandler)
{
	LibMCAssertNotNull(pParameterHandler);

	if (m_pItem.get() != nullptr)
		m_pItem->populateClientVariables(pParameterHandler);
}

/////////////////////////////////////////////////////////////////////////////////////
// New UI Frontend System
/////////////////////////////////////////////////////////////////////////////////////

void CUIModule_ContentLeaf::frontendWriteModuleStatusToJSON(CJSONWriter& writer, CJSONWriterObject& moduleObject, CUIFrontendState* pFrontendState, CStateMachineData* pStateMachineData)
{
	CUIModule::frontendWriteModuleStatusToJSON(writer, moduleObject, pFrontendState, pStateMachineData);

	CJSONWriterArray submodulesArray(writer);
	if (m_pItem.get() != nullptr) {
		CJSONWriterObject subModuleObject(writer);
		m_pItem->frontendWriteItemToJSON(writer, subModuleObject, pFrontendState, pStateMachineData);
		submodulesArray.addObject(subModuleObject);
	}

	moduleObject.addArray("submodules", submodulesArray);
}

bool CUIModule_ContentLeaf::isVersion2FrontendModule()
{
	return true;
}
