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


#ifndef __AMC_UI_MODULE_CONTENTITEM
#define __AMC_UI_MODULE_CONTENTITEM

#include "header_protection.hpp"

#ifndef __AMCIMPL_UI_MODULE
#error this header is protected and should only be included in the corresponding implementation CPP files.
#endif

#include "amc_jsonwriter.hpp"
#include "amc_ui_module_item.hpp"
#include "amc_ui_frontenddefinition.hpp"
#include "amc_ui_frontendstate.hpp"
#include "amc_ui_expression.hpp"

#define AMC_CONTENT_MAXENTRYCOUNT (1024 * 1024)

namespace AMC {

	amcDeclareDependingClass(CUIModule, PUIModule);
	amcDeclareDependingClass(CUIModuleItem, PUIModuleItem);
	amcDeclareDependingClass(CUIModule_ContentItem, PUIModule_ContentItem);
	amcDeclareDependingClass(CParameterHandler, PParameterHandler);
	amcDeclareDependingClass(CStateMachineData, PStateMachineData);

	class CUIModule_ContentItem : public CUIModuleItem {
	protected:		

		std::string m_sUUID;
		std::string m_sItemName;

		PUIFrontendDefinitionModuleStore m_pItemModuleStore;

	public:

		CUIModule_ContentItem(const std::string & sUUID, const std::string & sItemName, const std::string& sModulePath);
		
		virtual ~CUIModule_ContentItem();

		virtual std::string getUUID () override;

		virtual void addLegacyContentToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pLegacyClientVariableHandler, uint32_t nStateID) override;

		// Returns all UUIDs that could be contained in this Item
		virtual std::list <std::string> getReferenceUUIDs();

		virtual void populateClientVariables(CParameterHandler* pParameterHandler);

		/////////////////////////////////////////////////////////////////////////////////////
		// New UI Frontend System
		/////////////////////////////////////////////////////////////////////////////////////

		// Initialize the v2 frontend module store for this item. Called after construction.
		void initFrontendModuleStore(CUIFrontendDefinition* pFrontendDefinition);

		// Override in subclasses to register v2 attributes with the module store.
		virtual void registerFrontendAttributes();

		// Returns the item type string for the v2 frontend (e.g. "paragraph", "image").
		virtual std::string getItemType();

		// Write this item as a submodule in the v2 frontend JSON output.
		virtual void frontendWriteItemToJSON(CJSONWriter& writer, CJSONWriterObject& itemObject, CUIFrontendState* pFrontendState, CStateMachineData* pStateMachineData);

		// Helpers for registering v2 attributes
		PUIFrontendDefinitionAttribute registerItemStringAttribute(const std::string& sName, const CUIExpression& expression);
		PUIFrontendDefinitionAttribute registerItemBoolAttribute(const std::string& sName, const CUIExpression& expression);
		PUIFrontendDefinitionAttribute registerItemIntegerAttribute(const std::string& sName, const CUIExpression& expression);
		PUIFrontendDefinitionAttribute registerItemNumberAttribute(const std::string& sName, const CUIExpression& expression);
		PUIFrontendDefinitionAttribute registerItemUUIDAttribute(const std::string& sName, const CUIExpression& expression);

	};


}


#endif //__AMC_UI_MODULE_CONTENTITEM

