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


Abstract: This is the class declaration of CDriver_Ximc

*/


#ifndef __LIBMCDRIVER_XIMC_DRIVER_XIMC
#define __LIBMCDRIVER_XIMC_DRIVER_XIMC

#include "libmcdriver_ximc_interfaces.hpp"

// Parent classes
#include "libmcdriver_ximc_driver.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "libmcdriver_ximc_sdk.hpp"

namespace LibMCDriver_Ximc {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CDriver_Ximc 
**************************************************************************************************************************/

class CDriver_Ximc : public virtual IDriver_Ximc, public virtual CDriver {
private:

protected:
    std::string m_sName;
    LibMCEnv::PDriverEnvironment m_pDriverEnvironment;

    LibMCEnv::PWorkingDirectory m_pWorkingDirectory;
    LibMCEnv::PWorkingFile m_pLibXimcDLL;
    LibMCEnv::PWorkingFile m_pBindyDLL;
    LibMCEnv::PWorkingFile m_pXiWrapperDLL;

    PXimcSDK m_pXimcSDK;

    std::vector<std::string> m_FoundDevices;

    void findDevices();

public:
    CDriver_Ximc (const std::string & sName, LibMCEnv::PDriverEnvironment pDriverEnvironment);

	void Configure(const std::string& sConfigurationString) override;

	std::string GetName() override;

	std::string GetType() override;

	void GetVersion(LibMCDriver_Ximc_uint32& nMajor, LibMCDriver_Ximc_uint32& nMinor, LibMCDriver_Ximc_uint32& nMicro, std::string& sBuild) override;

	void QueryParameters() override;

    void QueryParametersEx(LibMCEnv::PDriverStatusUpdateSession pDriverUpdateInstance) override;

	void Initialize(const std::string& sDeviceName) override;

    LibMCDriver_Ximc_double GetCurrentPosition() override;

    LibMCDriver_Ximc_uint32 GetDetectedDeviceCount() override;

    std::string GetDetectedDeviceName(const LibMCDriver_Ximc_uint32 nDeviceIndex) override;
};

} // namespace Impl
} // namespace LibMCDriver_Ximc

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_XIMC_DRIVER_XIMC
