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
#include "amc_ui_module_content.hpp"
#include "amc_ui_modulefactory.hpp"
#include "amc_ui_module_contentleaf.hpp"

#include "amc_api_constants.hpp"
#include "amc_resourcepackage.hpp"
#include "amc_parameterhandler.hpp"

#include "amc_statemachinedata.hpp"
#include "Common/common_utils.hpp"

#include "libmc_exceptiontypes.hpp"

using namespace AMC;

CUIModule_Content::CUIModule_Content(pugi::xml_node& xmlNode, const std::string& sPath, PUIModuleEnvironment pUIModuleEnvironment)
	: CUIModule(getNameFromXML(xmlNode), sPath, pUIModuleEnvironment->getFrontendDefinition ()), m_nNamingIDCounter(1)
{
	LibMCAssertNotNull(pUIModuleEnvironment.get());

	if (getTypeFromXML(xmlNode) != getStaticType())
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDMODULETYPE, "should be " + getStaticType ());

	if (sPath.empty())
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDMODULEPATH, m_sName);
	
	auto headlineAttrib = xmlNode.attribute("headline");
	if (!headlineAttrib.empty ())
		m_sHeadLine = headlineAttrib.as_string();

	auto captionAttrib = xmlNode.attribute("caption");
	if (!captionAttrib.empty())
		m_sCaption = captionAttrib.as_string();
	
	auto titleAttrib = xmlNode.attribute("title");
	if (!titleAttrib.empty())
		m_sTitle = titleAttrib.as_string();

	auto subtitleAttrib = xmlNode.attribute("subtitle");
	if (!subtitleAttrib.empty())
		m_sSubtitle = subtitleAttrib.as_string();

	auto visibleAttrib = xmlNode.attribute("visible");
	if (!visibleAttrib.empty())
		m_bVisible = visibleAttrib.as_bool();
	else
		m_bVisible = true;

	auto cardStyleAttrib = xmlNode.attribute("cardstyle");
	m_sCardStyle = cardStyleAttrib.empty() ? "none" : cardStyleAttrib.as_string();

	auto cardColorAttrib = xmlNode.attribute("cardcolor");
	m_sCardColor = cardColorAttrib.empty() ? "" : cardColorAttrib.as_string();

	m_nSpacing   = (uint32_t) xmlNode.attribute("spacing").as_int(0);
	m_nElevation = (uint32_t) xmlNode.attribute("elevation").as_int(2);

	auto children = xmlNode.children();
	for (auto childNode : children) {
		std::string sChildName = childNode.name();
		if (sChildName.empty())
			continue;

		if (!CUIModule_ContentLeaf::isSupportedModuleType(sChildName) &&
			!CUIModuleFactory::moduleTypeIsRegistered(sChildName))
			continue;

		auto sSubModuleName = readSubModuleNameFromXML(childNode, sChildName);
		auto nameAttrib = childNode.attribute("name");
		if (nameAttrib.empty())
			childNode.append_attribute("name").set_value(sSubModuleName.c_str());
		else
			nameAttrib.set_value(sSubModuleName.c_str());

		// Keep submodule paths under the content module path.
		auto pSubModule = CUIModuleFactory::createModule(childNode, m_sModulePath, pUIModuleEnvironment);
		addSubModule(pSubModule);

	}

	/////////////////////////////////////////////////////////////////////////////////////
	// New UI Frontend System - register v2 attributes for this module and all items
	/////////////////////////////////////////////////////////////////////////////////////

	CUIExpression headlineExpr;
	headlineExpr.setFixedValue(m_sHeadLine);
	CUIExpression captionExpr;
	captionExpr.setFixedValue(m_sCaption);
	CUIExpression titleExpr;
	titleExpr.setFixedValue(m_sTitle);
	CUIExpression subtitleExpr;
	subtitleExpr.setFixedValue(m_sSubtitle);
	// "visible" supports both fixed values (visible="true") and dynamic
	// state-machine expressions (sync:visible="main.jobinfo.isactive").
	// Default is "1" (visible) when the attribute is absent.
	CUIExpression visibleExpr = CUIModule::makeVisibleExpressionFromXML(xmlNode);

	registerStringAttribute("headline", headlineExpr);
	registerStringAttribute("caption", captionExpr);
	registerStringAttribute("title", titleExpr);
	registerStringAttribute("subtitle", subtitleExpr);
	registerBoolAttribute("visible", visibleExpr);

	CUIExpression cardStyleExpr;
	cardStyleExpr.setFixedValue(m_sCardStyle);
	registerStringAttribute("cardstyle", cardStyleExpr);

	CUIExpression cardColorExpr;
	cardColorExpr.setFixedValue(m_sCardColor);
	registerStringAttribute("cardcolor", cardColorExpr);

	CUIExpression spacingExpr;
	spacingExpr.setFixedValue(std::to_string(m_nSpacing));
	registerStringAttribute("spacing", spacingExpr);

	CUIExpression elevationExpr;
	elevationExpr.setFixedValue(std::to_string(m_nElevation));
	registerStringAttribute("elevation", elevationExpr);

}


