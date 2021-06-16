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


Abstract: This is a stub class definition of CUIEnvironment

*/

#include "libmcenv_uienvironment.hpp"
#include "libmcenv_interfaceexception.hpp"
#include "amc_systemstate.hpp"
#include "libmcenv_signaltrigger.hpp"
#include "amc_logger.hpp"
#include "amc_parameterinstances.hpp"

// Include custom headers here.
#include "common_utils.hpp"

using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CUIEnvironment 
**************************************************************************************************************************/

CUIEnvironment::CUIEnvironment(AMC::PLogger pLogger, AMC::PParameterInstances pParameterInstances, AMC::PStateSignalHandler pSignalHandler, const std::string& sSenderUUID, const std::string& sContextUUID)
    : 
      m_pLogger(pLogger),
      m_pParameterInstances (pParameterInstances), 
      m_pSignalHandler (pSignalHandler),
      m_sLogSubSystem ("ui"),
      m_sContextUUID (AMCCommon::CUtils::normalizeUUIDString (sContextUUID)),
      m_sSenderUUID (AMCCommon::CUtils::normalizeUUIDString(sSenderUUID))
{
    if (pLogger.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
    if (pParameterInstances.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
    if (pSignalHandler.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

}

ISignalTrigger * CUIEnvironment::PrepareSignal(const std::string & sMachineInstance, const std::string & sSignalName)
{
    if (!m_pSignalHandler->hasSignalDefinition(sMachineInstance, sSignalName))
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNOTFINDSIGNALDEFINITON);

    return new CSignalTrigger(m_pSignalHandler, sMachineInstance, sSignalName);
}

std::string CUIEnvironment::GetMachineState(const std::string & sMachineInstance)
{
    auto pParameterHandler = m_pParameterInstances->getParameterHandler(sMachineInstance);
    return pParameterHandler->getInstanceStateName ();
}

void CUIEnvironment::LogMessage(const std::string & sLogString)
{	
    m_pLogger->logMessage(sLogString, m_sLogSubSystem, AMC::eLogLevel::Message);
}

void CUIEnvironment::LogWarning(const std::string & sLogString)
{	
    m_pLogger->logMessage(sLogString, m_sLogSubSystem, AMC::eLogLevel::Warning);
}

void CUIEnvironment::LogInfo(const std::string & sLogString)
{	
    m_pLogger->logMessage(sLogString, m_sLogSubSystem, AMC::eLogLevel::Info);
}

std::string CUIEnvironment::GetStringParameter(const std::string & sMachineInstance, const std::string & sParameterGroup, const std::string & sParameterName)
{
    auto pParameterHandler = m_pParameterInstances->getParameterHandler(sMachineInstance);
    auto pGroup = pParameterHandler->findGroup(sParameterGroup, true);
    return pGroup->getParameterValueByName(sParameterName);
}

std::string CUIEnvironment::GetUUIDParameter(const std::string & sMachineInstance, const std::string & sParameterGroup, const std::string & sParameterName)
{
    auto pParameterHandler = m_pParameterInstances->getParameterHandler(sMachineInstance);
    auto pGroup = pParameterHandler->findGroup(sParameterGroup, true);
    return pGroup->getParameterValueByName(sParameterName);
}

LibMCEnv_double CUIEnvironment::GetDoubleParameter(const std::string & sMachineInstance, const std::string & sParameterGroup, const std::string & sParameterName)
{
    auto pParameterHandler = m_pParameterInstances->getParameterHandler(sMachineInstance);
    auto pGroup = pParameterHandler->findGroup(sParameterGroup, true);
    return pGroup->getDoubleParameterValueByName(sParameterName);
}

LibMCEnv_int64 CUIEnvironment::GetIntegerParameter(const std::string & sMachineInstance, const std::string & sParameterGroup, const std::string & sParameterName)
{
    auto pParameterHandler = m_pParameterInstances->getParameterHandler(sMachineInstance);
    auto pGroup = pParameterHandler->findGroup(sParameterGroup, true);
    return pGroup->getIntParameterValueByName(sParameterName);
}

bool CUIEnvironment::GetBoolParameter(const std::string & sMachineInstance, const std::string & sParameterGroup, const std::string & sParameterName)
{
    auto pParameterHandler = m_pParameterInstances->getParameterHandler(sMachineInstance);
    auto pGroup = pParameterHandler->findGroup(sParameterGroup, true);
    return pGroup->getBoolParameterValueByName(sParameterName);
}

void CUIEnvironment::SetStringParameter(const std::string& sMachineInstance, const std::string& sParameterGroup, const std::string& sParameterName, const std::string& sValue)
{
    auto pParameterHandler = m_pParameterInstances->getParameterHandler(sMachineInstance);
    auto pGroup = pParameterHandler->findGroup(sParameterGroup, true);
    pGroup->setParameterValueByName(sParameterName, sValue);

}

void CUIEnvironment::SetUUIDParameter(const std::string& sMachineInstance, const std::string& sParameterGroup, const std::string& sParameterName, const std::string& sValue) 
{

    auto pParameterHandler = m_pParameterInstances->getParameterHandler(sMachineInstance);
    auto pGroup = pParameterHandler->findGroup(sParameterGroup, true);
    pGroup->setParameterValueByName(sParameterName, AMCCommon::CUtils::normalizeUUIDString (sValue));
}
    

void CUIEnvironment::SetDoubleParameter(const std::string& sMachineInstance, const std::string& sParameterGroup, const std::string& sParameterName, const LibMCEnv_double dValue) 
{
    auto pParameterHandler = m_pParameterInstances->getParameterHandler(sMachineInstance);
    auto pGroup = pParameterHandler->findGroup(sParameterGroup, true);
    pGroup->setDoubleParameterValueByName(sParameterName, dValue);

}


void CUIEnvironment::SetIntegerParameter(const std::string& sMachineInstance, const std::string& sParameterGroup, const std::string& sParameterName, const LibMCEnv_int64 nValue) 
{
    auto pParameterHandler = m_pParameterInstances->getParameterHandler(sMachineInstance);
    auto pGroup = pParameterHandler->findGroup(sParameterGroup, true);
    pGroup->setIntParameterValueByName(sParameterName, nValue);
}


void CUIEnvironment::SetBoolParameter(const std::string& sMachineInstance, const std::string& sParameterGroup, const std::string& sParameterName, const bool bValue) 
{
    auto pParameterHandler = m_pParameterInstances->getParameterHandler(sMachineInstance);
    auto pGroup = pParameterHandler->findGroup(sParameterGroup, true);
    pGroup->setBoolParameterValueByName(sParameterName, bValue);
}



bool CUIEnvironment::HasFormValue(const std::string& sFormIdentifier, const std::string& sValueIdentifier)
{
    auto iIter = m_FormValues.find(std::make_pair(sFormIdentifier, sValueIdentifier));
    return (iIter != m_FormValues.end());
}

std::string CUIEnvironment::GetFormStringValue(const std::string& sFormIdentifier, const std::string& sValueIdentifier)
{
    auto iIter = m_FormValues.find(std::make_pair (sFormIdentifier, sValueIdentifier));

    if (iIter == m_FormValues.end())
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_FORMVALUENOTFOUND, "form value not found: " + sValueIdentifier);

    return iIter->second;
}

std::string CUIEnvironment::GetFormUUIDValue(const std::string& sFormIdentifier, const std::string& sValueIdentifier)
{
    return AMCCommon::CUtils::normalizeUUIDString(GetFormStringValue(sFormIdentifier, sValueIdentifier));
}

LibMCEnv_double CUIEnvironment::GetFormDoubleValue(const std::string& sFormIdentifier, const std::string& sValueIdentifier)
{
    auto sValue = GetFormStringValue(sFormIdentifier, sValueIdentifier);

    try {
        return std::stod(sValue);
    }
    catch (std::exception& e) {
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDDOUBLEFORMVALUE, e.what() + (" / " + sValueIdentifier));

    }
}

LibMCEnv_int64 CUIEnvironment::GetFormIntegerValue(const std::string& sFormIdentifier, const std::string& sValueIdentifier)
{
    auto sValue = GetFormStringValue(sFormIdentifier, sValueIdentifier);

    try {
        return std::stoll(sValue);
    }
    catch (std::exception& e) {
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDINTEGERFORMVALUE, e.what() + (" / " + sValueIdentifier));

    }

}

bool CUIEnvironment::GetFormBoolValue(const std::string& sFormIdentifier, const std::string& sValueIdentifier)
{
    return (GetFormIntegerValue(sFormIdentifier, sValueIdentifier) != 0);
}



std::string CUIEnvironment::GetEventContext()
{
    return m_sContextUUID;
}


void CUIEnvironment::addFormValue(const std::string& sFormIdentifier, const std::string& sValueIdentifier, const std::string& sValue)
{
    m_FormValues.insert(std::make_pair (std::make_pair (sFormIdentifier, sValueIdentifier), sValue));
}