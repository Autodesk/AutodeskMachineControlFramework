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


	class CUIHandleEventResponse {
	private:
		uint32_t m_nErrorCode;
		std::string m_sErrorMessage;
		std::string m_sPageToActivate;
		std::string m_sDialogToShow;
		bool m_bCloseModalDialog;
	public:
		CUIHandleEventResponse(uint32_t nErrorCode,  const std::string& sErrorMessage, const std::string& sPageToActivate, bool bCloseModalDialog, const std::string& sDialogToShow);

		uint32_t getErrorCode();
		std::string getErrorMessage ();
		std::string getPageToActivate();
		std::string getDialogToShow();

		bool hasPageToActivate();
		bool hasDialogToShow();
		bool closeModalDialog();

	};

	class CUIHandler : public CUIModule_UIEventHandler {
	protected:

		std::mutex m_Mutex;
		std::map<std::string, uint32_t> m_Colors;

		std::string m_sAppName;
		std::string m_sCopyrightString;
		std::string m_sLogoUUID;
		double m_dLogoAspectRatio;

		PStateMachineData m_pStateMachineData;
		PStateSignalHandler m_pSignalHandler;
		PResourcePackage m_pCoreResourcePackage;
		PLogger m_pLogger;

		std::vector <PUIMenuItem> m_MenuItems;
		std::vector <PUIToolbarItem> m_ToolbarItems;

		std::map <std::string, PUIPage> m_Pages;
		std::map <std::string, PUIDialog> m_Dialogs;
		PUIPage m_pMainPage;

		LibMCUI::PWrapper m_pUIPluginWrapper;
		LibMCUI::PEventHandler m_pUIEventHandler;
		LibMCEnv::PWrapper m_pEnvironmentWrapper;

		void addMenuItem_Unsafe (const std::string& sID, const std::string& sIcon, const std::string& sCaption, const std::string& sTargetPage);
		void addToolbarItem_Unsafe (const std::string& sID, const std::string& sIcon, const std::string& sCaption, const std::string& sTargetPage);

		PUIPage addPage_Unsafe (const std::string& sName);
		PUIPage findPage(const std::string& sName);

		PUIDialog addDialog_Unsafe(const std::string& sName, const std::string& sTitle);
		PUIDialog findDialog(const std::string& sName);

	public:

		CUIHandler(PStateMachineData pStateMachineData, PStateSignalHandler pSignalHandler, LibMCEnv::PWrapper pEnvironmentWrapper, PLogger pLogger);
		
		virtual ~CUIHandler();
		
		std::string getAppName();
		std::string getCopyrightString();

		void writeConfigurationToJSON (CJSONWriter & writer, CParameterHandler* pClientVariableHandler);
		void writeStateToJSON(CJSONWriter& writer, CParameterHandler* pClientVariableHandler);

		void loadFromXML(pugi::xml_node& xmlNode, PResourcePackage pCoreResourcePackage, const std::string& sUILibraryPath, LibMCData::PBuildJobHandler pBuildJobHandler);

		PResourcePackage getCoreResourcePackage ();

		PUIModuleItem findModuleItem(const std::string & sUUID);
		
		PUIPage findPageOfModuleItem(const std::string& sUUID);

		CUIHandleEventResponse handleEvent(const std::string & sEventName, const std::string & sSenderUUID, const std::string& sContextUUID, const std::string& sFormValueJSON, PParameterHandler pClientVariableHandler);

		virtual void ensureUIEventExists(const std::string& sEventName) override;

		virtual void populateClientVariables(CParameterHandler * pClientVariableHandler);
	};
	
	typedef std::shared_ptr<CUIHandler> PUIHandler;
	
}


#endif //__AMC_UI_HANDLER

