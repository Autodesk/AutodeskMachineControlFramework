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


#include "libmcenv_stateenvironment.hpp"
#include "libmcenv_interfaceexception.hpp"

#include "libmcenv_signalhandler.hpp"
#include "libmcenv_signaltrigger.hpp"
#include "libmcenv_scenehandler.hpp"
#include "libmcenv_toolpathaccessor.hpp"
#include "libmcenv_build.hpp"
#include "libmcenv_buildexecution.hpp"
#include "libmcenv_dataseries.hpp"
#include "libmcenv_datetime.hpp"
#include "libmcenv_imagedata.hpp"
#include "libmcenv_testenvironment.hpp"
#include "libmcenv_telemetrychannel.hpp"
#include "libmcenv_xmldocument.hpp"
#include "libmcenv_discretefielddata2d.hpp"
#include "libmcenv_journalhandler_current.hpp"
#include "libmcenv_usermanagementhandler.hpp"
#include "libmcenv_meshobject.hpp"
#include "libmcenv_persistentmeshobject.hpp"
#include "libmcenv_alert.hpp"
#include "libmcenv_alertiterator.hpp"
#include "libmcenv_cryptocontext.hpp"
#include "libmcenv_tempstreamwriter.hpp"
#include "libmcenv_zipstreamwriter.hpp"
#include "libmcenv_streamreader.hpp"
#include "libmcenv_datatable.hpp"
#include "libmcenv_machineconfigurationhandler.hpp"
#include "libmcenv_modeldatacomponentinstance.hpp"
#include "libmcenv_imageloader.hpp"
#include "libmcenv_jsonobject.hpp"
#include "libmcenv_videostream.hpp"

#include "amc_telemetry.hpp"
#include "amc_logger.hpp"
#include "amc_driverhandler.hpp"
#include "amc_parameterhandler.hpp"
#include "amc_ui_handler.hpp"
#include "amc_statemachinedata.hpp"
#include "amc_xmldocument.hpp"
#include "amc_accesscontrol.hpp"
#include "amc_meshhandler.hpp"
#include "amc_alerthandler.hpp"
#include "amc_dataserieshandler.hpp"
#include "amc_streamregistry.hpp"

#include "common_chrono.hpp"
#include <thread> 
#include <chrono>

// Include custom headers here.

using namespace LibMCEnv::Impl;



/*************************************************************************************************************************
 Class definition of CStateEnvironment 
**************************************************************************************************************************/

CStateEnvironment::CStateEnvironment(AMC::PSystemState pSystemState, AMC::PParameterHandler pParameterHandler, std::string sInstanceName, uint64_t nAbsoluteEndTimeOfPreviousStateInMicroseconds, const std::string& sPreviousStateName)
	: m_pSystemState (pSystemState), m_pParameterHandler (pParameterHandler), m_sInstanceName(sInstanceName),
	  m_nAbsoluteEndTimeOfPreviousStateInMicroseconds (nAbsoluteEndTimeOfPreviousStateInMicroseconds), m_sPreviousStateName (sPreviousStateName)
{
	if (pSystemState.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
	if (pParameterHandler.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

	m_nAbsoluteStartTimeOfStateInMicroseconds = m_pSystemState->globalChrono()->getUTCTimeStampInMicrosecondsSince1970 ();
}


std::string CStateEnvironment::GetMachineState(const std::string& sMachineInstance)
{
	return m_pSystemState->stateMachineData()->getInstanceStateName(sMachineInstance);
}

std::string CStateEnvironment::GetPreviousState()
{
	return m_sPreviousStateName;
}


ISignalTrigger* CStateEnvironment::PrepareSignal(const std::string& sMachineInstance, const std::string& sSignalName)
{
	auto pSignalInstance = m_pSystemState->stateSignalHandler()->getInstance(sMachineInstance);
	if (!pSignalInstance->hasSignalDefinition(sSignalName))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNOTFINDSIGNALDEFINITON);

	return new CSignalTrigger (m_pSystemState->getStateSignalHandlerInstance (), sMachineInstance, sSignalName, m_pSystemState->getGlobalChronoInstance());

}

bool CStateEnvironment::WaitForSignal(const std::string& sSignalName, const LibMCEnv_uint32 nTimeOut, ISignalHandler*& pHandlerInstance)
{
	auto pChronoInstance = m_pSystemState->getGlobalChronoInstance();
	auto pSignalInstance = m_pSystemState->stateSignalHandler()->getInstance(m_sInstanceName);

	uint64_t nStartTime = pChronoInstance->getElapsedMicroseconds();
	uint64_t nEndTime = nStartTime + (static_cast<uint64_t>(nTimeOut) * 1000);

	bool bIsTimeOut = false;
	while (!bIsTimeOut) {

		uint64_t nCurrentTime = pChronoInstance->getElapsedMicroseconds();

		std::string sUnhandledSignalUUID;
		std::string sParameterDataJSON;
		bool bHasSignal = pSignalInstance->claimSignalMessage(sSignalName, true, nCurrentTime, nCurrentTime, sUnhandledSignalUUID, sParameterDataJSON, false);

		if (bHasSignal) {
			pHandlerInstance = new CSignalHandler(m_pSystemState->getStateSignalHandlerInstance(), m_sInstanceName, sSignalName, sUnhandledSignalUUID, sParameterDataJSON, m_pSystemState->getGlobalChronoInstance());

			return true;
		}

		bIsTimeOut = nCurrentTime >= nEndTime;

		if (!bIsTimeOut) {
			if (CheckForTermination())
				throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_TERMINATED);

			AMCCommon::CChrono chrono;
			chrono.sleepMilliseconds(AMC_SIGNAL_DEFAULT_WAITFOR_SLEEP_MS);
		}
	}

	return false; 
}

ISignalHandler* CStateEnvironment::GetUnhandledSignal(const std::string& sSignalTypeName)
{
	auto pChronoInstance = m_pSystemState->getGlobalChronoInstance();
	auto pSignalInstance = m_pSystemState->stateSignalHandler()->getInstance(m_sInstanceName);

	std::string sUnhandledSignalUUID;
	std::string sParameterDataJSON;
	uint64_t nCurrentTime = pChronoInstance->getElapsedMicroseconds();
	bool bHasSignal = pSignalInstance->claimSignalMessage(sSignalTypeName, true, nCurrentTime, nCurrentTime, sUnhandledSignalUUID, sParameterDataJSON, false);
	if (bHasSignal) {
		return new CSignalHandler(m_pSystemState->getStateSignalHandlerInstance(), m_sInstanceName, sSignalTypeName, sUnhandledSignalUUID, sParameterDataJSON, pChronoInstance);
	}

	return nullptr;
}

