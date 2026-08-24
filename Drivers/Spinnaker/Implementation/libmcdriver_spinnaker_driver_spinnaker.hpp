/*++

Copyright (C) 2025 Autodesk Inc.

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


Abstract: This is the class declaration of CDriver_Spinnaker

*/


#ifndef __LIBMCDRIVER_SPINNAKER_DRIVER_SPINNAKER
#define __LIBMCDRIVER_SPINNAKER_DRIVER_SPINNAKER

#include "libmcdriver_spinnaker_interfaces.hpp"
#include "libmcdriver_spinnaker_sdk.hpp"
#include "libmcdriver_spinnaker_spinnakerdeviceinstance.hpp"

// Parent classes
#include "libmcdriver_spinnaker_driver.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include <vector>
#include <map>


namespace LibMCDriver_Spinnaker {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CDriver_Spinnaker 
**************************************************************************************************************************/

class CDriver_Spinnaker : public virtual IDriver_Spinnaker, public virtual CDriver {
private:
	std::string m_sName;
	LibMCEnv::PDriverEnvironment m_pDriverEnvironment;

	std::vector<uint8_t> m_SpinnakerCDLLData;
	LibMCEnv::PWorkingDirectory m_pWorkingDirectory;
	LibMCEnv::PWorkingFile m_pSpinnakerCDLLFile;
	PLibSpinnakerSDK m_pSpinnakerSDK;
	libSpinnakerSystemHandle m_pSystemHandle;

	std::map<std::string, PSpinnakerDeviceInstance> m_ConnectedDevices;

	void loadSpinnakerSDK();
	void unloadSpinnakerSDK();
	ISpinnakerDevice* connectToUniqueDevice(const std::string& sIdentifier);

	void checkIdentifier(const std::string& sIdentifier);

public:

	CDriver_Spinnaker(const std::string & sName, LibMCEnv::PDriverEnvironment pDriverEnvironment);

	virtual ~CDriver_Spinnaker();

	void Configure(const std::string& sConfigurationString) override;

	std::string GetName() override;

	std::string GetType() override;

	void GetVersion(LibMCDriver_Spinnaker_uint32& nMajor, LibMCDriver_Spinnaker_uint32& nMinor, LibMCDriver_Spinnaker_uint32& nMicro, std::string& sBuild) override;

	void QueryParameters() override;

	void QueryParametersEx(LibMCEnv::PDriverStatusUpdateSession pDriverUpdateInstance) override;

	void SetCustomSDKResource(const std::string & sResourceName) override;

	ISpinnakerDevice * ConnectToUniqueGenericDevice(const std::string & sIdentifier) override;

	ISpinnakerDevice * ConnectToUniqueGigEDevice(const std::string & sIdentifier) override;

	bool ConnectionExists(const std::string & sIdentifier) override;

	ISpinnakerDevice* FindDeviceConnection(const std::string& sIdentifier) override;

	void CloseAllConnections() override;

};

} // namespace Impl
} // namespace LibMCDriver_Spinnaker

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_SPINNAKER_DRIVER_SPINNAKER
