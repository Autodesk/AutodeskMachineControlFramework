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


Abstract: This is a stub class definition of CVideoDevice

*/

#include "libmcdriver_camera_videodevice.hpp"
#include "libmcdriver_camera_interfaceexception.hpp"

// Include custom headers here.


using namespace LibMCDriver_Camera::Impl;

/*************************************************************************************************************************
 Class definition of CVideoDevice 
**************************************************************************************************************************/

CVideoDevice_Win32::CVideoDevice_Win32(PVideoDeviceInstance_Win32 pDeviceInstance)
    : m_pDeviceInstance (pDeviceInstance)
{
    if (pDeviceInstance.get() == nullptr)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_INVALIDPARAM);

}

CVideoDevice_Win32::~CVideoDevice_Win32()
{

}

std::string CVideoDevice_Win32::GetFriendlyName()
{
    return m_pDeviceInstance->getFriendlyName();
}

std::string CVideoDevice_Win32::GetOperatingSystemName()
{
    return m_pDeviceInstance->getOperatingSystemName();
}

bool CVideoDevice_Win32::IsVideoDevice()
{
    return m_pDeviceInstance->isVideoDevice();
}


std::string CVideoDevice_Win32::GetIdentifier()
{
    return m_pDeviceInstance->getIdentifier();
}

LibMCDriver_Camera_uint32 CVideoDevice_Win32::GetSupportedResolutionCount()
{
    return m_pDeviceInstance->getSupportedResolutionCount();
}

void CVideoDevice_Win32::GetSupportedResolution(const LibMCDriver_Camera_uint32 nIndex, LibMCDriver_Camera_uint32& nWidth, LibMCDriver_Camera_uint32& nHeight, LibMCDriver_Camera_uint32& nFramerate)
{
    m_pDeviceInstance->getSupportedResolution(nIndex, nWidth, nHeight, nFramerate);

}

void CVideoDevice_Win32::GetCurrentResolution(LibMCDriver_Camera_uint32& nWidth, LibMCDriver_Camera_uint32& nHeight, LibMCDriver_Camera_uint32& nFramerate)
{
    m_pDeviceInstance->getCurrentResolution(nWidth, nHeight, nFramerate);
}

void CVideoDevice_Win32::SetResolution(const LibMCDriver_Camera_uint32 nWidth, const LibMCDriver_Camera_uint32 nHeight, const LibMCDriver_Camera_uint32 nFramerate)
{
    m_pDeviceInstance->setResolution(nWidth, nHeight, nFramerate);
}


void CVideoDevice_Win32::CaptureRawImage(LibMCEnv::PImageData pImageData)
{
    throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_NOTIMPLEMENTED);
}


void CVideoDevice_Win32::StartStreamCapture(LibMCEnv::PVideoStream pStreamInstance)
{
	throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_NOTIMPLEMENTED);
}

void CVideoDevice_Win32::StopStreamCapture()
{
	throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_NOTIMPLEMENTED);
}

bool CVideoDevice_Win32::StreamCaptureIsActive()
{
	throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_NOTIMPLEMENTED);
}

void CVideoDevice_Win32::GetStreamCaptureStatistics(LibMCDriver_Camera_double & dDesiredFramerate, LibMCDriver_Camera_double & dMinFramerate, LibMCDriver_Camera_double & dMaxFramerate, LibMCDriver_Camera_double & dMeanFramerate, LibMCDriver_Camera_double & dStdDevFramerate)
{
	throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_NOTIMPLEMENTED);
}

