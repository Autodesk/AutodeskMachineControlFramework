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
#include <iostream>

using namespace LibMCDriver_ScanLabSMC::Impl;

#define SMCCONFIGURATION_MINSERIALNUMBER 1
#define SMCCONFIGURATION_MAXSERIALNUMBER 100000000

/*************************************************************************************************************************
 Class definition of CSMCConfiguration 
**************************************************************************************************************************/

CSMCConfiguration::CSMCConfiguration(LibMCEnv::PDriverEnvironment pDriverEnvironment)
    : m_pDriverEnvironment (pDriverEnvironment),
      m_DynamicViolationReaction (LibMCDriver_ScanLabSMC::eDynamicViolationReaction::WarningOnly),
     m_WarnLevel (LibMCDriver_ScanLabSMC::eWarnLevel::Error),
    m_nSerialNumber (0),
    m_BlendMode (LibMCDriver_ScanLabSMC::eBlendMode::Deactivated),
    m_bSendToHardware (false)

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

void CSMCConfiguration::SetSerialNumber(const LibMCDriver_ScanLabSMC_uint32 nValue)
{
    if ((nValue < SMCCONFIGURATION_MINSERIALNUMBER) || (nValue > SMCCONFIGURATION_MAXSERIALNUMBER))
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDRTCSERIALNUMBER, "invalid RTC serial number: " + std::to_string (nValue));

    m_nSerialNumber = nValue;
}

LibMCDriver_ScanLabSMC_uint32 CSMCConfiguration::GetSerialNumber()
{
    return m_nSerialNumber;
}

void CSMCConfiguration::SetIPAddress(const std::string& sValue)
{
    m_sIPAddress = sValue;
}

void CSMCConfiguration::SetSendToHardware(const bool bSendToHardware)
{
    m_bSendToHardware = bSendToHardware;
}

bool CSMCConfiguration::GetSendToHardware()
{
    return m_bSendToHardware;
}


std::string CSMCConfiguration::GetIPAddress()
{
    return m_sIPAddress;
}

void CSMCConfiguration::SetCorrectionFile(const LibMCDriver_ScanLabSMC_uint64 nCorrectionFileDataBufferSize, const LibMCDriver_ScanLabSMC_uint8* pCorrectionFileDataBuffer)
{
    m_CorrectionFileData.resize(0);
    if (nCorrectionFileDataBufferSize > 0) {
        if (pCorrectionFileDataBuffer == nullptr)
            throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPARAM);

        m_CorrectionFileData.resize(nCorrectionFileDataBufferSize);
        for (uint64_t nIndex = 0; nIndex < nCorrectionFileDataBufferSize; nIndex++)
            m_CorrectionFileData.at(nIndex) = pCorrectionFileDataBuffer[nIndex];

    }

    m_pDriverEnvironment->LogMessage("Correction file data loaded from memory, size: " + std::to_string(m_CorrectionFileData.size()));

}

void CSMCConfiguration::SetCorrectionFileResource(const std::string& sResourceName)
{
    m_CorrectionFileData.resize(0);

    if (sResourceName.empty ())
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_EMPTYRTCCORRECTIONRESOURCENAME, "empty RTC correction resource name");

    if (m_pDriverEnvironment->MachineHasResourceData(sResourceName)) {
        m_pDriverEnvironment->RetrieveMachineResourceData(sResourceName, m_CorrectionFileData);
    }
    else if (m_pDriverEnvironment->DriverHasResourceData(sResourceName)) 
    {
        m_pDriverEnvironment->RetrieveDriverResourceData(sResourceName, m_CorrectionFileData);
    }
    else {
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_RTCCORRECTIONRESOURCENOTFOUND, "RTC correction resource not found: " + sResourceName);
    }

    m_pDriverEnvironment->LogMessage ("Correction file data loaded from resource " + sResourceName + ", size: " + std::to_string (m_CorrectionFileData.size()));

}



