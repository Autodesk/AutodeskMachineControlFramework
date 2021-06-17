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

#include "amc_ui_module_contentitem_paragraph.hpp"
#include "amc_ui_module_contentitem_image.hpp"
#include "amc_ui_module_contentitem_buttongroup.hpp"
#include "amc_ui_module_contentitem_buildlist.hpp"
#include "amc_ui_module_contentitem_parameterlist.hpp"
#include "amc_ui_module_contentitem_upload.hpp"
#include "amc_ui_module_contentitem_layerview.hpp"
#include "amc_ui_module_contentitem_form.hpp"

#include "amc_api_constants.hpp"
#include "amc_resourcepackage.hpp"

#include "libmc_exceptiontypes.hpp"

using namespace AMC;

CUIModule_Content::CUIModule_Content(pugi::xml_node& xmlNode, PUIModuleEnvironment pUIModuleEnvironment)
: CUIModule (getNameFromXML(xmlNode))
{
	LibMCAssertNotNull(pUIModuleEnvironment.get());

	if (getTypeFromXML(xmlNode) != getStaticType())
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDMODULETYPE, "should be " + getStaticType ());

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

	auto children = xmlNode.children();
	for (auto childNode : children) {
		std::string sChildName = childNode.name();
		if (sChildName == "paragraph") {
			auto textAttrib = childNode.attribute("text");
			addItem (std::make_shared <CUIModule_ContentParagraph> (textAttrib.as_string ()));
		}

		if (sChildName == "image") {
			auto resourceAttrib = childNode.attribute("resource");
			auto pResourceEntry = pUIModuleEnvironment->resourcePackage()->findEntryByName(resourceAttrib.as_string(), true);
			double dLogoAspectRatio = 1.0;
			auto aspectratioAttrib = childNode.attribute("aspectratio");
			if (!aspectratioAttrib.empty()) {
				dLogoAspectRatio = aspectratioAttrib.as_double();
			}

			auto pItem = std::make_shared <CUIModule_ContentImage>(pResourceEntry->getUUID(), dLogoAspectRatio);
			addItem (pItem);

			auto maxWidthAttrib = childNode.attribute("maxwidth");
			if (!maxWidthAttrib.empty())
				pItem->setMaxWidth (maxWidthAttrib.as_double ());
			auto maxHeightAttrib = childNode.attribute("maxheight");
			if (!maxHeightAttrib.empty())
				pItem->setMaxHeight(maxHeightAttrib.as_double());

		}

		if (sChildName == "upload") {
			auto classAttrib = childNode.attribute("class");
			auto captionAttrib = childNode.attribute("caption");
			auto successpageAttrib = childNode.attribute("successpage");
			addItem (std::make_shared <CUIModule_ContentUpload>(classAttrib.as_string(), captionAttrib.as_string(), successpageAttrib.as_string ()));
		}

		if (sChildName == "layerview") {
			addItem(std::make_shared <CUIModule_ContentLayerView>());
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

			auto pParameterList = std::make_shared <CUIModule_ContentParameterList>(sLoadingText, nEntriesPerPage, pUIModuleEnvironment->parameterInstances());
			addItem (pParameterList);

			pParameterList->loadFromXML(childNode);
		}

		if (sChildName == "buttongroup") {

			auto pButtonGroup = std::make_shared <CUIModule_ContentButtonGroup>(pUIModuleEnvironment->formRegistry());
			addItem(pButtonGroup);

			auto buttonsNodes = childNode.children("button");
			for (auto buttonNode : buttonsNodes) {
				auto captionAttrib = buttonNode.attribute("caption");
				auto targetpageAttrib = buttonNode.attribute("targetpage");
				auto eventAttrib = buttonNode.attribute("event");
				auto formvaluesAttrib = buttonNode.attribute("formvalues");
				auto pButton = pButtonGroup->addButton(captionAttrib.as_string(), targetpageAttrib.as_string(), eventAttrib.as_string(), formvaluesAttrib.as_string ());
			}

		}


		if (sChildName == "form") {

			auto formNameAttrib = childNode.attribute("name");
			if (formNameAttrib.empty ())
				throw ELibMCCustomException(LIBMC_ERROR_FORMNAMEMISSING, getName());

			auto pForm = std::make_shared <CUIModule_ContentForm>(formNameAttrib.as_string ());
			addItem(pForm);

			pUIModuleEnvironment->formRegistry()->registerFormName(pForm->getUUID(), pForm->getName ());

			auto formNodes = childNode.children();
			for (auto formNode : formNodes) {

				std::string sNodeName = formNode.name();
				auto captionAttrib = formNode.attribute("caption");
				auto nameAttrib = formNode.attribute("name");
				auto valueAttrib = formNode.attribute("value");

				if (nameAttrib.empty ())
					throw ELibMCCustomException(LIBMC_ERROR_FORMENTITYNAMEMISSING, pForm->getName ());

				PUIModule_ContentFormEntity pEntity;

				if (sNodeName == "edit") {
					pEntity = pForm->addEdit(nameAttrib.as_string(),  captionAttrib.as_string(), valueAttrib.as_string ());
				}

				if (sNodeName == "switch") {
					pEntity = pForm->addSwitch(nameAttrib.as_string(), captionAttrib.as_string(), valueAttrib.as_string());
				}

				if (sNodeName == "memo") {
					pEntity = pForm->addMemo(nameAttrib.as_string(), captionAttrib.as_string(), valueAttrib.as_string());
				}

				if (sNodeName == "combobox") {
					pEntity = pForm->addCombobox(nameAttrib.as_string(), captionAttrib.as_string(), valueAttrib.as_string());
				}

				auto disabledAttrib = formNode.attribute("disabled");
				if (!disabledAttrib.empty())
					pEntity->setDisabled(disabledAttrib.as_bool());

				auto readonlyAttrib = formNode.attribute("readonly");
				if (!readonlyAttrib.empty())
					pEntity->setReadOnly(readonlyAttrib.as_bool());


			}

		}


		if (sChildName == "buildlist") {
			auto loadingtextAttrib = childNode.attribute("loadingtext");
			auto entriesperpageAttrib = childNode.attribute("entriesperpage");
			auto detailpageAttrib = childNode.attribute("detailpage");
			std::string sLoadingText = loadingtextAttrib.as_string();
			std::string sDetailPage = detailpageAttrib.as_string();

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

			auto pBuildList = std::make_shared <CUIModule_ContentBuildList>(sLoadingText, nEntriesPerPage, sDetailPage, pUIModuleEnvironment->buildJobHandler());
			addItem(pBuildList);
			
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

void CUIModule_Content::writeDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& moduleObject)
{
	moduleObject.addString(AMC_API_KEY_UI_MODULENAME, getName());
	moduleObject.addString(AMC_API_KEY_UI_MODULETYPE, getType());
	moduleObject.addString(AMC_API_KEY_UI_MODULEUUID, getUUID());
	moduleObject.addString(AMC_API_KEY_UI_HEADLINE, m_sHeadLine);
	moduleObject.addString(AMC_API_KEY_UI_TITLE, m_sTitle);
	moduleObject.addString(AMC_API_KEY_UI_SUBTITLE, m_sSubtitle);
	moduleObject.addString(AMC_API_KEY_UI_CAPTION, m_sCaption);

	CJSONWriterArray itemsNode(writer);
	for (auto item : m_Items) {
		CJSONWriterObject itemObject(writer);
		item->addDefinitionToJSON(writer, itemObject);
		itemsNode.addObject(itemObject);
	}
	moduleObject.addArray(AMC_API_KEY_UI_ITEMS, itemsNode);

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
	LibMCAssertNotNull(pItem.get());

	m_Items.push_back(pItem);

	auto referenceList = pItem->getReferenceUUIDs();
	for (auto sUUID : referenceList)
		m_ItemMap.insert(std::make_pair (sUUID, pItem));

}


void CUIModule_Content::populateItemMap(std::map<std::string, PUIModuleItem>& itemMap)
{
	for (auto item : m_Items) {
		auto referenceList = item->getReferenceUUIDs();
		for (auto sUUID : referenceList)
			itemMap.insert(std::make_pair(sUUID, item));

	}
}

void CUIModule_Content::configurePostLoading()
{
	for (auto item : m_Items)
		item->configurePostLoading();
}

