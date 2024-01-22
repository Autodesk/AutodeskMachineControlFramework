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

// Include custom headers here.
#include "common_utils.hpp"

using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CAlert 
**************************************************************************************************************************/

CAlert::CAlert(const std::string& sAlertUUID, LibMCData::PDataModel pDataModel)
	: m_sAlertUUID (AMCCommon::CUtils::normalizeUUIDString (sAlertUUID)),	
	m_bNeedsAcknowledgement (false),
	m_AlertLevel (LibMCEnv::eAlertLevel::CriticalError)
{
	if (pDataModel.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

	m_pAlertSession = pDataModel->CreateAlertSession();

}

CAlert::~CAlert()
{	
}


std::string CAlert::GetUUID()
{
	return m_sAlertUUID;
}

LibMCEnv::eAlertLevel CAlert::GetAlertLevel()
{
	return m_AlertLevel;
}

std::string CAlert::GetIdentifier()
{
	return m_sIdentifier;
}

std::string CAlert::GetReadableContextInformation()
{
	return m_sReadableContextInformation;
}

bool CAlert::NeedsAcknowledgement()
{
	return m_bNeedsAcknowledgement;
}

bool CAlert::IsAcknowledged()
{
	std::lock_guard<std::mutex> lockGuard(m_AlertSessionMutex);
	return m_pAlertSession->AlertHasBeenAcknowledged(m_sAlertUUID);
}

void CAlert::GetAcknowledgementInformation(std::string & sUserUUID, std::string & sUserComment, std::string & sAckTime)
{
	std::lock_guard<std::mutex> lockGuard(m_AlertSessionMutex);
	m_pAlertSession->GetAcknowledgementInformation(m_sAlertUUID, sUserUUID, sUserComment, sAckTime);
}

