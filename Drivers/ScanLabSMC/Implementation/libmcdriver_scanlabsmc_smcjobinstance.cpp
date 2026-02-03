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
#include "libmcdriver_scanlabsmc_smccsvparser.hpp"

// Include custom headers here.
#define SCANLABSMC_MICROSTEPSPERSECOND 100000
#define SCANLABSMC_MIN_MAXPOWERINWATTS 1.0
#define SCANLABSMC_MAX_MAXPOWERINWATTS 1000000.0

#include <array>
#include <thread>
#include <cmath>
#include <iostream>
#include <cstring>
#include <chrono>

using namespace LibMCDriver_ScanLabSMC::Impl;

/*************************************************************************************************************************
 Class definition of CSMCJob
**************************************************************************************************************************/

CSMCJobInstance::CSMCJobInstance(PSMCContextHandle pContextHandle, double dStartPositionX, double dStartPositionY, LibMCEnv::PWorkingDirectory pWorkingDirectory, std::string sSimulationSubDirectory, bool bSendToHardware, double dMaxPowerInWatts)
    : m_pContextHandle(pContextHandle), 
    m_JobID(0), 
    m_bIsFinalized(false), 
    m_pWorkingDirectory (pWorkingDirectory), 
    m_sSimulationSubDirectory (sSimulationSubDirectory),
    m_bHasJobDuration (false),
    m_dJobDuration (0.0),
    m_bSendToHardware (bSendToHardware),
    m_dMaxPowerInWatts (dMaxPowerInWatts)
{
    if (m_pWorkingDirectory.get() == nullptr)
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPARAM);
    if (m_pContextHandle.get() == nullptr)
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPARAM);

    m_pSDK = m_pContextHandle->getSDK();    

    auto contextHandle = m_pContextHandle->getHandle();

    if (m_bSendToHardware) {

        // Initialize digital output to 0
        m_pSDK->checkError(contextHandle, m_pSDK->slsc_ctrl_write_digital_x(contextHandle, slsc_DigitalOutput::slsc_DigitalOutput_1, 0));

        // Begin job
        m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_begin(contextHandle, &m_JobID));

        // Set digital output to 1
        m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_write_digital_x(contextHandle, slsc_DigitalOutput::slsc_DigitalOutput_1, 1, 0));

        slsc_RecordSet eRecordSetA = slsc_RecordSet::slsc_RecordSet_SetPositions;
        slsc_RecordSet eRecordSetB = slsc_RecordSet::slsc_RecordSet_LaserSwitches;

        m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_start_record(contextHandle, eRecordSetA, eRecordSetB));
    }
    else
    {
        // To start Module recording at start position (0,0).
        size_t JobID = 0;
        double StartPosition[2] = { 0.0, 0.0 };
            
        m_tmpSimulationFile = m_pWorkingDirectory->AddManagedTempFile("slm");

		auto sSimulationFileName = m_tmpSimulationFile->GetAbsoluteFileName();

        std::cout << "Starting planning layer : module file name : " << sSimulationFileName << std::endl;
        m_pSDK->slsc_job_begin_module(contextHandle, &JobID, StartPosition, sSimulationFileName.c_str());
    }


    if (dMaxPowerInWatts < SCANLABSMC_MIN_MAXPOWERINWATTS || dMaxPowerInWatts > SCANLABSMC_MAX_MAXPOWERINWATTS)
		throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDMAXPOWERVALUE);
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

    // Set digital output to 0
    m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_write_digital_x(contextHandle, slsc_DigitalOutput::slsc_DigitalOutput_1, 0, 0));

	// Stop recording
    m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_stop_record(contextHandle));

    m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_end(contextHandle));
    m_bIsFinalized = true;
}

bool CSMCJobInstance::IsFinalized()
{
    return m_bIsFinalized;
}

