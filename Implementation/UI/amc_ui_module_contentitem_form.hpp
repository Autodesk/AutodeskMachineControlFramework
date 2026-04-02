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
#include <functional>

namespace AMC {

	amcDeclareDependingClass(CUIModule_ContentForm, PUIModule_ContentForm);
	amcDeclareDependingClass(CUIModule_ContentFormEntity, PUIModule_ContentFormEntity);
	amcDeclareDependingClass(CUIModule_ContentFormEdit, PUIModule_ContentFormEdit);
	amcDeclareDependingClass(CUIModule_ContentFormSwitch, PUIModule_ContentFormSwitch);
	amcDeclareDependingClass(CUIModule_ContentFormMemo, PUIModule_ContentFormMemo);
	amcDeclareDependingClass(CUIModule_ContentFormCheckbox, PUIModule_ContentFormCheckbox);
	amcDeclareDependingClass(CUIModule_ContentFormCombobox, PUIModule_ContentFormCombobox);
	amcDeclareDependingClass(CUIModule_ContentFormNumber, PUIModule_ContentFormNumber);
	amcDeclareDependingClass(CUIModule_ContentFormSlider, PUIModule_ContentFormSlider);
	amcDeclareDependingClass(CUIModule_ContentFormDatetime, PUIModule_ContentFormDatetime);
	amcDeclareDependingClass(CUIModule_ContentFormMultiselect, PUIModule_ContentFormMultiselect);
	amcDeclareDependingClass(CUIModule_ContentFormCalculated, PUIModule_ContentFormCalculated);
	amcDeclareDependingClass(CUIModule_ContentFormSection, PUIModule_ContentFormSection);
	amcDeclareDependingClass(CUIModule_ContentFormActionBar, PUIModule_ContentFormActionBar);
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

		PUIFrontendDefinitionModuleStore m_pFrontendStore;

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

