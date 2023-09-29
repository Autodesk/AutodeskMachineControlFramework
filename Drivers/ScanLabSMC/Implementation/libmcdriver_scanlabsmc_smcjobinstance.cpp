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


Abstract: This is a stub class definition of CSMCJob

*/

#include "libmcdriver_scanlabsmc_smcjob.hpp"
#include "libmcdriver_scanlabsmc_interfaceexception.hpp"
#include "libmcdriver_scanlabsmc_sdk.hpp"

// Include custom headers here.

#include <array>
#include <thread>
#include <iostream>

using namespace LibMCDriver_ScanLabSMC::Impl;

/*************************************************************************************************************************
 Class definition of CSMCJob
**************************************************************************************************************************/

CSMCJobInstance::CSMCJobInstance(PSMCContextHandle pContextHandle, double dStartPositionX, double dStartPositionY, LibMCDriver_ScanLabSMC::eBlendMode eBlendMode)
    : m_pContextHandle(pContextHandle), m_JobID(0), m_bIsFinalized(false)
{
    m_pSDK = m_pContextHandle->getSDK();

    //m_pSDK->checkError(m_pSDK->slsc_cfg_set_blend_mode(contextHandle, (slsc_BlendModes)eBlendMode));
    m_pSDK->checkError(m_pSDK->slsc_job_begin(m_pContextHandle->getHandle(), &m_JobID));
    auto contextHandle = m_pContextHandle->getHandle();

}

CSMCJobInstance::~CSMCJobInstance()
{

}

LibMCDriver_ScanLabSMC_uint64 CSMCJobInstance::GetJobID()
{
    return m_JobID;
}

void CSMCJobInstance::Finalize()
{
    if (m_bIsFinalized)
        throw std::runtime_error("Job is already finalized!");

    m_pSDK->checkError(m_pSDK->slsc_job_end(m_pContextHandle->getHandle()));
    m_bIsFinalized = true;
}

bool CSMCJobInstance::IsFinalized()
{
    return m_bIsFinalized;
}

void CSMCJobInstance::drawPolylineEx(slscHandle contextHandle, const uint64_t nPointsBufferSize, const LibMCDriver_ScanLabSMC::sPoint2D* pPointsBuffer, bool bIsClosed)
{

    if (nPointsBufferSize < 2)
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPARAM);
    if (pPointsBuffer == nullptr)
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPARAM);

    auto& startPoint = pPointsBuffer[0];
    std::array<double, 2> startPosition;
    startPosition[0] = startPoint.m_X;
    startPosition[1] = startPoint.m_Y;
    m_pSDK->checkError(m_pSDK->slsc_job_jump(contextHandle, startPosition.data()));

    slsc_PolylineOptions polyLineOptions;
    if (bIsClosed) {
        polyLineOptions.Geometry = slsc_PolylineGeometry::slsc_Polyline_Closed;
    }
    else {
        polyLineOptions.Geometry = slsc_PolylineGeometry::slsc_Polyline_Open;
    }
    polyLineOptions.ProfileType = slsc_PolylineProfile::slsc_Maximize_Velocity;
    m_pSDK->checkError(m_pSDK->slsc_job_begin_polyline(contextHandle, polyLineOptions));

    for (size_t nPointIndex = 1; nPointIndex < nPointsBufferSize; nPointIndex++) {
        auto& nextPoint = pPointsBuffer[nPointIndex];
        std::array<double, 2> nextPosition;
        nextPosition[0] = nextPoint.m_X;
        nextPosition[1] = nextPoint.m_Y;
        m_pSDK->checkError(m_pSDK->slsc_job_line(contextHandle, nextPosition.data()));
    }

    m_pSDK->checkError(m_pSDK->slsc_job_end_polyline(contextHandle)); 
}


void CSMCJobInstance::DrawPolyline(const LibMCDriver_ScanLabSMC_uint64 nPointsBufferSize, const LibMCDriver_ScanLabSMC::sPoint2D* pPointsBuffer, const LibMCDriver_ScanLabSMC_double dMarkSpeed, const LibMCDriver_ScanLabSMC_double dMinimalMarkSpeed, const LibMCDriver_ScanLabSMC_double dJumpSpeed, const LibMCDriver_ScanLabSMC_double dPower, const LibMCDriver_ScanLabSMC_double dCornerTolerance, const LibMCDriver_ScanLabSMC_double dZValue)
{
    if (m_bIsFinalized)
        throw std::runtime_error("Job is already finalized!");

    if (nPointsBufferSize >= 2) {

        auto contextHandle = m_pContextHandle->getHandle();

        m_pSDK->checkError(m_pSDK->slsc_job_set_jump_speed(contextHandle, dJumpSpeed));
        m_pSDK->checkError(m_pSDK->slsc_job_set_mark_speed(contextHandle, dMarkSpeed));
        m_pSDK->checkError(m_pSDK->slsc_job_set_min_mark_speed(contextHandle, dMinimalMarkSpeed));

        drawPolylineEx(contextHandle, nPointsBufferSize, pPointsBuffer, false);


    }

}

