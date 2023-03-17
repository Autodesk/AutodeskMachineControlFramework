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

#include "libmcdriver_scanlab_driver_scanlab_rtc6.hpp"
#include "libmcdriver_scanlab_interfaceexception.hpp"
#include "libmcdriver_scanlab_configurationpreset.hpp"

#include <cmath>

// Include custom headers here.
#define RTC6_MIN_MAXLASERPOWER 10.0f
#define RTC6_MAX_MAXLASERPOWER 10000.0f

using namespace LibMCDriver_ScanLab::Impl;

/*************************************************************************************************************************
 Class definition of CDriver_ScanLab_RTC6 
**************************************************************************************************************************/

CDriver_ScanLab_RTC6::CDriver_ScanLab_RTC6(const std::string& sName, const std::string& sType, LibMCEnv::PDriverEnvironment pDriverEnvironment)
	: CDriver_ScanLab (pDriverEnvironment), m_sName (sName), m_sType (sType), m_fMaxLaserPowerInWatts (0.0f), m_SimulationMode (false)
{
}

void CDriver_ScanLab_RTC6::Configure(const std::string& sConfigurationString)
{
    m_pDriverEnvironment->RegisterIntegerParameter("serialnumber", "Serial Number", 0);
    m_pDriverEnvironment->RegisterBoolParameter("position_x_ok", "Scan Position X is ok", false);
    m_pDriverEnvironment->RegisterBoolParameter("position_y_ok", "Scan Position Y is ok", false);
    m_pDriverEnvironment->RegisterBoolParameter("temperature_ok", "Scan Head Temperature is ok", false);
    m_pDriverEnvironment->RegisterBoolParameter("power_ok", "Scan Head Power is ok", false);
    m_pDriverEnvironment->RegisterBoolParameter("laser_on", "Laser is On", false);
    m_pDriverEnvironment->RegisterIntegerParameter("position_x", "Laser Position X", 0);
    m_pDriverEnvironment->RegisterIntegerParameter("position_y", "Laser Position Y", 0);
    m_pDriverEnvironment->RegisterIntegerParameter("position_z", "Laser Position Z", 0);
    m_pDriverEnvironment->RegisterIntegerParameter("position_x_corrected", "corrected Laser Position X", 0);
    m_pDriverEnvironment->RegisterIntegerParameter("position_y_corrected", "corrected Laser Position Y", 0);
    m_pDriverEnvironment->RegisterIntegerParameter("position_z_corrected", "corrected Laser Position Z", 0);
    m_pDriverEnvironment->RegisterIntegerParameter("focus_shift", "current Focus Shift", 0);
    m_pDriverEnvironment->RegisterIntegerParameter("mark_speed", "current Mark Speed", 0);
    m_pDriverEnvironment->RegisterIntegerParameter("list_position", "current List Position", 0);
    m_pDriverEnvironment->RegisterBoolParameter("card_busy", "Card is busy", false);
    m_pDriverEnvironment->RegisterIntegerParameter("rtc_version", "Scanlab RTC Version", 0);
    m_pDriverEnvironment->RegisterIntegerParameter("card_type", "Scanlab RTC Type", 0);
    m_pDriverEnvironment->RegisterIntegerParameter("dll_version", "Scanlab DLL Version", 0);
    m_pDriverEnvironment->RegisterIntegerParameter("hex_version", "Scanlab HEX Version", 0);
    m_pDriverEnvironment->RegisterIntegerParameter("bios_version", "Scanlab BIOS Version", 0);
}

std::string CDriver_ScanLab_RTC6::GetName()
{
	return m_sName;
}

std::string CDriver_ScanLab_RTC6::GetType()
{
	return m_sType;
}

void CDriver_ScanLab_RTC6::QueryParameters()
{
    QueryParametersEx(m_pDriverEnvironment->CreateStatusUpdateSession ());
}

void CDriver_ScanLab_RTC6::QueryParametersEx(LibMCEnv::PDriverStatusUpdateSession pDriverUpdateInstance)
{
    updateCardStatus(pDriverUpdateInstance);
}



void CDriver_ScanLab_RTC6::SetToSimulationMode()
{
    m_SimulationMode = true;
}


bool CDriver_ScanLab_RTC6::IsInitialized()
{
    return (m_pRTCContext.get() != nullptr);
}

