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


Abstract: This is the class declaration of CSMCJob

*/


#ifndef __LIBMCDRIVER_SCANLABSMC_SMCJOB
#define __LIBMCDRIVER_SCANLABSMC_SMCJOB

#include "libmcdriver_scanlabsmc_interfaces.hpp"

// Parent classes
#include "libmcdriver_scanlabsmc_base.hpp"
#include "libmcdriver_scanlabsmc_smcjobinstance.hpp"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.


namespace LibMCDriver_ScanLabSMC {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CSMCJob 
**************************************************************************************************************************/

class CSMCJob : public virtual ISMCJob, public virtual CBase {
private:
private:

	PSMCJobInstance m_pJobInstance;
	
public:

	CSMCJob(PSMCJobInstance pJobInstance);

	virtual ~CSMCJob();


	LibMCDriver_ScanLabSMC_uint64 GetJobID() override;

	void Finalize() override;

	bool IsFinalized() override;

	void DrawPolyline(const LibMCDriver_ScanLabSMC_uint64 nPointsBufferSize, const LibMCDriver_ScanLabSMC::sPoint2D * pPointsBuffer, const LibMCDriver_ScanLabSMC_double dMarkSpeed, const LibMCDriver_ScanLabSMC_double dMinimalMarkSpeed, const LibMCDriver_ScanLabSMC_double dJumpSpeed, const LibMCDriver_ScanLabSMC_double dPower, const LibMCDriver_ScanLabSMC_double dCornerTolerance, const LibMCDriver_ScanLabSMC_double dZValue) override;

	void DrawLoop(const LibMCDriver_ScanLabSMC_uint64 nPointsBufferSize, const LibMCDriver_ScanLabSMC::sPoint2D * pPointsBuffer, const LibMCDriver_ScanLabSMC_double dMarkSpeed, const LibMCDriver_ScanLabSMC_double dMinimalMarkSpeed, const LibMCDriver_ScanLabSMC_double dJumpSpeed, const LibMCDriver_ScanLabSMC_double dPower, const LibMCDriver_ScanLabSMC_double dCornerTolerance, const LibMCDriver_ScanLabSMC_double dZValue) override;

	void DrawHatches(const LibMCDriver_ScanLabSMC_uint64 nHatchesBufferSize, const LibMCDriver_ScanLabSMC::sHatch2D * pHatchesBuffer, const LibMCDriver_ScanLabSMC_double dMarkSpeed, const LibMCDriver_ScanLabSMC_double dJumpSpeed, const LibMCDriver_ScanLabSMC_double dPower, const LibMCDriver_ScanLabSMC_double dZValue) override;

	bool IsReady() override;

	void Execute(const bool bBlocking) override;

	bool IsExecuting() override;

	void WaitForExecution(const LibMCDriver_ScanLabSMC_uint32 nTimeOutInMilliseconds) override;

	void StopExecution() override;

};

} // namespace Impl
} // namespace LibMCDriver_ScanLabSMC

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_SCANLABSMC_SMCJOB
