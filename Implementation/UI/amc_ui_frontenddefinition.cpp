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

#include "amc_ui_frontenddefinition.hpp"
#include "amc_ui_frontendstate.hpp"
#include "amc_parametergroup.hpp"
#include "libmc_exceptiontypes.hpp"
#include "common_utils.hpp"

#include <chrono>

using namespace AMC;

#define AMC_UI_SESSIONVARIABLES_GROUPNAME "session"


sUIFrontendBuildEpoch::sUIFrontendBuildEpoch()
	: m_nChangeCounter(0), m_nTimeSlot(0)
{
}

sUIFrontendBuildEpoch::sUIFrontendBuildEpoch(uint64_t nChangeCounter, uint64_t nTimeSlot)
	: m_nChangeCounter(nChangeCounter), m_nTimeSlot(nTimeSlot)
{
}

bool sUIFrontendBuildEpoch::operator==(const sUIFrontendBuildEpoch& other) const
{
	return (m_nChangeCounter == other.m_nChangeCounter) && (m_nTimeSlot == other.m_nTimeSlot);
}

uint64_t sUIFrontendBuildEpoch::currentTimeSlot()
{
	auto nMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
	return (uint64_t)nMilliseconds / AMC_UI_FRONTEND_EPOCH_TIMESLOT_MS;
}


CUIFrontendDefinitionAttribute::CUIFrontendDefinitionAttribute(const std::string& sName, eUIFrontendDefinitionAttributeType attributeType)
	: m_sName(sName), m_AttributeType(attributeType)
{
	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sName))
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDFRONTENDATTRIBUTENAME, sName);
}

CUIFrontendDefinitionAttribute::~CUIFrontendDefinitionAttribute()
{

}

void CUIFrontendDefinitionAttribute::writeToFrontendJSONForEpoch(CJSONWriter& writer, CJSONWriterObject& attributesObject, CStateMachineData* pStateMachineData, CUIFrontendState* pFrontendState, const sUIFrontendBuildEpoch& epoch)
{
	if (isSessionScoped()) {
		writeToFrontendJSON(writer, attributesObject, pStateMachineData, pFrontendState);
		return;
	}

	{
		std::lock_guard<std::mutex> lockGuard(m_SharedValueMutex);
		if ((m_pSharedValue.get() != nullptr) && (m_SharedValueEpoch == epoch)) {
			attributesObject.addMembersFrom(*m_pSharedValue);
			return;
		}
	}

	CJSONWriterObject valueObject(writer);
	writeToFrontendJSON(writer, valueObject, pStateMachineData, pFrontendState);

	auto pSharedValue = std::make_unique<rapidjson::Document>();
	pSharedValue->CopyFrom(valueObject.getValue(), pSharedValue->GetAllocator());

	{
		std::lock_guard<std::mutex> lockGuard(m_SharedValueMutex);
		m_pSharedValue = std::move(pSharedValue);
		m_SharedValueEpoch = epoch;
	}

	attributesObject.addMembersFrom(valueObject.getValue());
}


std::string CUIFrontendDefinitionAttribute::getName()
{
	return m_sName;
}

eUIFrontendDefinitionAttributeType CUIFrontendDefinitionAttribute::getAttributeType()
{
	return m_AttributeType;
}

std::string CUIFrontendDefinitionAttribute::getSessionReference()
{
	return "";
}

CUIFrontendDefinitionExpressionAttribute::CUIFrontendDefinitionExpressionAttribute(const std::string& sName, eUIFrontendDefinitionAttributeType attributeType, const CUIExpression& valueExpression)
	: CUIFrontendDefinitionAttribute(sName, attributeType), m_ValueExpression(valueExpression)
{
}

CUIFrontendDefinitionExpressionAttribute::~CUIFrontendDefinitionExpressionAttribute()
{

}

