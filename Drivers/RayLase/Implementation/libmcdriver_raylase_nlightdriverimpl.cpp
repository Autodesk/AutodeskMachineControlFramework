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


Abstract: This is a stub class definition of CNLightDriverBoard

*/

#include "libmcdriver_raylase_nlightdriverimpl.hpp"
#include "libmcdriver_raylase_interfaceexception.hpp"

#include "libmcdriver_raylase_apifield.hpp"
#include "../SDKSchema/libmcdriver_raylase_spiconfig_3.hpp"

#include <cmath>


using namespace LibMCDriver_Raylase::Impl;


CNLightDriverImpl::CNLightDriverImpl(PRaylaseSDK pSDK, LibMCEnv::PDriverEnvironment pDriverEnvironment)
    : m_pSDK (pSDK), 
    m_pDriverEnvironment (pDriverEnvironment), 
    m_nModeChangeSignalDelayInMicroseconds (10000),
    m_nModeChangeApplyDelayInMicroseconds (100000),
    m_AutomaticLaserModesEnabled (false),
    m_nSPITimeoutInMilliseconds (10),
    m_nSPIModuleIndex (2),
    m_nSPIRetryCount (5),
    m_nLaserReactionDelayInMilliseconds (100),
    m_nLaserReactionRetries (100)
{
    if (pSDK.get() == nullptr)
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDPARAM);
    if (pDriverEnvironment.get() == nullptr)
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDPARAM);
}

CNLightDriverImpl::~CNLightDriverImpl()
{

}