void CSMCJobInstance::DrawLoop(const LibMCDriver_ScanLabSMC_uint64 nPointsBufferSize, const LibMCDriver_ScanLabSMC::sPoint2D* pPointsBuffer, const LibMCDriver_ScanLabSMC_double dMarkSpeed, const LibMCDriver_ScanLabSMC_double dMinimalMarkSpeed, const LibMCDriver_ScanLabSMC_double dJumpSpeed, const LibMCDriver_ScanLabSMC_double dPower, const LibMCDriver_ScanLabSMC_double dCornerTolerance, const LibMCDriver_ScanLabSMC_double dZValue)
{
    if (m_bIsFinalized)
        throw std::runtime_error("Job is already finalized!");

    if (nPointsBufferSize >= 2) {

        auto contextHandle = m_pContextHandle->getHandle();

        m_pSDK->checkError(m_pSDK->slsc_job_set_jump_speed(contextHandle, dJumpSpeed));
        m_pSDK->checkError(m_pSDK->slsc_job_set_mark_speed(contextHandle, dMarkSpeed));
        m_pSDK->checkError(m_pSDK->slsc_job_set_min_mark_speed(contextHandle, dMinimalMarkSpeed));
        drawPolylineEx(contextHandle, nPointsBufferSize, pPointsBuffer, true);


    }

}


void CSMCJobInstance::DrawHatches(const LibMCDriver_ScanLabSMC_uint64 nHatchesBufferSize, const LibMCDriver_ScanLabSMC::sHatch2D* pHatchesBuffer, const LibMCDriver_ScanLabSMC_double dMarkSpeed, const LibMCDriver_ScanLabSMC_double dJumpSpeed, const LibMCDriver_ScanLabSMC_double dPower, const LibMCDriver_ScanLabSMC_double dZValue)
{
    if (m_bIsFinalized)
        throw std::runtime_error("Job is already finalized!");

    if (nHatchesBufferSize > 0) {
        if (pHatchesBuffer == nullptr)
            throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPARAM);

        auto contextHandle = m_pContextHandle->getHandle();

        m_pSDK->checkError(m_pSDK->slsc_job_set_jump_speed(contextHandle, dJumpSpeed));
        m_pSDK->checkError(m_pSDK->slsc_job_set_mark_speed(contextHandle, dMarkSpeed));

        for (uint64_t nHatchIndex = 0; nHatchIndex < nHatchesBufferSize; nHatchIndex++) {
            auto& hatch = pHatchesBuffer[nHatchIndex];
            std::array<double, 2> point1;
            point1[0] = hatch.m_X1;
            point1[1] = hatch.m_Y1;

            std::array<double, 2> point2;
            point2[0] = hatch.m_X2;
            point2[1] = hatch.m_Y2;


            m_pSDK->checkError(m_pSDK->slsc_job_jump(contextHandle, point1.data()));
            m_pSDK->checkError(m_pSDK->slsc_job_line(contextHandle, point2.data()));

        }
    }

}

bool CSMCJobInstance::IsReady()
{
    return true;
}

void CSMCJobInstance::Execute(const bool bBlocking)
{
    auto contextHandle = m_pContextHandle->getHandle();

    std::cout << "Waiting for execution" << std::endl;

    slsc_ExecState execState1 = slsc_ExecState_NotInitOrError;
    while (execState1 != slsc_ExecState_ReadyForExecution) {
        m_pSDK->checkError(m_pSDK->slsc_ctrl_get_exec_state(contextHandle, &execState1));
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    } 

    //std::cout << "Starting execution" << std::endl;

    m_pSDK->checkError(m_pSDK->slsc_ctrl_start_execution(contextHandle));

    //std::cout << "Waiting for execution finished" << std::endl;

    slsc_ExecState execState2 = slsc_ExecState_Executing;
    while (execState2 == slsc_ExecState_Executing) {
        m_pSDK->checkError(m_pSDK->slsc_ctrl_get_exec_state(contextHandle, &execState2));
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    } 
}

bool CSMCJobInstance::IsExecuting()
{
    return false;
}

void CSMCJobInstance::WaitForExecution(const LibMCDriver_ScanLabSMC_uint32 nTimeOutInMilliseconds)
{

}

