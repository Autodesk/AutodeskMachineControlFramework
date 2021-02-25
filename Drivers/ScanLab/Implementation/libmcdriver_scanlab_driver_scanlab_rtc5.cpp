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

#include "libmcdriver_scanlab_driver_scanlab_rtc5.hpp"
#include "libmcdriver_scanlab_interfaceexception.hpp"

// Include custom headers here.
#define RTC5_MIN_MAXLASERPOWER 10.0f
#define RTC5_MAX_MAXLASERPOWER 10000.0f

using namespace LibMCDriver_ScanLab::Impl;

/*************************************************************************************************************************
 Class definition of CDriver_ScanLab_RTC5 
**************************************************************************************************************************/

CDriver_ScanLab_RTC5::CDriver_ScanLab_RTC5(const std::string& sName, const std::string& sType, LibMCEnv::PDriverEnvironment pDriverEnvironment)
	: CDriver_ScanLab (pDriverEnvironment), m_sName (sName), m_sType (sType), m_fMaxLaserPowerInWatts (0.0f)
{

}

void CDriver_ScanLab_RTC5::Configure(const std::string& sConfigurationString)
{

}

std::string CDriver_ScanLab_RTC5::GetName()
{
	return m_sName;
}

std::string CDriver_ScanLab_RTC5::GetType()
{
	return m_sType;
}


void CDriver_ScanLab_RTC5::QueryParameters()
{

}


void CDriver_ScanLab_RTC5::Initialise(const std::string& sIP, const std::string& sNetmask, const LibMCDriver_ScanLab_uint32 nTimeout, const LibMCDriver_ScanLab_uint32 nSerialNumber)
{
	if (m_pRTCSelector.get() != nullptr)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CARDALREADYINITIALIZED);

	m_pRTCContext = nullptr;

	m_pRTCSelector = std::shared_ptr<IRTCSelector> (CreateRTCSelector());

	if (sIP.empty ()) {
		m_pRTCContext = std::shared_ptr<IRTCContext> (m_pRTCSelector->AcquireCardBySerial(nSerialNumber));
	}
	else {
		m_pRTCSelector->SearchCards(sIP, sNetmask, nTimeout);
		m_pRTCContext = std::shared_ptr<IRTCContext>(m_pRTCSelector->AcquireEthernetCardBySerial(nSerialNumber));
	}

}

void CDriver_ScanLab_RTC5::LoadFirmware(const std::string& sFirmwareResource, const std::string& sFPGAResource, const std::string& sAuxiliaryResource)
{
	if (m_pRTCContext.get() == nullptr)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CARDNOTINITIALIZED);

	m_pRTCContext->LoadFirmware(sFirmwareResource, sFPGAResource, sAuxiliaryResource);

}

void CDriver_ScanLab_RTC5::SetCorrectionFile(const LibMCDriver_ScanLab_uint64 nCorrectionFileBufferSize, const LibMCDriver_ScanLab_uint8* pCorrectionFileBuffer, const LibMCDriver_ScanLab_uint32 nTableNumber, const LibMCDriver_ScanLab_uint32 nDimension, const LibMCDriver_ScanLab_uint32 nTableNumberHeadA, const LibMCDriver_ScanLab_uint32 nTableNumberHeadB) 
{
	if (m_pRTCContext.get() == nullptr)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CARDNOTINITIALIZED);

	m_pRTCContext->LoadCorrectionFile (nCorrectionFileBufferSize, pCorrectionFileBuffer, nTableNumber, nDimension);
    m_pRTCContext->SelectCorrectionTable(nTableNumberHeadA, nTableNumberHeadB);

}

void CDriver_ScanLab_RTC5::ConfigureLaserMode(const LibMCDriver_ScanLab::eLaserMode eLaserMode, const LibMCDriver_ScanLab::eLaserPort eLaserPort, const LibMCDriver_ScanLab_double dMaxLaserPower, const bool bFinishLaserPulseAfterOn, const bool bPhaseShiftOfLaserSignal, const bool bLaserOnSignalLowActive, const bool bLaserHalfSignalsLowActive, const bool bSetDigitalInOneHighActive, const bool bOutputSynchronizationActive)
{

    if (m_pRTCContext.get() == nullptr)
        throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CARDNOTINITIALIZED);

    if (((float)dMaxLaserPower < RTC5_MIN_MAXLASERPOWER) || ((float)dMaxLaserPower > RTC5_MAX_MAXLASERPOWER))
        throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDMAXLASERPOWER);

    m_fMaxLaserPowerInWatts = (float)dMaxLaserPower;

    m_pRTCContext->ConfigureLists(1 << 22, 1 << 22);
    m_pRTCContext->SetLaserMode(eLaserMode, eLaserPort);
    m_pRTCContext->DisableAutoLaserControl ();
    m_pRTCContext->SetLaserControlParameters(false, bFinishLaserPulseAfterOn, bPhaseShiftOfLaserSignal, bLaserOnSignalLowActive, bLaserHalfSignalsLowActive, bSetDigitalInOneHighActive, bOutputSynchronizationActive); 
    //false, false, true, true, false, false);

    m_pRTCContext->SetLaserPulsesInMicroSeconds(5, 5);
    m_pRTCContext->SetStandbyInMicroSeconds(1, 1);

}

