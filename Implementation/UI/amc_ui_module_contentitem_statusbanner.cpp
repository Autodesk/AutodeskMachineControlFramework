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

#include "amc_ui_module_contentitem_statusbanner.hpp"
#include "libmc_interfaceexception.hpp"

#include "amc_api_constants.hpp"
#include "Common/common_utils.hpp"
#include "amc_parameterhandler.hpp"

#include "libmcdata_dynamic.hpp"

using namespace AMC;


PUIModule_ContentStatusBanner CUIModule_ContentStatusBanner::makeFromXML(const pugi::xml_node& xmlNode, const std::string& sItemName, const std::string& sModulePath)
{
	auto severityAttrib = xmlNode.attribute("severity");
	CUIExpression severityExpression(xmlNode, "severity", std::string("ok"));

	auto titleAttrib = xmlNode.attribute("title");
	CUIExpression titleExpression(xmlNode, "title", std::string(""));

	auto descriptionAttrib = xmlNode.attribute("description");
	CUIExpression descriptionExpression(xmlNode, "description", std::string(""));

	return std::make_shared<CUIModule_ContentStatusBanner>(
		severityAttrib.empty() ? "ok" : severityAttrib.as_string(),
		severityExpression,
		titleAttrib.as_string(),
		titleExpression,
		descriptionAttrib.as_string(),
		descriptionExpression,
		sItemName,
		sModulePath
	);
}

CUIModule_ContentStatusBanner::CUIModule_ContentStatusBanner(
	const std::string& sSeverity, const CUIExpression& severityExpression,
	const std::string& sTitle, const CUIExpression& titleExpression,
	const std::string& sDescription, const CUIExpression& descriptionExpression,
	const std::string& sItemName, const std::string& sModulePath)
	: CUIModule_ContentItem(AMCCommon::CUtils::createUUID(), sItemName, sModulePath),
	  m_sSeverity(sSeverity),
	  m_sTitle(sTitle),
	  m_sDescription(sDescription),
	  m_SeverityExpression(severityExpression),
	  m_TitleExpression(titleExpression),
	  m_DescriptionExpression(descriptionExpression)
{
}

CUIModule_ContentStatusBanner::~CUIModule_ContentStatusBanner()
{
}

void CUIModule_ContentStatusBanner::addLegacyContentToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler, uint32_t nStateID)
{
	object.addString(AMC_API_KEY_UI_ITEMTYPE, "statusbanner");
	object.addString(AMC_API_KEY_UI_ITEMUUID, m_sUUID);
	object.addString("severity", m_sSeverity);
	object.addString("title", m_sTitle);
	object.addString("description", m_sDescription);
}

std::string CUIModule_ContentStatusBanner::getItemType()
{
	return "statusbanner";
}

void CUIModule_ContentStatusBanner::registerFrontendAttributes()
{
	registerItemStringAttribute("severity", m_SeverityExpression);
	registerItemStringAttribute("title", m_TitleExpression);
	registerItemStringAttribute("description", m_DescriptionExpression);
}
