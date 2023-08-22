/*++

Copyright (C) 2023 Autodesk Inc.

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


Abstract: This is a stub class definition of CSMCConfiguration

*/

#include "libmcdriver_scanlabsmc_smcconfiguration.hpp"
#include "libmcdriver_scanlabsmc_interfaceexception.hpp"

// Include custom headers here.


using namespace LibMCDriver_ScanLabSMC::Impl;

/*************************************************************************************************************************
 Class definition of CSMCConfiguration 
**************************************************************************************************************************/

CSMCConfiguration::CSMCConfiguration(LibMCEnv::PDriverEnvironment pDriverEnvironment)
    : m_pDriverEnvironment (pDriverEnvironment),
      m_DynamicViolationReaction (LibMCDriver_ScanLabSMC::eDynamicViolationReaction::WarningOnly),
     m_WarnLevel (LibMCDriver_ScanLabSMC::eWarnLevel::Error)

{
    if (pDriverEnvironment.get() == nullptr)
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPARAM);
}

CSMCConfiguration::~CSMCConfiguration()
{

}

void CSMCConfiguration::SetDynamicViolationReaction(const LibMCDriver_ScanLabSMC::eDynamicViolationReaction eValue)
{
    m_DynamicViolationReaction = eValue;
}

LibMCDriver_ScanLabSMC::eDynamicViolationReaction CSMCConfiguration::GetDynamicViolationReaction()
{
    return m_DynamicViolationReaction;
}

void CSMCConfiguration::SetWarnLevel(const LibMCDriver_ScanLabSMC::eWarnLevel eValue)
{
    m_WarnLevel = eValue;
}

LibMCDriver_ScanLabSMC::eWarnLevel CSMCConfiguration::GetWarnLevel()
{
    return m_WarnLevel;
}

std::string CSMCConfiguration::buildConfigurationXML(LibMCEnv::CWorkingDirectory* pWorkingDirectory)
{
    auto pXMLDocument = m_pDriverEnvironment->CreateXMLDocument("Configuration", "SCANmotionControl");
    auto pConfigurationNode = pXMLDocument->GetRootNode();
    pConfigurationNode->AddAttribute("", "Version", "0.6");

    auto pGeneralConfigNode = pConfigurationNode->AddChild("", "GeneralConfig");
    
    return pXMLDocument->SaveToString (true);
}