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


Abstract: This is a stub class definition of CRaylaseCard

*/

#include "libmcdriver_raylase_raylasecardimpl.hpp"
#include "libmcdriver_raylase_interfaceexception.hpp"
#include "libmcdriver_raylase_apifield.hpp"
#include "../SDKSchema/libmcdriver_raylase_processvariables_8.hpp"
#include "../SDKSchema/libmcdriver_raylase_laserconfig_8.hpp"
#include "../SDKSchema/libmcdriver_raylase_scannerconfig_12.hpp"
#include "../SDKSchema/libmcdriver_raylase_spiconfig_3.hpp"

using namespace LibMCDriver_Raylase::Impl;


PRaylaseCardImpl CRaylaseCardImpl::connectByIP(PRaylaseSDK pSDK, const std::string& sCardName, const std::string& sCardIP, uint32_t nPort, double dMaxLaserPowerInWatts, bool bSimulationMode, LibMCEnv::PDriverEnvironment pDriverEnvironment, LibMCEnv::PWorkingDirectory pWorkingDirectory)
{
    return std::make_shared<CRaylaseCardImpl>(pSDK, sCardName, sCardIP, nPort, dMaxLaserPowerInWatts,  bSimulationMode, pDriverEnvironment, pWorkingDirectory);
}

CRaylaseCardImpl::CRaylaseCardImpl(PRaylaseSDK pSDK, const std::string& sCardName, const std::string& sCardIP, uint32_t nPort, double dMaxLaserPowerInWatts, bool bSimulationMode, LibMCEnv::PDriverEnvironment pDriverEnvironment, LibMCEnv::PWorkingDirectory pWorkingDirectory)
    : m_pSDK (pSDK), 
        m_sCardName (sCardName), 
        m_sCardIP (sCardIP), 
        m_nPort (nPort), 
        m_Handle (0), 
        m_bSimulationMode (bSimulationMode),
        m_bSimulatedPilotIsEnabled (false), 
        m_bSimulatedPilotIsArmed (false), 
        m_bSimulatedPilotIsAlarm (false),
        m_pDriverEnvironment (pDriverEnvironment), 
        m_pWorkingDirectory (pWorkingDirectory),
        m_dMaxLaserPowerInWatts (dMaxLaserPowerInWatts),
        m_nAssignedLaserIndex (0)

{

    if (pDriverEnvironment.get () == nullptr)
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDPARAM);
    if (pWorkingDirectory.get () == nullptr)
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDPARAM);

    m_pNLightDriverImpl = std::make_shared<CNLightDriverImpl>(m_pSDK, m_pDriverEnvironment);

    if ((dMaxLaserPowerInWatts < RAYLASE_MINLASERPOWER) || (dMaxLaserPowerInWatts > RAYLASE_MAXLASERPOWER))
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDLASERPOWER, "invalid laser power: " + std::to_string (dMaxLaserPowerInWatts));

    m_pCoordinateTransform = std::make_shared<CRaylaseCoordinateTransform>();

    if (!m_bSimulationMode) {
        if (pSDK.get() == nullptr)
            throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_SDKNOTINITIALIZED);

        m_Handle = m_pSDK->rlConnect(sCardIP.c_str(), nPort);
        if (m_Handle < 0)
            m_pSDK->checkError(m_Handle);

        //m_pSDK->ptrEnableCommandLogging (m_Handle, "d:/raylase.txt", -1);
    }
}

CRaylaseCardImpl::~CRaylaseCardImpl()
{
    Disconnect();

}

std::string CRaylaseCardImpl::getCardName()
{
    return m_sCardName;
}


