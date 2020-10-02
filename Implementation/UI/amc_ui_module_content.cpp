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
#include "amc_ui_module_contentitem.hpp"
#include "amc_ui_module_contentbutton.hpp"

#include "amc_api_constants.hpp"
#include "amc_resourcepackage.hpp"

#include "libmc_interfaceexception.hpp"

using namespace AMC;

CUIModule_Content::CUIModule_Content(pugi::xml_node& xmlNode, PParameterInstances pParameterInstances, PResourcePackage pResourcePackage)
: CUIModule (getNameFromXML(xmlNode))
{

	if (pParameterInstances.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
	if (pResourcePackage.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
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
			addItem (std::make_shared <CUIModule_ContentParagraph> (textAttrib.as_string ()));
		}

		if (sChildName == "image") {
			auto resourceAttrib = childNode.attribute("resource");
			auto pResourceEntry = pResourcePackage->findEntryByName(resourceAttrib.as_string(), true);
			double dLogoAspectRatio = 1.0;
			auto aspectratioAttrib = childNode.attribute("aspectratio");
			if (!aspectratioAttrib.empty()) {
				dLogoAspectRatio = aspectratioAttrib.as_float();
			}


			addItem (std::make_shared <CUIModule_ContentImage>(pResourceEntry->getUUID (), dLogoAspectRatio));
		}

		if (sChildName == "upload") {
			auto classAttrib = childNode.attribute("class");
			auto captionAttrib = childNode.attribute("caption");
			addItem (std::make_shared <CUIModule_ContentUpload>(classAttrib.as_string(), captionAttrib.as_string()));
		}

		if (sChildName == "parameterlist") {
			auto loadingtextAttrib = childNode.attribute("loadingtext");
			auto entriesperpageAttrib = childNode.attribute("entriesperpage");
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

			auto pParameterList = std::make_shared <CUIModule_ContentParameterList>(sLoadingText, nEntriesPerPage, pParameterInstances);
			addItem (pParameterList);

			pParameterList->loadFromXML(childNode);
		}

		if (sChildName == "button") {
			auto captionAttrib = childNode.attribute("caption");
			auto targetpageAttrib = childNode.attribute("targetpage");
			m_Buttons.push_back(std::make_shared <CUIModule_ContentButton>(captionAttrib.as_string (), targetpageAttrib.as_string ()));
		}

	}


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

std::string CUIModule_Content::getTitle()
{
	return m_sTitle;
}

std::string CUIModule_Content::getSubtitle()
{
	return m_sSubtitle;
}

void CUIModule_Content::writeDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& moduleObject)
{
	moduleObject.addString(AMC_API_KEY_UI_MODULENAME, getName());
	moduleObject.addString(AMC_API_KEY_UI_MODULETYPE, getType());
	moduleObject.addString(AMC_API_KEY_UI_HEADLINE, m_sHeadLine);
	moduleObject.addString(AMC_API_KEY_UI_TITLE, m_sTitle);
	moduleObject.addString(AMC_API_KEY_UI_SUBTITLE, m_sSubtitle);

	CJSONWriterArray itemsNode(writer);
	for (auto item : m_Items) {
		CJSONWriterObject itemObject(writer);
		item->addDefinitionToJSON(writer, itemObject);
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

PUIModuleItem CUIModule_Content::findItem(const std::string& sUUID)
{
	auto iIter = m_ItemMap.find(sUUID);
	if (iIter != m_ItemMap.end())
		return iIter->second;

	return nullptr;
}

void CUIModule_Content::addItem(PUIModule_ContentItem pItem)
{
	if (pItem.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	m_Items.push_back(pItem);
	m_ItemMap.insert(std::make_pair (pItem->getUUID (), pItem));

}