void CSMCJobInstance::drawPolylineEx(slscHandle contextHandle, const uint64_t nPointsBufferSize, const LibMCDriver_ScanLabSMC::sPoint2D* pPointsBuffer, bool bIsClosed, double dPowerInWatts)
{

    if (nPointsBufferSize < 2)
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPARAM);
    if (pPointsBuffer == nullptr)
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPARAM);

    double dPowerFactor = dPowerInWatts / m_dMaxPowerInWatts;
    m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_write_analog_x(contextHandle, slsc_AnalogOutput::slsc_AnalogOutput_1, dPowerFactor, 0.0));

    auto& startPoint = pPointsBuffer[0];
    std::array<double, 3> startPosition;
    startPosition[0] = startPoint.m_X;
    startPosition[1] = startPoint.m_Y;
    startPosition[2] = 0.0;
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
        std::array<double, 3> nextPosition;
        nextPosition[0] = nextPoint.m_X;
        nextPosition[1] = nextPoint.m_Y;
		nextPosition[2] = 0.0;
        m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_line(contextHandle, nextPosition.data()));
    }

    m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_end_polyline(contextHandle));
}


void CSMCJobInstance::DrawPolyline(const LibMCDriver_ScanLabSMC_uint64 nPointsBufferSize, const LibMCDriver_ScanLabSMC::sPoint2D* pPointsBuffer, const LibMCDriver_ScanLabSMC_double dMarkSpeed, const LibMCDriver_ScanLabSMC_double dMinimalMarkSpeed, const LibMCDriver_ScanLabSMC_double dJumpSpeed, const LibMCDriver_ScanLabSMC_double dPowerInWatts, const LibMCDriver_ScanLabSMC_double dCornerTolerance, const LibMCDriver_ScanLabSMC_double dZValue)
{
    if (m_bIsFinalized)
        throw std::runtime_error("Job is already finalized!");

    if (nPointsBufferSize >= 2) {

        auto contextHandle = m_pContextHandle->getHandle();

        m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_set_jump_speed(contextHandle, dJumpSpeed));
        m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_set_mark_speed(contextHandle, dMarkSpeed));
        m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_set_min_mark_speed(contextHandle, dMinimalMarkSpeed));

        drawPolylineEx(contextHandle, nPointsBufferSize, pPointsBuffer, false, dPowerInWatts);


    }

}

void CSMCJobInstance::DrawLoop(const LibMCDriver_ScanLabSMC_uint64 nPointsBufferSize, const LibMCDriver_ScanLabSMC::sPoint2D* pPointsBuffer, const LibMCDriver_ScanLabSMC_double dMarkSpeed, const LibMCDriver_ScanLabSMC_double dMinimalMarkSpeed, const LibMCDriver_ScanLabSMC_double dJumpSpeed, const LibMCDriver_ScanLabSMC_double dPowerInWatts, const LibMCDriver_ScanLabSMC_double dCornerTolerance, const LibMCDriver_ScanLabSMC_double dZValue)
{
    if (m_bIsFinalized)
        throw std::runtime_error("Job is already finalized!");

    if (nPointsBufferSize >= 2) {

        auto contextHandle = m_pContextHandle->getHandle();

        m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_set_jump_speed(contextHandle, dJumpSpeed));
        m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_set_mark_speed(contextHandle, dMarkSpeed));
        m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_set_min_mark_speed(contextHandle, dMinimalMarkSpeed));
        
        drawPolylineEx(contextHandle, nPointsBufferSize, pPointsBuffer, true, dPowerInWatts);


    }

}


void CSMCJobInstance::DrawHatches(const LibMCDriver_ScanLabSMC_uint64 nHatchesBufferSize, const LibMCDriver_ScanLabSMC::sHatch2D* pHatchesBuffer, const LibMCDriver_ScanLabSMC_double dMarkSpeed, const LibMCDriver_ScanLabSMC_double dJumpSpeed, const LibMCDriver_ScanLabSMC_double dPowerInWatts, const LibMCDriver_ScanLabSMC_double dZValue)
{    
    drawHatchesEx(nHatchesBufferSize, pHatchesBuffer, dMarkSpeed, dJumpSpeed, dPowerInWatts, dZValue);
}

