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


#include "libmcenv_signaltrigger.hpp"
#include "libmcenv_interfaceexception.hpp"
#include "amc_statesignalhandler.hpp"

// Include custom headers here.
#include "common_utils.hpp"
#include "common_chrono.hpp"


using namespace LibMCEnv::Impl;

CSignalTrigger::CSignalTrigger(AMC::PStateSignalHandler pSignalHandler, std::string sInstanceName, std::string sSignalName, AMCCommon::PChrono pGlobalChrono)
	: m_pSignalHandler (pSignalHandler), 
	m_sInstanceName (sInstanceName), 
	m_sSignalName (sSignalName), 
	m_pGlobalChrono (pGlobalChrono)
	
{
	if (pSignalHandler.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
	if (pGlobalChrono.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

	m_sSignalUUID = AMCCommon::CUtils::createUUID();
	m_bIsPreparing = true;
	

	auto pSignalInstance = m_pSignalHandler->getInstance(m_sInstanceName);
	m_nReactionTimeOutInMs = pSignalInstance->getDefaultReactionTimeout (m_sSignalName);

	m_pParameterGroup = std::make_shared<AMC::CParameterGroup>(pGlobalChrono);
	m_pResultGroup = std::make_shared<AMC::CParameterGroup>(pGlobalChrono);

	pSignalInstance->populateParameterGroup (m_sSignalName, m_pParameterGroup.get());
	pSignalInstance->populateResultGroup(m_sSignalName, m_pResultGroup.get());

}

CSignalTrigger::~CSignalTrigger()
{
}

std::string CSignalTrigger::GetSignalUUID()
{
	return m_sSignalUUID;
}

bool CSignalTrigger::CanTrigger()
{
	if (!m_bIsPreparing)
		return false;

	auto pSignalInstance = m_pSignalHandler->getInstance(m_sInstanceName);

	return pSignalInstance->canTrigger(m_sSignalName);
}


LibMCEnv_uint32 CSignalTrigger::GetAvailableSignalQueueSlots()
{
	auto pSignalInstance = m_pSignalHandler->getInstance(m_sInstanceName);
	return pSignalInstance->getAvailableSignalQueueEntryCount(m_sSignalName);
}

LibMCEnv_uint32 CSignalTrigger::GetTotalSignalQueueSlots()
{
	auto pSignalInstance = m_pSignalHandler->getInstance(m_sInstanceName);
	return pSignalInstance->getTotalSignalQueueSize(m_sSignalName);
}

LibMCEnv::eSignalPhase CSignalTrigger::GetSignalPhase()
{
	if (m_bIsPreparing)
		return LibMCEnv::eSignalPhase::InPreparation;

	AMC::eAMCSignalPhase signalPhase = m_pSignalHandler->getSignalPhase(m_sSignalUUID);

	switch (signalPhase) {
	case AMC::eAMCSignalPhase::Invalid:
		return LibMCEnv::eSignalPhase::Invalid;
	case AMC::eAMCSignalPhase::InPreparation:
		return LibMCEnv::eSignalPhase::InPreparation;
	case AMC::eAMCSignalPhase::InQueue:
		return LibMCEnv::eSignalPhase::InQueue;
	case AMC::eAMCSignalPhase::InProcess:
		return LibMCEnv::eSignalPhase::InProcess;
	case AMC::eAMCSignalPhase::Handled:
		return LibMCEnv::eSignalPhase::Handled;
	case AMC::eAMCSignalPhase::Failed:
		return LibMCEnv::eSignalPhase::Failed;
	case AMC::eAMCSignalPhase::TimedOut:
		return LibMCEnv::eSignalPhase::TimedOut;
	case AMC::eAMCSignalPhase::Cleared:
		return LibMCEnv::eSignalPhase::Cleared;
	case AMC::eAMCSignalPhase::Archived:
		return LibMCEnv::eSignalPhase::Archived;

	default:
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_UNDEFINEDINTERNALSIGNALPHASE);
	}
}

void CSignalTrigger::SetReactionTimeOut(const LibMCEnv_uint32 nReactionTimeOutInMs)
{
	if (nReactionTimeOutInMs < AMC_SIGNAL_MINREACTIONTIMEINMS || nReactionTimeOutInMs > AMC_SIGNAL_MAXREACTIONTIMEINMS)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDREACTIONTIMEOUT, std::to_string (nReactionTimeOutInMs));

	if (m_bIsPreparing) {
		m_nReactionTimeOutInMs = nReactionTimeOutInMs;
	} else {
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNOTSETREACTIONTIMEOUT);
	}
}

LibMCEnv_uint32 CSignalTrigger::GetReactionTimeOut()
{
	if (m_bIsPreparing) {
		return m_nReactionTimeOutInMs;
	} else {
		return m_pSignalHandler->getReactionTimeout(m_sSignalUUID);
	}
}


