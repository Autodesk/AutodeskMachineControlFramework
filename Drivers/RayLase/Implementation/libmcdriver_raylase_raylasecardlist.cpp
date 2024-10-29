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


CRaylaseCardList::CRaylaseCardList(PRaylaseSDK pSDK, rlHandle cardHandle, double dMaxLaserPowerInWatts)
    : m_pSDK(pSDK), 
    m_ListHandle(0), 
    m_CardHandle(cardHandle), 
    m_dMaxLaserPowerInWatts(dMaxLaserPowerInWatts),
    m_nListIDOnCard (RAYLASE_LISTONCARDNOTSET)

{
    if (pSDK.get() == nullptr)
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDPARAM);

    m_ListHandle = m_pSDK->rlListAllocate(m_CardHandle);
}

CRaylaseCardList::~CRaylaseCardList()
{
    if ((m_pSDK.get() != nullptr) && (m_ListHandle != 0)) {
        if (m_nListIDOnCard != RAYLASE_LISTONCARDNOTSET)
            m_pSDK->checkError(m_pSDK->rlListDelete(m_CardHandle, m_nListIDOnCard, true), "rlListDelete");
        m_nListIDOnCard = RAYLASE_LISTONCARDNOTSET;

        m_pSDK->checkError(m_pSDK->rlListReleaseHandle(m_ListHandle), "rlListReleaseHandle");
    }

    m_ListHandle = 0;
}

void CRaylaseCardList::addLayerToList(LibMCEnv::PToolpathLayer pLayer, uint32_t nLaserIndexFilter, bool bFailIfNonAssignedDataExists)
{
    if (pLayer.get() == nullptr)
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDPARAM);

    double dUnits = pLayer->GetUnits();

    m_pSDK->checkError(m_pSDK->rlListAppendLaserOn(m_ListHandle), "rlListAppendLaserOn");

    uint32_t nSegmentCount = pLayer->GetSegmentCount();
    for (uint32_t nSegmentIndex = 0; nSegmentIndex < nSegmentCount; nSegmentIndex++) {

        LibMCEnv::eToolpathSegmentType eSegmentType;
        uint32_t nPointCount;
        pLayer->GetSegmentInfo(nSegmentIndex, eSegmentType, nPointCount);

        bool bDrawSegment = true;

        // Check for laser index in file.
        int64_t nLaserIndexOfSegment = pLayer->GetSegmentProfileIntegerValueDef(nSegmentIndex, "", "laserindex", 0);
        if (nLaserIndexOfSegment == 0) {
            if (bFailIfNonAssignedDataExists)
                throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_SEGMENTHASNOASSIGNEDCARD, "Segment has no assigned card: " + std::to_string(nLaserIndexOfSegment));
        }

        // Only draw segments of current laser index
        if (nLaserIndexFilter != 0) {
            if (nLaserIndexOfSegment != nLaserIndexFilter)
                bDrawSegment = false;
        }



        if ((nPointCount >= 2) && bDrawSegment) {

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

            m_pSDK->checkError(m_pSDK->rlListAppendJumpSpeed(m_ListHandle, dJumpSpeedInMeterPerSecond), "rlListAppendJumpSpeed");
            m_pSDK->checkError(m_pSDK->rlListAppendMarkSpeed(m_ListHandle, dMarkSpeedInMeterPerSecond), "rlListAppendMarkSpeed");
            m_pSDK->checkError(m_pSDK->rlListAppendPower(m_ListHandle, nPowerInUnits, eRLPowerChannels::ptcAllChannels), "rlListAppendPower");

            switch (eSegmentType) {
            case LibMCEnv::eToolpathSegmentType::Loop:
            case LibMCEnv::eToolpathSegmentType::Polyline:
            {

                std::vector<LibMCEnv::sFloatPosition2D> PointsInMM;
                pLayer->GetSegmentPointDataInMM(nSegmentIndex, PointsInMM);

                if (nPointCount != PointsInMM.size())
                    throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDPOINTCOUNT);

                for (uint32_t nPointIndex = 0; nPointIndex < nPointCount; nPointIndex++) {
                    double dXinMM = (PointsInMM[nPointIndex].m_Coordinates[0]);
                    double dYinMM = (PointsInMM[nPointIndex].m_Coordinates[1]);

                    double dXinMicron = dXinMM * 1000.0;
                    double dYinMicron = dYinMM * 1000.0;

                    if (nPointIndex == 0) {
                        m_pSDK->checkError(m_pSDK->rlListAppendJumpAbs2D(m_ListHandle, dXinMicron, dYinMicron), "rlListAppendJumpAbs2D");
                    }
                    else {
                        m_pSDK->checkError(m_pSDK->rlListAppendMarkAbs2D(m_ListHandle, dXinMicron, dYinMicron), "rlListAppendMarkAbs2D");
                    }

                }


                break;
            }

            case LibMCEnv::eToolpathSegmentType::Hatch:
            {
                if (nPointCount % 2 == 1)
                    throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDPOINTCOUNT);

                std::vector<LibMCEnv::sFloatHatch2D> HatchesInMM;

                uint64_t nHatchCount = nPointCount / 2;
                pLayer->GetSegmentHatchDataInMM(nSegmentIndex, HatchesInMM);

                if (nHatchCount != HatchesInMM.size())
                    throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDPOINTCOUNT);

                for (uint64_t nHatchIndex = 0; nHatchIndex < nHatchCount; nHatchIndex++) {
                    double dX1inMM = HatchesInMM[nHatchIndex].m_X1;
                    double dY1inMM = HatchesInMM[nHatchIndex].m_Y1;
                    double dX2inMM = HatchesInMM[nHatchIndex].m_X2;
                    double dY2inMM = HatchesInMM[nHatchIndex].m_Y2;

                    double dX1inMicron = dX1inMM * 1000.0;
                    double dY1inMicron = dY1inMM * 1000.0;
                    double dX2inMicron = dX2inMM * 1000.0;
                    double dY2inMicron = dY2inMM * 1000.0;

                    m_pSDK->checkError(m_pSDK->rlListAppendJumpAbs2D(m_ListHandle, dX1inMicron, dY1inMicron), "rlListAppendJumpAbs2D");
                    m_pSDK->checkError(m_pSDK->rlListAppendMarkAbs2D(m_ListHandle, dX2inMicron, dY2inMicron), "rlListAppendMarkAbs2D");
                }

                break;
            }

            }

        }

    }

    m_pSDK->checkError(m_pSDK->rlListAppendLaserOff(m_ListHandle), "rlListAppendLaserOff");

}

