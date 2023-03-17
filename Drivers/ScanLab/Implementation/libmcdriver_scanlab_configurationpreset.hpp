/*++

Copyright (C) 2020 Autodesk Inc.

All rights reserved.

Abstract: This is the class declaration of CDriver

*/


#ifndef __LIBMCDRIVER_SCANLAB_CONFIGURATIONPRESET
#define __LIBMCDRIVER_SCANLAB_CONFIGURATIONPRESET

#include "libmcdriver_scanlab_interfaces.hpp"

namespace LibMCDriver_ScanLab {
namespace Impl {



    class CDriver_ScanLab_RTC6ConfigurationPreset {
    private:

        std::string m_sIP;
        std::string m_sNetmask;
        std::string m_sName;
        uint32_t m_nTimeout;
        uint32_t m_nSerialNumber;

        double m_dInitialTimeout;
        double m_dMaxTimeout;
        double m_dMultiplier;

        std::string m_sFirmwareResourceName;
        std::string m_sFPGAResourceName;
        std::string m_sAuxiliaryResourceName;

        std::string m_sCorrectionResourceName;
        uint32_t m_nCorrectionTableNumber;
        uint32_t m_nCorrectionDimension;
        uint32_t m_nCorrectionTableNumberHeadA;
        uint32_t m_nCorrectionTableNumberHeadB;

        LibMCDriver_ScanLab::eLaserMode m_LaserMode;
        LibMCDriver_ScanLab::eLaserPort m_LaserPort;
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

    public:

        CDriver_ScanLab_RTC6ConfigurationPreset();

        virtual ~CDriver_ScanLab_RTC6ConfigurationPreset();

        std::string getName();

        std::string getIP();
        std::string getNetmask();
        uint32_t getTimeout();
        uint32_t getSerialNumber();

        double getInitialTimeout();
        double getMaxTimeout();
        double getMultiplier();


        std::string getFirmwareResourceName();
        std::string getFPGAResourceName();
        std::string getAuxiliaryResourceName();

        std::string getCorrectionResourceName();
        uint32_t getCorrectionTableNumber();
        uint32_t getCorrectionDimension();
        uint32_t getCorrectionTableNumberHeadA();
        uint32_t getCorrectionTableNumberHeadB();

        LibMCDriver_ScanLab::eLaserMode getLaserMode();
        LibMCDriver_ScanLab::eLaserPort getLaserPort();
        double getMaxLaserPower();
        bool getFinishLaserPulseAfterOn();
        bool getPhaseShiftOfLaserSignal();
        bool getLaserOnSignalLowActive();
        bool getLaserHalfSignalsLowActive();
        bool getSetDigitalInOneHighActive();
        bool getOutputSynchronizationActive();

        double getLaserOnDelay();
        double getLaserOffDelay();
        double getMarkDelay();
        double getJumpDelay();
        double getPolygonDelay();

    };

    typedef std::shared_ptr<CDriver_ScanLab_RTC6ConfigurationPreset> PDriver_ScanLab_RTC6ConfigurationPreset;

} // namespace Impl
} // namespace LibMCDriver_ScanLab

#endif // __LIBMCDRIVER_SCANLAB_CONFIGURATIONPRESET
