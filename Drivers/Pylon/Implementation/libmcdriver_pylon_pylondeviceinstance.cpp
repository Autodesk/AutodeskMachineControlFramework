/*++

Copyright (C) 2025 Autodesk Inc.

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


Abstract: This is a stub class definition of CPylonDeviceInstance

*/

#include "libmcdriver_pylon_pylondeviceinstance.hpp"
#include "libmcdriver_pylon_interfaceexception.hpp"

//#include <iostream>
#include <cstring>

// Include custom headers here.
#define PYLONFEATURENAME_AUTOEXPOSURE "ExposureAuto"
#define PYLONFEATURENAME_EXPOSURETIME "ExposureTime"

/* pylon device access modes */
#define PYLONACCESS_MODE_MONITOR  0
#define PYLONACCESS_MODE_CONTROL   1
#define PYLONACCESS_MODE_STREAM    2
#define PYLONACCESS_MODE_EVENT     4
#define PYLONACCESS_MODE_EXCLUSIVE 8

using namespace LibMCDriver_Pylon::Impl;

/*************************************************************************************************************************
 Class definition of CPylonDevice 
**************************************************************************************************************************/

CPylonDeviceInstance::CPylonDeviceInstance(const std::string & sIdentifier, PLibPylonSDK pPylonSDK, libPylonDeviceHandle pDeviceHandle)
	: m_sIdentifier(sIdentifier), m_pPylonSDK(pPylonSDK), m_pDeviceHandle(pDeviceHandle)
{
    if (pPylonSDK.get() == nullptr)
        throw ELibMCDriver_PylonInterfaceException(LIBMCDRIVER_PYLON_ERROR_INVALIDPARAM);
    if (pDeviceHandle == nullptr)
        throw ELibMCDriver_PylonInterfaceException(LIBMCDRIVER_PYLON_ERROR_INVALIDPARAM);

    m_pPylonSDK->checkError (m_pPylonSDK->PylonDeviceOpen(m_pDeviceHandle, PYLONACCESS_MODE_CONTROL | PYLONACCESS_MODE_STREAM));

}

CPylonDeviceInstance::~CPylonDeviceInstance()
{
    if (m_pPylonSDK.get() != nullptr) {

        if (m_pDeviceHandle != nullptr) {
			pylonBool bIsOpen = 0;
            pylonResult nResult = m_pPylonSDK->PylonDeviceIsOpen(m_pDeviceHandle, &bIsOpen);

            if (nResult == 0) {
                if (bIsOpen != 0) {
                    m_pPylonSDK->PylonDeviceClose(m_pDeviceHandle);
                }
            }
		}

        m_pPylonSDK->PylonDestroyDevice(m_pDeviceHandle);
    }
    m_pDeviceHandle = nullptr;
    m_pPylonSDK = nullptr;
}

void CPylonDeviceInstance::close()
{
    if (m_pPylonSDK.get() != nullptr) {
        m_pPylonSDK->checkError(m_pPylonSDK->PylonDeviceClose(m_pDeviceHandle));
    }

}

bool CPylonDeviceInstance::isOpen()
{
    pylonBool bIsOpen = 0;
    m_pPylonSDK->checkError(m_pPylonSDK->PylonDeviceIsOpen(m_pDeviceHandle, &bIsOpen));

    return bIsOpen != 0;
}

std::string CPylonDeviceInstance::getIdentifier()
{
    return m_sIdentifier;
}

bool CPylonDeviceInstance::featureIsAvailable(const std::string& sFeatureName)
{
    return m_pPylonSDK->PylonDeviceFeatureIsAvailable(m_pDeviceHandle, sFeatureName.c_str ()) != 0;
}

bool CPylonDeviceInstance::featureIsImplemented(const std::string& sFeatureName)
{
    return m_pPylonSDK->PylonDeviceFeatureIsImplemented(m_pDeviceHandle, sFeatureName.c_str()) != 0;
}

bool CPylonDeviceInstance::featureIsReadable(const std::string& sFeatureName)
{
    return m_pPylonSDK->PylonDeviceFeatureIsReadable(m_pDeviceHandle, sFeatureName.c_str()) != 0;
}