void CSMCConfiguration::SetConfigurationTemplateResource(const std::string& sResourceName)
{
    if (sResourceName.empty())
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_EMPTYCONFIGURATIONRESOURCENAME, "empty RTC correction resource name");

    std::vector <uint8_t> configurationXMLData;
    if (m_pDriverEnvironment->MachineHasResourceData(sResourceName)) {        
        m_pDriverEnvironment->RetrieveMachineResourceData(sResourceName, configurationXMLData);

    }
    else if (m_pDriverEnvironment->DriverHasResourceData(sResourceName))
    {
        m_pDriverEnvironment->RetrieveDriverResourceData(sResourceName, configurationXMLData);
    }
    else {
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_CONFIGURATIONRESOURCENOTFOUND, "RTC correction resource not found: " + sResourceName);
    }
    
    m_sConfigurationTemplateXML = std::string(configurationXMLData.begin(), configurationXMLData.end());
}

void CSMCConfiguration::SetConfigurationTemplate(const std::string& sTemplateXML)
{
    m_sConfigurationTemplateXML = sTemplateXML;
}

std::string CSMCConfiguration::GetConfigurationTemplate()
{
    return m_sConfigurationTemplateXML;
}

void CSMCConfiguration::SetSimulationSubDirectory(const std::string& sSubDirectory)
{
    m_sSimulationSubDirectory = sSubDirectory;
}

std::string CSMCConfiguration::GetSimulationSubDirectory()
{
    return m_sSimulationSubDirectory;
}

void CSMCConfiguration::SetBlendMode(const LibMCDriver_ScanLabSMC::eBlendMode eBlendMode)
{
    m_BlendMode = eBlendMode;
}

LibMCDriver_ScanLabSMC::eBlendMode CSMCConfiguration::GetBlendMode()
{
    return m_BlendMode;
}


void CSMCConfiguration::SetFirmware(const LibMCDriver_ScanLabSMC_uint64 nFirmwareDataBufferSize, const LibMCDriver_ScanLabSMC_uint8* pFirmwareDataBuffer, const LibMCDriver_ScanLabSMC_uint64 nFPGADataBufferSize, const LibMCDriver_ScanLabSMC_uint8* pFPGADataBuffer, const LibMCDriver_ScanLabSMC_uint64 nAuxiliaryDataBufferSize, const LibMCDriver_ScanLabSMC_uint8* pAuxiliaryDataBuffer)
{
    m_FirmwareData.resize (0);
    m_FPGAData.resize (0);
    m_AuxiliaryData.resize (0);

    if (nFirmwareDataBufferSize > 0) {
        if (pFirmwareDataBuffer == nullptr)
            throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPARAM);

        m_FirmwareData.resize(nFirmwareDataBufferSize);
        for (uint64_t nIndex = 0; nIndex < nFirmwareDataBufferSize; nIndex++)
            m_FirmwareData.at(nIndex) = pFirmwareDataBuffer[nIndex];

    }


    if (nFPGADataBufferSize > 0) {
        if (pFPGADataBuffer == nullptr)
            throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPARAM);

        m_FPGAData.resize(nFPGADataBufferSize);
        for (uint64_t nIndex = 0; nIndex < nFPGADataBufferSize; nIndex++)
            m_FPGAData.at(nIndex) = pFPGADataBuffer[nIndex];

    }

    if (nAuxiliaryDataBufferSize > 0) {
        if (pAuxiliaryDataBuffer == nullptr)
            throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPARAM);

        m_AuxiliaryData.resize(nAuxiliaryDataBufferSize);
        for (uint64_t nIndex = 0; nIndex < nAuxiliaryDataBufferSize; nIndex++)
            m_AuxiliaryData.at(nIndex) = pAuxiliaryDataBuffer[nIndex];

    }

}