void CSMCJobInstance::drawHatchesEx(const LibMCDriver_ScanLabSMC_uint64 nHatchesBufferSize, const LibMCDriver_ScanLabSMC::sHatch2D* pHatchesBuffer, const LibMCDriver_ScanLabSMC_double dMarkSpeed, const LibMCDriver_ScanLabSMC_double dJumpSpeed, const LibMCDriver_ScanLabSMC_double dPowerInWatts, const LibMCDriver_ScanLabSMC_double dZValue)
{
    if (m_bIsFinalized)
        throw std::runtime_error("Job is already finalized!");

    //std::array<double, 1> paraPower;
    double dPowerFactor = dPowerInWatts / m_dMaxPowerInWatts;

	//std::cout << "drawing hatches with laser power " << dPowerInWatts << " and max power " << m_dMaxPowerInWatts << std::endl;

    if (nHatchesBufferSize > 0) {
        if (pHatchesBuffer == nullptr)
            throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPARAM);

        auto contextHandle = m_pContextHandle->getHandle();

        m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_set_jump_speed(contextHandle, dJumpSpeed));
        m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_set_mark_speed(contextHandle, dMarkSpeed));

        for (uint64_t nHatchIndex = 0; nHatchIndex < nHatchesBufferSize; nHatchIndex++) {
            auto& hatch = pHatchesBuffer[nHatchIndex];
            std::array<double, 3> point1;
            point1[0] = hatch.m_X1;
            point1[1] = hatch.m_Y1;
            point1[2] = 0.0;

            std::array<double, 3> point2;
            point2[0] = hatch.m_X2;
            point2[1] = hatch.m_Y2;
            point2[2] = 0.0;
                        
            std::array<double, 1> paraPower;
            paraPower[0] = dPowerFactor;

            m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_jump(contextHandle, point1.data()));
            m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_para_enable(contextHandle, paraPower.data()));
            m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_para_line(contextHandle, point2.data(), paraPower.data()));
        }
    }

}

void CSMCJobInstance::drawHatchesExLinearPower(const LibMCDriver_ScanLabSMC_uint64 nHatchesBufferSize, const LibMCDriver_ScanLabSMC::sHatch2D* pHatchesBuffer, const LibMCDriver_ScanLabSMC_double dMarkSpeed, const LibMCDriver_ScanLabSMC_double dJumpSpeed, const LibMCDriver_ScanLabSMC_double dPowerInWatts, const LibMCDriver_ScanLabSMC_double dZValue, std::vector<double>& PowerValuesInWatts1, std::vector<double>& PowerValuesInWatts2)
{
    if (m_bIsFinalized)
        throw std::runtime_error("Job is already finalized!");

    if (nHatchesBufferSize > 0) {
        if (pHatchesBuffer == nullptr)
            throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPARAM);

        if (PowerValuesInWatts1.size () != nHatchesBufferSize)
            throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_LINEARPOWERVALUESAREINCOMPLETE);
        if (PowerValuesInWatts2.size() != nHatchesBufferSize)
            throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_LINEARPOWERVALUESAREINCOMPLETE);

        auto contextHandle = m_pContextHandle->getHandle();

        m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_set_jump_speed(contextHandle, dJumpSpeed));
        m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_set_mark_speed(contextHandle, dMarkSpeed));

        for (uint64_t nHatchIndex = 0; nHatchIndex < nHatchesBufferSize; nHatchIndex++) {
            auto& hatch = pHatchesBuffer[nHatchIndex];
            std::array<double, 3> point1;
            point1[0] = hatch.m_X1;
            point1[1] = hatch.m_Y1;
            point1[2] = 0.0;

            std::array<double, 3> point2;
            point2[0] = hatch.m_X2;
            point2[1] = hatch.m_Y2;
            point2[2] = 0.0;

            std::array<double, 1> paraPower1;
            paraPower1[0] = PowerValuesInWatts1.at (nHatchIndex) / m_dMaxPowerInWatts;

            std::array<double, 1> paraPower2;
            paraPower2[0] = PowerValuesInWatts2.at(nHatchIndex) / m_dMaxPowerInWatts;


            m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_jump(contextHandle, point1.data()));
            m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_para_enable(contextHandle, paraPower1.data()));
            m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_para_line(contextHandle, point2.data(), paraPower2.data()));

        }
    }

}

