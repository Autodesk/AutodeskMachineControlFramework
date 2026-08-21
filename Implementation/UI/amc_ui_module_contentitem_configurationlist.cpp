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

#include "amc_ui_module_contentitem_configurationlist.hpp"
#include "libmc_interfaceexception.hpp"

#include "amc_api_constants.hpp"
#include "Common/common_utils.hpp"
#include "amc_parameterhandler.hpp"
#include "amc_statemachinedata.hpp"
#include "amc_ui_module.hpp"
#include "amc_resourcepackage.hpp"
#include "libmc_exceptiontypes.hpp"
#include "libmcdata_dynamic.hpp"

#include <cstdint>

using namespace AMC;

#define CONTENTCONFIGURATIONLISTHEADER_DEFAULTCAPTION_THUMBNAIL "Thumbnail"
#define CONTENTCONFIGURATIONLISTHEADER_DEFAULTCAPTION_CONFIGURATIONNAME "Configuration name"
#define CONTENTCONFIGURATIONLISTHEADER_DEFAULTCAPTION_CONFIGURATIONTIMESTAMP "Upload time"
#define CONTENTCONFIGURATIONLIST_DEFAULTSCHEMA "com.scanlab.ocmsmc"

static void fnv1aMixString(uint64_t& nHash, const std::string& sValue)
{
	for (unsigned char nChar : sValue) {
		nHash ^= (uint64_t)nChar;
		nHash *= 1099511628211ULL;
	}
}


CUIModule_ContentConfigurationListButton::CUIModule_ContentConfigurationListButton(const std::string& sButtonName, CUIExpression captionExpression, const std::string& sEvent)
	: m_sButtonName (sButtonName),
	m_CaptionExpression (captionExpression),
	m_sEvent (sEvent),
	m_sUUID (AMCCommon::CUtils::createUUID ())

{

}

CUIModule_ContentConfigurationListButton::~CUIModule_ContentConfigurationListButton()
{

}

std::string CUIModule_ContentConfigurationListButton::getButtonName() 
{
	return m_sButtonName;
}


std::string CUIModule_ContentConfigurationListButton::getUUID()
{
	return m_sUUID;
}

CUIExpression CUIModule_ContentConfigurationListButton::getCaptionExpression()
{
	return m_CaptionExpression;
}

std::string CUIModule_ContentConfigurationListButton::getEvent()
{
	return m_sEvent;

}

PUIModule_ContentConfigurationList CUIModule_ContentConfigurationList::makeFromXML(const pugi::xml_node& xmlNode, const std::string& sItemName, const std::string& sModulePath, PUIModuleEnvironment pUIModuleEnvironment)
{
	LibMCAssertNotNull(pUIModuleEnvironment);

	auto entriesperpageAttrib = xmlNode.attribute("entriesperpage");
	auto selectEventAttrib = xmlNode.attribute("selectevent");
	auto schemaAttrib = xmlNode.attribute("schema");
	std::string sSelectEvent = selectEventAttrib.as_string();
	std::string sSchema = schemaAttrib.as_string();
	if (sSchema.empty())
		sSchema = CONTENTCONFIGURATIONLIST_DEFAULTSCHEMA;

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
	std::string sThumbnailName = xmlNode.attribute("thumbnail").as_string ();

	if (!sThumbnailName.empty()) {
		auto resourcePackage = pUIModuleEnvironment->resourcePackage();
		auto pThumbnailPackageEntry = resourcePackage->findEntryByName(sThumbnailName, true);
		sDefaultThumbnailUUID = pThumbnailPackageEntry->getUUID();
	}



	CUIExpression loadingText(xmlNode, "loadingtext");

	auto pConfigurationList = std::make_shared <CUIModule_ContentConfigurationList>(loadingText, nEntriesPerPage, sSelectEvent, sSchema, pUIModuleEnvironment->dataModel(), sItemName, sModulePath, sDefaultThumbnailUUID, pUIModuleEnvironment->stateMachineData());

	auto buttonNodes = xmlNode.children("button");
	for (auto buttonNode : buttonNodes) {
		std::string sName = buttonNode.attribute("name").as_string ();
		std::string sEvent = buttonNode.attribute("event").as_string();

		if (sName.empty ())
			throw ELibMCInterfaceException(LIBMC_ERROR_CONFIGURATIONLISTBUTTONNAMEMISSING);
		if (!sEvent.empty()) {
			if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString (sEvent))
				throw ELibMCCustomException(LIBMC_ERROR_INVALIDEVENTNAME, sEvent);
		}

		CUIExpression captionExpression(buttonNode, "caption");

		pConfigurationList->addButton (sName, captionExpression, sEvent);
	}

	return pConfigurationList;


}