void CNLightDriverImpl::initializeNLightLaser(rlHandle cardHandle)
{

    m_pDriverEnvironment->LogMessage("Initializate nLights SPI mode..");
    CRaylaseAPIField_rlSpiConfig_v3 spiConfig;
    m_pDriverEnvironment->LogMessage("Init config..");
    m_pSDK->checkError(m_pSDK->rlSfioSpiInitConfig((rlScannerConfig*)spiConfig.getData()));

    m_pDriverEnvironment->LogMessage("Get SPI config..");

    m_pSDK->checkError(m_pSDK->rlSfioSpiGetConfig(cardHandle, (rlScannerConfig*)spiConfig.getData()));
    spiConfig.setBool("Module2.Enabled", true);
    spiConfig.setEnumRaw("Module2.BitOrder", 0 ); // "MsbFirst" 
    spiConfig.setDouble("Module2.PreDelay", 0.5);
    spiConfig.setDouble("Module2.PostDelay", 0.5);
    spiConfig.setDouble("Module2.FrameDelay", 0.25);
    spiConfig.setEnumRaw("Module2.SpiSyncMode", 1); // "SyncPerFrame"
    spiConfig.setDouble("Module2.ClockPeriod", 0.125);
    spiConfig.setInteger("Module2.BitsPerWord", 32); 

    /*
    
    uint32_t nSPICount = spiConfig.getVariableCount();
    for (uint32_t nSPIIndex = 0; nSPIIndex < nSPICount; nSPIIndex++) {
        std::string sName = spiConfig.getVariableName(nSPIIndex);
        m_pDriverEnvironment->LogMessage("SPI Variable " + sName + " = " + spiConfig.getVariableValueAsString(sName));
    } */

    uint32_t nRetryCount;

    m_pDriverEnvironment->LogMessage("Set SPI config..");
    m_pSDK->checkError(m_pSDK->rlSfioSpiSetConfig(cardHandle, (rlSpiConfig*)spiConfig.getData()));
    m_pDriverEnvironment->LogMessage("Set SPI config done.."); 

    m_pDriverEnvironment->LogMessage("Enabling nLight 24V...");
    m_pSDK->checkError(m_pSDK->rlGpioWrite(cardHandle, eRLIOPort::ioPortD, eRLPinAction::paSet, (uint32_t)eNlightDriverBoardIOPins::ENABLE_24V));

    m_pDriverEnvironment->LogMessage("Clearing nLight laser flags...");
    uint32_t nClearFlags = (uint32_t)eNlightDriverBoardIOPins::ARM_LASER | (uint32_t)eNlightDriverBoardIOPins::ENABLE_AIMING_LASER | (uint32_t)eNlightDriverBoardIOPins::GATE_IN;
    m_pSDK->checkError(m_pSDK->rlGpioWrite(cardHandle, eRLIOPort::ioPortD, eRLPinAction::paClear, nClearFlags));

    m_pDriverEnvironment->LogMessage("Disarming laser..");
    m_pSDK->checkError(m_pSDK->rlLaserArmLaser(cardHandle, false));

    m_pDriverEnvironment->Sleep(3000);

    m_pDriverEnvironment->LogMessage("Waiting for firmware ready...");

    nRetryCount = m_nLaserReactionRetries;
    while (nRetryCount > 0) {
        m_pDriverEnvironment->Sleep(m_nLaserReactionDelayInMilliseconds);
        if (laserIsFirmwareReady(cardHandle))
            break;

        nRetryCount--;
    }
    
    if (nRetryCount == 0)
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_NLIGHTFIRMWAREISNOTREADY);

    m_pDriverEnvironment->LogMessage("Enabling nLight external control");
    m_pSDK->checkError(m_pSDK->rlGpioWrite(cardHandle, eRLIOPort::ioPortD, eRLPinAction::paSet, (uint32_t)eNlightDriverBoardIOPins::ENABLE_EXTERNAL_CONTROL));

    m_pDriverEnvironment->Sleep(3000);

    nRetryCount = m_nLaserReactionRetries;
    while (nRetryCount > 0) {
        m_pDriverEnvironment->Sleep(m_nLaserReactionDelayInMilliseconds);
        if (laserExternalControlIsReady(cardHandle))
            break;

        nRetryCount--;
    }

    if (nRetryCount == 0)
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_NLIGHTEXTERNALCONTROLNOTREADY);

    m_pDriverEnvironment->Sleep(3000);

    m_pDriverEnvironment->LogMessage("Setting nLight system on..");
    m_pSDK->checkError(m_pSDK->rlGpioWrite(cardHandle, eRLIOPort::ioPortD, eRLPinAction::paClear, (uint32_t)eNlightDriverBoardIOPins::SYSTEM_ON));
    //m_pDriverEnvironment->Sleep(m_nLaserReactionDelayInMilliseconds);
    m_pDriverEnvironment->Sleep(2000);

    m_pSDK->checkError(m_pSDK->rlGpioWrite(cardHandle, eRLIOPort::ioPortD, eRLPinAction::paSet, (uint32_t)eNlightDriverBoardIOPins::SYSTEM_ON));

    m_pDriverEnvironment->Sleep(2000);

    nRetryCount = m_nLaserReactionRetries;
    while (nRetryCount > 0) {
        m_pDriverEnvironment->Sleep(m_nLaserReactionDelayInMilliseconds);
        if (laserIsReady(cardHandle))
            break;

        nRetryCount--;
    }

    //if (nRetryCount == 0)
        //throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_NLIGHTLASERNOTREADYAFTERSYSTEMON);

    m_pDriverEnvironment->Sleep(2000);

    m_pDriverEnvironment->LogMessage("Resetting beam profile");
    setNLightLaserMode(cardHandle, 0);

    m_pDriverEnvironment->LogMessage("nLight Initialization done..");
}

