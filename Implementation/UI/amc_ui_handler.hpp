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


#ifndef __AMC_UI_HANDLER
#define __AMC_UI_HANDLER

#include "header_protection.hpp"
#include "header_pugixml.hpp"
#include "amc_resourcepackage.hpp"
#include "amc_ui_interfaces.hpp"
#include "amc_ui_expression.hpp"

#include <memory>
#include <vector>
#include <string>
#include <mutex>

namespace LibMCUI {
	amcDeclareDependingClass(CWrapper, PWrapper);
	amcDeclareDependingClass(CEventHandler, PEventHandler);
}


namespace LibMCData {
	amcDeclareDependingClass(CBuildJobHandler, PBuildJobHandler);
}

namespace LibMCEnv {
	amcDeclareDependingClass(CWrapper, PWrapper);

	namespace Impl {
		amcDeclareDependingClass(CUIEnvironment, PUIEnvironment);
	}
}

namespace AMC {

	amcDeclareDependingClass(CUIMenuItem, PUIMenuItem);
	amcDeclareDependingClass(CUIToolbarItem, PUIToolbarItem);
	amcDeclareDependingClass(CJSONWriter, PJSONWriter);
	amcDeclareDependingClass(CUIPage, PUIPage);
	amcDeclareDependingClass(CUIDialog, PUIDialog);
	amcDeclareDependingClass(CLogger, PLogger);
	amcDeclareDependingClass(CStateSignalHandler, PStateSignalHandler);
	amcDeclareDependingClass(CUIModule, PUIModule);
	amcDeclareDependingClass(CUIModuleItem, PUIModuleItem);	
	amcDeclareDependingClass(CAPIJSONRequest, PAPIJSONRequest);
	amcDeclareDependingClass(CStateMachineData, PStateMachineData);
	amcDeclareDependingClass(CParameterHandler, PParameterHandler);
	amcDeclareDependingClass(CUIClientAction, PUIClientAction);


	class CUIHandleEventResponse {
	private:
		uint32_t m_nErrorCode;
		std::string m_sErrorMessage;

		std::vector<PUIClientAction> m_clientActions;
		
	public:
		CUIHandleEventResponse(uint32_t nErrorCode,  const std::string& sErrorMessage, const std::vector<PUIClientAction> & clientActions);

		uint32_t getErrorCode();

		std::string getErrorMessage();

		std::vector<PUIClientAction> & getClientActions ();
	};

	class CUIHandler : public CUIModule_UIEventHandler {
	protected:

		std::mutex m_Mutex;
		std::map<std::string, uint32_t> m_Colors;

		std::string m_sAppName;
		std::string m_sCopyrightString;
		std::string m_sLogoUUID;
		double m_dLogoAspectRatio;

		CUIExpression m_LoginBackgroundUUID;
		CUIExpression m_LoginWelcomeMessage;

		std::string m_sMainPageName;
		std::string m_sTestOutputPath;

		PStateMachineData m_pStateMachineData;
		PStateSignalHandler m_pSignalHandler;
		PLogger m_pLogger;

		std::vector <PUIMenuItem> m_MenuItems;
		std::vector <PUIToolbarItem> m_ToolbarItems;
		std::map<std::string, PUIToolbarItem> m_ToolbarItemUUIDMap;
		std::string m_ToolbarLogoResourceName;

		std::map <std::string, PUIPage> m_Pages;
		std::map <std::string, PUIDialog> m_Dialogs;
		
		PResourcePackage m_pCoreResourcePackage; // Might be null!

		LibMCUI::PWrapper m_pUIPluginWrapper;
		LibMCUI::PEventHandler m_pUIEventHandler;
		LibMCEnv::PWrapper m_pEnvironmentWrapper;

		void addMenuItem_Unsafe (const std::string& sID, const std::string& sIcon, const std::string& sCaption, const std::string & sDescription, const std::string& sTargetPage, const std::string & sEventName);
		void addToolbarItem_Unsafe (const std::string& sID, const std::string& sIcon, const std::string& sCaption, const std::string& sTargetPage, const std::string& sEventName);

		PUIPage addPage_Unsafe (const std::string& sName);

		PUIDialog addDialog_Unsafe(const std::string& sName, const std::string& sTitle);

	public:

		CUIHandler(PStateMachineData pStateMachineData, PStateSignalHandler pSignalHandler, LibMCEnv::PWrapper pEnvironmentWrapper, PLogger pLogger, const std::string & sTestOutputPath);
		
		virtual ~CUIHandler();
		
		std::string getAppName();
		std::string getCopyrightString();

		void writeConfigurationToJSON (CJSONWriter & writer, CParameterHandler* pClientVariableHandler);
		void writeStateToJSON(CJSONWriter& writer, CParameterHandler* pClientVariableHandler);

		void loadFromXML(pugi::xml_node& xmlNode, const std::string& sUILibraryPath, LibMCData::PBuildJobHandler pBuildJobHandler);

		void setCoreResourcePackage(PResourcePackage pCoreResourcePackage);

		PResourcePackage getCoreResourcePackage ();

		PUIModuleItem findModuleItem(const std::string & sUUID);

		PUIPage findPageOfModuleItem(const std::string& sUUID);

		CUIHandleEventResponse handleEvent(const std::string & sEventName, const std::string & sSenderUUID, const std::string& sEventPayloadJSON, PParameterHandler pClientVariableHandler);

		virtual void ensureUIEventExists(const std::string& sEventName) override;

		virtual void populateClientVariables(CParameterHandler * pClientVariableHandler);

		PUIPage findPage(const std::string& sName);
		PUIDialog findDialog(const std::string& sName);
	};
	
	typedef std::shared_ptr<CUIHandler> PUIHandler;
	
}


#endif //__AMC_UI_HANDLER

