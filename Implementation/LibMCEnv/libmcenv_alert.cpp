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

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL AUTODESK INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


Abstract: This is a stub class definition of CAlert

*/

#include "libmcenv_alert.hpp"
#include "libmcenv_interfaceexception.hpp"
#include "libmcenv_datetime.hpp"

// Include custom headers here.
#include "common_utils.hpp"
#include "common_chrono.hpp"

using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CAlert 
**************************************************************************************************************************/

CAlert::CAlert(LibMCData::PDataModel pDataModel, const std::string& sUUID, const std::string& sCurrentUserUUID, AMC::PLogger pLogger, const std::string& sLogInstance, AMCCommon::PChrono pGlobalChrono)
	: m_pDataModel(pDataModel), m_pLogger (pLogger), m_sLogInstance (sLogInstance), m_pGlobalChrono (pGlobalChrono)
{
	if (pDataModel.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
	if (pLogger.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
	if (pGlobalChrono.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

	m_sCurrentUserUUID = AMCCommon::CUtils::normalizeUUIDString(sCurrentUserUUID);
	m_bUserContextExists = (m_sCurrentUserUUID != AMCCommon::CUtils::createEmptyUUID());

	m_pAlertSession = pDataModel->CreateAlertSession();
	m_pAlertData = m_pAlertSession->GetAlertByUUID(sUUID);
}


CAlert::~CAlert()
{	
	m_pAlertData = nullptr;
	m_pAlertSession = nullptr;
}


std::string CAlert::GetUUID()
{
	std::lock_guard<std::mutex> lockGuard(m_AlertMutex);
	return m_pAlertData->GetUUID ();
}

bool CAlert::IsActive()
{
	std::lock_guard<std::mutex> lockGuard(m_AlertMutex);
	return m_pAlertData->IsActive();
}


LibMCEnv::eAlertLevel CAlert::GetAlertLevel()
{
	std::lock_guard<std::mutex> lockGuard(m_AlertMutex);
	auto alertDataLevel = m_pAlertData->GetLevel();
	
	switch (alertDataLevel) {
	case LibMCData::eAlertLevel::FatalError: return LibMCEnv::eAlertLevel::FatalError;
	case LibMCData::eAlertLevel::CriticalError: return LibMCEnv::eAlertLevel::CriticalError;
	case LibMCData::eAlertLevel::Message: return LibMCEnv::eAlertLevel::Message;
	case LibMCData::eAlertLevel::Warning: return LibMCEnv::eAlertLevel::Warning;
	default:
		return LibMCEnv::eAlertLevel::Unknown;
	}
}

std::string CAlert::GetIdentifier()
{
	std::lock_guard<std::mutex> lockGuard(m_AlertMutex);
	return m_pAlertData->GetIdentifier();
}

std::string CAlert::GetReadableContextInformation()
{
	std::lock_guard<std::mutex> lockGuard(m_AlertMutex);
	return m_pAlertData->GetReadableContextInformation();
}

bool CAlert::NeedsAcknowledgement()
{
	std::lock_guard<std::mutex> lockGuard(m_AlertMutex);
	return m_pAlertData->GetNeedsAcknowledgement();
}

bool CAlert::HasBeenAcknowledged()
{
	std::lock_guard<std::mutex> lockGuard(m_AlertMutex);
	return m_pAlertData->HasBeenAcknowledged();
}

void CAlert::GetAcknowledgementInformation(std::string & sUserUUID, std::string & sUserComment, std::string & sAckTime)
{
	std::lock_guard<std::mutex> lockGuard(m_AlertMutex);
	m_pAlertData->GetAcknowledgementInformation(sUserUUID, sUserComment, sAckTime);
}

IDateTime* CAlert::GetAcknowledgementTime()
{
	std::string sUserUUID;
	std::string sUserComment;
	std::string sAckTime;

	std::lock_guard<std::mutex> lockGuard(m_AlertMutex);
	m_pAlertData->GetAcknowledgementInformation(sUserUUID, sUserComment, sAckTime);

	return CDateTime::makefromUTC (sAckTime);
}


CAlert* CAlert::makeFrom(CAlert* pAlert)
{
	if (pAlert == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

	return new CAlert(pAlert->getDataModel(), pAlert->GetUUID (), pAlert->getCurrentUserUUID (), pAlert->getLogger (), pAlert->getLogInstance (), pAlert->m_pGlobalChrono);
}

std::shared_ptr<CAlert> CAlert::makeSharedFrom(CAlert* pAlert)
{
	return std::shared_ptr<CAlert>(makeFrom(pAlert));
}

void CAlert::AcknowledgeForUser(const std::string& sUserUUID, const std::string& sUserComment)
{
	std::lock_guard<std::mutex> lockGuard(m_AlertMutex);
	std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sUserUUID);

	m_pAlertData->AcknowledgeForUser(sNormalizedUUID, sUserComment, m_pGlobalChrono->getUTCTimeInISO8601 ());

	std::string sLogString = "Acknowledged alert " + m_pAlertData->GetUUID () + " (" + m_pAlertData->GetIdentifier () + ") for user " + sNormalizedUUID + " with comment: " + sUserComment;
	m_pLogger->logMessage(sLogString, m_sLogInstance, AMC::eLogLevel::Message);
}

void CAlert::AcknowledgeAlertForCurrentUser(const std::string& sUserComment)
{
	std::lock_guard<std::mutex> lockGuard(m_AlertMutex);
	if (!m_bUserContextExists)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_ALERTHASNOUSERCONTEXT, "alert has no user context: " + m_pAlertData->GetUUID ());

	m_pAlertData->AcknowledgeForUser(m_sCurrentUserUUID, sUserComment, m_pGlobalChrono->getUTCTimeInISO8601 ());

	std::string sLogString = "Acknowledged alert " + m_pAlertData->GetUUID() + " (" + m_pAlertData->GetIdentifier() + ") for current user " + m_sCurrentUserUUID + " with comment: " + sUserComment;
	m_pLogger->logMessage(sLogString, m_sLogInstance, AMC::eLogLevel::Message);
}

void CAlert::DeactivateAlert()
{
	std::lock_guard<std::mutex> lockGuard(m_AlertMutex);

	m_pAlertData->DeactivateAlert();

	std::string sLogString = "Deactivated alert " + m_pAlertData->GetUUID() + " (" + m_pAlertData->GetIdentifier() + ")";
	m_pLogger->logMessage(sLogString, m_sLogInstance, AMC::eLogLevel::Message);
}

LibMCData::PDataModel CAlert::getDataModel()
{
	return m_pDataModel;
}

std::string CAlert::getCurrentUserUUID()
{
	return m_sCurrentUserUUID;
}

AMC::PLogger CAlert::getLogger()
{
	return m_pLogger;
}

std::string CAlert::getLogInstance()
{
	return m_sLogInstance;
}
