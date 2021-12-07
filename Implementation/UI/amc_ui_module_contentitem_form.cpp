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
#include "libmc_exceptiontypes.hpp"

#include "amc_api_constants.hpp"
#include "Common/common_utils.hpp"

using namespace AMC;


CUIModule_ContentFormEntity::CUIModule_ContentFormEntity(const std::string& sName, const std::string& sCaption, const std::string & sDefaultValue)
	: m_sUUID(AMCCommon::CUtils::createUUID()), m_sCaption(sCaption), m_sName (sName), m_sDefaultValue (sDefaultValue), m_bDisabled (false), m_bReadOnly (false)
{
	if (sName == "")
		throw ELibMCInterfaceException(LIBMC_ERROR_FORMENTITYNAMEMISSING);

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

std::string CUIModule_ContentFormEntity::getCaption()
{
	return m_sCaption;
}

std::string CUIModule_ContentFormEntity::getDefaultValue()
{
	return m_sDefaultValue;
}


bool CUIModule_ContentFormEntity::getDisabled()
{
	return m_bDisabled;
}

bool CUIModule_ContentFormEntity::getReadOnly()
{
	return m_bReadOnly;
}


void CUIModule_ContentFormEntity::setDisabled(bool bDisabled)
{
	m_bDisabled = bDisabled;
}

void CUIModule_ContentFormEntity::setReadOnly(bool bReadOnly)
{
	m_bReadOnly = bReadOnly;
}

void CUIModule_ContentFormEntity::addDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& object)
{
	object.addString(AMC_API_KEY_UI_FORMUUID, getUUID());
	object.addString(AMC_API_KEY_UI_FORMCAPTION, getCaption());
	object.addString(AMC_API_KEY_UI_FORMTYPE, getTypeString());
	object.addString(AMC_API_KEY_UI_FORMDEFAULTVALUE, getDefaultValue());
	object.addBool(AMC_API_KEY_UI_FORMDISABLED, getDisabled());
	object.addBool(AMC_API_KEY_UI_FORMREADONLY, getReadOnly());

}


void CUIModule_ContentFormEntity::addContentToJSON(CJSONWriter& writer, CJSONWriterObject& object)
{
	object.addString(AMC_API_KEY_UI_FORMUUID, getUUID());
	object.addString(AMC_API_KEY_UI_FORMCAPTION, getCaption());
	object.addString(AMC_API_KEY_UI_FORMDEFAULTVALUE, getDefaultValue());
	object.addBool(AMC_API_KEY_UI_FORMDISABLED, getDisabled());
	object.addBool(AMC_API_KEY_UI_FORMREADONLY, getReadOnly());

}



CUIModule_ContentFormEdit::CUIModule_ContentFormEdit(const std::string& sName, const std::string& sCaption, const std::string& sDefaultValue, const std::string& sPrefix, const std::string& sSuffix, const std::string& sParameterMapping)
	: CUIModule_ContentFormEntity (sName, sCaption, sDefaultValue), m_sPrefix (sPrefix), m_sSuffix (sSuffix)
{
	if (!sParameterMapping.empty()) {
		CUIUtils::extractParameterDetailsFromDotString(sParameterMapping, m_sParameterInstanceName, m_sParameterGroupName, m_sParameterName);
	}
}

CUIModule_ContentFormEdit::~CUIModule_ContentFormEdit()
{

}

std::string CUIModule_ContentFormEdit::getTypeString()
{
	return "edit";
}

void CUIModule_ContentFormEdit::addDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& object)
{
	CUIModule_ContentFormEntity::addDefinitionToJSON(writer, object);

	object.addString(AMC_API_KEY_UI_FORMPREFIX, m_sPrefix);
	object.addString(AMC_API_KEY_UI_FORMSUFFIX, m_sSuffix);
}

void CUIModule_ContentFormEdit::addContentToJSON(CJSONWriter& writer, CJSONWriterObject& object)
{
	CUIModule_ContentFormEntity::addContentToJSON(writer, object);

	object.addString(AMC_API_KEY_UI_FORMPREFIX, m_sPrefix);
	object.addString(AMC_API_KEY_UI_FORMSUFFIX, m_sSuffix);
}



