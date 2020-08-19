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


#include "amc_ui_handler.hpp"
#include "API/amc_api_constants.hpp"

#include "libmc_interfaceexception.hpp"


using namespace AMC;

CUIHandler::CUIHandler()
{

}

CUIHandler::~CUIHandler()
{

}

void CUIHandler::Initialise(const std::string& sAppName, const std::string& sCopyRightString)
{
	m_sAppName = sAppName;
	m_sCopyrightString = sCopyRightString;
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

	writer.addArray(AMC_API_KEY_UI_TOOLBARITEMS, toolbarItems);

}
