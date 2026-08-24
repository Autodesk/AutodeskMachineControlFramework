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

#include "amc_ui_module_contentitem_form.hpp"
#include "amc_ui_utils.hpp"
#include "amc_statemachinedata.hpp"
#include "amc_parameterhandler.hpp"
#include "amc_ui_module.hpp"


#include "libmc_exceptiontypes.hpp"

#include "amc_api_constants.hpp"
#include "Common/common_utils.hpp"

#include <cstdlib>

using namespace AMC;


CUIModule_ContentFormEntity::CUIModule_ContentFormEntity(const std::string& sName, const std::string& sFormPath, CUIExpression Caption, PStateMachineData pStateMachineData)
	: m_sUUID(AMCCommon::CUtils::createUUID()),
	m_sName(sName),
	m_CaptionExpression(Caption),		
	m_pStateMachineData(pStateMachineData)	
{
	LibMCAssertNotNull(pStateMachineData.get());
	if (sFormPath.empty ())
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDFORMPATH);
	if (sName.empty ())
		throw ELibMCInterfaceException(LIBMC_ERROR_FORMENTITYNAMEMISSING);

	m_sElementPath = sFormPath + "." + sName;

	m_CaptionExpression.checkExpressionSyntax(m_pStateMachineData);


}

CUIModule_ContentFormEntity::~CUIModule_ContentFormEntity()
{

}

std::string CUIModule_ContentFormEntity::getName()
{
	return m_sName;
}


std::string CUIModule_ContentFormEntity::getUUID()
{
	return m_sUUID;
}

void CUIModule_ContentFormEntity::setDisabledExpression(CUIExpression Expression)
{
	Expression.checkExpressionSyntax(m_pStateMachineData);
	m_DisabledExpression = Expression;
}


void CUIModule_ContentFormEntity::setReadOnlyExpression(CUIExpression Expression)
{
	Expression.checkExpressionSyntax(m_pStateMachineData);
	m_ReadOnlyExpression = Expression;
}



void CUIModule_ContentFormEntity::addContentToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler)
{
	object.addString(AMC_API_KEY_UI_FORMUUID, getUUID());
	object.addString(AMC_API_KEY_UI_FORMTYPE, getTypeString());

	auto pGroup = getClientVariableGroup(pClientVariableHandler);

	if (m_CaptionExpression.needsSync())
		pGroup->setParameterValueByName(AMC_API_KEY_UI_FORMCAPTION, m_CaptionExpression.evaluateStringValue(m_pStateMachineData));
	if (m_DisabledExpression.needsSync())
		pGroup->setBoolParameterValueByName(AMC_API_KEY_UI_FORMDISABLED, m_DisabledExpression.evaluateBoolValue(m_pStateMachineData));
	if (m_ReadOnlyExpression.needsSync())
		pGroup->setBoolParameterValueByName(AMC_API_KEY_UI_FORMREADONLY, m_ReadOnlyExpression.evaluateBoolValue(m_pStateMachineData));

	object.addString(AMC_API_KEY_UI_FORMCAPTION, pGroup->getParameterValueByName(AMC_API_KEY_UI_FORMCAPTION));
	object.addBool(AMC_API_KEY_UI_FORMDISABLED, pGroup->getBoolParameterValueByName(AMC_API_KEY_UI_FORMDISABLED));
	object.addBool(AMC_API_KEY_UI_FORMREADONLY, pGroup->getBoolParameterValueByName(AMC_API_KEY_UI_FORMREADONLY));

	this->syncClientVariables(pClientVariableHandler);
	this->writeVariablesToJSON(writer, object, pClientVariableHandler);
}

std::string CUIModule_ContentFormEntity::getElementPath()
{
	return m_sElementPath;
}


PParameterGroup CUIModule_ContentFormEntity::registerClientVariableGroup(CParameterHandler* pClientVariableHandler)
{
	LibMCAssertNotNull(pClientVariableHandler);
	auto pGroup = pClientVariableHandler->addGroup(m_sElementPath, "form entity " + getTypeString());
	pGroup->addNewStringParameter(AMC_API_KEY_UI_FORMCAPTION, "edit caption", m_CaptionExpression.evaluateStringValue(m_pStateMachineData));
	pGroup->addNewBoolParameter(AMC_API_KEY_UI_FORMDISABLED, "entity is disabled", m_DisabledExpression.evaluateBoolValue (m_pStateMachineData));
	pGroup->addNewBoolParameter(AMC_API_KEY_UI_FORMREADONLY, "entity is readonly", m_ReadOnlyExpression.evaluateBoolValue (m_pStateMachineData));

	return pGroup;
}

PParameterGroup CUIModule_ContentFormEntity::getClientVariableGroup(CParameterHandler* pClientVariableHandler)
{
	LibMCAssertNotNull(pClientVariableHandler);
	return pClientVariableHandler->findGroup(m_sElementPath, true);
}


void CUIModule_ContentFormEntity::registerFrontendAttributes(PUIFrontendDefinitionModuleStore pStore)
{
	LibMCAssertNotNull(pStore.get());
	m_pFrontendStore = pStore;

	pStore->registerValue("caption", eUIFrontendDefinitionAttributeType::atString, m_CaptionExpression);
	pStore->registerValue("disabled", eUIFrontendDefinitionAttributeType::atBoolean, m_DisabledExpression);
	pStore->registerValue("readonly", eUIFrontendDefinitionAttributeType::atBoolean, m_ReadOnlyExpression);
}


PUIModule_ContentFormEdit CUIModule_ContentFormEdit::makeFromXML(const pugi::xml_node& xmlNode, const std::string& sFormPath, PStateMachineData pStateMachineData)
{
	auto nameAttrib = xmlNode.attribute("name");
	auto changeeventAttrib = xmlNode.attribute("changeevent");


	if (nameAttrib.empty())
		throw ELibMCInterfaceException(LIBMC_ERROR_FORMENTITYNAMEMISSING);
	CUIExpression caption(xmlNode, "caption");
	CUIExpression value(xmlNode, "value");
	CUIExpression prefix(xmlNode, "prefix");
	CUIExpression suffix(xmlNode, "suffix");

	CUIExpression validation(xmlNode, "validation");
	CUIExpression validationmessage(xmlNode, "validationmessage");
	CUIExpression minvalue(xmlNode, "minvalue");
	CUIExpression maxvalue(xmlNode, "maxvalue");

	std::string sOnChangeEvent = changeeventAttrib.as_string();

	auto pEdit = std::make_shared<CUIModule_ContentFormEdit>(nameAttrib.as_string(), sFormPath, caption, value, prefix, suffix, sOnChangeEvent, pStateMachineData);

	pEdit->setValidationExpressions(validation, validationmessage, minvalue, maxvalue);

	return pEdit;

}

CUIModule_ContentFormEdit::CUIModule_ContentFormEdit(const std::string& sName, const std::string& sFormPath, CUIExpression Caption, CUIExpression Value, CUIExpression Prefix, CUIExpression Suffix, std::string sOnChangeEvent, PStateMachineData pStateMachineData)
	: CUIModule_ContentFormEntity (sName, sFormPath, Caption, pStateMachineData), m_PrefixExpression (Prefix), m_SuffixExpression (Suffix), m_ValueExpression (Value), m_sOnChangeEvent(sOnChangeEvent)
{
	m_PrefixExpression.checkExpressionSyntax(m_pStateMachineData);
	m_SuffixExpression.checkExpressionSyntax(m_pStateMachineData);
	m_ValueExpression.checkExpressionSyntax(m_pStateMachineData);
}

void CUIModule_ContentFormEdit::setValidationExpressions(CUIExpression validationExpression, CUIExpression validationMessageExpression, CUIExpression minValueExpression, CUIExpression maxValueExpression)
{
	m_ValidationExpression = validationExpression;
	m_ValidationMessageExpression = validationMessageExpression;
	m_MinValueExpression = minValueExpression;
	m_MaxValueExpression = maxValueExpression;

	m_ValidationExpression.checkExpressionSyntax (m_pStateMachineData);
	m_ValidationMessageExpression.checkExpressionSyntax(m_pStateMachineData);
	m_MinValueExpression.checkExpressionSyntax (m_pStateMachineData);
	m_MaxValueExpression.checkExpressionSyntax (m_pStateMachineData);
}


CUIModule_ContentFormEdit::~CUIModule_ContentFormEdit()
{

}

std::string CUIModule_ContentFormEdit::getTypeString()
{
	return "edit";
}

void CUIModule_ContentFormEdit::populateClientVariables(CParameterHandler* pClientVariableHandler)
{
	auto pGroup = registerClientVariableGroup(pClientVariableHandler);
	pGroup->addNewStringParameter(AMC_API_KEY_UI_FORMDEFAULTVALUE, "edit value", m_ValueExpression.evaluateStringValue (m_pStateMachineData));
	pGroup->addNewStringParameter(AMC_API_KEY_UI_FORMPREFIX, "edit value prefix", m_PrefixExpression.evaluateStringValue(m_pStateMachineData));
	pGroup->addNewStringParameter(AMC_API_KEY_UI_FORMSUFFIX, "edit value suffix", m_SuffixExpression.evaluateStringValue(m_pStateMachineData));

	pGroup->addNewStringParameter(AMC_API_KEY_UI_FORMVALIDATION, "edit form validation", m_ValidationExpression.evaluateStringValue(m_pStateMachineData));
	pGroup->addNewStringParameter(AMC_API_KEY_UI_FORMVALIDATIONMESSAGE, "edit form validation message", m_ValidationMessageExpression.evaluateStringValue(m_pStateMachineData));
	pGroup->addNewStringParameter(AMC_API_KEY_UI_FORMMINVALUE, "edit form min value", m_MinValueExpression.evaluateStringValue(m_pStateMachineData));
	pGroup->addNewStringParameter(AMC_API_KEY_UI_FORMMAXVALUE, "edit form max value", m_MaxValueExpression.evaluateStringValue(m_pStateMachineData));

}