void CSMCJobInstance::StopExecution()
{

}

void CSMCJobInstance::AddLayerToList(LibMCEnv::PToolpathLayer pLayer)
{
    if (pLayer.get() == nullptr)
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPARAM);

    double dUnits = pLayer->GetUnits();
    double dZValue = 0.0;
    double dMinimalMarkSpeed = 0.0;
    double dCornerTolerance = 0.1;

    uint32_t nSegmentCount = pLayer->GetSegmentCount();
    for (uint32_t nSegmentIndex = 0; nSegmentIndex < nSegmentCount; nSegmentIndex++) {

        LibMCEnv::eToolpathSegmentType eSegmentType;
        uint32_t nPointCount;
        pLayer->GetSegmentInfo(nSegmentIndex, eSegmentType, nPointCount);

        bool bDrawSegment = true;
        if (bDrawSegment && (nPointCount >= 2)) {

            double dJumpSpeedInMMPerSecond = pLayer->GetSegmentProfileTypedValue(nSegmentIndex, LibMCEnv::eToolpathProfileValueType::JumpSpeed);
            double dMarkSpeedInMMPerSecond = pLayer->GetSegmentProfileTypedValue(nSegmentIndex, LibMCEnv::eToolpathProfileValueType::Speed);
            double dPowerInWatts = pLayer->GetSegmentProfileTypedValue(nSegmentIndex, LibMCEnv::eToolpathProfileValueType::LaserPower);
            double dPowerFactor = dPowerInWatts / 400.0;
            //int64_t nLaserIndexToDraw = pLayer->GetSegmentProfileIntegerValueDef(nSegmentIndex, "", "laserindex", 0);

                std::vector<LibMCEnv::sPosition2D> Points;
                pLayer->GetSegmentPointData(nSegmentIndex, Points);

                if (nPointCount != Points.size())
                    throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPOINTCOUNT);

                switch (eSegmentType) {
                case LibMCEnv::eToolpathSegmentType::Loop:
                case LibMCEnv::eToolpathSegmentType::Polyline:
                {

                    std::vector<LibMCDriver_ScanLabSMC::sPoint2D> ContourPoints;
                    ContourPoints.resize(nPointCount);

                    for (uint32_t nPointIndex = 0; nPointIndex < nPointCount; nPointIndex++) {
                        auto pContourPoint = &ContourPoints.at(nPointIndex);
                        pContourPoint->m_X = (double)(Points[nPointIndex].m_Coordinates[0] * dUnits);
                        pContourPoint->m_Y = (double)(Points[nPointIndex].m_Coordinates[1] * dUnits);
                    } 

                    if (ContourPoints.size() > 0) {
                        if (eSegmentType == LibMCEnv::eToolpathSegmentType::Loop) {
                            this->DrawLoop(ContourPoints.size(), ContourPoints.data(), dMarkSpeedInMMPerSecond, dMinimalMarkSpeed, dJumpSpeedInMMPerSecond, dPowerFactor, dCornerTolerance, dZValue);
                        }
                        else {
                            this->DrawPolyline(ContourPoints.size(), ContourPoints.data(), dMarkSpeedInMMPerSecond, dMinimalMarkSpeed, dJumpSpeedInMMPerSecond, dPowerFactor, dCornerTolerance, dZValue);
                        }
                        
                    }

                    break;
                }

                case LibMCEnv::eToolpathSegmentType::Hatch:
                {
                    if (nPointCount % 2 == 1)
                        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPOINTCOUNT);

                    uint64_t nHatchCount = nPointCount / 2;
                    std::vector<LibMCDriver_ScanLabSMC::sHatch2D> Hatches;
                    Hatches.resize(nHatchCount);

                    for (uint64_t nHatchIndex = 0; nHatchIndex < nHatchCount; nHatchIndex++) {
                        auto pHatch = &Hatches.at(nHatchIndex);
                        pHatch->m_X1 = (double)(Points[nHatchIndex * 2].m_Coordinates[0] * dUnits);
                        pHatch->m_Y1 = (double)(Points[nHatchIndex * 2].m_Coordinates[1] * dUnits);
                        pHatch->m_X2 = (double)(Points[nHatchIndex * 2 + 1].m_Coordinates[0] * dUnits);
                        pHatch->m_Y2 = (double)(Points[nHatchIndex * 2 + 1].m_Coordinates[1] * dUnits);
                    }

                    if (Hatches.size() > 0) {
                        this->DrawHatches(Hatches.size(), Hatches.data(), dMarkSpeedInMMPerSecond, dJumpSpeedInMMPerSecond, dPowerFactor, dZValue);
                    }

                    break;
                }

                
            }

        }

    }

}

