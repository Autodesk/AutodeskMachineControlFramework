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

// Include custom headers here.

#include <array>
#include <thread>
#include <iostream>

using namespace LibMCDriver_ScanLabSMC::Impl;

/*************************************************************************************************************************
 Class definition of CSMCJob
**************************************************************************************************************************/

CSMCJob::CSMCJob(PSMCContextHandle pContextHandle, double dStartPositionX, double dStartPositionY, LibMCDriver_ScanLabSMC::eBlendMode eBlendMode)
    : m_pContextHandle(pContextHandle), m_JobID(0), m_bIsFinalized(false)
{
    m_pSDK = m_pContextHandle->getSDK();

    //m_pSDK->checkError(m_pSDK->slsc_job_begin(m_pContextHandle->getHandle(), &m_JobID));
}

CSMCJob::~CSMCJob()
{

}

LibMCDriver_ScanLabSMC_uint64 CSMCJob::GetJobID()
{
    return m_JobID;
}

void CSMCJob::Finalize()
{
    if (m_bIsFinalized)
        throw std::runtime_error("Job is already finalized!");

    //m_pSDK->checkError(m_pSDK->slsc_job_end(m_pContextHandle->getHandle()));
    m_bIsFinalized = true;
}

bool CSMCJob::IsFinalized()
{
    return m_bIsFinalized;
}

void CSMCJob::drawPolylineEx(slscHandle contextHandle, const uint64_t nPointsBufferSize, const LibMCDriver_ScanLabSMC::sPoint2D* pPointsBuffer, bool bIsClosed)
{

    if (nPointsBufferSize < 2)
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPARAM);
    if (pPointsBuffer == nullptr)
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPARAM);

    /*auto& startPoint = pPointsBuffer[0];
    std::array<double, 2> startPosition;
    startPosition[0] = startPoint.m_X;
    startPosition[1] = startPoint.m_Y;
    m_pSDK->checkError(m_pSDK->slsc_job_jump(contextHandle, startPosition.data()));

    slsc_PolylineOptions polyLineOptions;
    if (bIsClosed) {
        polyLineOptions.Geometry = slsc_Polyline_Closed;
    }
    else {
        polyLineOptions.Geometry = slsc_Polyline_Open;
    }
    polyLineOptions.ProfileType = slsc_Maximize_Velocity;
    m_pSDK->checkError(m_pSDK->slsc_job_begin_polyline(contextHandle, polyLineOptions));

    for (size_t nPointIndex = 1; nPointIndex < nPointsBufferSize; nPointIndex++) {
        auto& nextPoint = pPointsBuffer[nPointIndex];
        std::array<double, 2> nextPosition;
        nextPosition[0] = nextPoint.m_X;
        nextPosition[1] = nextPoint.m_Y;
        m_pSDK->checkError(m_pSDK->slsc_job_line(contextHandle, nextPosition.data()));
    }

    m_pSDK->checkError(m_pSDK->slsc_job_end_polyline(contextHandle)); */
}


void CSMCJob::DrawPolyline(const LibMCDriver_ScanLabSMC_uint64 nPointsBufferSize, const LibMCDriver_ScanLabSMC::sPoint2D* pPointsBuffer, const LibMCDriver_ScanLabSMC_double dMarkSpeed, const LibMCDriver_ScanLabSMC_double dMinimalMarkSpeed, const LibMCDriver_ScanLabSMC_double dJumpSpeed, const LibMCDriver_ScanLabSMC_double dPower, const LibMCDriver_ScanLabSMC_double dCornerTolerance, const LibMCDriver_ScanLabSMC_double dZValue)
{
    if (m_bIsFinalized)
        throw std::runtime_error("Job is already finalized!");

    if (nPointsBufferSize >= 2) {



        auto contextHandle = m_pContextHandle->getHandle();

        // Set speed etc..
        drawPolylineEx(contextHandle, nPointsBufferSize, pPointsBuffer, false);


    }

}

void CSMCJob::DrawLoop(const LibMCDriver_ScanLabSMC_uint64 nPointsBufferSize, const LibMCDriver_ScanLabSMC::sPoint2D* pPointsBuffer, const LibMCDriver_ScanLabSMC_double dMarkSpeed, const LibMCDriver_ScanLabSMC_double dMinimalMarkSpeed, const LibMCDriver_ScanLabSMC_double dJumpSpeed, const LibMCDriver_ScanLabSMC_double dPower, const LibMCDriver_ScanLabSMC_double dCornerTolerance, const LibMCDriver_ScanLabSMC_double dZValue)
{
    if (m_bIsFinalized)
        throw std::runtime_error("Job is already finalized!");

    if (nPointsBufferSize >= 2) {

        auto contextHandle = m_pContextHandle->getHandle();

        // Set speed etc..
        drawPolylineEx(contextHandle, nPointsBufferSize, pPointsBuffer, true);


    }

}


void CSMCJob::DrawHatches(const LibMCDriver_ScanLabSMC_uint64 nHatchesBufferSize, const LibMCDriver_ScanLabSMC::sHatch2D* pHatchesBuffer, const LibMCDriver_ScanLabSMC_double dMarkSpeed, const LibMCDriver_ScanLabSMC_double dJumpSpeed, const LibMCDriver_ScanLabSMC_double dPower, const LibMCDriver_ScanLabSMC_double dZValue)
{
    if (m_bIsFinalized)
        throw std::runtime_error("Job is already finalized!");

}

bool CSMCJob::IsReady()
{
    return true;
}

void CSMCJob::Execute(const bool bBlocking)
{
    auto contextHandle = m_pContextHandle->getHandle();

   /* std::cout << "Waiting for execution" << std::endl;

    slsc_ExecState execState1 = slsc_ExecState_NotInitOrError;
    while (execState1 != slsc_ExecState_ReadyForExecution) {
        m_pSDK->checkError(m_pSDK->slsc_ctrl_get_exec_state(contextHandle, &execState1));
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    std::cout << "Starting execution" << std::endl;

    m_pSDK->checkError(m_pSDK->slsc_ctrl_start_execution(contextHandle));

    std::cout << "Waiting for execution finished" << std::endl;

    slsc_ExecState execState2 = slsc_ExecState_Executing;
    while (execState2 == slsc_ExecState_Executing) {
        checkError(slsc_ctrl_get_exec_state(contextHandle, &execState2));
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    } */
}

bool CSMCJob::IsExecuting()
{
    return false;
}

void CSMCJob::WaitForExecution(const LibMCDriver_ScanLabSMC_uint32 nTimeOutInMilliseconds)
{

}

void CSMCJob::StopExecution()
{

}