ISignalHandler* CStateEnvironment::ClaimSignalFromQueue(const std::string& sSignalTypeName) 
{
	auto pChronoInstance = m_pSystemState->getGlobalChronoInstance();
	auto pSignalInstance = m_pSystemState->stateSignalHandler()->getInstance(m_sInstanceName);

	std::string sUnhandledSignalUUID;
	std::string sParameterDataJSON;
	uint64_t nCurrentTime = pChronoInstance->getElapsedMicroseconds();
	bool bHasSignal = pSignalInstance->claimSignalMessage(sSignalTypeName, true, nCurrentTime, nCurrentTime, sUnhandledSignalUUID, sParameterDataJSON, true);
	if (bHasSignal) {
		return new CSignalHandler(m_pSystemState->getStateSignalHandlerInstance(), m_sInstanceName, sSignalTypeName, sUnhandledSignalUUID, sParameterDataJSON, pChronoInstance);
	}

	return nullptr;
}

bool CStateEnvironment::SignalQueueIsEmpty(const std::string& sSignalTypeName)
{
	auto pSignalInstance = m_pSystemState->stateSignalHandler()->getInstance(m_sInstanceName);
	return pSignalInstance->queueIsEmpty(sSignalTypeName);
}

bool CStateEnvironment::QueueHasSignal(const std::string& sSignalTypeName)
{
	return !SignalQueueIsEmpty(sSignalTypeName);
}


void CStateEnvironment::ClearAllUnhandledSignals()
{
	auto pChrono = m_pSystemState->globalChrono();
	auto pSignalInstance = m_pSystemState->stateSignalHandler()->getInstance(m_sInstanceName);
	pSignalInstance->clearUnhandledSignals(pChrono->getElapsedMicroseconds ());
}

void CStateEnvironment::ClearUnhandledSignalsOfType(const std::string& sSignalTypeName)
{
	auto pChrono = m_pSystemState->globalChrono();	
	auto pSignalInstance = m_pSystemState->stateSignalHandler()->getInstance(m_sInstanceName);
	pSignalInstance->clearUnhandledSignalsOfType(sSignalTypeName, pChrono->getElapsedMicroseconds ());
}

ISignalHandler* CStateEnvironment::GetUnhandledSignalByUUID(const std::string& sUUID, const bool bMustExist)
{
	std::string sNormalizedSignalUUID = AMCCommon::CUtils::normalizeUUIDString (sUUID);

	AMC::eAMCSignalPhase signalPhase = m_pSystemState->stateSignalHandler()->getSignalPhase(sNormalizedSignalUUID);

	if (signalPhase == AMC::eAMCSignalPhase::InQueue) {
		return new CSignalHandler(m_pSystemState->getStateSignalHandlerInstance(), sNormalizedSignalUUID, m_pSystemState->getGlobalChronoInstance());
	}
	else {
		if (bMustExist)
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_SIGNALUUIDNOTACTIVE, "signal uuid not in queue: " + sNormalizedSignalUUID);

		return nullptr;
	}


}

bool CStateEnvironment::HasBuildJob(const std::string& sBuildUUID)
{
	std::string sNormalizedBuildUUID = AMCCommon::CUtils::normalizeUUIDString(sBuildUUID);

	auto pDataModel = m_pSystemState->getDataModelInstance();
	auto pBuildJobHandler = pDataModel->CreateBuildJobHandler();
	try {
		auto pBuildJob = pBuildJobHandler->RetrieveJob(sNormalizedBuildUUID);
		return (pBuildJob.get () != nullptr);
	}
	catch (std::exception) {
		return false;
	}
}

IBuild* CStateEnvironment::GetBuildJob(const std::string& sBuildUUID)
{
	std::string sNormalizedBuildUUID = AMCCommon::CUtils::normalizeUUIDString(sBuildUUID);

	auto pDataModel = m_pSystemState->getDataModelInstance();
	auto pBuildJobHandler = pDataModel->CreateBuildJobHandler();
	auto pBuildJob = pBuildJobHandler->RetrieveJob(sNormalizedBuildUUID);
	return new CBuild(pDataModel, pBuildJob->GetUUID (), m_pSystemState->getToolpathHandlerInstance (), m_pSystemState->getMeshHandlerInstance(), m_pSystemState->getGlobalChronoInstance (), m_pSystemState->getStateJournalInstance ());
}

bool CStateEnvironment::HasBuildExecution(const std::string& sExecutionUUID)
{
	std::string sNormalizedExecutionUUID = AMCCommon::CUtils::normalizeUUIDString(sExecutionUUID);

	try {
		auto pDataModel = m_pSystemState->getDataModelInstance();
		auto pBuildJobHandler = pDataModel->CreateBuildJobHandler();
		auto pExecution = pBuildJobHandler->RetrieveJobExecution(sNormalizedExecutionUUID);

		return (pExecution.get() != nullptr);
	}
	catch (std::exception) {
		return false;
	}

}

IBuildExecution* CStateEnvironment::GetBuildExecution(const std::string& sExecutionUUID)
{
	std::string sNormalizedExecutionUUID = AMCCommon::CUtils::normalizeUUIDString(sExecutionUUID);
	auto pDataModel = m_pSystemState->getDataModelInstance();
	auto pBuildJobHandler = pDataModel->CreateBuildJobHandler();
	auto pBuildExecution = pBuildJobHandler->RetrieveJobExecution(sNormalizedExecutionUUID);
	return new CBuildExecution(pBuildExecution, pDataModel, m_pSystemState->getToolpathHandlerInstance(), m_pSystemState->getMeshHandlerInstance (), m_pSystemState->getGlobalChronoInstance(), m_pSystemState->getStateJournalInstance ());

}


void CStateEnvironment::UnloadAllToolpathes()
{
	m_pSystemState->toolpathHandler()->unloadAllEntities();
}



void CStateEnvironment::GetDriverLibrary(const std::string& sDriverName, std::string& sDriverType, LibMCEnv_pvoid& pDriverLookup)
{
	m_pSystemState->driverHandler()->GetDriverInformation (sDriverName, sDriverType, pDriverLookup);
}

void CStateEnvironment::CreateDriverAccess(const std::string& sDriverName, LibMCEnv_pvoid& pDriverHandle) 
{	
	pDriverHandle = m_pSystemState->driverHandler()->acquireDriver(sDriverName, m_sInstanceName);
}