void CRaylaseCardList::setListOnCard(uint32_t nListIDOnCard)
{
    if (m_nListIDOnCard != RAYLASE_LISTONCARDNOTSET)
        deleteListListOnCard();

    if (nListIDOnCard >= RAYLASE_MAXLISTONCARDID)
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDLISTID);

    m_pSDK->checkError(m_pSDK->rlListSet(m_CardHandle, nListIDOnCard, m_ListHandle, false, -1), "rlListSet");
    m_nListIDOnCard = nListIDOnCard;

}

void CRaylaseCardList::deleteListListOnCard ()
{
    if (m_nListIDOnCard != RAYLASE_LISTONCARDNOTSET)
        m_pSDK->checkError(m_pSDK->rlListDelete(m_CardHandle, m_nListIDOnCard, true), "rlListDelete");
    m_nListIDOnCard = RAYLASE_LISTONCARDNOTSET;
}

void CRaylaseCardList::executeList(uint32_t nListIDOnCard)
{
    if (nListIDOnCard >= RAYLASE_MAXLISTONCARDID)
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDLISTID);

    m_pSDK->checkError(m_pSDK->rlListExecute(m_CardHandle, nListIDOnCard), "rlListExecute");

}

bool CRaylaseCardList::waitForExecution(uint32_t nTimeOutInMS)
{
    bool done = false;
    int32_t listID = 0;
    m_pSDK->checkError(m_pSDK->rlListWaitForListDone(m_CardHandle, nTimeOutInMS, done, listID), "rlListWaitForListDone");

    return done;

}


