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
#define NOMINMAX

#include "libmcdriver_camera_videodeviceinstance_win32.hpp"
#include "libmcdriver_camera_interfaceexception.hpp"

#include <array>
#include <chrono>
#include <iostream>
#include <cmath>
#include <algorithm>

using namespace LibMCDriver_Camera::Impl;

#ifdef _WIN32

using Microsoft::WRL::ComPtr;

extern std::string wstring_to_utf8(const std::wstring& wstr);

#endif


/*************************************************************************************************************************
 Class definition of CVideoDevice 
**************************************************************************************************************************/

CVideoDeviceInstance_Win32::CVideoDeviceInstance_Win32(const std::string& sIdentifier, const std::string& sOSName, const std::string& sFriendlyName, LibMCEnv::PDriverEnvironment pDriverEnvironment)
    : m_sIdentifier (sIdentifier), 
    m_nCurrentResolutionX (0),
    m_nCurrentResolutionY (0), 
    m_nCurrentFrameRate (0),
    m_SourceFormat (eVideoSourceFormat::Invalid),
    m_pDriverEnvironment (pDriverEnvironment),
    m_bCapturing (false),
    m_dDesiredFramerateInFPS (0.0)
{
    if (pDriverEnvironment.get() == nullptr)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_INVALIDPARAM);

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

    refreshSupportedResolutions();

#endif //_WIN32


}

#ifdef _WIN32

std::string GUIDToString(const GUID& guid) {
    wchar_t guidString[39] = { 0 };  // GUID string format is 38 characters long plus null terminator
    int nCharsWritten = StringFromGUID2(guid, guidString, 39);
    if (nCharsWritten == 0)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_INVALIDPARAM, "could not convert GUID to string");
    guidString[38] = 0;

    // GUID strings are pure ASCII, so a simple narrowing loop is safe
    std::string sResult;
    sResult.reserve(38);
    for (int i = 0; i < 38 && guidString[i] != 0; i++) {
        sResult.push_back(static_cast<char>(guidString[i]));
    }

    return sResult;
}
#endif

void CVideoDeviceInstance_Win32::refreshSupportedResolutions()
{
#ifdef _WIN32

    HRESULT hResult;

    if (m_pSourceReader.Get () == nullptr)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_NOMEDIASOURCEREADERAVAILABLE);


    ComPtr<IMFMediaType> pNativeType = nullptr;
    DWORD nIndex = 0;

    m_SupportedResolutions.clear();

    while (m_pSourceReader->GetNativeMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, nIndex, &pNativeType) == S_OK) {
        UINT32 nWidth = 0, nHeight = 0;
        hResult = MFGetAttributeSize(pNativeType.Get(), MF_MT_FRAME_SIZE, &nWidth, &nHeight);
        if (hResult != S_OK)
            throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_COULDNOTGETMEDIATYPESIZE);

        GUID subtype;
        hResult = pNativeType->GetGUID(MF_MT_SUBTYPE, &subtype);
        if (hResult != S_OK) 
            throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_COULDNOTGETMEDIASUBTYPE);

        // Enums are from MFVideoFormat_RGB24, etc..
        std::string sTypeUUID = GUIDToString(subtype);

        UINT32 numerator = 0, denominator = 0;
        MFGetAttributeRatio(pNativeType.Get (), MF_MT_FRAME_RATE, &numerator, &denominator);
        if (hResult != S_OK)
            throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_COULDNOTGETMEDIATYPEFRAMERATE);

        if ((denominator == 0) || (numerator == 0))
            throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_INVALIDMEDIATYPEFRAMERATE);

        bool bUUIDIsSupported = convertUUIDToAnyVideoSourceFormat (sTypeUUID) != LibMCDriver_Camera::eVideoSourceFormat::Invalid;

        if (bUUIDIsSupported) {
            if ((numerator % denominator) == 0) {
                // We only support integer framerates for now!
                uint32_t nFramerate = numerator / denominator;

                if ((nWidth >= CAMERARESOLUTION_MIN) && (nWidth <= CAMERARESOLUTION_MAX) &&
                    (nHeight >= CAMERARESOLUTION_MIN) && (nHeight <= CAMERARESOLUTION_MAX) &&
                    (nFramerate >= CAMERAFRAMERATE_MIN) && (nFramerate <= CAMERAFRAMERATE_MAX)) {

                    m_SupportedResolutions.push_back(std::make_shared<CVideoResolution>(nWidth, nHeight, nFramerate, sTypeUUID));
                }

            }
        }

        nIndex++;
    }

