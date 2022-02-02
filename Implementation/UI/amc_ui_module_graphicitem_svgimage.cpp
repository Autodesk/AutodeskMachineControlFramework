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

#include "amc_ui_module_graphicitem_svgimage.hpp"
#include "libmc_interfaceexception.hpp"

#include "amc_api_constants.hpp"
#include "Common/common_utils.hpp"
#include "amc_parameterhandler.hpp"
#include "libmc_exceptiontypes.hpp"
#include "amc_ui_module.hpp"
#include "amc_resourcepackage.hpp"

#include "libmcdata_dynamic.hpp"


using namespace AMC;

PUIModule_GraphicSVGImage CUIModule_GraphicSVGImage::makeFromXML(const pugi::xml_node& xmlNode, const std::string& sItemName, const std::string& sModulePath, PUIModuleEnvironment pUIModuleEnvironment)
{
	LibMCAssertNotNull(pUIModuleEnvironment);

	CUIExpression resourceName(xmlNode, "resource", true);
	CUIExpression x(xmlNode, "x", "0.0");
	CUIExpression y(xmlNode, "y", "0.0");
	CUIExpression z(xmlNode, "z", "50.0");
	CUIExpression scaleX(xmlNode, "scalex", "1.0");
	CUIExpression scaleY(xmlNode, "scaley", "1.0");
	CUIExpression angle(xmlNode, "angle", "0.0");

	auto pItem = std::make_shared <CUIModule_GraphicSVGImage>(resourceName, x, y, z, scaleX, scaleY, angle, sItemName, sModulePath, pUIModuleEnvironment);
	return pItem;
}

CUIModule_GraphicSVGImage::CUIModule_GraphicSVGImage(CUIExpression resourceName, CUIExpression x, CUIExpression y, CUIExpression z, CUIExpression scaleX, CUIExpression scaleY, CUIExpression angle, const std::string& sItemName, const std::string& sModulePath, PUIModuleEnvironment pUIModuleEnvironment)
	: CUIModuleGraphicItem(AMCCommon::CUtils::createUUID(), sItemName, sModulePath),
	m_ResourceName(resourceName),
	m_X (x),
	m_Y (y),
	m_Z (z),
	m_ScaleX (scaleX),
	m_ScaleY(scaleY),
	m_Angle(angle),
	m_pUIModuleEnvironment (pUIModuleEnvironment)
{
	LibMCAssertNotNull(pUIModuleEnvironment);

}

CUIModule_GraphicSVGImage::~CUIModule_GraphicSVGImage()
{

}


void CUIModule_GraphicSVGImage::addDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler)
{
	auto pStateMachineData = m_pUIModuleEnvironment->stateMachineData();

	object.addString(AMC_API_KEY_UI_ITEMTYPE, "svgimage");
	object.addString(AMC_API_KEY_UI_ITEMNAME, m_sItemName);
	object.addString(AMC_API_KEY_UI_ITEMUUID, m_sUUID);

	std::string sResourceName = m_ResourceName.evaluateStringValue(pStateMachineData);
	auto pResourceEntry = m_pUIModuleEnvironment->resourcePackage()->findEntryByName(sResourceName, false);

	if (pResourceEntry.get() != nullptr) {
		object.addString(AMC_API_KEY_UI_ITEMIMAGEUUID, pResourceEntry->getUUID ());
	}

	object.addDouble(AMC_API_KEY_UI_ITEMX, m_X.evaluateNumberValue(pStateMachineData));
	object.addDouble(AMC_API_KEY_UI_ITEMY, m_Y.evaluateNumberValue (pStateMachineData));
	object.addDouble(AMC_API_KEY_UI_ITEMZ, m_Z.evaluateNumberValue(pStateMachineData));
	object.addDouble(AMC_API_KEY_UI_ITEMSCALEX, m_ScaleX.evaluateNumberValue(pStateMachineData));
	object.addDouble(AMC_API_KEY_UI_ITEMSCALEY, m_ScaleY.evaluateNumberValue(pStateMachineData));
	object.addDouble(AMC_API_KEY_UI_ITEMANGLE, m_Angle.evaluateNumberValue(pStateMachineData));
}