void CStateEnvironment::SetNextState(const std::string & sStateName)
{
	m_sNextState = sStateName;
}

std::string CStateEnvironment::getNextState()
{
	return m_sNextState;
}

void CStateEnvironment::LogMessage(const std::string& sLogString)
{
	m_pSystemState->logger()->logMessage(sLogString, m_sInstanceName, AMC::eLogLevel::Message);
}

void CStateEnvironment::LogWarning(const std::string& sLogString)
{
	m_pSystemState->logger()->logMessage(sLogString, m_sInstanceName, AMC::eLogLevel::Warning);
}

void CStateEnvironment::LogInfo(const std::string& sLogString)
{
	m_pSystemState->logger()->logMessage(sLogString, m_sInstanceName, AMC::eLogLevel::Info);
}

void CStateEnvironment::Sleep(const LibMCEnv_uint32 nDelay)
{
	AMCCommon::CChrono chrono;
	chrono.sleepMilliseconds(nDelay);
}

bool CStateEnvironment::CheckForTermination()
{
	// TODO
	return false;
}



void CStateEnvironment::StoreSignal(const std::string& sName, ISignalHandler* pHandler)
{
	if (pHandler == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

	AMC::CParameterGroup* pGroup = m_pSystemState->stateMachineData()->getDataStore(m_sInstanceName);

	if (!pGroup->hasParameter(sName)) {
		pGroup->addNewStringParameter(sName, "", pHandler->GetSignalUUID());
	}
	else {
		pGroup->setParameterValueByName(sName, pHandler->GetSignalUUID());
	}

}

ISignalHandler* CStateEnvironment::RetrieveSignal(const std::string& sName)
{
	AMC::CParameterGroup* pGroup = m_pSystemState->stateMachineData()->getDataStore(m_sInstanceName);

	std::string sSignalID = pGroup->getParameterValueByName(sName);
	return new CSignalHandler(m_pSystemState->getStateSignalHandlerInstance(), sSignalID, m_pSystemState->getGlobalChronoInstance ());
}


void CStateEnvironment::ClearStoredValue(const std::string& sName)
{
	AMC::CParameterGroup* pGroup = m_pSystemState->stateMachineData()->getDataStore(m_sInstanceName);
	pGroup->removeValue(sName);

}

void CStateEnvironment::SetStringParameter(const std::string& sParameterGroup, const std::string& sParameterName, const std::string& sValue)
{
	if (!m_pParameterHandler->hasGroup(sParameterGroup))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_PARAMETERGROUPNOTFOUND);

	auto pGroup = m_pParameterHandler->findGroup(sParameterGroup, true);
	if (!pGroup->hasParameter(sParameterName))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_PARAMETERNOTFOUND, "parameter not found: " + sParameterGroup + "/" + sParameterName);

	pGroup->setParameterValueByName(sParameterName, sValue);
}


void CStateEnvironment::SetUUIDParameter(const std::string& sParameterGroup, const std::string& sParameterName, const std::string& sValue)
{
	if (!m_pParameterHandler->hasGroup(sParameterGroup))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_PARAMETERGROUPNOTFOUND);

	auto pGroup = m_pParameterHandler->findGroup(sParameterGroup, true);
	if (!pGroup->hasParameter(sParameterName))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_PARAMETERNOTFOUND, "parameter not found: " + sParameterGroup + "/" + sParameterName);

	pGroup->setParameterValueByName(sParameterName, AMCCommon::CUtils::normalizeUUIDString (sValue));
}


void CStateEnvironment::SetDoubleParameter(const std::string& sParameterGroup, const std::string& sParameterName, const LibMCEnv_double dValue)
{
	if (!m_pParameterHandler->hasGroup(sParameterGroup))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_PARAMETERGROUPNOTFOUND);

	auto pGroup = m_pParameterHandler->findGroup(sParameterGroup, true);
	if (!pGroup->hasParameter(sParameterName))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_PARAMETERNOTFOUND, "parameter not found: " + sParameterGroup + "/" + sParameterName);

	pGroup->setDoubleParameterValueByName(sParameterName, dValue);
}

void CStateEnvironment::SetIntegerParameter(const std::string& sParameterGroup, const std::string& sParameterName, const LibMCEnv_int64 nValue)
{
	if (!m_pParameterHandler->hasGroup(sParameterGroup))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_PARAMETERGROUPNOTFOUND);

	auto pGroup = m_pParameterHandler->findGroup(sParameterGroup, true);
	if (!pGroup->hasParameter(sParameterName))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_PARAMETERNOTFOUND, "parameter not found: " + sParameterGroup + "/" + sParameterName);

	pGroup->setIntParameterValueByName(sParameterName, nValue);
}

void CStateEnvironment::SetBoolParameter(const std::string& sParameterGroup, const std::string& sParameterName, const bool bValue)
{
	if (!m_pParameterHandler->hasGroup(sParameterGroup))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_PARAMETERGROUPNOTFOUND);

	auto pGroup = m_pParameterHandler->findGroup(sParameterGroup, true);
	if (!pGroup->hasParameter(sParameterName))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_PARAMETERNOTFOUND, "parameter not found: " + sParameterGroup + "/" + sParameterName);

	pGroup->setBoolParameterValueByName(sParameterName, bValue);
}

std::string CStateEnvironment::GetStringParameter(const std::string& sParameterGroup, const std::string& sParameterName)
{
	if (!m_pParameterHandler->hasGroup(sParameterGroup))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_PARAMETERGROUPNOTFOUND);

	auto pGroup = m_pParameterHandler->findGroup(sParameterGroup, true);
	if (!pGroup->hasParameter(sParameterName))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_PARAMETERNOTFOUND, "parameter not found: " + sParameterGroup + "/" + sParameterName);

	return pGroup->getParameterValueByName(sParameterName);
}


std::string CStateEnvironment::GetUUIDParameter(const std::string& sParameterGroup, const std::string& sParameterName)
{
	if (!m_pParameterHandler->hasGroup(sParameterGroup))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_PARAMETERGROUPNOTFOUND);

	auto pGroup = m_pParameterHandler->findGroup(sParameterGroup, true);
	if (!pGroup->hasParameter(sParameterName))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_PARAMETERNOTFOUND, "parameter not found: " + sParameterGroup + "/" + sParameterName);

	return AMCCommon::CUtils::normalizeUUIDString (pGroup->getParameterValueByName(sParameterName));
}

