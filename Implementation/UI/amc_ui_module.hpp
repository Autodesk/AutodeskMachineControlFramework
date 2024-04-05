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

#include "header_protection.hpp"

#ifndef __AMCIMPL_UI_MODULE
#error this header is protected and should only be included in the corresponding implementation CPP files.
#endif

#include "Libraries/PugiXML/pugixml.hpp"
#include "Core/amc_jsonwriter.hpp"
#include "amc_ui_interfaces.hpp"
#include "amc_logger.hpp"

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

	public:
		CUIModuleEnvironment(PUISystemState pUISystemState, CUIModule_ContentRegistry* pContentRegistry, PResourcePackage pResourcePackage);
		virtual ~CUIModuleEnvironment();

		PStateMachineData stateMachineData();
		PResourcePackage resourcePackage();
		PMeshHandler meshHandler();
		PToolpathHandler toolpathHandler();
		PDataSeriesHandler dataSeriesHandler();
		LibMCData::PDataModel dataModel ();
		CUIModule_ContentRegistry* contentRegistry ();
		CLogger* getLogger();

	};

	class CUIModule {
	protected:
		std::string m_sName;
		std::string m_sUUID;
		
	public:

		CUIModule(const std::string & sName);	
		
		virtual ~CUIModule();

		std::string getName();

		virtual std::string getType() = 0;

		virtual void writeDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject & moduleObject, CParameterHandler* pClientVariableHandler) = 0;

		virtual PUIModuleItem findItem(const std::string& sUUID) = 0;

		virtual std::string getCaption() = 0;

		virtual std::string getUUID();

		virtual void populateItemMap (std::map<std::string, PUIModuleItem> & itemMap) = 0;

		static std::string getNameFromXML(pugi::xml_node& xmlNode);
		static std::string getTypeFromXML(pugi::xml_node& xmlNode);

		virtual void configurePostLoading() = 0;

		virtual void populateClientVariables(CParameterHandler* pParameterHandler) = 0;


	};

	
}


#endif //__AMC_UI_MODULE

