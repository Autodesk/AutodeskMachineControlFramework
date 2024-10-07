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

#include "libmcdriver_camera_videodeviceinstance_win32.hpp"
#include "libmcdriver_camera_interfaceexception.hpp"

// Include custom headers here.


using namespace LibMCDriver_Camera::Impl;

/*************************************************************************************************************************
 Class definition of CVideoDevice 
**************************************************************************************************************************/

CVideoDeviceInstance_Win32::CVideoDeviceInstance_Win32(const std::string& sIdentifier, const std::string& sOSName, const std::string& sFriendlyName)
    : m_sIdentifier (sIdentifier), m_sOSName (sOSName), m_sFriendlyName (sFriendlyName)
{

}

CVideoDeviceInstance_Win32::~CVideoDeviceInstance_Win32()
{

}

std::string CVideoDeviceInstance_Win32::getFriendlyName()
{
    return m_sFriendlyName;
}

std::string CVideoDeviceInstance_Win32::getOperatingSystemName()
{
    return m_sOSName;
}

bool CVideoDeviceInstance_Win32::isVideoDevice()
{
    return true;
}


std::string CVideoDeviceInstance_Win32::getIdentifier()
{
    return m_sIdentifier;
}

void CVideoDeviceInstance_Win32::getCurrentResolution(LibMCDriver_Camera_uint32 & nWidth, LibMCDriver_Camera_uint32 & nHeight)
{
    nWidth = 0;
    nHeight = 0;
}

void CVideoDeviceInstance_Win32::setResolution(LibMCDriver_Camera_uint32 nWidth, LibMCDriver_Camera_uint32 nHeight)
{
}

void CVideoDeviceInstance_Win32::startStreamCapture(const LibMCDriver_Camera_double dDesiredFramerate, LibMCEnv::PVideoStream pStreamInstance)
{
}

void CVideoDeviceInstance_Win32::stopStreamCapture()
{
}

bool CVideoDeviceInstance_Win32::streamCaptureIsActive()
{
    return false;
}

void CVideoDeviceInstance_Win32::getStreamCaptureStatistics(LibMCDriver_Camera_double & dDesiredFramerate, LibMCDriver_Camera_double & dMinFramerate, LibMCDriver_Camera_double & dMaxFramerate, LibMCDriver_Camera_double & dMeanFramerate, LibMCDriver_Camera_double & dStdDevFramerate)
{
}

