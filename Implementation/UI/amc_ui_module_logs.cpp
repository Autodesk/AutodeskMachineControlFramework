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

#include "amc_ui_module.hpp"
#include "amc_ui_module_item.hpp"
#include "amc_ui_modulefactory.hpp"

#include "amc_ui_module_logs.hpp"

#include "amc_api_constants.hpp"
#include "amc_resourcepackage.hpp"
#include "amc_parameterhandler.hpp"
#include "amc_ui_handler.hpp"
#include "amc_ui_frontendstate.hpp"

#include "common_utils.hpp"

#include "libmc_exceptiontypes.hpp"

using namespace AMC;



CUIModule_LogsItem::CUIModule_LogsItem (const std::string& sItemPath, PUIModuleEnvironment pUIModuleEnvironment)
  : CUIModuleItem(sItemPath), m_pUIModuleEnvironment(pUIModuleEnvironment), m_sUUID(AMCCommon::CUtils::createUUID())
{
	LibMCAssertNotNull(m_pUIModuleEnvironment);
}


std::string CUIModule_LogsItem::getUUID()
{
	return m_sUUID;
}

std::string CUIModule_LogsItem::findElementPathByUUID(const std::string& sUUID)
{
	if (sUUID == getUUID())
		return getItemPath();

	return "";
}


void CUIModule_LogsItem::addLegacyContentToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler, uint32_t nStateID)
{
	//auto pGroup = pClientVariableHandler->findGroup(getItemPath (), true);

	auto pStateMachineData = m_pUIModuleEnvironment->stateMachineData ();
	uint32_t nMaxEntriesToRetrieve = 128;

	CJSONWriterArray jsonLogEntryArray(writer);

	auto pLogger = m_pUIModuleEnvironment->getLogger();
	if (pLogger->supportsLogMessagesRetrieval()) {
		std::vector<CLoggerEntry> loggerEntries;
		uint32_t nEndID = pLogger->getLogMessageHeadID();
		uint32_t nStartID = nStateID;
		if ((nStartID + nMaxEntriesToRetrieve) < nEndID)
			nStartID = nEndID - nMaxEntriesToRetrieve;

		pLogger->retrieveLogMessages(loggerEntries, nStartID, nEndID, LibMCData::eLogLevel::Message);

		for (auto loggerEntry : loggerEntries) {
			CJSONWriterObject jsonEntryObject(writer);

			jsonEntryObject.addInteger (AMC_API_KEY_UI_LOGENTRYID, loggerEntry.getID ());
			jsonEntryObject.addString(AMC_API_KEY_UI_LOGSUBSYSTEM, loggerEntry.getSubSystem());
			jsonEntryObject.addString(AMC_API_KEY_UI_LOGTIMESTAMP, loggerEntry.getTimeStamp());
			jsonEntryObject.addString(AMC_API_KEY_UI_LOGMESSAGE, loggerEntry.getMessage());
			jsonEntryObject.addString(AMC_API_KEY_UI_LOGLEVEL, loggerEntry.getlogLevelString());

			jsonLogEntryArray.addObject(jsonEntryObject);
		}

	}


	object.addArray(AMC_API_KEY_UI_LOGENTRIES, jsonLogEntryArray);

}

void CUIModule_LogsItem::setEventPayloadValue(const std::string& sEventName, const std::string& sPayloadUUID, const std::string& sPayloadValue, CParameterHandler* pClientVariableHandler)
{

}

/////////////////////////////////////////////////////////////////////////////////////
// General module functionality
/////////////////////////////////////////////////////////////////////////////////////

