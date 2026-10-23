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

#include "amc_ui_module_tabs.hpp"

#include "amc_api_constants.hpp"
#include "amc_resourcepackage.hpp"

#include "libmc_exceptiontypes.hpp"

using namespace AMC;

/////////////////////////////////////////////////////////////////////////////////////
// General module functionality
/////////////////////////////////////////////////////////////////////////////////////

CUIModule_Tabs::CUIModule_Tabs(pugi::xml_node& xmlNode, const std::string& sPath, PUIModuleEnvironment pUIModuleEnvironment)
: CUIModule (getNameFromXML(xmlNode), getStaticType(), sPath, pUIModuleEnvironment->getFrontendDefinition ())
{

	LibMCAssertNotNull(pUIModuleEnvironment.get());

	m_pModuleStore->setAlwaysWriteSubmodules(true);

	auto children = xmlNode.children();
	for (auto childNode : children) {
		auto pTab = CUIModuleFactory::createModule(childNode, sPath, pUIModuleEnvironment);
		addTab (pTab);			
	}

	auto captionAttrib = xmlNode.attribute("caption");
	m_sCaption = captionAttrib.as_string();

	CUIExpression captionExpr;
	captionExpr.setFixedValue(m_sCaption);
	registerStringAttribute("caption", captionExpr);

	CUIExpression visibleExpr(xmlNode, "visible", "1");
	registerBoolAttribute("visible", visibleExpr);

	// Optional card framing (mirrors the "content" module), so a tab control can be
	// rendered inside an elevated/outlined/tinted card with an optional title/subtitle.
	auto cardStyleAttrib = xmlNode.attribute("cardstyle");
	CUIExpression cardStyleExpr;
	cardStyleExpr.setFixedValue(cardStyleAttrib.empty() ? "none" : cardStyleAttrib.as_string());
	registerStringAttribute("cardstyle", cardStyleExpr);

	CUIExpression cardTitleExpr;
	cardTitleExpr.setFixedValue(xmlNode.attribute("title").as_string());
	registerStringAttribute("title", cardTitleExpr);

	CUIExpression cardSubtitleExpr;
	cardSubtitleExpr.setFixedValue(xmlNode.attribute("subtitle").as_string());
	registerStringAttribute("subtitle", cardSubtitleExpr);

}


CUIModule_Tabs::~CUIModule_Tabs()
{
}



std::string CUIModule_Tabs::getStaticType()
{
	return "tabs";
}

std::string CUIModule_Tabs::getType()
{
	return getStaticType();
}

std::string CUIModule_Tabs::getCaption()
{
	return m_sCaption;
}


PUIModule CUIModule_Tabs::findTab(const std::string& sUUID)
{
	auto iIter = m_TabMap.find(sUUID);
	if (iIter != m_TabMap.end())
		return iIter->second;

	return nullptr;
}

void CUIModule_Tabs::addTab(PUIModule pTab)
{
	if (pTab.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	m_Tabs.push_back(pTab);
	m_TabMap.insert(std::make_pair(pTab->getUUID(), pTab));

	pTab->populateLegacyItemMap(m_ItemMap);

	if (pTab->isVersion2FrontendModule())
		m_pModuleStore->addChildStore(pTab->getFrontendModuleStore());

}


/////////////////////////////////////////////////////////////////////////////////////
// Legacy UI System
/////////////////////////////////////////////////////////////////////////////////////

PUIModuleItem CUIModule_Tabs::findLegacyItem(const std::string& sUUID)
{
	auto iIter = m_ItemMap.find(sUUID);
	if (iIter != m_ItemMap.end())
		return iIter->second;

	return nullptr;
}

void CUIModule_Tabs::populateLegacyItemMap(std::map<std::string, PUIModuleItem>& itemMap)
{
	for (auto pTab : m_Tabs)
		pTab->populateLegacyItemMap(itemMap);
}

void CUIModule_Tabs::configureLegacyPostLoading()
{
	for (auto pTab : m_Tabs)
		pTab->configureLegacyPostLoading();
}


void CUIModule_Tabs::populateLegacyClientVariables(CParameterHandler* pParameterHandler)
{
	LibMCAssertNotNull(pParameterHandler);
	for (auto pTab : m_Tabs)
		pTab->populateLegacyClientVariables(pParameterHandler);

}

void CUIModule_Tabs::populateModuleMap(std::map<std::string, PUIModule>& moduleMap)
{
	moduleMap.insert(std::make_pair(m_sUUID, std::make_shared<CUIModule_Tabs>(*this)));

	for (auto pTab : m_Tabs) {
		moduleMap.insert(std::make_pair(pTab->getUUID(), pTab));
		pTab->populateModuleMap(moduleMap);
	}
}

/////////////////////////////////////////////////////////////////////////////////////
// New UI Frontend System
/////////////////////////////////////////////////////////////////////////////////////


bool CUIModule_Tabs::isVersion2FrontendModule()
{
	return true;
}