CUIModule_ContentConfigurationList::CUIModule_ContentConfigurationList(const CUIExpression& loadingText, const uint32_t nEntriesPerPage, const std::string& sSelectEvent, const std::string& sConfigurationSchema, LibMCData::PDataModel pDataModel, const std::string& sItemName, const std::string& sModulePath, const std::string sDefaultThumbnailResourceUUID, PStateMachineData pStateMachineData)
	: CUIModule_ContentItem(AMCCommon::CUtils::createUUID(), sItemName, sModulePath), 
	  m_LoadingText(loadingText), 
	  m_nEntriesPerPage(nEntriesPerPage), 
	  m_sSelectEvent(sSelectEvent), 
	  m_sConfigurationSchema(sConfigurationSchema),
	  m_pDataModel (pDataModel), 
	  m_pStateMachineData (pStateMachineData),
	  m_sDefaultThumbnailResourceUUID (AMCCommon::CUtils::normalizeUUIDString (sDefaultThumbnailResourceUUID))

{
	if (sModulePath.empty ())
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDMODULEPATH);
	if (sItemName.empty ())
		throw ELibMCInterfaceException(LIBMC_ERROR_CONFIGURATIONLISTNAMEMISSING);
	if (pDataModel.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
	if (pStateMachineData.get () == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
	if (m_sConfigurationSchema.empty())
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	m_sSelectedConfigurationFieldUUID = AMCCommon::CUtils::createUUID();
	m_sSelectedButtonFieldUUID = AMCCommon::CUtils::createUUID();

	// In order to maintain compatibility with legacy code, set proper defaults
	m_ConfigurationThumbnailCaption.setFixedValue (CONTENTCONFIGURATIONLISTHEADER_DEFAULTCAPTION_THUMBNAIL);
	m_ConfigurationNameCaption.setFixedValue(CONTENTCONFIGURATIONLISTHEADER_DEFAULTCAPTION_CONFIGURATIONNAME);
	m_ConfigurationTimestampCaption.setFixedValue(CONTENTCONFIGURATIONLISTHEADER_DEFAULTCAPTION_CONFIGURATIONTIMESTAMP);

}

CUIModule_ContentConfigurationList::~CUIModule_ContentConfigurationList()
{

}

void CUIModule_ContentConfigurationList::writeHeadersToJSON(CJSONWriter& writer, CJSONWriterObject& object)
{

	CJSONWriterArray headersArray(writer);

#if 0
	CJSONWriterObject headerObject0(writer);
	headerObject0.addString(AMC_API_KEY_UI_ITEMTEXT, m_ConfigurationThumbnailCaption.evaluateStringValue (m_pStateMachineData));
	headerObject0.addString(AMC_API_KEY_UI_ITEMVALUE, AMC_API_KEY_UI_ITEMCONFIGURATIONTHUMBNAIL);
	headerObject0.addBool(AMC_API_KEY_UI_ITEMSORTABLE, true);
	headerObject0.addString(AMC_API_KEY_UI_ITEMWIDTH, "20vw");
	headersArray.addObject(headerObject0);
#endif

	{
		CJSONWriterObject headerObject(writer);
		headerObject.addString(AMC_API_KEY_UI_ITEMTEXT, "Active"/*m_ConfigurationThumbnailCaption.evaluateStringValue(m_pStateMachineData)*/);
		headerObject.addString(AMC_API_KEY_UI_ITEMVALUE, "configurationActive" /*AMC_API_KEY_UI_ITEMCONFIGURATIONTHUMBNAIL*/);
		headerObject.addBool(AMC_API_KEY_UI_ITEMSORTABLE, true);
		headerObject.addString(AMC_API_KEY_UI_ITEMWIDTH, "5vw");
		headerObject.addString("align", "center");
	
		headersArray.addObject(headerObject);
	}

	{
		CJSONWriterObject headerObject(writer);
		headerObject.addString(AMC_API_KEY_UI_ITEMTEXT, "Version"/*m_ConfigurationThumbnailCaption.evaluateStringValue(m_pStateMachineData)*/);
		headerObject.addString(AMC_API_KEY_UI_ITEMVALUE, "configurationVersion" /*AMC_API_KEY_UI_ITEMCONFIGURATIONTHUMBNAIL*/);
		headerObject.addBool(AMC_API_KEY_UI_ITEMSORTABLE, true);
		headerObject.addString(AMC_API_KEY_UI_ITEMWIDTH, "5vw");
		headerObject.addString("align", "center");
		headersArray.addObject(headerObject);
	}

	{
		CJSONWriterObject headerObject(writer);
		headerObject.addString(AMC_API_KEY_UI_ITEMTEXT, m_ConfigurationTimestampCaption.evaluateStringValue(m_pStateMachineData));
		headerObject.addString(AMC_API_KEY_UI_ITEMVALUE, AMC_API_KEY_UI_ITEMCONFIGURATIONTIMESTAMP);
		headerObject.addBool(AMC_API_KEY_UI_ITEMSORTABLE, true);
		headerObject.addString(AMC_API_KEY_UI_ITEMWIDTH, "10vw");
		headerObject.addString("align", "center");
		headersArray.addObject(headerObject);
	}

	{
		CJSONWriterObject headerObject(writer);
		headerObject.addString(AMC_API_KEY_UI_ITEMTEXT, "User"/*m_ConfigurationThumbnailCaption.evaluateStringValue(m_pStateMachineData)*/);
		headerObject.addString(AMC_API_KEY_UI_ITEMVALUE, "userName" /*AMC_API_KEY_UI_ITEMCONFIGURATIONTHUMBNAIL*/);
		headerObject.addBool(AMC_API_KEY_UI_ITEMSORTABLE, true);
		headerObject.addString(AMC_API_KEY_UI_ITEMWIDTH, "5vw");
		headerObject.addString("align", "center");
		headersArray.addObject(headerObject);
	}

	{
		CJSONWriterObject headerObject(writer);
		headerObject.addString(AMC_API_KEY_UI_ITEMTEXT, "Actions"/*m_ConfigurationThumbnailCaption.evaluateStringValue(m_pStateMachineData)*/);
		headerObject.addString(AMC_API_KEY_UI_ITEMVALUE, "configurationActions" /*AMC_API_KEY_UI_ITEMCONFIGURATIONTHUMBNAIL*/);
		headerObject.addBool(AMC_API_KEY_UI_ITEMSORTABLE, true);
		headerObject.addString(AMC_API_KEY_UI_ITEMWIDTH, "20vw");
		headerObject.addString("align", "center");
		headersArray.addObject(headerObject);
	}

#if 0
	{
		CJSONWriterObject headerObject(writer);
		headerObject.addString(AMC_API_KEY_UI_ITEMTEXT, m_ConfigurationNameCaption.evaluateStringValue(m_pStateMachineData));
		headerObject.addString(AMC_API_KEY_UI_ITEMVALUE, AMC_API_KEY_UI_ITEMCONFIGURATIONNAME);
		headerObject.addBool(AMC_API_KEY_UI_ITEMSORTABLE, true);
		headersArray.addObject(headerObject);
	}


	{
		CJSONWriterObject headerObject(writer);
		headerObject.addString(AMC_API_KEY_UI_ITEMTEXT, m_ConfigurationTimestampCaption.evaluateStringValue(m_pStateMachineData));
		headerObject.addString(AMC_API_KEY_UI_ITEMVALUE, AMC_API_KEY_UI_ITEMCONFIGURATIONTIMESTAMP);
		headerObject.addBool(AMC_API_KEY_UI_ITEMSORTABLE, true);
		headerObject.addString(AMC_API_KEY_UI_ITEMWIDTH, "20vw");
		headersArray.addObject(headerObject);
	}
#endif
	object.addArray(AMC_API_KEY_UI_ITEMHEADERS, headersArray);
}


void CUIModule_ContentConfigurationList::writeButtonsToJSON(CJSONWriter& writer, CJSONWriterObject& object)
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
		entryButton.addString(AMC_API_KEY_UI_ENTRYBUTTONSELECTEVENT, pButton->getEvent ());
		entryButtonsArray.addObject(entryButton);
	}

	object.addArray(AMC_API_KEY_UI_ENTRYBUTTONS, entryButtonsArray);

}