bool CDriver_ScanLab_RTC6::IsSimulationMode()
{
    return m_SimulationMode;
}

void CDriver_ScanLab_RTC6::Initialise(const std::string& sIP, const std::string& sNetmask, const LibMCDriver_ScanLab_uint32 nTimeout, const LibMCDriver_ScanLab_uint32 nSerialNumber)
{

    if (m_SimulationMode) {
        m_pDriverEnvironment->SetIntegerParameter("rtc_version", 1);
        m_pDriverEnvironment->SetIntegerParameter("card_type", 1);
        m_pDriverEnvironment->SetIntegerParameter("dll_version", 1);
        m_pDriverEnvironment->SetIntegerParameter("hex_version", 1);
        m_pDriverEnvironment->SetIntegerParameter("bios_version", 1);
        m_pDriverEnvironment->SetIntegerParameter("serialnumber", 123456);


    } else {

        if (m_pRTCSelector.get() != nullptr)
            throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CARDALREADYINITIALIZED);

        m_pRTCContext = nullptr;

        m_pRTCSelector = act_managed_ptr<IRTCSelector>(CreateRTCSelector());

        if (sIP.empty()) {
            m_pRTCContext = act_managed_ptr<IRTCContext>(m_pRTCSelector->AcquireCardBySerial(nSerialNumber));
        }
        else {
            m_pRTCSelector->SearchCards(sIP, sNetmask, nTimeout);
            auto pContext = m_pRTCSelector->AcquireEthernetCardBySerial(nSerialNumber);
            m_pRTCContext = act_managed_ptr<IRTCContext>(pContext);
            
            auto pContextInstance = dynamic_cast<CRTCContext*> (pContext);
            if (pContextInstance != nullptr)
                pContextInstance->setIPAddress(sIP, sNetmask);
        }

        uint32_t nRTCVersion = 0;
        uint32_t nRTCType = 0;
        uint32_t nDLLVersion = 0;
        uint32_t nHEXVersion = 0;
        uint32_t nBIOSVersion = 0;
        m_pRTCContext->GetRTCVersion(nRTCVersion, nRTCType, nDLLVersion, nHEXVersion, nBIOSVersion);
        m_pDriverEnvironment->SetIntegerParameter("rtc_version", nRTCVersion);
        m_pDriverEnvironment->SetIntegerParameter("card_type", nRTCType);
        m_pDriverEnvironment->SetIntegerParameter("dll_version", nDLLVersion);
        m_pDriverEnvironment->SetIntegerParameter("hex_version", nHEXVersion);
        m_pDriverEnvironment->SetIntegerParameter("bios_version", nBIOSVersion);
        m_pDriverEnvironment->SetIntegerParameter("serialnumber", m_pRTCContext->GetSerialNumber ());
    }

}


void CDriver_ScanLab_RTC6::InitialiseFromConfiguration(const std::string& sPresetName)
{
    PDriver_ScanLab_RTC6ConfigurationPreset pPreset = findPresetByName (sPresetName, true);

    Initialise(pPreset->getIP (), pPreset->getNetmask (), pPreset->getTimeout (), pPreset->getSerialNumber ());

    SetCommunicationTimeouts(pPreset->getInitialTimeout(), pPreset->getMaxTimeout(), pPreset->getMultiplier());

    LoadFirmware(pPreset->getFirmwareResourceName(), pPreset->getFPGAResourceName(), pPreset->getAuxiliaryResourceName());

    std::vector<uint8_t> CorrectionBuffer;
    m_pDriverEnvironment->RetrieveMachineResourceData(pPreset->getCorrectionResourceName(), CorrectionBuffer);

    if (CorrectionBuffer.size() == 0)
        throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDRTCCORRECTIONDATA);

    SetCorrectionFile(CorrectionBuffer.size(), CorrectionBuffer.data(), pPreset->getCorrectionTableNumber(), pPreset->getCorrectionDimension(), pPreset->getCorrectionTableNumberHeadA(), pPreset->getCorrectionTableNumberHeadB());

    ConfigureDelays(pPreset->getLaserOnDelay(), pPreset->getLaserOffDelay(), pPreset->getMarkDelay(), pPreset->getJumpDelay(), pPreset->getPolygonDelay());

    ConfigureLaserMode(pPreset->getLaserMode(), pPreset->getLaserPort(), pPreset->getMaxLaserPower(), pPreset->getFinishLaserPulseAfterOn(), pPreset->getPhaseShiftOfLaserSignal(), pPreset->getLaserOnSignalLowActive(), pPreset->getLaserHalfSignalsLowActive(), pPreset->getSetDigitalInOneHighActive(), pPreset->getOutputSynchronizationActive()); 

}


