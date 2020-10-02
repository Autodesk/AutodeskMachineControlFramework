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

#define __AMCIMPL_UI_MENUITEM
#define __AMCIMPL_UI_TOOLBARITEM
#define __AMCIMPL_API_CONSTANTS
#define __AMCIMPL_UI_PAGE
#define __AMCIMPL_UI_MODULE

#include "amc_ui_handler.hpp"
#include "amc_ui_menuitem.hpp"
#include "amc_ui_toolbaritem.hpp"
#include "amc_ui_page.hpp"
#include "amc_ui_modulefactory.hpp"
#include "amc_parameterinstances.hpp"
#include "amc_jsonwriter.hpp"
#include "amc_ui_module_item.hpp"

#include "amc_api_constants.hpp"

#include "libmc_interfaceexception.hpp"

#include "PugiXML/pugixml.hpp"

using namespace AMC;

CUIHandler::CUIHandler(PParameterInstances pParameterInstances)
    : m_dLogoAspectRatio (1.0), m_pParameterInstances (pParameterInstances)
{
    if (pParameterInstances.get() == nullptr)
        throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
}

CUIHandler::~CUIHandler()
{

}

std::string CUIHandler::getAppName()
{
    std::lock_guard<std::mutex> lockGuard(m_Mutex);

	return m_sAppName;
}

std::string CUIHandler::getCopyrightString()
{
    std::lock_guard<std::mutex> lockGuard(m_Mutex);

    return m_sCopyrightString;
}

void CUIHandler::addMenuItem_Unsafe(const std::string& sID, const std::string& sIcon, const std::string& sCaption, const std::string& sTargetPage)
{
    std::lock_guard<std::mutex> lockGuard(m_Mutex);

    auto pPage = findPage (sTargetPage);    
    m_MenuItems.push_back(std::make_shared<CUIMenuItem> (sID, sIcon, sCaption, pPage));
}

void CUIHandler::addToolbarItem_Unsafe(const std::string& sID, const std::string& sIcon, const std::string& sCaption, const std::string& sTargetPage)
{
    std::lock_guard<std::mutex> lockGuard(m_Mutex);
    
    auto pPage = findPage(sTargetPage);
    m_ToolbarItems.push_back(std::make_shared<CUIToolbarItem> (sID, sIcon, sCaption, pPage));
}

PUIPage CUIHandler::findPage(const std::string& sName)
{
    auto iter = m_Pages.find(sName);

    if (iter == m_Pages.end())
        throw ELibMCInterfaceException(LIBMC_ERROR_PAGENOTFOUND);

    return iter->second;
}


void CUIHandler::writeConfigurationToJSON(CJSONWriter& writer)
{
    writer.addString(AMC_API_KEY_UI_APPNAME, m_sAppName);
    writer.addString(AMC_API_KEY_UI_COPYRIGHT, m_sCopyrightString);
    writer.addString(AMC_API_KEY_UI_MAINPAGE, m_pMainPage->getName());
    writer.addString(AMC_API_KEY_UI_LOGOUUID, m_sLogoUUID);
    writer.addDouble(AMC_API_KEY_UI_LOGOASPECTRATIO, m_dLogoAspectRatio);

}

void CUIHandler::writeStateToJSON(CJSONWriter& writer)
{
	CJSONWriterArray menuItems(writer);

	for (auto iter : m_MenuItems) {
		CJSONWriterObject menuItem(writer);
		menuItem.addString(AMC_API_KEY_UI_ID, iter->getID ());
		menuItem.addString(AMC_API_KEY_UI_ICON, iter->getIcon ());
		menuItem.addString(AMC_API_KEY_UI_CAPTION, iter->getCaption ());
		menuItem.addString(AMC_API_KEY_UI_TARGETPAGE, iter->getPage()->getName ());
		menuItems.addObject(menuItem);
	}

	writer.addArray(AMC_API_KEY_UI_MENUITEMS, menuItems);

	CJSONWriterArray toolbarItems(writer);

	for (auto iter : m_ToolbarItems) {
		CJSONWriterObject toolbarItem(writer);
		toolbarItem.addString(AMC_API_KEY_UI_ID, iter->getID());
		toolbarItem.addString(AMC_API_KEY_UI_ICON, iter->getIcon());
		toolbarItem.addString(AMC_API_KEY_UI_CAPTION, iter->getCaption());
        toolbarItem.addString(AMC_API_KEY_UI_TARGETPAGE, iter->getPage()->getName());
        toolbarItems.addObject(toolbarItem);
	}
	writer.addArray(AMC_API_KEY_UI_TOOLBARITEMS, toolbarItems);

    CJSONWriterArray pages(writer);
    for (auto iter : m_Pages) {
        CJSONWriterObject page(writer);
        page.addString(AMC_API_KEY_UI_PAGENAME, iter.second->getName());

        CJSONWriterArray modules(writer);
        iter.second->writeModulesToJSON (writer, modules);

        page.addArray(AMC_API_KEY_UI_MODULES, modules);

        pages.addObject(page);
    }
    writer.addArray(AMC_API_KEY_UI_PAGES, pages);
}

