/*++

Copyright (C) 2026 Autodesk Inc.

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

#ifndef __AMC_UI_MODULE_CONTENTITEM_TOGGLEPANEL
#define __AMC_UI_MODULE_CONTENTITEM_TOGGLEPANEL

#include "header_protection.hpp"

#ifndef __AMCIMPL_UI_MODULE
#error this header is protected and should only be included in the corresponding implementation CPP files.
#endif

#include "amc_ui_module_contentitem.hpp"
#include "amc_ui_expression.hpp"

#include "pugixml.hpp"

namespace AMC {

	amcDeclareDependingClass(CStateMachineData, PStateMachineData);
	amcDeclareDependingClass(CUIModule_ContentTogglePanel, PUIModule_ContentTogglePanel);
	amcDeclareDependingClass(CUIModule_ContentToggle, PUIModule_ContentToggle);
	amcDeclareDependingClass(CUIModuleEnvironment, PUIModuleEnvironment);

	class CUIModule_ContentToggle {
	protected:
		std::string m_sUUID;
		std::string m_sLabel;
		CUIExpression m_ValueExpression;
		std::string m_sEvent;
		// mode: onoff | yesno | openclosed | leftright
		std::string m_sMode;
	public:
		CUIModule_ContentToggle(const std::string& sLabel, const CUIExpression& valueExpression, const std::string& sEvent, const std::string& sMode);
		~CUIModule_ContentToggle();

		std::string getUUID();
		std::string getLabel();
		std::string getEvent();
		std::string getMode();

		std::string evaluateValue(CStateMachineData* pStateMachineData);
	};


	class CUIModule_ContentTogglePanel : public CUIModule_ContentItem {
	protected:

		std::vector<PUIModule_ContentToggle> m_Toggles;

		uint32_t m_nColumns;

		PStateMachineData m_pStateMachineData;

		void writeTogglesToJSON(CJSONWriter& writer, CJSONWriterArray& toggleArray);

	public:

		static PUIModule_ContentTogglePanel makeFromXML(const pugi::xml_node& xmlNode, const std::string& sItemName, const std::string& sModulePath, PUIModuleEnvironment pUIModuleEnvironment);

		CUIModule_ContentTogglePanel(uint32_t nColumns, PStateMachineData pStateMachineData, const std::string& sItemName, const std::string& sModulePath);

		virtual ~CUIModule_ContentTogglePanel();

		void loadFromXML(const pugi::xml_node& xmlNode);

		void addToggle(PUIModule_ContentToggle pToggle);
		uint32_t getToggleCount();

		// Returns all UUIDs that could be contained in this Item (panel + toggles).
		virtual std::list<std::string> getReferenceUUIDs() override;

		virtual std::string findElementPathByUUID(const std::string& sUUID) override;

		virtual void addLegacyContentToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler, uint32_t nStateID) override;

		// New UI Frontend System
		virtual std::string getItemType() override;
		virtual void registerFrontendAttributes() override;
		virtual void frontendWriteItemToJSON(CJSONWriter& writer, CJSONWriterObject& itemObject, CUIFrontendState* pFrontendState, CStateMachineData* pStateMachineData) override;

	};


}


#endif //__AMC_UI_MODULE_CONTENTITEM_TOGGLEPANEL