#endif // _WIN32
}


CVideoDeviceInstance_Win32::~CVideoDeviceInstance_Win32()
{
    stopStreamCapture();
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


uint32_t CVideoDeviceInstance_Win32::getSupportedResolutionCount()
{
    return (uint32_t)m_SupportedResolutions.size();
}

LibMCDriver_Camera::eVideoSourceFormat CVideoDeviceInstance_Win32::convertUUIDToValidVideoSourceFormat(const std::string& sUUID)
{
    auto sourceFormat = convertUUIDToAnyVideoSourceFormat(sUUID);
    if (sourceFormat == LibMCDriver_Camera::eVideoSourceFormat::Invalid)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_INVALIDMEDIASUBTYPE, "Invalid media video source type: " + sUUID);

    return sourceFormat;
}

LibMCDriver_Camera::eVideoSourceFormat CVideoDeviceInstance_Win32::convertUUIDToAnyVideoSourceFormat(const std::string& sUUID)
{
#ifdef _WIN32

    GUID typeUUID;
    std::wstring sTypeUUIDW(sUUID.begin(), sUUID.end());
    HRESULT hResult = CLSIDFromString(sTypeUUIDW.c_str(), &typeUUID);
    if (hResult != S_OK)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_INVALIDMEDIASUBTYPE, "Invalid media video source type: " + sUUID);

    if (IsEqualGUID(typeUUID, MFVideoFormat_RGB32))
        return LibMCDriver_Camera::eVideoSourceFormat::RGB32;
    if (IsEqualGUID(typeUUID, MFVideoFormat_ARGB32))
        return LibMCDriver_Camera::eVideoSourceFormat::ARGB32;
    if (IsEqualGUID(typeUUID, MFVideoFormat_RGB24))
        return LibMCDriver_Camera::eVideoSourceFormat::RGB24;
    if (IsEqualGUID(typeUUID, MFVideoFormat_RGB555))
        return LibMCDriver_Camera::eVideoSourceFormat::RGB555;
    if (IsEqualGUID(typeUUID, MFVideoFormat_RGB565))
        return LibMCDriver_Camera::eVideoSourceFormat::RGB565;
    if (IsEqualGUID(typeUUID, MFVideoFormat_L8))
        return LibMCDriver_Camera::eVideoSourceFormat::Grayscale8;
    if (IsEqualGUID(typeUUID, MFVideoFormat_L16))
        return LibMCDriver_Camera::eVideoSourceFormat::Grayscale16;
    if (IsEqualGUID(typeUUID, MFVideoFormat_D16))
        return LibMCDriver_Camera::eVideoSourceFormat::Depth16;
    if (IsEqualGUID(typeUUID, MFVideoFormat_AYUV))
        return LibMCDriver_Camera::eVideoSourceFormat::AYUV;
    if (IsEqualGUID(typeUUID, MFVideoFormat_I420))
        return LibMCDriver_Camera::eVideoSourceFormat::I420;
    if (IsEqualGUID(typeUUID, MFVideoFormat_IYUV))
        return LibMCDriver_Camera::eVideoSourceFormat::IYUV;
    if (IsEqualGUID(typeUUID, MFVideoFormat_NV11))
        return LibMCDriver_Camera::eVideoSourceFormat::NV11;
    if (IsEqualGUID(typeUUID, MFVideoFormat_NV12))
        return LibMCDriver_Camera::eVideoSourceFormat::NV12;
    if (IsEqualGUID(typeUUID, MFVideoFormat_UYVY))
        return LibMCDriver_Camera::eVideoSourceFormat::UYVY;
    if (IsEqualGUID(typeUUID, MFVideoFormat_Y41P))
        return LibMCDriver_Camera::eVideoSourceFormat::Y41P;
    if (IsEqualGUID(typeUUID, MFVideoFormat_Y41T))
        return LibMCDriver_Camera::eVideoSourceFormat::Y41T;
    if (IsEqualGUID(typeUUID, MFVideoFormat_Y42T))
        return LibMCDriver_Camera::eVideoSourceFormat::Y42T;
    if (IsEqualGUID(typeUUID, MFVideoFormat_YUY2))
        return LibMCDriver_Camera::eVideoSourceFormat::YUY2;
    if (IsEqualGUID(typeUUID, MFVideoFormat_YVU9))
        return LibMCDriver_Camera::eVideoSourceFormat::YVU9;
    if (IsEqualGUID(typeUUID, MFVideoFormat_YV12))
        return LibMCDriver_Camera::eVideoSourceFormat::YV12;
    if (IsEqualGUID(typeUUID, MFVideoFormat_YVYU))
        return LibMCDriver_Camera::eVideoSourceFormat::YVYU;
    if (IsEqualGUID(typeUUID, MFVideoFormat_MJPG))
        return LibMCDriver_Camera::eVideoSourceFormat::MJPG;

    return LibMCDriver_Camera::eVideoSourceFormat::Invalid;