#if 0 // DEPRECATED
void CUIModule_ContentConfigurationList::addLegacyDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler)
{

	std::string sLoadingText = m_LoadingText.evaluateStringValue(m_pStateMachineData);

	object.addString(AMC_API_KEY_UI_ITEMTYPE, "configurationlist");
	object.addString(AMC_API_KEY_UI_ITEMUUID, m_sUUID);
	object.addString(AMC_API_KEY_UI_ITEMLOADINGTEXT, sLoadingText);
	object.addString(AMC_API_KEY_UI_ITEMSELECTEVENT, m_sSelectEvent);
	object.addString(AMC_API_KEY_UI_ITEMSELECTIONVALUEUUID, m_sSelectedConfigurationFieldUUID);
	object.addString(AMC_API_KEY_UI_ITEMBUTTONVALUEUUID, m_sSelectedButtonFieldUUID);
	object.addInteger(AMC_API_KEY_UI_ITEMENTRIESPERPAGE, m_nEntriesPerPage);

	writeHeadersToJSON(writer, object);
	writeButtonsToJSON(writer, object);

	CJSONWriterArray entriesArray(writer);
	object.addArray(AMC_API_KEY_UI_ITEMENTRIES, entriesArray);
}
#endif 

void CUIModule_ContentConfigurationList::addLegacyContentToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler, uint32_t nStateID)
{
	std::string sLoadingText = m_LoadingText.evaluateStringValue(m_pStateMachineData);

	object.addString(AMC_API_KEY_UI_ITEMTYPE, "configurationlist");
	object.addString(AMC_API_KEY_UI_ITEMUUID, m_sUUID);
	object.addString(AMC_API_KEY_UI_ITEMLOADINGTEXT, sLoadingText);
	object.addString(AMC_API_KEY_UI_ITEMSELECTEVENT, m_sSelectEvent);
	object.addString(AMC_API_KEY_UI_ITEMSELECTIONVALUEUUID, m_sSelectedConfigurationFieldUUID);
	object.addString(AMC_API_KEY_UI_ITEMBUTTONVALUEUUID, m_sSelectedButtonFieldUUID);
	object.addInteger(AMC_API_KEY_UI_ITEMENTRIESPERPAGE, m_nEntriesPerPage);

	writeHeadersToJSON(writer, object);
	writeButtonsToJSON(writer, object);

	CJSONWriterArray entryArray(writer);

	//auto pConfigurationJobHandler = m_pDataModel->CreateConfigurationJobHandler();


	auto pConfigurationType = m_pDataModel->FindConfigurationTypeBySchema(m_sConfigurationSchema);

	if (pConfigurationType.get() != nullptr)
	{

		auto pConfigurationVersionsIterator = pConfigurationType->ListAllConfigurationVersions();

		auto pActiveConfiguration = pConfigurationType->GetActiveConfigurationVersion();


		while (pConfigurationVersionsIterator->MoveNext()) {

			auto pConfigurationVersion = pConfigurationVersionsIterator->GetCurrent();

			CJSONWriterObject entryObject(writer);

			if(pConfigurationVersion->GetVersionUUID() == pActiveConfiguration->GetVersionUUID())
				entryObject.addBool("configurationActive", true);
			else
				entryObject.addBool("configurationActive", false);

			entryObject.addInteger("configurationVersion", pConfigurationVersion->GetNumericVersion());
			entryObject.addString("userName", pConfigurationVersion->GetUserUUID());
			entryObject.addString(AMC_API_KEY_UI_ITEMCONFIGURATIONUUID, pConfigurationVersion->GetVersionUUID());
			entryObject.addString(AMC_API_KEY_UI_ITEMCONFIGURATIONTIMESTAMP, pConfigurationVersion->GetTimestamp());

			entryArray.addObject(entryObject);
		}
	}

	//--------------------------------------------
#if 0
	auto pConfigurationTypeIterator = m_pDataModel->ListRegisteredConfigurationTypes();

	while (pConfigurationTypeIterator->MoveNext ()) {
		
		auto pConfigurationJob = pConfigurationTypeIterator->GetCurrent();

		CJSONWriterObject entryObject(writer);


		entryObject.addString("configurationVersion", "v1.2.3");
		entryObject.addString("userName", "test");

		entryObject.addString(AMC_API_KEY_UI_ITEMCONFIGURATIONNAME, pConfigurationJob->GetName());
		//entryObject.addInteger(AMC_API_KEY_UI_ITEMCONFIGURATIONLAYERS, pConfigurationJob->GetLayerCount());
		entryObject.addString(AMC_API_KEY_UI_ITEMCONFIGURATIONUUID, pConfigurationJob->GetUUID());
		entryObject.addString(AMC_API_KEY_UI_ITEMCONFIGURATIONTIMESTAMP, pConfigurationJob->GetTimestamp());
#if 0
		if (pConfigurationJob->HasThumbnailStream ())
			entryObject.addString(AMC_API_KEY_UI_ITEMCONFIGURATIONTHUMBNAIL, pConfigurationJob->GetThumbnailStreamUUID ());
		else
			entryObject.addString(AMC_API_KEY_UI_ITEMCONFIGURATIONTHUMBNAIL, m_sDefaultThumbnailResourceUUID);

		entryObject.addString(AMC_API_KEY_UI_ITEMCONFIGURATIONUSER, pConfigurationJob->GetCreatorName());
		entryObject.addInteger(AMC_API_KEY_UI_ITEMCONFIGURATIONEXECUTIONCOUNT, pConfigurationJob->GetExecutionCount());

#endif
		entryArray.addObject(entryObject);

	}
#endif
	object.addArray(AMC_API_KEY_UI_ITEMENTRIES, entryArray);
}


