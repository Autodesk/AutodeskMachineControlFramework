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


void CUIModule_ContentFormEntity::addDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler)
{
	object.addString(AMC_API_KEY_UI_FORMUUID, getUUID());
	object.addString(AMC_API_KEY_UI_FORMTYPE, getTypeString());

	auto pGroup = getClientVariableGroup(pClientVariableHandler);
	object.addString(AMC_API_KEY_UI_FORMCAPTION, pGroup->getParameterValueByName(AMC_API_KEY_UI_FORMCAPTION));
	object.addBool(AMC_API_KEY_UI_FORMDISABLED, pGroup->getBoolParameterValueByName(AMC_API_KEY_UI_FORMDISABLED));
	object.addBool(AMC_API_KEY_UI_FORMREADONLY, pGroup->getBoolParameterValueByName(AMC_API_KEY_UI_FORMREADONLY));

	this->writeVariablesToJSON(writer, object, pClientVariableHandler);
}


void CUIModule_ContentFormEntity::addContentToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler)
{
	object.addString(AMC_API_KEY_UI_FORMUUID, getUUID());

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


PUIModule_ContentFormEdit CUIModule_ContentFormEdit::makeFromXML(const pugi::xml_node& xmlNode, const std::string& sFormPath, PStateMachineData pStateMachineData)
{
	auto nameAttrib = xmlNode.attribute("name");

	if (nameAttrib.empty())
		throw ELibMCInterfaceException(LIBMC_ERROR_FORMENTITYNAMEMISSING);
	CUIExpression caption(xmlNode, "caption");
	CUIExpression value(xmlNode, "value");
	CUIExpression prefix(xmlNode, "prefix");
	CUIExpression suffix(xmlNode, "suffix");

	return std::make_shared<CUIModule_ContentFormEdit>(nameAttrib.as_string(), sFormPath, caption, value, prefix, suffix, pStateMachineData);

}

CUIModule_ContentFormEdit::CUIModule_ContentFormEdit(const std::string& sName, const std::string& sFormPath, CUIExpression Caption, CUIExpression Value, CUIExpression Prefix, CUIExpression Suffix, PStateMachineData pStateMachineData)
	: CUIModule_ContentFormEntity (sName, sFormPath, Caption, pStateMachineData), m_PrefixExpression (Prefix), m_SuffixExpression (Suffix), m_ValueExpression (Value)
{
	m_PrefixExpression.checkExpressionSyntax(m_pStateMachineData);
	m_SuffixExpression.checkExpressionSyntax(m_pStateMachineData);
	m_ValueExpression.checkExpressionSyntax(m_pStateMachineData);
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
}

void CUIModule_ContentFormEdit::syncClientVariables(CParameterHandler* pClientVariableHandler)
{
	auto pGroup = getClientVariableGroup(pClientVariableHandler);
	if (m_ValueExpression.needsSync())
		pGroup->setParameterValueByName(AMC_API_KEY_UI_FORMDEFAULTVALUE, m_ValueExpression.evaluateStringValue(m_pStateMachineData));
	if (m_PrefixExpression.needsSync())
		pGroup->setParameterValueByName(AMC_API_KEY_UI_FORMPREFIX, m_PrefixExpression.evaluateStringValue(m_pStateMachineData));
	if (m_SuffixExpression.needsSync())
		pGroup->setParameterValueByName(AMC_API_KEY_UI_FORMSUFFIX, m_SuffixExpression.evaluateStringValue(m_pStateMachineData));

}

void CUIModule_ContentFormEdit::writeVariablesToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler)
{
	auto pGroup = getClientVariableGroup(pClientVariableHandler);

	object.addString(AMC_API_KEY_UI_FORMDEFAULTVALUE, pGroup->getParameterValueByName(AMC_API_KEY_UI_FORMDEFAULTVALUE));
	object.addString(AMC_API_KEY_UI_FORMPREFIX, pGroup->getParameterValueByName(AMC_API_KEY_UI_FORMPREFIX));
	object.addString(AMC_API_KEY_UI_FORMSUFFIX, pGroup->getParameterValueByName(AMC_API_KEY_UI_FORMSUFFIX));
}


PUIModule_ContentFormSwitch CUIModule_ContentFormSwitch::makeFromXML(const pugi::xml_node& xmlNode, const std::string& sFormPath, PStateMachineData pStateMachineData)
{
	auto nameAttrib = xmlNode.attribute("name");

	if (nameAttrib.empty())
		throw ELibMCInterfaceException(LIBMC_ERROR_FORMENTITYNAMEMISSING);
	CUIExpression caption(xmlNode, "caption");
	CUIExpression value(xmlNode, "value");

	return std::make_shared<CUIModule_ContentFormSwitch>(nameAttrib.as_string(), sFormPath, caption, value, pStateMachineData);

}

CUIModule_ContentFormSwitch::CUIModule_ContentFormSwitch(const std::string& sName, const std::string& sFormPath, CUIExpression Caption, CUIExpression Value, PStateMachineData pStateMachineData)
	: CUIModule_ContentFormEntity (sName, sFormPath, Caption, pStateMachineData)
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
}


void CUIModule_ContentFormSwitch::syncClientVariables(CParameterHandler* pClientVariableHandler)
{
}