void CUIModule_ContentFormEdit::syncClientVariables(CParameterHandler* pClientVariableHandler)
{
	auto pGroup = getClientVariableGroup(pClientVariableHandler);
	if (m_CaptionExpression.needsSync())
		pGroup->setParameterValueByName(AMC_API_KEY_UI_FORMCAPTION, m_CaptionExpression.evaluateStringValue(m_pStateMachineData));
	if (m_ValueExpression.needsSync())
		pGroup->setParameterValueByName(AMC_API_KEY_UI_FORMDEFAULTVALUE, m_ValueExpression.evaluateStringValue(m_pStateMachineData));
	if (m_PrefixExpression.needsSync())
		pGroup->setParameterValueByName(AMC_API_KEY_UI_FORMPREFIX, m_PrefixExpression.evaluateStringValue(m_pStateMachineData));
	if (m_ValidationExpression.needsSync())
		pGroup->setParameterValueByName(AMC_API_KEY_UI_FORMVALIDATION, m_ValidationExpression.evaluateStringValue(m_pStateMachineData));
	if (m_ValidationMessageExpression.needsSync())
		pGroup->setParameterValueByName(AMC_API_KEY_UI_FORMVALIDATIONMESSAGE, m_ValidationMessageExpression.evaluateStringValue(m_pStateMachineData));
	if (m_MinValueExpression.needsSync())
		pGroup->setParameterValueByName(AMC_API_KEY_UI_FORMMINVALUE, m_MinValueExpression.evaluateStringValue(m_pStateMachineData));
	if (m_MaxValueExpression.needsSync())
		pGroup->setParameterValueByName(AMC_API_KEY_UI_FORMMAXVALUE, m_MaxValueExpression.evaluateStringValue(m_pStateMachineData));

}

void CUIModule_ContentFormEdit::writeVariablesToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler)
{
	auto pGroup = getClientVariableGroup(pClientVariableHandler);

	object.addString(AMC_API_KEY_UI_FORMDEFAULTVALUE, pGroup->getParameterValueByName(AMC_API_KEY_UI_FORMDEFAULTVALUE));
	object.addString(AMC_API_KEY_UI_FORMPREFIX, pGroup->getParameterValueByName(AMC_API_KEY_UI_FORMPREFIX));
	object.addString(AMC_API_KEY_UI_FORMSUFFIX, pGroup->getParameterValueByName(AMC_API_KEY_UI_FORMSUFFIX));
	std::string sValidation = pGroup->getParameterValueByName(AMC_API_KEY_UI_FORMVALIDATION);

	if (!m_sOnChangeEvent.empty())
		object.addString(AMC_API_KEY_UI_FORMCHANGEEVENT, m_sOnChangeEvent);

	if (sValidation == "double") {
		object.addString(AMC_API_KEY_UI_FORMVALIDATION, sValidation);
		object.addString(AMC_API_KEY_UI_FORMVALIDATIONMESSAGE, pGroup->getParameterValueByName(AMC_API_KEY_UI_FORMVALIDATIONMESSAGE));
		object.addDouble(AMC_API_KEY_UI_FORMMINVALUE, pGroup->getDoubleParameterValueByName(AMC_API_KEY_UI_FORMMINVALUE));
		object.addDouble(AMC_API_KEY_UI_FORMMAXVALUE, pGroup->getDoubleParameterValueByName(AMC_API_KEY_UI_FORMMAXVALUE));
	}
	if (sValidation == "string") {
		object.addString(AMC_API_KEY_UI_FORMVALIDATION, sValidation);
		object.addString(AMC_API_KEY_UI_FORMVALIDATIONMESSAGE, pGroup->getParameterValueByName(AMC_API_KEY_UI_FORMVALIDATIONMESSAGE));
		object.addInteger(AMC_API_KEY_UI_FORMMINLENGTH, pGroup->getIntParameterValueByName(AMC_API_KEY_UI_FORMMINVALUE));
		object.addInteger(AMC_API_KEY_UI_FORMMAXLENGTH, pGroup->getIntParameterValueByName(AMC_API_KEY_UI_FORMMAXVALUE));
	}
}

void CUIModule_ContentFormEdit::registerFrontendAttributes(PUIFrontendDefinitionModuleStore pStore)
{
	CUIModule_ContentFormEntity::registerFrontendAttributes(pStore);

	pStore->registerValue("value", eUIFrontendDefinitionAttributeType::atString, m_ValueExpression);
	pStore->registerValue("prefix", eUIFrontendDefinitionAttributeType::atString, m_PrefixExpression);
	pStore->registerValue("suffix", eUIFrontendDefinitionAttributeType::atString, m_SuffixExpression);
	pStore->registerValue("validation", eUIFrontendDefinitionAttributeType::atString, m_ValidationExpression);
	pStore->registerValue("validationmessage", eUIFrontendDefinitionAttributeType::atString, m_ValidationMessageExpression);
	pStore->registerValue("minvalue", eUIFrontendDefinitionAttributeType::atString, m_MinValueExpression);
	pStore->registerValue("maxvalue", eUIFrontendDefinitionAttributeType::atString, m_MaxValueExpression);

	CUIExpression changeEventExpr;
	changeEventExpr.setFixedValue(m_sOnChangeEvent);
	pStore->registerValue("changeevent", eUIFrontendDefinitionAttributeType::atString, changeEventExpr);
}


PUIModule_ContentFormSwitch CUIModule_ContentFormSwitch::makeFromXML(const pugi::xml_node& xmlNode, const std::string& sFormPath, PStateMachineData pStateMachineData)
{
	auto nameAttrib = xmlNode.attribute("name");
	auto changeeventAttrib = xmlNode.attribute ("changeevent");

	if (nameAttrib.empty())
		throw ELibMCInterfaceException(LIBMC_ERROR_FORMENTITYNAMEMISSING);

	CUIExpression caption(xmlNode, "caption");
	CUIExpression value(xmlNode, "value");

	std::string sOnChangeEvent = changeeventAttrib.as_string();

	return std::make_shared<CUIModule_ContentFormSwitch>(nameAttrib.as_string(), sFormPath, caption, value, sOnChangeEvent, pStateMachineData);

}

CUIModule_ContentFormSwitch::CUIModule_ContentFormSwitch(const std::string& sName, const std::string& sFormPath, CUIExpression Caption, CUIExpression Value, const std::string& sOnChangeEvent, PStateMachineData pStateMachineData)
	: CUIModule_ContentFormEntity (sName, sFormPath, Caption, pStateMachineData), m_ValueExpression (Value), m_sOnChangeEvent (sOnChangeEvent)
{

}

CUIModule_ContentFormSwitch::~CUIModule_ContentFormSwitch()
{

}

std::string CUIModule_ContentFormSwitch::getTypeString()
{
	return "switch";
}


void CUIModule_ContentFormSwitch::populateClientVariables(CParameterHandler* pClientVariableHandler)
{
	auto pGroup = registerClientVariableGroup(pClientVariableHandler);
	pGroup->addNewBoolParameter(AMC_API_KEY_UI_FORMDEFAULTVALUE, "switch value", m_ValueExpression.evaluateBoolValue(m_pStateMachineData));
}


void CUIModule_ContentFormSwitch::syncClientVariables(CParameterHandler* pClientVariableHandler)
{
	auto pGroup = getClientVariableGroup(pClientVariableHandler);
	if (m_ValueExpression.needsSync())
		pGroup->setBoolParameterValueByName(AMC_API_KEY_UI_FORMDEFAULTVALUE, m_ValueExpression.evaluateBoolValue(m_pStateMachineData));
}

void CUIModule_ContentFormSwitch::writeVariablesToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler)
{
	auto pGroup = getClientVariableGroup(pClientVariableHandler);

	object.addBool(AMC_API_KEY_UI_FORMDEFAULTVALUE, pGroup->getBoolParameterValueByName(AMC_API_KEY_UI_FORMDEFAULTVALUE));
	if (!m_sOnChangeEvent.empty ())
		object.addString(AMC_API_KEY_UI_FORMCHANGEEVENT, m_sOnChangeEvent);
}

void CUIModule_ContentFormSwitch::registerFrontendAttributes(PUIFrontendDefinitionModuleStore pStore)
{
	CUIModule_ContentFormEntity::registerFrontendAttributes(pStore);

	pStore->registerValue("value", eUIFrontendDefinitionAttributeType::atBoolean, m_ValueExpression);

	CUIExpression changeEventExpr;
	changeEventExpr.setFixedValue(m_sOnChangeEvent);
	pStore->registerValue("changeevent", eUIFrontendDefinitionAttributeType::atString, changeEventExpr);
}


