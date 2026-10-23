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

#define __AMCIMPL_UI_PAGE
#define __AMCIMPL_UI_MODULE

#include "amc_ui_page.hpp"
#include "amc_ui_module.hpp"
#include "amc_ui_frontendstate.hpp"
#include "amc_parameterhandler.hpp"
#include "libmc_exceptiontypes.hpp"
#include "common_utils.hpp"

#define __AMCIMPL_API_CONSTANTS
#include "amc_api_constants.hpp"

#include "amc_ui_module_contentitem_form.hpp"

using namespace AMC;


CUIPage::CUIPage(const std::string& sName, CUIModule_UIEventHandler* pUIEventHandler, const CUIExpression& icon, const CUIExpression& caption, const CUIExpression& description, const std::string& sShowEvent)
	: m_sName(sName), m_pUIEventHandler(pUIEventHandler), m_nGridColumns(1), m_nGridRows(1), m_Icon(icon), m_Description(description), m_Caption (caption), m_sShowEvent(sShowEvent)
{
	if (sName.empty())
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
	LibMCAssertNotNull(pUIEventHandler);

	m_sUUID = AMCCommon::CUtils::createUUID();

	m_Visible.setFixedValue("1");

}

CUIPage::~CUIPage()
{

}

std::string CUIPage::getName()
{
	return m_sName;
}

std::string CUIPage::getUUID()
{
	return m_sUUID;
}

std::string CUIPage::getShowEvent()
{
	return m_sShowEvent;
}

void CUIPage::addModule(PUIModule pModule)
{
	if (pModule.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	auto sName = pModule->getName();
	if (sName.empty ())
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDMODULENAME);

	m_Modules.push_back(pModule);
	m_ModuleMapOfPage.insert(std::make_pair(pModule->getUUID(), pModule));

	pModule->populateLegacyItemMap(m_ItemMapOfPage);

	pModule->populateModuleMap(m_ModuleMapOfPage);
	
}

void CUIPage::configurePostLoading()
{
	for (auto pModule : m_Modules)
		pModule->configureLegacyPostLoading();
}


uint32_t CUIPage::getModuleCount()
{
	return (uint32_t)m_Modules.size();
}

PUIModule CUIPage::getModule(const uint32_t nIndex)
{
	if (nIndex >= m_Modules.size())
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDINDEX);

	return m_Modules.at (nIndex);
}

void CUIPage::ensureUIEventExists(const std::string& sEventName)
{
	m_pUIEventHandler->ensureUIEventExists(sEventName);
}


/////////////////////////////////////////////////////////////////////////////////////
// Legacy UI System
/////////////////////////////////////////////////////////////////////////////////////

PUIModule CUIPage::findModuleByUUID(const std::string& sUUID)
{
	auto iIter = m_ModuleMapOfPage.find(sUUID);
	if (iIter != m_ModuleMapOfPage.end())
		return iIter->second;

	return nullptr;
}

PUIModuleItem CUIPage::findModuleItemByUUID(const std::string& sUUID)
{
	auto iIter = m_ItemMapOfPage.find (sUUID);
	if (iIter != m_ItemMapOfPage.end())
		return iIter->second;

	return nullptr;

}

void CUIPage::registerFormName(const std::string& sFormUUID, const std::string& sFormName)
{
	auto sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sFormUUID);

	auto iIter = m_FormNameMap.find(sFormName);
	if (iIter != m_FormNameMap.end())
		throw ELibMCCustomException(LIBMC_ERROR_DUPLICATEFORMNAME, sFormName);

	m_FormNameMap.insert(std::make_pair(sFormName, sNormalizedUUID));

}

std::string CUIPage::findFormUUIDByName(const std::string& sFormName)
{
	auto iIter = m_FormNameMap.find(sFormName);
	if (iIter != m_FormNameMap.end()) {
		return iIter->second;
	}
	return "";
}

void CUIPage::populateClientVariables(CParameterHandler* pParameterHandler)
{
	LibMCAssertNotNull(pParameterHandler);
	auto pGroup = pParameterHandler->addGroup(m_sName, "page");
	pGroup->addNewStringParameter ("custom", "custom page parameter", "");

	for (auto pModule : m_Modules) {
		pModule->populateLegacyClientVariables(pParameterHandler);
	}
}


/////////////////////////////////////////////////////////////////////////////////////
// New UI Frontend System
/////////////////////////////////////////////////////////////////////////////////////

void CUIPage::frontendWritePageStatusToJSON(CJSONWriter& writer, CJSONWriterObject& pageObject, CUIFrontendState* pFrontendState, CStateMachineData* pStateMachineData, bool bWriteModules, const sUIFrontendBuildEpoch& epoch)
{

	pageObject.addString("name", m_sName);
	pageObject.addString("uuid", AMCCommon::CUtils::normalizeUUIDString(m_sUUID));

	if (!m_Caption.isEmpty (pStateMachineData, pFrontendState))
		pageObject.addString("caption", m_Caption.evaluateStringValue (pStateMachineData, pFrontendState));
	if (!m_Description.isEmpty(pStateMachineData, pFrontendState))
		pageObject.addString("description", m_Description.evaluateStringValue (pStateMachineData, pFrontendState));
	if (!m_Icon.isEmpty(pStateMachineData, pFrontendState))
		pageObject.addString("icon", m_Icon.evaluateStringValue (pStateMachineData, pFrontendState));
	pageObject.addBool("visible", m_Visible.evaluateBoolValue(pStateMachineData, pFrontendState));

	pageObject.addInteger("gridcolumns", m_nGridColumns);
	pageObject.addInteger("gridrows", m_nGridRows);

	if (!bWriteModules)
		return;

	CJSONWriterArray moduleArray(writer);

	for (auto pModule : m_Modules) {

		if (pModule->isVersion2FrontendModule()) {
			CJSONWriterObject moduleObject(writer);

			pModule->frontendWriteModuleStatusToJSON(writer, moduleObject, pFrontendState, pStateMachineData, epoch);

			moduleArray.addObject(moduleObject);
		}
	}


	pageObject.addArray("modules", moduleArray);

}

void CUIPage::setVisibleExpression(const CUIExpression& visibleExpression)
{
	m_Visible = visibleExpression;
}

bool CUIPage::isVisible(CUIFrontendState* pFrontendState, CStateMachineData* pStateMachineData)
{
	return m_Visible.evaluateBoolValue(pStateMachineData, pFrontendState);
}

void CUIPage::collectSessionReferences(std::vector<std::string>& references)
{
	for (auto pExpression : { &m_Icon, &m_Caption, &m_Description, &m_Visible }) {
		std::string sReference = pExpression->getSessionReference();
		if (!sReference.empty())
			references.push_back(sReference);
	}
}