		virtual void addContentToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler);

		virtual std::string getElementPath();

		virtual void populateClientVariables(CParameterHandler* pClientVariableHandler) = 0;

		virtual void syncClientVariables(CParameterHandler* pClientVariableHandler) = 0;

		// v2 frontend: register entity attributes on the given child store
		virtual void registerFrontendAttributes(PUIFrontendDefinitionModuleStore pStore);

	};

		
	class CUIModule_ContentFormEdit : public CUIModule_ContentFormEntity {
	protected:
		CUIExpression m_ValueExpression;
		CUIExpression m_PrefixExpression;
		CUIExpression m_SuffixExpression;

		CUIExpression m_ValidationExpression;
		CUIExpression m_ValidationMessageExpression;
		CUIExpression m_MinValueExpression;
		CUIExpression m_MaxValueExpression;

		std::string m_sOnChangeEvent;
	public:

		static PUIModule_ContentFormEdit makeFromXML(const pugi::xml_node& xmlNode, const std::string& sFormPath, PStateMachineData pStateMachineData);

		CUIModule_ContentFormEdit(const std::string& sName, const std::string& sFormPath, CUIExpression Caption, CUIExpression Value, CUIExpression Prefix, CUIExpression Suffix, std::string sOnChangeEvent, PStateMachineData pStateMachineData);

		virtual ~CUIModule_ContentFormEdit();

		virtual std::string getTypeString() override;

		virtual void populateClientVariables(CParameterHandler* pClientVariableHandler) override;

		virtual void syncClientVariables(CParameterHandler* pClientVariableHandler) override;

		virtual void writeVariablesToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler) override;

		virtual void setValidationExpressions(CUIExpression validationExpression, CUIExpression validationMessageExpression, CUIExpression minValueExpression, CUIExpression m_maxValueExpression);

		virtual void registerFrontendAttributes(PUIFrontendDefinitionModuleStore pStore) override;

	};


	class CUIModule_ContentFormSwitch : public CUIModule_ContentFormEntity {
	protected:
		CUIExpression m_ValueExpression;
		std::string m_sOnChangeEvent;

	public:

		static PUIModule_ContentFormSwitch makeFromXML(const pugi::xml_node& xmlNode, const std::string& sFormPath, PStateMachineData pStateMachineData);

		CUIModule_ContentFormSwitch(const std::string& sName, const std::string& sFormPath, CUIExpression Caption, CUIExpression Value, const std::string & sOnChangeEvent, PStateMachineData pStateMachineData);

		virtual ~CUIModule_ContentFormSwitch();

		virtual std::string getTypeString() override;

		virtual void populateClientVariables(CParameterHandler* pClientVariableHandler) override;

		virtual void syncClientVariables(CParameterHandler* pClientVariableHandler) override;

		virtual void writeVariablesToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler) override;

		virtual void registerFrontendAttributes(PUIFrontendDefinitionModuleStore pStore) override;

	};
	
	class CUIModule_ContentFormMemo : public CUIModule_ContentFormEntity {
	protected:

	public:

		static PUIModule_ContentFormMemo makeFromXML(const pugi::xml_node& xmlNode, const std::string& sFormPath, PStateMachineData pStateMachineData);

		CUIModule_ContentFormMemo(const std::string& sName, const std::string& sFormPath, CUIExpression Caption, CUIExpression Value, PStateMachineData pStateMachineData);

		virtual ~CUIModule_ContentFormMemo();

		virtual std::string getTypeString() override;

		virtual void populateClientVariables(CParameterHandler* pClientVariableHandler) override;

		virtual void syncClientVariables(CParameterHandler* pClientVariableHandler) override;

		virtual void writeVariablesToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler) override;

		virtual void registerFrontendAttributes(PUIFrontendDefinitionModuleStore pStore) override;

	};
	
	class CUIModule_ContentFormCombobox : public CUIModule_ContentFormEntity {
	protected:
		CUIExpression m_ValueExpression;
		std::string m_sOnChangeEvent;
		std::vector<std::pair<std::string, int>> m_Items;
	public:

		static PUIModule_ContentFormCombobox makeFromXML(const pugi::xml_node& xmlNode, const std::string& sFormPath, PStateMachineData pStateMachineData);

		CUIModule_ContentFormCombobox(const std::string& sName, const std::string& sFormPath, CUIExpression Caption, CUIExpression Value, const std::string& sOnChangeEvent, PStateMachineData pStateMachineData, const std::vector<std::pair<std::string, int>>& items);

		virtual ~CUIModule_ContentFormCombobox();

		virtual std::string getTypeString() override;

		virtual void populateClientVariables(CParameterHandler* pClientVariableHandler) override;

		virtual void syncClientVariables(CParameterHandler* pClientVariableHandler) override;

		virtual void writeVariablesToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler) override;

		virtual void registerFrontendAttributes(PUIFrontendDefinitionModuleStore pStore) override;

	};	
	
	// ── Phase 2: number ──────────────────────────────────────────────────────
	class CUIModule_ContentFormNumber : public CUIModule_ContentFormEntity {
	protected:
		CUIExpression m_ValueExpression;
		CUIExpression m_MinValueExpression;
		CUIExpression m_MaxValueExpression;
		CUIExpression m_StepExpression;
		CUIExpression m_UnitExpression;
		std::string m_sOnChangeEvent;
	public:
		static PUIModule_ContentFormNumber makeFromXML(const pugi::xml_node& xmlNode, const std::string& sFormPath, PStateMachineData pStateMachineData);
		CUIModule_ContentFormNumber(const std::string& sName, const std::string& sFormPath, CUIExpression Caption, CUIExpression Value, CUIExpression Min, CUIExpression Max, CUIExpression Step, CUIExpression Unit, const std::string& sOnChangeEvent, PStateMachineData pStateMachineData);
		virtual ~CUIModule_ContentFormNumber();
		virtual std::string getTypeString() override;
		virtual void populateClientVariables(CParameterHandler* pClientVariableHandler) override;
		virtual void syncClientVariables(CParameterHandler* pClientVariableHandler) override;
		virtual void writeVariablesToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler) override;
		virtual void registerFrontendAttributes(PUIFrontendDefinitionModuleStore pStore) override;
	};

	// ── Phase 2: slider ──────────────────────────────────────────────────────
	class CUIModule_ContentFormSlider : public CUIModule_ContentFormEntity {
	protected:
		CUIExpression m_ValueExpression;
		CUIExpression m_MinValueExpression;
		CUIExpression m_MaxValueExpression;
		CUIExpression m_StepExpression;
		CUIExpression m_UnitExpression;
		std::string m_sOnChangeEvent;
	public:
		static PUIModule_ContentFormSlider makeFromXML(const pugi::xml_node& xmlNode, const std::string& sFormPath, PStateMachineData pStateMachineData);
		CUIModule_ContentFormSlider(const std::string& sName, const std::string& sFormPath, CUIExpression Caption, CUIExpression Value, CUIExpression Min, CUIExpression Max, CUIExpression Step, CUIExpression Unit, const std::string& sOnChangeEvent, PStateMachineData pStateMachineData);
		virtual ~CUIModule_ContentFormSlider();
		virtual std::string getTypeString() override;
		virtual void populateClientVariables(CParameterHandler* pClientVariableHandler) override;
		virtual void syncClientVariables(CParameterHandler* pClientVariableHandler) override;
		virtual void writeVariablesToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler) override;
		virtual void registerFrontendAttributes(PUIFrontendDefinitionModuleStore pStore) override;
	};

	// ── Phase 2: datetime ────────────────────────────────────────────────────
	class CUIModule_ContentFormDatetime : public CUIModule_ContentFormEntity {
	protected:
		CUIExpression m_ValueExpression;
		std::string m_sMode;
		std::string m_sOnChangeEvent;
	public:
		static PUIModule_ContentFormDatetime makeFromXML(const pugi::xml_node& xmlNode, const std::string& sFormPath, PStateMachineData pStateMachineData);
		CUIModule_ContentFormDatetime(const std::string& sName, const std::string& sFormPath, CUIExpression Caption, CUIExpression Value, const std::string& sMode, const std::string& sOnChangeEvent, PStateMachineData pStateMachineData);
		virtual ~CUIModule_ContentFormDatetime();
		virtual std::string getTypeString() override;
		virtual void populateClientVariables(CParameterHandler* pClientVariableHandler) override;
		virtual void syncClientVariables(CParameterHandler* pClientVariableHandler) override;
		virtual void writeVariablesToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler) override;
		virtual void registerFrontendAttributes(PUIFrontendDefinitionModuleStore pStore) override;
	};

	// ── Phase 2: multiselect ─────────────────────────────────────────────────
	class CUIModule_ContentFormMultiselect : public CUIModule_ContentFormEntity {
	protected:
		CUIExpression m_ValueExpression;
		std::string m_sOnChangeEvent;
		std::vector<std::pair<std::string, int>> m_Items;
	public:
		static PUIModule_ContentFormMultiselect makeFromXML(const pugi::xml_node& xmlNode, const std::string& sFormPath, PStateMachineData pStateMachineData);
		CUIModule_ContentFormMultiselect(const std::string& sName, const std::string& sFormPath, CUIExpression Caption, CUIExpression Value, const std::string& sOnChangeEvent, PStateMachineData pStateMachineData, const std::vector<std::pair<std::string, int>>& items);
		virtual ~CUIModule_ContentFormMultiselect();
		virtual std::string getTypeString() override;
		virtual void populateClientVariables(CParameterHandler* pClientVariableHandler) override;
		virtual void syncClientVariables(CParameterHandler* pClientVariableHandler) override;
		virtual void writeVariablesToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler) override;
		virtual void registerFrontendAttributes(PUIFrontendDefinitionModuleStore pStore) override;
	};

	// ── Phase 2: calculated ──────────────────────────────────────────────────
	class CUIModule_ContentFormCalculated : public CUIModule_ContentFormEntity {
	protected:
		CUIExpression m_ValueExpression;
		CUIExpression m_UnitExpression;
		CUIExpression m_FormatExpression;
	public:
		static PUIModule_ContentFormCalculated makeFromXML(const pugi::xml_node& xmlNode, const std::string& sFormPath, PStateMachineData pStateMachineData);
		CUIModule_ContentFormCalculated(const std::string& sName, const std::string& sFormPath, CUIExpression Caption, CUIExpression Value, CUIExpression Unit, CUIExpression Format, PStateMachineData pStateMachineData);
		virtual ~CUIModule_ContentFormCalculated();
		virtual std::string getTypeString() override;
		virtual void populateClientVariables(CParameterHandler* pClientVariableHandler) override;
		virtual void syncClientVariables(CParameterHandler* pClientVariableHandler) override;
		virtual void writeVariablesToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler) override;
		virtual void registerFrontendAttributes(PUIFrontendDefinitionModuleStore pStore) override;
	};

	// ── Phase 2: section ─────────────────────────────────────────────────────
	class CUIModule_ContentFormSection : public CUIModule_ContentFormEntity {
	protected:
		CUIExpression m_DescriptionExpression;
		CUIExpression m_IconExpression;
		bool m_bCollapsible;
		bool m_bDefaultOpen;
		std::list<PUIModule_ContentFormEntity> m_Entities;
		std::map<std::string, PUIModule_ContentFormEntity> m_EntityNameMap;
		std::map<std::string, PUIModule_ContentFormEntity> m_EntityUUIDMap;
	public:
		static PUIModule_ContentFormSection makeFromXML(const pugi::xml_node& xmlNode, const std::string& sFormPath, PStateMachineData pStateMachineData);
		CUIModule_ContentFormSection(const std::string& sName, const std::string& sFormPath, CUIExpression Caption, CUIExpression Description, CUIExpression Icon, bool bCollapsible, bool bDefaultOpen, PStateMachineData pStateMachineData);
		virtual ~CUIModule_ContentFormSection();
		virtual std::string getTypeString() override;
		void addEntity(PUIModule_ContentFormEntity pEntity);
		bool hasEntityWithName(const std::string& sName);
		PUIModule_ContentFormEntity findEntityByUUID(const std::string& sUUID);
		std::list<PUIModule_ContentFormEntity>& getEntities();
		virtual void populateClientVariables(CParameterHandler* pClientVariableHandler) override;
		virtual void syncClientVariables(CParameterHandler* pClientVariableHandler) override;
		virtual void writeVariablesToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler) override;
		virtual void registerFrontendAttributes(PUIFrontendDefinitionModuleStore pStore) override;
	};

	// ── Phase 2: actionbar ───────────────────────────────────────────────────
	class CUIModule_ContentFormActionBar : public CUIModule_ContentFormEntity {
	protected:
		std::string m_sSubmitEvent;
		std::string m_sCancelEvent;
		std::string m_sValidateEvent;
		std::string m_sSubmitCaption;
		std::string m_sCancelCaption;
		bool m_bOptimistic;
	public:
		static PUIModule_ContentFormActionBar makeFromXML(const pugi::xml_node& xmlNode, const std::string& sFormPath, PStateMachineData pStateMachineData);
		CUIModule_ContentFormActionBar(const std::string& sName, const std::string& sFormPath, const std::string& sSubmitEvent, const std::string& sCancelEvent, const std::string& sValidateEvent, const std::string& sSubmitCaption, const std::string& sCancelCaption, bool bOptimistic, PStateMachineData pStateMachineData);
		virtual ~CUIModule_ContentFormActionBar();
		virtual std::string getTypeString() override;
		virtual void populateClientVariables(CParameterHandler* pClientVariableHandler) override;
		virtual void syncClientVariables(CParameterHandler* pClientVariableHandler) override;
		virtual void writeVariablesToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler) override;
		virtual void registerFrontendAttributes(PUIFrontendDefinitionModuleStore pStore) override;
	};

	class CUIModule_ContentForm : public CUIModule_ContentItem {
	protected:		
		std::list<PUIModule_ContentFormEntity> m_Entities;
		std::map<std::string, PUIModule_ContentFormEntity> m_EntityNameMap;
		std::map<std::string, PUIModule_ContentFormEntity> m_EntityUUIDMap;
		std::string m_sName;
		bool m_bVisible;

		PStateMachineData m_pStateMachineData;

	public:

		static PUIModule_ContentForm makeFromXML(const pugi::xml_node& xmlNode, const std::string& sItemName, const std::string& sModulePath, PUIModuleEnvironment pUIModuleEnvironment);

		CUIModule_ContentForm(PStateMachineData pStateMachineData, const std::string& sName, const std::string& sModulePath, bool bVisible);

		virtual ~CUIModule_ContentForm();

		virtual void addLegacyContentToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler, uint32_t nStateID) override;

		void addEntity(PUIModule_ContentFormEntity pEntity);
		
		std::string getName();

		bool IsVisible();

		bool hasEntityWithName(const std::string& sName);
		PUIModule_ContentFormEntity findEntityByName(const std::string& sName);
		PUIModule_ContentFormEntity findEntityByUUID(const std::string& sUUID);

		static void parseXmlFormNode(const pugi::xml_node& xmlNode, const std::string& sFormPath, PStateMachineData pStateMachineData, std::function<void(PUIModule_ContentFormEntity)> addEntityFn);

		std::list<PUIModule_ContentFormEntity> getEntities ();

		// Returns all UUIDs that could be contained in this Item
		virtual std::list <std::string> getReferenceUUIDs() override;

		virtual void populateClientVariables(CParameterHandler* pClientVariableHandler) override;

		virtual void setEventPayloadValue(const std::string& sEventName, const std::string& sPayloadUUID, const std::string& sPayloadValue, CParameterHandler* pClientVariableHandler) override;

		virtual std::string findElementPathByUUID(const std::string& sUUID) override;

		// New UI Frontend System
		virtual std::string getItemType() override;
		virtual void registerFrontendAttributes() override;

	};




}


#endif //__AMC_UI_MODULE_CONTENTITEM_BUTTONGROUP