std::string CDriver_ScanLab_RTC6::GetIPAddress()
{
    if (m_pRTCContext.get() == nullptr)
        throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CARDNOTINITIALIZED);

    return m_pRTCContext->GetIPAddress();

}

std::string CDriver_ScanLab_RTC6::GetNetmask()
{
    if (m_pRTCContext.get() == nullptr)
        throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CARDNOTINITIALIZED);

    return m_pRTCContext->GetNetmask();
}

LibMCDriver_ScanLab_uint32 CDriver_ScanLab_RTC6::GetSerialNumber()
{
    if (m_pRTCContext.get() == nullptr)
        throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CARDNOTINITIALIZED);

    return m_pRTCContext->GetSerialNumber();
}



IRTCContext* CDriver_ScanLab_RTC6::GetContext()
{
    if (m_pRTCContext.get() == nullptr)
        throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CARDNOTINITIALIZED);

    return m_pRTCContext.get();
}

IRTCSelector* CDriver_ScanLab_RTC6::GetSelector()
{
    if (m_pRTCContext.get() == nullptr)
        throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CARDNOTINITIALIZED);
    if (m_pRTCSelector.get() == nullptr)
        throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CARDNOTINITIALIZED);

    return m_pRTCSelector.get();
}


void CDriver_ScanLab_RTC6::LoadFirmware(const std::string& sFirmwareResource, const std::string& sFPGAResource, const std::string& sAuxiliaryResource)
{
    if (!m_SimulationMode) {

        if (m_pRTCContext.get() == nullptr)
            throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CARDNOTINITIALIZED);

        std::vector<uint8_t> FirmwareData;
        if (m_pDriverEnvironment->MachineHasResourceData(sFirmwareResource)) {
            m_pDriverEnvironment->RetrieveMachineResourceData(sFirmwareResource, FirmwareData);
        }
        else {
            m_pDriverEnvironment->RetrieveDriverResourceData(sFirmwareResource, FirmwareData);
        }

        std::vector<uint8_t> FPGAData;
        if (m_pDriverEnvironment->MachineHasResourceData(sFPGAResource)) {
            m_pDriverEnvironment->RetrieveMachineResourceData(sFPGAResource, FPGAData);
        }
        else {
            m_pDriverEnvironment->RetrieveDriverResourceData(sFPGAResource, FPGAData);
        }

        std::vector<uint8_t> AuxiliaryData;
        if (m_pDriverEnvironment->MachineHasResourceData(sAuxiliaryResource)) {
            m_pDriverEnvironment->RetrieveMachineResourceData(sAuxiliaryResource, AuxiliaryData);
        }
        else {
            m_pDriverEnvironment->RetrieveDriverResourceData(sAuxiliaryResource, AuxiliaryData);
        }

        m_pRTCContext->LoadFirmware (FirmwareData.size (), FirmwareData.data (), FPGAData.size (), FPGAData.data(), AuxiliaryData.size (), AuxiliaryData.data ());

    }
}

void CDriver_ScanLab_RTC6::LoadCustomFirmware(const LibMCDriver_ScanLab_uint64 nFirmwareDataBufferSize, const LibMCDriver_ScanLab_uint8* pFirmwareDataBuffer, const LibMCDriver_ScanLab_uint64 nFPGADataBufferSize, const LibMCDriver_ScanLab_uint8* pFPGADataBuffer, const LibMCDriver_ScanLab_uint64 nAuxiliaryDataBufferSize, const LibMCDriver_ScanLab_uint8* pAuxiliaryDataBuffer)
{
    if (!m_SimulationMode) {
        m_pRTCContext->LoadFirmware(nFirmwareDataBufferSize, pFirmwareDataBuffer, nFPGADataBufferSize, pFPGADataBuffer, nAuxiliaryDataBufferSize, pAuxiliaryDataBuffer);
    }
}


