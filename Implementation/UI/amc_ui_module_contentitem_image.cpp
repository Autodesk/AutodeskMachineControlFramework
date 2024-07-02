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

#include "amc_ui_module_contentitem_image.hpp"
#include "libmc_interfaceexception.hpp"
#include "amc_api_constants.hpp"
#include "Common/common_utils.hpp"
#include "amc_parameterhandler.hpp"
#include "libmc_exceptiontypes.hpp"
#include "amc_ui_module.hpp"
#include "amc_resourcepackage.hpp"

#include "libmcdata_dynamic.hpp"

using namespace AMC;

PUIModule_ContentImage CUIModule_ContentImage::makeFromXML(const pugi::xml_node& xmlNode, const std::string& sItemName, const std::string& sModulePath, PUIModuleEnvironment pUIModuleEnvironment)
{
	LibMCAssertNotNull(pUIModuleEnvironment);

	CUIExpression imageResourceExpression (xmlNode, "resource", true);
	CUIExpression aspectRatioExpression(xmlNode, "aspectratio", false);
	CUIExpression maxWidthExpression(xmlNode, "maxwidth", false);
	CUIExpression maxHeightExpression(xmlNode, "maxheight", false);

	std::string sUUID = AMCCommon::CUtils::createUUID();

	auto pStateMachineData = pUIModuleEnvironment->stateMachineData();
	auto pResourcePackage = pUIModuleEnvironment->resourcePackage();

	auto pItem = std::make_shared <CUIModule_ContentImage>(sUUID, sItemName, sModulePath, imageResourceExpression, aspectRatioExpression, maxWidthExpression, maxHeightExpression, pStateMachineData, pResourcePackage);
	return pItem;
}


CUIModule_ContentImage::CUIModule_ContentImage(const std::string& sUUID, const std::string& sItemName, const std::string& sModulePath, CUIExpression imageResource, CUIExpression dAspectRatio, CUIExpression maxWidth, CUIExpression maxHeight, PStateMachineData pStateMachineData, PResourcePackage pResourcePackage)
	: CUIModule_ContentItem(sUUID, sItemName, sModulePath),
	  m_ImageResource (imageResource),
	  m_AspectRatio (dAspectRatio),
	  m_MaxWidth (maxWidth),
	  m_MaxHeight (maxHeight),
	  m_pStateMachineData (pStateMachineData),
	  m_pResourcePackage (pResourcePackage)
{
	LibMCAssertNotNull (pStateMachineData)
	LibMCAssertNotNull(pResourcePackage)
}

CUIModule_ContentImage::~CUIModule_ContentImage()
{

}


void CUIModule_ContentImage::addDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler)
{
	object.addString(AMC_API_KEY_UI_ITEMTYPE, "image");
	object.addString(AMC_API_KEY_UI_ITEMUUID, m_sUUID);

	addContentToJSON (writer, object, pClientVariableHandler, 0);

}

void CUIModule_ContentImage::addContentToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler, uint32_t nStateID)
{
	std::string sResourceUUID = AMCCommon::CUtils::createEmptyUUID();

	auto pClientVariableGroup = pClientVariableHandler->findGroup(getItemPath(), true);
	if (m_ImageResource.needsSync())
		pClientVariableGroup->setParameterValueByName(AMC_API_KEY_UI_ITEMIMAGERESOURCE, m_ImageResource.evaluateStringValue(m_pStateMachineData)); 
	if (m_AspectRatio.needsSync())
		pClientVariableGroup->setParameterValueByName(AMC_API_KEY_UI_ITEMASPECTRATIO, m_AspectRatio.evaluateStringValue(m_pStateMachineData));
	if (m_MaxWidth.needsSync())
		pClientVariableGroup->setParameterValueByName(AMC_API_KEY_UI_ITEMMAXWIDTH, m_MaxWidth.evaluateStringValue(m_pStateMachineData));
	if (m_MaxHeight.needsSync())
		pClientVariableGroup->setParameterValueByName(AMC_API_KEY_UI_ITEMMAXHEIGHT, m_MaxHeight.evaluateStringValue(m_pStateMachineData));

	std::string sResourceName = pClientVariableGroup->getParameterValueByName (AMC_API_KEY_UI_ITEMIMAGERESOURCE);
	if (!sResourceName.empty()) {

		bool bIsUUID = AMCCommon::CUtils::stringIsUUIDString(sResourceName);
		if (bIsUUID) {
			sResourceUUID = AMCCommon::CUtils::normalizeUUIDString(sResourceName);
		} else {

			auto pEntry = m_pResourcePackage->findEntryByName(sResourceName, false);
			if (pEntry.get() != nullptr) {
				sResourceUUID = pEntry->getUUID();
			}
		}
	}
	pClientVariableGroup->setParameterValueByName(AMC_API_KEY_UI_ITEMIMAGEUUID, sResourceUUID);

	object.addString(AMC_API_KEY_UI_ITEMIMAGERESOURCE, sResourceUUID);
	object.addString(AMC_API_KEY_UI_ITEMASPECTRATIO, pClientVariableGroup->getParameterValueByName(AMC_API_KEY_UI_ITEMASPECTRATIO));
	if (!m_MaxWidth.isEmpty(m_pStateMachineData))
		object.addString(AMC_API_KEY_UI_ITEMMAXWIDTH, pClientVariableGroup->getParameterValueByName(AMC_API_KEY_UI_ITEMMAXWIDTH));
	if (!m_MaxHeight.isEmpty(m_pStateMachineData))
		object.addString(AMC_API_KEY_UI_ITEMMAXHEIGHT, pClientVariableGroup->getParameterValueByName(AMC_API_KEY_UI_ITEMMAXHEIGHT));

}


void CUIModule_ContentImage::configurePostLoading()
{

}


void CUIModule_ContentImage::populateClientVariables(CParameterHandler* pClientVariableHandler)
{
	LibMCAssertNotNull(pClientVariableHandler);
	auto pGroup = pClientVariableHandler->addGroup(getItemPath(), "upload UI element");
	pGroup->addNewStringParameter(AMC_API_KEY_UI_ITEMIMAGERESOURCE, "Resource Name", m_ImageResource.evaluateStringValue(m_pStateMachineData));
	pGroup->addNewStringParameter(AMC_API_KEY_UI_ITEMIMAGEUUID, "Resource UUID", AMCCommon::CUtils::createEmptyUUID ());
	pGroup->addNewStringParameter(AMC_API_KEY_UI_ITEMASPECTRATIO, "Aspect Ratio", m_AspectRatio.evaluateStringValue (m_pStateMachineData));
	pGroup->addNewStringParameter(AMC_API_KEY_UI_ITEMMAXWIDTH, "Maximum Width", m_MaxWidth.evaluateStringValue(m_pStateMachineData));
	pGroup->addNewStringParameter(AMC_API_KEY_UI_ITEMMAXHEIGHT, "Maximum Height", m_MaxHeight.evaluateStringValue(m_pStateMachineData));
}



std::string CUIModule_ContentImage::findElementPathByUUID(const std::string& sUUID)
{
	if (sUUID == m_sUUID)
		return getItemPath();

	return "";
}