PUIModule_ContentFormMemo CUIModule_ContentFormMemo::makeFromXML(const pugi::xml_node& xmlNode, const std::string& sFormPath, PStateMachineData pStateMachineData)
{
	auto nameAttrib = xmlNode.attribute("name");

	if (nameAttrib.empty())
		throw ELibMCInterfaceException(LIBMC_ERROR_FORMENTITYNAMEMISSING);
	CUIExpression caption(xmlNode, "caption");
	CUIExpression value(xmlNode, "value");

	return std::make_shared<CUIModule_ContentFormMemo>(nameAttrib.as_string(), sFormPath, caption, value, pStateMachineData);

}

CUIModule_ContentFormMemo::CUIModule_ContentFormMemo(const std::string& sName, const std::string& sFormPath, CUIExpression Caption, CUIExpression Value, PStateMachineData pStateMachineData)
	: CUIModule_ContentFormEntity (sName, sFormPath, Caption, pStateMachineData)
{

}

CUIModule_ContentFormMemo::~CUIModule_ContentFormMemo()
{

}

std::string CUIModule_ContentFormMemo::getTypeString()
{
	return "memo";
}


void CUIModule_ContentFormMemo::populateClientVariables(CParameterHandler* pClientVariableHandler)
{
	auto pGroup = registerClientVariableGroup(pClientVariableHandler);
}

void CUIModule_ContentFormMemo::syncClientVariables(CParameterHandler* pClientVariableHandler)
{
}


void CUIModule_ContentFormMemo::writeVariablesToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler)
{
	auto pGroup = getClientVariableGroup(pClientVariableHandler);
}

void CUIModule_ContentFormMemo::registerFrontendAttributes(PUIFrontendDefinitionModuleStore pStore)
{
	CUIModule_ContentFormEntity::registerFrontendAttributes(pStore);
}

PUIModule_ContentFormCombobox CUIModule_ContentFormCombobox::makeFromXML(const pugi::xml_node& xmlNode, const std::string& sFormPath, PStateMachineData pStateMachineData)
{
	auto nameAttrib = xmlNode.attribute("name");
	auto changeeventAttrib = xmlNode.attribute("changeevent");

	if (nameAttrib.empty())
		throw ELibMCInterfaceException(LIBMC_ERROR_FORMENTITYNAMEMISSING);
	CUIExpression caption(xmlNode, "caption");
	CUIExpression value(xmlNode, "value");

	// Determine value type: "string" enables string-valued comboboxes, otherwise legacy integer behaviour.
	std::string sValueType = xmlNode.attribute("valuetype").as_string();
	bool bStringValue = (sValueType == "string");

	// Prepare vector to hold items (display text, value as string)
	std::vector<std::pair<std::string, std::string>> items;

	// If "itemsource" is given, populate the items dynamically by enumerating the referenced
	// parameter group ("instance.group"). Each parameter becomes one item, using the parameter
	// name as the (string) value and its description (falling back to the name) as the display text.
	std::string sItemSource = xmlNode.attribute("itemsource").as_string();
	if (!sItemSource.empty()) {
		std::string sInstance, sGroup, sDummyName;
		CStateMachineData::extractParameterDetailsFromDotString(sItemSource, sInstance, sGroup, sDummyName, true, true);

		auto pParameterHandler = pStateMachineData->getParameterHandler(sInstance);
		auto pGroup = pParameterHandler->findGroup(sGroup, true);
		uint32_t nParameterCount = pGroup->getParameterCount();
		for (uint32_t nIndex = 0; nIndex < nParameterCount; nIndex++) {
			std::string sParamName, sParamDescription, sParamDefault;
			pGroup->getParameterInfo(nIndex, sParamName, sParamDescription, sParamDefault);
			std::string sText = sParamDescription.empty() ? sParamName : sParamDescription;
			items.emplace_back(sText, sParamName);
		}

		// A parameter-group item source always yields string values (parameter names).
		bStringValue = true;
	}
	else {
		// Iterate over all statically declared child nodes named "item"
		for (auto itemNode : xmlNode.children("item")) {
			auto textAttrib = itemNode.attribute("text");
			auto valueAttrib = itemNode.attribute("value");
			std::string text = textAttrib.as_string();
			std::string valueStr = valueAttrib.as_string();
			items.emplace_back(text, valueStr);
		}
	}

	return std::make_shared<CUIModule_ContentFormCombobox>(nameAttrib.as_string(), sFormPath, caption, value, changeeventAttrib.as_string(), pStateMachineData, items, bStringValue);
}

CUIModule_ContentFormCombobox::CUIModule_ContentFormCombobox(const std::string& sName, const std::string& sFormPath, CUIExpression Caption, CUIExpression Value, const std::string& sOnChangeEvent, PStateMachineData pStateMachineData, const std::vector<std::pair<std::string, std::string>>& items, bool bStringValue)
	: CUIModule_ContentFormEntity(sName, sFormPath, Caption, pStateMachineData), m_ValueExpression(Value), m_sOnChangeEvent(sOnChangeEvent), m_Items(items), m_bStringValue(bStringValue)
{

}


CUIModule_ContentFormCombobox::~CUIModule_ContentFormCombobox()
{

}

std::string CUIModule_ContentFormCombobox::getTypeString()
{
	return "combobox";
}

void CUIModule_ContentFormCombobox::populateClientVariables(CParameterHandler* pClientVariableHandler)
{
	auto pGroup = registerClientVariableGroup(pClientVariableHandler);
	pGroup->addNewStringParameter(AMC_API_KEY_UI_FORMDEFAULTVALUE, "combo value", m_ValueExpression.evaluateStringValue(m_pStateMachineData));
}

void CUIModule_ContentFormCombobox::syncClientVariables(CParameterHandler* pClientVariableHandler)
{
	auto pGroup = getClientVariableGroup(pClientVariableHandler);
	if (m_ValueExpression.needsSync()) {
		if (m_bStringValue)
			pGroup->setParameterValueByName(AMC_API_KEY_UI_FORMDEFAULTVALUE, m_ValueExpression.evaluateStringValue(m_pStateMachineData));
		else
			pGroup->setIntParameterValueByName(AMC_API_KEY_UI_FORMDEFAULTVALUE, m_ValueExpression.evaluateIntegerValue(m_pStateMachineData));
	}
}

void CUIModule_ContentFormCombobox::writeVariablesToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler)
{
	auto pGroup = getClientVariableGroup(pClientVariableHandler);

	object.addString("type", "combobox");
	if (m_bStringValue)
		object.addString(AMC_API_KEY_UI_FORMDEFAULTVALUE, pGroup->getParameterValueByName(AMC_API_KEY_UI_FORMDEFAULTVALUE));
	else
		object.addInteger(AMC_API_KEY_UI_FORMDEFAULTVALUE, pGroup->getIntParameterValueByName(AMC_API_KEY_UI_FORMDEFAULTVALUE));
	object.addString("changeevent", m_sOnChangeEvent);

	// Create an array for the items
	CJSONWriterArray itemsArray(writer);
	for (const auto& item : m_Items) {
		CJSONWriterObject itemObject(writer);
		itemObject.addString("text", item.first);
		if (m_bStringValue)
			itemObject.addString("value", item.second);
		else
			itemObject.addInteger("value", (int64_t)std::atoll(item.second.c_str()));
		itemsArray.addObject(itemObject);
	}
	object.addArray("items", itemsArray);
}

void CUIModule_ContentFormCombobox::registerFrontendAttributes(PUIFrontendDefinitionModuleStore pStore)
{
	CUIModule_ContentFormEntity::registerFrontendAttributes(pStore);

	pStore->registerValue("value", m_bStringValue ? eUIFrontendDefinitionAttributeType::atString : eUIFrontendDefinitionAttributeType::atInteger, m_ValueExpression);

	CUIExpression changeEventExpr;
	changeEventExpr.setFixedValue(m_sOnChangeEvent);
	pStore->registerValue("changeevent", eUIFrontendDefinitionAttributeType::atString, changeEventExpr);

	uint32_t nItemIndex = 0;
	for (const auto& item : m_Items) {
		std::string sItemUUID = AMCCommon::CUtils::createUUID();
		auto pItemStore = pStore->addChildStore(sItemUUID, m_sElementPath + ".item" + std::to_string(nItemIndex), "comboboxitem");
		nItemIndex++;

		CUIExpression textExpr;
		textExpr.setFixedValue(item.first);
		pItemStore->registerValue("text", eUIFrontendDefinitionAttributeType::atString, textExpr);

		CUIExpression valueExpr;
		valueExpr.setFixedValue(item.second);
		pItemStore->registerValue("value", m_bStringValue ? eUIFrontendDefinitionAttributeType::atString : eUIFrontendDefinitionAttributeType::atInteger, valueExpr);
	}
}

// ── Phase 2: number ──────────────────────────────────────────────────────────

