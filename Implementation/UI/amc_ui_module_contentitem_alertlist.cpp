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

#include "amc_ui_module_contentitem_alertlist.hpp"
#include "libmc_interfaceexception.hpp"

#include "amc_api_constants.hpp"
#include "Common/common_utils.hpp"
#include "amc_parameterhandler.hpp"
#include "amc_statemachinedata.hpp"
#include "amc_ui_module.hpp"
#include "libmc_exceptiontypes.hpp"
#include "libmcdata_dynamic.hpp"

using namespace AMC;

PUIModule_ContentAlertList CUIModule_ContentAlertList::makeFromXML(const pugi::xml_node& xmlNode, const std::string& sItemName, const std::string& sModulePath, PUIModuleEnvironment pUIModuleEnvironment)
{
	LibMCAssertNotNull(pUIModuleEnvironment);

	auto loadingtextAttrib = xmlNode.attribute("loadingtext");
	auto entriesperpageAttrib = xmlNode.attribute("entriesperpage");
	auto selectEventAttrib = xmlNode.attribute("selectevent");
	std::string sLoadingText = loadingtextAttrib.as_string();
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

	return std::make_shared <CUIModule_ContentAlertList>(sLoadingText, nEntriesPerPage, sSelectEvent, pUIModuleEnvironment->dataModel(), sItemName, sModulePath);

}

CUIModule_ContentAlertList::CUIModule_ContentAlertList(const std::string& sLoadingText, const uint32_t nEntriesPerPage, const std::string& sSelectEvent, LibMCData::PDataModel pDataModel, const std::string& sItemName, const std::string& sModulePath)
	: CUIModule_ContentItem(AMCCommon::CUtils::createUUID(), sItemName, sModulePath), m_sLoadingText(sLoadingText), m_nEntriesPerPage(nEntriesPerPage), m_sSelectEvent(sSelectEvent), m_pDataModel (pDataModel)
{
	if (sModulePath.empty ())
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDMODULEPATH);
	if (sItemName.empty ())
		throw ELibMCInterfaceException(LIBMC_ERROR_ALERTLISTNAMEMISSING);
	if (pDataModel.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	m_sAlertTimeStampCaption = "Time";
	m_sAlertNameCaption = "Alert Name";
	m_sAlertDescriptionCaption = "Alert Description";
	m_sAlertContextCaption = "Context";
	m_sAlertAcknowledgeCaption = "Acknowledgement";

	m_sSelectedBuildField = AMCCommon::CUtils::createUUID();

}

CUIModule_ContentAlertList::~CUIModule_ContentAlertList()
{

}

