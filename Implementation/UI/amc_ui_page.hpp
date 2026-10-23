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


#ifndef __AMC_UI_PAGE
#define __AMC_UI_PAGE

#include <cstdint>
#include "header_protection.hpp"

#ifndef __AMCIMPL_UI_PAGE
#error this header is protected and should only be included in the corresponding implementation CPP files.
#endif

#include "Core/amc_jsonwriter.hpp"

#include <vector>
#include <map>

#include "Libraries/PugiXML/pugixml.hpp"
#include "amc_ui_interfaces.hpp"
#include "amc_ui_frontendstate.hpp"

namespace AMC {

	amcDeclareDependingClass(CUIModule, PUIModule);
	amcDeclareDependingClass(CUIPage, PUIPage);	
	amcDeclareDependingClass(CParameterHandler, PParameterHandler);
	amcDeclareDependingClass(CUIModuleItem, PUIModuleItem);


	class CUIPage : public CUIModule_ContentRegistry {
	protected:
		std::string m_sName;
		std::string m_sUUID;
		std::string m_sShowEvent;

		std::vector<PUIModule> m_Modules;
		std::map<std::string, PUIModule> m_ModuleMapOfPage;
		
		CUIModule_UIEventHandler* m_pUIEventHandler;



		/////////////////////////////////////////////////////////////////////////////////////
		// Legacy UI System
		/////////////////////////////////////////////////////////////////////////////////////
		std::map<std::string, PUIModuleItem> m_ItemMapOfPage;
		std::map<std::string, std::string> m_FormNameMap;

		/////////////////////////////////////////////////////////////////////////////////////
		// New UI Frontend System
		/////////////////////////////////////////////////////////////////////////////////////
		CUIExpression m_Icon;
		CUIExpression m_Caption;
		CUIExpression m_Description;
		CUIExpression m_Visible;
		uint32_t m_nGridColumns;
		uint32_t m_nGridRows;

	public:

		CUIPage(const std::string & sName, CUIModule_UIEventHandler* pUIEventHandler, const CUIExpression & icon, const CUIExpression & caption, const CUIExpression & description, const std::string& sShowEvent);
		
		virtual ~CUIPage();

		std::string getName();

		std::string getUUID();

		std::string getShowEvent();

		void addModule (PUIModule pModule);

		uint32_t getModuleCount();
		PUIModule getModule (const uint32_t nIndex);

		virtual void configurePostLoading();

		virtual void ensureUIEventExists(const std::string& sEventName) override;

		/////////////////////////////////////////////////////////////////////////////////////
		// Legacy UI System
		/////////////////////////////////////////////////////////////////////////////////////
		virtual PUIModule findModuleByUUID(const std::string& sUUID) override;
		virtual PUIModuleItem findModuleItemByUUID(const std::string& sUUID) override;
		virtual void registerFormName(const std::string& sFormUUID, const std::string& sFormName) override;
		virtual std::string findFormUUIDByName(const std::string& sFormName) override;
		virtual void populateClientVariables(CParameterHandler* pParameterHandler);

		/////////////////////////////////////////////////////////////////////////////////////
		// New UI Frontend System
		/////////////////////////////////////////////////////////////////////////////////////
		void frontendWritePageStatusToJSON(CJSONWriter& writer, CJSONWriterObject& pageObject, CUIFrontendState* pFrontendState, CStateMachineData * pStateMachineData, bool bWriteModules, const sUIFrontendBuildEpoch& epoch);

		void setVisibleExpression(const CUIExpression& visibleExpression);

		bool isVisible(CUIFrontendState* pFrontendState, CStateMachineData* pStateMachineData);

		// Collects the session references of the page level expressions (icon, caption, description, visible).
		void collectSessionReferences(std::vector<std::string>& references);
										
	};
		
}


#endif //__AMC_UI_PAGE