void CDriver_ScanLab_RTC5::DrawLayer(const std::string& sStreamUUID, const LibMCDriver_ScanLab_uint32 nLayerIndex)
{
    if (m_pRTCContext.get() == nullptr)
        throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CARDNOTINITIALIZED);
    if ((m_fMaxLaserPowerInWatts < RTC5_MIN_MAXLASERPOWER) || (m_fMaxLaserPowerInWatts > RTC5_MAX_MAXLASERPOWER))
        throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDMAXLASERPOWER);
    

    auto pToolpathAccessor = m_pDriverEnvironment->CreateToolpathAccessor(sStreamUUID);
    auto pLayer = pToolpathAccessor->LoadLayer(nLayerIndex);

    double dUnits = pToolpathAccessor->GetUnits();

    internalBegin();

    uint32_t nSegmentCount = pLayer->GetSegmentCount();
    for (uint32_t nSegmentIndex = 0; nSegmentIndex < nSegmentCount; nSegmentIndex++) {

        LibMCEnv::eToolpathSegmentType eSegmentType;
        uint32_t nPointCount;
        pLayer->GetSegmentInfo(nSegmentIndex, eSegmentType, nPointCount);

        if (nPointCount >= 2) {

            float fJumpSpeedInMMPerSecond = (float) pLayer->GetSegmentProfileTypedValue (nSegmentIndex, LibMCEnv::eToolpathProfileValueType::JumpSpeed);
            float fMarkSpeedInMMPerSecond = (float)pLayer->GetSegmentProfileTypedValue(nSegmentIndex, LibMCEnv::eToolpathProfileValueType::Speed);
            float fPowerInWatts = (float)pLayer->GetSegmentProfileTypedValue(nSegmentIndex, LibMCEnv::eToolpathProfileValueType::LaserPower);
            float fPowerInPercent = (fPowerInWatts * 100.f) / m_fMaxLaserPowerInWatts;
            float fLaserFocus = (float)pLayer->GetSegmentProfileTypedValue(nSegmentIndex, LibMCEnv::eToolpathProfileValueType::LaserFocus);

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
                    pContourPoint->m_X = (float) (Points[nPointIndex].m_Coordinates[0] * dUnits);
                    pContourPoint->m_Y = (float) (Points[nPointIndex].m_Coordinates[1] * dUnits);
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
                    auto pHatch = &Hatches.at (nHatchIndex);
                    pHatch->m_X1 = (float)(Points[nHatchIndex * 2].m_Coordinates[0] * dUnits);
                    pHatch->m_Y1 = (float)(Points[nHatchIndex * 2].m_Coordinates[1] * dUnits);
                    pHatch->m_X2 = (float)(Points[nHatchIndex * 2 + 1].m_Coordinates[0] * dUnits);
                    pHatch->m_Y2 = (float)(Points[nHatchIndex * 2 + 1].m_Coordinates[1] * dUnits);
                }

                m_pRTCContext->DrawHatches (Hatches.size (), Hatches.data (), fMarkSpeedInMMPerSecond, fJumpSpeedInMMPerSecond, fPowerInPercent, fLaserFocus);

                break;
            }

            }

        }

    }

    internalExecute();


}


void CDriver_ScanLab_RTC5::internalBegin()
{
    if (m_pRTCContext.get() == nullptr)
        throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CARDNOTINITIALIZED);

    m_pRTCContext->SetStartList(1, 0);

}


void CDriver_ScanLab_RTC5::internalExecute()
{
    if (m_pRTCContext.get() == nullptr)
        throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CARDNOTINITIALIZED);

    m_pRTCContext->SetEndOfList();
    m_pRTCContext->ExecuteList(1, 0);

    bool Busy = true;
    uint32_t Pos = 0;

    while (Busy) {
        m_pRTCContext->GetStatus (Busy, Pos);
        Sleep (10);
    }

}
