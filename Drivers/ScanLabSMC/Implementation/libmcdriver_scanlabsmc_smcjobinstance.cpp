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
#include "libmcdriver_scanlabsmc_smcsimulationparser.hpp"

// Include custom headers here.
#define SCANLABSMC_MICROSTEPSPERSECOND 100000

#include <array>
#include <thread>
#include <iostream>

using namespace LibMCDriver_ScanLabSMC::Impl;

/*************************************************************************************************************************
 Class definition of CSMCJob
**************************************************************************************************************************/

CSMCJobInstance::CSMCJobInstance(PSMCContextHandle pContextHandle, double dStartPositionX, double dStartPositionY, LibMCDriver_ScanLabSMC::eBlendMode eBlendMode, LibMCEnv::PWorkingDirectory pWorkingDirectory, std::string sSimulationSubDirectory)
    : m_pContextHandle(pContextHandle), 
    m_JobID(0), 
    m_bIsFinalized(false), 
    m_pWorkingDirectory (pWorkingDirectory), 
    m_sSimulationSubDirectory (sSimulationSubDirectory),
    m_bHasJobDuration (false),
    m_dJobDuration (0.0)
{

    if (m_pWorkingDirectory.get() == nullptr)
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPARAM);
    if (m_pContextHandle.get() == nullptr)
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPARAM);

    m_pSDK = m_pContextHandle->getSDK();

    auto contextHandle = m_pContextHandle->getHandle();
    //m_pSDK->checkError(m_pSDK->slsc_cfg_set_blend_mode(contextHandle, (slsc_BlendModes)eBlendMode));
    m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_begin(contextHandle, &m_JobID));

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
    
    auto contextHandle = m_pContextHandle->getHandle();

    m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_end(contextHandle));
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
    m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_jump(contextHandle, startPosition.data()));

    slsc_PolylineOptions polyLineOptions;
    if (bIsClosed) {
        polyLineOptions.Geometry = slsc_PolylineGeometry::slsc_Polyline_Closed;
    }
    else {
        polyLineOptions.Geometry = slsc_PolylineGeometry::slsc_Polyline_Open;
    }
    polyLineOptions.ProfileType = slsc_PolylineProfile::slsc_Maximize_Velocity;
    m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_begin_polyline(contextHandle, polyLineOptions));

    for (size_t nPointIndex = 1; nPointIndex < nPointsBufferSize; nPointIndex++) {
        auto& nextPoint = pPointsBuffer[nPointIndex];
        std::array<double, 2> nextPosition;
        nextPosition[0] = nextPoint.m_X;
        nextPosition[1] = nextPoint.m_Y;
        m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_line(contextHandle, nextPosition.data()));
    }

    m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_end_polyline(contextHandle));
}


void CSMCJobInstance::DrawPolyline(const LibMCDriver_ScanLabSMC_uint64 nPointsBufferSize, const LibMCDriver_ScanLabSMC::sPoint2D* pPointsBuffer, const LibMCDriver_ScanLabSMC_double dMarkSpeed, const LibMCDriver_ScanLabSMC_double dMinimalMarkSpeed, const LibMCDriver_ScanLabSMC_double dJumpSpeed, const LibMCDriver_ScanLabSMC_double dPower, const LibMCDriver_ScanLabSMC_double dCornerTolerance, const LibMCDriver_ScanLabSMC_double dZValue)
{
    if (m_bIsFinalized)
        throw std::runtime_error("Job is already finalized!");

    if (nPointsBufferSize >= 2) {

        auto contextHandle = m_pContextHandle->getHandle();

        m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_set_jump_speed(contextHandle, dJumpSpeed));
        m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_set_mark_speed(contextHandle, dMarkSpeed));
        m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_set_min_mark_speed(contextHandle, dMinimalMarkSpeed));

        drawPolylineEx(contextHandle, nPointsBufferSize, pPointsBuffer, false);


    }

}

