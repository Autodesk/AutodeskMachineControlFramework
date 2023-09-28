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

*/


#ifndef __LIBMCDRIVER_SCANLABSMC_SMCCONTEXTINSTANCE
#define __LIBMCDRIVER_SCANLABSMC_SMCCONTEXTINSTANCE

#include "libmcdriver_scanlabsmc_interfaces.hpp"

// Include custom headers here.
#include "libmcdriver_scanlabsmc_smccontexthandle.hpp"
#include "libmcdriver_scanlabsmc_sdk.hpp"
#include "libmcdriver_scanlabsmc_smcjobinstance.hpp"


namespace LibMCDriver_ScanLabSMC {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CSMCContext 
**************************************************************************************************************************/

class CSMCContextInstance {
private:

	PSMCContextHandle m_pContextHandle;
	PScanLabSMCSDK m_pSDK;
	std::string m_sContextName;

	LibMCEnv::PDriverEnvironment m_pDriverEnvironment;
	LibMCEnv::PWorkingDirectory m_pWorkingDirectory;

public:

	CSMCContextInstance(const std::string & sContextName, ISMCConfiguration* pSMCConfiguration, PScanLabSMCSDK pSDK, LibMCEnv::PDriverEnvironment pDriverEnvironment);

	virtual ~CSMCContextInstance();

	void SetToSimulationMode();

	bool IsSimulationMode();

	void SetFirmware(const LibMCDriver_ScanLabSMC_uint64 nFirmwareDataBufferSize, const LibMCDriver_ScanLabSMC_uint8 * pFirmwareDataBuffer, const LibMCDriver_ScanLabSMC_uint64 nFPGADataBufferSize, const LibMCDriver_ScanLabSMC_uint8 * pFPGADataBuffer, const LibMCDriver_ScanLabSMC_uint64 nAuxiliaryDataBufferSize, const LibMCDriver_ScanLabSMC_uint8 * pAuxiliaryDataBuffer);

	void SetCorrectionFile(const LibMCDriver_ScanLabSMC_uint64 nCorrectionFileDataBufferSize, const LibMCDriver_ScanLabSMC_uint8 * pCorrectionFileDataBuffer);

	void ReinitializeInstance();

	std::string GetIPAddress();

	std::string GetNetmask();

	LibMCDriver_ScanLabSMC_uint32 GetSerialNumber();

	LibMCDriver_ScanLabSMC_uint32 GetLaserIndex();

	PSMCJobInstance BeginJob(const double dStartPositionX, const double dStartPositionY, const LibMCDriver_ScanLabSMC::eBlendMode eBlendMode);

	PSMCJobInstance GetUnfinishedJob();

};

typedef std::shared_ptr<CSMCContextInstance> PSMCContextInstance;

} // namespace Impl
} // namespace LibMCDriver_ScanLabSMC

#endif // __LIBMCDRIVER_SCANLABSMC_SMCCONTEXTINSTANCE
