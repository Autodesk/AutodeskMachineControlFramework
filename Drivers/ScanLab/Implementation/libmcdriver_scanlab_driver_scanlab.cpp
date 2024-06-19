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


Abstract: This is a stub class definition of CDriver_ScanLab

*/

#include "libmcdriver_scanlab_driver_scanlab.hpp"
#include "libmcdriver_scanlab_interfaceexception.hpp"

#include "libmcdriver_scanlab_rtcselector.hpp"
// Include custom headers here.


using namespace LibMCDriver_ScanLab::Impl;

/*************************************************************************************************************************
 Class definition of CDriver_ScanLab 
**************************************************************************************************************************/

CDriver_ScanLab::CDriver_ScanLab(LibMCEnv::PDriverEnvironment pDriverEnvironment)
    : m_pDriverEnvironment (pDriverEnvironment), m_nDLLVersion (0), m_bEnableJournaling (false)
{
    if (pDriverEnvironment.get() == nullptr)
        throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);

    m_pOwnerData = std::make_shared<CRTCContextOwnerData>();

    m_dDefaultInitialTimeout = SCANLAB_DEFAULTTIMEOUT_INITIAL;
    m_dDefaultMaxTimeout = SCANLAB_DEFAULTTIMEOUT_MAX;
    m_dDefaultMultiplier = SCANLAB_DEFAULTTIMEOUT_MULTIPLIER;
}


IRTCSelector * CDriver_ScanLab::CreateRTCSelector()
{
    if (m_pScanLabSDK.get() == nullptr)
        throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_SCANLABSDKNOTLOADED);

    return new CRTCSelector(m_pOwnerData, m_pDriverEnvironment, m_dDefaultInitialTimeout, m_dDefaultMaxTimeout, m_dDefaultMultiplier);
}


void CDriver_ScanLab::LoadSDK(const std::string& sResourceName)
{
    if (m_pScanLabSDK.get() != nullptr)
        throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_SCANLABSDKALREADYLOADED);

    std::string sFileName;
#ifdef _WIN32
    sFileName = "RTC.dll";
#else
    sFileName = "RTC.so";
#endif

    m_pWorkingDirectory = m_pDriverEnvironment->CreateWorkingDirectory();

    if (m_pDriverEnvironment->MachineHasResourceData(sResourceName)) {
        std::vector<uint8_t> SDKBuffer;
        m_pDriverEnvironment->RetrieveMachineResourceData(sResourceName, SDKBuffer);
        if (SDKBuffer.size () == 0)
            throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDSCANLABSDK);

        m_pSDKLibraryFile = m_pWorkingDirectory->StoreCustomData(sFileName, LibMCEnv::CInputVector<uint8_t>(SDKBuffer.data(), SDKBuffer.size ()));
    }
    else {
        m_pSDKLibraryFile = m_pWorkingDirectory->StoreDriverData(sFileName, sResourceName);
    }

    if (m_pSDKLibraryFile->GetSize () == 0)
        throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDSCANLABSDK);

    m_pScanLabSDK = std::make_shared<CScanLabSDK>(m_pSDKLibraryFile->GetAbsoluteFileName());
    m_pOwnerData->setScanLabSDK(m_pScanLabSDK);

    if (m_bEnableJournaling) {
        std::string sJournalFileName = "journal.txt";
        m_pJournalFile = m_pWorkingDirectory->AddManagedFile(sJournalFileName);
        m_pScanLabSDK->setJournal(std::make_shared<CScanLabSDKJournal>(m_pJournalFile->GetAbsoluteFileName()));
    }

    m_nDLLVersion = m_pScanLabSDK->get_dll_version();

    updateDLLVersionParameter(m_nDLLVersion);

}

void CDriver_ScanLab::LoadCustomSDK(const LibMCDriver_ScanLab_uint64 nScanlabDLLBufferSize, const LibMCDriver_ScanLab_uint8* pScanlabDLLBuffer)
{
    if (m_pScanLabSDK.get() != nullptr)
        throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_SCANLABSDKALREADYLOADED);

    if ((pScanlabDLLBuffer == nullptr) || (nScanlabDLLBufferSize == 0))
        throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDSCANLABSDK);

    std::string sFileName;