void CUIFrontendDefinitionExpressionAttribute::writeToFrontendJSON(CJSONWriter& writer, CJSONWriterObject& attributesObject, CStateMachineData* pStateMachineData, CUIFrontendState* pFrontendState)
{
	CUIExpressionSessionContext* pSessionContext = pFrontendState;

	switch (getAttributeType()) {
		case eUIFrontendDefinitionAttributeType::atBoolean: {
			bool bValue = m_ValueExpression.evaluateBoolValue (pStateMachineData, pSessionContext);
			attributesObject.addBool(getName(), bValue);
			break;
		}

		case eUIFrontendDefinitionAttributeType::atString: {
			std::string sValue = m_ValueExpression.evaluateStringValue(pStateMachineData, pSessionContext);
			attributesObject.addString(getName(), sValue);
			break;
		}

		case eUIFrontendDefinitionAttributeType::atNumber: {
			double dValue = m_ValueExpression.evaluateNumberValue(pStateMachineData, pSessionContext);
			attributesObject.addDouble(getName(), dValue);
			break;
		}

		case eUIFrontendDefinitionAttributeType::atInteger: {
			int64_t nValue = m_ValueExpression.evaluateIntegerValue(pStateMachineData, pSessionContext);
			attributesObject.addInteger(getName(), nValue);
			break;
		}

		case eUIFrontendDefinitionAttributeType::atUUID: {
			std::string sValue = m_ValueExpression.evaluateUUIDValue(pStateMachineData, pSessionContext);
			attributesObject.addString(getName(), sValue);
			break;
		}

	}
}

std::string CUIFrontendDefinitionExpressionAttribute::getSessionReference()
{
	return m_ValueExpression.getSessionReference();
}

bool CUIFrontendDefinitionExpressionAttribute::isSessionScoped()
{
	return !m_ValueExpression.getSessionReference().empty();
}


CUIFrontendDefinitionProviderAttribute::CUIFrontendDefinitionProviderAttribute(const std::string& sName, eUIFrontendDefinitionAttributeType attributeType, eUIFrontendDefinitionAttributeScope scope, UIFrontendDefinitionProvider provider)
	: CUIFrontendDefinitionAttribute(sName, attributeType), m_Scope(scope), m_Provider(provider)
{
	if (!provider)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
}

CUIFrontendDefinitionProviderAttribute::~CUIFrontendDefinitionProviderAttribute()
{

}

void CUIFrontendDefinitionProviderAttribute::writeToFrontendJSON(CJSONWriter& writer, CJSONWriterObject& attributesObject, CStateMachineData* pStateMachineData, CUIFrontendState* pFrontendState)
{
	m_Provider(writer, attributesObject, getName(), pStateMachineData, pFrontendState);
}

bool CUIFrontendDefinitionProviderAttribute::isSessionScoped()
{
	return (m_Scope == eUIFrontendDefinitionAttributeScope::asSession);
}


CUIFrontendDefinitionModuleStore::CUIFrontendDefinitionModuleStore(CUIFrontendDefinition* pFrontendDefinition, const std::string& sModuleUUID, const std::string& sModulePath, const std::string& sModuleType)
	: m_pFrontendDefinition(pFrontendDefinition), m_sUUID(AMCCommon::CUtils::normalizeUUIDString(sModuleUUID)), m_sPath(sModulePath), m_sModuleType(sModuleType), m_bAlwaysWriteSubmodules(false)
{
	LibMCAssertNotNull(pFrontendDefinition);

	if (!AMCCommon::CUtils::stringIsValidAlphanumericPathString (sModulePath))
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDFRONTENDMODULEPATH, sModulePath);

}

CUIFrontendDefinitionModuleStore::~CUIFrontendDefinitionModuleStore()
{

}

void CUIFrontendDefinitionModuleStore::checkNewAttributeName(const std::string& sName)
{
	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sName))
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDFRONTENDATTRIBUTENAME, sName);

	if (m_Attributes.find(sName) != m_Attributes.end())
		throw ELibMCCustomException(LIBMC_ERROR_DUPLICATEFRONTENDATTRIBUTENAME, m_sPath + "." + sName);
}