CUIModule_Logs::CUIModule_Logs(pugi::xml_node& xmlNode, const std::string& sPath, PUIModuleEnvironment pUIModuleEnvironment)
: CUIModule (getNameFromXML(xmlNode), sPath, pUIModuleEnvironment->getFrontendDefinition ()),
  m_pUIModuleEnvironment (pUIModuleEnvironment)
{

	LibMCAssertNotNull(pUIModuleEnvironment.get());
	if (getTypeFromXML(xmlNode) != getStaticType())
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDMODULETYPE, "should be " + getStaticType ());

	if (sPath.empty())
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDMODULEPATH, m_sName);

	auto captionAttrib = xmlNode.attribute("caption");
	m_sCaption = captionAttrib.as_string();

	m_nDefaultCount = xmlNode.attribute("defaultcount").as_uint(200);
	m_bShowToolbar = xmlNode.attribute("showtoolbar").as_bool(true);
	m_nMaxClientEntries = xmlNode.attribute("maxcliententries").as_uint(2000);
	m_sDownloadPrefix = xmlNode.attribute("downloadprefix").as_string("log");

	CUIExpression defaultCountExpr;
	defaultCountExpr.setFixedValue(std::to_string(m_nDefaultCount));
	registerIntegerAttribute("defaultcount", defaultCountExpr);

	CUIExpression showToolbarExpr;
	showToolbarExpr.setFixedValue(m_bShowToolbar ? "1" : "0");
	registerBoolAttribute("showtoolbar", showToolbarExpr);

	CUIExpression maxClientEntriesExpr;
	maxClientEntriesExpr.setFixedValue(std::to_string(m_nMaxClientEntries));
	registerIntegerAttribute("maxcliententries", maxClientEntriesExpr);

	CUIExpression downloadPrefixExpr;
	downloadPrefixExpr.setFixedValue(m_sDownloadPrefix);
	registerStringAttribute("downloadprefix", downloadPrefixExpr);

	CUIExpression captionExpr;
	captionExpr.setFixedValue(m_sCaption);
	registerStringAttribute("caption", captionExpr);

	CUIExpression visibleExpr;
	visibleExpr.setFixedValue("1");
	registerBoolAttribute("visible", visibleExpr);

	m_LogsItem = std::make_shared<CUIModule_LogsItem>(getModulePath (), pUIModuleEnvironment);

}


CUIModule_Logs::~CUIModule_Logs()
{
}



std::string CUIModule_Logs::getStaticType()
{
	return "logs";
}

std::string CUIModule_Logs::getType()
{
	return getStaticType();
}

std::string CUIModule_Logs::getCaption()
{
	return m_sCaption;
}

/////////////////////////////////////////////////////////////////////////////////////
// Legacy UI System
/////////////////////////////////////////////////////////////////////////////////////


void CUIModule_Logs::writeLegacyDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& moduleObject, CParameterHandler* pLegacyClientVariableHandler)
{
	moduleObject.addString(AMC_API_KEY_UI_MODULENAME, getName());
	moduleObject.addString(AMC_API_KEY_UI_MODULEUUID, getUUID());
	moduleObject.addString(AMC_API_KEY_UI_MODULETYPE, getType());
	moduleObject.addString(AMC_API_KEY_UI_CAPTION, m_sCaption);


}

void CUIModule_Logs::addContentToJSON(CJSONWriter& writer, CJSONWriterObject& moduleObject, CParameterHandler* pClientVariableHandler, uint32_t nStateID)
{
}

PUIModuleItem CUIModule_Logs::findLegacyItem(const std::string& sUUID)
{
	if (sUUID == m_sUUID)
		return m_LogsItem;

	if (m_LogsItem->getUUID() == sUUID)
		return m_LogsItem;

	return nullptr;
}

void CUIModule_Logs::populateModuleMap(std::map<std::string, PUIModule>& moduleMap)
{
	moduleMap.insert(std::make_pair(m_sUUID, std::make_shared<CUIModule_Logs>(*this)));
}

void CUIModule_Logs::populateLegacyItemMap(std::map<std::string, PUIModuleItem>& itemMap)
{
	itemMap.insert (std::make_pair (m_sUUID, m_LogsItem));
	itemMap.insert(std::make_pair(m_LogsItem->getUUID(), m_LogsItem));
}

/////////////////////////////////////////////////////////////////////////////////////
// New UI Frontend System
/////////////////////////////////////////////////////////////////////////////////////

bool CUIModule_Logs::isVersion2FrontendModule()
{
	return true;
}

void CUIModule_Logs::frontendWriteModuleStatusToJSON(CJSONWriter& writer, CJSONWriterObject& moduleObject, CUIFrontendState* pFrontendState, CStateMachineData* pStateMachineData)
{
	if (pFrontendState == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	moduleObject.addString("moduletype", getType());
	moduleObject.addString("uuid", m_sUUID);

	CJSONWriterObject attributesObject(writer);
	pFrontendState->writeModuleAttributesToJSON(writer, attributesObject, m_pModuleStore.get(), pStateMachineData);

	auto pLogger = m_pUIModuleEnvironment->getLogger();
	if (pLogger->supportsLogMessagesRetrieval()) {
		attributesObject.addInteger("logheadid", pLogger->getLogMessageHeadID());
	}

	moduleObject.addObject("attributes", attributesObject);
}
