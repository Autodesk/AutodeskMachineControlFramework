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


Abstract: This is a stub class definition of CDriver_Camera

*/

#include "libmcdriver_camera_driver_camera.hpp"
#include "libmcdriver_camera_interfaceexception.hpp"
#include "libmcdriver_camera_devicelist.hpp"

// Include custom headers here.
#define __STRINGIZE(x) #x
#define __STRINGIZE_VALUE_OF(x) __STRINGIZE(x)


using namespace LibMCDriver_Camera::Impl;

/*************************************************************************************************************************
 Class definition of CDriver_Camera 
**************************************************************************************************************************/

CDriver_Camera::CDriver_Camera(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment)
	: m_sName (sName)
{

}

CDriver_Camera::~CDriver_Camera()
{

}

void CDriver_Camera::Configure(const std::string& sConfigurationString)
{
}

std::string CDriver_Camera::GetName()
{
	return m_sName;
}

std::string CDriver_Camera::getDummyDriverType()
{
	return "dummycamera";
}


std::string CDriver_Camera::GetType()
{
	return getDummyDriverType();
}

void CDriver_Camera::GetVersion(LibMCDriver_Camera_uint32& nMajor, LibMCDriver_Camera_uint32& nMinor, LibMCDriver_Camera_uint32& nMicro, std::string& sBuild)
{
	nMajor = LIBMCDRIVER_CAMERA_VERSION_MAJOR;
	nMinor = LIBMCDRIVER_CAMERA_VERSION_MINOR;
	nMicro = LIBMCDRIVER_CAMERA_VERSION_MICRO;
	sBuild = __STRINGIZE_VALUE_OF(__GITHASH);
}


void CDriver_Camera::QueryParameters()
{
	
}

void CDriver_Camera::QueryParametersEx(LibMCEnv::PDriverStatusUpdateSession pDriverUpdateInstance)
{
	
}



IDeviceList * CDriver_Camera::EnumerateDevices()
{
	return new CDeviceList();
}

IVideoDevice * CDriver_Camera::FindDeviceByIdentifier(const std::string & sIdentifier, const bool bMustExist)
{
	if (bMustExist)
		throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_CAMERADEVICENOTFOUND, "camera device not found: " + sIdentifier);

	return nullptr;
}

IVideoDevice * CDriver_Camera::FindDeviceByOperatingSystemName(const std::string & sOperatingSystemName, const bool bMustExist)
{
	if (bMustExist)
		throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_CAMERADEVICENOTFOUND, "camera device not found: " + sOperatingSystemName);

	return nullptr;
}

