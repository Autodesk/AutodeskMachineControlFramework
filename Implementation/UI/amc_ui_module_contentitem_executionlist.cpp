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

#include "amc_ui_module_contentitem_executionlist.hpp"
#include "libmc_interfaceexception.hpp"

#include "amc_api_constants.hpp"
#include "Common/common_utils.hpp"
#include "amc_parameterhandler.hpp"
#include "amc_statemachinedata.hpp"
#include "amc_ui_module.hpp"
#include "amc_resourcepackage.hpp"
#include "libmc_exceptiontypes.hpp"
#include "libmcdata_dynamic.hpp"

using namespace AMC;

#define CONTENTEXECUTIONLISTHEADER_DEFAULTCAPTION_THUMBNAIL "Thumbnail"
#define CONTENTEXECUTIONLISTHEADER_DEFAULTCAPTION_EXECUTIONNAME "Execution name"
#define CONTENTEXECUTIONLISTHEADER_DEFAULTCAPTION_EXECUTIONTIMESTAMP "Execution time"


CUIModule_ContentExecutionListButton::CUIModule_ContentExecutionListButton(const std::string& sButtonName, CUIExpression captionExpression, const std::string& sEvent)
	: m_sButtonName(sButtonName),
	m_CaptionExpression(captionExpression),
	m_sEvent(sEvent),
	m_sUUID(AMCCommon::CUtils::createUUID())

{

}

CUIModule_ContentExecutionListButton::~CUIModule_ContentExecutionListButton()
{

}

std::string CUIModule_ContentExecutionListButton::getButtonName()
{
	return m_sButtonName;
}


std::string CUIModule_ContentExecutionListButton::getUUID()
{
	return m_sUUID;
}

CUIExpression CUIModule_ContentExecutionListButton::getCaptionExpression()
{
	return m_CaptionExpression;
}

std::string CUIModule_ContentExecutionListButton::getEvent()
{
	return m_sEvent;

}

PUIModule_ContentExecutionList CUIModule_ContentExecutionList::makeFromXML(const pugi::xml_node& xmlNode, const std::string& sItemName, const std::string& sModulePath, PUIModuleEnvironment pUIModuleEnvironment)
{
	LibMCAssertNotNull(pUIModuleEnvironment);

	auto entriesperpageAttrib = xmlNode.attribute("entriesperpage");
	auto selectEventAttrib = xmlNode.attribute("selectevent");
	std::string sSelectEvent = selectEventAttrib.as_string();

	int nEntriesPerPage;
	if (!entriesperpageAttrib.empty()) {
		nEntriesPerPage = entriesperpageAttrib.as_int();
		if (nEntriesPerPage < AMC_API_KEY_UI_ITEM_MINENTRIESPERPAGE)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDENTRIESPERPAGE);
		if (nEntriesPerPage > AMC_API_KEY_UI_ITEM_MAXENTRIESPERPAGE)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDENTRIESPERPAGE);
	}
	else {
		nEntriesPerPage = AMC_API_KEY_UI_ITEM_DEFAULTENTRIESPERPAGE;
	}


	std::string sDefaultThumbnailUUID = AMCCommon::CUtils::createEmptyUUID();
	std::string sThumbnailName = xmlNode.attribute("thumbnail").as_string();

	if (!sThumbnailName.empty()) {
		auto resourcePackage = pUIModuleEnvironment->resourcePackage();
		auto pThumbnailPackageEntry = resourcePackage->findEntryByName(sThumbnailName, true);
		sDefaultThumbnailUUID = pThumbnailPackageEntry->getUUID();
	}



	CUIExpression loadingText(xmlNode, "loadingtext");

	auto pBuildList = std::make_shared <CUIModule_ContentExecutionList>(loadingText, nEntriesPerPage, sSelectEvent, pUIModuleEnvironment->dataModel(), sItemName, sModulePath, sDefaultThumbnailUUID, pUIModuleEnvironment->stateMachineData(), pUIModuleEnvironment->getGlobalChrono ());

	auto buttonNodes = xmlNode.children("button");
	for (auto buttonNode : buttonNodes) {
		std::string sName = buttonNode.attribute("name").as_string();
		std::string sEvent = buttonNode.attribute("event").as_string();

		if (sName.empty())
			throw ELibMCInterfaceException(LIBMC_ERROR_EXECUTIONLISTBUTTONNAMEMISSING);
		if (!sEvent.empty()) {
			if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sEvent))
				throw ELibMCCustomException(LIBMC_ERROR_INVALIDEVENTNAME, sEvent);
		}

		CUIExpression captionExpression(buttonNode, "caption");

		pBuildList->addButton(sName, captionExpression, sEvent);
	}

	return pBuildList;


}

