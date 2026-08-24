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

#include "libmcenv_signalhandler.hpp"
#include "libmcenv_interfaceexception.hpp"

// Include custom headers here.
#include "Common/common_utils.hpp"

using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CSignalHandler 
**************************************************************************************************************************/

CSignalHandler::CSignalHandler(AMC::PStateSignalHandler pSignalHandler, std::string& sSignalUUID, AMCCommon::PChrono pGlobalChrono)
	: m_pSignalHandler(pSignalHandler), m_sSignalUUID(sSignalUUID), m_pGlobalChrono (pGlobalChrono)
{
	if (pSignalHandler.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
	if (pGlobalChrono.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

	m_pParameterGroup = std::make_shared<AMC::CParameterGroup>(pGlobalChrono);
	m_pResultGroup = std::make_shared<AMC::CParameterGroup>(pGlobalChrono);

	std::string sParameterData;
	if (!m_pSignalHandler->findSignalPropertiesByUUID(m_sSignalUUID, m_sInstanceName, m_sSignalName, sParameterData))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_SIGNALNOTFOUND, "findSignalPropertiesByUUID: " + m_sSignalUUID + " (" + m_sInstanceName + "/" + m_sSignalName + ")");

	auto pSignalInstance = m_pSignalHandler->getInstance(m_sInstanceName);

	pSignalInstance->populateParameterGroup(m_sSignalName, m_pParameterGroup.get());
	pSignalInstance->populateResultGroup(m_sSignalName, m_pResultGroup.get());

	m_pParameterGroup->deserializeJSON(sParameterData, m_pGlobalChrono->getUTCTimeStampInMicrosecondsSince1970());

}

CSignalHandler::CSignalHandler(AMC::PStateSignalHandler pSignalHandler, const std::string& sInstanceName, const std::string& sSignalName, const std::string& sSignalUUID, const std::string& sParameterDataJSON, AMCCommon::PChrono pGlobalChrono)
	: m_pSignalHandler(pSignalHandler), m_sSignalUUID(AMCCommon::CUtils::normalizeUUIDString(sSignalUUID)), m_pGlobalChrono(pGlobalChrono)
{
	if (pSignalHandler.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
	if (pGlobalChrono.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

	m_sInstanceName = sInstanceName;
	m_sSignalName = sSignalName;

	m_pParameterGroup = std::make_shared<AMC::CParameterGroup>(pGlobalChrono);
	m_pResultGroup = std::make_shared<AMC::CParameterGroup>(pGlobalChrono);

	auto pSignalInstance = m_pSignalHandler->getInstance(m_sInstanceName);
	pSignalInstance->populateParameterGroup(m_sSignalName, m_pParameterGroup.get());
	pSignalInstance->populateResultGroup(m_sSignalName, m_pResultGroup.get());

	m_pParameterGroup->deserializeJSON(sParameterDataJSON, m_pGlobalChrono->getUTCTimeStampInMicrosecondsSince1970());
}

void CSignalHandler::SignalHandled()
{
	m_pSignalHandler->changeSignalPhaseToHandled (m_sSignalUUID, m_pResultGroup->serializeToJSON (), m_pGlobalChrono->getElapsedMicroseconds());
}

LibMCEnv::eSignalPhase CSignalHandler::GetSignalPhase()
{
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


void CSignalHandler::SignalInProcess()
{	
	m_pSignalHandler->changeSignalPhaseToInProcess (m_sSignalUUID, m_pGlobalChrono->getElapsedMicroseconds());
}

void CSignalHandler::SignalFailed(const std::string& sErrorMessage)
{
	m_pSignalHandler->changeSignalPhaseToFailed(m_sSignalUUID, m_pResultGroup->serializeToJSON(), sErrorMessage, m_pGlobalChrono->getElapsedMicroseconds());
}


std::string CSignalHandler::GetName()
{
	return m_sSignalName;
}

std::string CSignalHandler::GetStateMachine()
{
	return m_sInstanceName;
}

std::string CSignalHandler::GetSignalUUID()
{
	return m_sSignalUUID;
}

std::string CSignalHandler::GetString(const std::string & sName)
{
	return m_pParameterGroup->getParameterValueByName(sName);
}

std::string CSignalHandler::GetUUID(const std::string& sName)
{
	return AMCCommon::CUtils::normalizeUUIDString (m_pParameterGroup->getParameterValueByName(sName));
}

LibMCEnv_double CSignalHandler::GetDouble(const std::string & sName)
{
	return m_pParameterGroup->getDoubleParameterValueByName(sName);
}

LibMCEnv_int64 CSignalHandler::GetInteger(const std::string & sName)
{
	return m_pParameterGroup->getIntParameterValueByName(sName);
}

bool CSignalHandler::GetBool(const std::string & sName)
{
	return m_pParameterGroup->getBoolParameterValueByName(sName);
}

void CSignalHandler::SetStringResult(const std::string & sName, const std::string & sValue)
{
	m_pResultGroup->setParameterValueByName(sName, sValue);
}

void CSignalHandler::SetUUIDResult(const std::string& sName, const std::string& sValue)
{
	m_pResultGroup->setParameterValueByName(sName, AMCCommon::CUtils::normalizeUUIDString (sValue));
}


void CSignalHandler::SetDoubleResult(const std::string & sName, const LibMCEnv_double dValue)
{
	m_pResultGroup->setDoubleParameterValueByName(sName, dValue);
}

void CSignalHandler::SetIntegerResult(const std::string & sName, const LibMCEnv_int64 nValue)
{
	m_pResultGroup->setIntParameterValueByName(sName, nValue);
}

void CSignalHandler::SetBoolResult(const std::string & sName, const bool bValue)
{
	m_pResultGroup->setBoolParameterValueByName(sName, bValue);
}