#else 
    throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_PLATFORMERROR);
#endif //_WIN32
}

std::string CVideoDeviceInstance_Win32::convertVideoSourceFormatToUUID(LibMCDriver_Camera::eVideoSourceFormat sourceFormat)
{
#ifdef _WIN32

    switch (sourceFormat) {
        case LibMCDriver_Camera::eVideoSourceFormat::RGB32: return GUIDToString(MFVideoFormat_RGB32);
        case LibMCDriver_Camera::eVideoSourceFormat::ARGB32: return GUIDToString(MFVideoFormat_ARGB32);
        case LibMCDriver_Camera::eVideoSourceFormat::RGB24: return GUIDToString(MFVideoFormat_RGB24);
        case LibMCDriver_Camera::eVideoSourceFormat::RGB555: return GUIDToString(MFVideoFormat_RGB555);
        case LibMCDriver_Camera::eVideoSourceFormat::RGB565: return GUIDToString(MFVideoFormat_RGB565);
        case LibMCDriver_Camera::eVideoSourceFormat::Grayscale8: return GUIDToString(MFVideoFormat_L8);
        case LibMCDriver_Camera::eVideoSourceFormat::Grayscale16: return GUIDToString(MFVideoFormat_L16);
        case LibMCDriver_Camera::eVideoSourceFormat::Depth16: return GUIDToString(MFVideoFormat_D16);
        case LibMCDriver_Camera::eVideoSourceFormat::AYUV: return GUIDToString(MFVideoFormat_AYUV);
        case LibMCDriver_Camera::eVideoSourceFormat::I420: return GUIDToString(MFVideoFormat_I420);
        case LibMCDriver_Camera::eVideoSourceFormat::IYUV: return GUIDToString(MFVideoFormat_IYUV);
        case LibMCDriver_Camera::eVideoSourceFormat::NV11: return GUIDToString(MFVideoFormat_NV11);
        case LibMCDriver_Camera::eVideoSourceFormat::NV12: return GUIDToString(MFVideoFormat_NV12);
        case LibMCDriver_Camera::eVideoSourceFormat::UYVY: return GUIDToString(MFVideoFormat_UYVY);
        case LibMCDriver_Camera::eVideoSourceFormat::Y41P: return GUIDToString(MFVideoFormat_Y41P);
        case LibMCDriver_Camera::eVideoSourceFormat::Y41T: return GUIDToString(MFVideoFormat_Y41T);
        case LibMCDriver_Camera::eVideoSourceFormat::Y42T: return GUIDToString(MFVideoFormat_Y42T);
        case LibMCDriver_Camera::eVideoSourceFormat::YUY2: return GUIDToString(MFVideoFormat_YUY2);
        case LibMCDriver_Camera::eVideoSourceFormat::YVU9: return GUIDToString(MFVideoFormat_YVU9);
        case LibMCDriver_Camera::eVideoSourceFormat::YV12: return GUIDToString(MFVideoFormat_YV12);
        case LibMCDriver_Camera::eVideoSourceFormat::YVYU: return GUIDToString(MFVideoFormat_YVYU);
        case LibMCDriver_Camera::eVideoSourceFormat::MJPG: return GUIDToString(MFVideoFormat_MJPG);

        default:
            throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_INVALIDVIDEOSOURCEFORMAT);
    }


