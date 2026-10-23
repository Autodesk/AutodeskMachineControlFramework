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

#include "amc_ui_frontendstate.hpp"
#include "amc_parametergroup.hpp"
#include "amc_userinformation.hpp"
#include "amc_accesscontrol.hpp"
#include "libmc_exceptiontypes.hpp"
#include "common_utils.hpp"

using namespace AMC;

#define AMC_UI_SESSIONREFERENCE_VARIABLEPREFIX "session."
#define AMC_UI_SESSIONREFERENCE_PERMISSIONPREFIX "$user.permission."

CUIFrontendState::CUIFrontendState(PUIFrontendDefinition pFrontendDefinition)
	: m_pFrontendDefinition(pFrontendDefinition), m_nAppliedBroadcastCounter (0)
{
	if (pFrontendDefinition.get () == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	m_pLegacyParameterHandler = std::make_shared<CParameterHandler>("", pFrontendDefinition->getGlobalChrono (), pFrontendDefinition->getFrontendChangeCounter());

	// Values broadcast before this session existed are ignored; the session starts with the declared defaults.
	m_nAppliedBroadcastCounter = pFrontendDefinition->getSessionVariableBroadcastCounter();

	auto pDeclarations = pFrontendDefinition->getSessionVariableDeclarations();
	m_pSessionVariables = std::make_shared<CParameterGroup>(pDeclarations->getName (), pDeclarations->getDescription (), pFrontendDefinition->getGlobalChrono());
	m_pSessionVariables->addDuplicatesFromGroup(pDeclarations.get());
	m_pSessionVariables->setFrontendChangeCounter(pFrontendDefinition->getFrontendChangeCounter());

}

CUIFrontendState::~CUIFrontendState()
{

}

PParameterHandler CUIFrontendState::getLegacyParameterHandler()
{
	return m_pLegacyParameterHandler;
}

void CUIFrontendState::setSessionIdentity(const std::string& sSessionUUID, PUserInformation pUserInformation, PAccessControl pAccessControl)
{
	std::lock_guard<std::mutex> lockGuard(m_SessionMutex);
	m_sSessionUUID = sSessionUUID.empty() ? std::string() : AMCCommon::CUtils::normalizeUUIDString(sSessionUUID);
	m_pUserInformation = pUserInformation;
	m_pAccessControl = pAccessControl;
}

CUIFrontendRevisionLog& CUIFrontendState::getRevisionLog()
{
	return m_RevisionLog;
}

PJSONEventStreamInstance CUIFrontendState::getOrCreateEventStream(const std::function<PJSONEventStreamInstance()>& createStream)
{
	std::lock_guard<std::mutex> lockGuard(m_EventStreamMutex);

	if ((m_pEventStream.get() == nullptr) || (!m_pEventStream->isActive())) {
		auto pEventStream = createStream();
		if (pEventStream.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
		m_pEventStream = pEventStream;
	}

	return m_pEventStream;
}

void CUIFrontendState::endEventStream()
{
	PJSONEventStreamInstance pEventStream;
	{
		std::lock_guard<std::mutex> lockGuard(m_EventStreamMutex);
		pEventStream = m_pEventStream;
		m_pEventStream = nullptr;
	}

	if (pEventStream.get() != nullptr)
		pEventStream->endStream();
}

CParameterGroup* CUIFrontendState::getSessionVariables()
{
	std::lock_guard<std::mutex> lockGuard(m_SessionMutex);

	uint64_t nBroadcastCounter = m_pFrontendDefinition->getSessionVariableBroadcastCounter();
	if (nBroadcastCounter != m_nAppliedBroadcastCounter) {
		std::vector<std::pair<std::string, std::string>> broadcastValues;
		m_pFrontendDefinition->getSessionVariableBroadcastsSince(m_nAppliedBroadcastCounter, broadcastValues);
		for (auto& broadcastValue : broadcastValues)
			m_pSessionVariables->setParameterValueByName(broadcastValue.first, broadcastValue.second);

		m_nAppliedBroadcastCounter = nBroadcastCounter;
	}

	return m_pSessionVariables.get();
}

CParameterGroup* CUIFrontendState::findSessionVariableGroup(const std::string& sName)
{
	auto pSessionVariables = getSessionVariables();
	if (!pSessionVariables->hasParameter(sName))
		throw ELibMCCustomException(LIBMC_ERROR_SESSIONVARIABLENOTFOUND, sName);

	return pSessionVariables;
}

bool CUIFrontendState::hasSessionVariable(const std::string& sName)
{
	return m_pSessionVariables->hasParameter(sName);
}

std::string CUIFrontendState::getSessionVariable(const std::string& sName)
{
	return findSessionVariableGroup(sName)->getParameterValueByName(sName);
}

std::string CUIFrontendState::getSessionVariableAsUUID(const std::string& sName)
{
	return findSessionVariableGroup(sName)->getUUIDParameterValueByName(sName);
}

double CUIFrontendState::getSessionVariableAsDouble(const std::string& sName)
{
	return findSessionVariableGroup(sName)->getDoubleParameterValueByName(sName);
}

int64_t CUIFrontendState::getSessionVariableAsInteger(const std::string& sName)
{
	return findSessionVariableGroup(sName)->getIntParameterValueByName(sName);
}

bool CUIFrontendState::getSessionVariableAsBool(const std::string& sName)
{
	return findSessionVariableGroup(sName)->getBoolParameterValueByName(sName);
}

void CUIFrontendState::setSessionVariable(const std::string& sName, const std::string& sValue)
{
	findSessionVariableGroup(sName)->setParameterValueByName(sName, sValue);
}

void CUIFrontendState::setSessionVariableAsUUID(const std::string& sName, const std::string& sValue)
{
	std::string sNormalizedUUID = sValue.empty() ? AMCCommon::CUtils::createEmptyUUID() : AMCCommon::CUtils::normalizeUUIDString(sValue);
	findSessionVariableGroup(sName)->setParameterValueByName(sName, sNormalizedUUID);
}

void CUIFrontendState::setSessionVariableAsDouble(const std::string& sName, double dValue)
{
	findSessionVariableGroup(sName)->setDoubleParameterValueByName(sName, dValue);
}

void CUIFrontendState::setSessionVariableAsInteger(const std::string& sName, int64_t nValue)
{
	findSessionVariableGroup(sName)->setIntParameterValueByName(sName, nValue);
}

void CUIFrontendState::setSessionVariableAsBool(const std::string& sName, bool bValue)
{
	findSessionVariableGroup(sName)->setBoolParameterValueByName(sName, bValue);
}

std::string CUIFrontendState::evaluateBuiltinReference(const std::string& sReference)
{
	std::lock_guard<std::mutex> lockGuard(m_SessionMutex);

	if (sReference == "$session.uuid")
		return m_sSessionUUID;

	if (sReference.rfind(AMC_UI_SESSIONREFERENCE_PERMISSIONPREFIX, 0) == 0) {
		if ((m_pUserInformation.get() == nullptr) || (m_pAccessControl.get() == nullptr))
			return "0";

		std::string sPermission = sReference.substr(std::string(AMC_UI_SESSIONREFERENCE_PERMISSIONPREFIX).length());
		return m_pAccessControl->checkPermissionInRole(m_pUserInformation->getRoleIdentifier(), sPermission) ? "1" : "0";
	}

	if (m_pUserInformation.get() == nullptr)
		return "";

	if (sReference == "$user.login")
		return m_pUserInformation->getLogin();
	if (sReference == "$user.uuid")
		return m_pUserInformation->getUUID();
	if (sReference == "$user.role")
		return m_pUserInformation->getRoleIdentifier();
	if (sReference == "$user.description")
		return m_pUserInformation->getDescription();
	if (sReference == "$user.language")
		return m_pUserInformation->getLanguageIdentifier();

	throw ELibMCCustomException(LIBMC_ERROR_INVALIDSESSIONREFERENCE, sReference);
}

std::string CUIFrontendState::evaluateSessionReference(const std::string& sReference)
{
	if (sReference.rfind(AMC_UI_SESSIONREFERENCE_VARIABLEPREFIX, 0) == 0)
		return getSessionVariable(sReference.substr(std::string(AMC_UI_SESSIONREFERENCE_VARIABLEPREFIX).length()));

	return evaluateBuiltinReference(sReference);
}

bool CUIFrontendState::evaluateSessionReferenceAsBool(const std::string& sReference)
{
	if (sReference.rfind(AMC_UI_SESSIONREFERENCE_VARIABLEPREFIX, 0) == 0)
		return getSessionVariableAsBool(sReference.substr(std::string(AMC_UI_SESSIONREFERENCE_VARIABLEPREFIX).length()));

	std::string sValue = evaluateBuiltinReference(sReference);
	if (sReference.rfind(AMC_UI_SESSIONREFERENCE_PERMISSIONPREFIX, 0) == 0)
		return (sValue == "1");

	return !sValue.empty();
}

int64_t CUIFrontendState::evaluateSessionReferenceAsInteger(const std::string& sReference)
{
	if (sReference.rfind(AMC_UI_SESSIONREFERENCE_VARIABLEPREFIX, 0) == 0)
		return getSessionVariableAsInteger(sReference.substr(std::string(AMC_UI_SESSIONREFERENCE_VARIABLEPREFIX).length()));

	return evaluateSessionReferenceAsBool(sReference) ? 1 : 0;
}

double CUIFrontendState::evaluateSessionReferenceAsNumber(const std::string& sReference)
{
	if (sReference.rfind(AMC_UI_SESSIONREFERENCE_VARIABLEPREFIX, 0) == 0)
		return getSessionVariableAsDouble(sReference.substr(std::string(AMC_UI_SESSIONREFERENCE_VARIABLEPREFIX).length()));

	return evaluateSessionReferenceAsBool(sReference) ? 1.0 : 0.0;
}

void CUIFrontendState::validateSessionReference(const std::string& sReference, CUIFrontendDefinition* pFrontendDefinition, CAccessControl* pAccessControl)
{
	LibMCAssertNotNull(pFrontendDefinition);
	LibMCAssertNotNull(pAccessControl);

	if (sReference.rfind(AMC_UI_SESSIONREFERENCE_VARIABLEPREFIX, 0) == 0) {
		std::string sName = sReference.substr(std::string(AMC_UI_SESSIONREFERENCE_VARIABLEPREFIX).length());
		if (!pFrontendDefinition->hasSessionVariable(sName))
			throw ELibMCCustomException(LIBMC_ERROR_SESSIONVARIABLENOTFOUND, sName);
		return;
	}

	if (sReference.rfind(AMC_UI_SESSIONREFERENCE_PERMISSIONPREFIX, 0) == 0) {
		std::string sPermission = sReference.substr(std::string(AMC_UI_SESSIONREFERENCE_PERMISSIONPREFIX).length());
		if (!pAccessControl->hasPermission(sPermission))
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDSESSIONREFERENCE, sReference);
		return;
	}

	if ((sReference == "$session.uuid") || (sReference == "$user.login") || (sReference == "$user.uuid") ||
		(sReference == "$user.role") || (sReference == "$user.description") || (sReference == "$user.language"))
		return;

	throw ELibMCCustomException(LIBMC_ERROR_INVALIDSESSIONREFERENCE, sReference);
}

void CUIFrontendState::writeModuleAttributesToJSON(CJSONWriter& writer, CJSONWriterObject & attributesObject, CUIFrontendDefinitionModuleStore * pModuleStore, CStateMachineData* pStateMachineData, const sUIFrontendBuildEpoch& epoch)
{
	auto attributes = pModuleStore->getAttributes();
	for (auto& pAttribute : attributes) {
		pAttribute->writeToFrontendJSONForEpoch(writer, attributesObject, pStateMachineData, this, epoch);
	}

}


void CUIFrontendState::writeModuleStoreToJSON(CJSONWriter& writer, CJSONWriterObject& moduleObject, CUIFrontendDefinitionModuleStore* pModuleStore, CStateMachineData* pStateMachineData, const sUIFrontendBuildEpoch& epoch)
{
	LibMCAssertNotNull(pModuleStore);

	moduleObject.addString("moduletype", pModuleStore->getModuleType());
	moduleObject.addString("uuid", pModuleStore->getUUID());

	for (auto& structureProperty : pModuleStore->getStructureProperties())
		structureProperty.second(writer, moduleObject, structureProperty.first, pStateMachineData, this);

	CJSONWriterObject attributesObject(writer);
	writeModuleAttributesToJSON(writer, attributesObject, pModuleStore, pStateMachineData, epoch);
	moduleObject.addObject("attributes", attributesObject);

	if (pModuleStore->hasChildren() || pModuleStore->getAlwaysWriteSubmodules()) {
		CJSONWriterArray submodulesArray(writer);

		auto childStores = pModuleStore->getChildStores();
		for (auto& pChildStore : childStores) {
			CJSONWriterObject childObject(writer);
			writeModuleStoreToJSON(writer, childObject, pChildStore.get(), pStateMachineData, epoch);
			submodulesArray.addObject(childObject);
		}

		moduleObject.addArray("submodules", submodulesArray);
	}
}