PUIModule_ContentFormNumber CUIModule_ContentFormNumber::makeFromXML(const pugi::xml_node& xmlNode, const std::string& sFormPath, PStateMachineData pStateMachineData)
{
	auto nameAttrib = xmlNode.attribute("name");
	if (nameAttrib.empty())
		throw ELibMCInterfaceException(LIBMC_ERROR_FORMENTITYNAMEMISSING);

	CUIExpression caption(xmlNode, "caption");
	CUIExpression value(xmlNode, "value");
	CUIExpression min(xmlNode, "min");
	CUIExpression max(xmlNode, "max");
	CUIExpression step(xmlNode, "step");
	CUIExpression unit(xmlNode, "unit");
	std::string sOnChangeEvent = xmlNode.attribute("changeevent").as_string();

	return std::make_shared<CUIModule_ContentFormNumber>(nameAttrib.as_string(), sFormPath, caption, value, min, max, step, unit, sOnChangeEvent, pStateMachineData);
}

CUIModule_ContentFormNumber::CUIModule_ContentFormNumber(const std::string& sName, const std::string& sFormPath, CUIExpression Caption, CUIExpression Value, CUIExpression Min, CUIExpression Max, CUIExpression Step, CUIExpression Unit, const std::string& sOnChangeEvent, PStateMachineData pStateMachineData)
	: CUIModule_ContentFormEntity(sName, sFormPath, Caption, pStateMachineData),
	  m_ValueExpression(Value), m_MinValueExpression(Min), m_MaxValueExpression(Max),
	  m_StepExpression(Step), m_UnitExpression(Unit), m_sOnChangeEvent(sOnChangeEvent)
{
	m_ValueExpression.checkExpressionSyntax(m_pStateMachineData);
	m_MinValueExpression.checkExpressionSyntax(m_pStateMachineData);
	m_MaxValueExpression.checkExpressionSyntax(m_pStateMachineData);
	m_StepExpression.checkExpressionSyntax(m_pStateMachineData);
	m_UnitExpression.checkExpressionSyntax(m_pStateMachineData);
}

CUIModule_ContentFormNumber::~CUIModule_ContentFormNumber() {}

std::string CUIModule_ContentFormNumber::getTypeString() { return "number"; }

void CUIModule_ContentFormNumber::populateClientVariables(CParameterHandler* pClientVariableHandler)
{
	auto pGroup = registerClientVariableGroup(pClientVariableHandler);
	pGroup->addNewStringParameter(AMC_API_KEY_UI_FORMDEFAULTVALUE, "number value", m_ValueExpression.evaluateStringValue(m_pStateMachineData));
	pGroup->addNewStringParameter(AMC_API_KEY_UI_FORMMINVALUE, "number min", m_MinValueExpression.evaluateStringValue(m_pStateMachineData));
	pGroup->addNewStringParameter(AMC_API_KEY_UI_FORMMAXVALUE, "number max", m_MaxValueExpression.evaluateStringValue(m_pStateMachineData));
	pGroup->addNewStringParameter(AMC_API_KEY_UI_FORMSTEP, "number step", m_StepExpression.evaluateStringValue(m_pStateMachineData));
	pGroup->addNewStringParameter(AMC_API_KEY_UI_FORMUNIT, "number unit", m_UnitExpression.evaluateStringValue(m_pStateMachineData));
}

void CUIModule_ContentFormNumber::syncClientVariables(CParameterHandler* pClientVariableHandler)
{
	auto pGroup = getClientVariableGroup(pClientVariableHandler);
	if (m_ValueExpression.needsSync())
		pGroup->setParameterValueByName(AMC_API_KEY_UI_FORMDEFAULTVALUE, m_ValueExpression.evaluateStringValue(m_pStateMachineData));
	if (m_MinValueExpression.needsSync())
		pGroup->setParameterValueByName(AMC_API_KEY_UI_FORMMINVALUE, m_MinValueExpression.evaluateStringValue(m_pStateMachineData));
	if (m_MaxValueExpression.needsSync())
		pGroup->setParameterValueByName(AMC_API_KEY_UI_FORMMAXVALUE, m_MaxValueExpression.evaluateStringValue(m_pStateMachineData));
	if (m_StepExpression.needsSync())
		pGroup->setParameterValueByName(AMC_API_KEY_UI_FORMSTEP, m_StepExpression.evaluateStringValue(m_pStateMachineData));
	if (m_UnitExpression.needsSync())
		pGroup->setParameterValueByName(AMC_API_KEY_UI_FORMUNIT, m_UnitExpression.evaluateStringValue(m_pStateMachineData));
}

void CUIModule_ContentFormNumber::writeVariablesToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler)
{
	auto pGroup = getClientVariableGroup(pClientVariableHandler);
	object.addString(AMC_API_KEY_UI_FORMDEFAULTVALUE, pGroup->getParameterValueByName(AMC_API_KEY_UI_FORMDEFAULTVALUE));
	object.addString(AMC_API_KEY_UI_FORMMINVALUE, pGroup->getParameterValueByName(AMC_API_KEY_UI_FORMMINVALUE));
	object.addString(AMC_API_KEY_UI_FORMMAXVALUE, pGroup->getParameterValueByName(AMC_API_KEY_UI_FORMMAXVALUE));
	object.addString(AMC_API_KEY_UI_FORMSTEP, pGroup->getParameterValueByName(AMC_API_KEY_UI_FORMSTEP));
	object.addString(AMC_API_KEY_UI_FORMUNIT, pGroup->getParameterValueByName(AMC_API_KEY_UI_FORMUNIT));
	if (!m_sOnChangeEvent.empty())
		object.addString(AMC_API_KEY_UI_FORMCHANGEEVENT, m_sOnChangeEvent);
}

void CUIModule_ContentFormNumber::registerFrontendAttributes(PUIFrontendDefinitionModuleStore pStore)
{
	CUIModule_ContentFormEntity::registerFrontendAttributes(pStore);
	pStore->registerValue("value", eUIFrontendDefinitionAttributeType::atString, m_ValueExpression);
	pStore->registerValue("minvalue", eUIFrontendDefinitionAttributeType::atString, m_MinValueExpression);
	pStore->registerValue("maxvalue", eUIFrontendDefinitionAttributeType::atString, m_MaxValueExpression);
	pStore->registerValue("step", eUIFrontendDefinitionAttributeType::atString, m_StepExpression);
	pStore->registerValue("unit", eUIFrontendDefinitionAttributeType::atString, m_UnitExpression);
	CUIExpression changeEventExpr;
	changeEventExpr.setFixedValue(m_sOnChangeEvent);
	pStore->registerValue("changeevent", eUIFrontendDefinitionAttributeType::atString, changeEventExpr);
}


// ── Phase 2: slider ───────────────────────────────────────────────────────────

PUIModule_ContentFormSlider CUIModule_ContentFormSlider::makeFromXML(const pugi::xml_node& xmlNode, const std::string& sFormPath, PStateMachineData pStateMachineData)
{
	auto nameAttrib = xmlNode.attribute("name");
	if (nameAttrib.empty())
		throw ELibMCInterfaceException(LIBMC_ERROR_FORMENTITYNAMEMISSING);

	CUIExpression caption(xmlNode, "caption");
	CUIExpression value(xmlNode, "value");
	CUIExpression min(xmlNode, "min");
	CUIExpression max(xmlNode, "max");
	CUIExpression step(xmlNode, "step");
	CUIExpression unit(xmlNode, "unit");
	std::string sOnChangeEvent = xmlNode.attribute("changeevent").as_string();

	return std::make_shared<CUIModule_ContentFormSlider>(nameAttrib.as_string(), sFormPath, caption, value, min, max, step, unit, sOnChangeEvent, pStateMachineData);
}

CUIModule_ContentFormSlider::CUIModule_ContentFormSlider(const std::string& sName, const std::string& sFormPath, CUIExpression Caption, CUIExpression Value, CUIExpression Min, CUIExpression Max, CUIExpression Step, CUIExpression Unit, const std::string& sOnChangeEvent, PStateMachineData pStateMachineData)
	: CUIModule_ContentFormEntity(sName, sFormPath, Caption, pStateMachineData),
	  m_ValueExpression(Value), m_MinValueExpression(Min), m_MaxValueExpression(Max),
	  m_StepExpression(Step), m_UnitExpression(Unit), m_sOnChangeEvent(sOnChangeEvent)
{
	m_ValueExpression.checkExpressionSyntax(m_pStateMachineData);
	m_MinValueExpression.checkExpressionSyntax(m_pStateMachineData);
	m_MaxValueExpression.checkExpressionSyntax(m_pStateMachineData);
	m_StepExpression.checkExpressionSyntax(m_pStateMachineData);
	m_UnitExpression.checkExpressionSyntax(m_pStateMachineData);
}

CUIModule_ContentFormSlider::~CUIModule_ContentFormSlider() {}

std::string CUIModule_ContentFormSlider::getTypeString() { return "slider"; }

void CUIModule_ContentFormSlider::populateClientVariables(CParameterHandler* pClientVariableHandler)
{
	auto pGroup = registerClientVariableGroup(pClientVariableHandler);
	pGroup->addNewStringParameter(AMC_API_KEY_UI_FORMDEFAULTVALUE, "slider value", m_ValueExpression.evaluateStringValue(m_pStateMachineData));
	pGroup->addNewStringParameter(AMC_API_KEY_UI_FORMMINVALUE, "slider min", m_MinValueExpression.evaluateStringValue(m_pStateMachineData));
	pGroup->addNewStringParameter(AMC_API_KEY_UI_FORMMAXVALUE, "slider max", m_MaxValueExpression.evaluateStringValue(m_pStateMachineData));
	pGroup->addNewStringParameter(AMC_API_KEY_UI_FORMSTEP, "slider step", m_StepExpression.evaluateStringValue(m_pStateMachineData));
	pGroup->addNewStringParameter(AMC_API_KEY_UI_FORMUNIT, "slider unit", m_UnitExpression.evaluateStringValue(m_pStateMachineData));
}