#else 
    throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_PLATFORMERROR);
#endif //_WIN32
}


void CVideoDeviceInstance_Win32::getSupportedResolution(uint32_t nIndex, uint32_t& nWidth, uint32_t& nHeight, uint32_t& nFramerate, LibMCDriver_Camera::eVideoSourceFormat& sourceFormat)
{
    if (nIndex >= m_SupportedResolutions.size ())
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_INVALIDRESOLUTIONINDEX, "Invalid resolution index: " + std::to_string(nIndex));

    auto resolution = m_SupportedResolutions.at(nIndex);
    nWidth = resolution->getWidth();
    nHeight = resolution->getHeight();
    nFramerate = resolution->getFramerate();
    sourceFormat = convertUUIDToAnyVideoSourceFormat (resolution->getTypeUUID ());

}


void CVideoDeviceInstance_Win32::getCurrentResolution(uint32_t& nWidth, uint32_t& nHeight, uint32_t& nFramerate, LibMCDriver_Camera::eVideoSourceFormat& sourceFormat)
{
    if ((m_nCurrentResolutionX == 0) || (m_nCurrentResolutionY == 0) || (m_nCurrentFrameRate == 0))
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_CAMERARESOLUTIONNOTSET);

    nWidth = m_nCurrentResolutionX;
    nHeight = m_nCurrentResolutionY;
    nFramerate = m_nCurrentFrameRate;
    sourceFormat = m_SourceFormat;
}

#ifdef _WIN32
/*
ComPtr<IMFTransform> CVideoDeviceInstance_Win32::createMJPEGEncoder(IMFMediaType* pInputMediaType)
{
    if (pInputMediaType == nullptr)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_INVALIDPARAM);

    IMFActivate** ppActivate = nullptr;
    UINT32 numMFTs = 0;

    // Find a hardware encoder MFT that accepts YUY2 as input and outputs MJPEG
    HRESULT hResult = MFTEnumEx(
        MFT_CATEGORY_VIDEO_ENCODER,
        0,
        nullptr,
        nullptr,
        &ppActivate,
        &numMFTs
    );

    if (hResult != S_OK) 
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_COULDNOTQUERYMJPEGENCODERS);

    std::vector<ComPtr<IMFActivate>> resultList;
    if (ppActivate != nullptr) {
        for (uint32_t nIndex = 0; nIndex < numMFTs; nIndex++)
            resultList.push_back(ppActivate[nIndex]);
        CoTaskMemFree(ppActivate);
    }

    if (resultList.size () == 0)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_NOMJPEGENCODERFOUND);

    IMFTransform* pResultTransform = nullptr;

    for (auto pActivate : resultList) {
        IMFTransform* pTransform = nullptr;

        hResult = pActivate->ActivateObject(IID_PPV_ARGS(&pTransform));
        if (hResult != S_OK)
            throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_COULDNOTACTIVATEMJPEGENCODER);

        DWORD index = 0;
        IMFMediaType* pOutputType = nullptr;
        while (pTransform->GetOutputAvailableType(0, index, &pOutputType) == S_OK) {
            GUID subtype;
            hResult = pOutputType->GetGUID(MF_MT_SUBTYPE, &subtype);
            if (hResult == S_OK) {
                // Print the subtype (format)
                if (subtype == MFVideoFormat_RGB24) {
                    std::cout << "Output Format : RGB24" << std::endl;
                }
                else if (subtype == MFVideoFormat_MJPG) {
                    std::cout << "Output Format: MJPEG" << std::endl;
                }
                else if (subtype == MFVideoFormat_YUY2) {
                    std::cout << "Output Format: YUY2" << std::endl;
                }
                else {
                    std::cout << "Output Format: Other" << GUIDToString (subtype) << std::endl;
                }
            }

            index++;
        }
    }

    return pResultTransform;
}
*/
#endif //_WIN32


