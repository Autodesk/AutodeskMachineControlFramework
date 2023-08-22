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


Abstract: This is a stub class definition of CDriver_ScanLabSMC

*/

#include "libmcdriver_scanlabsmc_driver_scanlabsmc.hpp"
#include "libmcdriver_scanlabsmc_interfaceexception.hpp"
#include "libmcdriver_scanlabsmc_smccontext.hpp"
#include "libmcdriver_scanlabsmc_smcconfiguration.hpp"

// Include custom headers here.
#define __STRINGIZE(x) #x
#define __STRINGIZE_VALUE_OF(x) __STRINGIZE(x)

#define SCANLABSMC_DEFAULT_SMCDLLRESOURCENAME "scanmotioncontrol_x64"
#define SCANLABSMC_DEFAULT_RTCDLLRESOURCENAME "rtcdllx64"
#define SCANLABSMC_DEFAULT_XERCESDLLRESOURCENAME "xerces-c_3_2"

using namespace LibMCDriver_ScanLabSMC::Impl;

/*************************************************************************************************************************
 Class definition of CDriver_ScanLabSMC 
**************************************************************************************************************************/

CDriver_ScanLabSMC::CDriver_ScanLabSMC(const std::string& sName, const std::string& sType, LibMCEnv::PDriverEnvironment pDriverEnvironment)
    : m_sName (sName), m_sType (sType), m_pDriverEnvironment (pDriverEnvironment)
{
    if (pDriverEnvironment.get() == nullptr)
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPARAM);

    m_pDLLDirectory = m_pDriverEnvironment->CreateWorkingDirectory();
}

CDriver_ScanLabSMC::~CDriver_ScanLabSMC()
{

}

void CDriver_ScanLabSMC::SetDLLResources(const std::string& sSMCDLLResourceName, const std::string& sRTCDLLResourceName)
{
    if (m_pSDK.get() != nullptr)
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_SDKALREADYLOADED);

    m_SMCDLLResourceData.resize(0);
    m_RTCDLLResourceData.resize(0);

    if (sSMCDLLResourceName.empty())
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_EMPTYSMCDLLRESOURCENAME);
    if (sRTCDLLResourceName.empty())
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_EMPTYRTCDLLRESOURCENAME);

    if (m_pDriverEnvironment->MachineHasResourceData(sSMCDLLResourceName)) {
        m_pDriverEnvironment->RetrieveMachineResourceData(sSMCDLLResourceName, m_SMCDLLResourceData);
    }
    else {
        if (m_pDriverEnvironment->DriverHasResourceData(sSMCDLLResourceName)) {
            m_pDriverEnvironment->RetrieveDriverResourceData(sSMCDLLResourceName, m_SMCDLLResourceData);
        } else 
            throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_SMCSDKRESOURCENOTFOUND, "SMC SDK Resource not found: " + sSMCDLLResourceName);
    }

    if (m_SMCDLLResourceData.empty())
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_COULDNOTSTORESMCSDK);

    if (m_pDriverEnvironment->MachineHasResourceData(sRTCDLLResourceName)) {
        m_pDriverEnvironment->RetrieveMachineResourceData(sRTCDLLResourceName, m_RTCDLLResourceData);
    }
    else {
        if (m_pDriverEnvironment->DriverHasResourceData(sRTCDLLResourceName)) {
            m_pDriverEnvironment->RetrieveDriverResourceData(sRTCDLLResourceName, m_RTCDLLResourceData);
        }
        else
            throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_RTCSDKRESOURCENOTFOUND, "RTC SDK Resource not found: " + sRTCDLLResourceName);
    }

    if (m_RTCDLLResourceData.empty())
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_COULDNOTSTORERTCSDK);

}

void CDriver_ScanLabSMC::SetXercesDLLResource(const std::string& sXercesDLLResourceName)
{
    if (m_pSDK.get() != nullptr)
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_SDKALREADYLOADED);

    m_XercesDLLResourceData.resize(0);

    if (sXercesDLLResourceName.empty())
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_EMPTYXERCESDLLRESOURCENAME);

    if (m_pDriverEnvironment->MachineHasResourceData(sXercesDLLResourceName)) {
        m_pDriverEnvironment->RetrieveMachineResourceData(sXercesDLLResourceName, m_XercesDLLResourceData);
    }
    else {
        if (m_pDriverEnvironment->DriverHasResourceData(sXercesDLLResourceName)) {
            m_pDriverEnvironment->RetrieveDriverResourceData(sXercesDLLResourceName, m_XercesDLLResourceData);
        }
        else
            throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_XERCESRESOURCENOTFOUND, "Xerces Resource not found: " + sXercesDLLResourceName);
    }

    if (m_XercesDLLResourceData.empty())
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_COULDNOTSTOREXERCESDLL);

}