void CUIModule_ContentConfigurationList::populateClientVariables(CParameterHandler* pClientVariableHandler)
{
	LibMCAssertNotNull(pClientVariableHandler);
	auto pGroup = pClientVariableHandler->addGroup(getItemPath(), "configuration list UI element");
	pGroup->addNewStringParameter("selecteduuid", "selected configuration UUID", AMCCommon::CUtils::createEmptyUUID());
	pGroup->addNewStringParameter("buttonuuid", "button UUID", AMCCommon::CUtils::createEmptyUUID());
	pGroup->addNewStringParameter("buttonname", "button Name", "");	
}

void CUIModule_ContentConfigurationList::setEventPayloadValue(const std::string& sEventName, const std::string& sPayloadUUID, const std::string& sPayloadValue, CParameterHandler* pClientVariableHandler)
{
	LibMCAssertNotNull(pClientVariableHandler);
	std::string sNormalizedString = AMCCommon::CUtils::normalizeUUIDString(sPayloadUUID);
	if (sNormalizedString == m_sSelectedConfigurationFieldUUID) {
		auto pGroup = pClientVariableHandler->findGroup(getItemPath(), true);
		pGroup->setParameterValueByName("selecteduuid", AMCCommon::CUtils::normalizeUUIDString(sPayloadValue));

	}

	if (sNormalizedString == m_sSelectedButtonFieldUUID) {
		auto pGroup = pClientVariableHandler->findGroup(getItemPath(), true);
		std::string sButtonUUID = AMCCommon::CUtils::normalizeUUIDString(sPayloadValue);
		std::string sButtonName;

		auto iIter = m_ButtonUUIDMap.find(sButtonUUID );
		if (iIter != m_ButtonUUIDMap.end()) {
			sButtonName = iIter->second->getButtonName();
		}

		pGroup->setParameterValueByName("buttonuuid", sButtonUUID);
		pGroup->setParameterValueByName("buttonname", sButtonName);

	}

}