void CVideoDeviceInstance_Win32::setResolution(uint32_t nWidth, uint32_t nHeight, uint32_t nFramerate, LibMCDriver_Camera::eVideoSourceFormat& sourceFormat)
{
#ifdef _WIN32

    m_pMediaType = nullptr;

    ComPtr<IMFMediaType> pType;

    HRESULT hResult;

    GUID typeUUID;
    std::string sTypeUUID = convertVideoSourceFormatToUUID(sourceFormat);
    std::wstring sTypeUUIDW(sTypeUUID.begin(), sTypeUUID.end());
    hResult = CLSIDFromString(sTypeUUIDW.c_str (), &typeUUID);
    if (hResult != S_OK)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_INVALIDMEDIASUBTYPE, "Invalid media subtype: " + sTypeUUID);

    // Create a new media type for video
    hResult = MFCreateMediaType(&pType);
    if (hResult != S_OK)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_COULDNOTCREATEMEDIATYPE, "Could not create media type: " + std::to_string (hResult));

    // Set the media type to video
    hResult = pType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    if (hResult != S_OK)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_COULDNOTSETMEDIATYPETOVIDEO, "Could not set media type to video: " + std::to_string(hResult));

    // Set the video format to MJPEG or another format (e.g., YUY2, RGB32)
    hResult = pType->SetGUID(MF_MT_SUBTYPE, typeUUID);
    if (hResult != S_OK)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_COULDNOTSETMEDIATYPEFORMAT, "Could not set media type format: " + std::to_string(hResult));

    // Set the resolution (width x height)
    hResult = MFSetAttributeSize(pType.Get (), MF_MT_FRAME_SIZE, nWidth, nHeight);
    if (hResult != S_OK)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_COULDNOTSETMEDIARESOLUTION, "Could not set media resolution: " + std::to_string(hResult));

    // Set the frame rate (numerator/denominator)
    hResult = MFSetAttributeRatio(pType.Get(), MF_MT_FRAME_RATE, nFramerate, 1);
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

    m_pMediaType = pType;
    m_nCurrentResolutionX = nWidth;
    m_nCurrentResolutionY = nHeight;
    m_nCurrentFrameRate = nFramerate;
    m_SourceFormat = sourceFormat;

#else 
    throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_PLATFORMERROR);
#endif //_WIN32
}

void CVideoDeviceInstance_Win32::captureThreadFunction()
{
    while (m_bCapturing.load()) {
        try {
            auto pImage = m_pDriverEnvironment->CreateEmptyImage(
                m_nCurrentResolutionX, m_nCurrentResolutionY,
                72.0, 72.0, LibMCEnv::eImagePixelFormat::RGB24bit);

            auto startTime = std::chrono::high_resolution_clock::now();

            if (captureRawImage(pImage)) {
                m_pActiveStream->PushFrame(pImage.get());
            }

            auto endTime = std::chrono::high_resolution_clock::now();
            double dFrameDurationInSeconds = std::chrono::duration<double>(endTime - startTime).count();

            {
                std::lock_guard<std::mutex> lock(m_StatisticsMutex);
                m_FrameDurationsInSeconds.push_back(dFrameDurationInSeconds);
            }

            // Sleep for the remaining frame time if capture was faster than desired
            if (m_nCurrentFrameRate > 0) {
                double dDesiredDurationInSeconds = 1.0 / (double)m_nCurrentFrameRate;
                double dRemainingInSeconds = dDesiredDurationInSeconds - dFrameDurationInSeconds;
                if (dRemainingInSeconds > 0.0) {
                    auto nSleepMicroseconds = (uint64_t)(dRemainingInSeconds * STREAMCAPTURE_MICROSECONDS_PER_SECOND);
                    std::this_thread::sleep_for(std::chrono::microseconds(nSleepMicroseconds));
                }
            }
        }
        catch (...) {
            // If a single frame fails, continue capturing rather than killing the thread.
            // A brief sleep avoids busy-looping on persistent errors.
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

void CVideoDeviceInstance_Win32::startStreamCapture(LibMCEnv::PVideoStream pStreamInstance)
{
    if (pStreamInstance.get() == nullptr)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_INVALIDPARAM);

    if ((m_nCurrentResolutionX == 0) || (m_nCurrentResolutionY == 0) || (m_nCurrentFrameRate == 0))
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_CAMERARESOLUTIONNOTSET);

    // Validate that camera resolution matches the video stream dimensions
    uint32_t nStreamWidth = pStreamInstance->GetWidth();
    uint32_t nStreamHeight = pStreamInstance->GetHeight();
    if (nStreamWidth != m_nCurrentResolutionX || nStreamHeight != m_nCurrentResolutionY)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_INVALIDPARAM,
            "video stream dimensions (" + std::to_string(nStreamWidth) + "x" + std::to_string(nStreamHeight) +
            ") do not match camera resolution (" + std::to_string(m_nCurrentResolutionX) + "x" + std::to_string(m_nCurrentResolutionY) + ")");

    // Stop any existing capture before starting a new one
    stopStreamCapture();

    m_pActiveStream = pStreamInstance;
    m_dDesiredFramerateInFPS = (double)m_nCurrentFrameRate;

    {
        std::lock_guard<std::mutex> lock(m_StatisticsMutex);
        m_FrameDurationsInSeconds.clear();
    }

    m_bCapturing.store(true);
    m_pCaptureThread = std::make_unique<std::thread>(&CVideoDeviceInstance_Win32::captureThreadFunction, this);
}

