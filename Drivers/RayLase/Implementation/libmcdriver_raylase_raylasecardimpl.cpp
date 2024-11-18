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


using namespace LibMCDriver_Raylase::Impl;

#define MINLASERPOWER 0.1

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

    if (dMaxLaserPowerInWatts < MINLASERPOWER)
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDLASERPOWER);

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

    m_pSDK->checkError(m_pSDK->rlSystemResetToDefaults(m_Handle));
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
        bool bIsArmed = false;
        m_pSDK->checkError(m_pSDK->rlLaserArmLaser(m_Handle, bIsArmed));

        return bIsArmed;
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

    bool bPilotIsEnabled = false;
    m_pSDK->checkError(m_pSDK->rlLaserIsPilotEnabled(m_Handle, bPilotIsEnabled));

    return bPilotIsEnabled;
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
            bool bInProgress = false;
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


LibMCEnv::PDriverEnvironment CRaylaseCardImpl::getDriverEnvironment()
{
    return m_pDriverEnvironment;
}

PRaylaseCardList CRaylaseCardImpl::createNewList()
{
    return std::make_shared<CRaylaseCardList>(m_pSDK, m_Handle, m_dMaxLaserPowerInWatts);
}

void CRaylaseCardImpl::abortListExecution()
{
    bool bInProgress = false;
    m_pSDK->checkError(m_pSDK->rlListIsExecutionInProgress(m_Handle, bInProgress));

    if (bInProgress)
        m_pSDK->checkError(m_pSDK->rlListAbortExecution(m_Handle));
}

