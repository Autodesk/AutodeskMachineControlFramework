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

CSMCJob::CSMCJob(PSMCJobInstance pJobInstance)
    : m_pJobInstance (pJobInstance)
{
    if (pJobInstance.get() == nullptr)
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPARAM);
}

CSMCJob::~CSMCJob()
{

}

LibMCDriver_ScanLabSMC_uint64 CSMCJob::GetJobID()
{
    return m_pJobInstance->GetJobID();
}

void CSMCJob::Finalize()
{
    m_pJobInstance->Finalize();
}

bool CSMCJob::IsFinalized()
{
    return m_pJobInstance->IsFinalized ();
}



void CSMCJob::DrawPolyline(const LibMCDriver_ScanLabSMC_uint64 nPointsBufferSize, const LibMCDriver_ScanLabSMC::sPoint2D* pPointsBuffer, const LibMCDriver_ScanLabSMC_double dMarkSpeed, const LibMCDriver_ScanLabSMC_double dMinimalMarkSpeed, const LibMCDriver_ScanLabSMC_double dJumpSpeed, const LibMCDriver_ScanLabSMC_double dPower, const LibMCDriver_ScanLabSMC_double dCornerTolerance, const LibMCDriver_ScanLabSMC_double dZValue)
{
    m_pJobInstance->DrawPolyline(nPointsBufferSize, pPointsBuffer, dMarkSpeed, dMinimalMarkSpeed, dJumpSpeed, dPower, dCornerTolerance, dZValue);
}

void CSMCJob::DrawLoop(const LibMCDriver_ScanLabSMC_uint64 nPointsBufferSize, const LibMCDriver_ScanLabSMC::sPoint2D* pPointsBuffer, const LibMCDriver_ScanLabSMC_double dMarkSpeed, const LibMCDriver_ScanLabSMC_double dMinimalMarkSpeed, const LibMCDriver_ScanLabSMC_double dJumpSpeed, const LibMCDriver_ScanLabSMC_double dPower, const LibMCDriver_ScanLabSMC_double dCornerTolerance, const LibMCDriver_ScanLabSMC_double dZValue)
{
    m_pJobInstance->DrawLoop(nPointsBufferSize, pPointsBuffer, dMarkSpeed, dMinimalMarkSpeed, dJumpSpeed, dPower, dCornerTolerance, dZValue);
}


void CSMCJob::DrawHatches(const LibMCDriver_ScanLabSMC_uint64 nHatchesBufferSize, const LibMCDriver_ScanLabSMC::sHatch2D* pHatchesBuffer, const LibMCDriver_ScanLabSMC_double dMarkSpeed, const LibMCDriver_ScanLabSMC_double dJumpSpeed, const LibMCDriver_ScanLabSMC_double dPower, const LibMCDriver_ScanLabSMC_double dZValue)
{
    m_pJobInstance->DrawHatches(nHatchesBufferSize, pHatchesBuffer, dMarkSpeed, dJumpSpeed, dPower, dZValue);
}

bool CSMCJob::IsReady()
{
    return m_pJobInstance->IsReady();
}

void CSMCJob::Execute(const bool bBlocking)
{
    m_pJobInstance->Execute(bBlocking);
}

bool CSMCJob::IsExecuting()
{
    return m_pJobInstance->IsExecuting();
}

void CSMCJob::WaitForExecution(const LibMCDriver_ScanLabSMC_uint32 nTimeOutInMilliseconds)
{
    m_pJobInstance->WaitForExecution(nTimeOutInMilliseconds);
}

void CSMCJob::StopExecution()
{
    m_pJobInstance->StopExecution();
}

