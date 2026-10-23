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


#ifndef __AMC_UI_MODULE
#define __AMC_UI_MODULE

#include <cstdint>
#include "header_protection.hpp"

#ifndef __AMCIMPL_UI_MODULE
#error this header is protected and should only be included in the corresponding implementation CPP files.
#endif

#include "Libraries/PugiXML/pugixml.hpp"
#include "Core/amc_jsonwriter.hpp"
#include "amc_ui_interfaces.hpp"
#include "amc_logger.hpp"
#include "amc_ui_frontendstate.hpp"

namespace LibMCData {
	amcDeclareDependingClass(CDataModel, PDataModel);
}


namespace AMC {

	amcDeclareDependingClass(CUIModule, PUIModule);
	amcDeclareDependingClass(CUIModuleItem, PUIModuleItem);
	amcDeclareDependingClass(CUIModuleFactory, PUIModuleFactory);
	amcDeclareDependingClass(CStateMachineData, PStateMachineData);
	amcDeclareDependingClass(CResourcePackage, PResourcePackage);
	amcDeclareDependingClass(CUIModuleEnvironment, PUIModuleEnvironment);
	amcDeclareDependingClass(CParameterHandler, PParameterHandler);
	amcDeclareDependingClass(CMeshHandler, PMeshHandler);
	amcDeclareDependingClass(CToolpathHandler, PToolpathHandler);
	amcDeclareDependingClass(CDataSeriesHandler, PDataSeriesHandler);
	amcDeclareDependingClass(CUISystemState, PUISystemState);

	class CUIModuleEnvironment {
	private:
		PUISystemState m_pUISystemState;
		PResourcePackage m_pResourcePackage;
		CUIModule_ContentRegistry* m_pContentRegistry;
		CUIFrontendDefinition* m_pFrontendDefinition;

	public:
		CUIModuleEnvironment(PUISystemState pUISystemState, CUIModule_ContentRegistry* pContentRegistry, PResourcePackage pResourcePackage, CUIFrontendDefinition* pFrontendDefinition);
		virtual ~CUIModuleEnvironment();

		PStateMachineData stateMachineData();
		PResourcePackage resourcePackage();
		PMeshHandler meshHandler();
		PToolpathHandler toolpathHandler();
		PDataSeriesHandler dataSeriesHandler();
		LibMCData::PDataModel dataModel ();
		CUIModule_ContentRegistry* contentRegistry ();
		CLogger* getLogger();

		AMCCommon::PChrono getGlobalChrono();

		CUIFrontendDefinition* getFrontendDefinition ();

	};

	class CUIModule {
	protected:
		std::string m_sName;
		std::string m_sUUID;

		uint32_t m_nGridColumn;
		uint32_t m_nGridRow;
		uint32_t m_nGridColumnSpan;
		uint32_t m_nGridRowSpan;

		std::string m_sModulePath;

		PUIFrontendDefinitionModuleStore m_pModuleStore;
		
	public:

		/////////////////////////////////////////////////////////////////////////////////////
		// General module functionality
		/////////////////////////////////////////////////////////////////////////////////////

		CUIModule(const std::string & sName, const std::string & sModuleType, const std::string& sParentPath, CUIFrontendDefinition * pFrontendDefinition);
		
		virtual ~CUIModule();

		std::string getName();

		virtual std::string getType() = 0;

		virtual std::string getCaption() = 0;

		virtual std::string getUUID();

		virtual void populateModuleMap(std::map<std::string, PUIModule>& moduleMap) = 0;

		static std::string getNameFromXML(pugi::xml_node& xmlNode);

		static std::string getTypeFromXML(pugi::xml_node& xmlNode);

		/////////////////////////////////////////////////////////////////////////////////////
		// Legacy UI System
		/////////////////////////////////////////////////////////////////////////////////////

		virtual void populateLegacyItemMap(std::map<std::string, PUIModuleItem>& itemMap);

		virtual void populateLegacyClientVariables(CParameterHandler* pParameterHandler);

		virtual PUIModuleItem findLegacyItem(const std::string& sUUID);

		virtual void configureLegacyPostLoading();

		/////////////////////////////////////////////////////////////////////////////////////
		// New UI Frontend System
		/////////////////////////////////////////////////////////////////////////////////////

		// Writes the module store tree of this module.
		void frontendWriteModuleStatusToJSON(CJSONWriter& writer, CJSONWriterObject& moduleObject, CUIFrontendState* pFrontendState, CStateMachineData *pStateMachineData, const sUIFrontendBuildEpoch& epoch);

		virtual bool isVersion2FrontendModule();

		PUIFrontendDefinitionModuleStore getFrontendModuleStore();

		std::string getModulePath();

		void setGridSpan(uint32_t nGridColumn, uint32_t nGridRow, uint32_t nGridColumnSpan, uint32_t nGridRowSpan);
		void getGridSpan(uint32_t & nGridColumn, uint32_t & nGridRow, uint32_t & nGridColumnSpan, uint32_t & nGridRowSpan);

		PUIFrontendDefinitionAttribute registerUUIDAttribute(const std::string& sAttributeName, const CUIExpression & expression);
		PUIFrontendDefinitionAttribute registerIntegerAttribute(const std::string& sAttributeName, const CUIExpression& expression);
		PUIFrontendDefinitionAttribute registerNumberAttribute(const std::string& sAttributeName, const CUIExpression& expression);
		PUIFrontendDefinitionAttribute registerStringAttribute(const std::string& sAttributeName, const CUIExpression& expression);
		PUIFrontendDefinitionAttribute registerBoolAttribute(const std::string& sAttributeName, const CUIExpression& expression);
		PUIFrontendDefinitionAttribute registerProviderAttribute(const std::string& sAttributeName, eUIFrontendDefinitionAttributeType attributeType, eUIFrontendDefinitionAttributeScope scope, UIFrontendDefinitionProvider provider);
	};

	
}


#endif //__AMC_UI_MODULE

