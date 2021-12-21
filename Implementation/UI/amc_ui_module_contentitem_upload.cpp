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

#include "amc_ui_module_contentitem_upload.hpp"
#include "libmc_interfaceexception.hpp"

#include "amc_api_constants.hpp"
#include "Common/common_utils.hpp"
#include "amc_parameterhandler.hpp"
#include "amc_statemachinedata.hpp"
#include "amc_ui_module.hpp"
#include "amc_ui_interfaces.hpp"

#include "libmcdata_dynamic.hpp"

#include "libmc_exceptiontypes.hpp"

using namespace AMC;

PUIModule_ContentUpload CUIModule_ContentUpload::makeFromXML(const pugi::xml_node& xmlNode, const std::string& sItemName, const std::string& sModulePath, PUIModuleEnvironment pUIModuleEnvironment)
{
	LibMCAssertNotNull(pUIModuleEnvironment);
	CUIExpression classExpression(xmlNode, "class");
	CUIExpression captionExpression(xmlNode, "caption");

	auto successeventAttrib = xmlNode.attribute("successevent");
	auto failureeventAttrib = xmlNode.attribute("failureevent");

	return std::make_shared <CUIModule_ContentUpload> (pUIModuleEnvironment->contentRegistry(), classExpression, captionExpression, successeventAttrib.as_string (), failureeventAttrib.as_string (), sItemName, sModulePath, pUIModuleEnvironment->stateMachineData ());
}



CUIModule_ContentUpload::CUIModule_ContentUpload(CUIModule_ContentRegistry* pOwner, CUIExpression uploadClass, CUIExpression uploadCaption, const std::string& sSuccessEvent, const std::string& sFailureEvent, const std::string& sItemName, const std::string& sModulePath, PStateMachineData pStateMachineData)
	: CUIModule_ContentItem(AMCCommon::CUtils::createUUID(), sItemName, sModulePath), m_UploadClass(uploadClass), m_UploadCaption (uploadCaption), m_sSuccessEvent (sSuccessEvent), m_sFailureEvent (sFailureEvent), m_pStateMachineData (pStateMachineData), m_pOwner (pOwner)
{
	LibMCAssertNotNull(pStateMachineData);
	LibMCAssertNotNull(pOwner);

}

CUIModule_ContentUpload::~CUIModule_ContentUpload()
{

}

void CUIModule_ContentUpload::addDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler)
{
	object.addString(AMC_API_KEY_UI_ITEMTYPE, "upload");
	object.addString(AMC_API_KEY_UI_ITEMUUID, m_sUUID);
	object.addString(AMC_API_KEY_UI_ITEMUPLOADCLASS, m_UploadClass.evaluateStringValue (m_pStateMachineData));
	object.addString(AMC_API_KEY_UI_ITEMUPLOADCAPTION, m_UploadCaption.evaluateStringValue (m_pStateMachineData));
	object.addInteger(AMC_API_KEY_UI_ITEMUPLOADISINITIAL, 1);
	object.addInteger(AMC_API_KEY_UI_ITEMUPLOADISSAVING, 0);
	object.addString(AMC_API_KEY_UI_ITEMUPLOADFILENAME, "");
	object.addString(AMC_API_KEY_UI_ITEMUPLOADSUCCESSEVENT, m_sSuccessEvent);
	object.addString(AMC_API_KEY_UI_ITEMUPLOADFAILUREEVENT, m_sFailureEvent);
}


void CUIModule_ContentUpload::configurePostLoading()
{
	if (!m_sSuccessEvent.empty())
		m_pOwner->ensureUIEventExists(m_sSuccessEvent);
	if (!m_sFailureEvent.empty())
		m_pOwner->ensureUIEventExists(m_sFailureEvent);

}


void CUIModule_ContentUpload::populateClientVariables(CParameterHandler* pClientVariableHandler) 
{
	LibMCAssertNotNull(pClientVariableHandler);
	auto pGroup = pClientVariableHandler->addGroup(getItemPath(), "upload UI element");
	pGroup->addNewStringParameter("uploaduuid", "uploaded UUID", AMCCommon::CUtils::createEmptyUUID ());
}

void CUIModule_ContentUpload::setEventPayloadValue(const std::string& sEventName, const std::string& sPayloadUUID, const std::string& sPayloadValue, CParameterHandler* pClientVariableHandler)
{
	LibMCAssertNotNull(pClientVariableHandler);
	if (AMCCommon::CUtils::normalizeUUIDString (sPayloadUUID) == getUUID()) {
		auto pGroup = pClientVariableHandler->findGroup(getItemPath(), true);
		pGroup->setParameterValueByName("uploaduuid", AMCCommon::CUtils::normalizeUUIDString (sPayloadValue));

	}

}