void CUIModule_ContentFormSlider::syncClientVariables(CParameterHandler* pClientVariableHandler)
{
	auto pGroup = getClientVariableGroup(pClientVariableHandler);
	if (m_ValueExpression.needsSync())
		pGroup->setParameterValueByName(AMC_API_KEY_UI_FORMDEFAULTVALUE, m_ValueExpression.evaluateStringValue(m_pStateMachineData));
	if (m_MinValueExpression.needsSync())
		pGroup->setParameterValueByName(AMC_API_KEY_UI_FORMMINVALUE, m_MinValueExpression.evaluateStringValue(m_pStateMachineData));
	if (m_MaxValueExpression.needsSync())
		pGroup->setParameterValueByName(AMC_API_KEY_UI_FORMMAXVALUE, m_MaxValueExpression.evaluateStringValue(m_pStateMachineData));
	if (m_StepExpression.needsSync())
		pGroup->setParameterValueByName(AMC_API_KEY_UI_FORMSTEP, m_StepExpression.evaluateStringValue(m_pStateMachineData));
	if (m_UnitExpression.needsSync())
		pGroup->setParameterValueByName(AMC_API_KEY_UI_FORMUNIT, m_UnitExpression.evaluateStringValue(m_pStateMachineData));
}

void CUIModule_ContentFormSlider::writeVariablesToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler)
{
	auto pGroup = getClientVariableGroup(pClientVariableHandler);
	object.addString(AMC_API_KEY_UI_FORMDEFAULTVALUE, pGroup->getParameterValueByName(AMC_API_KEY_UI_FORMDEFAULTVALUE));
	object.addString(AMC_API_KEY_UI_FORMMINVALUE, pGroup->getParameterValueByName(AMC_API_KEY_UI_FORMMINVALUE));
	object.addString(AMC_API_KEY_UI_FORMMAXVALUE, pGroup->getParameterValueByName(AMC_API_KEY_UI_FORMMAXVALUE));
	object.addString(AMC_API_KEY_UI_FORMSTEP, pGroup->getParameterValueByName(AMC_API_KEY_UI_FORMSTEP));
	object.addString(AMC_API_KEY_UI_FORMUNIT, pGroup->getParameterValueByName(AMC_API_KEY_UI_FORMUNIT));
	if (!m_sOnChangeEvent.empty())
		object.addString(AMC_API_KEY_UI_FORMCHANGEEVENT, m_sOnChangeEvent);
}

void CUIModule_ContentFormSlider::registerFrontendAttributes(PUIFrontendDefinitionModuleStore pStore)
{
	CUIModule_ContentFormEntity::registerFrontendAttributes(pStore);
	pStore->registerValue("value", eUIFrontendDefinitionAttributeType::atString, m_ValueExpression);
	pStore->registerValue("minvalue", eUIFrontendDefinitionAttributeType::atString, m_MinValueExpression);
	pStore->registerValue("maxvalue", eUIFrontendDefinitionAttributeType::atString, m_MaxValueExpression);
	pStore->registerValue("step", eUIFrontendDefinitionAttributeType::atString, m_StepExpression);
	pStore->registerValue("unit", eUIFrontendDefinitionAttributeType::atString, m_UnitExpression);
	CUIExpression changeEventExpr;
	changeEventExpr.setFixedValue(m_sOnChangeEvent);
	pStore->registerValue("changeevent", eUIFrontendDefinitionAttributeType::atString, changeEventExpr);
}


// ── Phase 2: datetime ─────────────────────────────────────────────────────────

PUIModule_ContentFormDatetime CUIModule_ContentFormDatetime::makeFromXML(const pugi::xml_node& xmlNode, const std::string& sFormPath, PStateMachineData pStateMachineData)
{
	auto nameAttrib = xmlNode.attribute("name");
	if (nameAttrib.empty())
		throw ELibMCInterfaceException(LIBMC_ERROR_FORMENTITYNAMEMISSING);

	CUIExpression caption(xmlNode, "caption");
	CUIExpression value(xmlNode, "value");
	std::string sMode = xmlNode.attribute("mode").as_string("datetime");
	std::string sOnChangeEvent = xmlNode.attribute("changeevent").as_string();

	return std::make_shared<CUIModule_ContentFormDatetime>(nameAttrib.as_string(), sFormPath, caption, value, sMode, sOnChangeEvent, pStateMachineData);
}

CUIModule_ContentFormDatetime::CUIModule_ContentFormDatetime(const std::string& sName, const std::string& sFormPath, CUIExpression Caption, CUIExpression Value, const std::string& sMode, const std::string& sOnChangeEvent, PStateMachineData pStateMachineData)
	: CUIModule_ContentFormEntity(sName, sFormPath, Caption, pStateMachineData),
	  m_ValueExpression(Value), m_sMode(sMode), m_sOnChangeEvent(sOnChangeEvent)
{
	m_ValueExpression.checkExpressionSyntax(m_pStateMachineData);
}

CUIModule_ContentFormDatetime::~CUIModule_ContentFormDatetime() {}

std::string CUIModule_ContentFormDatetime::getTypeString() { return "datetime"; }

void CUIModule_ContentFormDatetime::populateClientVariables(CParameterHandler* pClientVariableHandler)
{
	auto pGroup = registerClientVariableGroup(pClientVariableHandler);
	pGroup->addNewStringParameter(AMC_API_KEY_UI_FORMDEFAULTVALUE, "datetime value", m_ValueExpression.evaluateStringValue(m_pStateMachineData));
}

void CUIModule_ContentFormDatetime::syncClientVariables(CParameterHandler* pClientVariableHandler)
{
	auto pGroup = getClientVariableGroup(pClientVariableHandler);
	if (m_ValueExpression.needsSync())
		pGroup->setParameterValueByName(AMC_API_KEY_UI_FORMDEFAULTVALUE, m_ValueExpression.evaluateStringValue(m_pStateMachineData));
}

void CUIModule_ContentFormDatetime::writeVariablesToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler)
{
	auto pGroup = getClientVariableGroup(pClientVariableHandler);
	object.addString(AMC_API_KEY_UI_FORMDEFAULTVALUE, pGroup->getParameterValueByName(AMC_API_KEY_UI_FORMDEFAULTVALUE));
	object.addString(AMC_API_KEY_UI_FORMDATETIMEMODE, m_sMode);
	if (!m_sOnChangeEvent.empty())
		object.addString(AMC_API_KEY_UI_FORMCHANGEEVENT, m_sOnChangeEvent);
}

void CUIModule_ContentFormDatetime::registerFrontendAttributes(PUIFrontendDefinitionModuleStore pStore)
{
	CUIModule_ContentFormEntity::registerFrontendAttributes(pStore);
	pStore->registerValue("value", eUIFrontendDefinitionAttributeType::atString, m_ValueExpression);
	CUIExpression modeExpr;
	modeExpr.setFixedValue(m_sMode);
	pStore->registerValue("mode", eUIFrontendDefinitionAttributeType::atString, modeExpr);
	CUIExpression changeEventExpr;
	changeEventExpr.setFixedValue(m_sOnChangeEvent);
	pStore->registerValue("changeevent", eUIFrontendDefinitionAttributeType::atString, changeEventExpr);
}


// ── Phase 2: multiselect ──────────────────────────────────────────────────────

PUIModule_ContentFormMultiselect CUIModule_ContentFormMultiselect::makeFromXML(const pugi::xml_node& xmlNode, const std::string& sFormPath, PStateMachineData pStateMachineData)
{
	auto nameAttrib = xmlNode.attribute("name");
	if (nameAttrib.empty())
		throw ELibMCInterfaceException(LIBMC_ERROR_FORMENTITYNAMEMISSING);

	CUIExpression caption(xmlNode, "caption");
	CUIExpression value(xmlNode, "value");
	std::string sOnChangeEvent = xmlNode.attribute("changeevent").as_string();

	std::vector<std::pair<std::string, int>> items;
	for (auto itemNode : xmlNode.children("item")) {
		std::string text = itemNode.attribute("text").as_string();
		int intValue = itemNode.attribute("value").as_int();
		items.emplace_back(text, intValue);
	}

	return std::make_shared<CUIModule_ContentFormMultiselect>(nameAttrib.as_string(), sFormPath, caption, value, sOnChangeEvent, pStateMachineData, items);
}

CUIModule_ContentFormMultiselect::CUIModule_ContentFormMultiselect(const std::string& sName, const std::string& sFormPath, CUIExpression Caption, CUIExpression Value, const std::string& sOnChangeEvent, PStateMachineData pStateMachineData, const std::vector<std::pair<std::string, int>>& items)
	: CUIModule_ContentFormEntity(sName, sFormPath, Caption, pStateMachineData),
	  m_ValueExpression(Value), m_sOnChangeEvent(sOnChangeEvent), m_Items(items)
{
	m_ValueExpression.checkExpressionSyntax(m_pStateMachineData);
}

