/*++

Copyright (C) 2026 Autodesk Inc.

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

#include "amc_ui_module_contentitem_partlist.hpp"
#include "libmc_interfaceexception.hpp"

#include "amc_api_constants.hpp"
#include "Common/common_utils.hpp"
#include "amc_parameterhandler.hpp"
#include "amc_ui_module.hpp"
#include "amc_ui_frontendstate.hpp"
#include "amc_toolpathhandler.hpp"
#include "libmc_exceptiontypes.hpp"
#include "libmcdata_dynamic.hpp"

using namespace AMC;


PUIModule_ContentPartList CUIModule_ContentPartList::makeFromXML(const pugi::xml_node& xmlNode, const std::string& sItemName, const std::string& sModulePath, PUIModuleEnvironment pUIModuleEnvironment)
{
	LibMCAssertNotNull(pUIModuleEnvironment);

	auto buildUUIDAttrib = xmlNode.attribute("builduuid");
	CUIExpression buildUUIDExpression(xmlNode, "builduuid", std::string("00000000-0000-0000-0000-000000000000"));

	CUIExpression loadingTextExpression(xmlNode, "loadingtext", std::string("Loading build details..."));
	CUIExpression showDetailsExpression(xmlNode, "showdetails", std::string("true"));

	std::string sSelectEvent = xmlNode.attribute("selectevent").as_string();
	if (!sSelectEvent.empty()) {
		if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sSelectEvent))
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDEVENTNAME, sSelectEvent);
	}

	return std::make_shared<CUIModule_ContentPartList>(
		buildUUIDAttrib.as_string(),
		buildUUIDExpression,
		loadingTextExpression,
		showDetailsExpression,
		sSelectEvent,
		sItemName,
		sModulePath,
		pUIModuleEnvironment
	);
}

CUIModule_ContentPartList::CUIModule_ContentPartList(
	const std::string& sBuildUUID, const CUIExpression& buildUUIDExpression,
	const CUIExpression& loadingText, const CUIExpression& showDetails, const std::string& sSelectEvent,
	const std::string& sItemName, const std::string& sModulePath, PUIModuleEnvironment pUIModuleEnvironment)
	: CUIModule_ContentItem(AMCCommon::CUtils::createUUID(), sItemName, sModulePath),
	  m_sBuildUUID(sBuildUUID),
	  m_BuildUUIDExpression(buildUUIDExpression),
	  m_LoadingText(loadingText),
	  m_ShowDetails(showDetails),
	  m_sSelectEvent(sSelectEvent),
	  m_sSelectedPartFieldUUID(AMCCommon::CUtils::createUUID()),
	  m_pUIModuleEnvironment(pUIModuleEnvironment)
{
	LibMCAssertNotNull(pUIModuleEnvironment);
}

CUIModule_ContentPartList::~CUIModule_ContentPartList()
{
}

void CUIModule_ContentPartList::populateClientVariables(CParameterHandler* pClientVariableHandler)
{
	LibMCAssertNotNull(pClientVariableHandler);
	auto pGroup = pClientVariableHandler->addGroup(getItemPath(), "part list UI element");
	pGroup->addNewUUIDParameter("selecteduuid", "selected part UUID", AMCCommon::CUtils::createEmptyUUID());
}

void CUIModule_ContentPartList::setEventPayloadValue(const std::string& sEventName, const std::string& sPayloadUUID, const std::string& sPayloadValue, CParameterHandler* pClientVariableHandler)
{
	LibMCAssertNotNull(pClientVariableHandler);
	if (AMCCommon::CUtils::normalizeUUIDString(sPayloadUUID) == m_sSelectedPartFieldUUID) {
		auto pGroup = pClientVariableHandler->findGroup(getItemPath(), true);
		pGroup->setParameterValueByName("selecteduuid", AMCCommon::CUtils::normalizeUUIDString(sPayloadValue));
	}
}

std::string CUIModule_ContentPartList::findElementPathByUUID(const std::string& sUUID)
{
	if ((sUUID == m_sSelectedPartFieldUUID) || (sUUID == getUUID()))
		return getItemPath();

	return "";
}

std::list <std::string> CUIModule_ContentPartList::getReferenceUUIDs()
{
	std::list <std::string> sUUIDList;
	sUUIDList.push_back(m_sSelectedPartFieldUUID);
	sUUIDList.push_back(getUUID());

	return sUUIDList;
}

std::string CUIModule_ContentPartList::getItemType()
{
	return "partlist";
}

void CUIModule_ContentPartList::registerFrontendAttributes()
{
	registerItemUUIDAttribute("builduuid", m_BuildUUIDExpression);
	registerItemStringAttribute("loadingtext", m_LoadingText);
	registerItemBoolAttribute("showdetails", m_ShowDetails);

	CUIExpression selectEventExpression;
	selectEventExpression.setFixedValue(m_sSelectEvent);
	registerItemStringAttribute("selectevent", selectEventExpression);

	CUIExpression selectionValueExpression;
	selectionValueExpression.setFixedValue(m_sSelectedPartFieldUUID);
	registerItemStringAttribute("selectionvalueuuid", selectionValueExpression);

	// The build UUID expression may reference session variables.
	registerItemProviderAttribute(AMC_API_KEY_PARTSTATEVERSION, eUIFrontendDefinitionAttributeType::atInteger, eUIFrontendDefinitionAttributeScope::asSession,
		[this](CJSONWriter& writer, CJSONWriterObject& object, const std::string& sName, CStateMachineData* pStateMachineData, CUIFrontendState* pFrontendState) {
			uint64_t nPartStateVersion = 0;
			std::string sBuildUUID = m_BuildUUIDExpression.evaluateStringValue(pStateMachineData, pFrontendState);
			if (AMCCommon::CUtils::stringIsNonEmptyUUIDString(sBuildUUID)) {
				auto pBuildJobHandler = m_pUIModuleEnvironment->dataModel()->CreateBuildJobHandler();
				if (pBuildJobHandler->JobExists(sBuildUUID)) {
					auto pBuildJob = pBuildJobHandler->RetrieveJob(sBuildUUID);
					nPartStateVersion = m_pUIModuleEnvironment->toolpathHandler()->getDisabledPartsVersion(pBuildJob->GetStorageStreamUUID());
				}
			}
			object.addInteger(sName, (int64_t)nPartStateVersion);
		});
}