void CNLightDriverImpl::disableNLightLaser(rlHandle cardHandle)
{
    m_pDriverEnvironment->LogMessage("Resetting beam profile");
    setNLightLaserMode(cardHandle, 0);

    m_pDriverEnvironment->LogMessage("Disarming laser..");
    m_pSDK->checkError(m_pSDK->rlLaserArmLaser(cardHandle, false));

    m_pDriverEnvironment->LogMessage("Clearing nLight laser flags...");
    uint32_t nClearFlags = (uint32_t)eNlightDriverBoardIOPins::ARM_LASER | (uint32_t)eNlightDriverBoardIOPins::ENABLE_AIMING_LASER | (uint32_t)eNlightDriverBoardIOPins::GATE_IN;
    m_pSDK->checkError(m_pSDK->rlGpioWrite(cardHandle, eRLIOPort::ioPortD, eRLPinAction::paClear, nClearFlags));
    m_pDriverEnvironment->Sleep(1000);

    m_pDriverEnvironment->LogMessage("Disabling nLight external control");
    m_pSDK->checkError(m_pSDK->rlGpioWrite(cardHandle, eRLIOPort::ioPortD, eRLPinAction::paSet, (uint32_t)eNlightDriverBoardIOPins::ENABLE_EXTERNAL_CONTROL));
    m_pDriverEnvironment->Sleep(1000);

    m_pDriverEnvironment->LogMessage("Disabling nLight 24V...");
    m_pSDK->checkError(m_pSDK->rlGpioWrite(cardHandle, eRLIOPort::ioPortD, eRLPinAction::paClear, (uint32_t)eNlightDriverBoardIOPins::ENABLE_24V));

    m_pDriverEnvironment->LogMessage("nLight Laser deinitialized..");

    m_AutomaticLaserModesEnabled = false;

}


void CNLightDriverImpl::clearNLightError(rlHandle cardHandle)
{
    uint32_t nSignalDelayInMS = (m_nModeChangeSignalDelayInMicroseconds + 999) / 1000;


    m_pDriverEnvironment->LogMessage("Disarming laser..");
    m_pSDK->checkError(m_pSDK->rlLaserArmLaser(cardHandle, false));

    m_pDriverEnvironment->LogMessage("Clearing nLight Error");
    m_pSDK->checkError(m_pSDK->rlGpioWrite(cardHandle, eRLIOPort::ioPortD, eRLPinAction::paClear, (uint32_t)eNlightDriverBoardIOPins::CLEAR_ERROR));
    m_pDriverEnvironment->Sleep(nSignalDelayInMS);
    m_pSDK->checkError(m_pSDK->rlGpioWrite(cardHandle, eRLIOPort::ioPortD, eRLPinAction::paSet, (uint32_t)eNlightDriverBoardIOPins::CLEAR_ERROR));
    m_pDriverEnvironment->Sleep(nSignalDelayInMS);

    uint32_t nRetryCount = m_nLaserReactionRetries;
    while (nRetryCount > 0) {
        m_pDriverEnvironment->Sleep(m_nLaserReactionDelayInMilliseconds);
        if (!laserHasError(cardHandle))
            break;

        nRetryCount--;
    }

    m_pSDK->checkError(m_pSDK->rlGpioWrite(cardHandle, eRLIOPort::ioPortD, eRLPinAction::paClear, (uint32_t)eNlightDriverBoardIOPins::CLEAR_ERROR));
    m_pDriverEnvironment->Sleep(nSignalDelayInMS);

    if (nRetryCount == 0)
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_NLIGHTEXTERNALCONTROLNOTREADY);


}


uint32_t CNLightDriverImpl::retrieveSPIStatus(rlHandle cardHandle)
{
    std::vector<uint32_t> inputBuffer;
    std::vector<uint32_t> outputBuffer;

    inputBuffer.push_back(RAYLASE_NLIGHT_SPISTATUSREQUESTCOMMAND);
    outputBuffer.push_back(0);
    outputBuffer.push_back(0);
    outputBuffer.push_back(0);
    outputBuffer.push_back(0);

    for (uint32_t nRetryIndex = 0; nRetryIndex < m_nSPIRetryCount; nRetryIndex++) {

        uint32_t nReceiveCount = 0;

        rlResult nErrorCode = m_pSDK->rlSfioSpiTransceive(cardHandle, m_nSPIModuleIndex, inputBuffer.data(), (uint32_t)inputBuffer.size(), m_nSPITimeoutInMilliseconds, outputBuffer.data(), (uint32_t)outputBuffer.size(), nReceiveCount);
        if (nErrorCode == 0) {
            if (nReceiveCount == 1) {
                return outputBuffer.at(0);
            }
        }

    }

    throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_COULDNOTRECEIVESPIPACKET);

}


