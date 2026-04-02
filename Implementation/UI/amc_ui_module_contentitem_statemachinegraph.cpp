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

#include "amc_ui_module_contentitem_statemachinegraph.hpp"
#include "amc_ui_frontendstate.hpp"
#include "libmc_interfaceexception.hpp"
#include "amc_api_constants.hpp"
#include "Common/common_utils.hpp"
#include "amc_parameterhandler.hpp"
#include "libmc_exceptiontypes.hpp"
#include "amc_ui_module.hpp"
#include "amc_statemachinedata.hpp"

#include <sstream>

using namespace AMC;

PUIModule_ContentStateMachineGraph CUIModule_ContentStateMachineGraph::makeFromXML(const pugi::xml_node& xmlNode, const std::string& sItemName, const std::string& sModulePath, PUIModuleEnvironment pUIModuleEnvironment)
{
	LibMCAssertNotNull(pUIModuleEnvironment);

	auto statemachineAttrib = xmlNode.attribute("statemachine");
	if (statemachineAttrib.empty())
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM, "statemachinegraph element requires a statemachine attribute");

	std::string sStateMachineName = statemachineAttrib.as_string();

	std::vector<sStateMachineGraphNodePosition> nodePositions;
	for (pugi::xml_node nodeChild : xmlNode.children("node")) {
		auto stateAttrib = nodeChild.attribute("state");
		auto xAttrib = nodeChild.attribute("x");
		auto yAttrib = nodeChild.attribute("y");

		if (stateAttrib.empty())
			continue;

		sStateMachineGraphNodePosition pos;
		pos.sStateName = stateAttrib.as_string();
		pos.dX = xAttrib.empty() ? -1.0 : xAttrib.as_double(-1.0);
		pos.dY = yAttrib.empty() ? -1.0 : yAttrib.as_double(-1.0);
		nodePositions.push_back(pos);
	}

	std::string sUUID = AMCCommon::CUtils::createUUID();

	auto pStateMachineData = pUIModuleEnvironment->stateMachineData();

	auto pItem = std::make_shared<CUIModule_ContentStateMachineGraph>(sUUID, sItemName, sModulePath, sStateMachineName, pStateMachineData, nodePositions);
	return pItem;
}


CUIModule_ContentStateMachineGraph::CUIModule_ContentStateMachineGraph(const std::string& sUUID, const std::string& sItemName, const std::string& sModulePath, const std::string& sStateMachineName, PStateMachineData pStateMachineData, std::vector<sStateMachineGraphNodePosition> nodePositions)
	: CUIModule_ContentItem(sUUID, sItemName, sModulePath),
	  m_sStateMachineName(sStateMachineName),
	  m_pStateMachineData(pStateMachineData),
	  m_NodePositions(nodePositions)
{
	LibMCAssertNotNull(pStateMachineData)
}

CUIModule_ContentStateMachineGraph::~CUIModule_ContentStateMachineGraph()
{
}


void CUIModule_ContentStateMachineGraph::addLegacyContentToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler, uint32_t nStateID)
{
	object.addString(AMC_API_KEY_UI_ITEMTYPE, "statemachinegraph");
	object.addString(AMC_API_KEY_UI_ITEMUUID, m_sUUID);
}


void CUIModule_ContentStateMachineGraph::configurePostLoading()
{
}


void CUIModule_ContentStateMachineGraph::populateClientVariables(CParameterHandler* pClientVariableHandler)
{
}


std::string CUIModule_ContentStateMachineGraph::findElementPathByUUID(const std::string& sUUID)
{
	if (sUUID == m_sUUID)
		return getItemPath();
	return "";
}

std::string CUIModule_ContentStateMachineGraph::getItemType()
{
	return "statemachinegraph";
}

void CUIModule_ContentStateMachineGraph::registerFrontendAttributes()
{
}

void CUIModule_ContentStateMachineGraph::frontendWriteItemToJSON(CJSONWriter& writer, CJSONWriterObject& itemObject, CUIFrontendState* pFrontendState, CStateMachineData* pStateMachineData)
{
	if (pFrontendState == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	itemObject.addString("moduletype", getItemType());
	itemObject.addString("uuid", m_sUUID);

	CJSONWriterObject attributesObject(writer);

	attributesObject.addString("statemachine", m_sStateMachineName);

	std::string sActiveState;
	try {
		sActiveState = pStateMachineData->getInstanceStateName(m_sStateMachineName);
	}
	catch (...) {
		sActiveState = "";
	}
	attributesObject.addString("activestate", sActiveState);

	if (!m_NodePositions.empty()) {
		std::stringstream posStream;
		for (size_t i = 0; i < m_NodePositions.size(); i++) {
			if (i > 0)
				posStream << ";";
			posStream << m_NodePositions[i].sStateName << ":" << m_NodePositions[i].dX << ":" << m_NodePositions[i].dY;
		}
		attributesObject.addString("nodepositions", posStream.str());
	}

	itemObject.addObject("attributes", attributesObject);
}
