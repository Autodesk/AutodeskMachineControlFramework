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

#include "amc_ui_module_contentitem_buttongroup.hpp"
#include "amc_ui_module_contentitem_form.hpp"
#include "libmc_interfaceexception.hpp"
#include "libmc_exceptiontypes.hpp"
#include "amc_ui_module.hpp"
#include "amc_api_constants.hpp"
#include "Common/common_utils.hpp"

using namespace AMC;

CUIModule_ContentButton::CUIModule_ContentButton(const std::string& sCaption, const std::string& sTargetPage, const std::string& sEvent, const std::string& sButtonName, const std::string& sEventFormValueSetting)
	: m_sUUID(AMCCommon::CUtils::createUUID()), m_sCaption(sCaption), m_sTargetPage(sTargetPage), m_sEvent(sEvent), m_sEventFormValueSetting(sEventFormValueSetting), m_sButtonName (sButtonName)
{
}

CUIModule_ContentButton::~CUIModule_ContentButton()
{

}

std::string CUIModule_ContentButton::getUUID()
{
	return m_sUUID;
}

std::string CUIModule_ContentButton::getCaption()
{
	return m_sCaption;
}

std::string CUIModule_ContentButton::getTargetPage()
{
	return m_sTargetPage;
}

std::string CUIModule_ContentButton::getEvent()
{
	return m_sEvent;
}

std::string CUIModule_ContentButton::getEventFormValueSetting()
{
	return m_sEventFormValueSetting;

}

std::string CUIModule_ContentButton::getButtonName()
{
	return m_sButtonName;
}


void CUIModule_ContentButton::addFormFieldValue(PUIModule_ContentFormEntity pEntity)
{
	LibMCAssertNotNull(pEntity.get());

	m_pFormValues.push_back(pEntity);
}

void CUIModule_ContentButton::writeFormValuesToJSON(CJSONWriterArray& pArray)
{
	for (auto pFormValue : m_pFormValues) {
		pArray.addString(pFormValue->getUUID());
	}
}

PUIModule_ContentButtonGroup CUIModule_ContentButtonGroup::makeFromXML(const pugi::xml_node& xmlNode, const std::string& sItemName, const std::string& sModulePath, PUIModuleEnvironment pUIModuleEnvironment)
{
	LibMCAssertNotNull(pUIModuleEnvironment);

	eUIModule_ContentButtonDistribution buttonDistribution = eUIModule_ContentButtonDistribution::cbdRightAligned;
	auto distributionAttrib = xmlNode.attribute ("distribution");
	if (!distributionAttrib.empty()) {
		buttonDistribution = stringToButtonDistribution(distributionAttrib.as_string ());
	}	

	auto pButtonGroup = std::make_shared <CUIModule_ContentButtonGroup>(pUIModuleEnvironment->contentRegistry(), buttonDistribution, sItemName, sModulePath);

	uint32_t nButtonIndex = 0;
	std::set<std::string> buttonNameMap;

	auto buttonsNodes = xmlNode.children("button");
	for (auto buttonNode : buttonsNodes) {

		auto captionAttrib = buttonNode.attribute("caption");
		auto targetpageAttrib = buttonNode.attribute("targetpage");
		auto eventAttrib = buttonNode.attribute("event");
		auto formvaluesAttrib = buttonNode.attribute("formvalues");
		auto buttonNameAttrib = buttonNode.attribute("name");
		std::string sButtonName = buttonNameAttrib.as_string ();
		if (sButtonName.empty ())
		{
			while (true) {
				nButtonIndex++;
				sButtonName = "button" + std::to_string(nButtonIndex);

				auto iIter = buttonNameMap.find(sButtonName);
				if (iIter == buttonNameMap.end())
					break;
			}
		}

		auto iIter = buttonNameMap.find(sButtonName);
		if (iIter != buttonNameMap.end())
			throw ELibMCCustomException(LIBMC_ERROR_DUPLICATEBUTTONNAME, pButtonGroup->getItemPath() + "." + sButtonName);

		buttonNameMap.insert(sButtonName);

		auto pButton = pButtonGroup->addButton(captionAttrib.as_string(), targetpageAttrib.as_string(), eventAttrib.as_string(), sButtonName, formvaluesAttrib.as_string());
	}

	return pButtonGroup;
}


CUIModule_ContentButtonGroup::CUIModule_ContentButtonGroup(CUIModule_ContentRegistry* pFormOwner, const eUIModule_ContentButtonDistribution buttonDistribution, const std::string& sItemName, const std::string& sModulePath)
	: CUIModule_ContentItem(AMCCommon::CUtils::createUUID(), sItemName, sModulePath), m_pFormOwner (pFormOwner), m_ButtonDistribution (buttonDistribution)
{
	LibMCAssertNotNull(pFormOwner);
}

CUIModule_ContentButtonGroup::~CUIModule_ContentButtonGroup()
{

}


void CUIModule_ContentButtonGroup::addDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler)
{
	object.addString(AMC_API_KEY_UI_ITEMTYPE, "buttongroup");
	object.addString(AMC_API_KEY_UI_ITEMUUID, m_sUUID);
	object.addString(AMC_API_KEY_UI_BUTTONDISTRIBUTION, buttonDistributionToString (m_ButtonDistribution));

	CJSONWriterArray buttonArray(writer);

	for (auto pButton : m_Buttons) {
		CJSONWriterObject buttonobject(writer);
		buttonobject.addString(AMC_API_KEY_UI_BUTTONUUID, pButton->getUUID ());
		buttonobject.addString(AMC_API_KEY_UI_BUTTONCAPTION, pButton->getCaption ());
		buttonobject.addString(AMC_API_KEY_UI_BUTTONTARGETPAGE, pButton->getTargetPage ());
		buttonobject.addString(AMC_API_KEY_UI_BUTTONEVENT, pButton->getEvent());

		CJSONWriterArray buttonEventFormValues (writer);
		pButton->writeFormValuesToJSON(buttonEventFormValues);
		buttonobject.addArray(AMC_API_KEY_UI_BUTTONEVENTFORMVALUES, buttonEventFormValues);

		buttonArray.addObject(buttonobject);
	}


	object.addArray(AMC_API_KEY_UI_ITEMBUTTONS, buttonArray);

}