void CNLightDriverImpl::setNLightLaserMode(rlHandle cardHandle, uint32_t nLaserMode)
{
    // Always round the delays up to the next Millisecond!
    uint32_t nSignalDelayInMS = (m_nModeChangeSignalDelayInMicroseconds + 999) / 1000;
    uint32_t nApplyDelayInMS = (m_nModeChangeApplyDelayInMicroseconds + 999) / 1000;

    if (nLaserMode > RAYLASE_NLIGHT_MAXLASERMODE)
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDNLIGHTLASERMODE, "Invalid nLight laser mode: " + std::to_string(nLaserMode));

    uint32_t nSetMask = (uint32_t)eNlightDriverBoardIOPins::PRO_B7;
    uint32_t nClearMask = (uint32_t)eNlightDriverBoardIOPins::ENABLE_PROFILE;

    if ((nLaserMode & 1) != 0)
        nSetMask |= (uint32_t)eNlightDriverBoardIOPins::PRO_B1;
    else
        nClearMask |= (uint32_t)eNlightDriverBoardIOPins::PRO_B1;

    if ((nLaserMode & 2) != 0)
        nSetMask |= (uint32_t)eNlightDriverBoardIOPins::PRO_B2;
    else
        nClearMask |= (uint32_t)eNlightDriverBoardIOPins::PRO_B2;

    if ((nLaserMode & 4) != 0)
        nSetMask |= (uint32_t)eNlightDriverBoardIOPins::PRO_B3;
    else
        nClearMask |= (uint32_t)eNlightDriverBoardIOPins::PRO_B3;

    if ((nLaserMode & 8) != 0)
        nSetMask |= (uint32_t)eNlightDriverBoardIOPins::PRO_B4;
    else
        nClearMask |= (uint32_t)eNlightDriverBoardIOPins::PRO_B4;

    m_pSDK->checkError(m_pSDK->rlGpioWrite(cardHandle, eRLIOPort::ioPortD, eRLPinAction::paSet, nSetMask));
    m_pDriverEnvironment->Sleep(nSignalDelayInMS);
    m_pSDK->checkError(m_pSDK->rlGpioWrite(cardHandle, eRLIOPort::ioPortD, eRLPinAction::paClear, nClearMask));
    m_pDriverEnvironment->Sleep(nSignalDelayInMS);
    m_pSDK->checkError(m_pSDK->rlGpioWrite(cardHandle, eRLIOPort::ioPortD, eRLPinAction::paSet, (uint32_t)eNlightDriverBoardIOPins::PRO_START));
    m_pDriverEnvironment->Sleep(nSignalDelayInMS);
    m_pSDK->checkError(m_pSDK->rlGpioWrite(cardHandle, eRLIOPort::ioPortD, eRLPinAction::paClear, (uint32_t)eNlightDriverBoardIOPins::PRO_START));
    m_pDriverEnvironment->Sleep(nApplyDelayInMS);

}