CUIModule_Content::~CUIModule_Content()
{
}



std::string CUIModule_Content::getStaticType()
{
	return "content";
}

std::string CUIModule_Content::getType()
{
	return getStaticType();
}

std::string CUIModule_Content::getHeadLine()
{
	return m_sHeadLine;
}

std::string CUIModule_Content::getCaption()
{
	return m_sCaption;
}


std::string CUIModule_Content::getTitle()
{
	return m_sTitle;
}

std::string CUIModule_Content::getSubtitle()
{
	return m_sSubtitle;
}

bool CUIModule_Content::isVisible()
{
	return m_bVisible;
}

void CUIModule_Content::populateLegacyClientVariables(CParameterHandler* pParameterHandler)
{
	LibMCAssertNotNull(pParameterHandler);

	auto pGroup = pParameterHandler->addGroup(m_sModulePath, "content UI element");
	pGroup->addNewBoolParameter(AMC_API_KEY_UI_VISIBLE, "visibility of the UI content", m_bVisible);

	for (auto pSubModule : m_SubModules)
		pSubModule->populateLegacyClientVariables(pParameterHandler);

}

void CUIModule_Content::writeLegacyDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& moduleObject, CParameterHandler* pLegacyClientVariableHandler)
{
	moduleObject.addString(AMC_API_KEY_UI_MODULENAME, getName());
	moduleObject.addString(AMC_API_KEY_UI_MODULETYPE, getType());
	moduleObject.addString(AMC_API_KEY_UI_MODULEUUID, getUUID());
	moduleObject.addString(AMC_API_KEY_UI_HEADLINE, m_sHeadLine);
	moduleObject.addString(AMC_API_KEY_UI_TITLE, m_sTitle);
	moduleObject.addString(AMC_API_KEY_UI_SUBTITLE, m_sSubtitle);
	moduleObject.addString(AMC_API_KEY_UI_CAPTION, m_sCaption);
	moduleObject.addBool(AMC_API_KEY_UI_VISIBLE, m_bVisible);
	moduleObject.addString("cardstyle", m_sCardStyle);
	moduleObject.addString("cardcolor", m_sCardColor);
	moduleObject.addInteger("spacing",   (int64_t) m_nSpacing);
	moduleObject.addInteger("elevation", (int64_t) m_nElevation);

	CJSONWriterArray modulesNode(writer);
	for (auto pSubModule : m_SubModules) {
		CJSONWriterObject subModuleObject(writer);
		pSubModule->writeLegacyDefinitionToJSON(writer, subModuleObject, pLegacyClientVariableHandler);
		modulesNode.addObject(subModuleObject);
	}
	moduleObject.addArray(AMC_API_KEY_UI_MODULES, modulesNode);

}