void CVideoDeviceInstance_Win32::stopStreamCapture()
{
    m_bCapturing.store(false);

    if (m_pCaptureThread && m_pCaptureThread->joinable()) {
        m_pCaptureThread->join();
    }

    m_pCaptureThread.reset();
    m_pActiveStream = nullptr;
}

bool CVideoDeviceInstance_Win32::streamCaptureIsActive()
{
    return m_bCapturing.load();
}

void CVideoDeviceInstance_Win32::getStreamCaptureStatistics(LibMCDriver_Camera_double & dDesiredFramerate, LibMCDriver_Camera_double & dMinFramerate, LibMCDriver_Camera_double & dMaxFramerate, LibMCDriver_Camera_double & dMeanFramerate, LibMCDriver_Camera_double & dStdDevFramerate)
{
    std::lock_guard<std::mutex> lock(m_StatisticsMutex);

    dDesiredFramerate = m_dDesiredFramerateInFPS;
    dMinFramerate = 0.0;
    dMaxFramerate = 0.0;
    dMeanFramerate = 0.0;
    dStdDevFramerate = 0.0;

    if (m_FrameDurationsInSeconds.size() < STREAMCAPTURE_STATISTICS_MIN_FRAME_COUNT)
        return;

    // Compute min/max/mean of actual frame durations, then convert to FPS
    double dMinDuration = m_FrameDurationsInSeconds[0];
    double dMaxDuration = m_FrameDurationsInSeconds[0];
    double dSumDuration = 0.0;

    for (const auto& dDuration : m_FrameDurationsInSeconds) {
        if (dDuration < dMinDuration)
            dMinDuration = dDuration;
        if (dDuration > dMaxDuration)
            dMaxDuration = dDuration;
        dSumDuration += dDuration;
    }

    double dMeanDuration = dSumDuration / (double)m_FrameDurationsInSeconds.size();

    // Standard deviation of durations
    double dVarianceSum = 0.0;
    for (const auto& dDuration : m_FrameDurationsInSeconds) {
        double dDiff = dDuration - dMeanDuration;
        dVarianceSum += dDiff * dDiff;
    }
    double dStdDevDuration = std::sqrt(dVarianceSum / (double)m_FrameDurationsInSeconds.size());

    // Convert durations to framerates (shorter duration = higher framerate)
    if (dMinDuration > 0.0)
        dMaxFramerate = 1.0 / dMinDuration;
    if (dMaxDuration > 0.0)
        dMinFramerate = 1.0 / dMaxDuration;
    if (dMeanDuration > 0.0) {
        dMeanFramerate = 1.0 / dMeanDuration;
        // Propagate std dev from duration to framerate via |df/dt| = 1/t^2
        dStdDevFramerate = dStdDevDuration / (dMeanDuration * dMeanDuration);
    }
}


inline void convertYUVToRGB24 (uint8_t Y, uint8_t U, uint8_t V, uint8_t & R, uint8_t & G, uint8_t &B)
{
    // YUV to RGB conversion formulas
    int32_t C = (int32_t)Y - 16;
    int32_t D = (int32_t)U - 128;
    int32_t E = (int32_t)V - 128;

    int32_t iR = (298 * C + 409 * E + 128) >> 8;
    int32_t iG = (298 * C - 100 * D - 208 * E + 128) >> 8;
    int32_t iB = (298 * C + 516 * D + 128) >> 8;

    // Clamp RGB values to [0, 255]
    R = (uint8_t) std::min(255, std::max(0, iR));
    G = (uint8_t) std::min(255, std::max(0, iG));
    B = (uint8_t) std::min(255, std::max(0, iB));
}