void CRaylaseCardImpl::ResetToSystemDefaults()
{
    if (m_bSimulationMode)
        return;

    //m_pSDK->checkError(m_pSDK->rlSystemResetToDefaults(m_Handle));
    m_pSDK->checkError(m_pSDK->rlScannerLoadConfig(m_Handle));
    m_pSDK->checkError(m_pSDK->rlLaserLoadConfig(m_Handle));
    m_pSDK->checkError(m_pSDK->rlProcessLoadVariables(m_Handle));

    CRaylaseAPIField_rlLaserConfig_v8 laserConfig;
    m_pSDK->checkError(m_pSDK->rlLaserInitConfig((rlLaserConfig*)laserConfig.getData()));
    m_pSDK->checkError(m_pSDK->rlLaserGetConfig(m_Handle, (rlLaserConfig*)laserConfig.getData()));

    CRaylaseAPIField_rlProcessVariables_v17 processVariables;
    m_pSDK->checkError(m_pSDK->rlProcessInitVariables((rlProcessVariables*)processVariables.getData()));
    m_pSDK->checkError(m_pSDK->rlProcessGetVariables(m_Handle, (rlProcessVariables*)processVariables.getData()));

    CRaylaseAPIField_rlScannerConfig_v12 scannerConfig;
    m_pSDK->checkError(m_pSDK->rlScannerInitConfig((rlScannerConfig*)scannerConfig.getData()));
    m_pSDK->checkError(m_pSDK->rlScannerGetConfig(m_Handle, (rlScannerConfig*)scannerConfig.getData()));

    /*laserConfig.setDouble("FpsStart", 40.0);

    m_pSDK->checkError(m_pSDK->rlScannerSetConfig(m_Handle, (rlScannerConfig*)scannerConfig.getData()));
    m_pSDK->checkError(m_pSDK->rlScannerStoreConfig(m_Handle));
    m_pSDK->checkError(m_pSDK->rlScannerGetConfig(m_Handle, (rlScannerConfig*)scannerConfig.getData())); */


    m_pDriverEnvironment->LogMessage("Laser Config:");
    uint32_t nLaserCount = laserConfig.getVariableCount();
    for (uint32_t nIndex = 0; nIndex < nLaserCount; nIndex++) {
        std::string sName = laserConfig.getVariableName(nIndex);
        m_pDriverEnvironment->LogMessage(" -" + sName + ": " + laserConfig.getVariableValueAsString (sName));
    } 

    m_pDriverEnvironment->LogMessage("Process Config:");
    uint32_t nProcessCount = processVariables.getVariableCount();
    for (uint32_t nIndex = 0; nIndex < nProcessCount; nIndex++) {
        std::string sName = processVariables.getVariableName(nIndex);
        m_pDriverEnvironment->LogMessage(" -" + sName + ": " + processVariables.getVariableValueAsString(sName));
    }

    m_pDriverEnvironment->LogMessage("Scanner Config:");
    uint32_t nScannerCount = scannerConfig.getVariableCount();
    for (uint32_t nIndex = 0; nIndex < nScannerCount; nIndex++) {
        std::string sName = scannerConfig.getVariableName(nIndex);
        m_pDriverEnvironment->LogMessage(" -" + sName + ": " + scannerConfig.getVariableValueAsString(sName));
    }
    

}

void CRaylaseCardImpl::EnableCommandLogging()
{
    if (m_bSimulationMode)
        return;

    auto pUtils = m_pDriverEnvironment->CreateCryptoContext();
    auto sUUID = pUtils->CreateUUID();

    m_pLoggingFile = m_pWorkingDirectory->AddManagedFile("command_logging_" + sUUID);
    std::string sFileName = m_pLoggingFile->GetAbsoluteFileName();

    m_pSDK->checkError(m_pSDK->rlEnableCommandLogging(m_Handle, sFileName.c_str (), -1));

}

void CRaylaseCardImpl::DisableCommandLogging()
{
    m_pSDK->checkError(m_pSDK->rlDisableCommandLogging(m_Handle));

}


void CRaylaseCardImpl::LaserOn()
{
    if (m_bSimulationMode)
        return;

    m_pSDK->checkError(m_pSDK->rlLaserLaserOn(m_Handle));
}