void CSMCConfiguration::SetFirmwareResources(const std::string& sFirmwareDataResource, const std::string& sFPGADataResource, const std::string& sAuxiliaryDataResource)
{
    m_FirmwareData.resize(0);
    m_FPGAData.resize(0);
    m_AuxiliaryData.resize(0);

    if (sFirmwareDataResource.empty())
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_EMPTYRTCFIRMWARERESOURCENAME);

    m_sFirmwareDataResource = sFirmwareDataResource;

    if (m_pDriverEnvironment->MachineHasResourceData(sFirmwareDataResource)) {
        m_pDriverEnvironment->RetrieveMachineResourceData(sFirmwareDataResource, m_FirmwareData);
    }
    else if (m_pDriverEnvironment->DriverHasResourceData(sFirmwareDataResource))
    {
        m_pDriverEnvironment->RetrieveDriverResourceData(sFirmwareDataResource, m_FirmwareData);
    }
    else {
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_RTCFIRMWARERESOURCENOTFOUND, "RTC firmware resource not found: " + sFirmwareDataResource);
    }


    if (sFPGADataResource.empty())
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_EMPTYRTCFPGARESOURCENAME);

    if (m_pDriverEnvironment->MachineHasResourceData(sFPGADataResource)) {
        m_pDriverEnvironment->RetrieveMachineResourceData(sFPGADataResource, m_FPGAData);
    }
    else if (m_pDriverEnvironment->DriverHasResourceData(sFPGADataResource))
    {
        m_pDriverEnvironment->RetrieveDriverResourceData(sFPGADataResource, m_FPGAData);
    }
    else {
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_RTCFPGARESOURCENOTFOUND, "RTC FPGA resource not found: " + sFPGADataResource);
    }

    if (sAuxiliaryDataResource.empty())
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_EMPTYRTCAUXILIARYRESOURCENAME);

    if (m_pDriverEnvironment->MachineHasResourceData(sAuxiliaryDataResource)) {
        m_pDriverEnvironment->RetrieveMachineResourceData(sAuxiliaryDataResource, m_AuxiliaryData);
    }
    else if (m_pDriverEnvironment->DriverHasResourceData(sAuxiliaryDataResource))
    {
        m_pDriverEnvironment->RetrieveDriverResourceData(sAuxiliaryDataResource, m_AuxiliaryData);
    }
    else {
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_RTCAUXILIARYRESOURCENOTFOUND, "RTC auxiliary resource not found: " + sAuxiliaryDataResource);
    }

}