bool CPylonDeviceInstance::featureIsWriteable(const std::string& sFeatureName)
{
    return m_pPylonSDK->PylonDeviceFeatureIsWritable(m_pDeviceHandle, sFeatureName.c_str()) != 0;
}

void CPylonDeviceInstance::setFloatFeature(const std::string& sFeatureName, const double dFloatValue)
{
    m_pPylonSDK->checkError (m_pPylonSDK->PylonDeviceSetFloatFeature(m_pDeviceHandle, sFeatureName.c_str(), dFloatValue));
}

double CPylonDeviceInstance::getFloatFeature(const std::string& sFeatureName)
{
    double dValue = 0.0;
    m_pPylonSDK->checkError(m_pPylonSDK->PylonDeviceGetFloatFeature(m_pDeviceHandle, sFeatureName.c_str(), &dValue));
    return dValue;

}

void CPylonDeviceInstance::getFloatFeatureRange(const std::string& sFeatureName, double& dMinFloatValue, double& dMaxFloatValue)
{
    dMinFloatValue = 0.0;
    dMaxFloatValue = 0.0;
    m_pPylonSDK->checkError(m_pPylonSDK->PylonDeviceGetFloatFeatureMin(m_pDeviceHandle, sFeatureName.c_str(), &dMinFloatValue));
    m_pPylonSDK->checkError(m_pPylonSDK->PylonDeviceGetFloatFeatureMax(m_pDeviceHandle, sFeatureName.c_str(), &dMaxFloatValue));


}


void CPylonDeviceInstance::setIntegerFeature(const std::string& sFeatureName, const int64_t nIntegerValue)
{
    m_pPylonSDK->checkError(m_pPylonSDK->PylonDeviceSetIntegerFeature(m_pDeviceHandle, sFeatureName.c_str(), nIntegerValue));

}

LibMCDriver_Pylon_int64 CPylonDeviceInstance::getIntegerFeature(const std::string& sFeatureName)
{
    int64_t nValue = 0;
    m_pPylonSDK->checkError(m_pPylonSDK->PylonDeviceGetIntegerFeature(m_pDeviceHandle, sFeatureName.c_str(), &nValue));
    return nValue;

}

void CPylonDeviceInstance::getIntegerFeatureRange(const std::string& sFeatureName, int64_t& nMinIntegerValue, int64_t& nMaxIntegerValue)
{
    nMinIntegerValue = 0;
    nMaxIntegerValue = 0;
    m_pPylonSDK->checkError(m_pPylonSDK->PylonDeviceGetIntegerFeatureMin(m_pDeviceHandle, sFeatureName.c_str(), &nMinIntegerValue));
    m_pPylonSDK->checkError(m_pPylonSDK->PylonDeviceGetIntegerFeatureMax(m_pDeviceHandle, sFeatureName.c_str(), &nMinIntegerValue));
}

void CPylonDeviceInstance::setBoolFeature(const std::string& sFeatureName, const bool bBoolValue)
{
    pylonBool bPylonBool = 0;
    if (bBoolValue)
        bPylonBool = 1;

    m_pPylonSDK->checkError(m_pPylonSDK->PylonDeviceSetBooleanFeature(m_pDeviceHandle, sFeatureName.c_str(), bPylonBool));

}

bool CPylonDeviceInstance::getBoolFeature(const std::string& sFeatureName)
{
    pylonBool bPylonBool = 0;
    m_pPylonSDK->checkError(m_pPylonSDK->PylonDeviceGetBooleanFeature(m_pDeviceHandle, sFeatureName.c_str(), &bPylonBool));
    return bPylonBool != 0;

}

void CPylonDeviceInstance::setAutoExposure(const bool bDoAutoExpose)
{
    setBoolFeature(PYLONFEATURENAME_EXPOSURETIME, bDoAutoExpose);
}

bool CPylonDeviceInstance::getAutoExposure()
{
    return getBoolFeature(PYLONFEATURENAME_EXPOSURETIME);
}

void CPylonDeviceInstance::setExposureTime(const double dExposureTime)
{
    setFloatFeature(PYLONFEATURENAME_EXPOSURETIME, dExposureTime);
}

double CPylonDeviceInstance::getExposureTime()
{
    return getFloatFeature(PYLONFEATURENAME_EXPOSURETIME);
}

