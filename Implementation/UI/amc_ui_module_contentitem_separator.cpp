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

#include "amc_ui_module_contentitem_separator.hpp"
#include "libmc_interfaceexception.hpp"

#include "amc_api_constants.hpp"
#include "Common/common_utils.hpp"
#include "amc_parameterhandler.hpp"

#include "libmcdata_dynamic.hpp"

using namespace AMC;


PUIModule_ContentSeparator CUIModule_ContentSeparator::makeFromXML(const pugi::xml_node& xmlNode, const std::string& sItemName, const std::string& sModulePath)
{
	auto orientationAttrib = xmlNode.attribute("orientation");
	CUIExpression orientationExpression(xmlNode, "orientation", std::string("horizontal"));

	auto variantAttrib = xmlNode.attribute("variant");
	CUIExpression variantExpression(xmlNode, "variant", std::string("light"));

	return std::make_shared<CUIModule_ContentSeparator>(
		orientationAttrib.empty() ? "horizontal" : orientationAttrib.as_string(),
		orientationExpression,
		variantAttrib.empty() ? "light" : variantAttrib.as_string(),
		variantExpression,
		sItemName,
		sModulePath
	);
}

CUIModule_ContentSeparator::CUIModule_ContentSeparator(const std::string& sOrientation, const CUIExpression& orientationExpression, const std::string& sVariant, const CUIExpression& variantExpression, const std::string& sItemName, const std::string& sModulePath)
	: CUIModule_ContentItem(AMCCommon::CUtils::createUUID(), sItemName, sModulePath),
	  m_sOrientation(sOrientation),
	  m_sVariant(sVariant),
	  m_OrientationExpression(orientationExpression),
	  m_VariantExpression(variantExpression)
{
}

CUIModule_ContentSeparator::~CUIModule_ContentSeparator()
{
}

void CUIModule_ContentSeparator::addLegacyContentToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler, uint32_t nStateID)
{
	object.addString(AMC_API_KEY_UI_ITEMTYPE, "separator");
	object.addString(AMC_API_KEY_UI_ITEMUUID, m_sUUID);
	object.addString("orientation", m_sOrientation);
	object.addString("variant", m_sVariant);
}

std::string CUIModule_ContentSeparator::getItemType()
{
	return "separator";
}

void CUIModule_ContentSeparator::registerFrontendAttributes()
{
	registerItemStringAttribute("orientation", m_OrientationExpression);
	registerItemStringAttribute("variant", m_VariantExpression);
}
