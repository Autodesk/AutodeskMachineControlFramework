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

CVideoDeviceInstance_Win32::CVideoDeviceInstance_Win32(const std::string& sIdentifier, const std::string& sOSName, const std::string& sFriendlyName)
    : m_sIdentifier (sIdentifier), m_nCurrentResolutionX (0), m_nCurrentResolutionY (0), m_nCurrentFrameRate (0)
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

    refreshSupportedResolutions();

#endif //_WIN32


}

void CVideoDeviceInstance_Win32::refreshSupportedResolutions()
{
#ifdef _WIN32
    if (m_pSourceReader.Get () == nullptr)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_NOMEDIASOURCEREADERAVAILABLE);

    ComPtr<IMFMediaType> pNativeType = nullptr;
    DWORD nIndex = 0;

    m_SupportedResolutions.clear();

    HRESULT hResult;

    while (m_pSourceReader->GetNativeMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, nIndex, &pNativeType) == S_OK) {
        UINT32 nWidth = 0, nHeight = 0;
        hResult = MFGetAttributeSize(pNativeType.Get(), MF_MT_FRAME_SIZE, &nWidth, &nHeight);
        if (hResult != S_OK)
            throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_COULDNOTGETMEDIATYPESIZE);

        UINT32 numerator = 0, denominator = 0;
        MFGetAttributeRatio(pNativeType.Get (), MF_MT_FRAME_RATE, &numerator, &denominator);
        if (hResult != S_OK)
            throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_COULDNOTGETMEDIATYPEFRAMERATE);

        if ((denominator == 0) || (numerator == 0))
            throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_INVALIDMEDIATYPEFRAMERATE);

        if ((numerator % denominator) == 0) { 
            // We only support integer framerates for now!
            uint32_t nFramerate = numerator / denominator;

            if ((nWidth >= CAMERARESOLUTION_MIN) && (nWidth <= CAMERARESOLUTION_MAX) &&
                (nHeight >= CAMERARESOLUTION_MIN) && (nHeight <= CAMERARESOLUTION_MAX) &&
                (nFramerate >= CAMERAFRAMERATE_MIN) && (nFramerate <= CAMERAFRAMERATE_MAX)) {

                m_SupportedResolutions.push_back(std::make_shared<CVideoResolution> (nWidth, nHeight, nFramerate));
            }
                
        }

        nIndex++;
    }

#endif // _WIN32
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


uint32_t CVideoDeviceInstance_Win32::getSupportedResolutionCount()
{
    return (uint32_t)m_SupportedResolutions.size();
}

void CVideoDeviceInstance_Win32::getSupportedResolution(uint32_t nIndex, uint32_t& nWidth, uint32_t& nHeight, uint32_t& nFramerate)
{
    if (nIndex >= m_SupportedResolutions.size ())
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_INVALIDRESOLUTIONINDEX, "Invalid resolution index: " + std::to_string(nIndex));

    auto resolution = m_SupportedResolutions.at(nIndex);
    nWidth = resolution->getWidth();
    nHeight = resolution->getHeight();
    nFramerate = resolution->getFramerate();

}

void CVideoDeviceInstance_Win32::getCurrentResolution(uint32_t& nWidth, uint32_t& nHeight, uint32_t& nFramerate)
{
    nWidth = 0;
    nHeight = 0;
    nFramerate = 0;
}
std::wstring GUIDToString(const GUID& guid) {
    wchar_t guidString[39] = { 0 };  // GUID string format is 38 characters long plus null terminator
    StringFromGUID2(guid, guidString, 39);
    return std::wstring(guidString);
}

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
                    std::wcout << L"Output Format: Other" << GUIDToString (subtype) << std::endl;
                }
            }

            index++;
        }
    }

    return pResultTransform;
}


void CVideoDeviceInstance_Win32::setResolution(uint32_t nWidth, uint32_t nHeight, uint32_t nFramerate)
{
#ifdef _WIN32

    m_pMediaType = nullptr;

    ComPtr<IMFMediaType> pType;

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
    hResult = pType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_YUY2);
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

#endif //_WIN32
}

void CVideoDeviceInstance_Win32::startStreamCapture(LibMCEnv::PVideoStream pStreamInstance)
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
        BYTE Y1 = yuy2Data[i];
        BYTE U = yuy2Data[i + 1];
        BYTE Y2 = yuy2Data[i + 2];
        BYTE V = yuy2Data[i + 3];

        // Convert YUV to RGB for two pixels
        for (int j = 0; j < 2; j++) {
            BYTE Y = (j == 0) ? Y1 : Y2;

            // YUV to RGB conversion formulas
            int C = Y - 16;
            int D = U - 128;
            int E = V - 128;

            int R = (298 * C + 409 * E + 128) >> 8;
            int G = (298 * C - 100 * D - 208 * E + 128) >> 8;
            int B = (298 * C + 516 * D + 128) >> 8;

            // Clamp RGB values to [0, 255]
            R = min(255, max(0, R));
            G = min(255, max(0, G));
            B = min(255, max(0, B));

            // Write the RGB values (RGB24 format)
            rgbData[pixelIndex++] = (BYTE)R;
            rgbData[pixelIndex++] = (BYTE)G;
            rgbData[pixelIndex++] = (BYTE)B;
        }
    }
}

void CVideoDeviceInstance_Win32::captureRawImage(LibMCEnv::PImageData pImageData)
{
    std::vector<uint8_t> rgbBuffer;

    if (pImageData.get() == nullptr)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_INVALIDPARAM);

    if (m_pSourceReader.Get () == nullptr)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_CAMERAREADERNOTINITIALIZED);

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

            if (currentLength != (m_nCurrentResolutionX * m_nCurrentResolutionY * 2)) 
                throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_YUY2SAMPLEBUFFERSIZEMISMATCH);

            rgbBuffer.resize (m_nCurrentResolutionX * m_nCurrentResolutionY * 3);

            ConvertYUY2ToRGB24 (rawData, rgbBuffer.data(), m_nCurrentResolutionX, m_nCurrentResolutionY);

            pMediaBuffer->Unlock();

            uint32_t nImageSizeX = 0;
            uint32_t nImageSizeY = 0;

            pImageData->SetPixels(0, 0, m_nCurrentResolutionX, m_nCurrentResolutionY, LibMCEnv::eImagePixelFormat::RGB24bit, rgbBuffer);

        }
    }

}