#ifdef _WIN32
    sFileName = "RTC.dll";
#else
    sFileName = "RTC.so";
#endif

    m_pWorkingDirectory = m_pDriverEnvironment->CreateWorkingDirectory();
    m_pSDKLibraryFile = m_pWorkingDirectory->StoreCustomData(sFileName, LibMCEnv::CInputVector<uint8_t> (pScanlabDLLBuffer, nScanlabDLLBufferSize));
    m_pScanLabSDK = std::make_shared<CScanLabSDK>(m_pSDKLibraryFile->GetAbsoluteFileName());
    m_pOwnerData->setScanLabSDK(m_pScanLabSDK);

    m_nDLLVersion = m_pScanLabSDK->get_dll_version();

    updateDLLVersionParameter(m_nDLLVersion);

}

uint32_t CDriver_ScanLab::getDLLVersion()
{
    return m_nDLLVersion;
}

void CDriver_ScanLab::EnableJournaling()
{
    m_bEnableJournaling = true;
}

void CDriver_ScanLab::SetFirmware(const std::string& sFirmwareResource, const std::string& sFPGAResource, const std::string& sAuxiliaryResource)
{
    if (m_pDriverEnvironment->MachineHasResourceData(sFirmwareResource)) {
        m_pDriverEnvironment->RetrieveMachineResourceData(sFirmwareResource, m_FirmwareData);
    }
    else {
        m_pDriverEnvironment->RetrieveDriverResourceData(sFirmwareResource, m_FirmwareData);
    }

    if (m_pDriverEnvironment->MachineHasResourceData(sFPGAResource)) {
        m_pDriverEnvironment->RetrieveMachineResourceData(sFPGAResource, m_FPGAData);
    }
    else {
        m_pDriverEnvironment->RetrieveDriverResourceData(sFPGAResource, m_FPGAData);
    }

    if (m_pDriverEnvironment->MachineHasResourceData(sAuxiliaryResource)) {
        m_pDriverEnvironment->RetrieveMachineResourceData(sAuxiliaryResource, m_AuxiliaryData);
    }
    else {
        m_pDriverEnvironment->RetrieveDriverResourceData(sAuxiliaryResource, m_AuxiliaryData);
    }

}

void CDriver_ScanLab::SetCustomFirmware(const LibMCDriver_ScanLab_uint64 nFirmwareDataBufferSize, const LibMCDriver_ScanLab_uint8* pFirmwareDataBuffer, const LibMCDriver_ScanLab_uint64 nFPGADataBufferSize, const LibMCDriver_ScanLab_uint8* pFPGADataBuffer, const LibMCDriver_ScanLab_uint64 nAuxiliaryDataBufferSize, const LibMCDriver_ScanLab_uint8* pAuxiliaryDataBuffer)
{
    m_FirmwareData.clear();
    m_FPGAData.clear();
    m_AuxiliaryData.clear();

    if ((nFirmwareDataBufferSize > 0) && (pFirmwareDataBuffer != nullptr)) {
        m_FirmwareData.resize(nFirmwareDataBufferSize);
        for (uint64_t nIndex = 0; nIndex < nFirmwareDataBufferSize; nIndex++)
            m_FirmwareData[nIndex] = pFirmwareDataBuffer[nIndex];
    }

    if ((nFPGADataBufferSize > 0) && (pFPGADataBuffer != nullptr)) {
        m_FPGAData.resize(nFPGADataBufferSize);
        for (uint64_t nIndex = 0; nIndex < nFPGADataBufferSize; nIndex++)
            m_FPGAData[nIndex] = pFPGADataBuffer[nIndex];
    }

    if ((nAuxiliaryDataBufferSize > 0) && (pAuxiliaryDataBuffer != nullptr)) {
        m_AuxiliaryData.resize(nAuxiliaryDataBufferSize);
        for (uint64_t nIndex = 0; nIndex < nAuxiliaryDataBufferSize; nIndex++)
            m_AuxiliaryData[nIndex] = pAuxiliaryDataBuffer[nIndex];
    }

}