void CUIModule_ContentAlertList::addDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler)
{

	object.addString(AMC_API_KEY_UI_ITEMTYPE, "alertlist");
	object.addString(AMC_API_KEY_UI_ITEMUUID, m_sUUID);
	object.addString(AMC_API_KEY_UI_ITEMLOADINGTEXT, m_sLoadingText);
	object.addString(AMC_API_KEY_UI_ITEMSELECTEVENT, m_sSelectEvent);
	object.addString(AMC_API_KEY_UI_ITEMSELECTIONVALUEUUID, m_sSelectedBuildField);

	object.addInteger(AMC_API_KEY_UI_ITEMENTRIESPERPAGE, m_nEntriesPerPage);

	CJSONWriterArray headersArray(writer);


	CJSONWriterObject headerObject1(writer);
	headerObject1.addString(AMC_API_KEY_UI_ITEMTEXT, m_sAlertTimeStampCaption);
	headerObject1.addString(AMC_API_KEY_UI_ITEMVALUE, AMC_API_KEY_UI_ITEMALERTTIMESTAMP);
	headersArray.addObject(headerObject1);

	CJSONWriterObject headerObject2(writer);
	headerObject2.addString(AMC_API_KEY_UI_ITEMTEXT, m_sAlertNameCaption);
	headerObject2.addString(AMC_API_KEY_UI_ITEMVALUE, AMC_API_KEY_UI_ITEMALERTCAPTION);
	headersArray.addObject(headerObject2);

	CJSONWriterObject headerObject3(writer);
	headerObject3.addString(AMC_API_KEY_UI_ITEMTEXT, m_sAlertDescriptionCaption);
	headerObject3.addString(AMC_API_KEY_UI_ITEMVALUE, AMC_API_KEY_UI_ITEMALERTDESCRIPTION);
	headersArray.addObject(headerObject3);

	CJSONWriterObject headerObject4(writer);
	headerObject4.addString(AMC_API_KEY_UI_ITEMTEXT, m_sAlertContextCaption);
	headerObject4.addString(AMC_API_KEY_UI_ITEMVALUE, AMC_API_KEY_UI_ITEMALERTCONTEXT);
	headersArray.addObject(headerObject4);

	CJSONWriterObject headerObject5(writer);
	headerObject5.addString(AMC_API_KEY_UI_ITEMTEXT, m_sAlertAcknowledgeCaption);
	headerObject5.addString(AMC_API_KEY_UI_ITEMVALUE, AMC_API_KEY_UI_ITEMALERTACKNOWLEDGE);
	headersArray.addObject(headerObject5);

	object.addArray(AMC_API_KEY_UI_ITEMHEADERS, headersArray);

	CJSONWriterArray entriesArray(writer);
	object.addArray(AMC_API_KEY_UI_ITEMENTRIES, entriesArray);
}

void CUIModule_ContentAlertList::addContentToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler, uint32_t nStateID)
{

	CJSONWriterArray entryArray(writer);

	{

		CJSONWriterObject entryObject(writer);
		entryObject.addString(AMC_API_KEY_UI_ITEMALERTTIMESTAMP, "12:30:23");
		entryObject.addString(AMC_API_KEY_UI_ITEMALERTCAPTION, "Alert Test");
		entryObject.addString(AMC_API_KEY_UI_ITEMALERTDESCRIPTION, "Alert Description");
		entryObject.addString(AMC_API_KEY_UI_ITEMALERTCONTEXT, "Alert Context");
		entryObject.addString(AMC_API_KEY_UI_ITEMALERTACKNOWLEDGE, "Alert Acknowledge");

		entryArray.addObject(entryObject);

	}

	object.addArray(AMC_API_KEY_UI_ITEMENTRIES, entryArray);
}


void CUIModule_ContentAlertList::populateClientVariables(CParameterHandler* pClientVariableHandler)
{
	LibMCAssertNotNull(pClientVariableHandler);
	auto pGroup = pClientVariableHandler->addGroup(getItemPath(), "build list UI element");
	pGroup->addNewStringParameter("selecteduuid", "selected build UUID", AMCCommon::CUtils::createEmptyUUID());
}

void CUIModule_ContentAlertList::setEventPayloadValue(const std::string& sEventName, const std::string& sPayloadUUID, const std::string& sPayloadValue, CParameterHandler* pClientVariableHandler)
{
	LibMCAssertNotNull(pClientVariableHandler);
	if (AMCCommon::CUtils::normalizeUUIDString(sPayloadUUID) == m_sSelectedBuildField) {
		auto pGroup = pClientVariableHandler->findGroup(getItemPath(), true);
		pGroup->setParameterValueByName("selecteduuid", AMCCommon::CUtils::normalizeUUIDString(sPayloadValue));

	}

}


std::string CUIModule_ContentAlertList::findElementPathByUUID(const std::string& sUUID)
{
	if (sUUID == m_sSelectedBuildField)
		return getItemPath();
	if (sUUID == getUUID ())
		return getItemPath();

	return "";
}


std::list <std::string> CUIModule_ContentAlertList::getReferenceUUIDs()
{
	std::list <std::string> sUUIDList;
	sUUIDList.push_back(m_sSelectedBuildField);
	sUUIDList.push_back(getUUID ());

	return sUUIDList;
}
