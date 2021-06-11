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
#include "libmc_interfaceexception.hpp"

#include "amc_api_constants.hpp"
#include "Common/common_utils.hpp"

using namespace AMC;


CUIModule_ContentFormEntity::CUIModule_ContentFormEntity(const std::string& sName, const std::string& sCaption)
	: m_sUUID(AMCCommon::CUtils::createUUID()), m_sCaption(sCaption), m_sName (sName)
{

}

CUIModule_ContentFormEntity::~CUIModule_ContentFormEntity()
{

}

std::string CUIModule_ContentFormEntity::getUUID()
{
	return m_sUUID;
}

std::string CUIModule_ContentFormEntity::getCaption()
{
	return m_sCaption;
}



CUIModule_ContentFormEdit::CUIModule_ContentFormEdit(const std::string& sName, const std::string& sCaption)
	: CUIModule_ContentFormEntity (sName, sCaption)
{

}

CUIModule_ContentFormEdit::~CUIModule_ContentFormEdit()
{

}

std::string CUIModule_ContentFormEdit::getTypeString()
{
	return "edit";
}


CUIModule_ContentFormSwitch::CUIModule_ContentFormSwitch(const std::string& sName, const std::string& sCaption)
	: CUIModule_ContentFormEntity (sName, sCaption)
{

}

CUIModule_ContentFormSwitch::~CUIModule_ContentFormSwitch()
{

}

std::string CUIModule_ContentFormSwitch::getTypeString()
{
	return "switch";
}


CUIModule_ContentFormMemo::CUIModule_ContentFormMemo(const std::string& sName, const std::string& sCaption)
	: CUIModule_ContentFormEntity (sName, sCaption)
{

}

CUIModule_ContentFormMemo::~CUIModule_ContentFormMemo()
{

}

std::string CUIModule_ContentFormMemo::getTypeString()
{
	return "memo";
}

CUIModule_ContentFormCombobox::CUIModule_ContentFormCombobox(const std::string& sName, const std::string& sCaption)
	: CUIModule_ContentFormEntity(sName, sCaption)
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
		entityObject.addString(AMC_API_KEY_UI_FORMUUID, pEntity->getUUID ());
		entityObject.addString(AMC_API_KEY_UI_FORMCAPTION, pEntity->getCaption ());
		entityObject.addString(AMC_API_KEY_UI_FORMTYPE, pEntity->getTypeString ());
		entityArray.addObject(entityObject);
	}


	object.addArray(AMC_API_KEY_UI_FORMENTITIES, entityArray); 

}


void CUIModule_ContentForm::addEdit(const std::string& sName, const std::string& sCaption)
{
	m_Entities.push_back(std::make_shared<CUIModule_ContentFormEdit> (sName, sCaption));
}

void CUIModule_ContentForm::addSwitch(const std::string& sName, const std::string& sCaption)
{
	m_Entities.push_back(std::make_shared<CUIModule_ContentFormSwitch>(sName, sCaption));
}

void CUIModule_ContentForm::addMemo(const std::string& sName, const std::string& sCaption)
{
	m_Entities.push_back(std::make_shared<CUIModule_ContentFormMemo>(sName, sCaption));
}

void CUIModule_ContentForm::addCombobox(const std::string& sName, const std::string& sCaption)
{
	m_Entities.push_back(std::make_shared<CUIModule_ContentFormCombobox>(sName, sCaption));
}

std::string CUIModule_ContentForm::getName()
{
	return m_sName;
}