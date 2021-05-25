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


Abstract: This is a stub class definition of CDriver_Ximc

*/

#include "libmcdriver_ximc_driver_ximc.hpp"
#include "libmcdriver_ximc_interfaceexception.hpp"

// Include custom headers here.


using namespace LibMCDriver_Ximc::Impl;

/*************************************************************************************************************************
 Class definition of CDriver_Ximc 
**************************************************************************************************************************/

CDriver_Ximc::CDriver_Ximc(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment)
    : m_sName (sName), m_pDriverEnvironment (pDriverEnvironment)
{
    pDriverEnvironment->RegisterStringParameter("dllversion", "Version of the XIMC DLL", "unknown");
}


void CDriver_Ximc::Configure(const std::string& sConfigurationString)
{
    m_pWorkingDirectory = m_pDriverEnvironment->CreateWorkingDirectory();
    m_pLibXimcDLL = m_pWorkingDirectory->StoreDriverData("libximc.dll", "libximc");
    m_pBindyDLL = m_pWorkingDirectory->StoreDriverData("bindy.dll", "bindy");
    m_pXiWrapperDLL = m_pWorkingDirectory->StoreDriverData("xiwrapper.dll", "xiwrapper");

    SetDllDirectory(m_pWorkingDirectory->GetAbsoluteFilePath().c_str());
    m_pXimcSDK = std::make_shared<CXimcSDK>(m_pLibXimcDLL->GetAbsoluteFileName());

    char ximc_version_str[33];
    m_pXimcSDK->ximc_version(ximc_version_str);
    ximc_version_str[32] = 0;
    std::string sVersionString = ximc_version_str;

    m_pDriverEnvironment->SetStringParameter("dllversion", sVersionString);
    findDevices();
}

std::string CDriver_Ximc::GetName()
{
    return m_sName;
}

std::string CDriver_Ximc::GetType()
{
    return "ximc";
}

void CDriver_Ximc::GetVersion(LibMCDriver_Ximc_uint32& nMajor, LibMCDriver_Ximc_uint32& nMinor, LibMCDriver_Ximc_uint32& nMicro, std::string& sBuild)
{
    nMajor = 1;
    nMinor = 0;
    nMicro = 0;
}

void CDriver_Ximc::GetHeaderInformation(std::string& sNameSpace, std::string& sBaseName)
{
    sNameSpace = "LibMCDriver_Ximc";
    sBaseName = "libmcdriver_ximc";
}

void CDriver_Ximc::QueryParameters()
{
    
}

void CDriver_Ximc::findDevices()
{
    m_FoundDevices.clear();
    if (m_pXimcSDK.get() == nullptr)
        return;

    ximc_device_enumeration_t deviceEnumeration = m_pXimcSDK->enumerate_devices(XIMC_ENUMERATEFLAG_PROBE, "");
    if (!deviceEnumeration)
        throw std::runtime_error("could not enumerate devices");

    int deviceCount = m_pXimcSDK->get_device_count(deviceEnumeration);
    if (deviceCount < 0) {
        m_pXimcSDK->free_enumerate_devices(deviceEnumeration);
        throw std::runtime_error("could not get number of devices");
    }

    for (int deviceIndex = 0; deviceIndex < deviceCount; deviceIndex++) {
        char* pName = m_pXimcSDK->get_device_name(deviceEnumeration, deviceIndex);
        if (pName != "") {
            m_FoundDevices.push_back(pName);
        }
    }

    m_pXimcSDK->free_enumerate_devices(deviceEnumeration);

}

void CDriver_Ximc::Initialize(const std::string& sDeviceName)
{


}


LibMCDriver_Ximc_double CDriver_Ximc::GetCurrentPosition()
{
    return 5.0;
}


LibMCDriver_Ximc_uint32 CDriver_Ximc::GetDetectedDeviceCount()
{
    return (uint32_t) m_FoundDevices.size();
}

std::string CDriver_Ximc::GetDetectedDeviceName(const LibMCDriver_Ximc_uint32 nDeviceIndex)
{
    if (nDeviceIndex < m_FoundDevices.size())
        return m_FoundDevices[nDeviceIndex];
    else
        return "";
}
