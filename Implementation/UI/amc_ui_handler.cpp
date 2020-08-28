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

#include "amc_ui_handler.hpp"
#include "amc_ui_menuitem.hpp"
#include "amc_ui_toolbaritem.hpp"
#include "amc_jsonwriter.hpp"

#include "amc_api_constants.hpp"

#include "libmc_interfaceexception.hpp"

#include "PugiXML/pugixml.hpp"

using namespace AMC;

CUIHandler::CUIHandler()
{

}

CUIHandler::~CUIHandler()
{

}

std::string CUIHandler::getAppName()
{
	return m_sAppName;
}

std::string CUIHandler::getCopyrightString()
{
	return m_sCopyrightString;
}

void CUIHandler::addMenuItem(const std::string& sID, const std::string& sIcon, const std::string& sCaption, const std::string& sTargetPage)
{
	m_MenuItems.push_back(std::make_shared<CUIMenuItem> (sID, sIcon, sCaption));
}

void CUIHandler::addToolbarItem(const std::string& sID, const std::string& sIcon, const std::string& sCaption, const std::string& sTargetPage)
{
	m_ToolbarItems.push_back(std::make_shared<CUIToolbarItem> (sID, sIcon, sCaption));
}


void CUIHandler::writeToJSON(CJSONWriter& writer)
{
	writer.addString(AMC_API_KEY_UI_APPNAME, m_sAppName);
	writer.addString(AMC_API_KEY_UI_COPYRIGHT, m_sCopyrightString);
	//writer.addString(AMC_API_KEY_UI_MAINPAGE, m_);

	CJSONWriterArray menuItems(writer);

	for (auto iter : m_MenuItems) {
		CJSONWriterObject menuItem(writer);
		menuItem.addString(AMC_API_KEY_UI_ID, iter->getID ());
		menuItem.addString(AMC_API_KEY_UI_ICON, iter->getIcon ());
		menuItem.addString(AMC_API_KEY_UI_CAPTION, iter->getCaption ());
		//menuItem.addString(AMC_API_KEY_UI_TARGETPAGE, "main");
		menuItems.addObject(menuItem);
	}

	writer.addArray(AMC_API_KEY_UI_MENUITEMS, menuItems);

	CJSONWriterArray toolbarItems(writer);

	for (auto iter : m_ToolbarItems) {
		CJSONWriterObject toolbarItem(writer);
		toolbarItem.addString(AMC_API_KEY_UI_ID, iter->getID());
		toolbarItem.addString(AMC_API_KEY_UI_ICON, iter->getIcon());
		toolbarItem.addString(AMC_API_KEY_UI_CAPTION, iter->getCaption());
		//menuItem.addString(AMC_API_KEY_UI_TARGETPAGE, "main");
		toolbarItems.addObject(toolbarItem);
	}
	writer.addArray(AMC_API_KEY_UI_TOOLBARITEMS, toolbarItems);

}

PUIPage CUIHandler::addPage(const std::string& sInstanceName, const std::string& sName)    
{
    //auto pPage = std::make_shared<CUIPage> ();

    return nullptr;
}


void CUIHandler::loadFromXML(pugi::xml_node& xmlNode)
{
    m_sAppName = "";
    m_sCopyrightString = "";
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

        addMenuItem(idAttrib.as_string(), iconAttrib.as_string(), captionAttrib.as_string(), targetPageAttrib.as_string());
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

        addToolbarItem(idAttrib.as_string(), iconAttrib.as_string(), captionAttrib.as_string(), targetPageAttrib.as_string());
    }
}