CUIModule_ContentFormMultiselect::~CUIModule_ContentFormMultiselect() {}

std::string CUIModule_ContentFormMultiselect::getTypeString() { return "multiselect"; }

void CUIModule_ContentFormMultiselect::populateClientVariables(CParameterHandler* pClientVariableHandler)
{
	auto pGroup = registerClientVariableGroup(pClientVariableHandler);
	pGroup->addNewStringParameter(AMC_API_KEY_UI_FORMDEFAULTVALUE, "multiselect value", m_ValueExpression.evaluateStringValue(m_pStateMachineData));
}

void CUIModule_ContentFormMultiselect::syncClientVariables(CParameterHandler* pClientVariableHandler)
{
	auto pGroup = getClientVariableGroup(pClientVariableHandler);
	if (m_ValueExpression.needsSync())
		pGroup->setParameterValueByName(AMC_API_KEY_UI_FORMDEFAULTVALUE, m_ValueExpression.evaluateStringValue(m_pStateMachineData));
}

void CUIModule_ContentFormMultiselect::writeVariablesToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler)
{
	auto pGroup = getClientVariableGroup(pClientVariableHandler);
	object.addString(AMC_API_KEY_UI_FORMDEFAULTVALUE, pGroup->getParameterValueByName(AMC_API_KEY_UI_FORMDEFAULTVALUE));
	if (!m_sOnChangeEvent.empty())
		object.addString(AMC_API_KEY_UI_FORMCHANGEEVENT, m_sOnChangeEvent);
	CJSONWriterArray itemsArray(writer);
	for (const auto& item : m_Items) {
		CJSONWriterObject itemObject(writer);
		itemObject.addString("text", item.first);
		itemObject.addInteger("value", item.second);
		itemsArray.addObject(itemObject);
	}
	object.addArray(AMC_API_KEY_UI_FORMITEMS, itemsArray);
}

void CUIModule_ContentFormMultiselect::registerFrontendAttributes(PUIFrontendDefinitionModuleStore pStore)
{
	CUIModule_ContentFormEntity::registerFrontendAttributes(pStore);
	pStore->registerValue("value", eUIFrontendDefinitionAttributeType::atString, m_ValueExpression);
	CUIExpression changeEventExpr;
	changeEventExpr.setFixedValue(m_sOnChangeEvent);
	pStore->registerValue("changeevent", eUIFrontendDefinitionAttributeType::atString, changeEventExpr);

	for (const auto& item : m_Items) {
		std::string sItemUUID = AMCCommon::CUtils::createUUID();
		auto pItemStore = pStore->addChildStore(sItemUUID, m_sElementPath + ".item" + std::to_string(item.second), "multiselectitem");
		CUIExpression textExpr;
		textExpr.setFixedValue(item.first);
		pItemStore->registerValue("text", eUIFrontendDefinitionAttributeType::atString, textExpr);
		CUIExpression valueExpr;
		valueExpr.setFixedValue(std::to_string(item.second));
		pItemStore->registerValue("value", eUIFrontendDefinitionAttributeType::atInteger, valueExpr);
	}
}


// ── Phase 2: calculated ───────────────────────────────────────────────────────

PUIModule_ContentFormCalculated CUIModule_ContentFormCalculated::makeFromXML(const pugi::xml_node& xmlNode, const std::string& sFormPath, PStateMachineData pStateMachineData)
{
	auto nameAttrib = xmlNode.attribute("name");
	if (nameAttrib.empty())
		throw ELibMCInterfaceException(LIBMC_ERROR_FORMENTITYNAMEMISSING);

	CUIExpression caption(xmlNode, "caption");
	CUIExpression value(xmlNode, "value");
	CUIExpression unit(xmlNode, "unit");
	CUIExpression format(xmlNode, "format");

	return std::make_shared<CUIModule_ContentFormCalculated>(nameAttrib.as_string(), sFormPath, caption, value, unit, format, pStateMachineData);
}

CUIModule_ContentFormCalculated::CUIModule_ContentFormCalculated(const std::string& sName, const std::string& sFormPath, CUIExpression Caption, CUIExpression Value, CUIExpression Unit, CUIExpression Format, PStateMachineData pStateMachineData)
	: CUIModule_ContentFormEntity(sName, sFormPath, Caption, pStateMachineData),
	  m_ValueExpression(Value), m_UnitExpression(Unit), m_FormatExpression(Format)
{
	m_ValueExpression.checkExpressionSyntax(m_pStateMachineData);
	m_UnitExpression.checkExpressionSyntax(m_pStateMachineData);
	m_FormatExpression.checkExpressionSyntax(m_pStateMachineData);
}

CUIModule_ContentFormCalculated::~CUIModule_ContentFormCalculated() {}

std::string CUIModule_ContentFormCalculated::getTypeString() { return "calculated"; }

void CUIModule_ContentFormCalculated::populateClientVariables(CParameterHandler* pClientVariableHandler)
{
	auto pGroup = registerClientVariableGroup(pClientVariableHandler);
	pGroup->addNewStringParameter(AMC_API_KEY_UI_FORMDEFAULTVALUE, "calculated value", m_ValueExpression.evaluateStringValue(m_pStateMachineData));
	pGroup->addNewStringParameter(AMC_API_KEY_UI_FORMUNIT, "calculated unit", m_UnitExpression.evaluateStringValue(m_pStateMachineData));
	pGroup->addNewStringParameter(AMC_API_KEY_UI_FORMFORMAT, "calculated format", m_FormatExpression.evaluateStringValue(m_pStateMachineData));
}

void CUIModule_ContentFormCalculated::syncClientVariables(CParameterHandler* pClientVariableHandler)
{
	auto pGroup = getClientVariableGroup(pClientVariableHandler);
	if (m_ValueExpression.needsSync())
		pGroup->setParameterValueByName(AMC_API_KEY_UI_FORMDEFAULTVALUE, m_ValueExpression.evaluateStringValue(m_pStateMachineData));
	if (m_UnitExpression.needsSync())
		pGroup->setParameterValueByName(AMC_API_KEY_UI_FORMUNIT, m_UnitExpression.evaluateStringValue(m_pStateMachineData));
	if (m_FormatExpression.needsSync())
		pGroup->setParameterValueByName(AMC_API_KEY_UI_FORMFORMAT, m_FormatExpression.evaluateStringValue(m_pStateMachineData));
}

void CUIModule_ContentFormCalculated::writeVariablesToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler)
{
	auto pGroup = getClientVariableGroup(pClientVariableHandler);
	object.addString(AMC_API_KEY_UI_FORMDEFAULTVALUE, pGroup->getParameterValueByName(AMC_API_KEY_UI_FORMDEFAULTVALUE));
	object.addString(AMC_API_KEY_UI_FORMUNIT, pGroup->getParameterValueByName(AMC_API_KEY_UI_FORMUNIT));
	object.addString(AMC_API_KEY_UI_FORMFORMAT, pGroup->getParameterValueByName(AMC_API_KEY_UI_FORMFORMAT));
	object.addBool(AMC_API_KEY_UI_FORMREADONLY, true);
}

void CUIModule_ContentFormCalculated::registerFrontendAttributes(PUIFrontendDefinitionModuleStore pStore)
{
	CUIModule_ContentFormEntity::registerFrontendAttributes(pStore);
	pStore->registerValue("value", eUIFrontendDefinitionAttributeType::atString, m_ValueExpression);
	pStore->registerValue("unit", eUIFrontendDefinitionAttributeType::atString, m_UnitExpression);
	pStore->registerValue("format", eUIFrontendDefinitionAttributeType::atString, m_FormatExpression);
}


// ── Phase 2: section ──────────────────────────────────────────────────────────

PUIModule_ContentFormSection CUIModule_ContentFormSection::makeFromXML(const pugi::xml_node& xmlNode, const std::string& sFormPath, PStateMachineData pStateMachineData)
{
	auto nameAttrib = xmlNode.attribute("name");
	if (nameAttrib.empty())
		throw ELibMCInterfaceException(LIBMC_ERROR_FORMENTITYNAMEMISSING);

	CUIExpression caption(xmlNode, "caption");
	CUIExpression description(xmlNode, "description");
	CUIExpression icon(xmlNode, "icon");
	bool bCollapsible = xmlNode.attribute("collapsible").as_bool(true);
	bool bDefaultOpen = xmlNode.attribute("defaultopen").as_bool(true);

	auto pSection = std::make_shared<CUIModule_ContentFormSection>(nameAttrib.as_string(), sFormPath, caption, description, icon, bCollapsible, bDefaultOpen, pStateMachineData);

	CUIModule_ContentForm::parseXmlFormNode(xmlNode, sFormPath, pStateMachineData, [&pSection](PUIModule_ContentFormEntity pEntity) {
		pSection->addEntity(pEntity);
	});

	return pSection;
}

