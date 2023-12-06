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

#define __AMCIMPL_UI_MODULE
#define __AMCIMPL_API_CONSTANTS

#include "amc_ui_module_contentitem_chart.hpp"
#include "libmc_interfaceexception.hpp"

#include "amc_api_constants.hpp"
#include "Common/common_utils.hpp"
#include "amc_parameterhandler.hpp"
#include "libmc_exceptiontypes.hpp"
#include "amc_ui_module.hpp"
#include "amc_resourcepackage.hpp"

#include "libmcdata_dynamic.hpp"

using namespace AMC;

PUIModule_ContentChart CUIModule_ContentChart::makeFromXML(const pugi::xml_node& xmlNode, const std::string& sItemName, const std::string& sModulePath, PUIModuleEnvironment pUIModuleEnvironment)
{
	LibMCAssertNotNull(pUIModuleEnvironment);

	auto pItem = std::make_shared <CUIModule_ContentChart>(AMCCommon::CUtils::createUUID (), sItemName, sModulePath, pUIModuleEnvironment->stateMachineData ());

	return pItem;
}


CUIModule_ContentChart::CUIModule_ContentChart(const std::string& sUUID, const std::string& sItemName, const std::string& sModulePath, PStateMachineData pStateMachineData)
	: CUIModule_ContentItem(sUUID, sItemName, sModulePath), m_pStateMachineData (pStateMachineData)
{
	LibMCAssertNotNull(m_pStateMachineData.get ());
}

CUIModule_ContentChart::~CUIModule_ContentChart()
{

}

void CUIModule_ContentChart::populateClientVariables(CParameterHandler* pClientVariableHandler)
{
	LibMCAssertNotNull(pClientVariableHandler);
	auto pGroup = pClientVariableHandler->addGroup(getItemPath(), "chart UI element");
	pGroup->addNewStringParameter("dataseries", "data series UUID", AMCCommon::CUtils::createEmptyUUID());
}

void CUIModule_ContentChart::addDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler)
{
	auto pGroup = pClientVariableHandler->findGroup(getItemPath(), true);

	object.addString(AMC_API_KEY_UI_ITEMTYPE, "chart");
	object.addString(AMC_API_KEY_UI_ITEMUUID, m_sUUID);
	object.addString(AMC_API_KEY_UI_DATASERIES, pGroup->getParameterValueByName ("dataseries"));

}

void CUIModule_ContentChart::addContentToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler, uint32_t nStateID)
{
	auto pGroup = pClientVariableHandler->findGroup(getItemPath(), true);

	object.addString(AMC_API_KEY_UI_ITEMTYPE, "chart");
	object.addString(AMC_API_KEY_UI_ITEMUUID, m_sUUID);
	object.addString(AMC_API_KEY_UI_DATASERIES, pGroup->getParameterValueByName("dataseries"));
}

