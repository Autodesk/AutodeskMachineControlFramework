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

void CDriver_TML::SetCustomSDKResource(const std::string& sLibResourceName, const std::string& sCommsResourceName)
{
	
    if (sdkIsLoaded())
        throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_SDKALREADYLOADED);

    m_SDKLibDLLBuffer.resize(0);
    m_SDKCommsDLLBuffer.resize(0);

    m_pDriverEnvironment->RetrieveMachineResourceData(sLibResourceName, m_SDKLibDLLBuffer);
    m_pDriverEnvironment->RetrieveMachineResourceData(sCommsResourceName, m_SDKCommsDLLBuffer);

}

bool CDriver_TML::sdkIsLoaded()
{
    return m_pTMLSDK.get() != nullptr;
}

void CDriver_TML::ensureSDKIsLoaded()
{
    if (m_pTMLSDK.get() == nullptr) {

        m_pWorkingDirectory = m_pDriverEnvironment->CreateWorkingDirectory();

        if (m_SDKLibDLLBuffer.empty())
            throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_NOSDKLIBRESOURCEDLLGIVEN);
        if (m_SDKCommsDLLBuffer.empty())
            throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_NOSDKLIBRESOURCECOMMSGIVEN);

#ifdef _WIN32        

        m_pTMLLibDLLLibrary = m_pWorkingDirectory->StoreCustomData("TML_lib.dll", m_SDKLibDLLBuffer);

        m_pTMLCommsDLLLibrary = m_pWorkingDirectory->StoreCustomData("tmlcomms.dll", m_SDKCommsDLLBuffer);

#else

        m_pTMLLibDLLLibrary = m_pWorkingDirectory->StoreCustomData("tml_lib.so", m_SDKLibDLLBuffer);

        m_pTMLCommsDLLLibrary = m_pWorkingDirectory->StoreCustomData("tmlcomms.so", m_SDKCommsDLLBuffer);

#endif

        m_pTMLSDK = std::make_shared<CTMLSDK> (m_pTMLLibDLLLibrary->GetAbsoluteFileName());

        m_pTMLInstance = std::make_shared<CTMLInstance>(m_pTMLSDK, m_pWorkingDirectory);

    }

}



IChannel * CDriver_TML::OpenChannel(const std::string & sIdentifier, const std::string & sDeviceName, const LibMCDriver_TML::eChannelType eChannelTypeToUse, const LibMCDriver_TML::eProtocolType eProtocolTypeToUse, const LibMCDriver_TML_uint32 nHostID, const LibMCDriver_TML_uint32 nBaudrate)
{
    ensureSDKIsLoaded();

    m_pTMLInstance->openChannel (sIdentifier, sDeviceName, eChannelTypeToUse, eProtocolTypeToUse, nHostID, nBaudrate);

    return new CChannel(m_pTMLInstance, sIdentifier);

}

IChannel * CDriver_TML::FindChannel(const std::string & sIdentifier)
{
    ensureSDKIsLoaded();

    if (!m_pTMLInstance->checkIdentifierString (sIdentifier))
        throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_INVALIDCHANNELIDENTIFIER, "invalid channel identifier: " + sIdentifier);

    if (!m_pTMLInstance->channelExists (sIdentifier))
        throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_CHANNELDOESNOTEXIST);

    return new CChannel(m_pTMLInstance, sIdentifier);
}

bool CDriver_TML::ChannelExists(const std::string& sIdentifier)
{
    ensureSDKIsLoaded();

    return m_pTMLInstance->channelExists(sIdentifier);

}
