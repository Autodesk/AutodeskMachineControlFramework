/*++

Copyright (C) 2024 Autodesk Inc.

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


Abstract: This is the class declaration of CDriver_Camera

*/


#ifndef __LIBMCDRIVER_CAMERA_DRIVER_CAMERA
#define __LIBMCDRIVER_CAMERA_DRIVER_CAMERA

#include "libmcdriver_camera_interfaces.hpp"

// Parent classes
#include "libmcdriver_camera_driver.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#define CAMERA_MINIDENTIFIERLENGTH 1
#define CAMERA_MAXIDENTIFIERLENGTH 256

namespace LibMCDriver_Camera {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CDriver_Camera 
**************************************************************************************************************************/

class CDriver_Camera : public virtual IDriver_Camera, public virtual CDriver {
private:

	std::string m_sName;

public:

	CDriver_Camera(const std::string & sName, LibMCEnv::PDriverEnvironment m_pDriverEnvironment);

	virtual ~CDriver_Camera();

	void Configure(const std::string& sConfigurationString) override;

	static std::string getDummyDriverType();

	std::string GetName() override;

	std::string GetType() override;

	void GetVersion(LibMCDriver_Camera_uint32& nMajor, LibMCDriver_Camera_uint32& nMinor, LibMCDriver_Camera_uint32& nMicro, std::string& sBuild) override;

	void QueryParameters() override;

	void QueryParametersEx(LibMCEnv::PDriverStatusUpdateSession pDriverUpdateInstance) override;

	IDeviceList * EnumerateDevices() override;

	IVideoDevice* OpenVideoDevice(const std::string& sIdentifier, IDeviceInfo* pVideoDeviceInfo) override;

	IVideoDevice * FindDeviceByIdentifier(const std::string & sIdentifier, const bool bMustExist) override;

	IVideoDevice * FindDeviceByOperatingSystemName(const std::string & sOperatingSystemName, const bool bMustExist) override;

};

} // namespace Impl
} // namespace LibMCDriver_Camera

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_CAMERA_DRIVER_CAMERA
