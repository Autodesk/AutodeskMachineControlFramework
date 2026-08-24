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


#ifndef __AMC_UI_MODULE_CONTENTITEM_STATEMACHINEGRAPH
#define __AMC_UI_MODULE_CONTENTITEM_STATEMACHINEGRAPH

#include "header_protection.hpp"

#ifndef __AMCIMPL_UI_MODULE
#error this header is protected and should only be included in the corresponding implementation CPP files.
#endif

#include "amc_ui_module_contentitem.hpp"
#include "pugixml.hpp"
#include "amc_ui_expression.hpp"
#include "amc_statemachinedata.hpp"

#include <vector>

namespace AMC {

	amcDeclareDependingClass(CUIModule_ContentStateMachineGraph, PUIModule_ContentStateMachineGraph);
	amcDeclareDependingClass(CUIModuleEnvironment, PUIModuleEnvironment);

	struct sStateMachineGraphNodePosition {
		std::string sStateName;
		double dX;
		double dY;
	};

	class CUIModule_ContentStateMachineGraph : public CUIModule_ContentItem {
	protected:
		std::string m_sStateMachineName;
		PStateMachineData m_pStateMachineData;
		std::vector<sStateMachineGraphNodePosition> m_NodePositions;

	public:

		static PUIModule_ContentStateMachineGraph makeFromXML(const pugi::xml_node& xmlNode, const std::string& sItemName, const std::string& sModulePath, PUIModuleEnvironment pUIModuleEnvironment);

		CUIModule_ContentStateMachineGraph(const std::string& sUUID, const std::string& sItemName, const std::string& sModulePath, const std::string& sStateMachineName, PStateMachineData pStateMachineData, std::vector<sStateMachineGraphNodePosition> nodePositions);

		virtual ~CUIModule_ContentStateMachineGraph();

		void addLegacyContentToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler, uint32_t nStateID) override;

		virtual void configurePostLoading() override;

		virtual void populateClientVariables(CParameterHandler* pClientVariableHandler) override;

		virtual std::string findElementPathByUUID(const std::string& sUUID) override;

		virtual std::string getItemType() override;
		virtual void registerFrontendAttributes() override;
		virtual void frontendWriteItemToJSON(CJSONWriter& writer, CJSONWriterObject& itemObject, CUIFrontendState* pFrontendState, CStateMachineData* pStateMachineData) override;
	};

}


#endif //__AMC_UI_MODULE_CONTENTITEM_STATEMACHINEGRAPH
