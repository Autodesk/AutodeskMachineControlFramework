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
    m_nSerialNumber (0)

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

std::string CSMCConfiguration::buildConfigurationXML(LibMCEnv::CWorkingDirectory* pWorkingDirectory, LibMCEnv::PWorkingFile& newCorrectionFile)
{
    if (pWorkingDirectory == nullptr)
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPARAM);
    if ((m_nSerialNumber < SMCCONFIGURATION_MINSERIALNUMBER) || (m_nSerialNumber > SMCCONFIGURATION_MAXSERIALNUMBER))
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDRTCSERIALNUMBER);
    if (m_CorrectionFileData.size () == 0)
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_EMPTYRTCCORRECTIONFILE);

    if (m_sIPAddress.empty ())
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_EMPTYIPADDRESS);
    for (auto ch : m_sIPAddress)
        if (!( ((ch >= '0') && (ch <= '9')) || (ch == '.') ))
            throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDIPADDRESS, "invalid RTC IP Address: " + m_sIPAddress);

    newCorrectionFile = pWorkingDirectory->StoreCustomDataInTempFile("ct5", m_CorrectionFileData);

    std::string sBaseDirectoryPath = pWorkingDirectory->GetAbsoluteFilePath();
    std::string sCorrectionFilePath = newCorrectionFile->GetAbsoluteFileName();
    std::string sLogFilePath = pWorkingDirectory->GetAbsoluteFilePath() + "/log.txt";

    auto pXMLDocument = m_pDriverEnvironment->CreateXMLDocument("Configuration", "SCANmotionControl");

    pXMLDocument->ChangeNamespacePrefix("", "cfg");
    pXMLDocument->RegisterNamespace("http://www.w3.org/2001/XMLSchema-instance", "xsi");

    auto pConfigurationNode = pXMLDocument->GetRootNode();
    pConfigurationNode->AddAttribute("", "Version", "0.6");
    pConfigurationNode->AddAttribute("http://www.w3.org/2001/XMLSchema-instance", "schemaLocation", "cfg SCANmotionControl_0_6.xsd");

    auto pGeneralConfigNode = pConfigurationNode->AddChild("", "GeneralConfig");
    auto pDynamicViolationReaction = pGeneralConfigNode->AddChildText("", "DynamicViolationReaction", "WarningOnly");
    
    auto pLogConfigNode = pGeneralConfigNode->AddChild("", "LogConfig");
    pLogConfigNode->AddChildText("", "LogfilePath", sLogFilePath);
    pLogConfigNode->AddChildText("", "Loglevel", "Warn");
    pLogConfigNode->AddChildText("", "EnableConsoleLogging", "false");
    pLogConfigNode->AddChildText("", "EnableFilelogging", "true");
    pLogConfigNode->AddChildText("", "MaxLogfileSize", "26214400");
    pLogConfigNode->AddChildText("", "MaxBackupFileCount", "0");

    pGeneralConfigNode->AddChildText("", "BaseDirectoryPath", sBaseDirectoryPath);

    auto pSimulationConfigNode = pGeneralConfigNode->AddChild("", "SimulationConfig");
    pSimulationConfigNode->AddChildText("", "SimulationMode", "true");
    pSimulationConfigNode->AddChildText("", "SimOutputFileDirectory", "[BaseDirectoryPath]/Simulate/");
    pSimulationConfigNode->AddChildText("", "BinaryOutput", "false");
    pSimulationConfigNode->AddChildText("", "DisableFileOutput", "false");

    auto pRTCConfigNode = pConfigurationNode->AddChild("", "RTCConfig");
    pRTCConfigNode->AddChildText("", "BoardIdentificationMethod", "BySerialNumber");
    pRTCConfigNode->AddChildText("", "ProgramFileDirectory", "[BaseDirectoryPath]");
    auto pBoardsNode = pRTCConfigNode->AddChild("", "Boards");

    auto pRTC6Node = pBoardsNode->AddChild("", "RTC6");
    pRTC6Node->AddChildText("", "SerialNumber", std::to_string (m_nSerialNumber));
    pRTC6Node->AddChildText("", "HeadA", "ScanDevice1");
    pRTC6Node->AddChildText("", "HeadB", "None");

    auto pEthSearchNode = pRTCConfigNode->AddChild("", "EthSearch");
    auto pIPListNode = pEthSearchNode->AddChild("", "IPList");
    pIPListNode->AddChildText("", "IPAddress", m_sIPAddress);
    pEthSearchNode->AddChildText("", "EthMaxTimeout", "2.0");


    auto pScanDeviceConfigNode = pConfigurationNode->AddChild("", "ScanDeviceConfig");
    auto pDynamicLimitsNode = pScanDeviceConfigNode->AddChild("", "DynamicLimits");
    auto pDynamicLimitsVelocityNode = pDynamicLimitsNode->AddChildText("", "Velocity", "90");
    pDynamicLimitsVelocityNode->AddAttribute("", "Unit", "rad/s");
    auto pDynamicLimitsAccelerationNode = pDynamicLimitsNode->AddChildText("", "Acceleration", "113140");
    pDynamicLimitsAccelerationNode->AddAttribute("", "Unit", "rad/s^2");
    auto pDynamicLimitsJerkNode = pDynamicLimitsNode->AddChildText("", "Jerk", "4000000000");
    pDynamicLimitsJerkNode->AddAttribute("", "Unit", "rad/s^3");

    auto pCalculationDynamicsNode = pScanDeviceConfigNode->AddChild("", "CalculationDynamics");
    auto pMarkDynamicsNode = pCalculationDynamicsNode->AddChild("", "MarkDynamics");
    auto pMarkDynamicsAccelerationNode = pMarkDynamicsNode->AddChildText("", "Acceleration", "113140");
    pMarkDynamicsAccelerationNode->AddAttribute("", "Unit", "rad/s^2");
    auto pMarkDynamicsJerkNode = pMarkDynamicsNode->AddChildText("", "Jerk", "4000000000");
    pMarkDynamicsJerkNode->AddAttribute("", "Unit", "rad/s^3");

    auto pJumpDynamicsNode = pCalculationDynamicsNode->AddChild("", "JumpDynamics");
    auto pJumpDynamicsAccelerationNode = pJumpDynamicsNode->AddChildText("", "Acceleration", "113140");
    pJumpDynamicsAccelerationNode->AddAttribute("", "Unit", "rad/s^2");
    auto pJumpDynamicsJerkNode = pJumpDynamicsNode->AddChildText("", "Jerk", "4000000000");
    pJumpDynamicsJerkNode->AddAttribute("", "Unit", "rad/s^3");


    auto pFieldLimitsNode = pScanDeviceConfigNode->AddChild("", "FieldLimits");
    auto pXDirectionNode = pFieldLimitsNode->AddChild("", "XDirection");
    pXDirectionNode->AddAttribute("", "Unit", "mm");
    pXDirectionNode->AddAttribute("", "Max", "27");
    pXDirectionNode->AddAttribute("", "Min", "-27");

    auto pYDirectionNode = pFieldLimitsNode->AddChild("", "YDirection");
    pYDirectionNode->AddAttribute("", "Unit", "mm");
    pYDirectionNode->AddAttribute("", "Max", "27");
    pYDirectionNode->AddAttribute("", "Min", "-27");

    pScanDeviceConfigNode->AddChildText("", "MonitoringLevel", "Position");
    auto pFocalLengthNode = pScanDeviceConfigNode->AddChildText("", "FocalLength", "500");
    pFocalLengthNode->AddAttribute("", "Unit", "mm");

    auto pDelayNode = pScanDeviceConfigNode->AddChildText("", "Delay", "0.00125");
    pDelayNode->AddAttribute("", "Unit", "s");

    auto pScanDeviceListNode = pScanDeviceConfigNode->AddChild("", "ScanDeviceList");
    auto pScanDeviceNode = pScanDeviceListNode->AddChild("", "ScanDevice");
    pScanDeviceNode->AddAttribute("", "Name", "ScanDevice1");

    auto pCorrectionFileListNode = pScanDeviceNode->AddChild("", "CorrectionFileList");
    auto pCorrectionFilePathNode = pCorrectionFileListNode->AddChildText("", "CorrectionFilePath", sCorrectionFilePath);
    pCorrectionFilePathNode->AddAttribute("", "CalibrationFactor", "-1");

    auto pAlignmentNode = pScanDeviceNode->AddChild("", "Alignment");
    auto pAlignmentMatrixNode = pAlignmentNode->AddChild("", "Matrix");
    pAlignmentMatrixNode->AddChildText("", "T11", "1.0000");
    pAlignmentMatrixNode->AddChildText("", "T12", "0.0000");
    pAlignmentMatrixNode->AddChildText("", "T21", "0.0000");
    pAlignmentMatrixNode->AddChildText("", "T22", "1.0000");

    auto pAlignmentOffsetNode = pAlignmentNode->AddChild("", "Offset");
    pAlignmentOffsetNode->AddAttribute("", "X", "0.0000");
    pAlignmentOffsetNode->AddAttribute("", "Y", "0.0000");
    pAlignmentOffsetNode->AddAttribute("", "Unit", "mm");


    auto pBasePartDisplacementNode = pScanDeviceNode->AddChild("", "BasePartDisplacement");
    auto pBasePartDisplacementMatrixNode = pBasePartDisplacementNode->AddChild("", "Matrix");
    pBasePartDisplacementMatrixNode->AddChildText("", "T11", "1.0000");
    pBasePartDisplacementMatrixNode->AddChildText("", "T12", "0.0000");
    pBasePartDisplacementMatrixNode->AddChildText("", "T21", "0.0000");
    pBasePartDisplacementMatrixNode->AddChildText("", "T22", "1.0000");

    auto pBasePartDisplacementOffsetNode = pBasePartDisplacementNode->AddChild("", "Offset");
    pBasePartDisplacementOffsetNode->AddAttribute("", "X", "0.0000");
    pBasePartDisplacementOffsetNode->AddAttribute("", "Y", "0.0000");
    pBasePartDisplacementOffsetNode->AddAttribute("", "Unit", "mm");

    pScanDeviceConfigNode->AddChildText("", "DefaultCorrectionFile", "0");

    auto pLaserConfigNode = pConfigurationNode->AddChild("", "LaserConfig");
    pLaserConfigNode->AddChildText("", "LaserMode", "5");

    auto pLaserPortConfigNode = pLaserConfigNode->AddChild("", "LaserPortCfg");
    pLaserPortConfigNode->AddChildText("", "LaserOn", "LaserOnSignal");
    pLaserPortConfigNode->AddChildText("", "Laser1", "Laser1Signal");
    pLaserPortConfigNode->AddChildText("", "Laser2", "Laser2Signal");

    auto pLaserOutputNode = pLaserConfigNode->AddChild("", "LaserOutput");
    pLaserOutputNode->AddAttribute("", "Unit", "s");
    pLaserOutputNode->AddAttribute("", "HalfPeriod", "5E-06");
    pLaserOutputNode->AddAttribute("", "PulseLength", "1E-06");

    auto pLaserStandbyNode = pLaserConfigNode->AddChild("", "LaserStandby");
    pLaserStandbyNode->AddAttribute("", "Unit", "s");
    pLaserStandbyNode->AddAttribute("", "HalfPeriod", "5E-06");
    pLaserStandbyNode->AddAttribute("", "PulseLength", "1E-06");

    auto pQSwitchDelayNode = pLaserConfigNode->AddChildText("", "QSwitchDelay", "0");
    pQSwitchDelayNode->AddAttribute("", "Unit", "s");

    auto pFPulseKillerLengthNode = pLaserConfigNode->AddChildText("", "FPulseKillerLength", "0");
    pFPulseKillerLengthNode->AddAttribute("", "Unit", "s");

    auto pLaserControlFlagsNode = pLaserConfigNode->AddChild("", "LaserControlFlags");
    pLaserControlFlagsNode->AddChildText("", "LaserDisable", "false");
    pLaserControlFlagsNode->AddChildText("", "PulseSwitchSetting", "false");
    pLaserControlFlagsNode->AddChildText("", "LaserSignalPhaseShift", "false");
    pLaserControlFlagsNode->AddChildText("", "LaserOnSignalActiveLow", "false");
    pLaserControlFlagsNode->AddChildText("", "Laser1Laser2SignalActiveLow", "false");
    pLaserControlFlagsNode->AddChildText("", "LaserPulsesAtRisingEdge", "false");
    pLaserControlFlagsNode->AddChildText("", "OutputSynchronizationOn", "false");

    auto pAutomaticLaserControlNode = pLaserConfigNode->AddChild("", "AutomaticLaserControl");
    pAutomaticLaserControlNode->AddChild("", "ActiveChannel");

    /*
        
        
        <cfg:AutomaticLaserControl>
        <cfg:ActiveChannel / >
        <cfg:AnalogOut1 DefaultOutput = "0.5" Format = "Factor">
        <cfg:Shift Unit = "s">-0.001 < / cfg:Shift >
        <cfg:RadiusFactor Enabled = "false" RadiusUnit = "mm" / >
        <cfg:VelocityFactor Enabled = "true" VelocityUnit = "mm/s">
        <cfg:DataPoint Velocity = "0" Factor = "0" / >
        <cfg:DataPoint Velocity = "400" Factor = "1" / >
        <cfg:DataPoint Velocity = "4000" Factor = "2" / >
        < / cfg:VelocityFactor>
        < / cfg:AnalogOut1>
        <cfg:AnalogOut2 DefaultOutput = "0" Format = "Factor">
        < cfg:Shift Unit = "s">0 < / cfg:Shift >
        <cfg:RadiusFactor Enabled = "false" RadiusUnit = "mm" / >
        <cfg:VelocityFactor Enabled = "false" VelocityUnit = "mm/s" / >
        < / cfg:AnalogOut2>
        <cfg:PulseLength DefaultOutput = "0" Unit = "s">
        < cfg:Shift Unit = "s">0 < / cfg:Shift >
        <cfg:RadiusFactor Enabled = "false" RadiusUnit = "mm" / >
        <cfg:VelocityFactor Enabled = "false" VelocityUnit = "mm/s" / >
        < / cfg:PulseLength>
        <cfg:HalfPeriod DefaultOutput = "0" Unit = "s">
        < cfg:Shift Unit = "s">0 < / cfg:Shift >
        <cfg:RadiusFactor Enabled = "false" RadiusUnit = "mm" / >
        <cfg:VelocityFactor Enabled = "false" VelocityUnit = "mm/s" / >
        < / cfg:HalfPeriod>
        <cfg:SpotDistance DefaultOutput = "0.005" Unit = "mm">
        <cfg:RadiusFactor Enabled = "false" RadiusUnit = "mm" / >
        <cfg:VelocityFactor Enabled = "false" VelocityUnit = "mm/s" / >
        < / cfg:SpotDistance>
        < / cfg:AutomaticLaserControl>
*/

    auto pIOConfigNode = pConfigurationNode->AddChild("", "IOConfig");

    auto pDefaultOutputsNode = pIOConfigNode->AddChild("", "DefaultOutputs");
    auto pDefaultOutputsLaserPinOutNode = pDefaultOutputsNode->AddChild("", "LaserPinOut");
    pDefaultOutputsLaserPinOutNode->AddAttribute("", "Format", "Bitpattern");
    pDefaultOutputsLaserPinOutNode->AddAttribute("", "Value", "1");
    auto pDefaultOutputsAnalogOut1Node = pDefaultOutputsNode->AddChild("", "AnalogOut1");
    pDefaultOutputsAnalogOut1Node->AddAttribute("", "Format", "Factor");
    pDefaultOutputsAnalogOut1Node->AddAttribute("", "Value", "0.5");
    auto pDefaultOutputsAnalogOut2Node = pDefaultOutputsNode->AddChild("", "AnalogOut2");
    pDefaultOutputsAnalogOut2Node->AddAttribute("", "Format", "Factor");
    pDefaultOutputsAnalogOut2Node->AddAttribute("", "Value", "0.5"); 

    auto pLaserInitSequenceNode = pIOConfigNode->AddChild("", "LaserInitSequence");
    auto pLaserInitSequenceAnalogOut1Node = pLaserInitSequenceNode->AddChild("", "SetAnalogOut1");
    pLaserInitSequenceAnalogOut1Node->AddAttribute("", "Format", "Factor");
    pLaserInitSequenceAnalogOut1Node->AddAttribute("", "Value", "0.5");
    auto pLaserInitSequenceAnalogOut2Node = pLaserInitSequenceNode->AddChild("", "SetAnalogOut2");
    pLaserInitSequenceAnalogOut2Node->AddAttribute("", "Format", "Factor");
    pLaserInitSequenceAnalogOut2Node->AddAttribute("", "Value", "0.5");
    auto pLaserInitSequencePinOutNode = pLaserInitSequenceNode->AddChild("", "SetLaserPinOut");
    pLaserInitSequencePinOutNode->AddAttribute("", "Format", "Bitpattern");
    pLaserInitSequencePinOutNode->AddAttribute("", "Value", "1");

    auto pLaserShutdownSequenceNode = pIOConfigNode->AddChild("", "LaserShutdownSequence");
    auto pLaserShutdownSequencePinOutNode = pLaserShutdownSequenceNode->AddChild("", "SetLaserPinOut");
    pLaserShutdownSequencePinOutNode->AddAttribute("", "Format", "Bitpattern");
    pLaserShutdownSequencePinOutNode->AddAttribute("", "Value", "0");
    auto pLaserShutdownSequenceAnalogOut2Node = pLaserShutdownSequenceNode->AddChild("", "SetAnalogOut2");
    pLaserShutdownSequenceAnalogOut2Node->AddAttribute("", "Format", "Factor");
    pLaserShutdownSequenceAnalogOut2Node->AddAttribute("", "Value", "0");
    auto pLaserShutdownSequenceAnalogOut1Node = pLaserShutdownSequenceNode->AddChild("", "SetAnalogOut1");
    pLaserShutdownSequenceAnalogOut1Node->AddAttribute("", "Format", "Factor");
    pLaserShutdownSequenceAnalogOut1Node->AddAttribute("", "Value", "0");


    auto pTrajectoryConfigNode = pConfigurationNode->AddChild("", "TrajectoryConfig");
    auto pTrajectoryMarkConfigNode = pTrajectoryConfigNode->AddChild("", "MarkConfig");
    pTrajectoryMarkConfigNode->AddAttribute("", "VelocityUnit", "mm/s");

    auto pTrajectoryJumpSpeedNode = pTrajectoryMarkConfigNode->AddChildText("", "JumpSpeed", "400");
    pTrajectoryJumpSpeedNode->AddAttribute("", "Unit", "mm/s");

    auto pTrajectoryMarkSpeedNode = pTrajectoryMarkConfigNode->AddChildText("", "MarkSpeed", "400");
    pTrajectoryMarkSpeedNode->AddAttribute("", "Unit", "mm/s");

    auto pTrajectoryMinimalMarkSpeedNode = pTrajectoryMarkConfigNode->AddChildText("", "MinimalMarkSpeed", "50");
    pTrajectoryMinimalMarkSpeedNode->AddAttribute("", "Unit", "mm/s");

    auto pTrajectoryLaserSwitchConfigNode = pTrajectoryMarkConfigNode->AddChild("", "LaserSwitchConfig");
    pTrajectoryLaserSwitchConfigNode->AddAttribute("", "Unit", "s");

    auto pLaserPreTriggerTimeNode = pTrajectoryLaserSwitchConfigNode->AddChildText("", "LaserPreTriggerTime", "0");
    pLaserPreTriggerTimeNode->AddAttribute("", "Unit", "s");
    auto pLaserSwitchOffsetTimeNode = pTrajectoryLaserSwitchConfigNode->AddChildText("", "LaserSwitchOffsetTime", "-2E-05");
    pLaserSwitchOffsetTimeNode->AddAttribute("", "Unit", "s");
    auto pLaserMinOffTimeNode = pTrajectoryLaserSwitchConfigNode->AddChildText("", "LaserMinOffTime", "1.5625E-08");
    pLaserMinOffTimeNode->AddAttribute("", "Unit", "s");

    auto pTrajectoryGeometryConfigNode = pTrajectoryConfigNode->AddChild("", "GeometryConfig");
    auto pTrajectoryCornerToleranceNode = pTrajectoryGeometryConfigNode->AddChildText("", "CornerTolerance", "0.5");
    pTrajectoryCornerToleranceNode->AddAttribute("", "Unit", "mm");

    auto pTrajectoryLineToleranceNode = pTrajectoryGeometryConfigNode->AddChildText("", "LineTolerance", "0.0125");
    pTrajectoryLineToleranceNode->AddAttribute("", "Unit", "mm");

    pTrajectoryGeometryConfigNode->AddChildText("", "BlendMode", "SwiftBlending");

    std::string sXMLString = pXMLDocument->SaveToString (true);

    //std::cout << sXMLString << std::endl;
    
    //m_pDriverEnvironment->Sleep(100000);

    return sXMLString;
}