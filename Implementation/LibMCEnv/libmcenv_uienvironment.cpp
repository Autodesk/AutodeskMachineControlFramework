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
#include "amc_statemachinedata.hpp"

// Include custom headers here.
#include "common_utils.hpp"

using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CUIEnvironment 
**************************************************************************************************************************/

CUIEnvironment::CUIEnvironment(AMC::PLogger pLogger, AMC::PStateMachineData pStateMachineData, AMC::PStateSignalHandler pSignalHandler, const std::string& sSenderUUID, const std::string& sSenderName, AMC::PParameterHandler pClientVariableHandler)
    : 
      m_pLogger(pLogger),
      m_pStateMachineData(pStateMachineData),
      m_pSignalHandler (pSignalHandler),
      m_sLogSubSystem ("ui"),
      m_sSenderName (sSenderName),
      m_pClientVariableHandler (pClientVariableHandler),
      m_bCloseModalDialog (false)
{

    if (pLogger.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
    if (pStateMachineData.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
    if (pSignalHandler.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
    if (pClientVariableHandler.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

    if (!sSenderUUID.empty()) {
        m_sSenderUUID = (AMCCommon::CUtils::normalizeUUIDString(sSenderUUID));
    }
    else {
        m_sSenderUUID = AMCCommon::CUtils::createEmptyUUID();
    }

}

void CUIEnvironment::ActivateModalDialog(const std::string& sDialogName)
{
    m_sModalDialogToShow = sDialogName;
    m_bCloseModalDialog = false;
}

void CUIEnvironment::CloseModalDialog()
{
    m_sModalDialogToShow = "";
    m_bCloseModalDialog = true;
}

void CUIEnvironment::ActivatePage(const std::string& sPageName)
{
    m_sPageToActivate = sPageName;
}

std::string CUIEnvironment::RetrieveEventSender()
{
    return m_sSenderName;
}


ISignalTrigger * CUIEnvironment::PrepareSignal(const std::string & sMachineInstance, const std::string & sSignalName)
{
    if (!m_pSignalHandler->hasSignalDefinition(sMachineInstance, sSignalName))
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNOTFINDSIGNALDEFINITON);

    return new CSignalTrigger(m_pSignalHandler, sMachineInstance, sSignalName);
}

std::string CUIEnvironment::GetMachineState(const std::string & sMachineInstance)
{
    return m_pStateMachineData->getInstanceStateName (sMachineInstance);
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



std::string CUIEnvironment::GetMachineParameter(const std::string& sMachineInstance, const std::string& sParameterGroup, const std::string& sParameterName)
{
    auto pParameterHandler = m_pStateMachineData->getParameterHandler(sMachineInstance);
    auto pGroup = pParameterHandler->findGroup(sParameterGroup, true);
    return pGroup->getParameterValueByName(sParameterName);
}

std::string CUIEnvironment::GetMachineParameterAsUUID(const std::string& sMachineInstance, const std::string& sParameterGroup, const std::string& sParameterName)
{
    auto pParameterHandler = m_pStateMachineData->getParameterHandler(sMachineInstance);
    auto pGroup = pParameterHandler->findGroup(sParameterGroup, true);
    return pGroup->getParameterValueByName(sParameterName);
}

LibMCEnv_double CUIEnvironment::GetMachineParameterAsDouble(const std::string& sMachineInstance, const std::string& sParameterGroup, const std::string& sParameterName)
{
    auto pParameterHandler = m_pStateMachineData->getParameterHandler(sMachineInstance);
    auto pGroup = pParameterHandler->findGroup(sParameterGroup, true);
    return pGroup->getDoubleParameterValueByName(sParameterName);
}

LibMCEnv_int64 CUIEnvironment::GetMachineParameterAsInteger(const std::string& sMachineInstance, const std::string& sParameterGroup, const std::string& sParameterName)
{
    auto pParameterHandler = m_pStateMachineData->getParameterHandler(sMachineInstance);
    auto pGroup = pParameterHandler->findGroup(sParameterGroup, true);
    return pGroup->getIntParameterValueByName(sParameterName);
}

bool CUIEnvironment::GetMachineParameterAsBool(const std::string& sMachineInstance, const std::string& sParameterGroup, const std::string& sParameterName)
{
    auto pParameterHandler = m_pStateMachineData->getParameterHandler(sMachineInstance);
    auto pGroup = pParameterHandler->findGroup(sParameterGroup, true);
    return pGroup->getBoolParameterValueByName(sParameterName);
}


std::string CUIEnvironment::GetUIProperty(const std::string& sElementPath, const std::string& sPropertyName) 
{   
    if (m_pClientVariableHandler.get() == nullptr)
        throw ELibMCEnvInterfaceException (LIBMCENV_ERROR_COULDNNOTACCESSCLIENTVARIABLES);

    auto pGroup = m_pClientVariableHandler->findGroup(sElementPath, true);
    return pGroup->getParameterValueByName(sPropertyName);
}

std::string CUIEnvironment::GetUIPropertyAsUUID(const std::string& sElementPath, const std::string& sPropertyName)
{
    if (m_pClientVariableHandler.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNNOTACCESSCLIENTVARIABLES);

    auto pGroup = m_pClientVariableHandler->findGroup(sElementPath, true);
    return pGroup->getParameterValueByName(sPropertyName);

}

LibMCEnv_double CUIEnvironment::GetUIPropertyAsDouble(const std::string& sElementPath, const std::string& sPropertyName)
{
    if (m_pClientVariableHandler.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNNOTACCESSCLIENTVARIABLES);

    auto pGroup = m_pClientVariableHandler->findGroup(sElementPath, true);
    return pGroup->getDoubleParameterValueByName(sPropertyName);
}

LibMCEnv_int64 CUIEnvironment::GetUIPropertyAsInteger(const std::string& sElementPath, const std::string& sPropertyName)
{
    if (m_pClientVariableHandler.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNNOTACCESSCLIENTVARIABLES);

    auto pGroup = m_pClientVariableHandler->findGroup(sElementPath, true);
    return pGroup->getIntParameterValueByName(sPropertyName);
}

bool CUIEnvironment::GetUIPropertyAsBool(const std::string& sElementPath, const std::string& sPropertyName)
{
    if (m_pClientVariableHandler.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNNOTACCESSCLIENTVARIABLES);

    auto pGroup = m_pClientVariableHandler->findGroup(sElementPath, true);
    return pGroup->getBoolParameterValueByName(sPropertyName);
}

void CUIEnvironment::SetUIProperty(const std::string& sElementPath, const std::string& sPropertyName, const std::string& sValue)
{
    if (m_pClientVariableHandler.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNNOTACCESSCLIENTVARIABLES);

    auto pGroup = m_pClientVariableHandler->findGroup(sElementPath, true);
    pGroup->setParameterValueByName(sPropertyName, sValue);
}

void CUIEnvironment::SetUIPropertyAsUUID(const std::string& sElementPath, const std::string& sPropertyName, const std::string& sValue)
{
    if (m_pClientVariableHandler.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNNOTACCESSCLIENTVARIABLES);


    auto pGroup = m_pClientVariableHandler->findGroup(sElementPath, true);
    if (!sValue.empty())
        pGroup->setParameterValueByName(sPropertyName, AMCCommon::CUtils::normalizeUUIDString (sValue));
    else
        pGroup->setParameterValueByName(sPropertyName, AMCCommon::CUtils::createEmptyUUID());
}

void CUIEnvironment::SetUIPropertyAsDouble(const std::string& sElementPath, const std::string& sPropertyName, const LibMCEnv_double dValue) 
{
    if (m_pClientVariableHandler.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNNOTACCESSCLIENTVARIABLES);

    auto pGroup = m_pClientVariableHandler->findGroup(sElementPath, true);
    pGroup->setDoubleParameterValueByName(sPropertyName, dValue);

}

void CUIEnvironment::SetUIPropertyAsInteger(const std::string& sElementPath, const std::string& sPropertyName, const LibMCEnv_int64 nValue) 
{
    if (m_pClientVariableHandler.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNNOTACCESSCLIENTVARIABLES);

    auto pGroup = m_pClientVariableHandler->findGroup(sElementPath, true);
    pGroup->setIntParameterValueByName(sPropertyName, nValue);
}

void CUIEnvironment::SetUIPropertyAsBool(const std::string& sElementPath, const std::string& sPropertyName, const bool bValue)
{
    if (m_pClientVariableHandler.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNNOTACCESSCLIENTVARIABLES);

    auto pGroup = m_pClientVariableHandler->findGroup(sElementPath, true);
    pGroup->setBoolParameterValueByName(sPropertyName, bValue);
}



std::string CUIEnvironment::getModalDialogToShow()
{
    return m_sModalDialogToShow;
}

bool CUIEnvironment::getCloseModalDialog()
{
    return m_bCloseModalDialog;
}

std::string CUIEnvironment::getPageToActivate()
{
    return m_sPageToActivate;
}