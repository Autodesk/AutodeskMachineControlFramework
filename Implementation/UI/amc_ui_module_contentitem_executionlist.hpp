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


#ifndef __AMC_UI_MODULE_CONTENTITEM_EXECUTIONLIST
#define __AMC_UI_MODULE_CONTENTITEM_EXECUTIONLIST

#include "header_protection.hpp"

#ifndef __AMCIMPL_UI_MODULE
#error this header is protected and should only be included in the corresponding implementation CPP files.
#endif

#include "amc_ui_module_contentitem.hpp"
#include "amc_ui_expression.hpp"
#include "pugixml.hpp"

namespace LibMCData {
	amcDeclareDependingClass(CDataModel, PDataModel);
}

namespace AMC {

	amcDeclareDependingClass(CUIModule_ContentExecutionList, PUIModule_ContentExecutionList);
	amcDeclareDependingClass(CStateMachineData, PStateMachineData);
	amcDeclareDependingClass(CUIModuleEnvironment, PUIModuleEnvironment);
	amcDeclareDependingClass(CUIModule_ContentExecutionListButton, PUIModule_ContentExecutionListButton);

	class CUIModule_ContentExecutionListButton {
	private:
		std::string m_sUUID;
		std::string m_sButtonName;
		CUIExpression m_CaptionExpression;
		std::string m_sEvent;

	public:

		CUIModule_ContentExecutionListButton(const std::string& sButtonName, CUIExpression captionExpression, const std::string& sEvent);

		virtual ~CUIModule_ContentExecutionListButton();

		std::string getButtonName();

		std::string getUUID();

		CUIExpression getCaptionExpression();

		std::string getEvent();
	};


	class CUIModule_ContentExecutionList : public CUIModule_ContentItem {
	protected:

		std::string m_sItemName;

		CUIExpression m_LoadingText;
		CUIExpression m_ExecutionNameCaption;
		CUIExpression m_ExecutionThumbnailCaption;
		CUIExpression m_ExecutionTimestampCaption;

		std::string m_sSelectEvent;
		std::string m_sSelectedExecutionFieldUUID;
		std::string m_sSelectedButtonFieldUUID;
		std::string m_sDefaultThumbnailResourceUUID;

		std::vector<PUIModule_ContentExecutionListButton> m_Buttons;
		std::map<std::string, PUIModule_ContentExecutionListButton> m_ButtonNameMap;
		std::map<std::string, PUIModule_ContentExecutionListButton> m_ButtonUUIDMap;

		uint32_t m_nEntriesPerPage;

		LibMCData::PDataModel m_pDataModel;

		PStateMachineData m_pStateMachineData;

		void writeHeadersToJSON(CJSONWriter& writer, CJSONWriterObject& object);

		void writeButtonsToJSON(CJSONWriter& writer, CJSONWriterObject& object);

	public:

		static PUIModule_ContentExecutionList makeFromXML(const pugi::xml_node& xmlNode, const std::string& sItemName, const std::string& sModulePath, PUIModuleEnvironment pUIModuleEnvironment);

		CUIModule_ContentExecutionList(const CUIExpression& loadingText, const uint32_t nEntriesPerPage, const std::string& sSelectEvent, LibMCData::PDataModel pDataModel, const std::string& sItemName, const std::string& sModulePath, const std::string sDefaultThumbnailResourceUUID, PStateMachineData pStateMachineData);

		virtual ~CUIModule_ContentExecutionList();

		void addDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler) override;

		void addContentToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler, uint32_t nStateID) override;

		virtual void populateClientVariables(CParameterHandler* pClientVariableHandler) override;

		virtual void setEventPayloadValue(const std::string& sEventName, const std::string& sPayloadUUID, const std::string& sPayloadValue, CParameterHandler* pClientVariableHandler) override;

		virtual std::string findElementPathByUUID(const std::string& sUUID) override;

		virtual std::list <std::string> getReferenceUUIDs() override;

		void addButton(const std::string& sButtonName, CUIExpression captionExpression, const std::string& sEvent);

	};


}


#endif //__AMC_UI_MODULE_CONTENTITEM_EXECUTIONLIST