PUIFrontendDefinitionAttribute CUIFrontendDefinitionModuleStore::registerValue (const std::string& sName, eUIFrontendDefinitionAttributeType attributeType, const CUIExpression& valueExpression)
{
	checkNewAttributeName(sName);

	auto pAttribute = std::make_shared<CUIFrontendDefinitionExpressionAttribute>(sName, attributeType, valueExpression);
	m_Attributes.insert(std::make_pair(sName, pAttribute));

	return pAttribute;
}

PUIFrontendDefinitionAttribute CUIFrontendDefinitionModuleStore::registerProvider(const std::string& sName, eUIFrontendDefinitionAttributeType attributeType, eUIFrontendDefinitionAttributeScope scope, UIFrontendDefinitionProvider provider)
{
	checkNewAttributeName(sName);

	auto pAttribute = std::make_shared<CUIFrontendDefinitionProviderAttribute>(sName, attributeType, scope, provider);
	m_Attributes.insert(std::make_pair(sName, pAttribute));

	return pAttribute;
}

void CUIFrontendDefinitionModuleStore::registerStructureProperty(const std::string& sName, UIFrontendDefinitionProvider provider)
{
	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sName))
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDFRONTENDATTRIBUTENAME, sName);
	if (!provider)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	for (auto& property : m_StructureProperties) {
		if (property.first == sName)
			throw ELibMCCustomException(LIBMC_ERROR_DUPLICATEFRONTENDATTRIBUTENAME, m_sPath + "." + sName);
	}

	m_StructureProperties.push_back(std::make_pair(sName, provider));
}


std::vector<PUIFrontendDefinitionAttribute> CUIFrontendDefinitionModuleStore::getAttributes()
{
	std::vector<PUIFrontendDefinitionAttribute> attributes;
	for (auto attributePair : m_Attributes) {
		attributes.push_back(attributePair.second);
	}
	return attributes;
}

const std::vector<std::pair<std::string, UIFrontendDefinitionProvider>>& CUIFrontendDefinitionModuleStore::getStructureProperties()
{
	return m_StructureProperties;
}


PUIFrontendDefinitionModuleStore CUIFrontendDefinitionModuleStore::addChildStore(const std::string& sChildUUID, const std::string& sChildPath, const std::string& sChildModuleType)
{
	auto pChildStore = m_pFrontendDefinition->registerModuleStore(sChildUUID, sChildPath, sChildModuleType);
	m_ChildStores.push_back(pChildStore);
	return pChildStore;
}

void CUIFrontendDefinitionModuleStore::addChildStore(PUIFrontendDefinitionModuleStore pChildStore)
{
	LibMCAssertNotNull(pChildStore.get());
	if (pChildStore.get() == this)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	m_ChildStores.push_back(pChildStore);
}

std::vector<PUIFrontendDefinitionModuleStore> CUIFrontendDefinitionModuleStore::getChildStores()
{
	return m_ChildStores;
}

bool CUIFrontendDefinitionModuleStore::hasChildren()
{
	return !m_ChildStores.empty();
}

void CUIFrontendDefinitionModuleStore::setAlwaysWriteSubmodules(bool bAlwaysWriteSubmodules)
{
	m_bAlwaysWriteSubmodules = bAlwaysWriteSubmodules;
}

bool CUIFrontendDefinitionModuleStore::getAlwaysWriteSubmodules()
{
	return m_bAlwaysWriteSubmodules;
}

std::string CUIFrontendDefinitionModuleStore::getModuleType()
{
	return m_sModuleType;
}

void CUIFrontendDefinitionModuleStore::setModuleType(const std::string& sModuleType)
{
	m_sModuleType = sModuleType;
}