void CDriver_ScanLab_RTC6::SetCorrectionFile(const LibMCDriver_ScanLab_uint64 nCorrectionFileBufferSize, const LibMCDriver_ScanLab_uint8* pCorrectionFileBuffer, const LibMCDriver_ScanLab_uint32 nTableNumber, const LibMCDriver_ScanLab_uint32 nDimension, const LibMCDriver_ScanLab_uint32 nTableNumberHeadA, const LibMCDriver_ScanLab_uint32 nTableNumberHeadB) 
{
    if (!m_SimulationMode) {
        if (m_pRTCContext.get() == nullptr)
            throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CARDNOTINITIALIZED);

        m_pRTCContext->LoadCorrectionFile(nCorrectionFileBufferSize, pCorrectionFileBuffer, nTableNumber, nDimension);
        m_pRTCContext->SelectCorrectionTable(nTableNumberHeadA, nTableNumberHeadB);

    }
}

void CDriver_ScanLab_RTC6::ConfigureLaserMode(const LibMCDriver_ScanLab::eLaserMode eLaserMode, const LibMCDriver_ScanLab::eLaserPort eLaserPort, const LibMCDriver_ScanLab_double dMaxLaserPower, const bool bFinishLaserPulseAfterOn, const bool bPhaseShiftOfLaserSignal, const bool bLaserOnSignalLowActive, const bool bLaserHalfSignalsLowActive, const bool bSetDigitalInOneHighActive, const bool bOutputSynchronizationActive) 
{

    if (!m_SimulationMode) {

        if (m_pRTCContext.get() == nullptr)
            throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CARDNOTINITIALIZED);

        if (((float)dMaxLaserPower < RTC6_MIN_MAXLASERPOWER) || ((float)dMaxLaserPower > RTC6_MAX_MAXLASERPOWER))
            throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDMAXLASERPOWER);

        m_fMaxLaserPowerInWatts = (float)dMaxLaserPower;

        m_pRTCContext->ConfigureLists(1 << 22, 1 << 22);
        m_pRTCContext->SetLaserMode(eLaserMode, eLaserPort);
        m_pRTCContext->DisableAutoLaserControl();
        m_pRTCContext->SetLaserControlParameters(false, bFinishLaserPulseAfterOn, bPhaseShiftOfLaserSignal, bLaserOnSignalLowActive, bLaserHalfSignalsLowActive, bSetDigitalInOneHighActive, bOutputSynchronizationActive);
        m_pRTCContext->SetLaserPulsesInMicroSeconds(5, 5);
        m_pRTCContext->SetStandbyInMicroSeconds(1, 1);

    }
}

void CDriver_ScanLab_RTC6::ConfigureDelays(const LibMCDriver_ScanLab_double dLaserOnDelay, const LibMCDriver_ScanLab_double dLaserOffDelay, const LibMCDriver_ScanLab_double dMarkDelay, const LibMCDriver_ScanLab_double dJumpDelay, const LibMCDriver_ScanLab_double dPolygonDelay)
{

    if (!m_SimulationMode) {

        if (m_pRTCContext.get() == nullptr)
            throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CARDNOTINITIALIZED);

        if ((dLaserOnDelay < 0.0) || (dLaserOnDelay > 10000000.0))
            throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDLASERDELAY);
        if ((dLaserOffDelay < 0.0) || (dLaserOffDelay > 10000000.0))
            throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDLASERDELAY);
        if ((dMarkDelay < 0.0) || (dMarkDelay > 10000000.0))
            throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDSCANNERDELAY);
        if ((dJumpDelay < 0.0) || (dJumpDelay > 10000000.0))
            throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDSCANNERDELAY);
        if ((dPolygonDelay < 0.0) || (dPolygonDelay > 10000000.0))
            throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDSCANNERDELAY);


        int nIntMarkDelay = ((int)round(dMarkDelay / 10.0)) * 10;
        int nIntJumpDelay = ((int)round(dJumpDelay / 10.0)) * 10;
        int nIntPolygonDelay = ((int)round(dPolygonDelay / 10.0)) * 10;

        if (nIntMarkDelay < 10)
            nIntMarkDelay = 10;
        if (nIntJumpDelay < 10)
            nIntJumpDelay = 10;
        if (nIntPolygonDelay < 10)
            nIntPolygonDelay = 10;

        m_pRTCContext->SetLaserDelaysInMicroseconds(dLaserOnDelay, dLaserOffDelay);
        m_pRTCContext->SetDelays(nIntMarkDelay, nIntJumpDelay, nIntPolygonDelay);


    }
}