void CDriver_ScanLabSMC::SetCustomDLLData(const LibMCDriver_ScanLabSMC_uint64 nSMCDLLResourceDataBufferSize, const LibMCDriver_ScanLabSMC_uint8* pSMCDLLResourceDataBuffer, const LibMCDriver_ScanLabSMC_uint64 nRTCDLLResourceDataBufferSize, const LibMCDriver_ScanLabSMC_uint8* pRTCDLLResourceDataBuffer)
{
    if (m_pSDK.get() != nullptr)
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_SDKALREADYLOADED);

    m_SMCDLLResourceData.resize(0);
    m_RTCDLLResourceData.resize(0);

    if (nSMCDLLResourceDataBufferSize == 0)
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_EMPTYSMCDLLRESOURCEDATA);
    if (nRTCDLLResourceDataBufferSize == 0)
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_EMPTYRTCDLLRESOURCEDATA);

    if (pSMCDLLResourceDataBuffer == nullptr)
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPARAM);
    if (pRTCDLLResourceDataBuffer == nullptr)
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPARAM);

    m_SMCDLLResourceData.resize(nSMCDLLResourceDataBufferSize);
    auto pSMCSrc = pSMCDLLResourceDataBuffer;
    for (size_t nSMCIndex = 0; nSMCIndex < nSMCDLLResourceDataBufferSize; nSMCIndex++) {
        m_SMCDLLResourceData.at(nSMCIndex) = *pSMCSrc;
        pSMCSrc++;
    }

    m_RTCDLLResourceData.resize(nRTCDLLResourceDataBufferSize);
    auto pRTCSrc = pRTCDLLResourceDataBuffer;
    for (size_t nRTCIndex = 0; nRTCIndex < nRTCDLLResourceDataBufferSize; nRTCIndex++) {
        m_RTCDLLResourceData.at(nRTCIndex) = *pRTCSrc;
        pRTCSrc++;
    }

}

void CDriver_ScanLabSMC::SetCustomXercesDLLData(const LibMCDriver_ScanLabSMC_uint64 nXercesDLLResourceDataBufferSize, const LibMCDriver_ScanLabSMC_uint8* pXercesDLLResourceDataBuffer)
{
    if (m_pSDK.get() != nullptr)
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_SDKALREADYLOADED);

    m_XercesDLLResourceData.resize(0);

    if (nXercesDLLResourceDataBufferSize == 0)
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_EMPTYXERCESRESOURCEDATA);

    if (pXercesDLLResourceDataBuffer == nullptr)
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPARAM);

    m_XercesDLLResourceData.resize(nXercesDLLResourceDataBufferSize);
    auto pXercesSrc = pXercesDLLResourceDataBuffer;
    for (size_t nXercesIndex = 0; nXercesIndex < nXercesDLLResourceDataBufferSize; nXercesIndex++) {
        m_SMCDLLResourceData.at(nXercesIndex) = *pXercesSrc;
        pXercesSrc++;
    }
}


void CDriver_ScanLabSMC::LoadSDK()
{
#ifndef _WIN32
    throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_SMCSDK_UNSUPPORTEDPLATFORM);
#endif

    if (m_pSDK.get() != nullptr)
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_SDKALREADYLOADED);

    if (m_SMCDLLResourceData.empty() || m_RTCDLLResourceData.empty())
        SetDLLResources(SCANLABSMC_DEFAULT_SMCDLLRESOURCENAME, SCANLABSMC_DEFAULT_RTCDLLRESOURCENAME);
    if (m_XercesDLLResourceData.empty())
        SetXercesDLLResource(SCANLABSMC_DEFAULT_XERCESDLLRESOURCENAME);

    m_pSMCDLL = m_pDLLDirectory->StoreCustomData("SCANmotionControl_x64.dll", m_SMCDLLResourceData);
    m_pRTCDLL = m_pDLLDirectory->StoreCustomData("RTC6DLLx64.dll", m_RTCDLLResourceData);
    m_pXercesDLL = m_pDLLDirectory->StoreCustomData("xerces-c_3_2.dll", m_XercesDLLResourceData);

    m_pSDK = std::make_shared<CScanLabSMCSDK>(m_pSMCDLL->GetAbsoluteFileName ());

    // Free up resource data buffers
    m_SMCDLLResourceData.resize(0);
    m_RTCDLLResourceData.resize(0);
    m_XercesDLLResourceData.resize(0);
}


ISMCConfiguration* CDriver_ScanLabSMC::CreateEmptyConfiguration()
{
    return new CSMCConfiguration(m_pDriverEnvironment);
}

ISMCConfiguration* CDriver_ScanLabSMC::CreateTemplateConfiguration(const std::string& sTemplateName)
{
    throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_NOTIMPLEMENTED);
}

ISMCContext* CDriver_ScanLabSMC::CreateContext(ISMCConfiguration* pSMCConfiguration) 
{
    if (pSMCConfiguration == nullptr)
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_NOTIMPLEMENTED);

    if (m_pSDK.get() == nullptr)
        LoadSDK();

    return new CSMCContext (pSMCConfiguration, m_pSDK, m_pDriverEnvironment);
}


void CDriver_ScanLabSMC::Configure(const std::string& sConfigurationString)
{
    
}

std::string CDriver_ScanLabSMC::GetName()
{
    return m_sName;
}

std::string CDriver_ScanLabSMC::GetType()
{
    return m_sType;
}

void CDriver_ScanLabSMC::GetVersion(LibMCDriver_ScanLabSMC_uint32& nMajor, LibMCDriver_ScanLabSMC_uint32& nMinor, LibMCDriver_ScanLabSMC_uint32& nMicro, std::string& sBuild)
{
    nMajor = LIBMCDRIVER_SCANLABSMC_VERSION_MAJOR;
    nMinor = LIBMCDRIVER_SCANLABSMC_VERSION_MINOR;
    nMicro = LIBMCDRIVER_SCANLABSMC_VERSION_MICRO;
    sBuild = __STRINGIZE_VALUE_OF(__GITHASH);
}

void CDriver_ScanLabSMC::QueryParameters()
{
    QueryParametersEx(m_pDriverEnvironment->CreateStatusUpdateSession());

}

void CDriver_ScanLabSMC::QueryParametersEx(LibMCEnv::PDriverStatusUpdateSession pDriverUpdateInstance)
{
    
}