std::string CUIFrontendDefinitionModuleStore::getUUID()
{
	return m_sUUID;
}

std::string CUIFrontendDefinitionModuleStore::getPath()
{
	return m_sPath;
}

void CUIFrontendDefinitionModuleStore::collectSessionReferences(std::vector<std::string>& references)
{
	for (auto& attributePair : m_Attributes) {
		std::string sReference = attributePair.second->getSessionReference();
		if (!sReference.empty())
			references.push_back(sReference);
	}
}


CUIFrontendDefinition::CUIFrontendDefinition(AMCCommon::PChrono pGlobalChrono, PFrontendChangeCounter pFrontendChangeCounter)
	: m_pGlobalChrono (pGlobalChrono), m_pFrontendChangeCounter (pFrontendChangeCounter), m_nSessionVariableBroadcastCounter (0)
{
	if (pGlobalChrono.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
	if (pFrontendChangeCounter.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	m_pSessionVariableDeclarations = std::make_shared<CParameterGroup>(AMC_UI_SESSIONVARIABLES_GROUPNAME, "Session variables", pGlobalChrono);
	m_pSessionVariableBroadcasts = std::make_shared<CParameterGroup>(AMC_UI_SESSIONVARIABLES_GROUPNAME, "Session variable broadcasts", pGlobalChrono);
}

CUIFrontendDefinition::~CUIFrontendDefinition()
{

}

PUIFrontendDefinitionModuleStore CUIFrontendDefinition::registerModuleStore(const std::string& sModuleUUID, const std::string& sPath, const std::string& sModuleType)
{
	auto pModuleStore = std::make_shared<CUIFrontendDefinitionModuleStore>(this, sModuleUUID, sPath, sModuleType);

	auto sNormalizedUUID = pModuleStore->getUUID();
	if (m_ModuleStoreUUIDMap.find(sNormalizedUUID) != m_ModuleStoreUUIDMap.end())
		throw ELibMCCustomException(LIBMC_ERROR_DUPLICATEMODULE, sPath + " (" + sNormalizedUUID + ")");

	m_ModuleStores.push_back(pModuleStore);
	m_ModuleStoreUUIDMap.insert(std::make_pair(sNormalizedUUID, pModuleStore));
	return pModuleStore;

}

PUIFrontendDefinitionModuleStore CUIFrontendDefinition::findModuleStore(const std::string& sModuleUUID, bool bMustExist)
{
	auto sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sModuleUUID);
	auto iIter = m_ModuleStoreUUIDMap.find(sNormalizedUUID);
	if (iIter != m_ModuleStoreUUIDMap.end())
		return iIter->second;

	if (bMustExist)
		throw ELibMCCustomException(LIBMC_ERROR_MODULENOTFOUND, sNormalizedUUID);

	return nullptr;
}


AMCCommon::PChrono CUIFrontendDefinition::getGlobalChrono()
{
	return m_pGlobalChrono;
}

PFrontendChangeCounter CUIFrontendDefinition::getFrontendChangeCounter()
{
	return m_pFrontendChangeCounter;
}

void CUIFrontendDefinition::addSessionVariable(const std::string& sName, const std::string& sType, const std::string& sDescription, const std::string& sDefaultValue)
{
	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sName))
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDSESSIONVARIABLENAME, sName);

	if (m_pSessionVariableDeclarations->hasParameter(sName))
		throw ELibMCCustomException(LIBMC_ERROR_DUPLICATESESSIONVARIABLE, sName);

	m_pSessionVariableDeclarations->addNewTypedParameter(sName, sType, sDescription, sDefaultValue, "");

	std::lock_guard<std::mutex> lockGuard(m_BroadcastMutex);
	m_pSessionVariableBroadcasts->addNewTypedParameter(sName, sType, sDescription, sDefaultValue, "");
}

bool CUIFrontendDefinition::hasSessionVariable(const std::string& sName)
{
	return m_pSessionVariableDeclarations->hasParameter(sName);
}