LibMCEnv_double CStateEnvironment::GetDoubleParameter(const std::string& sParameterGroup, const std::string& sParameterName)
{
	if (!m_pParameterHandler->hasGroup(sParameterGroup))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_PARAMETERGROUPNOTFOUND);

	auto pGroup = m_pParameterHandler->findGroup(sParameterGroup, true);
	if (!pGroup->hasParameter(sParameterName))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_PARAMETERNOTFOUND, "parameter not found: " + sParameterGroup + "/" + sParameterName);

	return pGroup->getDoubleParameterValueByName(sParameterName);
}

LibMCEnv_int64 CStateEnvironment::GetIntegerParameter(const std::string& sParameterGroup, const std::string& sParameterName)
{
	if (!m_pParameterHandler->hasGroup(sParameterGroup))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_PARAMETERGROUPNOTFOUND);

	auto pGroup = m_pParameterHandler->findGroup(sParameterGroup, true);
	if (!pGroup->hasParameter(sParameterName))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_PARAMETERNOTFOUND, "parameter not found: " + sParameterGroup + "/" + sParameterName);

	return pGroup->getIntParameterValueByName(sParameterName);
}

bool CStateEnvironment::GetBoolParameter(const std::string& sParameterGroup, const std::string& sParameterName)
{
	if (!m_pParameterHandler->hasGroup(sParameterGroup))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_PARAMETERGROUPNOTFOUND);

	auto pGroup = m_pParameterHandler->findGroup(sParameterGroup, true);
	if (!pGroup->hasParameter(sParameterName))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_PARAMETERNOTFOUND, "parameter not found: " + sParameterGroup + "/" + sParameterName);

	return pGroup->getBoolParameterValueByName(sParameterName);
}

bool CStateEnvironment::HasParameterGroup(const std::string& sParameterGroup)
{
	return m_pParameterHandler->hasGroup(sParameterGroup);
}

bool CStateEnvironment::HasParameter(const std::string& sParameterGroup, const std::string& sParameterName)
{
	if (!m_pParameterHandler->hasGroup(sParameterGroup))
		return false;

	auto pGroup = m_pParameterHandler->findGroup(sParameterGroup, true);
	return pGroup->hasParameter(sParameterName);
}

LibMCEnv_uint32 CStateEnvironment::GetParameterGroupParameterCount(const std::string& sParameterGroup)
{
	if (!m_pParameterHandler->hasGroup(sParameterGroup))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_PARAMETERGROUPNOTFOUND);

	auto pGroup = m_pParameterHandler->findGroup(sParameterGroup, true);
	return pGroup->getParameterCount();
}

std::string CStateEnvironment::GetParameterGroupParameterName(const std::string& sParameterGroup, const LibMCEnv_uint32 nIndex)
{
	if (!m_pParameterHandler->hasGroup(sParameterGroup))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_PARAMETERGROUPNOTFOUND);

	auto pGroup = m_pParameterHandler->findGroup(sParameterGroup, true);
	if (nIndex >= pGroup->getParameterCount())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM, "parameter index out of range: " + sParameterGroup + "/" + std::to_string(nIndex));

	std::string sName, sDescription, sDefaultValue;
	pGroup->getParameterInfo(nIndex, sName, sDescription, sDefaultValue);
	return sName;
}

std::string CStateEnvironment::GetParameterGroupParameterDescription(const std::string& sParameterGroup, const LibMCEnv_uint32 nIndex)
{
	if (!m_pParameterHandler->hasGroup(sParameterGroup))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_PARAMETERGROUPNOTFOUND);

	auto pGroup = m_pParameterHandler->findGroup(sParameterGroup, true);
	if (nIndex >= pGroup->getParameterCount())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM, "parameter index out of range: " + sParameterGroup + "/" + std::to_string(nIndex));

	std::string sName, sDescription, sDefaultValue;
	pGroup->getParameterInfo(nIndex, sName, sDescription, sDefaultValue);
	return sDescription;
}

// Maps an internal parameter data type to the plugin-facing enum.
static LibMCEnv::eParameterDataType parameterDataTypeToEnum(AMC::eParameterDataType eType)
{
	switch (eType) {
		case AMC::eParameterDataType::String: return LibMCEnv::eParameterDataType::String;
		case AMC::eParameterDataType::UUID: return LibMCEnv::eParameterDataType::UUID;
		case AMC::eParameterDataType::Integer: return LibMCEnv::eParameterDataType::Integer;
		case AMC::eParameterDataType::Double: return LibMCEnv::eParameterDataType::Double;
		case AMC::eParameterDataType::Bool: return LibMCEnv::eParameterDataType::Bool;
		default: return LibMCEnv::eParameterDataType::Unknown;
	}
}

LibMCEnv::eParameterDataType CStateEnvironment::GetParameterGroupParameterType(const std::string& sParameterGroup, const std::string& sParameterName)
{
	if (!m_pParameterHandler->hasGroup(sParameterGroup))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_PARAMETERGROUPNOTFOUND);

	auto pGroup = m_pParameterHandler->findGroup(sParameterGroup, true);
	return parameterDataTypeToEnum(pGroup->getParameterDataTypeByName(sParameterName));
}

bool CStateEnvironment::HasResourceData(const std::string& sIdentifier)
{
	auto pUIHandler = m_pSystemState->uiHandler();
	if (pUIHandler == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INTERNALERROR);

	auto pResourcePackage = pUIHandler->getCoreResourcePackage();
	if (pResourcePackage.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INTERNALERROR);

	auto pResourceEntry = pResourcePackage->findEntryByName(sIdentifier, false);

	return (pResourceEntry.get() != nullptr);
}


void CStateEnvironment::LoadResourceData(const std::string& sResourceName, LibMCEnv_uint64 nResourceDataBufferSize, LibMCEnv_uint64* pResourceDataNeededCount, LibMCEnv_uint8* pResourceDataBuffer)
{
	auto pUIHandler = m_pSystemState->uiHandler();
	if (pUIHandler == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INTERNALERROR);

	auto pResourcePackage = pUIHandler->getCoreResourcePackage();
	if (pResourcePackage.get () == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INTERNALERROR);

	auto pResourceEntry = pResourcePackage->findEntryByName(sResourceName, true);
	auto nResourceSize = pResourceEntry->getSize();

	if (pResourceDataNeededCount != nullptr)
		*pResourceDataNeededCount = nResourceSize;

	if (pResourceDataBuffer != nullptr) {
		if (nResourceDataBufferSize < nResourceSize)
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_BUFFERTOOSMALL);

		pResourcePackage->readEntryEx(sResourceName, pResourceDataBuffer, nResourceDataBufferSize);
	}


}

