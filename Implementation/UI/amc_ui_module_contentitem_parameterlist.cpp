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

#include "amc_ui_module_contentitem_parameterlist.hpp"
#include "libmc_interfaceexception.hpp"

#include "amc_api_constants.hpp"
#include "Common/common_utils.hpp"
#include "amc_parameterhandler.hpp"
#include "amc_parameterinstances.hpp"

#include "libmcdata_dynamic.hpp"

using namespace AMC;




CUIModule_ContentParameterListEntry::CUIModule_ContentParameterListEntry(const std::string& sStateMachine, const std::string& sParameterGroup, const std::string& sParameter)
	: m_sStateMachine(sStateMachine), m_sParameterGroup(sParameterGroup), m_sParameter(sParameter)
{

}

CUIModule_ContentParameterListEntry::~CUIModule_ContentParameterListEntry()
{

}

std::string CUIModule_ContentParameterListEntry::getStateMachine()
{
	return m_sStateMachine;
}

std::string CUIModule_ContentParameterListEntry::getParameterGroup()
{
	return m_sParameterGroup;
}

std::string CUIModule_ContentParameterListEntry::getParameter()
{
	return m_sParameter;
}

bool CUIModule_ContentParameterListEntry::isFullGroup()
{
	return m_sParameter.empty ();
}



CUIModule_ContentParameterList::CUIModule_ContentParameterList(const std::string& sLoadingText, const uint32_t nEntriesPerPage, PParameterInstances pParameterInstances)
	: CUIModule_ContentItem (AMCCommon::CUtils::createUUID()), m_sLoadingText (sLoadingText), m_nEntriesPerPage (nEntriesPerPage), m_pParameterInstances(pParameterInstances)
{
	if (pParameterInstances.get() == nullptr)
		throw ELibMCInterfaceException (LIBMC_ERROR_INVALIDPARAM);

	m_sParameterDescCaption = "Parameter";
	m_sParameterValueCaption = "Value";
	m_sParameterGroupCaption = "Group";
	m_sParameterSystemCaption = "System";

}

CUIModule_ContentParameterList::~CUIModule_ContentParameterList()
{

}

void CUIModule_ContentParameterList::addDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& object)
{
	object.addString(AMC_API_KEY_UI_ITEMTYPE, "parameterlist");
	object.addString(AMC_API_KEY_UI_ITEMUUID, m_sUUID);
	object.addString(AMC_API_KEY_UI_ITEMLOADINGTEXT, m_sLoadingText);
	object.addInteger(AMC_API_KEY_UI_ITEMENTRIESPERPAGE, m_nEntriesPerPage);
	
	CJSONWriterArray headersArray(writer);	

	CJSONWriterObject headerObject1(writer);
	headerObject1.addString(AMC_API_KEY_UI_ITEMTEXT, m_sParameterDescCaption);
	headerObject1.addString(AMC_API_KEY_UI_ITEMVALUE, AMC_API_KEY_UI_ITEMPARAMETERDESCRIPTION);
	headersArray.addObject(headerObject1);

	CJSONWriterObject headerObject2(writer);
	headerObject2.addString(AMC_API_KEY_UI_ITEMTEXT, m_sParameterValueCaption);
	headerObject2.addString(AMC_API_KEY_UI_ITEMVALUE, AMC_API_KEY_UI_ITEMPARAMETERVALUE);
	headersArray.addObject(headerObject2);

	CJSONWriterObject headerObject3(writer);
	headerObject3.addString(AMC_API_KEY_UI_ITEMTEXT, m_sParameterGroupCaption);
	headerObject3.addString(AMC_API_KEY_UI_ITEMVALUE, AMC_API_KEY_UI_ITEMPARAMETERGROUP);
	headersArray.addObject(headerObject3);

	CJSONWriterObject headerObject4(writer);
	headerObject4.addString(AMC_API_KEY_UI_ITEMTEXT, m_sParameterSystemCaption);
	headerObject4.addString(AMC_API_KEY_UI_ITEMVALUE, AMC_API_KEY_UI_ITEMPARAMETERSYSTEM);
	headersArray.addObject(headerObject4);

	object.addArray(AMC_API_KEY_UI_ITEMHEADERS, headersArray);

	CJSONWriterArray entriesArray(writer);
	object.addArray(AMC_API_KEY_UI_ITEMENTRIES, entriesArray);

}