CUIModule_ContentFormSection::CUIModule_ContentFormSection(const std::string& sName, const std::string& sFormPath, CUIExpression Caption, CUIExpression Description, CUIExpression Icon, bool bCollapsible, bool bDefaultOpen, PStateMachineData pStateMachineData)
	: CUIModule_ContentFormEntity(sName, sFormPath, Caption, pStateMachineData),
	  m_DescriptionExpression(Description), m_IconExpression(Icon),
	  m_bCollapsible(bCollapsible), m_bDefaultOpen(bDefaultOpen)
{
	m_DescriptionExpression.checkExpressionSyntax(m_pStateMachineData);
	m_IconExpression.checkExpressionSyntax(m_pStateMachineData);
}

CUIModule_ContentFormSection::~CUIModule_ContentFormSection() {}

std::string CUIModule_ContentFormSection::getTypeString() { return "section"; }

void CUIModule_ContentFormSection::addEntity(PUIModule_ContentFormEntity pEntity)
{
	LibMCAssertNotNull(pEntity.get());
	m_EntityNameMap.insert(std::make_pair(pEntity->getName(), pEntity));
	m_EntityUUIDMap.insert(std::make_pair(pEntity->getUUID(), pEntity));
	m_Entities.push_back(pEntity);
}

bool CUIModule_ContentFormSection::hasEntityWithName(const std::string& sName)
{
	return m_EntityNameMap.find(sName) != m_EntityNameMap.end();
}

PUIModule_ContentFormEntity CUIModule_ContentFormSection::findEntityByUUID(const std::string& sUUID)
{
	auto iIter = m_EntityUUIDMap.find(sUUID);
	if (iIter != m_EntityUUIDMap.end())
		return iIter->second;
	return nullptr;
}

std::list<PUIModule_ContentFormEntity>& CUIModule_ContentFormSection::getEntities()
{
	return m_Entities;
}

void CUIModule_ContentFormSection::populateClientVariables(CParameterHandler* pClientVariableHandler)
{
	registerClientVariableGroup(pClientVariableHandler);
	for (auto pEntity : m_Entities)
		pEntity->populateClientVariables(pClientVariableHandler);
}

void CUIModule_ContentFormSection::syncClientVariables(CParameterHandler* pClientVariableHandler)
{
	for (auto pEntity : m_Entities)
		pEntity->syncClientVariables(pClientVariableHandler);
}

void CUIModule_ContentFormSection::writeVariablesToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler)
{
	object.addString(AMC_API_KEY_UI_FORMSECTIONDESC, m_DescriptionExpression.evaluateStringValue(m_pStateMachineData));
	object.addString(AMC_API_KEY_UI_FORMSECTIONICON, m_IconExpression.evaluateStringValue(m_pStateMachineData));
	object.addBool(AMC_API_KEY_UI_FORMCOLLAPSIBLE, m_bCollapsible);
	object.addBool(AMC_API_KEY_UI_FORMDEFAULTOPEN, m_bDefaultOpen);

	CJSONWriterArray entityArray(writer);
	for (auto pEntity : m_Entities) {
		CJSONWriterObject entityObject(writer);
		pEntity->addContentToJSON(writer, entityObject, pClientVariableHandler);
		entityArray.addObject(entityObject);
	}
	object.addArray(AMC_API_KEY_UI_FORMENTITIES, entityArray);
}

void CUIModule_ContentFormSection::registerFrontendAttributes(PUIFrontendDefinitionModuleStore pStore)
{
	CUIModule_ContentFormEntity::registerFrontendAttributes(pStore);
	pStore->registerValue("description", eUIFrontendDefinitionAttributeType::atString, m_DescriptionExpression);
	pStore->registerValue("icon", eUIFrontendDefinitionAttributeType::atString, m_IconExpression);
	CUIExpression collapsibleExpr;
	collapsibleExpr.setFixedValue(m_bCollapsible ? "1" : "0");
	pStore->registerValue("collapsible", eUIFrontendDefinitionAttributeType::atBoolean, collapsibleExpr);
	CUIExpression defaultOpenExpr;
	defaultOpenExpr.setFixedValue(m_bDefaultOpen ? "1" : "0");
	pStore->registerValue("defaultopen", eUIFrontendDefinitionAttributeType::atBoolean, defaultOpenExpr);

	for (auto& pEntity : m_Entities) {
		auto pChildStore = pStore->addChildStore(pEntity->getUUID(), pEntity->getElementPath(), pEntity->getTypeString());
		pEntity->registerFrontendAttributes(pChildStore);
	}
}


// ── Phase 2: actionbar ────────────────────────────────────────────────────────

PUIModule_ContentFormActionBar CUIModule_ContentFormActionBar::makeFromXML(const pugi::xml_node& xmlNode, const std::string& sFormPath, PStateMachineData pStateMachineData)
{
	auto nameAttrib = xmlNode.attribute("name");
	std::string sName = nameAttrib.empty() ? "_actionbar" : nameAttrib.as_string();

	std::string sSubmitEvent = xmlNode.attribute("submitevent").as_string();
	std::string sCancelEvent = xmlNode.attribute("cancelevent").as_string();
	std::string sValidateEvent = xmlNode.attribute("validateevent").as_string();
	std::string sSubmitCaption = xmlNode.attribute("submitcaption").as_string("Save");
	std::string sCancelCaption = xmlNode.attribute("cancelcaption").as_string("Cancel");
	bool bOptimistic = xmlNode.attribute("optimistic").as_bool(false);

	CUIExpression captionExpr;
	captionExpr.setFixedValue("");

	return std::make_shared<CUIModule_ContentFormActionBar>(sName, sFormPath, sSubmitEvent, sCancelEvent, sValidateEvent, sSubmitCaption, sCancelCaption, bOptimistic, pStateMachineData);
}

CUIModule_ContentFormActionBar::CUIModule_ContentFormActionBar(const std::string& sName, const std::string& sFormPath, const std::string& sSubmitEvent, const std::string& sCancelEvent, const std::string& sValidateEvent, const std::string& sSubmitCaption, const std::string& sCancelCaption, bool bOptimistic, PStateMachineData pStateMachineData)
	: CUIModule_ContentFormEntity(sName, sFormPath, [] { CUIExpression e; e.setFixedValue(""); return e; }(), pStateMachineData),
	  m_sSubmitEvent(sSubmitEvent), m_sCancelEvent(sCancelEvent), m_sValidateEvent(sValidateEvent),
	  m_sSubmitCaption(sSubmitCaption), m_sCancelCaption(sCancelCaption), m_bOptimistic(bOptimistic)
{
}

CUIModule_ContentFormActionBar::~CUIModule_ContentFormActionBar() {}

std::string CUIModule_ContentFormActionBar::getTypeString() { return "actionbar"; }

void CUIModule_ContentFormActionBar::populateClientVariables(CParameterHandler* pClientVariableHandler)
{
	registerClientVariableGroup(pClientVariableHandler);
}

void CUIModule_ContentFormActionBar::syncClientVariables(CParameterHandler* pClientVariableHandler)
{
}

void CUIModule_ContentFormActionBar::writeVariablesToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler)
{
	object.addString(AMC_API_KEY_UI_FORMSUBMITEVENT, m_sSubmitEvent);
	object.addString(AMC_API_KEY_UI_FORMCANCELEVENT, m_sCancelEvent);
	object.addString(AMC_API_KEY_UI_FORMVALIDATEEVENT, m_sValidateEvent);
	object.addString(AMC_API_KEY_UI_FORMSUBMITCAPTION, m_sSubmitCaption);
	object.addString(AMC_API_KEY_UI_FORMCANCELCAPTION, m_sCancelCaption);
	object.addBool(AMC_API_KEY_UI_FORMOPTIMISTIC, m_bOptimistic);
}

void CUIModule_ContentFormActionBar::registerFrontendAttributes(PUIFrontendDefinitionModuleStore pStore)
{
	CUIModule_ContentFormEntity::registerFrontendAttributes(pStore);

	CUIExpression submitExpr; submitExpr.setFixedValue(m_sSubmitEvent);
	pStore->registerValue("submitevent", eUIFrontendDefinitionAttributeType::atString, submitExpr);
	CUIExpression cancelExpr; cancelExpr.setFixedValue(m_sCancelEvent);
	pStore->registerValue("cancelevent", eUIFrontendDefinitionAttributeType::atString, cancelExpr);
	CUIExpression validateExpr; validateExpr.setFixedValue(m_sValidateEvent);
	pStore->registerValue("validateevent", eUIFrontendDefinitionAttributeType::atString, validateExpr);
	CUIExpression submitCaptionExpr; submitCaptionExpr.setFixedValue(m_sSubmitCaption);
	pStore->registerValue("submitcaption", eUIFrontendDefinitionAttributeType::atString, submitCaptionExpr);
	CUIExpression cancelCaptionExpr; cancelCaptionExpr.setFixedValue(m_sCancelCaption);
	pStore->registerValue("cancelcaption", eUIFrontendDefinitionAttributeType::atString, cancelCaptionExpr);
	CUIExpression optimisticExpr; optimisticExpr.setFixedValue(m_bOptimistic ? "1" : "0");
	pStore->registerValue("optimistic", eUIFrontendDefinitionAttributeType::atBoolean, optimisticExpr);
}


// ── Shared XML parser ─────────────────────────────────────────────────────────