void CSMCJobInstance::drawHatchesExNonLinearPower(const LibMCDriver_ScanLabSMC_uint64 nHatchesBufferSize, const LibMCDriver_ScanLabSMC::sHatch2D* pHatchesBuffer, const LibMCDriver_ScanLabSMC_double dMarkSpeed, const LibMCDriver_ScanLabSMC_double dJumpSpeed, const LibMCDriver_ScanLabSMC_double dPowerInWatts, const LibMCDriver_ScanLabSMC_double dZValue, std::vector<double>& PowerValues1, std::vector<double>& PowerValues2, std::vector<uint32_t> SubInterpolationCounts, std::vector<LibMCEnv::sHatch2DSubInterpolationData> SubInterpolationData)
{
    if (m_bIsFinalized)
        throw std::runtime_error("Job is already finalized!");

    if (nHatchesBufferSize > 0) {
        if (pHatchesBuffer == nullptr)
            throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPARAM);

        if (PowerValues1.size() != nHatchesBufferSize)
            throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_LINEARPOWERVALUESAREINCOMPLETE);
        if (PowerValues2.size() != nHatchesBufferSize)
            throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_LINEARPOWERVALUESAREINCOMPLETE);
        if (SubInterpolationCounts.size() != nHatchesBufferSize)
            throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_NONLINEARPOWERVALUESAREINCOMPLETE);

        auto contextHandle = m_pContextHandle->getHandle();

        m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_set_jump_speed(contextHandle, dJumpSpeed));
        m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_set_mark_speed(contextHandle, dMarkSpeed));

        uint32_t nTotalInterpolationIndex = 0;

        for (uint64_t nHatchIndex = 0; nHatchIndex < nHatchesBufferSize; nHatchIndex++) {
            auto& hatch = pHatchesBuffer[nHatchIndex];
            std::array<double, 3> point1;
            point1[0] = hatch.m_X1;
            point1[1] = hatch.m_Y1;
            point1[2] = 0.0;

            std::array<double, 3> point2;
            point2[0] = hatch.m_X2;
            point2[1] = hatch.m_Y2;
            point2[2] = 0.0;

            double dX = point2[0] - point1[0];
            double dY = point2[1] - point1[1];
            double dLen = sqrt(dX * dX + dY * dY);

            double dLastSection = 0.0;

            std::array<double, 1> paraPower1;
            paraPower1[0] = PowerValues1.at(nHatchIndex) / m_dMaxPowerInWatts;

            uint32_t nSubinterpolationCount = SubInterpolationCounts.at(nHatchIndex);

            std::vector<slsc_ParaSection> paraSections (nSubinterpolationCount + 1);
            for (uint32_t nSubinterpolationIndex = 0; nSubinterpolationIndex < nSubinterpolationCount; nSubinterpolationIndex++) {

                auto & interpolationData = SubInterpolationData.at (nTotalInterpolationIndex);

                double dCurrentSection = interpolationData.m_Parameter * dLen;
                double dSectionDelta = dCurrentSection - dLastSection;

                if (dSectionDelta < 0.0)
                    throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INTERPOLATIONDATAISNOTINCREASING, "interpolation data is not increasing: " + std::to_string (dLastSection) + " MUST be smaller than " + std::to_string (dCurrentSection));

                auto& section = paraSections.at(nSubinterpolationIndex);
                section.m_dS = dSectionDelta;
                section.m_dParaTargetFactor = interpolationData.m_Value / m_dMaxPowerInWatts;

                dLastSection = dCurrentSection;
                nTotalInterpolationIndex++;
            }

            double dSectionDelta = dLen - dLastSection;
            auto& section = paraSections.at(nSubinterpolationCount);
            section.m_dS = dSectionDelta;
            section.m_dParaTargetFactor = PowerValues2.at(nHatchIndex) / m_dMaxPowerInWatts;

            //paraSections.push_back({  });

            slsc_MultiParaTarget multiTarget;
            multiTarget.m_nNumTargets = nSubinterpolationCount + 1;
            multiTarget.m_pTargets = paraSections.data();


            m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_jump(contextHandle, point1.data()));
            m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_para_enable(contextHandle, paraPower1.data()));
            m_pSDK->checkError(contextHandle, m_pSDK->slsc_job_multi_para_line(contextHandle, point2.data(), &multiTarget));

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

    std::cout << "Starting execution" << std::endl;

    m_pSDK->checkError(contextHandle, m_pSDK->slsc_ctrl_start_execution(contextHandle));

    std::cout << "Waiting for execution finished" << std::endl;

    slsc_ExecState execState2 = slsc_ExecState::slsc_ExecState_Executing;
    while (execState2 == slsc_ExecState::slsc_ExecState_Executing) {
        m_pSDK->checkError(contextHandle, m_pSDK->slsc_ctrl_get_exec_state(contextHandle, &execState2));
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    } 

    std::cout << "Execution is finished" << std::endl;
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