void CSMCJobInstance::DrawLoop(const LibMCDriver_ScanLabSMC_uint64 nPointsBufferSize, const LibMCDriver_ScanLabSMC::sPoint2D* pPointsBuffer, const LibMCDriver_ScanLabSMC_double dMarkSpeed, const LibMCDriver_ScanLabSMC_double dMinimalMarkSpeed, const LibMCDriver_ScanLabSMC_double dJumpSpeed, const LibMCDriver_ScanLabSMC_double dPower, const LibMCDriver_ScanLabSMC_double dCornerTolerance, const LibMCDriver_ScanLabSMC_double dZValue)
{
    if (m_bIsFinalized)
        throw std::runtime_error("Job is already finalized!");

    if (nPointsBufferSize >= 2) {

        auto contextHandle = m_pContextHandle->getHandle();

        m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_set_jump_speed(contextHandle, dJumpSpeed));
        m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_set_mark_speed(contextHandle, dMarkSpeed));
        m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_set_min_mark_speed(contextHandle, dMinimalMarkSpeed));
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

        m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_set_jump_speed(contextHandle, dJumpSpeed));
        m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_set_mark_speed(contextHandle, dMarkSpeed));

        for (uint64_t nHatchIndex = 0; nHatchIndex < nHatchesBufferSize; nHatchIndex++) {
            auto& hatch = pHatchesBuffer[nHatchIndex];
            std::array<double, 2> point1;
            point1[0] = hatch.m_X1;
            point1[1] = hatch.m_Y1;

            std::array<double, 2> point2;
            point2[0] = hatch.m_X2;
            point2[1] = hatch.m_Y2;


            m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_jump(contextHandle, point1.data()));
            m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_line(contextHandle, point2.data()));

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

    slsc_ExecState execState1 = slsc_ExecState::slsc_ExecState_NotInitOrError;
    while (execState1 != slsc_ExecState::slsc_ExecState_ReadyForExecution) {
        m_pSDK->checkError(contextHandle, m_pSDK->slsc_ctrl_get_exec_state(contextHandle, &execState1));
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    } 

    //std::cout << "Starting execution" << std::endl;

    m_pSDK->checkError(contextHandle, m_pSDK->slsc_ctrl_start_execution(contextHandle));

    //std::cout << "Waiting for execution finished" << std::endl;

    slsc_ExecState execState2 = slsc_ExecState::slsc_ExecState_Executing;
    while (execState2 == slsc_ExecState::slsc_ExecState_Executing) {
        m_pSDK->checkError(contextHandle, m_pSDK->slsc_ctrl_get_exec_state(contextHandle, &execState2));
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
    m_pSDK->checkError(m_pSDK->slsc_ctrl_stop(m_pContextHandle->getHandle()), LIBMCDRIVER_SCANLABSMC_ERROR_COULDNOTSTOPJOBEXECUTION);
}


