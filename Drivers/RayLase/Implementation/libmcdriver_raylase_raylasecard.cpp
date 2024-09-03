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

#include "libmcdriver_raylase_raylasecard.hpp"
#include "libmcdriver_raylase_interfaceexception.hpp"


using namespace LibMCDriver_Raylase::Impl;

#define MINLASERPOWER 0.1

PRaylaseCardImpl CRaylaseCardImpl::connectByIP(PRaylaseSDK pSDK, const std::string& sCardName, const std::string& sCardIP, uint32_t nPort, double dMaxLaserPowerInWatts, bool bSimulationMode, LibMCEnv::PDriverEnvironment pDriverEnvironment)
{
    return std::make_shared<CRaylaseCardImpl>(pSDK, sCardName, sCardIP, nPort, dMaxLaserPowerInWatts,  bSimulationMode, pDriverEnvironment);
}

CRaylaseCardImpl::CRaylaseCardImpl(PRaylaseSDK pSDK, const std::string& sCardName, const std::string& sCardIP, uint32_t nPort, double dMaxLaserPowerInWatts, bool bSimulationMode, LibMCEnv::PDriverEnvironment pDriverEnvironment)
    : m_pSDK (pSDK), m_sCardName (sCardName), m_sCardIP (sCardIP), m_nPort (nPort), m_Handle (0), m_bSimulationMode (bSimulationMode),
        m_bSimulatedPilotIsEnabled (false), m_bSimulatedPilotIsArmed (false), m_bSimulatedPilotIsAlarm (false),
        m_pDriverEnvironment (pDriverEnvironment), m_dMaxLaserPowerInWatts (dMaxLaserPowerInWatts)

{
    if (pDriverEnvironment.get () == nullptr)
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDPARAM);

    if (dMaxLaserPowerInWatts < MINLASERPOWER)
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDLASERPOWER);

    if (!m_bSimulationMode) {
        if (pSDK.get() == nullptr)
            throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_SDKNOTINITIALIZED);

        m_Handle = m_pSDK->rlConnect(sCardIP.c_str(), nPort);
        if (m_Handle < 0)
            m_pSDK->checkError(m_Handle);
    }
}

CRaylaseCardImpl::~CRaylaseCardImpl()
{
    Disconnect();

}

