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


Abstract: This is the class declaration of CDriver_Euler

*/


#ifndef __LIBMCDRIVER_EULER_DRIVER_EULER
#define __LIBMCDRIVER_EULER_DRIVER_EULER

#include "libmcdriver_euler_interfaces.hpp"
#include "libmcdriver_euler_connectioninstance.hpp"

// Parent classes
#include "libmcdriver_euler_driver.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include <map>

namespace LibMCDriver_Euler {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CDriver_Euler 
**************************************************************************************************************************/

	enum class eulerDriverType {
		libEulerUnknown = 0,
		libEuler_0_1 = 1,
	};

class CDriver_Euler : public virtual IDriver_Euler, public virtual CDriver {
private:

	std::string m_sName;

	LibMCEnv::PDriverEnvironment m_pDriverEnvironment;

	eulerDriverType m_DriverType;

	std::vector<uint8_t> m_LibEulerDLLData;

	LibMCEnv::PWorkingDirectory m_pWorkingDirectory;
	LibMCEnv::PWorkingFile m_pLibEulerDLLFile;

	PLibEulerSDK m_pLibEulerSDK;

	std::string m_sApplicationName;
	std::string m_sApplicationVersion;

	std::map<std::string, PEulerConnectionInstance> m_Connections;

	void LoadSDK();


public:

	CDriver_Euler(const std::string & sName, LibMCEnv::PDriverEnvironment pDriverEnvironment, eulerDriverType driverType);

	virtual ~CDriver_Euler();

	void Configure(const std::string& sConfigurationString) override;

	std::string GetName() override;

	std::string GetType() override;

	void GetVersion(LibMCDriver_Euler_uint32& nMajor, LibMCDriver_Euler_uint32& nMinor, LibMCDriver_Euler_uint32& nMicro, std::string& sBuild) override;

	void QueryParameters() override;

	void QueryParametersEx(LibMCEnv::PDriverStatusUpdateSession pDriverUpdateInstance) override;

	void SetCustomSDKResource(const std::string & sResourceName) override;

	void SetApplicationDetails(const std::string & sApplicationName, const std::string & sApplicationVersion) override;

	IEulerConnection * Connect(const std::string & sIdentifier, const std::string & sBaseURL, const std::string & sAPIKey, const std::string & sDeviceID) override;

	IEulerConnection* ConnectWithLicenseData(const std::string& sIdentifier, const LibMCDriver_Euler_uint64 nLicenseDataBufferSize, const LibMCDriver_Euler_uint8* pLicenseDataBuffer, const std::string& sDeviceID) override;

	IEulerConnection* ConnectWithLicenseResource(const std::string& sIdentifier, const std::string& sLicenseResourceName, const std::string& sDeviceID) override;

	IEulerConnection * FindConnection(const std::string & sIdentifier) override;

	bool ConnectionExists(const std::string & sIdentifier) override;

	void CloseAllConnections() override;

};

} // namespace Impl
} // namespace LibMCDriver_Euler

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_EULER_DRIVER_EULER