CUIModule_ContentExecutionList::CUIModule_ContentExecutionList(const CUIExpression& loadingText, const uint32_t nEntriesPerPage, const std::string& sSelectEvent, LibMCData::PDataModel pDataModel, const std::string& sItemName, const std::string& sModulePath, const std::string sDefaultThumbnailResourceUUID, PStateMachineData pStateMachineData, AMCCommon::PChrono pGlobalChrono)
	: CUIModule_ContentItem(AMCCommon::CUtils::createUUID(), sItemName, sModulePath),
	m_LoadingText(loadingText),
	m_nEntriesPerPage(nEntriesPerPage),
	m_sSelectEvent(sSelectEvent),
	m_pDataModel(pDataModel),
	m_pStateMachineData(pStateMachineData),
	m_pGlobalChrono (pGlobalChrono),
	m_sDefaultThumbnailResourceUUID(AMCCommon::CUtils::normalizeUUIDString(sDefaultThumbnailResourceUUID))

{
	if (sModulePath.empty())
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDMODULEPATH);
	if (sItemName.empty())
		throw ELibMCInterfaceException(LIBMC_ERROR_EXECUTIONLISTNAMEMISSING);
	if (pDataModel.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
	if (pStateMachineData.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
	if (pGlobalChrono.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	m_sSelectedExecutionFieldUUID = AMCCommon::CUtils::createUUID();
	m_sSelectedButtonFieldUUID = AMCCommon::CUtils::createUUID();

	// In order to maintain compatibility with legacy code, set proper defaults
	m_ExecutionTimestampCaption.setFixedValue(CONTENTEXECUTIONLISTHEADER_DEFAULTCAPTION_EXECUTIONTIMESTAMP);
	m_ExecutionThumbnailCaption.setFixedValue(CONTENTEXECUTIONLISTHEADER_DEFAULTCAPTION_THUMBNAIL);
	m_ExecutionNameCaption.setFixedValue(CONTENTEXECUTIONLISTHEADER_DEFAULTCAPTION_EXECUTIONNAME);

}

CUIModule_ContentExecutionList::~CUIModule_ContentExecutionList()
{

}

void CUIModule_ContentExecutionList::writeHeadersToJSON(CJSONWriter& writer, CJSONWriterObject& object)
{

	CJSONWriterArray headersArray(writer);

	CJSONWriterObject headerObject0(writer);
	headerObject0.addString(AMC_API_KEY_UI_ITEMTEXT, m_ExecutionThumbnailCaption.evaluateStringValue(m_pStateMachineData));
	headerObject0.addString(AMC_API_KEY_UI_ITEMVALUE, AMC_API_KEY_UI_ITEMEXECUTIONTHUMBNAIL);
	headerObject0.addBool(AMC_API_KEY_UI_ITEMSORTABLE, true);
	headerObject0.addString(AMC_API_KEY_UI_ITEMWIDTH, "20vw");
	headersArray.addObject(headerObject0);

	CJSONWriterObject headerObject1(writer);
	headerObject1.addString(AMC_API_KEY_UI_ITEMTEXT, m_ExecutionNameCaption.evaluateStringValue(m_pStateMachineData));
	headerObject1.addString(AMC_API_KEY_UI_ITEMVALUE, AMC_API_KEY_UI_ITEMEXECUTIONNAME);
	headerObject1.addBool(AMC_API_KEY_UI_ITEMSORTABLE, true);
	headersArray.addObject(headerObject1);

	CJSONWriterObject headerObject4(writer);
	headerObject4.addString(AMC_API_KEY_UI_ITEMTEXT, m_ExecutionTimestampCaption.evaluateStringValue(m_pStateMachineData));
	headerObject4.addString(AMC_API_KEY_UI_ITEMVALUE, AMC_API_KEY_UI_ITEMEXECUTIONSTARTTIMESTAMP);
	headerObject4.addBool(AMC_API_KEY_UI_ITEMSORTABLE, true);
	headerObject4.addString(AMC_API_KEY_UI_ITEMWIDTH, "20vw");
	headersArray.addObject(headerObject4);

	object.addArray(AMC_API_KEY_UI_ITEMHEADERS, headersArray);
}


void CUIModule_ContentExecutionList::writeButtonsToJSON(CJSONWriter& writer, CJSONWriterObject& object)
{
	CJSONWriterArray entryButtonsArray(writer);

	for (auto pButton : m_Buttons) {

		auto captionExpression = pButton->getCaptionExpression();
		std::string sCaption = captionExpression.evaluateStringValue(m_pStateMachineData);

		CJSONWriterObject entryButton(writer);
		entryButton.addString(AMC_API_KEY_UI_ENTRYBUTTONUUID, pButton->getUUID());
		entryButton.addString(AMC_API_KEY_UI_ENTRYBUTTONCAPTION, sCaption);
		entryButton.addString(AMC_API_KEY_UI_ENTRYBUTTONCOLOR, "primary");
		entryButton.addString(AMC_API_KEY_UI_ENTRYBUTTONCURSOR, "cursor-pointer");
		entryButton.addString(AMC_API_KEY_UI_ENTRYBUTTONSELECTEVENT, pButton->getEvent());
		entryButtonsArray.addObject(entryButton);
	}

	object.addArray(AMC_API_KEY_UI_ENTRYBUTTONS, entryButtonsArray);

}


void CUIModule_ContentExecutionList::addDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler)
{

	std::string sLoadingText = m_LoadingText.evaluateStringValue(m_pStateMachineData);

	object.addString(AMC_API_KEY_UI_ITEMTYPE, "executionlist");
	object.addString(AMC_API_KEY_UI_ITEMUUID, m_sUUID);
	object.addString(AMC_API_KEY_UI_ITEMLOADINGTEXT, sLoadingText);
	object.addString(AMC_API_KEY_UI_ITEMSELECTEVENT, m_sSelectEvent);
	object.addString(AMC_API_KEY_UI_ITEMSELECTIONVALUEUUID, m_sSelectedExecutionFieldUUID);
	object.addString(AMC_API_KEY_UI_ITEMBUTTONVALUEUUID, m_sSelectedButtonFieldUUID);
	object.addInteger(AMC_API_KEY_UI_ITEMENTRIESPERPAGE, m_nEntriesPerPage);

	writeHeadersToJSON(writer, object);
	writeButtonsToJSON(writer, object);

	CJSONWriterArray entriesArray(writer);
	object.addArray(AMC_API_KEY_UI_ITEMENTRIES, entriesArray);
}

void CUIModule_ContentExecutionList::addContentToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler, uint32_t nStateID)
{

	writeHeadersToJSON(writer, object);
	writeButtonsToJSON(writer, object);

	CJSONWriterArray entryArray(writer);

	auto pBuildJobHandler = m_pDataModel->CreateBuildJobHandler();
	

	auto pExecutionIterator = pBuildJobHandler->ListJobExecutions("", "", "");
	while (pExecutionIterator->MoveNext()) {

		auto pExecution = pExecutionIterator->GetCurrentJobExecution();

		LibMCData::eBuildJobExecutionStatus status = pExecution->GetStatus();
		std::string sStatusString = pExecution->GetStatusString();
		bool bHasEndTime = (status == LibMCData::eBuildJobExecutionStatus::Finished);

		uint64_t nStartTimeStamp = pExecution->GetStartTimeStampInMicroseconds();
		uint64_t nEndTimeStamp = 0;
		
		if (bHasEndTime) {
			nEndTimeStamp = pExecution->GetEndTimeStampInMicroseconds();
		}

		int64_t nDurationInSeconds = pExecution->ComputeElapsedTimeInMicroseconds (m_pGlobalChrono->getUTCTimeStampInMicrosecondsSince1970 (), false) / 1000000LL;

		CJSONWriterObject entryObject(writer);
		entryObject.addString(AMC_API_KEY_UI_ITEMEXECUTIONNAME, pExecution->GetJobName());
		entryObject.addString(AMC_API_KEY_UI_ITEMEXECUTIONUUID, pExecution->GetExecutionUUID());
		entryObject.addString(AMC_API_KEY_UI_ITEMEXECUTIONDESCRIPTION, pExecution->GetDescription());
		entryObject.addString(AMC_API_KEY_UI_ITEMEXECUTIONSTARTTIMESTAMP, AMCCommon::CChrono::convertToISO8601TimeUTC (nStartTimeStamp));
		entryObject.addString(AMC_API_KEY_UI_ITEMEXECUTIONENDTIMESTAMP, AMCCommon::CChrono::convertToISO8601TimeUTC(nEndTimeStamp));
		entryObject.addInteger(AMC_API_KEY_UI_ITEMEXECUTIONDURATION, nDurationInSeconds);
		entryObject.addString(AMC_API_KEY_UI_ITEMEXECUTIONTHUMBNAIL, m_sDefaultThumbnailResourceUUID);
		entryObject.addInteger(AMC_API_KEY_UI_ITEMEXECUTIONLAYERCOUNT, pExecution->GetJobLayerCount());
		entryObject.addString(AMC_API_KEY_UI_ITEMEXECUTIONSTATUS, sStatusString);
		entryObject.addString(AMC_API_KEY_UI_ITEMEXECUTIONBUILDSTATUS, pExecution->GetJobStatusString());

		//		entryObject.addString(AMC_API_KEY_UI_ITEMEXECUTIONUSER, pExecution->GetCreatorName());

		entryArray.addObject(entryObject);

	}

	object.addArray(AMC_API_KEY_UI_ITEMENTRIES, entryArray);

}


