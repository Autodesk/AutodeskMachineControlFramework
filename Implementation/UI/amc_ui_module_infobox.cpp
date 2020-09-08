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
#include "amc_ui_module_infobox.hpp"
#include "amc_ui_module_infoboxitem.hpp"
#include "amc_ui_module_infoboxbutton.hpp"

#include "amc_api_constants.hpp"

#include "libmc_interfaceexception.hpp"

using namespace AMC;

CUIModule_Infobox::CUIModule_Infobox(pugi::xml_node& xmlNode)	
: CUIModule (getNameFromXML(xmlNode))
{

	if (getTypeFromXML(xmlNode) != getStaticType())
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDMODULETYPE);

	auto headlineAttrib = xmlNode.attribute("headline");
	if (!headlineAttrib.empty ())
		m_sHeadLine = headlineAttrib.as_string();

	auto titleAttrib = xmlNode.attribute("title");
	if (!titleAttrib.empty())
		m_sTitle = titleAttrib.as_string();

	auto subtitleAttrib = xmlNode.attribute("subtitle");
	if (!subtitleAttrib.empty())
		m_sSubtitle = subtitleAttrib.as_string();

	auto children = xmlNode.children();
	for (auto childNode : children) {
		std::string sChildName = childNode.name();
		if (sChildName == "paragraph") {
			auto textAttrib = childNode.attribute("text");
			m_Items.push_back(std::make_shared <CUIModule_InfoboxParagraph> (textAttrib.as_string ()));
		}

		if (sChildName == "image") {
			auto uuidAttrib = childNode.attribute("uuid");
			m_Items.push_back(std::make_shared <CUIModule_InfoboxImage>(uuidAttrib.as_string()));
		}

		if (sChildName == "button") {
			auto captionAttrib = childNode.attribute("caption");
			auto targetpageAttrib = childNode.attribute("targetpage");
			m_Buttons.push_back(std::make_shared <CUIModule_InfoboxButton>(captionAttrib.as_string (), targetpageAttrib.as_string ()));
		}

	}


}


CUIModule_Infobox::~CUIModule_Infobox()
{
}



std::string CUIModule_Infobox::getStaticType()
{
	return "infobox";
}

std::string CUIModule_Infobox::getType()
{
	return getStaticType();
}

std::string CUIModule_Infobox::getHeadLine()
{
	return m_sHeadLine;
}

std::string CUIModule_Infobox::getTitle()
{
	return m_sTitle;
}

std::string CUIModule_Infobox::getSubtitle()
{
	return m_sSubtitle;
}

void CUIModule_Infobox::writeToJSON(CJSONWriter& writer, CJSONWriterObject& moduleObject)
{
	moduleObject.addString(AMC_API_KEY_UI_MODULENAME, getName());
	moduleObject.addString(AMC_API_KEY_UI_MODULETYPE, getType());
	moduleObject.addString(AMC_API_KEY_UI_HEADLINE, m_sHeadLine);
	moduleObject.addString(AMC_API_KEY_UI_TITLE, m_sTitle);
	moduleObject.addString(AMC_API_KEY_UI_SUBTITLE, m_sSubtitle);

	CJSONWriterArray itemsNode(writer);
	for (auto item : m_Items) {
		CJSONWriterObject itemObject(writer);
		item->addToJSON(writer, itemObject);
		itemsNode.addObject(itemObject);
	}
	moduleObject.addArray(AMC_API_KEY_UI_ITEMS, itemsNode);

	CJSONWriterArray buttonsNode(writer);
	for (auto button : m_Buttons) {
		CJSONWriterObject buttonObject(writer);
		button->addToJSON(writer, buttonObject);
		buttonsNode.addObject(buttonObject);
	}
	moduleObject.addArray(AMC_API_KEY_UI_BUTTONS, buttonsNode);

}

