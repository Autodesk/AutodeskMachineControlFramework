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

#include "amc_api_constants.hpp"
#include "Common/common_utils.hpp"

using namespace AMC;

CUIModule_ContentButton::CUIModule_ContentButton(const std::string& sCaption, const std::string& sTargetPage, const std::string& sEvent, const std::string& sEventFormValues)
	: m_sUUID(AMCCommon::CUtils::createUUID()), m_sCaption(sCaption), m_sTargetPage(sTargetPage), m_sEvent(sEvent), m_sEventFormValueSetting(sEventFormValues)
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


CUIModule_ContentButtonGroup::CUIModule_ContentButtonGroup(CUIModule_ContentRegistry* pFormOwner)
	: CUIModule_ContentItem(AMCCommon::CUtils::createUUID()), m_pFormOwner (pFormOwner)
{
	LibMCAssertNotNull(pFormOwner);
}

CUIModule_ContentButtonGroup::~CUIModule_ContentButtonGroup()
{

}


void CUIModule_ContentButtonGroup::addDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& object)
{
	object.addString(AMC_API_KEY_UI_ITEMTYPE, "buttongroup");
	object.addString(AMC_API_KEY_UI_ITEMUUID, m_sUUID);

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



PUIModule_ContentButton CUIModule_ContentButtonGroup::addButton(const std::string& sCaption, const std::string& sTargetPage, const std::string& sEvent, const std::string& sEventFormValues)
{
	auto pButton = std::make_shared<CUIModule_ContentButton>(sCaption, sTargetPage, sEvent, sEventFormValues);
	m_Buttons.push_back(pButton);

	return pButton;
}

void CUIModule_ContentButtonGroup::configurePostLoading()
{
	for (auto pButton : m_Buttons) {

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
