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


Abstract: This is the class declaration of CDriver_A3200

*/


#ifndef __LIBMCDRIVER_A3200_DRIVER_A3200
#define __LIBMCDRIVER_A3200_DRIVER_A3200

#include "libmcdriver_a3200_interfaces.hpp"

// Parent classes
#include "libmcdriver_a3200_driver.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "libmcdriver_a3200_sdk.hpp"

#include <vector>

#define A3200_MINTASKID 1
#define A3200_MAXTASKID 31

namespace LibMCDriver_A3200 {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CDriver_A3200 
**************************************************************************************************************************/

class CDriver_A3200 : public virtual IDriver_A3200, public virtual CDriver {
private:

	bool m_bSimulationMode;
	std::string m_sName;
	LibMCEnv::PDriverEnvironment m_pDriverEnvironment;

	std::vector <uint8_t> m_CoreSystemBuffer;
	std::vector <uint8_t> m_SystemSDKBuffer;
	std::vector <uint8_t> m_CompilerSDKBuffer;
	std::vector <uint8_t> m_UtilitiesSDKBuffer;
	std::vector <uint8_t> m_LicenseDecoderSDKBuffer;

	std::string m_sCoreSystemResourceName;
	std::string m_sSystemSDKResourceName;
	std::string m_sCompilerSDKResourceName;
	std::string m_sUtilitiesSDKResourceName;
	std::string m_sLicenseDecoderSDKResourceName;

	LibMCEnv::PWorkingDirectory m_pWorkingDirectory;
	LibMCEnv::PWorkingFile m_pCoreSystemDLL;
	LibMCEnv::PWorkingFile m_pSystemSDKDLL;
	LibMCEnv::PWorkingFile m_pCompilerSDKDLL;
	LibMCEnv::PWorkingFile m_pUtilitiesSDKDLL;
	LibMCEnv::PWorkingFile m_pLicenseDecoderSDKDLL;

	PA3200SDK m_pSDK;
	A3200Handle m_pHandle;

	void loadSDK ();
	void unloadSDK();

	void storeCustomSDK(std::vector<uint8_t> & buffer, uint64_t nArraySize, const uint8_t * pData);
	LibMCEnv::PWorkingFile createCustomDLL (const std::string & sFileNameOnDisk, std::vector<uint8_t>& buffer, const std::string & sResourceName, const std::string & sDefaultResourceName);

public:

	CDriver_A3200 (const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment);

	virtual ~CDriver_A3200();

	void Configure(const std::string& sConfigurationString) override;

	std::string GetName() override;

	std::string GetType() override;

	void GetVersion(LibMCDriver_A3200_uint32& nMajor, LibMCDriver_A3200_uint32& nMinor, LibMCDriver_A3200_uint32& nMicro, std::string& sBuild) override;

	void QueryParameters() override;

	void QueryParametersEx(LibMCEnv::PDriverStatusUpdateSession pDriverUpdateInstance) override;


	void SetToSimulationMode() override;

	bool IsSimulationMode() override;

	void SetCustomSDKResource(const std::string & sCoreResourceName, const std::string & sSystemResourceName, const std::string & sCompilerResourceName, const std::string & sUtilitiesResourceName, const std::string & sLicenseDecoderResourceName) override;

	void SetCustomSDK(const LibMCDriver_A3200_uint64 nCoreSDKBufferBufferSize, const LibMCDriver_A3200_uint8 * pCoreSDKBufferBuffer, const LibMCDriver_A3200_uint64 nSystemSDKBufferBufferSize, const LibMCDriver_A3200_uint8 * pSystemSDKBufferBuffer, const LibMCDriver_A3200_uint64 nCompilerSDKBufferBufferSize, const LibMCDriver_A3200_uint8 * pCompilerSDKBufferBuffer, const LibMCDriver_A3200_uint64 nUtilitiesSDKBufferBufferSize, const LibMCDriver_A3200_uint8 * pUtilitiesSDKBufferBuffer, const LibMCDriver_A3200_uint64 nLicenseDecoderSDKBufferBufferSize, const LibMCDriver_A3200_uint8 * pLicenseDecoderSDKBufferBuffer) override;

	void Connect() override;

	void Disconnect() override;

	void RunAeroBasicScript(const LibMCDriver_A3200_uint32 nTaskID, const std::string& sScript) override;

	void StopProgram(const LibMCDriver_A3200_uint32 nTaskID, const LibMCDriver_A3200_uint32 nTimeout) override;


};


} // namespace Impl
} // namespace LibMCDriver_A3200

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_A3200_DRIVER_A3200
