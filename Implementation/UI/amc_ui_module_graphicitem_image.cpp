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

#include "amc_ui_module_graphicitem_image.hpp"
#include "libmc_interfaceexception.hpp"

#include "amc_api_constants.hpp"
#include "Common/common_utils.hpp"
#include "amc_parameterhandler.hpp"
#include "libmc_exceptiontypes.hpp"
#include "amc_ui_module.hpp"
#include "amc_resourcepackage.hpp"

#include "libmcdata_dynamic.hpp"


using namespace AMC;

PUIModule_GraphicImage CUIModule_GraphicImage::makeFromXML(const pugi::xml_node& xmlNode, const std::string& sItemName, const std::string& sModulePath, PUIModuleEnvironment pUIModuleEnvironment)
{
	LibMCAssertNotNull(pUIModuleEnvironment);

	CUIExpression resourceName(xmlNode, "resource");
	CUIExpression x(xmlNode, "x");
	CUIExpression y(xmlNode, "y");
	CUIExpression maxWidth(xmlNode, "maxwidth");
	CUIExpression maxHeight(xmlNode, "maxheight");

	auto pItem = std::make_shared <CUIModule_GraphicImage>(resourceName, x, y, maxWidth, maxHeight, sItemName, sModulePath, pUIModuleEnvironment);
	return pItem;
}


CUIModule_GraphicImage::CUIModule_GraphicImage(CUIExpression resourceName, CUIExpression x, CUIExpression y, CUIExpression maxWidth, CUIExpression maxHeight, const std::string& sItemName, const std::string& sModulePath, PUIModuleEnvironment pUIModuleEnvironment)
	: CUIModuleGraphicItem(AMCCommon::CUtils::createUUID(), sItemName, sModulePath),
	m_ResourceName(resourceName),
	m_X (x),
	m_Y (y),
	m_MaxWidth (maxWidth),
	m_MaxHeight (maxHeight),
	m_pUIModuleEnvironment (pUIModuleEnvironment)
{
	LibMCAssertNotNull(pUIModuleEnvironment);

}

CUIModule_GraphicImage::~CUIModule_GraphicImage()
{

}


void CUIModule_GraphicImage::addContentToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler, uint32_t nStateID)
{
	auto pStateMachineData = m_pUIModuleEnvironment->stateMachineData();

	object.addString(AMC_API_KEY_UI_ITEMTYPE, "image");
	object.addString(AMC_API_KEY_UI_ITEMUUID, m_sUUID);

	std::string sResourceName = m_ResourceName.evaluateStringValue(pStateMachineData);
	auto pResourceEntry = m_pUIModuleEnvironment->resourcePackage()->findEntryByName(sResourceName, false);

	if (pResourceEntry.get() != nullptr) {
		object.addString(AMC_API_KEY_UI_ITEMIMAGEUUID, pResourceEntry->getUUID ());
	}
	if (!m_MaxWidth.isEmpty (pStateMachineData))
		object.addDouble(AMC_API_KEY_UI_ITEMMAXWIDTH, m_MaxWidth.evaluateNumberValue(pStateMachineData));
	if (!m_MaxHeight.isEmpty(pStateMachineData))
		object.addDouble(AMC_API_KEY_UI_ITEMMAXHEIGHT, m_MaxHeight.evaluateNumberValue(pStateMachineData));

	object.addDouble(AMC_API_KEY_UI_ITEMX, m_X.evaluateNumberValue(pStateMachineData));
	object.addDouble(AMC_API_KEY_UI_ITEMY, m_Y.evaluateNumberValue (pStateMachineData));
}