CUIModule_ContentFormSwitch::CUIModule_ContentFormSwitch(const std::string& sName, const std::string& sCaption, const std::string& sDefaultValue)
	: CUIModule_ContentFormEntity (sName, sCaption, sDefaultValue)
{

}

CUIModule_ContentFormSwitch::~CUIModule_ContentFormSwitch()
{

}

std::string CUIModule_ContentFormSwitch::getTypeString()
{
	return "switch";
}


CUIModule_ContentFormMemo::CUIModule_ContentFormMemo(const std::string& sName, const std::string& sCaption, const std::string& sDefaultValue)
	: CUIModule_ContentFormEntity (sName, sCaption, sDefaultValue)
{

}

CUIModule_ContentFormMemo::~CUIModule_ContentFormMemo()
{

}

std::string CUIModule_ContentFormMemo::getTypeString()
{
	return "memo";
}

CUIModule_ContentFormCombobox::CUIModule_ContentFormCombobox(const std::string& sName, const std::string& sCaption, const std::string& sDefaultValue)
	: CUIModule_ContentFormEntity(sName, sCaption, sDefaultValue)
{

}


CUIModule_ContentFormCombobox::~CUIModule_ContentFormCombobox()
{

}

std::string CUIModule_ContentFormCombobox::getTypeString()
{
	return "combobox";
}


CUIModule_ContentForm::CUIModule_ContentForm(const std::string& sName)
	: CUIModule_ContentItem(AMCCommon::CUtils::createUUID()), m_sName(sName)
{

}

CUIModule_ContentForm::~CUIModule_ContentForm()	
{

}


void CUIModule_ContentForm::addDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& object)
{
	object.addString(AMC_API_KEY_UI_ITEMTYPE, "form");
	object.addString(AMC_API_KEY_UI_ITEMUUID, m_sUUID);

	CJSONWriterArray entityArray(writer);

	for (auto pEntity : m_Entities) {
		CJSONWriterObject entityObject(writer);
		pEntity->addDefinitionToJSON(writer, entityObject);
		entityArray.addObject(entityObject);
	}


	object.addArray(AMC_API_KEY_UI_FORMENTITIES, entityArray); 

}


void CUIModule_ContentForm::addContentToJSON(CJSONWriter& writer, CJSONWriterObject& object)
{
	CJSONWriterArray entityArray(writer);
	for (auto pEntity : m_Entities) {
		CJSONWriterObject entityObject(writer);
		pEntity->addContentToJSON(writer, entityObject);
		entityArray.addObject(entityObject);
	}

	object.addArray(AMC_API_KEY_UI_ITEMENTRIES, entityArray);
}


PUIModule_ContentFormEntity CUIModule_ContentForm::addEdit(const std::string& sName, const std::string& sCaption, const std::string& sDefaultValue, const std::string& sPrefix, const std::string& sSuffix, const std::string& sParameterMapping)
{	
	auto pEntity = std::make_shared<CUIModule_ContentFormEdit>(sName, sCaption, sDefaultValue, sPrefix, sSuffix, sParameterMapping);
	addEntityEx(pEntity);
	return pEntity;
}

PUIModule_ContentFormEntity CUIModule_ContentForm::addSwitch(const std::string& sName, const std::string& sCaption, const std::string& sDefaultValue)
{
	auto pEntity = std::make_shared<CUIModule_ContentFormSwitch>(sName, sCaption, sDefaultValue);
	addEntityEx(pEntity);
	return pEntity;
}

PUIModule_ContentFormEntity CUIModule_ContentForm::addMemo(const std::string& sName, const std::string& sCaption, const std::string& sDefaultValue)
{
	auto pEntity = std::make_shared<CUIModule_ContentFormMemo>(sName, sCaption, sDefaultValue);
	addEntityEx(pEntity);
	return pEntity;
}

PUIModule_ContentFormEntity CUIModule_ContentForm::addCombobox(const std::string& sName, const std::string& sCaption, const std::string& sDefaultValue)
{
	auto pEntity = std::make_shared<CUIModule_ContentFormCombobox>(sName, sCaption, sDefaultValue);
	addEntityEx(pEntity);
	return pEntity;
}

std::string CUIModule_ContentForm::getName()
{
	return m_sName;
}

void CUIModule_ContentForm::addEntityEx(PUIModule_ContentFormEntity pEntity)
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
