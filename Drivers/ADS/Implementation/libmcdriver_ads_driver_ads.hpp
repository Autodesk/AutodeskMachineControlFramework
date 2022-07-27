/*++

Copyright (C) 2020 Autodesk Inc.

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


Abstract: This is the class declaration of CDriver_ADS

*/


#ifndef __LIBMCDRIVER_ADS_DRIVER_ADS
#define __LIBMCDRIVER_ADS_DRIVER_ADS

#include "libmcdriver_ads_interfaces.hpp"
#include "libmcdriver_ads_client.hpp"
#include "libmcdriver_ads_sdk.hpp"

// Parent classes
#include "libmcdriver_ads_driver.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include <mutex>

namespace LibMCDriver_ADS {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CDriver_ADS 
**************************************************************************************************************************/

class CDriver_ADS : public virtual IDriver_ADS, public virtual CDriver {
private:

	bool m_bSimulationMode;

	std::string m_sName;
	std::mutex m_driverEnvironmentMutex;
	LibMCEnv::PDriverEnvironment m_pDriverEnvironment;

	PADSSDK m_pADSSDK;
	PADSClient m_pADSClient;

	uint32_t m_nMajorVersion;
	uint32_t m_nMinorVersion;
	uint32_t m_nPatchVersion;

	LibMCEnv::PWorkingDirectory m_pWorkingDirectory;
	LibMCEnv::PWorkingFile m_pADSDLLFile;

protected:


public:
	CDriver_ADS(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment);

	virtual ~CDriver_ADS();

	void Configure(const std::string& sConfigurationString) override;

	void SetToSimulationMode() override;

	bool IsSimulationMode() override;

	void Connect(const LibMCDriver_ADS_uint32 nPort, const LibMCDriver_ADS_uint32 nTimeout) override;

	void Disconnect() override;
	
	bool VariableExists(const std::string & sVariableName) override;

	LibMCDriver_ADS_int64 ReadIntegerValue(const std::string& sVariableName) override;

	void WriteIntegerValue(const std::string& sVariableName, const LibMCDriver_ADS_int64 nValue) override;

	void GetVariableBounds(const std::string& sVariableName, LibMCDriver_ADS_int64& nMinValue, LibMCDriver_ADS_int64& nMaxValue) override;

};

} // namespace Impl
} // namespace LibMCDriver_ADS

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_ADS_DRIVER_ADS