void CUIModule_ContentExecutionList::populateClientVariables(CParameterHandler* pClientVariableHandler)
{
	LibMCAssertNotNull(pClientVariableHandler);
	auto pGroup = pClientVariableHandler->addGroup(getItemPath(), "build list UI element");
	pGroup->addNewStringParameter("selecteduuid", "selected build UUID", AMCCommon::CUtils::createEmptyUUID());
	pGroup->addNewStringParameter("buttonuuid", "button UUID", AMCCommon::CUtils::createEmptyUUID());
	pGroup->addNewStringParameter("buttonname", "button Name", "");
}

void CUIModule_ContentExecutionList::setEventPayloadValue(const std::string& sEventName, const std::string& sPayloadUUID, const std::string& sPayloadValue, CParameterHandler* pClientVariableHandler)
{
	LibMCAssertNotNull(pClientVariableHandler);
	std::string sNormalizedString = AMCCommon::CUtils::normalizeUUIDString(sPayloadUUID);
	if (sNormalizedString == m_sSelectedExecutionFieldUUID) {
		auto pGroup = pClientVariableHandler->findGroup(getItemPath(), true);
		pGroup->setParameterValueByName("selecteduuid", AMCCommon::CUtils::normalizeUUIDString(sPayloadValue));

	}

	if (sNormalizedString == m_sSelectedButtonFieldUUID) {
		auto pGroup = pClientVariableHandler->findGroup(getItemPath(), true);
		std::string sButtonUUID = AMCCommon::CUtils::normalizeUUIDString(sPayloadValue);
		std::string sButtonName;

		auto iIter = m_ButtonUUIDMap.find(sButtonUUID);
		if (iIter != m_ButtonUUIDMap.end()) {
			sButtonName = iIter->second->getButtonName();
		}

		pGroup->setParameterValueByName("buttonuuid", sButtonUUID);
		pGroup->setParameterValueByName("buttonname", sButtonName);

	}

}