void CRaylaseCardImpl::LaserOff()
{
    if (m_bSimulationMode)
        return;

    m_pSDK->checkError(m_pSDK->rlLaserLaserOff(m_Handle));
}

void CRaylaseCardImpl::ArmLaser(const bool bShallBeArmed)
{
    if (m_bSimulationMode) {
        m_bSimulatedPilotIsArmed = bShallBeArmed;
    }
    else {
        m_pSDK->checkError(m_pSDK->rlLaserArmLaser(m_Handle, bShallBeArmed));
    }

}

bool CRaylaseCardImpl::IsLaserArmed()
{
    if (m_bSimulationMode) {
        return m_bSimulatedPilotIsArmed;
    }
    else {
        uint32_t bIsArmed = 0;
        m_pSDK->checkError(m_pSDK->rlLaserIsLaserArmed(m_Handle, bIsArmed));

        return bIsArmed != 0;
    }
}

void CRaylaseCardImpl::EnablePilot(const bool bShallBeEnabled)
{
    if (m_bSimulationMode) {
        m_bSimulatedPilotIsEnabled = bShallBeEnabled;
    }
    else {

        m_pSDK->checkError(m_pSDK->rlLaserEnablePilot(m_Handle, bShallBeEnabled));
    }

}

bool CRaylaseCardImpl::PilotIsEnabled()
{
    if (m_bSimulationMode)
        return m_bSimulatedPilotIsEnabled;

    uint32_t bPilotIsEnabled = 0;
    m_pSDK->checkError(m_pSDK->rlLaserIsPilotEnabled(m_Handle, bPilotIsEnabled));

    return bPilotIsEnabled != 0;
}

void CRaylaseCardImpl::GetLaserStatus(bool& bPilotIsEnabled, bool& bLaserIsArmed, bool& bLaserAlarm)
{

    if (m_bSimulationMode) {
        bPilotIsEnabled = m_bSimulatedPilotIsEnabled;
        bLaserIsArmed = m_bSimulatedPilotIsArmed;
        bLaserAlarm = m_bSimulatedPilotIsAlarm;
        return;
    }

    uint32_t nStatusFlag = 0;
    m_pSDK->checkError(m_pSDK->rlLaserReadLaserStatus(m_Handle, nStatusFlag));

    bPilotIsEnabled = ((uint32_t)nStatusFlag & (uint32_t)eRLLaserStatus::lsPilot) != 0;
    bLaserIsArmed = ((uint32_t)nStatusFlag & (uint32_t)eRLLaserStatus::lsArmed) != 0;
    bLaserAlarm = ((uint32_t)nStatusFlag & (uint32_t)eRLLaserStatus::lsAlarm) != 0;

}


bool CRaylaseCardImpl::IsConnected()
{
    if (m_bSimulationMode)
        return true;

    return (m_Handle > 0);
}

void CRaylaseCardImpl::Disconnect()
{
    if (m_bSimulationMode)
        return;

    if (m_pSDK.get() != nullptr) {
        if (m_Handle > 0) {
            uint32_t bInProgress = 0;
            m_pSDK->rlListIsExecutionInProgress(m_Handle, bInProgress);

            if (bInProgress)
                m_pSDK->rlListAbortExecution(m_Handle);

            m_pSDK->rlDisconnect(m_Handle);
        }
        m_Handle = 0;
    }

}

void CRaylaseCardImpl::assignLaserIndex(uint32_t nLaserIndex)
{
    m_nAssignedLaserIndex = nLaserIndex;
}

uint32_t CRaylaseCardImpl::getAssignedLaserIndex()
{
    return m_nAssignedLaserIndex;
}

bool CRaylaseCardImpl::isSimulationMode()
{
    return m_bSimulationMode;
}

