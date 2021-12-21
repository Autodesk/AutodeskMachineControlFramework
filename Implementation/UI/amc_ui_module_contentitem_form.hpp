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

#ifndef __AMC_UI_MODULE_CONTENTITEM_FORM
#define __AMC_UI_MODULE_CONTENTITEM_FORM

#include "header_protection.hpp"

#ifndef __AMCIMPL_UI_MODULE
#error this header is protected and should only be included in the corresponding implementation CPP files.
#endif

#include "amc_ui_module_contentitem.hpp"
#include "amc_ui_expression.hpp"

#include "pugixml.hpp"

namespace AMC {

	amcDeclareDependingClass(CUIModule_ContentForm, PUIModule_ContentForm);
	amcDeclareDependingClass(CUIModule_ContentFormEntity, PUIModule_ContentFormEntity);
	amcDeclareDependingClass(CUIModule_ContentFormEdit, PUIModule_ContentFormEdit);
	amcDeclareDependingClass(CUIModule_ContentFormSwitch, PUIModule_ContentFormSwitch);
	amcDeclareDependingClass(CUIModule_ContentFormMemo, PUIModule_ContentFormMemo);
	amcDeclareDependingClass(CUIModule_ContentFormCheckbox, PUIModule_ContentFormCheckbox);
	amcDeclareDependingClass(CStateMachineData, PStateMachineData);
	amcDeclareDependingClass(CParameterHandler, PParameterHandler);
	amcDeclareDependingClass(CParameterGroup, PParameterGroup);
	amcDeclareDependingClass(CUIModuleEnvironment, PUIModuleEnvironment);


	class CUIModule_ContentFormEntity {
	protected:

		std::string m_sElementPath;
		std::string m_sName;
		std::string m_sUUID;

		CUIExpression m_CaptionExpression;
		CUIExpression m_DisabledExpression;
		CUIExpression m_ReadOnlyExpression;

		PStateMachineData m_pStateMachineData;

		PParameterGroup registerClientVariableGroup(CParameterHandler* pClientVariableHandler);

		PParameterGroup getClientVariableGroup(CParameterHandler* pClientVariableHandler);

		virtual void writeVariablesToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler) = 0;

	public:


		CUIModule_ContentFormEntity(const std::string& sName, const std::string & sFormPath, CUIExpression Caption, PStateMachineData pStateMachineData);

		virtual ~CUIModule_ContentFormEntity();

		std::string getName();

		std::string getUUID();

		void setDisabledExpression(CUIExpression Expression);

		void setReadOnlyExpression(CUIExpression Expression);

		virtual std::string getTypeString() = 0;

		virtual void addDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler);

		virtual void addContentToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler);

		virtual std::string getElementPath();

		virtual void populateClientVariables(CParameterHandler* pClientVariableHandler) = 0;

		virtual void syncClientVariables(CParameterHandler* pClientVariableHandler) = 0;

	};

		
	class CUIModule_ContentFormEdit : public CUIModule_ContentFormEntity {
	protected:
		CUIExpression m_ValueExpression;
		CUIExpression m_PrefixExpression;
		CUIExpression m_SuffixExpression;

	public:

		CUIModule_ContentFormEdit(const std::string& sName, const std::string& sFormPath, CUIExpression Caption, CUIExpression Value, CUIExpression Prefix, CUIExpression Suffix, PStateMachineData pStateMachineData);

		virtual ~CUIModule_ContentFormEdit();

		virtual std::string getTypeString() override;

		virtual void populateClientVariables(CParameterHandler* pClientVariableHandler) override;

		virtual void syncClientVariables(CParameterHandler* pClientVariableHandler) override;

		virtual void writeVariablesToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler) override;

	};


	class CUIModule_ContentFormSwitch : public CUIModule_ContentFormEntity {
	protected:

	public:

		CUIModule_ContentFormSwitch(const std::string& sName, const std::string& sFormPath, CUIExpression Caption, CUIExpression Value, PStateMachineData pStateMachineData);

		virtual ~CUIModule_ContentFormSwitch();

		virtual std::string getTypeString() override;

		virtual void populateClientVariables(CParameterHandler* pClientVariableHandler) override;

		virtual void syncClientVariables(CParameterHandler* pClientVariableHandler) override;

		virtual void writeVariablesToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler) override;

	};
	
	class CUIModule_ContentFormMemo : public CUIModule_ContentFormEntity {
	protected:

	public:

		CUIModule_ContentFormMemo(const std::string& sName, const std::string& sFormPath, CUIExpression Caption, CUIExpression Value, PStateMachineData pStateMachineData);

		virtual ~CUIModule_ContentFormMemo();

		virtual std::string getTypeString() override;

		virtual void populateClientVariables(CParameterHandler* pClientVariableHandler) override;

		virtual void syncClientVariables(CParameterHandler* pClientVariableHandler) override;

		virtual void writeVariablesToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler) override;

	};
	
	class CUIModule_ContentFormCombobox : public CUIModule_ContentFormEntity {
	protected:

	public:

		CUIModule_ContentFormCombobox(const std::string& sName, const std::string& sFormPath, CUIExpression Caption, CUIExpression Value, PStateMachineData pStateMachineData);

		virtual ~CUIModule_ContentFormCombobox();

		virtual std::string getTypeString() override;

		virtual void populateClientVariables(CParameterHandler* pClientVariableHandler) override;

		virtual void syncClientVariables(CParameterHandler* pClientVariableHandler) override;

		virtual void writeVariablesToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler) override;

	};	
	
	class CUIModule_ContentForm : public CUIModule_ContentItem {
	protected:		
		std::list<PUIModule_ContentFormEntity> m_Entities;
		std::map<std::string, PUIModule_ContentFormEntity> m_EntityNameMap;
		std::map<std::string, PUIModule_ContentFormEntity> m_EntityUUIDMap;
		std::string m_sName;

		PStateMachineData m_pStateMachineData;

		void addEntityEx(PUIModule_ContentFormEntity pEntity);

	public:

		static PUIModule_ContentForm makeFromXML(const pugi::xml_node& xmlNode, const std::string& sItemName, const std::string& sModulePath, PUIModuleEnvironment pUIModuleEnvironment);

		CUIModule_ContentForm(PStateMachineData pStateMachineData, const std::string& sName, const std::string& sModulePath);

		virtual ~CUIModule_ContentForm();

		void addDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler) override;

		virtual void addContentToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler) override;

		PUIModule_ContentFormEntity addEdit(const std::string& sName, CUIExpression Caption, CUIExpression Value, CUIExpression Prefix, CUIExpression Suffix);
		PUIModule_ContentFormEntity addSwitch(const std::string& sName, CUIExpression Caption, CUIExpression Value);
		PUIModule_ContentFormEntity addMemo(const std::string& sName, CUIExpression Caption, CUIExpression Value);
		PUIModule_ContentFormEntity addCombobox(const std::string& sName, CUIExpression Caption, CUIExpression Value);
		
		std::string getName();

		bool hasEntityWithName(const std::string& sName);
		PUIModule_ContentFormEntity findEntityByName(const std::string& sName);
		PUIModule_ContentFormEntity findEntityByUUID(const std::string& sUUID);

		std::list<PUIModule_ContentFormEntity> getEntities ();

		// Returns all UUIDs that could be contained in this Item
		virtual std::list <std::string> getReferenceUUIDs() override;

		virtual void populateClientVariables(CParameterHandler* pClientVariableHandler) override;

	};




}


#endif //__AMC_UI_MODULE_CONTENTITEM_BUTTONGROUP



