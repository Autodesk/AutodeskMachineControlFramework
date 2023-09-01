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
    std::string sBaseDirectoryPath = pWorkingDirectory->GetAbsoluteFilePath();

    auto pXMLDocument = m_pDriverEnvironment->CreateXMLDocument("Configuration", "SCANmotionControl");

    pXMLDocument->ChangeNamespacePrefix("", "cfg");
    pXMLDocument->RegisterNamespace("http://www.w3.org/2001/XMLSchema-instance", "xsi");

    auto pConfigurationNode = pXMLDocument->GetRootNode();
    pConfigurationNode->AddAttribute("", "Version", "0.6");
    pConfigurationNode->AddAttribute("http://www.w3.org/2001/XMLSchema-instance", "schemaLocation", "cfg SCANmotionControl_0_6.xsd");

    auto pGeneralConfigNode = pConfigurationNode->AddChild("", "GeneralConfig");
    auto pDynamicViolationReaction = pGeneralConfigNode->AddChildText("", "DynamicViolationReaction", "WarningOnly");
    
    auto pLogConfigNode = pGeneralConfigNode->AddChild("", "LogConfig");
    pLogConfigNode->AddChildText("", "LogfilePath", "[BaseDirectoryPath]/Log/Log.txt");
    pLogConfigNode->AddChildText("", "Loglevel", "Warn");
    pLogConfigNode->AddChildText("", "EnableConsoleLogging", "false");
    pLogConfigNode->AddChildText("", "EnableFilelogging", "false");
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
    pRTCConfigNode->AddChildText("", "ProgramFileDirectory", "[BaseDirectoryPath]/ProgramFiles");
    auto pBoardsNode = pRTCConfigNode->AddChild("", "Boards");

    auto pEthSearchNode = pRTCConfigNode->AddChild("", "EthSearch");
    auto pIPListNode = pEthSearchNode->AddChild("", "IPList");
    pIPListNode->AddChildText("", "IPAddress", "192.168.0.1");
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
    auto pFocalLengthNode = pScanDeviceConfigNode->AddChildText("", "FocalLength", "100");
    pFocalLengthNode->AddAttribute("", "Unit", "mm");

    auto pDelayNode = pScanDeviceConfigNode->AddChildText("", "Delay", "0.00125");
    pDelayNode->AddAttribute("", "Unit", "s");

    auto pScanDeviceListNode = pScanDeviceConfigNode->AddChild("", "ScanDeviceList");

    pScanDeviceConfigNode->AddChildText("", "DefaultCorrectionFile", "0");

    auto pLaserConfigNode = pConfigurationNode->AddChild("", "LaserConfig");
    pLaserConfigNode->AddChildText("", "LaserMode", "5");

    auto pIOConfigNode = pConfigurationNode->AddChild("", "IOConfig");

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

    auto pTrajectoryLineToleranceNode = pTrajectoryGeometryConfigNode->AddChildText("", "LineTolerance", "0.5");
    pTrajectoryLineToleranceNode->AddAttribute("", "Unit", "mm");

    pTrajectoryGeometryConfigNode->AddChildText("", "BlendMode", "SwiftBlending");

    std::string sXMLString = pXMLDocument->SaveToString (true);

    //std::cout << sXMLString << std::endl;
    
    //m_pDriverEnvironment->Sleep(100000);

    return sXMLString;
}