void CDriver_ScanLab_RTC6::AddLayerToCurrentList(const std::string& sStreamUUID, const LibMCDriver_ScanLab_uint32 nLayerIndex)
{
    if (!m_SimulationMode) {

        if (m_pRTCContext.get() == nullptr)
            throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CARDNOTINITIALIZED);
        if ((m_fMaxLaserPowerInWatts < RTC6_MIN_MAXLASERPOWER) || (m_fMaxLaserPowerInWatts > RTC6_MAX_MAXLASERPOWER))
            throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDMAXLASERPOWER);

        auto pToolpathAccessor = m_pDriverEnvironment->CreateToolpathAccessor(sStreamUUID);
        auto pLayer = pToolpathAccessor->LoadLayer(nLayerIndex);

        double dUnits = pToolpathAccessor->GetUnits();

        uint32_t nSegmentCount = pLayer->GetSegmentCount();
        for (uint32_t nSegmentIndex = 0; nSegmentIndex < nSegmentCount; nSegmentIndex++) {

            LibMCEnv::eToolpathSegmentType eSegmentType;
            uint32_t nPointCount;
            pLayer->GetSegmentInfo(nSegmentIndex, eSegmentType, nPointCount);

            if (nPointCount >= 2) {

                float fJumpSpeedInMMPerSecond = (float)pLayer->GetSegmentProfileTypedValue(nSegmentIndex, LibMCEnv::eToolpathProfileValueType::JumpSpeed);
                float fMarkSpeedInMMPerSecond = (float)pLayer->GetSegmentProfileTypedValue(nSegmentIndex, LibMCEnv::eToolpathProfileValueType::Speed);
                float fPowerInWatts = (float)pLayer->GetSegmentProfileTypedValue(nSegmentIndex, LibMCEnv::eToolpathProfileValueType::LaserPower);
                float fPowerInPercent = (fPowerInWatts * 100.f) / m_fMaxLaserPowerInWatts;
                float fLaserFocus = (float)pLayer->GetSegmentProfileTypedValue(nSegmentIndex, LibMCEnv::eToolpathProfileValueType::LaserFocus);

                int64_t nSkywritingMode = pLayer->GetSegmentProfileIntegerValueDef(nSegmentIndex, "http://schemas.scanlab.com/skywriting/2023/01", "mode", 0);

                if (nSkywritingMode != 0) {
                    double dSkywritingTimeLag = pLayer->GetSegmentProfileDoubleValue(nSegmentIndex, "http://schemas.scanlab.com/skywriting/2023/01", "timelag");
                    int64_t nSkywritingLaserOnShift = pLayer->GetSegmentProfileIntegerValue(nSegmentIndex, "http://schemas.scanlab.com/skywriting/2023/01", "laseronshift");
                    int64_t nSkywritingPrev = pLayer->GetSegmentProfileIntegerValue(nSegmentIndex, "http://schemas.scanlab.com/skywriting/2023/01", "nprev");
                    int64_t nSkywritingPost = pLayer->GetSegmentProfileIntegerValue(nSegmentIndex, "http://schemas.scanlab.com/skywriting/2023/01", "npost");

                    double dSkywritingLimit = 0.0;
                    if (nSkywritingMode == 3) {
                        dSkywritingLimit = pLayer->GetSegmentProfileDoubleValue(nSegmentIndex, "http://schemas.scanlab.com/skywriting/2023/01", "limit");
                    }


                    switch (nSkywritingMode) {
                    case 1:
                        m_pRTCContext->EnableSkyWritingMode1(dSkywritingTimeLag, nSkywritingLaserOnShift, nSkywritingPrev, nSkywritingPost);
                        break;
                    case 2:
                        m_pRTCContext->EnableSkyWritingMode2(dSkywritingTimeLag, nSkywritingLaserOnShift, nSkywritingPrev, nSkywritingPost);
                        break;
                    case 3:
                        m_pRTCContext->EnableSkyWritingMode3(dSkywritingTimeLag, nSkywritingLaserOnShift, nSkywritingPrev, nSkywritingPost, dSkywritingLimit);
                        break;
                    default:
                        m_pRTCContext->DisableSkyWriting();
                    }

                }

                std::vector<LibMCEnv::sPosition2D> Points;
                pLayer->GetSegmentPointData(nSegmentIndex, Points);

                if (nPointCount != Points.size())
                    throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPOINTCOUNT);

                switch (eSegmentType) {
                case LibMCEnv::eToolpathSegmentType::Loop:
                case LibMCEnv::eToolpathSegmentType::Polyline:
                {

                    std::vector<sPoint2D> ContourPoints;
                    ContourPoints.resize(nPointCount);

                    for (uint32_t nPointIndex = 0; nPointIndex < nPointCount; nPointIndex++) {
                        auto pContourPoint = &ContourPoints.at(nPointIndex);
                        pContourPoint->m_X = (float)(Points[nPointIndex].m_Coordinates[0] * dUnits);
                        pContourPoint->m_Y = (float)(Points[nPointIndex].m_Coordinates[1] * dUnits);
                    }

                    m_pRTCContext->DrawPolyline(nPointCount, ContourPoints.data(), fMarkSpeedInMMPerSecond, fJumpSpeedInMMPerSecond, fPowerInPercent, fLaserFocus);

                    break;
                }

                case LibMCEnv::eToolpathSegmentType::Hatch:
                {
                    if (nPointCount % 2 == 1)
                        throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPOINTCOUNT);

                    uint64_t nHatchCount = nPointCount / 2;
                    std::vector<sHatch2D> Hatches;
                    Hatches.resize(nHatchCount);

                    for (uint64_t nHatchIndex = 0; nHatchIndex < nHatchCount; nHatchIndex++) {
                        auto pHatch = &Hatches.at(nHatchIndex);
                        pHatch->m_X1 = (float)(Points[nHatchIndex * 2].m_Coordinates[0] * dUnits);
                        pHatch->m_Y1 = (float)(Points[nHatchIndex * 2].m_Coordinates[1] * dUnits);
                        pHatch->m_X2 = (float)(Points[nHatchIndex * 2 + 1].m_Coordinates[0] * dUnits);
                        pHatch->m_Y2 = (float)(Points[nHatchIndex * 2 + 1].m_Coordinates[1] * dUnits);
                    }

                    m_pRTCContext->DrawHatches(Hatches.size(), Hatches.data(), fMarkSpeedInMMPerSecond, fJumpSpeedInMMPerSecond, fPowerInPercent, fLaserFocus);

                    break;
                }

                }

            }

        }


    }
}