void ConvertYUY2ToRGB24 (uint8_t* yuy2Data, uint8_t* rgbData, uint32_t nWidth, uint32_t nHeight)
{
    if (yuy2Data == nullptr)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_INVALIDPARAM);
    if (rgbData == nullptr)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_INVALIDPARAM);
    if (nWidth == 0)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_INVALIDPARAM);
    if (nHeight == 0)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_INVALIDPARAM);


    uint64_t pixelIndex = 0;
    uint64_t pixelCount = nWidth * nHeight * 2;
    for (uint64_t i = 0; i < pixelCount; i += 4) {
        // Read the YUY2 data (2 pixels per 4 bytes)
        uint8_t Y1 = yuy2Data[i];
        uint8_t U = yuy2Data[i + 1];
        uint8_t Y2 = yuy2Data[i + 2];
        uint8_t V = yuy2Data[i + 3];

        // Convert YUV to RGB for two pixels
        for (int j = 0; j < 2; j++) {
            uint8_t Y = (j == 0) ? Y1 : Y2;

            convertYUVToRGB24(Y, U, V, rgbData[pixelIndex], rgbData[pixelIndex + 1], rgbData[pixelIndex + 2]);
            pixelIndex += 3;

        }
    }
}


void ConvertNV12ToRGB24(uint8_t* nv12Data, uint8_t* rgbData, uint32_t nWidth, uint32_t nHeight) 
{
    if (nv12Data == nullptr)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_INVALIDPARAM);
    if (rgbData == nullptr)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_INVALIDPARAM);
    if (nWidth == 0)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_INVALIDPARAM);
    if (nHeight == 0)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_INVALIDPARAM);

    uint32_t frameSize = nWidth * nHeight;
    uint8_t* yPlane = nv12Data;
    uint8_t* uvPlane = nv12Data + frameSize;

    uint32_t pixelIndex = 0;
    for (uint32_t y = 0; y < nHeight; ++y) {
        for (uint32_t x = 0; x < nWidth; ++x) {
            // Get Y component
            uint8_t Y = yPlane[y * nWidth + x];

            // Get UV components (subsampled, 2x2 blocks)
            uint32_t uvIndex = (y / 2) * nWidth + (x & ~1);
            uint8_t U = uvPlane[uvIndex];
            uint8_t V = uvPlane[uvIndex + 1];

            convertYUVToRGB24(Y, U, V, rgbData[pixelIndex], rgbData[pixelIndex + 1], rgbData[pixelIndex + 2]);
            pixelIndex += 3;

        }
    }
}

std::string CVideoDeviceInstance_Win32::getSourceFormatDescription(const LibMCDriver_Camera::eVideoSourceFormat eSourceFormat)
{
    switch (eSourceFormat) {
    case eVideoSourceFormat::RGB32:
        return "RGB32";
    case eVideoSourceFormat::ARGB32:
        return "ARGB32";
    case eVideoSourceFormat::RGB24:
        return "RGB24";
    case eVideoSourceFormat::RGB555:
        return "RGB555";
    case eVideoSourceFormat::RGB565:
        return "RGB565";
    case eVideoSourceFormat::Grayscale8:
        return "Grayscale8";
    case eVideoSourceFormat::Grayscale16:
        return "Grayscale16";
    case eVideoSourceFormat::Depth16:
        return "Depth16";
    case eVideoSourceFormat::AYUV:
        return "AYUV";
    case eVideoSourceFormat::I420:
        return "I420";
    case eVideoSourceFormat::IYUV:
        return "IYUV";
    case eVideoSourceFormat::NV11:
        return "NV11";
    case eVideoSourceFormat::NV12:
        return "NV12";
    case eVideoSourceFormat::NV21:
        return "NV21";
    case eVideoSourceFormat::UYVY:
        return "UYVY";
    case eVideoSourceFormat::Y41P:
        return "Y41P";
    case eVideoSourceFormat::Y41T:
        return "Y41T";
    case eVideoSourceFormat::Y42T:
        return "Y42T";
    case eVideoSourceFormat::YUY2:
        return "YUY2";
    case eVideoSourceFormat::YVU9:
        return "YVU9";
    case eVideoSourceFormat::YV12:
        return "YV12";
    case eVideoSourceFormat::YVYU:
        return "YVYU";

    default: return "Invalid";
    }
}