PUIModule_ContentButton CUIModule_ContentButtonGroup::addButton(const std::string& sCaption, const std::string& sTargetPage, const std::string& sEvent, const std::string& sButtonName, const std::string& sEventFormValues)
{
	auto pButton = std::make_shared<CUIModule_ContentButton>(sCaption, sTargetPage, sEvent, sButtonName, sEventFormValues);
	m_Buttons.push_back(pButton);
	m_ButtonMap.insert(std::make_pair (pButton->getUUID (), pButton));

	return pButton;
}

void CUIModule_ContentButtonGroup::configurePostLoading()
{
	for (auto pButton : m_Buttons) {

		auto sEvent = pButton->getEvent();
		if (!sEvent.empty()) {
			m_pFormOwner->ensureUIEventExists (sEvent);
		}

		auto sFormValuesSetting = pButton->getEventFormValueSetting();
		if (!sFormValuesSetting.empty()) {
			std::vector <std::string> formValueList;
			AMCCommon::CUtils::splitString(sFormValuesSetting, " ", formValueList);

			for (auto sFormSetting : formValueList) {
				if (!sFormSetting.empty()) {

					std::string sFormName, sFieldName;

					auto nPointPosition = sFormSetting.find(".");
					if (nPointPosition != std::string::npos) {
						sFormName = sFormSetting.substr (0, nPointPosition);
						sFieldName = sFormSetting.substr(nPointPosition + 1);
					}
					else {
						sFormName = sFormSetting;
					}

					auto sFormUUID = m_pFormOwner->findFormUUIDByName(sFormName);
					if (sFormUUID.empty())
						throw ELibMCCustomException(LIBMC_ERROR_FORMNOTFOUND, sFormName);

					auto pFormItem = m_pFormOwner->findModuleItemByUUID(sFormUUID);
					auto pForm = std::dynamic_pointer_cast<CUIModule_ContentForm> (pFormItem);
					if (pForm.get () == nullptr)
						throw ELibMCCustomException(LIBMC_ERROR_FORMNOTFOUND, sFormName);

					if (sFieldName.empty()) {
						auto formEntities = pForm->getEntities();
						for (auto pEntity : formEntities) {
							pButton->addFormFieldValue(pEntity);
						}

					}
					else {
						auto pEntity = pForm->findEntityByName(sFieldName);
						if (pEntity == nullptr)
							throw ELibMCCustomException(LIBMC_ERROR_FORMENTITYNOTFOUND, sFieldName);

						pButton->addFormFieldValue (pEntity);
					}
				}
			}


		}
		 
	}

}


// Returns all UUIDs that could be contained in this Item
std::list <std::string> CUIModule_ContentButtonGroup::getReferenceUUIDs()
{
	std::list <std::string> resultList;
	resultList.push_back(getUUID());
	for (auto pButton : m_Buttons) {
		resultList.push_back(pButton->getUUID());
	}

	return resultList;
}


eUIModule_ContentButtonDistribution CUIModule_ContentButtonGroup::getButtonDistribution()
{
	return m_ButtonDistribution;
}

void CUIModule_ContentButtonGroup::setButtonDistribution(const eUIModule_ContentButtonDistribution buttonDistribution)
{
	m_ButtonDistribution = buttonDistribution;
}

eUIModule_ContentButtonDistribution CUIModule_ContentButtonGroup::stringToButtonDistribution(const std::string& sValue)
{
	if (sValue == "rightaligned")
		return eUIModule_ContentButtonDistribution::cbdRightAligned;
	if (sValue == "leftaligned")
		return eUIModule_ContentButtonDistribution::cbdLeftAligned;
	if (sValue == "centered")
		return eUIModule_ContentButtonDistribution::cbdCentered;
	if (sValue == "equal")
		return eUIModule_ContentButtonDistribution::cbdEquallyDistributed;

	throw ELibMCCustomException(LIBMC_ERROR_INVALIDBUTTONDISTRIBUTION, "invalid button distribution: " + sValue);
}


std::string CUIModule_ContentButtonGroup::buttonDistributionToString(const eUIModule_ContentButtonDistribution buttonDistribution)
{
	switch (buttonDistribution)
	{

		case eUIModule_ContentButtonDistribution::cbdRightAligned:
			return "rightaligned";
		case eUIModule_ContentButtonDistribution::cbdLeftAligned:
			return "leftaligned";
		case eUIModule_ContentButtonDistribution::cbdCentered:
			return "centered";
		case eUIModule_ContentButtonDistribution::cbdEquallyDistributed:
			return "equal";

		default:
			return "";

	}
}

std::string CUIModule_ContentButtonGroup::findElementPathByUUID(const std::string& sUUID)
{
	if (sUUID == getUUID())
		return getItemPath();

	auto iIter = m_ButtonMap.find(sUUID);
	if (iIter != m_ButtonMap.end())
		return getItemPath() + "." + iIter->second->getButtonName();

	return "";
}