void CSMCJobInstance::ExecuteLaserInitSequence()
{
    auto contextHandle = m_pContextHandle->getHandle();

    //std::cout << "Executing laser init sequence" << std::endl;
 
    m_pSDK->checkError(contextHandle, m_pSDK->slsc_ctrl_exec_init_laser_sequence(contextHandle));
}

void CSMCJobInstance::ExecuteLaserShutdownSequence()
{
    auto contextHandle = m_pContextHandle->getHandle();

    //std::cout << "Executing laser shutdown sequence" << std::endl;

    m_pSDK->checkError(contextHandle, m_pSDK->slsc_ctrl_exec_shutdown_laser_sequence(contextHandle));
}

void CSMCJobInstance::ReadSimulationFile(LibMCEnv::PDataTable pDataTable)
{
    slsc_VersionInfo version = m_pSDK->slsc_cfg_get_scanmotioncontrol_version();

    if (version.m_nMajor == 0) {
        if (version.m_nMinor == 8 || version.m_nMinor == 9) {
            ReadSimulationFile_SMC_v0_8(pDataTable);
        }
        else {
            throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_SIMULATIONDATALOADINGISNOTSUPPORTED);
        }
    }
    else if (version.m_nMajor == 1) {
        if (version.m_nMinor == 0 || version.m_nMinor == 1)
        {
            if (m_bSendToHardware)
                ReadLogRecordFile(pDataTable);
            else
                ReadSimulationFile_SMC_v1_0(pDataTable);
        }
        else {
            throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_SIMULATIONDATALOADINGISNOTSUPPORTED);
        }
    }
}

void CSMCJobInstance::ReadSimulationFile_SMC_v0_8(LibMCEnv::PDataTable pDataTable)
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

void CSMCJobInstance::ReadSimulationFile_SMC_v1_0(LibMCEnv::PDataTable pDataTable)
{
    if (pDataTable.get() == nullptr)
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPARAM);

    auto contextHandle = m_pContextHandle->getHandle();

    std::vector<char> buffer;
    buffer.resize(16384);
    m_pSDK->checkError(contextHandle, m_pSDK->slsc_ctrl_get_simulation_filename(contextHandle, m_JobID, buffer.data(), buffer.size()));
    buffer.at(buffer.size() - 1) = 0;

    std::string sSimulationFileName(buffer.data());

    std::string sSimulationDirectory = m_pWorkingDirectory->GetAbsoluteFilePath() + "/";
    if (!m_sSimulationSubDirectory.empty())
        sSimulationDirectory += m_sSimulationSubDirectory + "/";

    CSMCCSVParser parser(sSimulationDirectory + sSimulationFileName, ';');

    std::vector<double>     scanheadX;            // DisplacedX_Galvo_1
    std::vector<double>     scanheadY;            // DisplacedY_Galvo_1
    std::vector<uint32_t>   laserSignal;        // LaserSignal
    std::vector<uint32_t>   laserToggle;        // LaserToggle
    std::vector<double>     activeChannel0;     // ActiveChannel0
    std::vector<double>     activeChannel1;     // ActiveChannel1
    std::vector<int>        cmdCount;           // CommandCount
    std::vector<int>        triggerSignal;      // TriggerSignal
    std::vector<int>        dummy;               //
    std::vector<double>     timestampValues;    //

    std::vector<CSMCCSVParser::FieldBinding> bindings = {
        {{CSMCCSVParser::FieldParserType::Double, CSMCCSVParser::FieldProcessingStep::Extend | CSMCCSVParser::FieldProcessingStep::Interpolate }, &scanheadX},
        {{CSMCCSVParser::FieldParserType::Double, CSMCCSVParser::FieldProcessingStep::Extend | CSMCCSVParser::FieldProcessingStep::Interpolate }, &scanheadY},
        {{CSMCCSVParser::FieldParserType::LaserSignal,CSMCCSVParser::FieldProcessingStep::Nop}, &laserSignal},
        {{CSMCCSVParser::FieldParserType::UInt32,CSMCCSVParser::FieldProcessingStep::Nop}, &laserToggle},
        //{{CSMCCSVParser::FieldParserType::None,CSMCCSVParser::FieldProcessingStep::Nop}, nullptr},
        //{{CSMCCSVParser::FieldParserType::None,CSMCCSVParser::FieldProcessingStep::Nop}, nullptr},
        {{CSMCCSVParser::FieldParserType::Double,CSMCCSVParser::FieldProcessingStep::Nop}, &activeChannel0},
        {{CSMCCSVParser::FieldParserType::Double,CSMCCSVParser::FieldProcessingStep::Nop}, &activeChannel1},
        {{CSMCCSVParser::FieldParserType::Int,CSMCCSVParser::FieldProcessingStep::Nop}, &cmdCount},
        {{CSMCCSVParser::FieldParserType::None,CSMCCSVParser::FieldProcessingStep::Nop}, nullptr},
        {{CSMCCSVParser::FieldParserType::None,CSMCCSVParser::FieldProcessingStep::Nop}, nullptr},
        {{CSMCCSVParser::FieldParserType::Timestamp,CSMCCSVParser::FieldProcessingStep::Nop}, &timestampValues}
    };

    parser.Parse(bindings);