void CNLightDriverImpl::addNLightLaserModeToList(rlListHandle listHandle, uint32_t nLaserMode)
{
    double dSignalDelay = (double)m_nModeChangeSignalDelayInMicroseconds;
    double dApplyDelay = (double)m_nModeChangeApplyDelayInMicroseconds;

    if (nLaserMode > RAYLASE_NLIGHT_MAXLASERMODE)
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDNLIGHTLASERMODE, "Invalid nLight laser mode: " + std::to_string(nLaserMode));

    uint32_t nSetMask = (uint32_t)eNlightDriverBoardIOPins::PRO_B7;
    uint32_t nClearMask = (uint32_t)eNlightDriverBoardIOPins::ENABLE_PROFILE;

    if ((nLaserMode & 1) != 0)
        nSetMask |= (uint32_t)eNlightDriverBoardIOPins::PRO_B1;
    else
        nClearMask |= (uint32_t)eNlightDriverBoardIOPins::PRO_B1;

    if ((nLaserMode & 2) != 0)
        nSetMask |= (uint32_t)eNlightDriverBoardIOPins::PRO_B2;
    else
        nClearMask |= (uint32_t)eNlightDriverBoardIOPins::PRO_B2;

    if ((nLaserMode & 4) != 0)
        nSetMask |= (uint32_t)eNlightDriverBoardIOPins::PRO_B3;
    else
        nClearMask |= (uint32_t)eNlightDriverBoardIOPins::PRO_B3;

    if ((nLaserMode & 8) != 0)
        nSetMask |= (uint32_t)eNlightDriverBoardIOPins::PRO_B4;
    else
        nClearMask |= (uint32_t)eNlightDriverBoardIOPins::PRO_B4;

    m_pSDK->checkError(m_pSDK->rlListAppendGpioValue(listHandle, eRLIOPort::ioPortD, eRLPinAction::paSet, nSetMask));
    m_pSDK->checkError(m_pSDK->rlListAppendSleep(listHandle, dSignalDelay));
    m_pSDK->checkError(m_pSDK->rlListAppendGpioValue(listHandle, eRLIOPort::ioPortD, eRLPinAction::paClear, nClearMask));
    m_pSDK->checkError(m_pSDK->rlListAppendSleep(listHandle, dSignalDelay));
    m_pSDK->checkError(m_pSDK->rlListAppendGpioValue(listHandle, eRLIOPort::ioPortD, eRLPinAction::paSet, (uint32_t)eNlightDriverBoardIOPins::PRO_START));
    m_pSDK->checkError(m_pSDK->rlListAppendSleep(listHandle, dSignalDelay));
    m_pSDK->checkError(m_pSDK->rlListAppendGpioValue(listHandle, eRLIOPort::ioPortD, eRLPinAction::paClear, (uint32_t)eNlightDriverBoardIOPins::PRO_START));
    m_pSDK->checkError(m_pSDK->rlListAppendSleep(listHandle, dApplyDelay));
}

bool CNLightDriverImpl::automaticLaserModesAreEnabled()
{
    return m_AutomaticLaserModesEnabled;
}

void CNLightDriverImpl::setAutomaticLaserModesEnable(bool bValue)
{
    m_AutomaticLaserModesEnabled = bValue;
}

void CNLightDriverImpl::setModeChangeDelays(uint32_t nModeChangeSignalDelayInMicroseconds, uint32_t nModeChangeApplyDelayInMicroseconds)
{
    if ((nModeChangeSignalDelayInMicroseconds == 0) || (nModeChangeSignalDelayInMicroseconds > RAYLASE_NLIGHT_MAXCHANGEMODEDELAY)) {
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDNLIGHTMODECHANGESIGNALDELAY, "Invalid nLight Mode change signal delay: " + std::to_string (nModeChangeSignalDelayInMicroseconds));
    }

    if ((nModeChangeApplyDelayInMicroseconds == 0) || (nModeChangeApplyDelayInMicroseconds > RAYLASE_NLIGHT_MAXCHANGEMODEDELAY)) {
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDNLIGHTMODECHANGEAPPLYDELAY, "Invalid nLight Mode change apply delay: " + std::to_string(nModeChangeApplyDelayInMicroseconds));
    }

    m_nModeChangeSignalDelayInMicroseconds = nModeChangeSignalDelayInMicroseconds;
    m_nModeChangeApplyDelayInMicroseconds = nModeChangeApplyDelayInMicroseconds;

}

uint32_t CNLightDriverImpl::getModeChangeSignalDelay()
{
    return m_nModeChangeSignalDelayInMicroseconds;
}

uint32_t CNLightDriverImpl::getModeChangeApplyDelay()
{
    return m_nModeChangeApplyDelayInMicroseconds;
}