PUIPage CUIHandler::addPage_Unsafe(const std::string& sName)
{
    if (sName.empty ())
        throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPAGENAME);

    auto iIterator = m_Pages.find (sName);
    if (iIterator != m_Pages.end())
        throw ELibMCInterfaceException(LIBMC_ERROR_DUPLICATEPAGE);

    auto pPage = std::make_shared<CUIPage> (sName);
    m_Pages.insert (std::make_pair (sName, pPage));

    return pPage;
}


void CUIHandler::loadFromXML(pugi::xml_node& xmlNode, PResourcePackage pResourcePackage)
{
    if (pResourcePackage.get() == nullptr)
        throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

    m_sAppName = "";
    m_sCopyrightString = "";
    m_pCoreResourcePackage = pResourcePackage;

    m_ToolbarItems.clear();
    m_MenuItems.clear();

    auto appnameAttrib = xmlNode.attribute("appname");
    if (appnameAttrib.empty())
        throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGAPPNAME);
    m_sAppName = appnameAttrib.as_string();

    auto copyrightAttrib = xmlNode.attribute("copyright");
    if (copyrightAttrib.empty())
        throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGCOPYRIGHT);
    m_sCopyrightString = copyrightAttrib.as_string();

    auto mainpageAttrib = xmlNode.attribute("mainpage");
    if (mainpageAttrib.empty())
        throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGMAINPAGE);
    std::string sMainPage(mainpageAttrib.as_string());

    auto logoNode = xmlNode.child("logo");
    if (!logoNode.empty()) {

        auto resourceAttrib = logoNode.attribute("resource");
        auto pResourceEntry = pResourcePackage->findEntryByName(resourceAttrib.as_string(), true);
        m_sLogoUUID = pResourceEntry->getUUID ();

        auto aspectratioAttrib = logoNode.attribute("aspectratio");
        if (!aspectratioAttrib.empty()) {
            m_dLogoAspectRatio = aspectratioAttrib.as_float();
        }
        else {
            m_dLogoAspectRatio = 1.0;
        }

        if ((m_dLogoAspectRatio < AMC_UI_IMAGE_MINASPECTRATIO) || (m_dLogoAspectRatio > AMC_UI_IMAGE_MAXASPECTRATIO))
            throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDASPECTRATIO, std::to_string(m_dLogoAspectRatio));
    }

    auto pageNodes = xmlNode.children("page");
    for (pugi::xml_node pageNode : pageNodes) {

        auto pageNameAttrib = pageNode.attribute("name");
        if (pageNameAttrib.empty())
            throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGPAGENAME);
        std::string sPageName(pageNameAttrib.as_string());

        auto pPage = addPage_Unsafe(sPageName);

        auto pageChildren = pageNode.children();
        for (pugi::xml_node pageChild : pageChildren) {
            
            auto pModule = CUIModuleFactory::createModule(pageChild, m_pParameterInstances, m_pCoreResourcePackage);
            pPage->addModule(pModule);

        }

    }


    auto menuNode = xmlNode.child("menu");
    if (menuNode.empty())
        throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGMENUNODE);

    auto menuItems = menuNode.children("item");
    for (pugi::xml_node menuItem : menuItems) {
        auto idAttrib = menuItem.attribute("id");
        if (idAttrib.empty())
            throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGMENUITEMID);

        auto iconAttrib = menuItem.attribute("icon");
        if (iconAttrib.empty())
            throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGMENUITEMICON);

        auto captionAttrib = menuItem.attribute("caption");
        if (captionAttrib.empty())
            throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGMENUITEMCAPTION);

        auto targetPageAttrib = menuItem.attribute("targetpage");
        if (targetPageAttrib.empty())
            throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGTARGETPAGE);

        addMenuItem_Unsafe(idAttrib.as_string(), iconAttrib.as_string(), captionAttrib.as_string(), targetPageAttrib.as_string());
    }

    auto toolbarNode = xmlNode.child("toolbar");
    if (toolbarNode.empty())
        throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGTOOLBARNODE);

    auto toolbarItems = toolbarNode.children("item");
    for (pugi::xml_node toolbarItem : toolbarItems) {
        auto idAttrib = toolbarItem.attribute("id");
        if (idAttrib.empty())
            throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGTOOLBARITEMID);

        auto iconAttrib = toolbarItem.attribute("icon");
        if (iconAttrib.empty())
            throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGTOOLBARITEMICON);

        auto captionAttrib = toolbarItem.attribute("caption");
        if (captionAttrib.empty())
            throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGTOOLBARITEMCAPTION);

        auto targetPageAttrib = toolbarItem.attribute("targetpage");
        if (targetPageAttrib.empty())
            throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGTARGETPAGE);

        addToolbarItem_Unsafe(idAttrib.as_string(), iconAttrib.as_string(), captionAttrib.as_string(), targetPageAttrib.as_string());
    }

    m_pMainPage = findPage (sMainPage);
}

PUIModuleItem CUIHandler::findModuleItem(const std::string& sUUID)
{
    for (auto pPage : m_Pages) {
        auto pModuleItem = pPage.second->findModuleItem(sUUID);
        if (pModuleItem.get() != nullptr)
            return pModuleItem;
    }

    return nullptr;
}


PResourcePackage CUIHandler::getCoreResourcePackage()
{
    if (m_pCoreResourcePackage.get() == nullptr)
        throw ELibMCInterfaceException(LIBMC_ERROR_NOCORERESOURCEPACKAGE);

    return m_pCoreResourcePackage;

}


