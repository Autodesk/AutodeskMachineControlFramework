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

#ifndef __AMC_UI_MODULE_CONTENTLEAF
#define __AMC_UI_MODULE_CONTENTLEAF

#include "header_protection.hpp"

#ifndef __AMCIMPL_UI_MODULE
#error this header is protected and should only be included in the corresponding implementation CPP files.
#endif

#include "amc_ui_module.hpp"
#include "amc_ui_expression.hpp"

namespace AMC {

	amcDeclareDependingClass(CUIModule_ContentLeaf, PUIModule_ContentLeaf);
	amcDeclareDependingClass(CUIModule_ContentItem, PUIModule_ContentItem);
	amcDeclareDependingClass(CStateMachineData, PStateMachineData);

	class CUIModule_ContentLeaf : public CUIModule {
	protected:

		std::string m_sModuleType;
		std::string m_sCaption;

		CUIExpression m_VisibleExpression;
		PStateMachineData m_pStateMachineData;

		PUIModule_ContentItem m_pItem;

	public:

		/////////////////////////////////////////////////////////////////////////////////////
		// General module functionality
		/////////////////////////////////////////////////////////////////////////////////////

		CUIModule_ContentLeaf(pugi::xml_node& xmlNode, const std::string& sPath, PUIModuleEnvironment pUIModuleEnvironment);
		virtual ~CUIModule_ContentLeaf();

		static bool isSupportedModuleType(const std::string& sType);

		virtual std::string getType() override;
		virtual std::string getCaption() override;

		/////////////////////////////////////////////////////////////////////////////////////
		// Legacy UI System
		/////////////////////////////////////////////////////////////////////////////////////

		virtual void addContentToJSON(CJSONWriter& writer, CJSONWriterObject& moduleObject, CParameterHandler* pClientVariableHandler, uint32_t nStateID) override;
		virtual void writeLegacyDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& moduleObject, CParameterHandler* pClientVariableHandler) override;
		virtual void populateModuleMap(std::map<std::string, PUIModule>& moduleMap) override;
		virtual void populateLegacyItemMap(std::map<std::string, PUIModuleItem>& itemMap) override;
		virtual PUIModuleItem findLegacyItem(const std::string& sUUID) override;
		virtual void configureLegacyPostLoading() override;
		virtual void populateLegacyClientVariables(CParameterHandler* pParameterHandler) override;

		/////////////////////////////////////////////////////////////////////////////////////
		// New UI Frontend System
		/////////////////////////////////////////////////////////////////////////////////////

		void frontendWriteModuleStatusToJSON(CJSONWriter& writer, CJSONWriterObject& moduleObject, CUIFrontendState* pFrontendState, CStateMachineData* pStateMachineData) override;
		bool isVersion2FrontendModule() override;
	};

}

#endif //__AMC_UI_MODULE_CONTENTLEAF