bool CVideoDeviceInstance_Win32::captureRawImage(LibMCEnv::PImageData pImageData)
{

#ifdef _WIN32
    std::vector<uint8_t> rgbBuffer;

    if (pImageData.get() == nullptr)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_INVALIDPARAM);

    if (m_pSourceReader.Get () == nullptr)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_CAMERAREADERNOTINITIALIZED);

    if ((m_nCurrentResolutionX == 0) || (m_nCurrentResolutionY == 0) || (m_nCurrentFrameRate == 0))
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_CAMERARESOLUTIONNOTSET);

    ComPtr<IMFSample> pSample = nullptr;
    DWORD streamIndex = 0;
    DWORD flags = 0;
    LONGLONG llTimeStamp = 0;

    HRESULT hResult = m_pSourceReader->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, &streamIndex, &flags, &llTimeStamp, &pSample);
    if (hResult != S_OK)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_COULDNOTREADSAMPLEFRAME, "could not read sample frame: " + std::to_string (hResult));


    if (pSample.Get () != nullptr) {
        ComPtr<IMFMediaBuffer> pMediaBuffer = nullptr;
        hResult = pSample->ConvertToContiguousBuffer (&pMediaBuffer);

        if (hResult != S_OK)
            throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_COULDNOTREADSAMPLEBUFFER, "could not read sample buffer: " + std::to_string(hResult));

        if (pMediaBuffer.Get() != nullptr) {

            BYTE* rawData = nullptr;
            DWORD maxLength = 0;
            DWORD currentLength = 0;

            hResult = pMediaBuffer->Lock(&rawData, &maxLength, &currentLength);
            if (hResult != S_OK)
                throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_COULDNOTLOCKMEDIABUFFER, "could not lock media buffer: " + std::to_string(hResult));

            if (rawData == nullptr)
                throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_RAWBUFFERRETURNEDNULL);

            rgbBuffer.resize(m_nCurrentResolutionX * m_nCurrentResolutionY * 3);

            try {

                switch (m_SourceFormat) {
                    case eLibMCDriver_CameraVideoSourceFormat::YUY2: {
                        if (currentLength != (m_nCurrentResolutionX * m_nCurrentResolutionY * 2))
                            throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_YUY2SAMPLEBUFFERSIZEMISMATCH);

                        ConvertYUY2ToRGB24(rawData, rgbBuffer.data(), m_nCurrentResolutionX, m_nCurrentResolutionY);
                        break;
                    }

                    case eLibMCDriver_CameraVideoSourceFormat::NV12: {
                        if (currentLength != (m_nCurrentResolutionX * m_nCurrentResolutionY * 3 / 2))
                            throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_NV12SAMPLEBUFFERSIZEMISMATCH);

                        if (((m_nCurrentResolutionX % 2) != 0) || ((m_nCurrentResolutionY % 2) != 0))
                            throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_CAMERAENCODINGNEEDSEVENIMAGEEXTENTS);

                        ConvertNV12ToRGB24(rawData, rgbBuffer.data(), m_nCurrentResolutionX, m_nCurrentResolutionY);

                        break;
                    }
                                                                   

                    default:                                                                    
                        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_SOURCEFORMATNOTSUPPORTED, "Source format not supported: " + getSourceFormatDescription (m_SourceFormat));
                }


            }
            catch (...) {
                pMediaBuffer->Unlock();
                throw;
            }

            pMediaBuffer->Unlock();

            uint32_t nImageSizeX = 0;
            uint32_t nImageSizeY = 0;

            pImageData->SetPixels(0, 0, m_nCurrentResolutionX, m_nCurrentResolutionY, LibMCEnv::eImagePixelFormat::RGB24bit, rgbBuffer);

            return true;
        }
        else {

            pImageData->Clear(0);

            return false;
        }
    }
    else {
        pImageData->Clear(0);

        return false;
    }

#else 
throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_PLATFORMERROR);
#endif //_WIN32

}