std::string CStateEnvironment::LoadResourceString(const std::string& sResourceName)
{
	auto pUIHandler = m_pSystemState->uiHandler();
	if (pUIHandler == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INTERNALERROR);

	auto pResourcePackage = pUIHandler->getCoreResourcePackage();
	if (pResourcePackage.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INTERNALERROR);

	return pResourcePackage->readEntryUTF8String(sResourceName);
}



IImageData* CStateEnvironment::CreateEmptyImage(const LibMCEnv_uint32 nPixelSizeX, const LibMCEnv_uint32 nPixelSizeY, const LibMCEnv_double dDPIValueX, const LibMCEnv_double dDPIValueY, const LibMCEnv::eImagePixelFormat ePixelFormat)
{
	return CImageData::createEmpty(nPixelSizeX, nPixelSizeY, dDPIValueX, dDPIValueY, ePixelFormat);
}

IImageLoader* CStateEnvironment::CreateImageLoader()
{
	auto pUIHandler = m_pSystemState->uiHandler();
	if (pUIHandler == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INTERNALERROR);

	auto pResourcePackage = pUIHandler->getCoreResourcePackage();
	if (pResourcePackage.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INTERNALERROR);

	return new CImageLoader(pResourcePackage);

}

IVideoStream* CStateEnvironment::CreateVideoStream(const LibMCEnv_uint32 nPixelSizeX, const LibMCEnv_uint32 nPixelSizeY, const LibMCEnv_uint32 nDesiredFrameDurationInMicroseconds, const LibMCEnv_uint32 nPauseToleranceInMicroseconds, const LibMCEnv_uint32 nFrameCacheDurationInMicroseconds)
{
	auto pRegistry = m_pSystemState->getStreamRegistryInstance();
	if (pRegistry.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INTERNALERROR);

	auto pInstance = pRegistry->createVideoStream(nPixelSizeX, nPixelSizeY, nDesiredFrameDurationInMicroseconds, nPauseToleranceInMicroseconds, nFrameCacheDurationInMicroseconds);

	return new CVideoStream(pInstance);
}

IVideoStream* CStateEnvironment::FindVideoStream(const std::string& sStreamUUID)
{
	auto pRegistry = m_pSystemState->getStreamRegistryInstance();
	if (pRegistry.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INTERNALERROR);

	auto pInstance = pRegistry->findVideoStream(sStreamUUID);
	if (pInstance.get() == nullptr)
		return nullptr;

	return new CVideoStream(pInstance);
}

LibMCEnv_uint64 CStateEnvironment::GetGlobalTimerInMilliseconds()
{
	return GetGlobalTimerInMicroseconds () / 1000ULL;
}

LibMCEnv_uint64 CStateEnvironment::GetGlobalTimerInMicroseconds()
{
	auto pGlobalChrono = m_pSystemState->getGlobalChronoInstance();
	uint64_t nStartTime = pGlobalChrono->getStartTimeStampInMicrosecondsSince1970();
	uint64_t nCurrentTime = pGlobalChrono->getUTCTimeStampInMicrosecondsSince1970();

	if (nCurrentTime < nStartTime)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_GLOBALTIMERNOTCONTINUOUS);

	return nCurrentTime - nStartTime;
}

LibMCEnv_uint64 CStateEnvironment::GetStartTimeOfStateInMilliseconds()
{
	return GetStartTimeOfStateInMicroseconds() / 1000ULL;
}

LibMCEnv_uint64 CStateEnvironment::GetStartTimeOfStateInMicroseconds()
{
	auto pGlobalChrono = m_pSystemState->getGlobalChronoInstance();
	uint64_t nGlobalStartTime = pGlobalChrono->getStartTimeStampInMicrosecondsSince1970();
	if (m_nAbsoluteStartTimeOfStateInMicroseconds < nGlobalStartTime)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_GLOBALTIMERNOTCONTINUOUS);

	return m_nAbsoluteStartTimeOfStateInMicroseconds - nGlobalStartTime;
}

LibMCEnv_uint64 CStateEnvironment::GetEndTimeOfPreviousStateInMilliseconds()
{
	return GetEndTimeOfPreviousStateInMicroseconds () / 1000ULL;
}

LibMCEnv_uint64 CStateEnvironment::GetEndTimeOfPreviousStateInMicroseconds()
{
	auto pGlobalChrono = m_pSystemState->getGlobalChronoInstance();
	uint64_t nGlobalStartTime = pGlobalChrono->getStartTimeStampInMicrosecondsSince1970();
	if (m_nAbsoluteEndTimeOfPreviousStateInMicroseconds < nGlobalStartTime)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_GLOBALTIMERNOTCONTINUOUS);

	return m_nAbsoluteEndTimeOfPreviousStateInMicroseconds - nGlobalStartTime;
}

LibMCEnv_uint64 CStateEnvironment::GetElapsedTimeInStateInMilliseconds()
{
	return GetElapsedTimeInStateInMicroseconds() / 1000ULL;
}


LibMCEnv_uint64 CStateEnvironment::GetElapsedTimeInStateInMicroseconds()
{
	auto pGlobalChrono = m_pSystemState->getGlobalChronoInstance();
	uint64_t nCurrentTime = pGlobalChrono->getUTCTimeStampInMicrosecondsSince1970();
	if (nCurrentTime < m_nAbsoluteStartTimeOfStateInMicroseconds)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_GLOBALTIMERNOTCONTINUOUS);

	return nCurrentTime - m_nAbsoluteStartTimeOfStateInMicroseconds;
}


ITestEnvironment* CStateEnvironment::GetTestEnvironment()
{
	return new CTestEnvironment(m_pSystemState->getTestEnvironmentPath ());
}

LibMCEnv::Impl::IXMLDocument* CStateEnvironment::CreateXMLDocument(const std::string& sRootNodeName, const std::string& sDefaultNamespace)
{
	auto pDocument = std::make_shared<AMC::CXMLDocumentInstance>();

	pDocument->createEmptyDocument(sRootNodeName, sDefaultNamespace);

	return new CXMLDocument(pDocument);
}

LibMCEnv::Impl::IXMLDocument* CStateEnvironment::ParseXMLString(const std::string& sXMLString)
{
	auto pDocument = std::make_shared<AMC::CXMLDocumentInstance>();

	try {
		pDocument->parseXMLString(sXMLString);
	}
	catch (std::exception& E) {
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNOTPARSEXMLSTRING, "could not parse XML string: " + std::string(E.what()));
	}

	return new CXMLDocument(pDocument);

}

