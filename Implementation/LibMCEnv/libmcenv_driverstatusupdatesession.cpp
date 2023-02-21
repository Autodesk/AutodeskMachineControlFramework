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


Abstract: This is a stub class definition of CDriverStatusUpdateSession

*/

#include "libmcenv_driverstatusupdatesession.hpp"
#include "libmcenv_interfaceexception.hpp"

// Include custom headers here.
#include "Common/common_utils.hpp"

using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CDriverStatusUpdateSession 
**************************************************************************************************************************/

CDriverStatusUpdateSession::CDriverStatusUpdateSession(AMC::PParameterGroup pParameterGroup, AMC::PLogger pLogger, std::string sDriverName)
	: m_pParameterGroup (pParameterGroup), m_pLogger (pLogger), m_sDriverName (sDriverName)
{
	if (pParameterGroup.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
	if (pLogger.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

}

CDriverStatusUpdateSession::~CDriverStatusUpdateSession()
{

}


void CDriverStatusUpdateSession::SetStringParameter(const std::string & sParameterName, const std::string & sValue)
{
	m_pParameterGroup->setParameterValueByName(sParameterName, sValue);
}

void CDriverStatusUpdateSession::SetUUIDParameter(const std::string & sParameterName, const std::string & sValue)
{
	m_pParameterGroup->setParameterValueByName(sParameterName, AMCCommon::CUtils::normalizeUUIDString(sValue));
}

void CDriverStatusUpdateSession::SetDoubleParameter(const std::string & sParameterName, const LibMCEnv_double dValue)
{
	m_pParameterGroup->setDoubleParameterValueByName(sParameterName, dValue);
}

void CDriverStatusUpdateSession::SetIntegerParameter(const std::string & sParameterName, const LibMCEnv_int64 nValue)
{
	m_pParameterGroup->setIntParameterValueByName(sParameterName, nValue);
}

void CDriverStatusUpdateSession::SetBoolParameter(const std::string & sParameterName, const bool bValue)
{
	m_pParameterGroup->setBoolParameterValueByName(sParameterName, bValue);
}

void CDriverStatusUpdateSession::LogMessage(const std::string & sLogString)
{
	m_pLogger->logMessage(sLogString, m_sDriverName, AMC::eLogLevel::Message);
}

void CDriverStatusUpdateSession::LogWarning(const std::string & sLogString)
{
	m_pLogger->logMessage(sLogString, m_sDriverName, AMC::eLogLevel::Warning);
}

void CDriverStatusUpdateSession::LogInfo(const std::string & sLogString)
{
	m_pLogger->logMessage(sLogString, m_sDriverName, AMC::eLogLevel::Info);
}

void CDriverStatusUpdateSession::Sleep(const LibMCEnv_uint32 nDelay)
{
	m_Chrono.sleepMilliseconds(nDelay);
}

