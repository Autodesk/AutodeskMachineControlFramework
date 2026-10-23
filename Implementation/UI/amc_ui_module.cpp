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

#include "amc_ui_module.hpp"
#include "libmc_exceptiontypes.hpp"
#include "Common/common_utils.hpp"
#include "amc_ui_systemstate.hpp"


using namespace AMC;

CUIModuleEnvironment::CUIModuleEnvironment(PUISystemState pUISystemState, CUIModule_ContentRegistry* pContentRegistry, PResourcePackage pResourcePackage, CUIFrontendDefinition* pFrontendDefinition)
	: m_pUISystemState (pUISystemState), m_pContentRegistry (pContentRegistry), m_pResourcePackage (pResourcePackage), m_pFrontendDefinition (pFrontendDefinition)
{
	LibMCAssertNotNull(pUISystemState);
	LibMCAssertNotNull(pContentRegistry);
	LibMCAssertNotNull(pResourcePackage);
	LibMCAssertNotNull(pFrontendDefinition);
}

CUIModuleEnvironment::~CUIModuleEnvironment()
{

}

PStateMachineData CUIModuleEnvironment::stateMachineData()
{
	return m_pUISystemState->getStateMachineData ();
}

PResourcePackage CUIModuleEnvironment::resourcePackage()
{
	return m_pResourcePackage;
}

LibMCData::PDataModel CUIModuleEnvironment::dataModel()
{
	return m_pUISystemState->getDataModel();
}


CUIModule_ContentRegistry* CUIModuleEnvironment::contentRegistry()
{
	return m_pContentRegistry;
}

CLogger* CUIModuleEnvironment::getLogger()
{
	return m_pUISystemState->getLogger().get();
}

	
AMCCommon::PChrono CUIModuleEnvironment::getGlobalChrono()
{
	return m_pUISystemState->getGlobalChronoInstance();
}

CUIFrontendDefinition* CUIModuleEnvironment::getFrontendDefinition()
{
	return m_pFrontendDefinition;
}


PMeshHandler CUIModuleEnvironment::meshHandler()
{
	return m_pUISystemState->getMeshHandler ();
}

PToolpathHandler CUIModuleEnvironment::toolpathHandler()
{
	return m_pUISystemState->getToolpathHandler ();
}

PDataSeriesHandler CUIModuleEnvironment::dataSeriesHandler()
{
	return m_pUISystemState->getDataSeriesHandler ();
}


CUIModule::CUIModule(const std::string& sName, const std::string& sModuleType, const std::string& sParentPath, CUIFrontendDefinition* pFrontendDefinition)
	: m_sName (sName), 
	m_sUUID (AMCCommon::CUtils::createUUID ()),
	m_nGridColumn (1), m_nGridRow (1), m_nGridColumnSpan (1), m_nGridRowSpan (1)

{
	LibMCAssertNotNull(pFrontendDefinition);

	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sName))
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDMODULENAME, sName);
	if (!AMCCommon::CUtils::stringIsValidAlphanumericPathString(sParentPath))
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDMODULEPATH, sParentPath);

	m_sModulePath = sParentPath + "." + sName;

	m_pModuleStore = pFrontendDefinition->registerModuleStore(m_sUUID, m_sModulePath, sModuleType);

	m_pModuleStore->registerStructureProperty("name", [this](CJSONWriter& writer, CJSONWriterObject& object, const std::string& sPropertyName, CStateMachineData* pStateMachineData, CUIFrontendState* pFrontendState) {
		object.addString(sPropertyName, m_sName);
	});

	// Grid placement is only written if it differs from the default cell.
	m_pModuleStore->registerStructureProperty("gridcolumn", [this](CJSONWriter& writer, CJSONWriterObject& object, const std::string& sPropertyName, CStateMachineData* pStateMachineData, CUIFrontendState* pFrontendState) {
		if ((m_nGridColumn > 1) || (m_nGridRow > 1))
			object.addInteger(sPropertyName, m_nGridColumn);
	});
	m_pModuleStore->registerStructureProperty("gridrow", [this](CJSONWriter& writer, CJSONWriterObject& object, const std::string& sPropertyName, CStateMachineData* pStateMachineData, CUIFrontendState* pFrontendState) {
		if ((m_nGridColumn > 1) || (m_nGridRow > 1))
			object.addInteger(sPropertyName, m_nGridRow);
	});
	m_pModuleStore->registerStructureProperty("gridcolumnspan", [this](CJSONWriter& writer, CJSONWriterObject& object, const std::string& sPropertyName, CStateMachineData* pStateMachineData, CUIFrontendState* pFrontendState) {
		if ((m_nGridColumnSpan > 1) || (m_nGridRowSpan > 1))
			object.addInteger(sPropertyName, m_nGridColumnSpan);
	});
	m_pModuleStore->registerStructureProperty("gridrowspan", [this](CJSONWriter& writer, CJSONWriterObject& object, const std::string& sPropertyName, CStateMachineData* pStateMachineData, CUIFrontendState* pFrontendState) {
		if ((m_nGridColumnSpan > 1) || (m_nGridRowSpan > 1))
			object.addInteger(sPropertyName, m_nGridRowSpan);
	});

}

