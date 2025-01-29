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


Abstract: This is the class declaration of CVideoDevice

*/


#ifndef __LIBMCDRIVER_CAMERA_VIDEODEVICE
#define __LIBMCDRIVER_CAMERA_VIDEODEVICE

#include "libmcdriver_camera_interfaces.hpp"

// Parent classes
#include "libmcdriver_camera_devicebase.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "libmcdriver_camera_videodeviceinstance_win32.hpp"

namespace LibMCDriver_Camera {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CVideoDevice 
**************************************************************************************************************************/

class CVideoDevice_Win32 : public virtual IVideoDevice, public virtual CDeviceBase {
private:

	PVideoDeviceInstance_Win32 m_pDeviceInstance;
	
public:

	CVideoDevice_Win32(PVideoDeviceInstance_Win32 pDeviceInstance);

	virtual ~CVideoDevice_Win32();

	std::string GetFriendlyName() override;

	std::string GetOperatingSystemName() override;

	bool IsVideoDevice() override;

	std::string GetIdentifier() override;

	LibMCDriver_Camera_uint32 GetSupportedResolutionCount() override;

	void GetSupportedResolution(const LibMCDriver_Camera_uint32 nIndex, LibMCDriver_Camera_uint32& nWidth, LibMCDriver_Camera_uint32& nHeight, LibMCDriver_Camera_uint32& nFramerate) override;

	void GetCurrentResolution(LibMCDriver_Camera_uint32& nWidth, LibMCDriver_Camera_uint32& nHeight, LibMCDriver_Camera_uint32& nFramerate) override;

	void SetResolution(const LibMCDriver_Camera_uint32 nWidth, const LibMCDriver_Camera_uint32 nHeight, const LibMCDriver_Camera_uint32 nFramerate) override;

	void CaptureRawImage(LibMCEnv::PImageData pImageData) override;

	void StartStreamCapture(LibMCEnv::PVideoStream pStreamInstance) override;

	void StopStreamCapture() override;

	bool StreamCaptureIsActive() override;

	void GetStreamCaptureStatistics(LibMCDriver_Camera_double & dDesiredFramerate, LibMCDriver_Camera_double & dMinFramerate, LibMCDriver_Camera_double & dMaxFramerate, LibMCDriver_Camera_double & dMeanFramerate, LibMCDriver_Camera_double & dStdDevFramerate) override;

};

} // namespace Impl
} // namespace LibMCDriver_Camera

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_CAMERA_VIDEODEVICE
