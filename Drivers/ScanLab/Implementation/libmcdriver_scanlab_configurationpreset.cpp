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

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL AUTODESK INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "libmcdriver_scanlab_configurationpreset.hpp"
#include "libmcdriver_scanlab_interfaceexception.hpp"


using namespace LibMCDriver_ScanLab::Impl;

   



    double m_dMaxLaserPower;
    bool m_bFinishLaserPulseAfterOn;
    bool m_bPhaseShiftOfLaserSignal;
    bool m_bLaserOnSignalLowActive;
    bool m_bLaserHalfSignalsLowActive;
    bool m_bSetDigitalInOneHighActive;
    bool m_bOutputSynchronizationActive;

    double m_dLaserOnDelay;
    double m_dLaserOffDelay;
    double m_dMarkDelay;
    double m_dJumpDelay;
    double m_dPolygonDelay;


    CDriver_ScanLab_RTC6ConfigurationPreset::CDriver_ScanLab_RTC6ConfigurationPreset()
        : m_nTimeout (0), m_nSerialNumber (0), m_dInitialTimeout (0.0), m_dMaxTimeout (0.0), m_dMultiplier (0.0),
         m_LaserMode (LibMCDriver_ScanLab::eLaserMode::CO2), m_LaserPort (LibMCDriver_ScanLab::eLaserPort::Port12BitAnalog1),
         m_nCorrectionTableNumber (0), m_nCorrectionDimension (0), m_nCorrectionTableNumberHeadA (0), m_nCorrectionTableNumberHeadB (0),
        m_nLaserIndex (0)
    {

    }

    CDriver_ScanLab_RTC6ConfigurationPreset::~CDriver_ScanLab_RTC6ConfigurationPreset()
    {
    }

    std::string CDriver_ScanLab_RTC6ConfigurationPreset::getName()
    {
        return m_sName;
    }

    std::string CDriver_ScanLab_RTC6ConfigurationPreset::getIP()
    {
        return m_sIP;
    }

    std::string CDriver_ScanLab_RTC6ConfigurationPreset::getNetmask()
    {
        return m_sNetmask;
    }

    uint32_t CDriver_ScanLab_RTC6ConfigurationPreset::getTimeout()
    {
        return m_nTimeout;
    }

    uint32_t CDriver_ScanLab_RTC6ConfigurationPreset::getSerialNumber()
    {
        return m_nSerialNumber;
    }

    double CDriver_ScanLab_RTC6ConfigurationPreset::getInitialTimeout()
    {
        return m_dInitialTimeout;
    }

    double CDriver_ScanLab_RTC6ConfigurationPreset::getMaxTimeout()
    {
        return m_dMaxTimeout;
    }

    double CDriver_ScanLab_RTC6ConfigurationPreset::getMultiplier()
    {
        return m_dMultiplier;
    }


    std::string CDriver_ScanLab_RTC6ConfigurationPreset::getFirmwareResourceName()
    {
        return m_sFirmwareResourceName;
    }

    std::string CDriver_ScanLab_RTC6ConfigurationPreset::getFPGAResourceName()
    {
        return m_sFPGAResourceName;
    }

    std::string CDriver_ScanLab_RTC6ConfigurationPreset::getAuxiliaryResourceName()
    {
        return m_sAuxiliaryResourceName;
    }

    std::string CDriver_ScanLab_RTC6ConfigurationPreset::getCorrectionResourceName()
    {
        return m_sCorrectionResourceName;
    }

    uint32_t CDriver_ScanLab_RTC6ConfigurationPreset::getCorrectionTableNumber()
    {
        return m_nCorrectionTableNumber;
    }

    uint32_t CDriver_ScanLab_RTC6ConfigurationPreset::getCorrectionDimension()
    {
        return m_nCorrectionDimension;
    }

    uint32_t CDriver_ScanLab_RTC6ConfigurationPreset::getCorrectionTableNumberHeadA()
    {
        return m_nCorrectionTableNumberHeadA;
    }

    uint32_t CDriver_ScanLab_RTC6ConfigurationPreset::getCorrectionTableNumberHeadB()
    {
        return m_nCorrectionTableNumberHeadB;
    }


    LibMCDriver_ScanLab::eLaserMode CDriver_ScanLab_RTC6ConfigurationPreset::getLaserMode()
    {
        return m_LaserMode;
    }

    LibMCDriver_ScanLab::eLaserPort CDriver_ScanLab_RTC6ConfigurationPreset::getLaserPort()
    {
        return m_LaserPort;
    }

    double CDriver_ScanLab_RTC6ConfigurationPreset::getMaxLaserPower()
    {
        return m_dMaxLaserPower;
    }

    bool CDriver_ScanLab_RTC6ConfigurationPreset::getFinishLaserPulseAfterOn()
    {
        return m_bFinishLaserPulseAfterOn;
    }

    bool CDriver_ScanLab_RTC6ConfigurationPreset::getPhaseShiftOfLaserSignal()
    {
        return m_bPhaseShiftOfLaserSignal;
    }

    bool CDriver_ScanLab_RTC6ConfigurationPreset::getLaserOnSignalLowActive()
    {
        return m_bLaserOnSignalLowActive;
    }

    bool CDriver_ScanLab_RTC6ConfigurationPreset::getLaserHalfSignalsLowActive()
    {
        return m_bLaserHalfSignalsLowActive;
    }

    bool CDriver_ScanLab_RTC6ConfigurationPreset::getSetDigitalInOneHighActive()
    {
        return m_bSetDigitalInOneHighActive;
    }
    
    bool CDriver_ScanLab_RTC6ConfigurationPreset::getOutputSynchronizationActive()
    {
        return m_bOutputSynchronizationActive;
    }


    double CDriver_ScanLab_RTC6ConfigurationPreset::getLaserOnDelay()
    {
        return m_dLaserOnDelay;
    }

    double CDriver_ScanLab_RTC6ConfigurationPreset::getLaserOffDelay()
    {
        return m_dLaserOffDelay;
    }

    double CDriver_ScanLab_RTC6ConfigurationPreset::getMarkDelay()
    {
        return m_dMarkDelay;
    }

    double CDriver_ScanLab_RTC6ConfigurationPreset::getJumpDelay() 
    {
        return m_dJumpDelay;
    }

    double CDriver_ScanLab_RTC6ConfigurationPreset::getPolygonDelay()
    {
        return m_dPolygonDelay;
    }

    uint32_t CDriver_ScanLab_RTC6ConfigurationPreset::getLaserIndex()
    {
        return m_nLaserIndex;
    }