IDataTable* CStateEnvironment::CreateDataTable()
{
	return new CDataTable(m_pSystemState->getToolpathHandlerInstance ());
}

LibMCEnv::Impl::IXMLDocument* CStateEnvironment::ParseXMLData(const LibMCEnv_uint64 nXMLDataBufferSize, const LibMCEnv_uint8* pXMLDataBuffer)
{
	auto pDocument = std::make_shared<AMC::CXMLDocumentInstance>();

	try {
		pDocument->parseXMLData(nXMLDataBufferSize, pXMLDataBuffer);
	}
	catch (std::exception& E) {
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNOTPARSEXMLDATA, "could not parse XML data: " + std::string(E.what()));
	}

	return new CXMLDocument(pDocument);

}

IJSONObject* CStateEnvironment::CreateJSONObject()
{
	return new CJSONObject();
}

IJSONObject* CStateEnvironment::ParseJSONString(const std::string& sJSONString)
{
	return new CJSONObject(sJSONString);
}

IJSONObject* CStateEnvironment::ParseJSONData(const LibMCEnv_uint64 nJSONDataBufferSize, const LibMCEnv_uint8* pJSONDataBuffer)
{
	return new CJSONObject(pJSONDataBuffer, nJSONDataBufferSize);
}

IMachineConfigurationHandler* CStateEnvironment::CreateMachineConfigurationHandler()
{
	return new CMachineConfigurationHandler(m_pSystemState->getDataModelInstance ());
}


IDiscreteFieldData2D* CStateEnvironment::CreateDiscreteField2D(const LibMCEnv_uint32 nPixelSizeX, const LibMCEnv_uint32 nPixelSizeY, const LibMCEnv_double dDPIValueX, const LibMCEnv_double dDPIValueY, const LibMCEnv_double dOriginX, const LibMCEnv_double dOriginY, const LibMCEnv_double dDefaultValue)
{
	AMC::PDiscreteFieldData2DInstance pInstance = std::make_shared<AMC::CDiscreteFieldData2DInstance>(nPixelSizeX, nPixelSizeY, dDPIValueX, dDPIValueY, dOriginX, dOriginY, dDefaultValue, true);
	return new CDiscreteFieldData2D(pInstance);
}

IDiscreteFieldData2D* CStateEnvironment::CreateDiscreteField2DFromImage(IImageData* pImageDataInstance, const LibMCEnv_double dBlackValue, const LibMCEnv_double dWhiteValue, const LibMCEnv_double dOriginX, const LibMCEnv_double dOriginY)
{
	if (pImageDataInstance == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

	auto pImageDataImpl = dynamic_cast<CImageData*> (pImageDataInstance);
	if (pImageDataImpl == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDCAST);

	uint32_t nPixelSizeX, nPixelSizeY;
	pImageDataImpl->GetSizeInPixels(nPixelSizeX, nPixelSizeY);

	double dDPIValueX, dDPIValueY;
	pImageDataImpl->GetDPI(dDPIValueX, dDPIValueY);

	AMC::PDiscreteFieldData2DInstance pFieldInstance = std::make_shared<AMC::CDiscreteFieldData2DInstance>(nPixelSizeX, nPixelSizeY, dDPIValueX, dDPIValueY, dOriginX, dOriginY, 0.0, false);

	auto pixelFormat = pImageDataImpl->GetPixelFormat();
	auto& rawPixelData = pImageDataImpl->getPixelData();
	
	pFieldInstance->loadFromRawPixelData (rawPixelData, pixelFormat, dBlackValue, dWhiteValue);

	return new CDiscreteFieldData2D(pFieldInstance);

}

bool CStateEnvironment::CheckUserPermission(const std::string& sUserLogin, const std::string& sPermissionIdentifier)
{
	if (sUserLogin.empty())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_EMPTYUSERLOGIN);
	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString (sUserLogin))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDUSERLOGIN, sUserLogin);

	if (sPermissionIdentifier.empty())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_EMPTYPERMISSIONIDENTIFIER);
	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sPermissionIdentifier))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPERMISSIONIDENTIFIER, sPermissionIdentifier);

	auto pDataModel = m_pSystemState->getDataModelInstance();
	auto pLoginHandler = pDataModel->CreateLoginHandler();
	auto pAccessControl = m_pSystemState->accessControl();

	std::string sUserRole = pLoginHandler->GetUserRole (sUserLogin);
	AMC::PAccessRole pRole;
	if (sUserRole.empty())
		pRole = pAccessControl->findRole(sUserRole, true);
	else
		pRole = pAccessControl->getDefaultRole();

	return pRole->hasPermission(sPermissionIdentifier);
}

IUserManagementHandler* CStateEnvironment::CreateUserManagement()
{
	return new CUserManagementHandler (m_pSystemState->getDataModelInstance (), m_pSystemState->getAccessControlInstance (), m_pSystemState->getLanguageHandlerInstance ());
}

IJournalHandler* CStateEnvironment::GetCurrentJournal()
{
	return new CJournalHandler_Current(m_pSystemState->getStateJournalInstance());
}

/*IMeshObject* CStateEnvironment::RegisterMeshFrom3MFResource(const std::string& sResourceName)
{
	auto pUIHandler = m_pSystemState->uiHandler();
	if (pUIHandler == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INTERNALERROR);

	auto pResourcePackage = pUIHandler->getCoreResourcePackage();
	if (pResourcePackage.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INTERNALERROR);

	auto pResourceEntry = pResourcePackage->findEntryByName(sResourceName, true);

	auto pMeshHandler = m_pSystemState->getMeshHandlerInstance();
	auto pLib3MFWrapper = m_pSystemState->getToolpathHandlerInstance()->getLib3MFWrapper();
	
	auto pMeshEntity = pMeshHandler->register3MFResource(pLib3MFWrapper.get(), pResourcePackage.get(), sResourceName);

	return new CMeshObject(pMeshHandler, pMeshEntity->getUUID());
}

*/


