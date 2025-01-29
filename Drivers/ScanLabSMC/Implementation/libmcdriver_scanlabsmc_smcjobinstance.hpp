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


#ifndef __LIBMCDRIVER_SCANLABSMC_SMCJOBINSTANCE
#define __LIBMCDRIVER_SCANLABSMC_SMCJOBINSTANCE

#include "libmcdriver_scanlabsmc_interfaces.hpp"

#include "libmcdriver_scanlabsmc_smccontexthandle.hpp"
#include "libmcdriver_scanlabsmc_sdk.hpp"


namespace LibMCDriver_ScanLabSMC {
namespace Impl {



class CSMCJobInstance {
private:
private:

	PSMCContextHandle m_pContextHandle;
	PScanLabSMCSDK m_pSDK;

	LibMCEnv::PWorkingDirectory m_pWorkingDirectory;
	std::string m_sSimulationSubDirectory;

	slscJobID m_JobID;
	bool m_bIsFinalized;

	bool m_bHasJobDuration;
	double m_dJobDuration;

	void drawPolylineEx(slscHandle contextHandle, const uint64_t nPointsBufferSize, const LibMCDriver_ScanLabSMC::sPoint2D* pPointsBuffer, bool bIsClosed);

public:

	CSMCJobInstance(PSMCContextHandle pContextHandle, double dStartPositionX, double dStartPositionY, LibMCDriver_ScanLabSMC::eBlendMode eBlendMode, LibMCEnv::PWorkingDirectory pWorkingDirectory, std::string sSimulationSubDirectory);

	virtual ~CSMCJobInstance();


	LibMCDriver_ScanLabSMC_uint64 GetJobID();

	void Finalize();

	bool IsFinalized();

	void DrawPolyline(const LibMCDriver_ScanLabSMC_uint64 nPointsBufferSize, const LibMCDriver_ScanLabSMC::sPoint2D * pPointsBuffer, const LibMCDriver_ScanLabSMC_double dMarkSpeed, const LibMCDriver_ScanLabSMC_double dMinimalMarkSpeed, const LibMCDriver_ScanLabSMC_double dJumpSpeed, const LibMCDriver_ScanLabSMC_double dPower, const LibMCDriver_ScanLabSMC_double dCornerTolerance, const LibMCDriver_ScanLabSMC_double dZValue);

	void DrawLoop(const LibMCDriver_ScanLabSMC_uint64 nPointsBufferSize, const LibMCDriver_ScanLabSMC::sPoint2D * pPointsBuffer, const LibMCDriver_ScanLabSMC_double dMarkSpeed, const LibMCDriver_ScanLabSMC_double dMinimalMarkSpeed, const LibMCDriver_ScanLabSMC_double dJumpSpeed, const LibMCDriver_ScanLabSMC_double dPower, const LibMCDriver_ScanLabSMC_double dCornerTolerance, const LibMCDriver_ScanLabSMC_double dZValue);

	void DrawHatches(const LibMCDriver_ScanLabSMC_uint64 nHatchesBufferSize, const LibMCDriver_ScanLabSMC::sHatch2D * pHatchesBuffer, const LibMCDriver_ScanLabSMC_double dMarkSpeed, const LibMCDriver_ScanLabSMC_double dJumpSpeed, const LibMCDriver_ScanLabSMC_double dPower, const LibMCDriver_ScanLabSMC_double dZValue);

	bool IsReady();

	void Execute(const bool bBlocking);

	bool IsExecuting();

	void WaitForExecution(const LibMCDriver_ScanLabSMC_uint32 nTimeOutInMilliseconds);

	void StopExecution();

	void AddLayerToList(LibMCEnv::PToolpathLayer pLayer);

	void ReadSimulationFile (LibMCEnv::PDataTable pDataTable);

	double GetJobCharacteristic(const LibMCDriver_ScanLabSMC::eJobCharacteristic eValueType);

	double GetJobDuration();


};

typedef std::shared_ptr<CSMCJobInstance> PSMCJobInstance;

} // namespace Impl
} // namespace LibMCDriver_ScanLabSMC

#endif // __LIBMCDRIVER_SCANLABSMC_SMCJOBINSTANCE