CUIModule::~CUIModule()
{

}

std::string CUIModule::getName()
{
	return m_sName;
}

std::string CUIModule::getUUID()
{
	return m_sUUID;
}

std::string CUIModule::getNameFromXML(pugi::xml_node& xmlNode)
{
	auto nameAttrib = xmlNode.attribute("name");
	if (nameAttrib.empty())
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDMODULENAME);
	return nameAttrib.as_string();
}

std::string CUIModule::getTypeFromXML(pugi::xml_node& xmlNode)
{
	return xmlNode.name();
}

bool CUIModule::isVersion2FrontendModule()
{
	return false;
}

void CUIModule::frontendWriteModuleStatusToJSON(CJSONWriter& writer, CJSONWriterObject& moduleObject, CUIFrontendState* pFrontendState, CStateMachineData* pStateMachineData, const sUIFrontendBuildEpoch& epoch)
{
	if (pFrontendState == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	pFrontendState->writeModuleStoreToJSON(writer, moduleObject, m_pModuleStore.get(), pStateMachineData, epoch);

}

PUIFrontendDefinitionModuleStore CUIModule::getFrontendModuleStore()
{
	return m_pModuleStore;
}


void CUIModule::populateLegacyItemMap(std::map<std::string, PUIModuleItem>& itemMap)
{

}

void CUIModule::populateLegacyClientVariables(CParameterHandler* pParameterHandler)
{

}

PUIModuleItem CUIModule::findLegacyItem(const std::string& sUUID)
{
	return nullptr;
}

void CUIModule::configureLegacyPostLoading()
{

}

std::string CUIModule::getModulePath()
{
	return m_sModulePath;
}

void CUIModule::setGridSpan(uint32_t nGridColumn, uint32_t nGridRow, uint32_t nGridColumnSpan, uint32_t nGridRowSpan)
{
	m_nGridColumn = nGridColumn;
	m_nGridColumnSpan = nGridColumnSpan;
	m_nGridRow = nGridRow;
	m_nGridRowSpan = nGridRowSpan;

}

void CUIModule::getGridSpan(uint32_t& nGridColumn, uint32_t& nGridRow, uint32_t& nGridColumnSpan, uint32_t& nGridRowSpan)
{

	nGridColumn = m_nGridColumn;
	nGridRow = m_nGridRow;	
	nGridColumnSpan = m_nGridColumnSpan;	
	nGridRowSpan = m_nGridRowSpan;

}


PUIFrontendDefinitionAttribute CUIModule::registerUUIDAttribute(const std::string& sAttributeName, const CUIExpression& expression)
{
	return m_pModuleStore->registerValue(sAttributeName, eUIFrontendDefinitionAttributeType::atUUID, expression);
}

PUIFrontendDefinitionAttribute CUIModule::registerIntegerAttribute(const std::string& sAttributeName, const CUIExpression& expression)
{
	return m_pModuleStore->registerValue(sAttributeName, eUIFrontendDefinitionAttributeType::atInteger, expression);
}

PUIFrontendDefinitionAttribute CUIModule::registerNumberAttribute(const std::string& sAttributeName, const CUIExpression& expression)
{
	return m_pModuleStore->registerValue(sAttributeName, eUIFrontendDefinitionAttributeType::atNumber, expression);
}

PUIFrontendDefinitionAttribute CUIModule::registerStringAttribute(const std::string& sAttributeName, const CUIExpression& expression)
{
	return m_pModuleStore->registerValue(sAttributeName, eUIFrontendDefinitionAttributeType::atString, expression);
}

PUIFrontendDefinitionAttribute CUIModule::registerBoolAttribute(const std::string& sAttributeName, const CUIExpression& expression)
{
	return m_pModuleStore->registerValue(sAttributeName, eUIFrontendDefinitionAttributeType::atBoolean, expression);
}

PUIFrontendDefinitionAttribute CUIModule::registerProviderAttribute(const std::string& sAttributeName, eUIFrontendDefinitionAttributeType attributeType, eUIFrontendDefinitionAttributeScope scope, UIFrontendDefinitionProvider provider)
{
	return m_pModuleStore->registerProvider(sAttributeName, attributeType, scope, provider);
}