ISceneHandler* CStateEnvironment::CreateSceneHandler()
{
	auto pToolpathHandler = m_pSystemState->getToolpathHandlerInstance();
	if (pToolpathHandler == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INTERNALERROR);

	auto pLib3MFWrapper = pToolpathHandler->getLib3MFWrapper();
	if (pLib3MFWrapper == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INTERNALERROR);

	auto pMeshHandler = m_pSystemState->getMeshHandlerInstance();
	if (pMeshHandler == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INTERNALERROR);

	auto pUIHandler = m_pSystemState->uiHandler();
	if (pUIHandler == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INTERNALERROR);

	auto pCoreResourcePackage = pUIHandler->getCoreResourcePackage();
	if (pCoreResourcePackage.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INTERNALERROR);

	return new CSceneHandler(pMeshHandler, pLib3MFWrapper, pCoreResourcePackage);
}


IDataSeries* CStateEnvironment::CreateDataSeries(const std::string& sName)
{
	auto pDataSeriesHandler = m_pSystemState->getDataSeriesHandlerInstance();
	auto pDataSeries = pDataSeriesHandler->createDataSeries(sName);

	return new CDataSeries(pDataSeries);

}

bool CStateEnvironment::HasDataSeries(const std::string& sDataSeriesUUID)
{
	auto pDataSeriesHandler = m_pSystemState->getDataSeriesHandlerInstance();
	return pDataSeriesHandler->hasDataSeries(sDataSeriesUUID);
}

IDataSeries* CStateEnvironment::FindDataSeries(const std::string& sDataSeriesUUID)
{
	auto pDataSeriesHandler = m_pSystemState->getDataSeriesHandlerInstance();
	auto pDataSeries = pDataSeriesHandler->findDataSeries(sDataSeriesUUID, true);

	return new CDataSeries(pDataSeries);


}

void CStateEnvironment::ReleaseDataSeries(const std::string& sDataSeriesUUID)
{
	auto pDataSeriesHandler = m_pSystemState->getDataSeriesHandlerInstance();
	pDataSeriesHandler->unloadDataSeries(sDataSeriesUUID);

}


IAlert* CStateEnvironment::CreateAlert(const std::string& sIdentifier, const std::string& sReadableContextInformation, const bool bAutomaticLogEntry) 
{
	if (sIdentifier.empty())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_EMPTYALERTIDENTIFIER);

	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString (sIdentifier))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDALERTIDENTIFIER, "invalid alert identifier: " + sIdentifier);

	auto sNewUUID = AMCCommon::CUtils::createUUID();
	
	auto sTimeStamp = m_pSystemState->globalChrono ()->getUTCTimeInISO8601 ();

	auto pDefinition = m_pSystemState->alertHandler()->findDefinition(sIdentifier, true);
	auto alertDescription = pDefinition->getDescription();
	auto alertLevel = pDefinition->getAlertLevel();

	auto pDataModel = m_pSystemState->getDataModelInstance();
	auto pAlertSession = pDataModel->CreateAlertSession();

	auto pAlertData = pAlertSession->AddAlert (sNewUUID, pDefinition->getIdentifier (), alertLevel, alertDescription.getCustomValue (), alertDescription.getStringIdentifier (), sReadableContextInformation, pDefinition->needsAcknowledgement (), sTimeStamp);

	if (bAutomaticLogEntry) {
		std::string sLogString = "Created alert " + sNewUUID + ": " + pDefinition->getIdentifier() + " / " + alertDescription.getCustomValue();
		AMC::eLogLevel logLevel;
		switch (alertLevel) {
		case LibMCData::eAlertLevel::FatalError:
			logLevel = AMC::eLogLevel::FatalError;
			break;
		case LibMCData::eAlertLevel::CriticalError:
			logLevel = AMC::eLogLevel::CriticalError;
			break;
		case LibMCData::eAlertLevel::Message:
			logLevel = AMC::eLogLevel::Message;
			break;
		case LibMCData::eAlertLevel::Warning:
			logLevel = AMC::eLogLevel::Warning;
			break;
		default:
			logLevel = AMC::eLogLevel::Unknown;
			break;

		}

		auto pLogger = m_pSystemState->getLoggerInstance();
		pLogger->logMessage(sLogString, m_sInstanceName, logLevel);
	}

	// State Environments have no user context...
	std::string sCurrentUserUUID = AMCCommon::CUtils::createEmptyUUID();
	return new CAlert (pDataModel, pAlertData->GetUUID (), sCurrentUserUUID, m_pSystemState->getLoggerInstance (), m_sInstanceName, m_pSystemState->getGlobalChronoInstance ());
}

IAlert* CStateEnvironment::FindAlert(const std::string& sUUID)
{
	std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);

	auto pDataModel = m_pSystemState->getDataModelInstance();
	auto pAlertSession = pDataModel->CreateAlertSession();

	if (!pAlertSession->HasAlert(sNormalizedUUID))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_ALERTNOTFOUND, "alert not found: " + sNormalizedUUID + " (in StateEnvironment::FindAlert)");

	auto pAlertData = pAlertSession->GetAlertByUUID(sNormalizedUUID);

	std::string sCurrentUserUUID = AMCCommon::CUtils::createEmptyUUID();
	return new CAlert(pDataModel, pAlertData->GetUUID(), sCurrentUserUUID, m_pSystemState->getLoggerInstance (), m_sInstanceName, m_pSystemState->getGlobalChronoInstance ());

}

bool CStateEnvironment::AlertExists(const std::string& sUUID)
{
	std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);

	auto pDataModel = m_pSystemState->getDataModelInstance();
	auto pAlertSession = pDataModel->CreateAlertSession();

	return pAlertSession->HasAlert(sNormalizedUUID);

}

IAlertIterator* CStateEnvironment::RetrieveAlerts(const bool bOnlyActive)
{
	std::unique_ptr<LibMCEnv::Impl::CAlertIterator> returnIterator (new CAlertIterator ());

	// State Environments have no user context...
	std::string sCurrentUserUUID = AMCCommon::CUtils::createEmptyUUID ();

	auto pDataModel = m_pSystemState->getDataModelInstance();
	auto pAlertSession = pDataModel->CreateAlertSession();

	auto pLogger = m_pSystemState->getLoggerInstance();

	auto pAlertIterator = pAlertSession->RetrieveAlerts(bOnlyActive);
	while (pAlertIterator->MoveNext()) {
		auto pAlertData = pAlertIterator->GetCurrentAlert();

		// State Environments have no user context...
		returnIterator->AddAlert(std::make_shared<CAlert>(pDataModel, pAlertData->GetUUID (), sCurrentUserUUID, pLogger, m_sInstanceName, m_pSystemState->getGlobalChronoInstance ()));
	}

	return returnIterator.release();
}

