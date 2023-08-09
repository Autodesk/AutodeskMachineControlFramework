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


Abstract: This is the class declaration of CSMCContext

*/


#ifndef __LIBMCDRIVER_SCANLABSMC_SMCCONTEXT
#define __LIBMCDRIVER_SCANLABSMC_SMCCONTEXT

#include "libmcdriver_scanlabsmc_interfaces.hpp"

// Parent classes
#include "libmcdriver_scanlabsmc_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "libmcdriver_scanlabsmc_smccontexthandle.hpp"
#include "libmcdriver_scanlabsmc_sdk.hpp"


namespace LibMCDriver_ScanLabSMC {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CSMCContext 
**************************************************************************************************************************/

class CSMCContext : public virtual ISMCContext, public virtual CBase {
private:

	PSMCContextHandle m_pContextHandle;
	PScanLabSMCSDK m_pSDK;

	LibMCEnv::PDriverEnvironment m_pDriverEnvironment;
	LibMCEnv::PWorkingDirectory m_pWorkingDirectory;

public:

	CSMCContext(const std::string& sConfigurationXML, PScanLabSMCSDK pSDK, LibMCEnv::PDriverEnvironment pDriverEnvironment);

	virtual ~CSMCContext();

	void SetToSimulationMode() override;

	bool IsSimulationMode() override;

	void SetFirmware(const LibMCDriver_ScanLabSMC_uint64 nFirmwareDataBufferSize, const LibMCDriver_ScanLabSMC_uint8 * pFirmwareDataBuffer, const LibMCDriver_ScanLabSMC_uint64 nFPGADataBufferSize, const LibMCDriver_ScanLabSMC_uint8 * pFPGADataBuffer, const LibMCDriver_ScanLabSMC_uint64 nAuxiliaryDataBufferSize, const LibMCDriver_ScanLabSMC_uint8 * pAuxiliaryDataBuffer) override;

	void SetCorrectionFile(const LibMCDriver_ScanLabSMC_uint64 nCorrectionFileDataBufferSize, const LibMCDriver_ScanLabSMC_uint8 * pCorrectionFileDataBuffer) override;

	void ReinitializeInstance() override;

	std::string GetIPAddress() override;

	std::string GetNetmask() override;

	LibMCDriver_ScanLabSMC_uint32 GetSerialNumber() override;

	LibMCDriver_ScanLabSMC_uint32 GetLaserIndex() override;

	void SetLaserOrigin(const LibMCDriver_ScanLabSMC_double dOriginX, const LibMCDriver_ScanLabSMC_double dOriginY) override;

	void GetLaserOrigin(LibMCDriver_ScanLabSMC_double & dOriginX, LibMCDriver_ScanLabSMC_double & dOriginY) override;

	void SetLaserField(const LibMCDriver_ScanLabSMC_double dMinX, const LibMCDriver_ScanLabSMC_double dMinY, const LibMCDriver_ScanLabSMC_double dMaxX, const LibMCDriver_ScanLabSMC_double dMaxY) override;

	void ResetLaserField() override;

	bool GetLaserField(LibMCDriver_ScanLabSMC_double & dMinX, LibMCDriver_ScanLabSMC_double & dMinY, LibMCDriver_ScanLabSMC_double & dMaxX, LibMCDriver_ScanLabSMC_double & dMaxY) override;

	ISMCJob * BeginJob(const LibMCDriver_ScanLabSMC_double dStartPositionX, const LibMCDriver_ScanLabSMC_double dStartPositionY, const LibMCDriver_ScanLabSMC::eBlendMode eBlendMode) override;

	ISMCJob * GetUnfinishedJob() override;

};

} // namespace Impl
} // namespace LibMCDriver_ScanLabSMC

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_SCANLABSMC_SMCCONTEXT