void CUIModule_ContentFormSwitch::writeVariablesToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler)
{
	auto pGroup = getClientVariableGroup(pClientVariableHandler);
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

PUIModule_ContentFormCombobox CUIModule_ContentFormCombobox::makeFromXML(const pugi::xml_node& xmlNode, const std::string& sFormPath, PStateMachineData pStateMachineData)
{
	auto nameAttrib = xmlNode.attribute("name");

	if (nameAttrib.empty())
		throw ELibMCInterfaceException(LIBMC_ERROR_FORMENTITYNAMEMISSING);
	CUIExpression caption(xmlNode, "caption");
	CUIExpression value(xmlNode, "value");

	return std::make_shared<CUIModule_ContentFormCombobox>(nameAttrib.as_string(), sFormPath, caption, value, pStateMachineData);

}

CUIModule_ContentFormCombobox::CUIModule_ContentFormCombobox(const std::string& sName, const std::string& sFormPath, CUIExpression Caption, CUIExpression Value, PStateMachineData pStateMachineData)
	: CUIModule_ContentFormEntity(sName, sFormPath, Caption, pStateMachineData)
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
}

void CUIModule_ContentFormCombobox::syncClientVariables(CParameterHandler* pClientVariableHandler)
{

}

void CUIModule_ContentFormCombobox::writeVariablesToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler)
{
	auto pGroup = getClientVariableGroup(pClientVariableHandler);
}



PUIModule_ContentForm CUIModule_ContentForm::makeFromXML(const pugi::xml_node& xmlNode, const std::string& sItemName, const std::string& sModulePath, PUIModuleEnvironment pUIModuleEnvironment)
{
	LibMCAssertNotNull(pUIModuleEnvironment);

	auto pForm = std::make_shared <CUIModule_ContentForm>(pUIModuleEnvironment->stateMachineData(), sItemName, sModulePath);

	pUIModuleEnvironment->formRegistry()->registerFormName(pForm->getUUID(), pForm->getName());

	auto formNodes = xmlNode.children();
	for (auto formNode : formNodes) {

		std::string sNodeName = formNode.name();

		PUIModule_ContentFormEntity pEntity;

		if (sNodeName == "edit") 
			pEntity = CUIModule_ContentFormEdit::makeFromXML(formNode, pForm->getItemPath(), pUIModuleEnvironment->stateMachineData());
		if (sNodeName == "switch") 
			pEntity = CUIModule_ContentFormSwitch::makeFromXML(formNode, pForm->getItemPath(), pUIModuleEnvironment->stateMachineData());
		if (sNodeName == "memo")
			pEntity = CUIModule_ContentFormMemo::makeFromXML(formNode, pForm->getItemPath(), pUIModuleEnvironment->stateMachineData());
		if (sNodeName == "combobox")
			pEntity = CUIModule_ContentFormCombobox::makeFromXML(formNode, pForm->getItemPath(), pUIModuleEnvironment->stateMachineData());

		if (pEntity != nullptr) {
			pEntity->setDisabledExpression(CUIExpression(formNode, "disabled"));
			pEntity->setReadOnlyExpression(CUIExpression(formNode, "readonly"));
		}

	}

	return pForm;

}


CUIModule_ContentForm::CUIModule_ContentForm(PStateMachineData pStateMachineData, const std::string& sName, const std::string& sModulePath)
	: CUIModule_ContentItem(AMCCommon::CUtils::createUUID(), sName, sModulePath),
	  m_sName(sName),
	  m_pStateMachineData(pStateMachineData)
{
	LibMCAssertNotNull(pStateMachineData);


}

CUIModule_ContentForm::~CUIModule_ContentForm()	
{

}


void CUIModule_ContentForm::addDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler)
{
	object.addString(AMC_API_KEY_UI_ITEMTYPE, "form");
	object.addString(AMC_API_KEY_UI_ITEMUUID, m_sUUID);

	CJSONWriterArray entityArray(writer);

	for (auto pEntity : m_Entities) {
		CJSONWriterObject entityObject(writer);
		pEntity->addDefinitionToJSON(writer, entityObject, pClientVariableHandler);
		entityArray.addObject(entityObject);
	}


	object.addArray(AMC_API_KEY_UI_FORMENTITIES, entityArray); 

}


void CUIModule_ContentForm::addContentToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler)
{
	CJSONWriterArray entityArray(writer);
	for (auto pEntity : m_Entities) {
		CJSONWriterObject entityObject(writer);
		pEntity->addContentToJSON(writer, entityObject, pClientVariableHandler);
		entityArray.addObject(entityObject);
	}

	object.addArray(AMC_API_KEY_UI_ITEMENTRIES, entityArray);
}

void CUIModule_ContentForm::populateClientVariables(CParameterHandler* pClientVariableHandler)
{
	LibMCAssertNotNull(pClientVariableHandler);
	for (auto pEntity : m_Entities) {
		pEntity->populateClientVariables(pClientVariableHandler);
	}

}

std::string CUIModule_ContentForm::getName()
{
	return m_sName;
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
	for (auto pEntity : m_Entities)
		resultList.push_back(pEntity->getUUID ());

	return resultList;
}