double CSMCJobInstance::GetJobCharacteristic(const LibMCDriver_ScanLabSMC::eJobCharacteristic eValueType)
{
    double dResult = 0.0;

    slsc_JobCharacteristic eKey;

    switch (eValueType) {
        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_XPos_ScanAxis:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_XPos_ScanAxis;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_YPos_ScanAxis:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_YPos_ScanAxis;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_ZPos_ScanAxis:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_ZPos_ScanAxis;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_XPos_StageAxis:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_XPos_StageAxis;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_YPos_StageAxis:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_YPos_StageAxis;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_ZPos_StageAxis:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_ZPos_StageAxis;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_XVel_ScanAxis:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_XVel_ScanAxis;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_YVel_ScanAxis:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_YVel_ScanAxis;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_ZVel_ScanAxis:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_ZVel_ScanAxis;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_XVel_StageAxis:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_XVel_StageAxis;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_YVel_StageAxis:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_YVel_StageAxis;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_ZVel_StageAxis:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_ZVel_StageAxis;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_XAcc_ScanAxis:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_XAcc_ScanAxis;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_YAcc_ScanAxis:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_YAcc_ScanAxis;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_ZAcc_ScanAxis:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_ZAcc_ScanAxis;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_XAcc_StageAxis:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_XAcc_StageAxis;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_YAcc_StageAxis:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_YAcc_StageAxis;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_ZAcc_StageAxis:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_ZAcc_StageAxis;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_XJerk_StageAxis:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_XJerk_StageAxis;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_YJerk_StageAxis:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_YJerk_StageAxis;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_ZJerk_StageAxis:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_ZJerk_StageAxis;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_XPos_ScanAxis_LaserOn:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_XPos_ScanAxis_LaserOn;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_YPos_ScanAxis_LaserOn:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_YPos_ScanAxis_LaserOn;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_ZPos_ScanAxis_LaserOn:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_ZPos_ScanAxis_LaserOn;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_XPos_StageAxis_LaserOn:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_XPos_StageAxis_LaserOn;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_YPos_StageAxis_LaserOn:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_YPos_StageAxis_LaserOn;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_ZPos_StageAxis_LaserOn:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_ZPos_StageAxis_LaserOn;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_XPos_ScanAxis_Max:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_XPos_ScanAxis_Max;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_YPos_ScanAxis_Max:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_YPos_ScanAxis_Max;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_ZPos_ScanAxis_Max:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_ZPos_ScanAxis_Max;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_XPos_ScanAxis_Min:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_XPos_ScanAxis_Min;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_YPos_ScanAxis_Min:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_YPos_ScanAxis_Min;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_ZPos_ScanAxis_Min:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_ZPos_ScanAxis_Min;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_XPos_ScanAxis_LaserOn_Max:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_XPos_ScanAxis_LaserOn_Max;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_YPos_ScanAxis_LaserOn_Max:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_YPos_ScanAxis_LaserOn_Max;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_ZPos_ScanAxis_LaserOn_Max:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_ZPos_ScanAxis_LaserOn_Max;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_XPos_ScanAxis_LaserOn_Min:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_XPos_ScanAxis_LaserOn_Min;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_YPos_ScanAxis_LaserOn_Min:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_YPos_ScanAxis_LaserOn_Min;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_ZPos_ScanAxis_LaserOn_Min:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_ZPos_ScanAxis_LaserOn_Min;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_XPos_StageAxis_Max:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_XPos_StageAxis_Max;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_YPos_StageAxis_Max:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_YPos_StageAxis_Max;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_ZPos_StageAxis_Max:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_ZPos_StageAxis_Max;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_XPos_StageAxis_Min:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_XPos_StageAxis_Min;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_YPos_StageAxis_Min:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_YPos_StageAxis_Min;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_ZPos_StageAxis_Min:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_ZPos_StageAxis_Min;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_XPos_StageAxis_LaserOn_Max:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_XPos_StageAxis_LaserOn_Max;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_YPos_StageAxis_LaserOn_Max:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_YPos_StageAxis_LaserOn_Max;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_ZPos_StageAxis_LaserOn_Max:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_ZPos_StageAxis_LaserOn_Max;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_XPos_StageAxis_LaserOn_Min:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_XPos_StageAxis_LaserOn_Min;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_YPos_StageAxis_LaserOn_Min:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_YPos_StageAxis_LaserOn_Min;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::Cart_ZPos_StageAxis_LaserOn_Min:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_Cart_ZPos_StageAxis_LaserOn_Min;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::InsertedSkywritings:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_InsertedSkywritings;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::MotionMicroSteps:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_MotionMicroSteps;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::MinimalMarkSpeed:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_MinimalMarkSpeed;
            break;

        case LibMCDriver_ScanLabSMC::eJobCharacteristic::MaximalMarkSpeed:
            eKey = slsc_JobCharacteristic::slsc_JobCharacteristic_MaximalMarkSpeed;
            break;

        default: 
            throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDJOBCHARACTERISTIC, "Invalid job characteristic: " + std::to_string ((int64_t)eValueType));

    }
    
    m_pSDK->slsc_ctrl_get_job_characteristic (m_pContextHandle->getHandle (), m_JobID, eKey, &dResult);

    return dResult;
}

double CSMCJobInstance::GetJobDuration()
{
    if (!m_bHasJobDuration)
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_JOBDURATIONHASNOTBEENPARSED);

    return m_dJobDuration;

}