std::string CUIModule_ContentConfigurationList::findElementPathByUUID(const std::string& sUUID)
{
	if (sUUID == m_sSelectedConfigurationFieldUUID)
		return getItemPath();
	if (sUUID == m_sSelectedButtonFieldUUID)
		return getItemPath();
	if (sUUID == getUUID ())
		return getItemPath();

	return "";
}


std::list <std::string> CUIModule_ContentConfigurationList::getReferenceUUIDs()
{
	std::list <std::string> sUUIDList;
	sUUIDList.push_back(m_sSelectedConfigurationFieldUUID);
	sUUIDList.push_back(m_sSelectedButtonFieldUUID);
	sUUIDList.push_back(getUUID ());

	return sUUIDList;
}


void CUIModule_ContentConfigurationList::addButton(const std::string& sButtonName, CUIExpression captionExpression, const std::string& sEvent)
{
	if (sButtonName.empty())
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDBUTTONNAME);

	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString (sButtonName))
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDBUTTONNAME, sButtonName);

	auto iIter = m_ButtonNameMap.find(sButtonName);
	if (iIter != m_ButtonNameMap.end ())
		throw ELibMCCustomException(LIBMC_ERROR_DUPLICATEBUTTONNAME, sButtonName);

	auto pButton = std::make_shared<CUIModule_ContentConfigurationListButton>(sButtonName, captionExpression, sEvent);
	m_Buttons.push_back(pButton);
	m_ButtonNameMap.insert(std::make_pair (sButtonName, pButton));
	m_ButtonUUIDMap.insert(std::make_pair(pButton->getUUID (), pButton));


}