void CUIModule_Content::addContentToJSON(CJSONWriter& writer, CJSONWriterObject& moduleObject, CParameterHandler* pClientVariableHandler, uint32_t nStateID)
{
	moduleObject.addString(AMC_API_KEY_UI_UUID, m_sUUID);

	auto pGroup = pClientVariableHandler->findGroup(m_sModulePath, true);
	auto bVisible = pGroup->getBoolParameterValueByName(AMC_API_KEY_UI_VISIBLE);
	moduleObject.addBool(AMC_API_KEY_UI_VISIBLE, bVisible);
}

PUIModuleItem CUIModule_Content::findLegacyItem(const std::string& sUUID)
{
	for (auto pSubModule : m_SubModules) {
		auto pItem = pSubModule->findLegacyItem(sUUID);
		if (pItem.get() != nullptr)
			return pItem;
	}

	return nullptr;
}

void CUIModule_Content::addSubModule(PUIModule pSubModule)
{
	LibMCAssertNotNull(pSubModule.get());

	m_SubModules.push_back(pSubModule);
	m_SubModuleMap.insert(std::make_pair(pSubModule->getUUID(), pSubModule));

}

void CUIModule_Content::populateModuleMap(std::map<std::string, PUIModule>& moduleMap)
{
	moduleMap.insert(std::make_pair(m_sUUID, std::make_shared<CUIModule_Content>(*this)));
	for (auto pSubModule : m_SubModules) {
		moduleMap.insert(std::make_pair(pSubModule->getUUID(), pSubModule));
		pSubModule->populateModuleMap(moduleMap);
	}
}

void CUIModule_Content::populateLegacyItemMap(std::map<std::string, PUIModuleItem>& itemMap)
{
	for (auto pSubModule : m_SubModules)
		pSubModule->populateLegacyItemMap(itemMap);
}

void CUIModule_Content::configureLegacyPostLoading()
{
	for (auto pSubModule : m_SubModules)
		pSubModule->configureLegacyPostLoading();
}

std::string CUIModule_Content::getDefaultContentName(const std::string& sPrefix)
{
	if (sPrefix.empty())
		throw ELibMCCustomException(LIBMC_ERROR_EMPTYITEMPREFIX, m_sModulePath);

	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sPrefix))
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDITEMPREFIX, m_sModulePath + "." + sPrefix);


	std::string sName = sPrefix + std::to_string(m_nNamingIDCounter);
	m_nNamingIDCounter++;

	return sName;
}


std::string CUIModule_Content::readSubModuleNameFromXML(const pugi::xml_node& moduleNode, const std::string& sPrefix)
{
	auto nameAttrib = moduleNode.attribute("name");
	std::string sSubModuleName = nameAttrib.as_string();
	if (sSubModuleName.empty())
		sSubModuleName = getDefaultContentName(sPrefix);

	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sSubModuleName))
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDITEMPATH, m_sModulePath + "." + sSubModuleName);

	return sSubModuleName;
}


/////////////////////////////////////////////////////////////////////////////////////
// New UI Frontend System
/////////////////////////////////////////////////////////////////////////////////////

bool CUIModule_Content::isVersion2FrontendModule()
{
	return true;
}

void CUIModule_Content::frontendWriteModuleStatusToJSON(CJSONWriter& writer, CJSONWriterObject& moduleObject, CUIFrontendState* pFrontendState, CStateMachineData* pStateMachineData)
{
	CUIModule::frontendWriteModuleStatusToJSON(writer, moduleObject, pFrontendState, pStateMachineData);

	CJSONWriterArray submodulesArray(writer);

	for (auto& pSubModule : m_SubModules) {
		if (pSubModule->isVersion2FrontendModule()) {
			CJSONWriterObject subModuleObject(writer);
			pSubModule->frontendWriteModuleStatusToJSON(writer, subModuleObject, pFrontendState, pStateMachineData);
			submodulesArray.addObject(subModuleObject);
		}
	}

	moduleObject.addArray("submodules", submodulesArray);
}
