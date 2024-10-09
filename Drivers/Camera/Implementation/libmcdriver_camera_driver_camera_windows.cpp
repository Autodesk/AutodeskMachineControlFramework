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


Abstract: This is a stub class definition of CDriver_Camera_Windows

*/

#include "libmcdriver_camera_driver_camera_windows.hpp"
#include "libmcdriver_camera_interfaceexception.hpp"

// Include custom headers here.
#include "libmcdriver_camera_devicelist.hpp"
#include "libmcdriver_camera_videodevice.hpp"

#ifdef _WIN32
#include <windows.h>
#include <mfapi.h>
#include <mfplay.h>
#include <mfreadwrite.h>
#include <mfobjects.h>
#include <mfidl.h>

#include <wrl/client.h>

#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "mf.lib")

#endif

#include <array>

using namespace LibMCDriver_Camera::Impl;

/*************************************************************************************************************************
 Class definition of CDriver_Camera_Windows 
**************************************************************************************************************************/

#ifdef _WIN32

using Microsoft::WRL::ComPtr;

std::string wstring_to_utf8(const std::wstring& wstr) {

    if (wstr.empty()) {
        return std::string();  // Handle empty string case
    }

    // Get the length of the resulting UTF-8 string
    int utf8_len = WideCharToMultiByte(
        CP_UTF8,              // Code page for UTF-8
        0,                    // Conversion flags
        wstr.c_str(),         // Input UTF-16 string
        static_cast<int>(wstr.size()),  // Input string length
        nullptr,              // No output buffer yet, we want the size only
        0,                    // Specify 0 to calculate the required buffer size
        nullptr,              // No default character substitution
        nullptr               // No default character used flag
    );

    if (utf8_len == 0) {
        throw std::runtime_error("WideCharToMultiByte failed to get required size");
    }

    // Allocate a string for the resulting UTF-8 data
    std::string utf8_str(utf8_len, 0);

    // Convert the UTF-16 string to UTF-8
    int result = WideCharToMultiByte(
        CP_UTF8,
        0,
        wstr.c_str(),
        static_cast<int>(wstr.size()),
        &utf8_str[0],  // Output buffer to hold the UTF-8 string
        utf8_len,
        nullptr,
        nullptr
    );

    if (result == 0) {
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_WIDECHARTOUTF8CONVERSIONFAILED);
    }

    return utf8_str;
}

#endif


CDriver_Camera_Windows::CDriver_Camera_Windows(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment)
    : CDriver_Camera (sName, pDriverEnvironment), m_pDriverEnvironment (pDriverEnvironment)
{

#ifdef _WIN32
    HRESULT hResult = MFStartup(MF_VERSION);
    if (hResult != S_OK)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_COULDNOTINITIALIZEWMF);
#endif // _WIN32
}

CDriver_Camera_Windows::~CDriver_Camera_Windows()
{
#ifdef _WIN32
    MFShutdown();
#endif // _WIN32
}

std::string CDriver_Camera_Windows::getWindowsMFDriverType()
{
    return "windowsmf";
}

std::string CDriver_Camera_Windows::GetType()
{
    return getWindowsMFDriverType();
}

IDeviceList* CDriver_Camera_Windows::EnumerateDevices()
{
    std::unique_ptr<CDeviceList> pDeviceList (new CDeviceList());

#ifdef _WIN32
    ComPtr<IMFAttributes> pConfig = nullptr;

    // Create attributes to specify the capture device
    HRESULT hResult = MFCreateAttributes(&pConfig, 1);
    if (hResult != S_OK)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_COULDNOTCREATEWMFATTRIBUTES, "could not create Windows Media Foundation Attributes: " + std::to_string (hResult));

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

    // Store all found devices
    for (uint32_t nDeviceIndex = 0; nDeviceIndex < nDeviceCount; nDeviceIndex++) {

        std::string sFriendlyName;
        std::string sSymbolicLink;
        bool bIsVideo = false;

        bool bSuccess = true;

        auto pDevice = foundDevices.at (nDeviceIndex);

        // get Friendly Name
        {
            std::array<wchar_t, 256> bufferFriendlyName;
            UINT32 lengthFriendlyName = 0;
            hResult = pDevice->GetString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, bufferFriendlyName.data(), (uint32_t)bufferFriendlyName.size(), &lengthFriendlyName);
            if (hResult == S_OK) {
                if (lengthFriendlyName >= bufferFriendlyName.size())
                    lengthFriendlyName = (uint32_t)bufferFriendlyName.size() - 1;
                bufferFriendlyName.at(lengthFriendlyName) = 0;
                sFriendlyName = wstring_to_utf8(bufferFriendlyName.data());
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
                sSymbolicLink = wstring_to_utf8(bufferSymbolicLink.data());
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

        if (bSuccess)
            pDeviceList->addInfo(std::make_shared<CDeviceInfoInstance>(sFriendlyName, sSymbolicLink, bIsVideo));

    }


#endif _WIN32

    return pDeviceList.release();
}

IVideoDevice* CDriver_Camera_Windows::FindDeviceByIdentifier(const std::string& sIdentifier, const bool bMustExist)
{
    auto iIter = m_VideoDeviceIdentifierMap.find(sIdentifier);
    if (iIter != m_VideoDeviceIdentifierMap.end())
        return new CVideoDevice_Win32(iIter->second);

    if (bMustExist)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_CAMERADEVICENOTFOUND, "camera device not found: " + sIdentifier);

    return nullptr;
}

IVideoDevice* CDriver_Camera_Windows::FindDeviceByOperatingSystemName(const std::string& sOperatingSystemName, const bool bMustExist)
{
    auto iIter = m_VideoDeviceOSNameMap.find(sOperatingSystemName);
    if (iIter != m_VideoDeviceOSNameMap.end())
        return new CVideoDevice_Win32(iIter->second);

    if (bMustExist)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_CAMERADEVICENOTFOUND, "camera device not found: " + sOperatingSystemName);

    return nullptr;
}

IVideoDevice* CDriver_Camera_Windows::OpenVideoDevice(const std::string& sIdentifier, IDeviceInfo* pVideoDeviceInfo)
{
    if (pVideoDeviceInfo == nullptr)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_INVALIDPARAM);

    std::string sFriendlyName = pVideoDeviceInfo->GetFriendlyName();
    std::string sOSName = pVideoDeviceInfo->GetOperatingSystemName();

    if ((sIdentifier.length () < CAMERA_MINIDENTIFIERLENGTH) || (sIdentifier.length() > CAMERA_MAXIDENTIFIERLENGTH))
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_INVALIDCAMERAIDENTIFIERLENGTH);

    for (auto ch : sIdentifier) {
        bool bIsOK = isalnum(ch) || (ch == '_') || (ch == '-') || (ch == '.');
        if (!bIsOK)
            throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_INVALIDCAMERAIDENTIFIER, "invalid camera identifier: " + sIdentifier);
    }

    auto iIter = m_VideoDeviceIdentifierMap.find(sIdentifier);
    if (iIter != m_VideoDeviceIdentifierMap.end())
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_CAMERAIDENTIFIERALREADYREGISTERED, "camera identifier already registered: " + sIdentifier);

    auto pDeviceInstance = std::make_shared<CVideoDeviceInstance_Win32> (sIdentifier, sOSName, sFriendlyName);

    m_VideoDeviceIdentifierMap.insert(std::make_pair (sIdentifier, pDeviceInstance));
    m_VideoDeviceOSNameMap.insert(std::make_pair (sOSName, pDeviceInstance));

    return new CVideoDevice_Win32 (pDeviceInstance);
}