void CUIModule_ContentForm::parseXmlFormNode(const pugi::xml_node& xmlNode, const std::string& sFormPath, PStateMachineData pStateMachineData, std::function<void(PUIModule_ContentFormEntity)> addEntityFn)
{
	for (auto formNode : xmlNode.children()) {
		std::string sNodeName = formNode.name();
		PUIModule_ContentFormEntity pEntity;

		if (sNodeName == "edit")
			pEntity = CUIModule_ContentFormEdit::makeFromXML(formNode, sFormPath, pStateMachineData);
		else if (sNodeName == "switch")
			pEntity = CUIModule_ContentFormSwitch::makeFromXML(formNode, sFormPath, pStateMachineData);
		else if (sNodeName == "memo")
			pEntity = CUIModule_ContentFormMemo::makeFromXML(formNode, sFormPath, pStateMachineData);
		else if (sNodeName == "combobox")
			pEntity = CUIModule_ContentFormCombobox::makeFromXML(formNode, sFormPath, pStateMachineData);
		else if (sNodeName == "number")
			pEntity = CUIModule_ContentFormNumber::makeFromXML(formNode, sFormPath, pStateMachineData);
		else if (sNodeName == "slider")
			pEntity = CUIModule_ContentFormSlider::makeFromXML(formNode, sFormPath, pStateMachineData);
		else if (sNodeName == "datetime")
			pEntity = CUIModule_ContentFormDatetime::makeFromXML(formNode, sFormPath, pStateMachineData);
		else if (sNodeName == "multiselect")
			pEntity = CUIModule_ContentFormMultiselect::makeFromXML(formNode, sFormPath, pStateMachineData);
		else if (sNodeName == "calculated")
			pEntity = CUIModule_ContentFormCalculated::makeFromXML(formNode, sFormPath, pStateMachineData);
		else if (sNodeName == "section")
			pEntity = CUIModule_ContentFormSection::makeFromXML(formNode, sFormPath, pStateMachineData);
		else if (sNodeName == "actionbar")
			pEntity = CUIModule_ContentFormActionBar::makeFromXML(formNode, sFormPath, pStateMachineData);

		if (pEntity != nullptr) {
			pEntity->setDisabledExpression(CUIExpression(formNode, "disabled"));
			pEntity->setReadOnlyExpression(CUIExpression(formNode, "readonly"));
			addEntityFn(pEntity);
		}
	}
}


PUIModule_ContentForm CUIModule_ContentForm::makeFromXML(const pugi::xml_node& xmlNode, const std::string& sItemName, const std::string& sModulePath, PUIModuleEnvironment pUIModuleEnvironment)
{
	LibMCAssertNotNull(pUIModuleEnvironment);

	auto visibleAttrib = xmlNode.attribute("visible");
	auto bVisible = visibleAttrib.empty() ? true : visibleAttrib.as_bool();

	auto pForm = std::make_shared <CUIModule_ContentForm>(pUIModuleEnvironment->stateMachineData(), sItemName, sModulePath, bVisible);

	pUIModuleEnvironment->contentRegistry()->registerFormName(pForm->getUUID(), pForm->getName());

	parseXmlFormNode(xmlNode, pForm->getItemPath(), pUIModuleEnvironment->stateMachineData(), [&pForm](PUIModule_ContentFormEntity pEntity) {
		pForm->addEntity(pEntity);
	});

	return pForm;

}


CUIModule_ContentForm::CUIModule_ContentForm(PStateMachineData pStateMachineData, const std::string& sName, const std::string& sModulePath, bool bVisible)
	: CUIModule_ContentItem(AMCCommon::CUtils::createUUID(), sName, sModulePath),
	  m_sName(sName),
	  m_pStateMachineData(pStateMachineData),
	  m_bVisible(bVisible)

{
	LibMCAssertNotNull(pStateMachineData);


}

CUIModule_ContentForm::~CUIModule_ContentForm()	
{

}


void CUIModule_ContentForm::addLegacyContentToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler, uint32_t nStateID)
{
	object.addString(AMC_API_KEY_UI_ITEMTYPE, "form");
	object.addString(AMC_API_KEY_UI_ITEMUUID, m_sUUID);
	object.addBool(AMC_API_KEY_UI_VISIBLE, m_bVisible);

	CJSONWriterArray entityArray(writer);
	auto pGroup = pClientVariableHandler->findGroup(getItemPath(), true);
	auto bVisible = pGroup->getBoolParameterValueByName(AMC_API_KEY_UI_VISIBLE);
	object.addBool(AMC_API_KEY_UI_VISIBLE, bVisible);

	for (auto pEntity : m_Entities) {
		CJSONWriterObject entityObject(writer);
		pEntity->addContentToJSON(writer, entityObject, pClientVariableHandler);
		entityArray.addObject(entityObject);
	}

	object.addArray(AMC_API_KEY_UI_FORMENTITIES, entityArray);
}

void CUIModule_ContentForm::populateClientVariables(CParameterHandler* pClientVariableHandler)
{
	LibMCAssertNotNull(pClientVariableHandler);

	auto pGroup = pClientVariableHandler->addGroup(getItemPath(), "form UI element");
	pGroup->addNewBoolParameter(AMC_API_KEY_UI_VISIBLE, "visibility of the UI form", m_bVisible);

	for (auto pEntity : m_Entities) {
		pEntity->populateClientVariables(pClientVariableHandler);
	}
}

std::string CUIModule_ContentForm::getName()
{
	return m_sName;
}

bool CUIModule_ContentForm::IsVisible()
{
	return m_bVisible;
}

void CUIModule_ContentForm::addEntity(PUIModule_ContentFormEntity pEntity)
{
	LibMCAssertNotNull(pEntity.get());

	auto sName = pEntity->getName();
	if (hasEntityWithName (sName))
		throw ELibMCCustomException (LIBMC_ERROR_DUPLICATEENTITYNAME, pEntity->getName());

	m_Entities.push_back(pEntity);
	m_EntityNameMap.insert(std::make_pair (sName, pEntity));
	m_EntityUUIDMap.insert(std::make_pair(pEntity->getUUID(), pEntity));
}

bool CUIModule_ContentForm::hasEntityWithName(const std::string& sName)
{
	auto iIter = m_EntityNameMap.find(sName);
	return (iIter != m_EntityNameMap.end());
}


PUIModule_ContentFormEntity CUIModule_ContentForm::findEntityByName(const std::string& sName)
{
	auto iIter = m_EntityNameMap.find(sName);
	if (iIter != m_EntityNameMap.end())
		return iIter->second;

	return nullptr;

}

PUIModule_ContentFormEntity CUIModule_ContentForm::findEntityByUUID(const std::string& sUUID)
{
	auto iIter = m_EntityUUIDMap.find(sUUID);
	if (iIter != m_EntityUUIDMap.end())
		return iIter->second;

	for (auto& pEntity : m_Entities) {
		auto pSection = std::dynamic_pointer_cast<CUIModule_ContentFormSection>(pEntity);
		if (pSection) {
			auto pFound = pSection->findEntityByUUID(sUUID);
			if (pFound)
				return pFound;
		}
	}

	return nullptr;

}

std::list<PUIModule_ContentFormEntity> CUIModule_ContentForm::getEntities()
{
	return m_Entities;
}

// Returns all UUIDs that could be contained in this Item
std::list <std::string> CUIModule_ContentForm::getReferenceUUIDs()
{
	std::list <std::string> resultList;
	resultList.push_back(getUUID());
	for (auto pEntity : m_Entities) {
		resultList.push_back(pEntity->getUUID());
		auto pSection = std::dynamic_pointer_cast<CUIModule_ContentFormSection>(pEntity);
		if (pSection) {
			for (auto& pChild : pSection->getEntities())
				resultList.push_back(pChild->getUUID());
		}
	}
	return resultList;
}

void CUIModule_ContentForm::setEventPayloadValue(const std::string& sEventName, const std::string& sPayloadUUID, const std::string& sPayloadValue, CParameterHandler* pClientVariableHandler)
{
	LibMCAssertNotNull(pClientVariableHandler);
	auto pFormEntity = findEntityByUUID(sPayloadUUID);
	if (pFormEntity.get() == nullptr)
		throw ELibMCCustomException(LIBMC_ERROR_FORMENTITYNOTFOUND, sEventName + "/" + sPayloadUUID);

	auto pGroup = pClientVariableHandler->findGroup(pFormEntity->getElementPath(), true);
	pGroup->setParameterValueByName("value", sPayloadValue);

}

std::string CUIModule_ContentForm::findElementPathByUUID(const std::string& sUUID)
{
	if (sUUID == m_sUUID)
		return getItemPath();

	auto pFormEntity = findEntityByUUID(sUUID);
	if (pFormEntity.get() != nullptr) {
		return pFormEntity->getElementPath();
	}

	return "";
}

std::string CUIModule_ContentForm::getItemType()
{
	return "form";
}

void CUIModule_ContentForm::registerFrontendAttributes()
{
	CUIExpression visibleExpr;
	visibleExpr.setFixedValue(m_bVisible ? "1" : "0");
	registerItemBoolAttribute("visible", visibleExpr);

	for (auto& pEntity : m_Entities) {
		auto pChildStore = m_pItemModuleStore->addChildStore(pEntity->getUUID(), pEntity->getElementPath(), pEntity->getTypeString());
		pEntity->registerFrontendAttributes(pChildStore);
	}
}