void CPylonDeviceInstance::grabSingleGreyscaleImage(const LibMCDriver_Pylon_uint32 nChannelID, const LibMCDriver_Pylon_uint32 nTimeoutInMS, LibMCEnv::PImageData pImageInstance)
{
    if (pImageInstance.get () == nullptr)
		throw ELibMCDriver_PylonInterfaceException(LIBMCDRIVER_PYLON_ERROR_INVALIDPARAM);

    int64_t nPayloadSize = getIntegerFeature("PayloadSize");
    if (nPayloadSize <= 0)
		throw ELibMCDriver_PylonInterfaceException(LIBMCDRIVER_PYLON_ERROR_INVALIDPAYLOADSIZE);

    std::vector<uint8_t> payloadBuffer;
    payloadBuffer.resize((size_t)nPayloadSize);
	sPylonGrabResult grabResult;
	memset((void*)&grabResult, 0, sizeof(sPylonGrabResult));
	pylonBool bReady = 0;

	m_pPylonSDK->checkError(m_pPylonSDK->PylonDeviceGrabSingleFrame(m_pDeviceHandle, nChannelID, payloadBuffer.data (), nPayloadSize, &grabResult, &bReady, nTimeoutInMS));

    if (bReady == 0)
		throw ELibMCDriver_PylonInterfaceException(LIBMCDRIVER_PYLON_ERROR_IMAGENOTREADY);

	if (grabResult.m_Status != ePylonGrabStatus::pgsGrabbed)
        throw ELibMCDriver_PylonInterfaceException(LIBMCDRIVER_PYLON_ERROR_COULDNOTGRABIMAGE, "Could not grab image (#" + std::to_string (grabResult.m_nErrorCode) + ")");

    //if ((grabResult.m_OffsetX != 0) || (grabResult.m_OffsetY != 0))
        //throw ELibMCDriver_PylonInterfaceException(LIBMCDRIVER_PYLON_ERROR_UNSUPPORTEDGRABOFFSET, "Unsupported Grab Offset (" + std::to_string(grabResult.m_OffsetX) + "/" + std::to_string(grabResult.m_OffsetY) + ")");

	//std::cout << "Grab offset: " << std::to_string(grabResult.m_OffsetX) + "/" + std::to_string(grabResult.m_OffsetY) << std::endl;
    //std::cout << "Padding: " << std::to_string(grabResult.m_PaddingX) + "/" + std::to_string(grabResult.m_PaddingY) << std::endl;

    if ((grabResult.m_SizeX <= 0) || (grabResult.m_SizeY <= 0))
        throw ELibMCDriver_PylonInterfaceException(LIBMCDRIVER_PYLON_ERROR_INVALIDGRABIMAGESIZE, "Invalid Grab Image Size (" + std::to_string(grabResult.m_SizeX) + "/" + std::to_string(grabResult.m_SizeY) + ")");

	uint32_t nImageSizeX = 0;
    uint32_t nImageSizeY = 0;
    pImageInstance->GetSizeInPixels(nImageSizeX, nImageSizeY);

    if ((nImageSizeX != (uint32_t)grabResult.m_SizeX) || (nImageSizeY != (uint32_t)grabResult.m_SizeY))
		throw ELibMCDriver_PylonInterfaceException(LIBMCDRIVER_PYLON_ERROR_IMAGEDATASIZEMISMATCH, "Image data size does not match grabbed image size " + 
            std::to_string(nImageSizeX) + "x" + std::to_string(nImageSizeY) + " != " +
			std::to_string(grabResult.m_SizeX) + "x" + std::to_string(grabResult.m_SizeY));

    //if ((grabResult.m_PaddingX != 0) || (grabResult.m_PaddingY != 0))
//		throw ELibMCDriver_PylonInterfaceException(LIBMCDRIVER_PYLON_ERROR_UNSUPPORTEDGRABPADDING, "Unsupported Grab Padding (" + std::to_string(grabResult.m_PaddingX) + "/" + std::to_string(grabResult.m_PaddingY) + ")");
	
	pImageInstance->SetPixels(0, 0, nImageSizeX, nImageSizeY, LibMCEnv::eImagePixelFormat::GreyScale8bit, payloadBuffer);

}