void CDriver_ScanLab_RTC6::DrawLayer(const std::string& sStreamUUID, const LibMCDriver_ScanLab_uint32 nLayerIndex)
{
    if (!m_SimulationMode) {


        if (m_pRTCContext.get() == nullptr)
            throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CARDNOTINITIALIZED);
        if ((m_fMaxLaserPowerInWatts < RTC6_MIN_MAXLASERPOWER) || (m_fMaxLaserPowerInWatts > RTC6_MAX_MAXLASERPOWER))
            throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDMAXLASERPOWER);

        SetStartList(1, 0);

        AddLayerToCurrentList(sStreamUUID, nLayerIndex);

        SetEndOfList();
        
        ExecuteList(1, 0);


    }

}


void CDriver_ScanLab_RTC6::SetStartList(const LibMCDriver_ScanLab_uint32 nListIndex, const LibMCDriver_ScanLab_uint32 nPosition)
{
    if (m_pRTCContext.get() == nullptr)
        throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CARDNOTINITIALIZED);

    m_pRTCContext->SetStartList(nListIndex, nPosition);

}



void CDriver_ScanLab_RTC6::SetEndOfList()
{
    if (m_pRTCContext.get() == nullptr)
        throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CARDNOTINITIALIZED);

    m_pRTCContext->SetEndOfList();

}

void CDriver_ScanLab_RTC6::ExecuteList(const LibMCDriver_ScanLab_uint32 nListIndex, const LibMCDriver_ScanLab_uint32 nPosition)
{

    if (m_pRTCContext.get() == nullptr)
        throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CARDNOTINITIALIZED);

    m_pRTCContext->ExecuteList(nListIndex, nPosition);


    auto pDriverUpdateInstance = m_pDriverEnvironment->CreateStatusUpdateSession();

    bool Busy = true;
    uint32_t Pos = 0;

    while (Busy) {
        m_pRTCContext->GetStatus(Busy, Pos);
        pDriverUpdateInstance->Sleep(10);

        updateCardStatus(pDriverUpdateInstance);
    }
}