void CSignalTrigger::Trigger()
{
	if (!TryTrigger ())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNOTTRIGGERSIGNAL);

}

bool CSignalTrigger::TryTrigger()
{
	auto pSignalInstance = m_pSignalHandler->getInstance(m_sInstanceName);
	bool bSuccess = pSignalInstance->addNewInQueueSignal(m_sSignalName, m_sSignalUUID, m_pParameterGroup->serializeToJSON(), m_nReactionTimeOutInMs, m_pGlobalChrono->getElapsedMicroseconds());
	if (bSuccess ) {
		m_bIsPreparing = false;
		return true;

	} else {
		return false;

	}
}

bool CSignalTrigger::TryTriggerWithTimeout(const LibMCEnv_uint32 nReactionTimeOutInMs)
{
	SetReactionTimeOut(nReactionTimeOutInMs);
	return TryTrigger();
}

bool CSignalTrigger::HasBeenHandled()
{
	return WaitForHandling(0);
}

bool CSignalTrigger::WaitForHandling(const LibMCEnv_uint32 nTimeOutInMilliseconds)
{

	AMCCommon::CChrono chrono;

	uint64_t nTimeOutTimeStamp = chrono.getUTCTimeStampInMicrosecondsSince1970() + (nTimeOutInMilliseconds * 1000ULL);

	if (m_bIsPreparing)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_SIGNALHASNOTBEENTRIGGERED);

	bool bIsTimeOut = false;
	while (!bIsTimeOut) {	

		auto signalPhase = m_pSignalHandler->getSignalPhase(m_sSignalUUID);
		bool bHasBeenHandled = (signalPhase == AMC::eAMCSignalPhase::Handled) || (signalPhase == AMC::eAMCSignalPhase::Failed) || (signalPhase == AMC::eAMCSignalPhase::Cleared) || (signalPhase == AMC::eAMCSignalPhase::Archived) || (signalPhase == AMC::eAMCSignalPhase::TimedOut);
		
		if (bHasBeenHandled) {
			std::string sResultDataJSON = m_pSignalHandler->getResultDataJSON (m_sSignalUUID);
			if (!sResultDataJSON.empty()) {
				m_pResultGroup->deserializeJSON(sResultDataJSON, m_pGlobalChrono->getUTCTimeStampInMicrosecondsSince1970());
			}
			
			return true;
		} 

		bIsTimeOut = chrono.getUTCTimeStampInMicrosecondsSince1970() > nTimeOutTimeStamp;

		if (!bIsTimeOut) {			
			chrono.sleepMilliseconds(AMC_SIGNAL_DEFAULT_WAITFOR_SLEEP_MS);
		}
	} 

	return false;
}

std::string CSignalTrigger::GetName()
{
	return m_sSignalName;
}

std::string CSignalTrigger::GetStateMachine()
{
	return m_sInstanceName;
}

void CSignalTrigger::SetString(const std::string & sName, const std::string & sValue)
{
	m_pParameterGroup->setParameterValueByName(sName, sValue);
}

void CSignalTrigger::SetUUID(const std::string& sName, const std::string& sValue)
{
	m_pParameterGroup->setParameterValueByName(sName, AMCCommon::CUtils::normalizeUUIDString (sValue));
}


void CSignalTrigger::SetDouble(const std::string & sName, const LibMCEnv_double dValue)
{
	m_pParameterGroup->setDoubleParameterValueByName(sName, dValue);
}

void CSignalTrigger::SetInteger(const std::string & sName, const LibMCEnv_int64 nValue)
{
	m_pParameterGroup->setIntParameterValueByName(sName, nValue);
}

void CSignalTrigger::SetBool(const std::string & sName, const bool bValue)
{
	m_pParameterGroup->setBoolParameterValueByName(sName, bValue);
}

std::string CSignalTrigger::GetStringResult(const std::string & sName)
{
	return m_pResultGroup->getParameterValueByName(sName);
}


std::string CSignalTrigger::GetUUIDResult(const std::string& sName)
{
	return AMCCommon::CUtils::normalizeUUIDString (m_pResultGroup->getParameterValueByName(sName));
}


LibMCEnv_double CSignalTrigger::GetDoubleResult(const std::string & sName)
{
	return m_pResultGroup->getDoubleParameterValueByName(sName);
}

LibMCEnv_int64 CSignalTrigger::GetIntegerResult(const std::string & sName)
{
	return m_pResultGroup->getIntParameterValueByName(sName);
}

bool CSignalTrigger::GetBoolResult(const std::string & sName)
{
	return m_pResultGroup->getBoolParameterValueByName(sName);
}