PParameterGroup CUIFrontendDefinition::getSessionVariableDeclarations()
{
	return m_pSessionVariableDeclarations;
}

void CUIFrontendDefinition::broadcastSessionVariable(const std::string& sName, const std::string& sValue)
{
	std::lock_guard<std::mutex> lockGuard(m_BroadcastMutex);
	if (!m_pSessionVariableBroadcasts->hasParameter(sName))
		throw ELibMCCustomException(LIBMC_ERROR_SESSIONVARIABLENOTFOUND, sName);

	m_pSessionVariableBroadcasts->setParameterValueByName(sName, sValue);
	m_nSessionVariableBroadcastCounter++;
	m_SessionVariableBroadcastCounters[sName] = m_nSessionVariableBroadcastCounter;
	m_pFrontendChangeCounter->bump();
}

void CUIFrontendDefinition::broadcastSessionVariableAsDouble(const std::string& sName, double dValue)
{
	std::lock_guard<std::mutex> lockGuard(m_BroadcastMutex);
	if (!m_pSessionVariableBroadcasts->hasParameter(sName))
		throw ELibMCCustomException(LIBMC_ERROR_SESSIONVARIABLENOTFOUND, sName);

	m_pSessionVariableBroadcasts->setDoubleParameterValueByName(sName, dValue);
	m_nSessionVariableBroadcastCounter++;
	m_SessionVariableBroadcastCounters[sName] = m_nSessionVariableBroadcastCounter;
	m_pFrontendChangeCounter->bump();
}

void CUIFrontendDefinition::broadcastSessionVariableAsInteger(const std::string& sName, int64_t nValue)
{
	std::lock_guard<std::mutex> lockGuard(m_BroadcastMutex);
	if (!m_pSessionVariableBroadcasts->hasParameter(sName))
		throw ELibMCCustomException(LIBMC_ERROR_SESSIONVARIABLENOTFOUND, sName);

	m_pSessionVariableBroadcasts->setIntParameterValueByName(sName, nValue);
	m_nSessionVariableBroadcastCounter++;
	m_SessionVariableBroadcastCounters[sName] = m_nSessionVariableBroadcastCounter;
	m_pFrontendChangeCounter->bump();
}

void CUIFrontendDefinition::broadcastSessionVariableAsBool(const std::string& sName, bool bValue)
{
	std::lock_guard<std::mutex> lockGuard(m_BroadcastMutex);
	if (!m_pSessionVariableBroadcasts->hasParameter(sName))
		throw ELibMCCustomException(LIBMC_ERROR_SESSIONVARIABLENOTFOUND, sName);

	m_pSessionVariableBroadcasts->setBoolParameterValueByName(sName, bValue);
	m_nSessionVariableBroadcastCounter++;
	m_SessionVariableBroadcastCounters[sName] = m_nSessionVariableBroadcastCounter;
	m_pFrontendChangeCounter->bump();
}

uint64_t CUIFrontendDefinition::getSessionVariableBroadcastCounter()
{
	std::lock_guard<std::mutex> lockGuard(m_BroadcastMutex);
	return m_nSessionVariableBroadcastCounter;
}

void CUIFrontendDefinition::getSessionVariableBroadcastsSince(uint64_t nSinceCounter, std::vector<std::pair<std::string, std::string>>& values)
{
	std::lock_guard<std::mutex> lockGuard(m_BroadcastMutex);
	for (auto& counterPair : m_SessionVariableBroadcastCounters) {
		if (counterPair.second > nSinceCounter)
			values.push_back(std::make_pair(counterPair.first, m_pSessionVariableBroadcasts->getParameterValueByName(counterPair.first)));
	}
}

void CUIFrontendDefinition::collectSessionReferences(std::vector<std::string>& references)
{
	for (auto& pModuleStore : m_ModuleStores)
		pModuleStore->collectSessionReferences(references);
}