#if NOT_IMPLEMENTED
    const auto& headers = parser.getHeader();
    const auto& types = parser.getColumnTypes();
    const auto& rows = parser.getRows();
#endif

    pDataTable->AddColumn("timestamp", "Timestamp", LibMCEnv::eDataTableColumnType::DoubleColumn);
    pDataTable->AddColumn("x", "X", LibMCEnv::eDataTableColumnType::DoubleColumn);
    pDataTable->AddColumn("y", "Y", LibMCEnv::eDataTableColumnType::DoubleColumn);
    pDataTable->AddColumn("laseron", "LaserOn", LibMCEnv::eDataTableColumnType::Uint32Column);
    pDataTable->AddColumn("power", "Laser Power", LibMCEnv::eDataTableColumnType::DoubleColumn);
    pDataTable->AddColumn("active1", "Active Channel 1", LibMCEnv::eDataTableColumnType::DoubleColumn);
    pDataTable->AddColumn("cmdindex", "Command Index", LibMCEnv::eDataTableColumnType::Int32Column);

    m_dJobDuration = (double)timestampValues.size() / (double)SCANLABSMC_MICROSTEPSPERSECOND;
    m_bHasJobDuration = true;

    pDataTable->SetDoubleColumnValues("timestamp", timestampValues);
    timestampValues.resize(0);

    pDataTable->SetDoubleColumnValues("x", scanheadX);
    scanheadX.resize(0);

    pDataTable->SetDoubleColumnValues("y", scanheadY);
    scanheadY.resize(0);

    pDataTable->SetUint32ColumnValues("laseron", laserSignal);
    laserSignal.resize(0);

    pDataTable->SetDoubleColumnValues("power", activeChannel0);
    activeChannel0.resize(0);

    pDataTable->SetDoubleColumnValues("active1", activeChannel1);
    activeChannel1.resize(0);    
}

void CSMCJobInstance::ReadLogRecordFile(LibMCEnv::PDataTable pDataTable)
{
    if (pDataTable.get() == nullptr)
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPARAM);

    auto contextHandle = m_pContextHandle->getHandle();

    auto pLogRecordFile = m_pWorkingDirectory->AddManagedTempFile("csv");

    //if(!pLogRecordFile->FileExists())
    //    throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPARAM);

    auto sLogRecordAbsoluteFileName = pLogRecordFile->GetAbsoluteFileName();

    //auto eTransformationStep = slsc_TransformationStep::slsc_TransformationStep_Rtc; // needs scaling and rotation 
    auto eTransformationStep = slsc_TransformationStep::slsc_TransformationStep_Corrected;

    m_pSDK->slsc_ctrl_log_record(m_pContextHandle->getHandle(), sLogRecordAbsoluteFileName.c_str(), eTransformationStep);

    //-----------------------------------

    CSMCCSVParser parser(sLogRecordAbsoluteFileName, ';');

    std::vector<double> timestampValues;
    std::vector<double> scanheadX;
    std::vector<double> scanheadY;
    std::vector<uint32_t> laserSignal;

    std::vector<CSMCCSVParser::FieldBinding> bindings = {
        {{CSMCCSVParser::FieldParserType::Double, CSMCCSVParser::FieldProcessingStep::Extend | CSMCCSVParser::FieldProcessingStep::Interpolate }, &scanheadX},
        {{CSMCCSVParser::FieldParserType::Double, CSMCCSVParser::FieldProcessingStep::Extend | CSMCCSVParser::FieldProcessingStep::Interpolate }, &scanheadY},
        {{CSMCCSVParser::FieldParserType::LaserSignal,CSMCCSVParser::FieldProcessingStep::Nop}, &laserSignal},
        {{CSMCCSVParser::FieldParserType::Timestamp,CSMCCSVParser::FieldProcessingStep::Nop}, &timestampValues}
    };

    auto start = std::chrono::high_resolution_clock::now();

    parser.Parse(bindings);

