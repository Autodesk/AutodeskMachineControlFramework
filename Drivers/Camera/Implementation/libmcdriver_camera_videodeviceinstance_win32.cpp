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

#include <array>

using namespace LibMCDriver_Camera::Impl;

#ifdef _WIN32

using Microsoft::WRL::ComPtr;

extern std::string wstring_to_utf8(const std::wstring& wstr);

#endif


/*************************************************************************************************************************
 Class definition of CVideoDevice 
**************************************************************************************************************************/

CVideoDeviceInstance_Win32::CVideoDeviceInstance_Win32(const std::string& sIdentifier, const std::string& sOSName, const std::string& sFriendlyName)
    : m_sIdentifier (sIdentifier)
{

#ifdef _WIN32

    ComPtr<IMFAttributes> pConfig = nullptr;

    // Create attributes to specify the capture device
    HRESULT hResult = MFCreateAttributes(&pConfig, 1);
    if (hResult != S_OK)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_COULDNOTCREATEWMFATTRIBUTES, "could not create Windows Media Foundation Attributes: " + std::to_string(hResult));

    hResult = pConfig->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
    if (hResult != S_OK)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_COULDNOTFILTERFORVIDEODEVICES, "could not filter for video devices: " + std::to_string(hResult));

    // Get the list of video capture devices
    IMFActivate** devicesPtr = nullptr;
    std::vector<ComPtr<IMFActivate>> foundDevices;

    // Store the devices memory-safe
    UINT32 nDeviceCount = 0;
    hResult = MFEnumDeviceSources(pConfig.Get(), &devicesPtr, &nDeviceCount);
    if (hResult != S_OK)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_COULDNOTENUMERATEDEVICESOURCES, "could not enumerate device sources: " + std::to_string(hResult));

    for (uint32_t nDeviceIndex = 0; nDeviceIndex < nDeviceCount; nDeviceIndex++)
        foundDevices.push_back(devicesPtr[nDeviceIndex]);
    CoTaskMemFree(devicesPtr);

    IMFActivate* pFoundDevice = nullptr;


    for (uint32_t nDeviceIndex = 0; nDeviceIndex < nDeviceCount; nDeviceIndex++) {

        std::string sDeviceFriendlyName;
        std::string sDeviceSymbolicLink;
        bool bIsVideo = false;

        bool bSuccess = true;

        auto pDevice = foundDevices.at (nDeviceIndex).Get ();

        // get Friendly Name
        {
            std::array<wchar_t, 256> bufferFriendlyName;
            UINT32 lengthFriendlyName = 0;
            hResult = pDevice->GetString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, bufferFriendlyName.data(), (uint32_t)bufferFriendlyName.size(), &lengthFriendlyName);
            if (hResult == S_OK) {
                if (lengthFriendlyName >= bufferFriendlyName.size())
                    lengthFriendlyName = (uint32_t)bufferFriendlyName.size() - 1;
                bufferFriendlyName.at(lengthFriendlyName) = 0;
                sDeviceFriendlyName = wstring_to_utf8(bufferFriendlyName.data());
            }
            else {
                bSuccess = false;
            }
        }

        // get Symbolic Link Name
        {
            std::array<wchar_t, 1024> bufferSymbolicLink;
            UINT32 lengthSymbolicLink = 0;
            hResult = pDevice->GetString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK, bufferSymbolicLink.data(), (uint32_t)bufferSymbolicLink.size(), &lengthSymbolicLink);
            if (hResult == S_OK) {
                if (lengthSymbolicLink >= bufferSymbolicLink.size())
                    lengthSymbolicLink = (uint32_t)bufferSymbolicLink.size() - 1;
                bufferSymbolicLink.at(lengthSymbolicLink) = 0;
                sDeviceSymbolicLink = wstring_to_utf8(bufferSymbolicLink.data());
            }
            else {
                bSuccess = false;
            }
        }

        {
            GUID sourceType;
            hResult = pDevice->GetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, &sourceType);
            if (hResult == S_OK) {
                bIsVideo = (sourceType == MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
            }
            else {
                bSuccess = false;
            }

        }

        if (bSuccess) {
            if (sOSName == sDeviceSymbolicLink) {
                m_sFriendlyName = sDeviceFriendlyName;
                m_sOSName = sDeviceSymbolicLink;
                pFoundDevice = pDevice;
                break;
            }
        }

    }


    if (pFoundDevice == nullptr)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_CAMERADEVICENOTFOUND, "camera device not found: " + sOSName);

    ComPtr<IMFMediaSource> mediaSource = nullptr;
    hResult = pFoundDevice->ActivateObject(__uuidof(IMFMediaSource), (void**)&mediaSource);

    if (hResult != S_OK)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_COULDNOTACTIVATEDEVICE, "could not activate device: " + sFriendlyName + " (" + std::to_string(hResult) + ")");
        
    // Create a media reader for the camera
    hResult = MFCreateSourceReaderFromMediaSource(mediaSource.Get(), nullptr, &m_pSourceReader);
    if (hResult != S_OK)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_COULDNOTCREATEREADER, "could not create reader: " + sFriendlyName + " (" + std::to_string(hResult) + ")");

#endif _WIN32


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
#ifdef _WIN32

    
    ComPtr<IMFMediaType> pType = nullptr;

    uint32_t nNumerator = 30;
    uint32_t nDenominator = 1;

    HRESULT hResult;

    // Create a new media type for video
    hResult = MFCreateMediaType(&pType);
    if (hResult != S_OK)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_COULDNOTCREATEMEDIATYPE, "Could not create media type: " + std::to_string (hResult));

    // Set the media type to video
    hResult = pType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    if (hResult != S_OK)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_COULDNOTSETMEDIATYPETOVIDEO, "Could not set media type to video: " + std::to_string(hResult));

    // Set the video format to MJPEG or another format (e.g., YUY2, RGB32)
    hResult = pType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_MJPG);
    if (hResult != S_OK)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_COULDNOTSETMEDIATYPEFORMAT, "Could not set media type format: " + std::to_string(hResult));

    // Set the resolution (width x height)
    hResult = MFSetAttributeSize(pType.Get (), MF_MT_FRAME_SIZE, nWidth, nHeight);
    if (hResult != S_OK)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_COULDNOTSETMEDIARESOLUTION, "Could not set media resolution: " + std::to_string(hResult));

    // Set the frame rate (numerator/denominator)
    hResult = MFSetAttributeRatio(pType.Get(), MF_MT_FRAME_RATE, nNumerator, nDenominator);
    if (hResult != S_OK)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_COULDNOTSETMEDIAFRAMERATE, "Could not set media framerate: " + std::to_string(hResult));

    // Optionally set the pixel aspect ratio if needed (usually 1:1)
    hResult = MFSetAttributeRatio(pType.Get(), MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
    if (hResult != S_OK)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_COULDNOTSETPIXELASPECTRATIO, "Could not set pixel aspect ratio: " + std::to_string(hResult));

    // Apply the media type to the source reader (video stream)
    hResult = m_pSourceReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, NULL, pType.Get());
    if (hResult != S_OK)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_COULDNOTSETMEDIATYPE, "Could not set media type: " + std::to_string(hResult));

#endif //_WIN32
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