std::string CSMCConfiguration::buildConfigurationXML(LibMCEnv::CWorkingDirectory* pWorkingDirectory, LibMCDriver_ScanLabSMC::eSMCConfigVersion configVersion, const std::string& sRTCIPAddress, LibMCEnv::PWorkingFile pCorrectionFileToUse)
{

    if (pWorkingDirectory == nullptr)
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPARAM);
    if ((m_nSerialNumber < SMCCONFIGURATION_MINSERIALNUMBER) || (m_nSerialNumber > SMCCONFIGURATION_MAXSERIALNUMBER))
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDRTCSERIALNUMBER);
    if (pCorrectionFileToUse.get() == nullptr)
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPARAM);

    if (pCorrectionFileToUse->GetSize () == 0)
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_EMPTYRTCCORRECTIONFILE);

    if (!sRTCIPAddress.empty()) {
        for (auto ch : sRTCIPAddress)
            if (!(((ch >= '0') && (ch <= '9')) || (ch == '.')))
                throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDIPADDRESS, "invalid RTC IP Address: " + sRTCIPAddress);
    }

    pWorkingDirectory->StoreCustomData("RTC6RBF.rbf", m_FPGAData);
    if (sRTCIPAddress.empty()) {
        pWorkingDirectory->StoreCustomData("RTC6OUT.out", m_FirmwareData);
    }
    else {
        pWorkingDirectory->StoreCustomData("RTC6ETH.out", m_FirmwareData);
    }

    pWorkingDirectory->StoreCustomData("RTC6DAT.dat", m_AuxiliaryData);

    std::string sBaseDirectoryPath = pWorkingDirectory->GetAbsoluteFilePath();
    std::string sCorrectionFilePath = pCorrectionFileToUse->GetAbsoluteFileName();
    std::string sLogFilePath = pWorkingDirectory->GetAbsoluteFilePath() + "/log.txt";

    std::string sSimulationDirectory = sBaseDirectoryPath + "/";
    if (!m_sSimulationSubDirectory.empty())
        sSimulationDirectory += m_sSimulationSubDirectory + "/";

    auto pXMLDocument = m_pDriverEnvironment->CreateXMLDocument("Configuration", "SCANmotionControl");
    
    pXMLDocument->ChangeNamespacePrefix("", "cfg");
    pXMLDocument->RegisterNamespace("http://www.w3.org/2001/XMLSchema-instance", "xsi");

    auto pConfigurationNode = pXMLDocument->GetRootNode();

    std::string sBoardsNodeName;
    std::string sVersionString;
    std::string sSchemaLocationString;
    std::string sSimulationMode;
    std::vector<std::string> nodesToCopyFromTemplate;

    switch (configVersion) {
        case eSMCConfigVersion::Version_0_8: 

            // Customize writing to SMC Version 0.8
            sVersionString = "0.8";
            sSchemaLocationString = "cfg SCANmotionControl_0_8.xsd";
            sBoardsNodeName = "Boards";
            if (m_bSendToHardware)
                sSimulationMode = "false";
            else
                sSimulationMode = "true";

            nodesToCopyFromTemplate.push_back("ScanDeviceConfig");
            nodesToCopyFromTemplate.push_back("LaserConfig");
            nodesToCopyFromTemplate.push_back("IOConfig");
            nodesToCopyFromTemplate.push_back("TrajectoryConfig");

            break;
        case eSMCConfigVersion::Version_0_9:
            // Customize writing to SMC Version 0.9
            sVersionString = "0.9";
            sSchemaLocationString = "cfg SCANmotionControl_0_9.xsd";
            sBoardsNodeName = "BoardList";
            if (m_bSendToHardware)
                sSimulationMode = "false";
            else
                sSimulationMode = "true";
            nodesToCopyFromTemplate.push_back("AxesList");
            nodesToCopyFromTemplate.push_back("ScanheadList");
            nodesToCopyFromTemplate.push_back("KinematicsList");
            nodesToCopyFromTemplate.push_back("LaserConfig");
            nodesToCopyFromTemplate.push_back("IOConfig");

            break;
        case eSMCConfigVersion::Version_1_0:
            // Customize writing to SMC Version 1.0
            sVersionString = "1.0";
            sSchemaLocationString = "cfg SCANmotionControlConfig_1_0.xsd";
            sBoardsNodeName = "BoardList";
            if (m_bSendToHardware)
                sSimulationMode = "Hardware";
            else
                sSimulationMode = "Simulation";

            nodesToCopyFromTemplate.push_back("AxesList");
            nodesToCopyFromTemplate.push_back("ScanheadList");
            nodesToCopyFromTemplate.push_back("KinematicsList");
            nodesToCopyFromTemplate.push_back("LaserConfig");
            nodesToCopyFromTemplate.push_back("IOConfig");
            nodesToCopyFromTemplate.push_back("SystemConfig");

            break;
        case eSMCConfigVersion::Version_1_1:
            // Customize writing to SMC Version 1.1
            sVersionString = "1.1";
            sSchemaLocationString = "cfg SCANmotionControlConfig_1_1.xsd";
            sBoardsNodeName = "BoardList";
            if (m_bSendToHardware)
                sSimulationMode = "Hardware";
            else
                sSimulationMode = "Simulation";

            nodesToCopyFromTemplate.push_back("AxesList");
            nodesToCopyFromTemplate.push_back("ScanheadList");
            nodesToCopyFromTemplate.push_back("KinematicsList");
            nodesToCopyFromTemplate.push_back("LaserConfig");
            nodesToCopyFromTemplate.push_back("IOConfig");
            nodesToCopyFromTemplate.push_back("SystemConfig");

            break;
        default:
            throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_UNSUPPORTEDSMCVERSION);
    }
    pConfigurationNode->AddAttribute("", "Version", sVersionString);
    pConfigurationNode->AddAttribute("http://www.w3.org/2001/XMLSchema-instance", "schemaLocation", sSchemaLocationString);

    auto pGeneralConfigNode = pConfigurationNode->AddChild("", "GeneralConfig");
    auto pDynamicViolationReaction = pGeneralConfigNode->AddChildText("", "DynamicViolationReaction", "WarningOnly");
    
    auto pLogConfigNode = pGeneralConfigNode->AddChild("", "LogConfig");
    pLogConfigNode->AddChildText("", "LogfilePath", sLogFilePath);
    switch (m_WarnLevel) {
        case eWarnLevel::Error: 
            pLogConfigNode->AddChildText("", "Loglevel", "Error");
            break;
        case eWarnLevel::Warn:
            pLogConfigNode->AddChildText("", "Loglevel", "Warn");
            break;
        case eWarnLevel::Info:
            pLogConfigNode->AddChildText("", "Loglevel", "Info");
            break;
        default:
            throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDWARNINGLEVEL);

    }
    
    pLogConfigNode->AddChildText("", "EnableConsoleLogging", "false");
    pLogConfigNode->AddChildText("", "EnableFilelogging", "true");
    pLogConfigNode->AddChildText("", "MaxLogfileSize", "26214400");
    pLogConfigNode->AddChildText("", "MaxBackupFileCount", "0");

    pGeneralConfigNode->AddChildText("", "BaseDirectoryPath", sBaseDirectoryPath);

    auto pSimulationConfigNode = pGeneralConfigNode->AddChild("", "SimulationConfig");
    pSimulationConfigNode->AddChildText("", "SimulationMode", sSimulationMode);
    pSimulationConfigNode->AddChildText("", "SimOutputFileDirectory", sSimulationDirectory);
    pSimulationConfigNode->AddChildText("", "BinaryOutput", "false");
    pSimulationConfigNode->AddChildText("", "DisableFileOutput", "false");

    auto pRTCConfigNode = pConfigurationNode->AddChild("", "RTCConfig");
    pRTCConfigNode->AddChildText("", "BoardIdentificationMethod", "BySerialNumber");
    pRTCConfigNode->AddChildText("", "ProgramFileDirectory", "[BaseDirectoryPath]");
    
    
    auto pBoardsNode = pRTCConfigNode->AddChild("", sBoardsNodeName);

    auto pRTC6Node = pBoardsNode->AddChild("", "RTC6");
    if (configVersion != eSMCConfigVersion::Version_0_8)
        pRTC6Node->AddAttribute("", "Name", "RtcSingle");

    pRTC6Node->AddChildText("", "SerialNumber", std::to_string(m_nSerialNumber));
    switch (configVersion) {
        case eSMCConfigVersion::Version_0_8: {
            pRTC6Node->AddChildText("", "HeadA", "ScanDevice1");
            pRTC6Node->AddChildText("", "HeadB", "None");
            break;
        }

        case eSMCConfigVersion::Version_0_9: 
        case eSMCConfigVersion::Version_1_0: 
        case eSMCConfigVersion::Version_1_1: {
            if (!sRTCIPAddress.empty()) {
                auto pEthSearchNode = pRTCConfigNode->AddChild("", "EthSearch");
                auto pIPListNode = pEthSearchNode->AddChild("", "IPList");
                pIPListNode->AddChildText("", "IPAddress", sRTCIPAddress);
            }

            break;
        }

    }

    if (!m_sConfigurationTemplateXML.empty()) {
        auto pTemplateDocument = m_pDriverEnvironment->ParseXMLString(m_sConfigurationTemplateXML);
        auto pTemplateRootNode = pTemplateDocument->GetRootNode();
        auto sTemplateNameSpace = pXMLDocument->GetDefaultNamespace();

        auto pTemplateVersionAttribute = pTemplateRootNode->FindAttribute (sTemplateNameSpace, "Version", true);
        std::string sTemplateVersionString = pTemplateVersionAttribute->GetValue();
        if (sTemplateVersionString != sVersionString)
            throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_SMCTEMPLATEVERSIONMISMATCH, "SMC Template version mismatch: " + sTemplateVersionString + " != " + sVersionString);

        for (auto sNodeToCopy : nodesToCopyFromTemplate) {
            auto pNewScanDeviceConfigNode = pConfigurationNode->AddChild(sTemplateNameSpace, sNodeToCopy);
            auto pOldScanDeviceConfigNode = pTemplateRootNode->FindChild(sTemplateNameSpace, sNodeToCopy, false);
            if (pOldScanDeviceConfigNode != nullptr) {
                pNewScanDeviceConfigNode->CopyFrom(pOldScanDeviceConfigNode);
            }
        }


    }

    // Patch Correction file path!
    auto sCfgNameSpace = pXMLDocument->GetDefaultNamespace();
    if (configVersion == eSMCConfigVersion::Version_0_8) {
        auto pScanDeviceConfigNode = pXMLDocument->GetRootNode()->FindChild(sCfgNameSpace, "ScanDeviceConfig", true);
        auto pScanDeviceListNode = pScanDeviceConfigNode->FindChild(sCfgNameSpace, "ScanDeviceList", true);

        auto pScanDevicesNode = pScanDeviceListNode->GetChildrenByName(sCfgNameSpace, "ScanDevice");
        uint64_t nDeviceCount = pScanDevicesNode->GetNodeCount();
        for (uint64_t nDeviceIndex = 0; nDeviceIndex < nDeviceCount; nDeviceIndex++) {
            auto pScanDeviceNode = pScanDevicesNode->GetNode(nDeviceIndex);
            auto pCorrectionFileListNode = pScanDeviceNode->FindChild(sCfgNameSpace, "CorrectionFileList", true);
            pCorrectionFileListNode->RemoveChildrenWithName(sCfgNameSpace, "CorrectionFilePath");

            auto pPatchedCorrectionFileNode = pCorrectionFileListNode->AddChildText(sCfgNameSpace, "CorrectionFilePath", sCorrectionFilePath);
            pPatchedCorrectionFileNode->AddAttribute("", "CalibrationFactor", "-1");
        }

        
    } 

    if (configVersion == eSMCConfigVersion::Version_1_0 || configVersion == eSMCConfigVersion::Version_1_1) {
        auto pKinematicsListNode = pXMLDocument->GetRootNode()->FindChild(sCfgNameSpace, "KinematicsList", true);
        auto pKinematicsListNodes = pKinematicsListNode->GetChildrenByName(sCfgNameSpace, "Kinematic");
        uint64_t nKinematicsListCount = pKinematicsListNodes->GetNodeCount();
        for (uint64_t nKinematicsListIndex = 0; nKinematicsListIndex < nKinematicsListCount; nKinematicsListIndex++) {

            auto pKinematicNode = pKinematicsListNodes->GetNode(nKinematicsListIndex);
			auto pTrafoListNode = pKinematicNode->FindChild(sCfgNameSpace, "TrafoList", true);

			auto pTrafoListNodes = pTrafoListNode->GetChildrenByName(sCfgNameSpace, "PosTrafos");
			uint64_t nTrafoListCount = pTrafoListNodes->GetNodeCount();
            for (uint64_t nTrafoListIndex = 0; nTrafoListIndex < nTrafoListCount; nTrafoListIndex++) {
				auto pTrafoNode = pTrafoListNodes->GetNode(nTrafoListIndex);
				auto pCorrectionFileListNode = pTrafoNode->FindChild(sCfgNameSpace, "CorrectionFileList", true);
                pCorrectionFileListNode->RemoveChildrenWithName(sCfgNameSpace, "CorrectionFilePath");

                auto pPatchedCorrectionFileNode = pCorrectionFileListNode->AddChildText(sCfgNameSpace, "CorrectionFilePath", sCorrectionFilePath);
                pPatchedCorrectionFileNode->AddAttribute("", "CalibrationFactor", "-1");

            }
        }
    }

    // Patch Blend Mode!
    if (configVersion == eSMCConfigVersion::Version_0_8) {
        auto pTrajectoryConfigNode = pXMLDocument->GetRootNode()->FindChild(sCfgNameSpace, "TrajectoryConfig", true);
        auto pGeometryConfigNode = pTrajectoryConfigNode->FindChild(sCfgNameSpace, "GeometryConfig", true);
        auto pBlendModeNode = pGeometryConfigNode->FindChild(sCfgNameSpace, "BlendMode", true);
        switch (m_BlendMode) {
            case eBlendMode::SwiftBlending: pBlendModeNode->SetTextContent("SwiftBlending"); break;
            case eBlendMode::MaxAccuracy: pBlendModeNode->SetTextContent("MaxAccuracy"); break;
            default: pBlendModeNode->SetTextContent("Deactivated");
        }
    }

    std::string sXMLString = pXMLDocument->SaveToString (true);

    //std::cout << sXMLString << std::endl;
    
    //m_pDriverEnvironment->Sleep(100000);

    return sXMLString;
};

std::vector<uint8_t> & CSMCConfiguration::getCorrectionFileData()
{
    return m_CorrectionFileData;
}