std::string CUIModule_ContentExecutionList::findElementPathByUUID(const std::string& sUUID)
{
	if (sUUID == m_sSelectedExecutionFieldUUID)
		return getItemPath();
	if (sUUID == m_sSelectedButtonFieldUUID)
		return getItemPath();
	if (sUUID == getUUID())
		return getItemPath();

	return "";
}


std::list <std::string> CUIModule_ContentExecutionList::getReferenceUUIDs()
{
	std::list <std::string> sUUIDList;
	sUUIDList.push_back(m_sSelectedExecutionFieldUUID);
	sUUIDList.push_back(m_sSelectedButtonFieldUUID);
	sUUIDList.push_back(getUUID());

	return sUUIDList;
}


void CUIModule_ContentExecutionList::addButton(const std::string& sButtonName, CUIExpression captionExpression, const std::string& sEvent)
{
	if (sButtonName.empty())
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDBUTTONNAME);

	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sButtonName))
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDBUTTONNAME, sButtonName);

	auto iIter = m_ButtonNameMap.find(sButtonName);
	if (iIter != m_ButtonNameMap.end())
		throw ELibMCCustomException(LIBMC_ERROR_DUPLICATEBUTTONNAME, sButtonName);

	auto pButton = std::make_shared<CUIModule_ContentExecutionListButton>(sButtonName, captionExpression, sEvent);
	m_Buttons.push_back(pButton);
	m_ButtonNameMap.insert(std::make_pair(sButtonName, pButton));
	m_ButtonUUIDMap.insert(std::make_pair(pButton->getUUID(), pButton));


}