void CDriver_ScanLab_RTC6::updateCardStatus(LibMCEnv::PDriverStatusUpdateSession pDriverUpdateInstance)
{
    if (pDriverUpdateInstance.get() == nullptr)
        return;

    if (!m_SimulationMode) {
        if (m_pRTCContext.get() == nullptr)
            throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CARDNOTINITIALIZED);

        bool Busy = true;
        uint32_t ListPosition = 0;
        bool bPositionXisOK, bPositionYisOK, bTemperatureisOK, bPowerisOK;

        m_pRTCContext->GetStatus(Busy, ListPosition);
        m_pRTCContext->GetHeadStatus(1, bPositionXisOK, bPositionYisOK, bTemperatureisOK, bPowerisOK);

        pDriverUpdateInstance->SetBoolParameter("position_x_ok", bPositionXisOK);
        pDriverUpdateInstance->SetBoolParameter("position_y_ok", bPositionYisOK);
        pDriverUpdateInstance->SetBoolParameter("temperature_ok", bTemperatureisOK);
        pDriverUpdateInstance->SetBoolParameter("power_ok", bPowerisOK);

        pDriverUpdateInstance->SetIntegerParameter("list_position", ListPosition);
        pDriverUpdateInstance->SetBoolParameter("card_busy", Busy);

        bool bLaserIsOn;
        int32_t nPositionX, nPositionY, nPositionZ;
        int32_t nCorrectedPositionX, nCorrectedPositionY, nCorrectedPositionZ;
        int32_t nFocusShift, nMarkSpeed;

        m_pRTCContext->GetStateValues(bLaserIsOn, nPositionX, nPositionY, nPositionZ, nCorrectedPositionX, nCorrectedPositionY, nCorrectedPositionZ, nFocusShift, nMarkSpeed);
        pDriverUpdateInstance->SetBoolParameter("laser_on", bLaserIsOn);
        pDriverUpdateInstance->SetIntegerParameter("position_x", nPositionX);
        pDriverUpdateInstance->SetIntegerParameter("position_y", nPositionY);
        pDriverUpdateInstance->SetIntegerParameter("position_z", nPositionZ);
        pDriverUpdateInstance->SetIntegerParameter("position_x_corrected", nCorrectedPositionX);
        pDriverUpdateInstance->SetIntegerParameter("position_y_corrected", nCorrectedPositionY);
        pDriverUpdateInstance->SetIntegerParameter("position_z_corrected", nCorrectedPositionZ);
        pDriverUpdateInstance->SetIntegerParameter("focus_shift", nFocusShift);
        pDriverUpdateInstance->SetIntegerParameter("mark_speed", nMarkSpeed);

    }

}

void CDriver_ScanLab_RTC6::SetCommunicationTimeouts(const LibMCDriver_ScanLab_double dInitialTimeout, const LibMCDriver_ScanLab_double dMaxTimeout, const LibMCDriver_ScanLab_double dMultiplier)
{
    if (!m_SimulationMode) {
        if (m_pRTCContext.get() == nullptr)
            throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CARDNOTINITIALIZED);

        m_pRTCContext->SetCommunicationTimeouts (dInitialTimeout, dMaxTimeout, dMultiplier);
    }
}

void CDriver_ScanLab_RTC6::GetCommunicationTimeouts(LibMCDriver_ScanLab_double& dInitialTimeout, LibMCDriver_ScanLab_double& dMaxTimeout, LibMCDriver_ScanLab_double& dMultiplier)
{
    if (!m_SimulationMode) {
        if (m_pRTCContext.get() == nullptr)
            throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CARDNOTINITIALIZED);

        m_pRTCContext->GetCommunicationTimeouts (dInitialTimeout, dMaxTimeout, dMultiplier);

    }
    else {
        dInitialTimeout = 0.75;
        dMaxTimeout = 20.0;
        dMultiplier = 1.3;
    }

}

PDriver_ScanLab_RTC6ConfigurationPreset CDriver_ScanLab_RTC6::findPresetByName(const std::string& sPresetName, bool bMustExist)
{
    auto iIterator = m_ConfigurationPresets.find(sPresetName);
    if (iIterator != m_ConfigurationPresets.end()) {
        return iIterator->second;
    }
    else {
        if (bMustExist)
            throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CONFIGURATIONPRESETNOTFOUND, sPresetName);

        return nullptr;
    }
}