#if NOT_IMPLEMENTED
    const auto& headers = parser.getHeader();
    const auto& types = parser.getColumnTypes();
    const auto& rows = parser.getRows();
#endif

    pDataTable->AddColumn("timestamp", "Timestamp", LibMCEnv::eDataTableColumnType::DoubleColumn);
    pDataTable->AddColumn("x", "X", LibMCEnv::eDataTableColumnType::DoubleColumn);
    pDataTable->AddColumn("y", "Y", LibMCEnv::eDataTableColumnType::DoubleColumn);
    pDataTable->AddColumn("laseron", "LaserOn", LibMCEnv::eDataTableColumnType::Uint32Column);
    pDataTable->AddColumn("active1", "Active Channel 1", LibMCEnv::eDataTableColumnType::DoubleColumn);
    pDataTable->AddColumn("active2", "Active Channel 2", LibMCEnv::eDataTableColumnType::DoubleColumn);
    pDataTable->AddColumn("cmdindex", "Command Index", LibMCEnv::eDataTableColumnType::Int32Column);

    m_dJobDuration = (double)timestampValues.size() / (double)SCANLABSMC_MICROSTEPSPERSECOND;
    m_bHasJobDuration = true;

    pDataTable->SetDoubleColumnValues("timestamp", timestampValues);
    timestampValues.resize(0);

    pDataTable->SetDoubleColumnValues("x", scanheadX);
    scanheadX.resize(0);

    pDataTable->SetDoubleColumnValues("y", scanheadY);
    scanheadY.resize(0);

    pDataTable->SetUint32ColumnValues("laseron", laserSignal);
    laserSignal.resize(0);
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
            //int64_t nLaserIndexToDraw = pLayer->GetSegmentProfileIntegerValueDef(nSegmentIndex, "", "laserindex", 0);

            double dMinimalMarkSpeed = pLayer->GetSegmentProfileDoubleValueDef(nSegmentIndex, "http://schemas.scanlab.com/smc/2024/10", "minimummarkspeed", dMarkSpeedInMMPerSecond);
            double dCornerTolerance = pLayer->GetSegmentProfileDoubleValueDef(nSegmentIndex, "http://schemas.scanlab.com/smc/2024/10", "cornertolerance", 0.0);


            switch (eSegmentType) {
                case LibMCEnv::eToolpathSegmentType::Polyline:
                {

                    std::vector<LibMCEnv::sPosition2D> PointData;
                    pLayer->GetSegmentPolylineData(nSegmentIndex, PointData);

                    std::vector<LibMCDriver_ScanLabSMC::sPoint2D> ContourPoints;
                    ContourPoints.resize(nPointCount);

                    for (uint32_t nPointIndex = 0; nPointIndex < nPointCount; nPointIndex++) {
                        auto pContourPoint = &ContourPoints.at(nPointIndex);
                        pContourPoint->m_X = (float)(((double)PointData.at(nPointIndex).m_Coordinates[0]) * dUnits);
                        pContourPoint->m_Y = (float)(((double)PointData.at(nPointIndex).m_Coordinates[1]) * dUnits);
                    } 

                    if (ContourPoints.size() > 0) {
                        this->DrawPolyline(ContourPoints.size(), ContourPoints.data(), dMarkSpeedInMMPerSecond, dMinimalMarkSpeed, dJumpSpeedInMMPerSecond, dPowerInWatts, dCornerTolerance, dZValue);
                    }

                    break;
                }

                case LibMCEnv::eToolpathSegmentType::Hatch:
                {
                    if (nPointCount % 2 == 1)
                        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPOINTCOUNT);

                    std::vector<LibMCEnv::sHatch2D> HatchData;
                    pLayer->GetSegmentHatchData(nSegmentIndex, HatchData);

                    size_t nHatchCount = HatchData.size();

                    std::vector<LibMCDriver_ScanLabSMC::sHatch2D> SMCHatches;
                    SMCHatches.resize(nHatchCount);

                    for (uint64_t nHatchIndex = 0; nHatchIndex < nHatchCount; nHatchIndex++) {
                        auto& srcHatch = HatchData.at(nHatchIndex);
                        auto& targetHatch = SMCHatches.at(nHatchIndex);
                        targetHatch.m_X1 = (float)((double)srcHatch.m_X1 * dUnits);
                        targetHatch.m_Y1 = (float)((double)srcHatch.m_Y1 * dUnits);
                        targetHatch.m_X2 = (float)((double)srcHatch.m_X2 * dUnits);
                        targetHatch.m_Y2 = (float)((double)srcHatch.m_Y2 * dUnits);
                    }

                    std::vector<double> DataBuffer1;
                    std::vector<double> DataBuffer2;

                    std::vector<uint32_t> SubinterpolationCounts;
                    std::vector<LibMCEnv::sHatch2DSubInterpolationData> SubinterpolationData;
                    LibMCEnv::eToolpathProfileModificationType modificationType = pLayer->GetSegmentProfileTypedModificationType(nSegmentIndex, LibMCEnv::eToolpathProfileValueType::LaserPower);

                    if ((modificationType == LibMCEnv::eToolpathProfileModificationType::LinearModification) 
                        || (modificationType == LibMCEnv::eToolpathProfileModificationType::LinearModification)
                        || (modificationType == LibMCEnv::eToolpathProfileModificationType::NonlinearModification)) {

                        pLayer->EvaluateTypedHatchProfileModifier(nSegmentIndex, LibMCEnv::eToolpathProfileValueType::LaserPower, DataBuffer1, DataBuffer2);

                    }

                    if (modificationType == LibMCEnv::eToolpathProfileModificationType::NonlinearModification) {
                        pLayer->EvaluateTypedHatchProfileInterpolation(nSegmentIndex, LibMCEnv::eToolpathProfileValueType::LaserPower, SubinterpolationCounts, SubinterpolationData);
                    }


                    if (SMCHatches.size() > 0) {

                        switch (modificationType) {
                            case LibMCEnv::eToolpathProfileModificationType::NoModification:
                            case LibMCEnv::eToolpathProfileModificationType::ConstantModification:
                                this->drawHatchesEx(SMCHatches.size(), SMCHatches.data(), dMarkSpeedInMMPerSecond, dJumpSpeedInMMPerSecond, dPowerInWatts, dZValue);
                                break;
                            case LibMCEnv::eToolpathProfileModificationType::LinearModification:
                                this->drawHatchesExLinearPower(SMCHatches.size(), SMCHatches.data(), dMarkSpeedInMMPerSecond, dJumpSpeedInMMPerSecond, dPowerInWatts, dZValue, DataBuffer1, DataBuffer2);
                                break;
                            case LibMCEnv::eToolpathProfileModificationType::NonlinearModification:
                                this->drawHatchesExNonLinearPower(SMCHatches.size(), SMCHatches.data(), dMarkSpeedInMMPerSecond, dJumpSpeedInMMPerSecond, dPowerInWatts, dZValue, DataBuffer1, DataBuffer2, SubinterpolationCounts, SubinterpolationData);
                                break;
                        }
                        
                    }

                    break;
                }

                
            }

        }

    }

}

void CSMCJobInstance::LoadRawSimulationData(LibMCDriver_ScanLabSMC_uint64 nDataBufferSize, LibMCDriver_ScanLabSMC_uint64* pDataNeededCount, LibMCDriver_ScanLabSMC_uint8* pDataBuffer)
{   
    if(pDataNeededCount == nullptr)
		throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPARAM);

    if (m_tmpSimulationFile.get() == nullptr)
		throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_SIMULATIONWORKINGFILEISNOTINITIALIZED);

    size_t nDataSize = m_tmpSimulationFile->GetSize();
    *pDataNeededCount = nDataSize;

    if (pDataBuffer)
    {		
        std::vector<LibMCEnv_uint8> tmpFileContentBuffer;
        m_tmpSimulationFile->ReadContent(tmpFileContentBuffer);

        if (nDataSize != tmpFileContentBuffer.size ())
            throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_SIMULATIONWORKINGFILESIZEMISMATCH);

        if (tmpFileContentBuffer.size () > 0)
            memcpy(pDataBuffer, tmpFileContentBuffer.data(), tmpFileContentBuffer.size());
    }
}