void CSMCJobInstance::ReadSimulationFile(LibMCEnv::PDataTable pDataTable)
{
    if (pDataTable.get() == nullptr)
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPARAM);

    auto contextHandle = m_pContextHandle->getHandle();

    std::vector<char> buffer;
    buffer.resize(16384);
    m_pSDK->checkError(contextHandle, m_pSDK->slsc_ctrl_get_simulation_filename(contextHandle, m_JobID, buffer.data (), buffer.size ()));
    buffer.at(buffer.size() - 1) = 0;

    std::string sSimulationFileName(buffer.data());

    std::string sSimulationDirectory = m_pWorkingDirectory->GetAbsoluteFilePath() + "/";
    if (!m_sSimulationSubDirectory.empty ())
        sSimulationDirectory += m_sSimulationSubDirectory + "/";

    CSMCSimulationParser parser(sSimulationDirectory + sSimulationFileName);

    pDataTable->AddColumn("timestamp", "Timestamp", LibMCEnv::eDataTableColumnType::DoubleColumn);
    pDataTable->AddColumn("x", "X", LibMCEnv::eDataTableColumnType::DoubleColumn);
    pDataTable->AddColumn("y", "Y", LibMCEnv::eDataTableColumnType::DoubleColumn);
    pDataTable->AddColumn("laseron", "LaserOn", LibMCEnv::eDataTableColumnType::Int32Column);
    pDataTable->AddColumn("active1", "Active Channel 1", LibMCEnv::eDataTableColumnType::DoubleColumn);
    pDataTable->AddColumn("active2", "Active Channel 2", LibMCEnv::eDataTableColumnType::DoubleColumn);
    pDataTable->AddColumn("cmdindex", "Command Index", LibMCEnv::eDataTableColumnType::Int32Column);

    std::vector<double> timestampValues;
    parser.readTimestampValues(timestampValues);
    pDataTable->SetDoubleColumnValues("timestamp", timestampValues);
    timestampValues.resize(0);

    std::vector<double> xValues;
    parser.readXValues(xValues);
    pDataTable->SetDoubleColumnValues("x", xValues);
    xValues.resize(0);

    std::vector<double> yValues;
    parser.readYValues(yValues);
    pDataTable->SetDoubleColumnValues("y", yValues);
    yValues.resize(0);

    m_dJobDuration = (double)parser.getCount() / (double)SCANLABSMC_MICROSTEPSPERSECOND;
    m_bHasJobDuration = true;
}


void CSMCJobInstance::AddLayerToList(LibMCEnv::PToolpathLayer pLayer)
{
    if (pLayer.get() == nullptr)
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPARAM);

    double dUnits = pLayer->GetUnits();
    double dZValue = 0.0;

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

            double dMinimalMarkSpeed = pLayer->GetSegmentProfileDoubleValueDef(nSegmentIndex, "http://schemas.scanlab.com/smc/2024/10", "minimummarkspeed", dMarkSpeedInMMPerSecond);
            double dCornerTolerance = pLayer->GetSegmentProfileDoubleValueDef(nSegmentIndex, "http://schemas.scanlab.com/smc/2024/10", "cornertolerance", 0.0);

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
                        pContourPoint->m_X = (float)(((double)Points[nPointIndex].m_Coordinates[0]) * dUnits);
                        pContourPoint->m_Y = (float)(((double)Points[nPointIndex].m_Coordinates[1]) * dUnits);
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
                        pHatch->m_X1 = (float)((double)Points[nHatchIndex * 2].m_Coordinates[0] * dUnits);
                        pHatch->m_Y1 = (float)((double)Points[nHatchIndex * 2].m_Coordinates[1] * dUnits);
                        pHatch->m_X2 = (float)((double)Points[nHatchIndex * 2 + 1].m_Coordinates[0] * dUnits);
                        pHatch->m_Y2 = (float)((double)Points[nHatchIndex * 2 + 1].m_Coordinates[1] * dUnits);
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

