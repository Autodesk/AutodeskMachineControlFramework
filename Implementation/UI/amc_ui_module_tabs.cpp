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

CUIModule_Tabs::CUIModule_Tabs(pugi::xml_node& xmlNode, const std::string& sPath, PUIModuleEnvironment pUIModuleEnvironment)
: CUIModule (getNameFromXML(xmlNode))
{

	LibMCAssertNotNull(pUIModuleEnvironment.get());

	auto children = xmlNode.children();
	for (auto childNode : children) {
		auto pTab = CUIModuleFactory::createModule(childNode, sPath, pUIModuleEnvironment);
		addTab (pTab);			
	}

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


void CUIModule_Tabs::writeDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& moduleObject)
{
	moduleObject.addString(AMC_API_KEY_UI_MODULENAME, getName());
	moduleObject.addString(AMC_API_KEY_UI_MODULETYPE, getType());
	moduleObject.addString(AMC_API_KEY_UI_CAPTION, m_sCaption);

	CJSONWriterArray tabsNode(writer);
	for (auto tab : m_Tabs) {
		CJSONWriterObject tabObject(writer);
		tab->writeDefinitionToJSON (writer, tabObject);
		tabsNode.addObject(tabObject);
	}
	moduleObject.addArray(AMC_API_KEY_UI_TABS, tabsNode);

}

PUIModuleItem CUIModule_Tabs::findItem(const std::string& sUUID)
{
	auto iIter = m_ItemMap.find(sUUID);
	if (iIter != m_ItemMap.end())
		return iIter->second;

	return nullptr;
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
	m_TabMap.insert(std::make_pair (pTab->getUUID (), pTab));

	pTab->populateItemMap(m_ItemMap);

}

void CUIModule_Tabs::populateItemMap(std::map<std::string, PUIModuleItem>& itemMap)
{
	for (auto pTab : m_Tabs)
		pTab->populateItemMap(itemMap);
}

void CUIModule_Tabs::configurePostLoading()
{
	for (auto pTab : m_Tabs)
		pTab->configurePostLoading();
}