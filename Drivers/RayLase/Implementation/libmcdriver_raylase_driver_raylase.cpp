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


Abstract: This is a stub class definition of CDriver_Raylase

*/

#include "libmcdriver_raylase_driver_raylase.hpp"
#include "libmcdriver_raylase_interfaceexception.hpp"

// Include custom headers here.

#define __STRINGIZE(x) #x
#define __STRINGIZE_VALUE_OF(x) __STRINGIZE(x)

using namespace LibMCDriver_Raylase::Impl;

/*************************************************************************************************************************
 Class definition of CDriver_Raylase 
**************************************************************************************************************************/
CDriver_Raylase::CDriver_Raylase(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment)
    : m_pDriverEnvironment (pDriverEnvironment), m_sName (sName), m_bSimulationMode (false)
{

}

CDriver_Raylase::~CDriver_Raylase()
{
    m_pRayLaseSDK = nullptr;
    m_pSDKClientLib = nullptr;
    m_pSDKWorkingDirectory = nullptr;

}

void CDriver_Raylase::Configure(const std::string& sConfigurationString)
{
}

std::string CDriver_Raylase::GetName()
{
    return m_sName;
}

std::string CDriver_Raylase::GetType()
{
    return "raylase";
}

void CDriver_Raylase::GetVersion(LibMCDriver_Raylase_uint32& nMajor, LibMCDriver_Raylase_uint32& nMinor, LibMCDriver_Raylase_uint32& nMicro, std::string& sBuild)
{
    nMajor = LIBMCDRIVER_RAYLASE_VERSION_MAJOR;
    nMinor = LIBMCDRIVER_RAYLASE_VERSION_MINOR;
    nMicro = LIBMCDRIVER_RAYLASE_VERSION_MICRO;
    sBuild = __STRINGIZE_VALUE_OF(__GITHASH);;
}

void CDriver_Raylase::GetHeaderInformation(std::string& sNameSpace, std::string& sBaseName)
{
    sNameSpace = "LibMCDriver_Raylase";
    sBaseName = "libmcdriver_raylase";
}

void CDriver_Raylase::QueryParameters()
{
}



void CDriver_Raylase::LoadSDK()
{
    m_pRayLaseSDK = nullptr;
    m_pSDKClientLib = nullptr;
    m_pSDKWorkingDirectory = nullptr;

    m_pSDKWorkingDirectory = m_pDriverEnvironment->CreateWorkingDirectory();
    std::string sDLLFileName = "RAYLASE.SPICE3.Native.ClientLib.dll";

#ifdef _WIN32
    m_pSDKClientLib = m_pSDKWorkingDirectory->StoreDriverData(sDLLFileName, "raylase_clientlib_win_x64");
#else
    m_pSDKClientLib = m_pSDKWorkingDirectory->StoreDriverData("sDLLFileName", "raylase_clientlib_linux_x64");
#endif

    m_pRayLaseSDK = std::make_shared<CRaylaseSDK>(m_pSDKClientLib->GetAbsoluteFileName());

}

IRaylaseCard* CDriver_Raylase::ConnectByIP(const std::string& sCardName, const std::string& sCardIP, const LibMCDriver_Raylase_uint32 nPort, const LibMCDriver_Raylase_double dMaxLaserPowerInWatts)
{
    if (sCardName.empty())
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDCARDNAME);
    if (sCardIP.empty())
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDCARDIP);
    if ((nPort < 32) || (nPort > 65535))
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDCARDPORT);

    auto iIter = m_CardInstances.find(sCardName);
    if (iIter != m_CardInstances.end ())
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_CARDALREADYREGISTERED, "card already registered: " + sCardName);

    auto pCard = CRaylaseCardImpl::connectByIP(m_pRayLaseSDK, sCardName, sCardIP, nPort, dMaxLaserPowerInWatts, m_bSimulationMode, m_pDriverEnvironment);
    m_CardInstances.insert(std::make_pair (sCardName, pCard));

    return new CRaylaseCard (pCard);
}

IRaylaseCard* CDriver_Raylase::GetConnectedCard(const std::string& sCardName)
{
    if (sCardName.empty())
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDCARDNAME);

    auto iIter = m_CardInstances.find(sCardName);
    if (iIter == m_CardInstances.end())
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_CARDNOTREGISTERED, "card not registered: " + sCardName);

    return new CRaylaseCard(iIter->second);

}


void CDriver_Raylase::SetToSimulationMode()
{
    m_bSimulationMode = true;
}

bool CDriver_Raylase::IsSimulationMode()
{
    return m_bSimulationMode;
}