void CRaylaseCardImpl::ResetToSystemDefaults()
{
    if (m_bSimulationMode)
        return;

    m_pSDK->checkError(m_pSDK->rlSystemResetToDefaults(m_Handle));
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

void CRaylaseCardImpl::DrawLayer(const std::string& sStreamUUID, const LibMCDriver_Raylase_uint32 nLayerIndex)
{
    if (m_bSimulationMode)
        return;

    auto pToolpathAccessor = m_pDriverEnvironment->CreateToolpathAccessor(sStreamUUID);
    auto pLayer = pToolpathAccessor->LoadLayer(nLayerIndex);

    double dUnits = pToolpathAccessor->GetUnits();

    rlListHandle listHandle = m_pSDK->rlListAllocate(m_Handle);
    //m_pSDK->checkError(m_pSDK->rlListAppendLaserOn(listHandle), "rlListAppendLaserOn");

    uint32_t nSegmentCount = pLayer->GetSegmentCount();
    for (uint32_t nSegmentIndex = 0; nSegmentIndex < nSegmentCount; nSegmentIndex++) {

        LibMCEnv::eToolpathSegmentType eSegmentType;
        uint32_t nPointCount;
        pLayer->GetSegmentInfo(nSegmentIndex, eSegmentType, nPointCount);

        if (nPointCount >= 2) {

            double dJumpSpeedInMMPerSecond = pLayer->GetSegmentProfileTypedValue(nSegmentIndex, LibMCEnv::eToolpathProfileValueType::JumpSpeed);
            double dMarkSpeedInMMPerSecond = pLayer->GetSegmentProfileTypedValue(nSegmentIndex, LibMCEnv::eToolpathProfileValueType::Speed);
            double dPowerInWatts = pLayer->GetSegmentProfileTypedValue(nSegmentIndex, LibMCEnv::eToolpathProfileValueType::LaserPower);
            double dPowerFactor = (dPowerInWatts / m_dMaxLaserPowerInWatts);
            int32_t nPowerInUnits = (int32_t)(dPowerFactor * 65535.0);
            if (nPowerInUnits < 0)
                nPowerInUnits = 0;
            if (nPowerInUnits > 65535)
                nPowerInUnits = 65535;

            double dJumpSpeedInMeterPerSecond = dJumpSpeedInMMPerSecond * 0.001;
            double dMarkSpeedInMeterPerSecond = dMarkSpeedInMMPerSecond * 0.001;

            m_pSDK->checkError(m_pSDK->rlListAppendJumpSpeed(listHandle, dJumpSpeedInMeterPerSecond), "rlListAppendJumpSpeed");
            m_pSDK->checkError(m_pSDK->rlListAppendMarkSpeed(listHandle, dMarkSpeedInMeterPerSecond), "rlListAppendMarkSpeed");
            //m_pSDK->checkError(m_pSDK->rlListAppendPower(listHandle, nPowerInUnits), "rlListAppendPower");


            std::vector<LibMCEnv::sPosition2D> Points;
            pLayer->GetSegmentPointData(nSegmentIndex, Points);

            if (nPointCount != Points.size())
                throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDPOINTCOUNT);

            switch (eSegmentType) {
            case LibMCEnv::eToolpathSegmentType::Loop:
            case LibMCEnv::eToolpathSegmentType::Polyline:
            {

                for (uint32_t nPointIndex = 0; nPointIndex < nPointCount; nPointIndex++) {
                    double dXinMM = (Points[nPointIndex].m_Coordinates[0] * dUnits);
                    double dYinMM = (Points[nPointIndex].m_Coordinates[1] * dUnits);

                    double dXinMicron = dXinMM * 1000.0;
                    double dYinMicron = dYinMM * 1000.0;

                    if (nPointIndex == 0) {
                        m_pSDK->checkError(m_pSDK->rlListAppendJumpAbs2D(listHandle, dXinMicron, dYinMicron), "rlListAppendJumpAbs2D");
                    }
                    else {
                        m_pSDK->checkError(m_pSDK->rlListAppendMarkAbs2D(listHandle, dXinMicron, dYinMicron), "rlListAppendMarkAbs2D");
                    }

                }


                break;
            }

            case LibMCEnv::eToolpathSegmentType::Hatch:
            {
                if (nPointCount % 2 == 1)
                    throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDPOINTCOUNT);

                uint64_t nHatchCount = nPointCount / 2;
                std::vector<sHatch2D> Hatches;
                Hatches.resize(nHatchCount);

                for (uint64_t nHatchIndex = 0; nHatchIndex < nHatchCount; nHatchIndex++) {
                    double dX1inMM = (float)(Points[nHatchIndex * 2].m_Coordinates[0] * dUnits);
                    double dY1inMM = (float)(Points[nHatchIndex * 2].m_Coordinates[1] * dUnits);
                    double dX2inMM = (float)(Points[nHatchIndex * 2 + 1].m_Coordinates[0] * dUnits);
                    double dY2inMM = (float)(Points[nHatchIndex * 2 + 1].m_Coordinates[1] * dUnits);

                    double dX1inMicron = dX1inMM * 1000.0;
                    double dY1inMicron = dY1inMM * 1000.0;
                    double dX2inMicron = dX2inMM * 1000.0;
                    double dY2inMicron = dY2inMM * 1000.0;

                    m_pSDK->checkError(m_pSDK->rlListAppendJumpAbs2D(listHandle, dX1inMicron, dY1inMicron), "rlListAppendJumpAbs2D");
                    m_pSDK->checkError(m_pSDK->rlListAppendMarkAbs2D(listHandle, dX2inMicron, dY2inMicron), "rlListAppendMarkAbs2D");
                }

                break;
            }

            }

        }

    }

    //m_pSDK->checkError(m_pSDK->rlListAppendLaserOff(listHandle), "rlListAppendLaserOff");

    m_pSDK->checkError(m_pSDK->rlListSet(m_Handle, 0, listHandle, false, -1), "rlListSet");
    m_pSDK->checkError(m_pSDK->rlListExecute(m_Handle, 0), "rlListExecute");
    int timeoutMs = 30000;
    bool done = false;
    int32_t listID = 0;
    m_pSDK->checkError(m_pSDK->rlListWaitForListDone(m_Handle, timeoutMs, done, listID), "rlListWaitForListDone");
    m_pSDK->checkError(m_pSDK->rlListDelete(m_Handle, 0, true), "rlListDelete");
    m_pSDK->checkError(m_pSDK->rlListReleaseHandle(listHandle), "rlListReleaseHandle");


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
        if (m_Handle > 0)
            m_pSDK->rlDisconnect(m_Handle);
        m_Handle = 0;
    }

}



/*************************************************************************************************************************
 Class definition of CRaylaseCard 
**************************************************************************************************************************/

CRaylaseCard::CRaylaseCard(PRaylaseCardImpl pRaylaseCardImpl)
    : m_pRaylaseCardImpl (pRaylaseCardImpl)
{
    if (pRaylaseCardImpl.get() == nullptr)
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDPARAM);

}

CRaylaseCard::~CRaylaseCard()
{

}


void CRaylaseCard::ResetToSystemDefaults()
{
    m_pRaylaseCardImpl->ResetToSystemDefaults ();
}

void CRaylaseCard::LaserOn()
{
    m_pRaylaseCardImpl->LaserOn();

}

void CRaylaseCard::LaserOff()
{
    m_pRaylaseCardImpl->LaserOff();
}

void CRaylaseCard::ArmLaser(const bool bShallBeArmed)
{
    m_pRaylaseCardImpl->ArmLaser(bShallBeArmed);
}

bool CRaylaseCard::IsLaserArmed()
{
    return m_pRaylaseCardImpl->IsLaserArmed ();
}

void CRaylaseCard::EnablePilot(const bool bShallBeEnabled)
{
    m_pRaylaseCardImpl->EnablePilot(bShallBeEnabled);
}

bool CRaylaseCard::PilotIsEnabled()
{
    return m_pRaylaseCardImpl->PilotIsEnabled();
}

void CRaylaseCard::GetLaserStatus(bool & bPilotIsEnabled, bool & bLaserIsArmed, bool & bLaserAlarm)
{
    m_pRaylaseCardImpl->GetLaserStatus(bPilotIsEnabled, bLaserIsArmed, bLaserAlarm);
}

void CRaylaseCard::DrawLayer(const std::string & sStreamUUID, const LibMCDriver_Raylase_uint32 nLayerIndex)
{
    m_pRaylaseCardImpl->DrawLayer(sStreamUUID, nLayerIndex);
}

bool CRaylaseCard::IsConnected()
{
    return m_pRaylaseCardImpl->IsConnected();
}

void CRaylaseCard::Disconnect()
{
    m_pRaylaseCardImpl->Disconnect();

}