/*++

Copyright (C) 2024 Autodesk Inc.

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


Abstract: This is a stub class definition of CDriver_TML

*/

#include "libmcdriver_tml_driver_tml.hpp"
#include "libmcdriver_tml_interfaceexception.hpp"

#include "libmcdriver_tml_channel.hpp"

// Include custom headers here.


using namespace LibMCDriver_TML::Impl;

/*************************************************************************************************************************
 Class definition of CDriver_TML 
**************************************************************************************************************************/

CDriver_TML::CDriver_TML(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment)
    : m_sName (sName), m_pDriverEnvironment (pDriverEnvironment), m_bSimulationMode (false)
{
    if (pDriverEnvironment.get() == nullptr)
        throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_INVALIDPARAM);

}

CDriver_TML::~CDriver_TML()
{

}

void CDriver_TML::SetToSimulationMode()
{
    m_bSimulationMode = true;
}

bool CDriver_TML::IsSimulationMode()
{
    return m_bSimulationMode;
}

void CDriver_TML::SetCustomSDKResource(const std::string & sResourceName)
{
	
    if (sdkIsLoaded())
        throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_SDKALREADYLOADED);

    m_pDriverEnvironment->RetrieveMachineResourceData(sResourceName, m_SDKDLLBuffer);

}

bool CDriver_TML::sdkIsLoaded()
{
    return m_pTMLSDK.get() != nullptr;
}

void CDriver_TML::ensureSDKIsLoaded()
{
    if (m_pTMLSDK.get() == nullptr) {

        m_pWorkingDirectory = m_pDriverEnvironment->CreateWorkingDirectory();

        m_pTMLDLLLibrary = m_pWorkingDirectory->StoreCustomData("tml_lib.dll", m_SDKDLLBuffer);

        m_pTMLSDK = std::make_shared<CTMLSDK> (m_pTMLDLLLibrary->GetAbsoluteFileName());

    }

}


void CDriver_TML::LoadSetup(const std::string & sSetupConfig, const std::string & sVariablesConfig)
{
    ensureSDKIsLoaded();
    m_pWorkingDirectory->StoreCustomString("setup.cfg", sSetupConfig);
    m_pWorkingDirectory->StoreCustomString("variables.cfg", sVariablesConfig);

    int32_t nSetupIndex = m_pTMLSDK->TS_LoadSetup(m_pWorkingDirectory->GetAbsoluteFilePath().c_str());

}

IChannel * CDriver_TML::OpenChannel(const std::string & sIdentifier, const std::string & sDeviceName, const LibMCDriver_TML::eChannelType eChannelTypeToUse, const LibMCDriver_TML::eProtocolType eProtocolTypeToUse, const LibMCDriver_TML_uint32 nHostID, const LibMCDriver_TML_uint32 nBaudrate)
{
    ensureSDKIsLoaded();

    return new CChannel();

}

IChannel * CDriver_TML::FindChannel(const std::string & sIdentifier)
{
    ensureSDKIsLoaded();

    return new CChannel();
}