std::string CUIModule_ContentConfigurationList::getItemType()
{
	return "configurationlist";
}

void CUIModule_ContentConfigurationList::registerFrontendAttributes()
{
	registerItemStringAttribute("loadingtext", m_LoadingText);
	CUIExpression selectEventExpr;
	selectEventExpr.setFixedValue(m_sSelectEvent);
	registerItemStringAttribute("selectevent", selectEventExpr);
	CUIExpression entriesExpr;
	entriesExpr.setFixedValue(std::to_string(m_nEntriesPerPage));
	registerItemIntegerAttribute("entriesperpage", entriesExpr);

	CUIExpression selectionValueUUIDExpr;
	selectionValueUUIDExpr.setFixedValue(m_sSelectedConfigurationFieldUUID);
	registerItemUUIDAttribute("selectionvalueuuid", selectionValueUUIDExpr);

	CUIExpression buttonValueUUIDExpr;
	buttonValueUUIDExpr.setFixedValue(m_sSelectedButtonFieldUUID);
	registerItemUUIDAttribute("buttonvalueuuid", buttonValueUUIDExpr);

	CUIExpression schemaExpr;
	schemaExpr.setFixedValue(m_sConfigurationSchema);
	registerItemStringAttribute("schema", schemaExpr);
}

void CUIModule_ContentConfigurationList::frontendWriteItemToJSON(CJSONWriter& writer, CJSONWriterObject& itemObject, CUIFrontendState* pFrontendState, CStateMachineData* pStateMachineData)
{
	if (pFrontendState == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
	if (m_pItemModuleStore == nullptr)
		return;

	std::string sItemType = m_pItemModuleStore->getModuleType();
	if (sItemType.empty())
		return;

	itemObject.addString("moduletype", sItemType);
	itemObject.addString("uuid", m_pItemModuleStore->getUUID());

	CJSONWriterObject attributesObject(writer);
	pFrontendState->writeModuleAttributesToJSON(writer, attributesObject, m_pItemModuleStore.get(), pStateMachineData);

	uint64_t nConfigurationListHeadID = 1469598103934665603ULL;
	fnv1aMixString(nConfigurationListHeadID, m_sConfigurationSchema);

	if (m_pDataModel) {
		auto pConfigurationType = m_pDataModel->FindConfigurationTypeBySchema(m_sConfigurationSchema);
		if (pConfigurationType.get() != nullptr) {
			auto pVersionIterator = pConfigurationType->ListAllConfigurationVersions();
			auto pActiveVersion = pConfigurationType->GetActiveConfigurationVersion();
			std::string sActiveUUID;
			if (pActiveVersion.get() != nullptr)
				sActiveUUID = pActiveVersion->GetVersionUUID();

			while (pVersionIterator->MoveNext()) {
				auto pVersion = pVersionIterator->GetCurrent();
				std::string sVersionUUID = pVersion->GetVersionUUID();
				fnv1aMixString(nConfigurationListHeadID, sVersionUUID);
				fnv1aMixString(nConfigurationListHeadID, std::to_string(pVersion->GetNumericVersion()));
				fnv1aMixString(nConfigurationListHeadID, pVersion->GetTimestamp());
				fnv1aMixString(nConfigurationListHeadID, (sVersionUUID == sActiveUUID) ? "1" : "0");
			}
		}
	}

	uint32_t nHeadID = (uint32_t)(nConfigurationListHeadID & 0x7fffffffULL);
	attributesObject.addInteger(AMC_API_KEY_CONFIGURATIONS_HEADID, (int64_t)nHeadID);
	itemObject.addObject("attributes", attributesObject);

	itemObject.addString(AMC_API_KEY_UI_ITEMSELECTIONVALUEUUID, m_sSelectedConfigurationFieldUUID);
	itemObject.addString(AMC_API_KEY_UI_ITEMBUTTONVALUEUUID, m_sSelectedButtonFieldUUID);
	writeButtonsToJSON(writer, itemObject);
}
