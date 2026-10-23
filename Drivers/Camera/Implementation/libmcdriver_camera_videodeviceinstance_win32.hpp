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


#ifndef __LIBMCDRIVER_CAMERA_VIDEODEVICEINSTANCE_WIN32
#define __LIBMCDRIVER_CAMERA_VIDEODEVICEINSTANCE_WIN32

#include "libmcdriver_camera_interfaces.hpp"
#include "libmcdriver_camera_videoresolution.hpp"

#include <thread>
#include <atomic>
#include <mutex>
#include <vector>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include <mfapi.h>
#include <mfplay.h>
#include <mfreadwrite.h>
#include <mfobjects.h>
#include <mfidl.h>
#include <wrl/client.h>
#endif

#define STREAMCAPTURE_MICROSECONDS_PER_SECOND 1000000.0
#define STREAMCAPTURE_STATISTICS_MIN_FRAME_COUNT 2
#define STREAMCAPTURE_THREAD_SHUTDOWN_TIMEOUT_MS 5000

namespace LibMCDriver_Camera {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CVideoDevice 
**************************************************************************************************************************/

class CVideoDeviceInstance_Win32 {
private:
	std::string m_sIdentifier;
	std::string m_sOSName;
	std::string m_sFriendlyName;

	uint32_t m_nCurrentResolutionX;
	uint32_t m_nCurrentResolutionY;
	uint32_t m_nCurrentFrameRate;

	eVideoSourceFormat m_SourceFormat;

	// Driver environment for creating images during stream capture
	LibMCEnv::PDriverEnvironment m_pDriverEnvironment;

	// Stream capture state
	LibMCEnv::PVideoStream m_pActiveStream;
	std::unique_ptr<std::thread> m_pCaptureThread;
	std::atomic<bool> m_bCapturing;

	// Stream capture statistics (protected by m_StatisticsMutex)
	mutable std::mutex m_StatisticsMutex;
	std::vector<double> m_FrameDurationsInSeconds;
	double m_dDesiredFramerateInFPS;

	void captureThreadFunction();

#ifdef _WIN32
	Microsoft::WRL::ComPtr<IMFSourceReader> m_pSourceReader;
	Microsoft::WRL::ComPtr<IMFMediaType> m_pMediaType;

	//Microsoft::WRL::ComPtr<IMFTransform> createMJPEGEncoder (IMFMediaType * pInputMediaType);

#endif

	std::vector<PVideoResolution> m_SupportedResolutions;

	void refreshSupportedResolutions();

	LibMCDriver_Camera::eVideoSourceFormat convertUUIDToAnyVideoSourceFormat(const std::string& sUUID);

	LibMCDriver_Camera::eVideoSourceFormat convertUUIDToValidVideoSourceFormat(const std::string& sUUID);
	std::string convertVideoSourceFormatToUUID(LibMCDriver_Camera::eVideoSourceFormat sourceFormat);

public:

	CVideoDeviceInstance_Win32(const std::string & sIdentifier, const std::string & sOSName, const std::string & sFriendlyName, LibMCEnv::PDriverEnvironment pDriverEnvironment);

	virtual ~CVideoDeviceInstance_Win32();

	std::string getFriendlyName();

	std::string getOperatingSystemName();

	bool isVideoDevice();

	std::string getIdentifier();

	uint32_t getSupportedResolutionCount();

	void getSupportedResolution(uint32_t nIndex, uint32_t& nWidth, uint32_t& nHeight, uint32_t& nFramerate, LibMCDriver_Camera::eVideoSourceFormat & sourceFormat);

	void getCurrentResolution(uint32_t & nWidth, uint32_t& nHeight, uint32_t & nFramerate, LibMCDriver_Camera::eVideoSourceFormat& sourceFormat);

	void setResolution(uint32_t nWidth, uint32_t nHeight, uint32_t nFramerate, LibMCDriver_Camera::eVideoSourceFormat& sourceFormat);

	void startStreamCapture(LibMCEnv::PVideoStream pStreamInstance);

	void stopStreamCapture();

	bool streamCaptureIsActive();

	void getStreamCaptureStatistics(LibMCDriver_Camera_double & dDesiredFramerate, LibMCDriver_Camera_double & dMinFramerate, LibMCDriver_Camera_double & dMaxFramerate, LibMCDriver_Camera_double & dMeanFramerate, LibMCDriver_Camera_double & dStdDevFramerate);

	bool captureRawImage(LibMCEnv::PImageData pImageData);

	static std::string getSourceFormatDescription(const LibMCDriver_Camera::eVideoSourceFormat eSourceFormat);

};

typedef std::shared_ptr<CVideoDeviceInstance_Win32> PVideoDeviceInstance_Win32;

} // namespace Impl
} // namespace LibMCDriver_Camera


#endif // __LIBMCDRIVER_CAMERA_VIDEODEVICEINSTANCE_WIN32
