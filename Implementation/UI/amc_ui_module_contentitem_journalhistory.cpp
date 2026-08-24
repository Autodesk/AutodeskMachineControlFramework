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

#include "amc_ui_module_contentitem_journalhistory.hpp"
#include "libmc_interfaceexception.hpp"

#include "amc_api_constants.hpp"
#include "Common/common_utils.hpp"
#include "amc_parameterhandler.hpp"

#include "libmcdata_dynamic.hpp"

using namespace AMC;


PUIModule_ContentJournalHistory CUIModule_ContentJournalHistory::makeFromXML(const pugi::xml_node& xmlNode, const std::string& sItemName, const std::string& sModulePath)
{
	auto titleAttrib = xmlNode.attribute("title");
	CUIExpression titleExpression(xmlNode, "title", std::string("Process History"));

	auto defaultVariablesAttrib = xmlNode.attribute("defaultvariables");
	CUIExpression defaultVariablesExpression(xmlNode, "defaultvariables", std::string(""));

	return std::make_shared<CUIModule_ContentJournalHistory>(
		titleAttrib.empty() ? "Process History" : titleAttrib.as_string(),
		titleExpression,
		defaultVariablesAttrib.empty() ? "" : defaultVariablesAttrib.as_string(),
		defaultVariablesExpression,
		sItemName,
		sModulePath
	);
}

CUIModule_ContentJournalHistory::CUIModule_ContentJournalHistory(const std::string& sTitle, const CUIExpression& titleExpression, const std::string& sDefaultVariables, const CUIExpression& defaultVariablesExpression, const std::string& sItemName, const std::string& sModulePath)
	: CUIModule_ContentItem(AMCCommon::CUtils::createUUID(), sItemName, sModulePath),
	  m_sTitle(sTitle),
	  m_sDefaultVariables(sDefaultVariables),
	  m_TitleExpression(titleExpression),
	  m_DefaultVariablesExpression(defaultVariablesExpression)
{
}

CUIModule_ContentJournalHistory::~CUIModule_ContentJournalHistory()
{
}

void CUIModule_ContentJournalHistory::addLegacyContentToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler, uint32_t nStateID)
{
	object.addString(AMC_API_KEY_UI_ITEMTYPE, "journalhistory");
	object.addString(AMC_API_KEY_UI_ITEMUUID, m_sUUID);
	object.addString("title", m_sTitle);
	object.addString("defaultvariables", m_sDefaultVariables);
}

std::string CUIModule_ContentJournalHistory::getItemType()
{
	return "journalhistory";
}

void CUIModule_ContentJournalHistory::registerFrontendAttributes()
{
	registerItemStringAttribute("title", m_TitleExpression);
	registerItemStringAttribute("defaultvariables", m_DefaultVariablesExpression);
}