IAlertIterator* CStateEnvironment::RetrieveAlertsByType(const std::string& sIdentifier, const bool bOnlyActive)
{
	std::unique_ptr<LibMCEnv::Impl::CAlertIterator> returnIterator(new CAlertIterator());

	// State Environments have no user context...
	std::string sCurrentUserUUID = AMCCommon::CUtils::createEmptyUUID();

	auto pDataModel = m_pSystemState->getDataModelInstance();
	auto pAlertSession = pDataModel->CreateAlertSession();
	auto pLogger = m_pSystemState->getLoggerInstance();

	auto pAlertIterator = pAlertSession->RetrieveAlertsByType(sIdentifier, bOnlyActive);
	while (pAlertIterator->MoveNext()) {
		auto pAlertData = pAlertIterator->GetCurrentAlert();

		returnIterator->AddAlert(std::make_shared<CAlert>(pDataModel, pAlertData->GetUUID(), sCurrentUserUUID, pLogger, m_sInstanceName, m_pSystemState->getGlobalChronoInstance ()));
	}

	return returnIterator.release();
}


bool CStateEnvironment::HasAlertOfType(const std::string& sIdentifier, const bool bOnlyActive)
{
	// State Environments have no user context...
	std::string sCurrentUserUUID = AMCCommon::CUtils::createEmptyUUID();

	auto pDataModel = m_pSystemState->getDataModelInstance();
	auto pAlertSession = pDataModel->CreateAlertSession();
	auto pLogger = m_pSystemState->getLoggerInstance();

	auto pAlertIterator = pAlertSession->RetrieveAlertsByType(sIdentifier, bOnlyActive);
	return (pAlertIterator->MoveNext());
}

ICryptoContext* CStateEnvironment::CreateCryptoContext()
{
	return new CCryptoContext();
}


ITempStreamWriter* CStateEnvironment::CreateTemporaryStream(const std::string& sName, const std::string& sMIMEType)
{
	if (sName.empty())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_EMPTYJOURNALSTREAMNAME);
	if (sMIMEType.empty())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_EMPTYJOURNALSTREAMMIMETYPE);


	std::string sUserUUID = AMCCommon::CUtils::createEmptyUUID();
	return new CTempStreamWriter(m_pSystemState->getDataModelInstance(), sName, sMIMEType, sUserUUID, m_pSystemState->getGlobalChronoInstance());
}

IZIPStreamWriter* CStateEnvironment::CreateZIPStream(const std::string& sName)
{
	if (sName.empty())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_EMPTYJOURNALSTREAMNAME);

	auto pChrono = m_pSystemState->getGlobalChronoInstance();

	std::string sUserUUID = AMCCommon::CUtils::createEmptyUUID();
	std::string sStreamUUID = AMCCommon::CUtils::createUUID();

	auto pDataModel = m_pSystemState->getDataModelInstance();
	auto pStorage = pDataModel->CreateStorage();
	auto pZIPWriter = pStorage->CreateZIPStream(sStreamUUID, sName, sUserUUID, pChrono->getUTCTimeStampInMicrosecondsSince1970());

	return new CZIPStreamWriter (pDataModel, pZIPWriter, sStreamUUID, sName, pChrono);
}


IStreamReader* CStateEnvironment::LoadStream(const std::string& sUUID, const bool bMustExist)
{
	auto pDataModel = m_pSystemState->getDataModelInstance();
	auto pStorage = pDataModel->CreateStorage();

	std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);

	if (pStorage->StreamIsReady(sNormalizedUUID)) {

		auto pStorageStream = pStorage->RetrieveStream(sNormalizedUUID);
		return new CStreamReader(pStorage, pStorageStream);

	}
	else {
		if (bMustExist)
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_STORAGESTREAMNOTFOUND, "Storage Stream not found: " + sUUID);
	}

	return nullptr;

}


IDateTime* CStateEnvironment::GetCurrentDateTime()
{
	auto pChrono = m_pSystemState->getGlobalChronoInstance();
	return new CDateTime(pChrono->getUTCTimeStampInMicrosecondsSince1970());
		
}

IDateTime* CStateEnvironment::GetCustomDateTime(const LibMCEnv_uint32 nYear, const LibMCEnv_uint32 nMonth, const LibMCEnv_uint32 nDay, const LibMCEnv_uint32 nHour, const LibMCEnv_uint32 nMinute, const LibMCEnv_uint32 nSecond, const LibMCEnv_uint32 nMicrosecond)
{
	return new CDateTime(AMCCommon::CChrono::getMicrosecondsSince1970FromDateTime(nYear, nMonth, nDay, nHour, nMinute, nSecond, nMicrosecond));
}

IDateTime* CStateEnvironment::GetStartDateTime()
{
	auto pGlobalChrono = m_pSystemState->getGlobalChronoInstance();
	return new CDateTime(pGlobalChrono->getStartTimeStampInMicrosecondsSince1970 ());
}


ITelemetryChannel* CStateEnvironment::RegisterTelemetryChannel(const std::string& sChannelIdentifier, const std::string& sChannelDescription, const LibMCEnv::eTelemetryChannelType eChannelType)
{
	if (!AMCCommon::CUtils::stringIsValidAlphanumericPathString (sChannelIdentifier))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM, "invalid state machine telemetry channel identifier: " + sChannelIdentifier + " (state machine " + m_sInstanceName + ")");
	
	std::string sIdentifier = m_sInstanceName + "." + sChannelIdentifier;

	auto pTelemetryHandler = m_pSystemState->getTelemetryHandlerInstance();
	pTelemetryHandler->registerChannel(sIdentifier, sChannelDescription, CTelemetryChannel::mapEnvChannelTypeToDataChannelType (eChannelType));

	return new CTelemetryChannel(pTelemetryHandler, m_sInstanceName, sChannelIdentifier);
}

ITelemetryChannel* CStateEnvironment::FindTelemetryChannel(const std::string& sChannelIdentifier, const bool bFailIfNotExisting)
{
	if (!AMCCommon::CUtils::stringIsValidAlphanumericPathString(sChannelIdentifier))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM, "invalid state machine telemetry channel identifier: " + sChannelIdentifier + " (state machine " + m_sInstanceName + ")");

	std::string sIdentifier = m_sInstanceName + "." + sChannelIdentifier;

	auto pTelemetryHandler = m_pSystemState->getTelemetryHandlerInstance();
	pTelemetryHandler->findChannelByIdentifier(sIdentifier, true);

	return new CTelemetryChannel(pTelemetryHandler, m_sInstanceName, sChannelIdentifier);

}