void CUIModule_ContentParameterList::addContentToJSON(CJSONWriter& writer, CJSONWriterObject& object)
{

	CJSONWriterArray entryArray(writer);

	for (auto entry : m_List) {
		auto pParameterHandler = m_pParameterInstances->getParameterHandler(entry->getStateMachine ());
		auto sParameterHandlerDescription = pParameterHandler->getDescription();

		auto pParameterGroup = pParameterHandler->findGroup(entry->getParameterGroup(), true);

		std::string sGroupDescription = pParameterGroup->getDescription();

		if (entry->isFullGroup()) {
			
			uint32_t nCount = pParameterGroup->getParameterCount();
			for (uint32_t nIndex = 0; nIndex < nCount; nIndex++) {

				std::string sParameterName;
				std::string sDescription;
				std::string sDefaultValue;

				pParameterGroup->getParameterInfo(nIndex, sParameterName, sDescription, sDefaultValue);
				std::string sValue = pParameterGroup->getParameterValueByIndex (nIndex);

				CJSONWriterObject entryObject(writer);
				entryObject.addString(AMC_API_KEY_UI_ITEMPARAMETERDESCRIPTION, sDescription);
				entryObject.addString(AMC_API_KEY_UI_ITEMPARAMETERVALUE, sValue);
				entryObject.addString(AMC_API_KEY_UI_ITEMPARAMETERGROUP, sGroupDescription);
				entryObject.addString(AMC_API_KEY_UI_ITEMPARAMETERSYSTEM, sParameterHandlerDescription);
				entryArray.addObject(entryObject);

			}
		}
		else {
			std::string sParameterName = entry->getParameter();
			std::string sDescription;
			std::string sDefaultValue;
			std::string sValue;

			pParameterGroup->getParameterInfoByName(sParameterName, sDescription, sDefaultValue);
			sValue = pParameterGroup->getParameterValueByName(sParameterName);

			CJSONWriterObject entryObject(writer);
			entryObject.addString (AMC_API_KEY_UI_ITEMPARAMETERDESCRIPTION, sDescription);
			entryObject.addString(AMC_API_KEY_UI_ITEMPARAMETERVALUE, sValue);
			entryObject.addString(AMC_API_KEY_UI_ITEMPARAMETERGROUP, sGroupDescription);
			entryObject.addString(AMC_API_KEY_UI_ITEMPARAMETERSYSTEM, sParameterHandlerDescription);
			entryArray.addObject(entryObject);

		}


	}

	object.addArray (AMC_API_KEY_UI_ITEMENTRIES, entryArray);

}


void CUIModule_ContentParameterList::addEntry(const std::string& sStateMachine, const std::string& sParameterGroup, const std::string& sParameter)
{
	if (m_List.size() >= AMC_CONTENT_MAXENTRYCOUNT)
		throw ELibMCInterfaceException(LIBMC_ERROR_TOOMANYCONTENTPARAMETERS);

	m_List.push_back (std::make_shared <CUIModule_ContentParameterListEntry> (sStateMachine, sParameterGroup, sParameter));
}


uint32_t CUIModule_ContentParameterList::getEntryCount()
{
	return (uint32_t)m_List.size();
}

CUIModule_ContentParameterListEntry* CUIModule_ContentParameterList::getEntry(const uint32_t nIndex)
{
	if (nIndex >= m_List.size())
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDINDEX);

	return m_List[nIndex].get();
}


void CUIModule_ContentParameterList::loadFromXML(pugi::xml_node& xmlNode)
{
	auto entryNodes = xmlNode.children ("entry");
	for (auto entryNode : entryNodes) {

		auto stateMachineAttrib = entryNode.attribute("statemachine");
		if (stateMachineAttrib.empty ())
			throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGCONTENTSTATEMACHINENAME);
		std::string sStateMachineName = stateMachineAttrib.as_string();

		auto groupAttrib = entryNode.attribute("group");
		if (groupAttrib.empty())
			throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGCONTENTGROUPNAME);
		std::string sGroupName = groupAttrib.as_string();

		auto pStateMachineInstance = m_pParameterInstances->getParameterHandler(sStateMachineName);
		pStateMachineInstance->findGroup(sGroupName, true);

		// Parameter may be empty (then add full group)
		auto parameterAttrib = entryNode.attribute("parameter");
		addEntry(stateMachineAttrib.as_string(), groupAttrib.as_string(), parameterAttrib.as_string());

	}
}