uint32_t CNLightDriverImpl::getMaxAFXMode()
{
    return RAYLASE_NLIGHT_MAXLASERMODE;
}



bool CNLightDriverImpl::laserHasError(rlHandle cardHandle)
{
    uint32_t nStatus = retrieveSPIStatus(cardHandle);
    return ((nStatus & (1UL << RAYLASE_NLIGHT_ERRORBIT)) != 0);
}

bool CNLightDriverImpl::laserIsReady(rlHandle cardHandle)
{
    uint32_t nStatus = retrieveSPIStatus(cardHandle);
    return ((nStatus & (1UL << RAYLASE_NLIGHT_READYBIT)) != 0);

}

bool CNLightDriverImpl::laserExternalControlIsReady(rlHandle cardHandle)
{
    uint32_t nStatus = retrieveSPIStatus(cardHandle);
    return ((nStatus & (1UL << RAYLASE_NLIGHT_EXTCONTROLREADYBIT)) != 0);
}

bool CNLightDriverImpl::laserIsEmission(rlHandle cardHandle)
{
    uint32_t nStatus = retrieveSPIStatus(cardHandle);
    return ((nStatus & (1UL << RAYLASE_NLIGHT_EMISSIONBIT)) != 0);
}

bool CNLightDriverImpl::laserIsFirmwareReady(rlHandle cardHandle)
{
    uint32_t nStatus = retrieveSPIStatus(cardHandle);
    return ((nStatus & (1UL << RAYLASE_NLIGHT_FIRMWAREREADYBIT)) != 0);

}

bool CNLightDriverImpl::laserIsWaterFlow(rlHandle cardHandle)
{
    uint32_t nStatus = retrieveSPIStatus(cardHandle);
    return ((nStatus & (1UL << RAYLASE_NLIGHT_EXTWATERFLOWBIT)) != 0);

}

bool CNLightDriverImpl::laserBPPIsReady(rlHandle cardHandle)
{
    uint32_t nStatus = retrieveSPIStatus(cardHandle);
    return ((nStatus & (1UL << RAYLASE_NLIGHT_BPPREADYBIT)) != 0);
    
}

void CNLightDriverImpl::setLaserModeMaxPowerOverride(const uint32_t nLaserMode, const double dMaxPowerInWatts)
{
    if ((dMaxPowerInWatts < RAYLASE_MINLASERPOWER) || (dMaxPowerInWatts > RAYLASE_MAXLASERPOWER))
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDLASERPOWER, "invalid laser power: " + std::to_string(dMaxPowerInWatts));

    if ((nLaserMode == 0) || (nLaserMode > RAYLASE_NLIGHT_MAXLASERMODE))
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDLASERMODE, "invalid laser power: " + std::to_string(nLaserMode));

    m_LaserModeMaxPowerOverrides.insert(std::make_pair (nLaserMode, dMaxPowerInWatts));
}

bool CNLightDriverImpl::hasLaserModeMaxPowerOverride(const uint32_t nLaserMode)
{
    auto iIter = m_LaserModeMaxPowerOverrides.find(nLaserMode);
    return (iIter != m_LaserModeMaxPowerOverrides.end());

}

double CNLightDriverImpl::getLaserModeMaxPowerOverride(const uint32_t nLaserMode)
{
    auto iIter = m_LaserModeMaxPowerOverrides.find(nLaserMode);
    if (iIter == m_LaserModeMaxPowerOverrides.end ())
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_NLIGHTLASERMODEHASNOPOWEROVERRIDE, "nLight Laser Mode has no power override: " + std::to_string(nLaserMode));

    return iIter->second;
}

void CNLightDriverImpl::clearLaserModeMaxPowerOverride(const uint32_t nLaserMode)
{
    m_LaserModeMaxPowerOverrides.erase(nLaserMode);
}

void CNLightDriverImpl::clearAllLaserModeMaxPowerOverrides()
{
    m_LaserModeMaxPowerOverrides.clear();
}
