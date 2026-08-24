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

#ifndef __AMC_UI_MODULE_CONTENTITEM_STATUSLIST
#define __AMC_UI_MODULE_CONTENTITEM_STATUSLIST

#include "header_protection.hpp"

#ifndef __AMCIMPL_UI_MODULE
#error this header is protected and should only be included in the corresponding implementation CPP files.
#endif

#include "amc_ui_module_contentitem.hpp"
#include "amc_ui_expression.hpp"

#include "pugixml.hpp"

namespace AMC {

	amcDeclareDependingClass(CStateMachineData, PStateMachineData);
	amcDeclareDependingClass(CUIModule_ContentStatusList, PUIModule_ContentStatusList);
	amcDeclareDependingClass(CUIModule_ContentStatusListRow, PUIModule_ContentStatusListRow);
	amcDeclareDependingClass(CUIModuleEnvironment, PUIModuleEnvironment);

	// A single value->text+severity mapping rule for a "map" row.
	struct sStatusListMapEntry {
		std::string m_sValue;
		std::string m_sText;
		std::string m_sSeverity;
	};

	class CUIModule_ContentStatusListRow {
	protected:
		std::string m_sLabel;
		CUIExpression m_ValueExpression;
		// display type: text | number | onoff | yesno | openclosed | leftright | map
		std::string m_sDisplay;
		uint32_t m_nDecimals;
		std::string m_sUnit;
		std::vector<sStatusListMapEntry> m_Maps;
	public:
		CUIModule_ContentStatusListRow(const std::string& sLabel, const CUIExpression& valueExpression, const std::string& sDisplay, uint32_t nDecimals, const std::string& sUnit);
		~CUIModule_ContentStatusListRow();

		void addMap(const std::string& sValue, const std::string& sText, const std::string& sSeverity);

		std::string getLabel();
		std::string getDisplay();
		uint32_t getDecimals();
		std::string getUnit();
		const std::vector<sStatusListMapEntry>& getMaps();

		// Resolves the current live value of this row against the state machine data.
		std::string evaluateValue(CStateMachineData* pStateMachineData);
	};


	class CUIModule_ContentStatusList : public CUIModule_ContentItem {
	protected:

		std::vector<PUIModule_ContentStatusListRow> m_Rows;

		std::string m_sTitle;
		std::string m_sLoadingText;

		PStateMachineData m_pStateMachineData;

		void writeRowsToJSON(CJSONWriter& writer, CJSONWriterArray& rowArray);

	public:

		static PUIModule_ContentStatusList makeFromXML(const pugi::xml_node& xmlNode, const std::string& sItemName, const std::string& sModulePath, PUIModuleEnvironment pUIModuleEnvironment);

		CUIModule_ContentStatusList(const std::string& sTitle, const std::string& sLoadingText, PStateMachineData pStateMachineData, const std::string& sItemName, const std::string& sModulePath);

		virtual ~CUIModule_ContentStatusList();

		void loadFromXML(const pugi::xml_node& xmlNode);

		void addRow(PUIModule_ContentStatusListRow pRow);
		uint32_t getRowCount();

		virtual void addLegacyContentToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler, uint32_t nStateID) override;

		// New UI Frontend System
		virtual std::string getItemType() override;
		virtual void registerFrontendAttributes() override;
		virtual void frontendWriteItemToJSON(CJSONWriter& writer, CJSONWriterObject& itemObject, CUIFrontendState* pFrontendState, CStateMachineData* pStateMachineData) override;

	};


}


#endif //__AMC_UI_MODULE_CONTENTITEM_STATUSLIST