void CRaylaseCardImpl::addPartSuppression(const std::string& sPartUUID, const LibMCDriver_Raylase::ePartSuppressionMode eSuppressionMode)
{
    auto pUtils = m_pDriverEnvironment->CreateCryptoContext();
    std::string sNormalizedUUID = pUtils->NormalizeUUIDString(sPartUUID);

    if ((eSuppressionMode == LibMCDriver_Raylase::ePartSuppressionMode::NoPower) ||
        (eSuppressionMode == LibMCDriver_Raylase::ePartSuppressionMode::SkipPart)) {
        m_PartSuppressions.insert(std::make_pair(sNormalizedUUID, eSuppressionMode));
    }
    else {
        m_PartSuppressions.erase(sNormalizedUUID);
    }

}

void CRaylaseCardImpl::clearAllPartSuppressions()
{
    m_PartSuppressions.clear();
}

void CRaylaseCardImpl::removePartSuppression(const std::string& sPartUUID)
{
    auto pUtils = m_pDriverEnvironment->CreateCryptoContext();
    std::string sNormalizedUUID = pUtils->NormalizeUUIDString(sPartUUID);
    m_PartSuppressions.erase(sNormalizedUUID);
}

LibMCDriver_Raylase::ePartSuppressionMode CRaylaseCardImpl::getPartSuppressionMode(const std::string& sPartUUID)
{
    auto pUtils = m_pDriverEnvironment->CreateCryptoContext();
    std::string sNormalizedUUID = pUtils->NormalizeUUIDString(sPartUUID);

    auto iIter = m_PartSuppressions.find(sNormalizedUUID);
    if (iIter != m_PartSuppressions.end())
        return iIter->second;

    return LibMCDriver_Raylase::ePartSuppressionMode::DontSuppress;
}


LibMCEnv::PDriverEnvironment CRaylaseCardImpl::getDriverEnvironment()
{
    return m_pDriverEnvironment;
}

PRaylaseCardList CRaylaseCardImpl::createNewList()
{
    return std::make_shared<CRaylaseCardList>(m_pSDK, m_Handle, m_dMaxLaserPowerInWatts, m_pCoordinateTransform, m_PartSuppressions, m_pNLightDriverImpl);
}

void CRaylaseCardImpl::abortListExecution()
{
    uint32_t bInProgress = 0;
    m_pSDK->checkError(m_pSDK->rlListIsExecutionInProgress(m_Handle, bInProgress));

    if (bInProgress != 0)
        m_pSDK->checkError(m_pSDK->rlListAbortExecution(m_Handle));
}




PRaylaseCoordinateTransform CRaylaseCardImpl::getCoordinateTransform()
{
    return m_pCoordinateTransform;
}

rlHandle CRaylaseCardImpl::getHandle()
{
    return m_Handle;
}

PNLightDriverImpl CRaylaseCardImpl::getNlightImplementation()
{
    return m_pNLightDriverImpl;
}

double CRaylaseCardImpl::getMaxLaserPowerInWatts()
{
    return m_dMaxLaserPowerInWatts;
}

PRaylaseIOCycleImpl CRaylaseCardImpl::createIOCycle(uint32_t nCycleID)
{
    if (nCycleID == 0)
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDIOCYCLEID);

    auto iIter = m_IOCycles.find(nCycleID);
    if (iIter != m_IOCycles.end())
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_IOCYCLEALREADYEXISTS);

    auto pIOCycle = std::make_shared<CRaylaseIOCycleImpl>(nCycleID);
    m_IOCycles.insert(std::make_pair(nCycleID, pIOCycle));

    return pIOCycle;
}

bool CRaylaseCardImpl::ioCycleExists(uint32_t nCycleID) const
{
    auto iIter = m_IOCycles.find(nCycleID);
    return (iIter != m_IOCycles.end());
}

PRaylaseIOCycleImpl CRaylaseCardImpl::getIOCycle(uint32_t nCycleID) const
{
    auto iIter = m_IOCycles.find(nCycleID);
    if (iIter == m_IOCycles.end())
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_IOCYCLENOTFOUND);

    return iIter->second;
}

void CRaylaseCardImpl::removeIOCycle(uint32_t nCycleID)
{
    m_IOCycles.erase(nCycleID);
}
