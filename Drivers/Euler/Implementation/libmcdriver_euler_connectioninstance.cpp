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

*/

#include "libmcdriver_euler_connectioninstance.hpp"
#include "libmcdriver_euler_interfaceexception.hpp"

// Include custom headers here.
#include <cstring>

using namespace LibMCDriver_Euler::Impl;


CEulerConnectionInstance::CEulerConnectionInstance(PLibEulerSDK pLibEulerSDK, const std::string& sIdentifier, const std::string& sApplicationName, const std::string& sApplicationVersion, const std::string& sBaseURL, const std::string& sAPIKey, const std::string& sDeviceID)
    : m_pLibEulerSDK (pLibEulerSDK), m_hConnectionHandle (nullptr)
{
    if (pLibEulerSDK.get() == nullptr)
        throw ELibMCDriver_EulerInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
        
    m_sIdentifier = sIdentifier;

    m_sApplicationName = sApplicationName;

    m_sApplicationVersion = sApplicationVersion;

    m_sBaseURL = sBaseURL;

    m_sAPIKey = sAPIKey;

    m_sDeviceID = sDeviceID;

    memset((void*)&m_ConnectConfig, 0, sizeof(m_ConnectConfig));

    m_ConnectConfig.m_pszApplicationName = m_sApplicationName.c_str();
    m_ConnectConfig.m_pszApplicationVersion = m_sApplicationVersion.c_str();
    m_ConnectConfig.m_pszAPIKey = m_sAPIKey.c_str();
    m_ConnectConfig.m_pszBaseURL = m_sBaseURL.c_str();
    m_ConnectConfig.m_pszDeviceID = m_sDeviceID.c_str();
    m_ConnectConfig.m_bEnabledStdOutLogging = 0;

    m_hConnectionHandle = m_pLibEulerSDK->euler_connect_new(&m_ConnectConfig);

    if (m_hConnectionHandle == nullptr)
        throw ELibMCDriver_EulerInterfaceException(LIBMCDRIVER_EULER_ERROR_COULDNOTCONNECT);
}

CEulerConnectionInstance::~CEulerConnectionInstance()
{
    closeConnection();

	m_pLibEulerSDK = nullptr;

}

std::string CEulerConnectionInstance::getIdentifier()
{
    return m_sIdentifier;
}

std::string CEulerConnectionInstance::getApplicationName()
{
    return m_sApplicationName;
}

std::string CEulerConnectionInstance::getApplicationVersion()
{
    return m_sApplicationVersion;
}

std::string CEulerConnectionInstance::getBaseURL()
{
    return m_sBaseURL;
}

std::string CEulerConnectionInstance::getAPIKey()
{
    return m_sAPIKey;
}

std::string CEulerConnectionInstance::getDeviceID()
{
    return m_sDeviceID;
}

void CEulerConnectionInstance::startHeartBeat()
{
    if (m_hConnectionHandle == nullptr)
        throw ELibMCDriver_EulerInterfaceException(LIBMCDRIVER_EULER_ERROR_HASBEENDISCONNECTED);

	bool bSuccess = m_pLibEulerSDK->euler_connect_start_heartbeat(m_hConnectionHandle);
    if (!bSuccess)
		throw ELibMCDriver_EulerInterfaceException(LIBMCDRIVER_EULER_ERROR_COULDNOTSTARTHEARTBEAT);

}

void CEulerConnectionInstance::stopHeartBeat()
{
    if (m_hConnectionHandle == nullptr)
        throw ELibMCDriver_EulerInterfaceException(LIBMCDRIVER_EULER_ERROR_HASBEENDISCONNECTED);

    bool bSuccess = m_pLibEulerSDK->euler_connect_stop_heartbeat(m_hConnectionHandle);
    if (!bSuccess)
        throw ELibMCDriver_EulerInterfaceException(LIBMCDRIVER_EULER_ERROR_COULDNOTSTOPHEARTBEAT);

}

std::string CEulerConnectionInstance::createBuild(const std::string& sJobName, const LibMCDriver_Euler_uint32 nLayerCount)
{
    if (sJobName.empty ())
        throw ELibMCDriver_EulerInterfaceException(LIBMCDRIVER_EULER_ERROR_EMPTYJOBNAME);

    if (m_hConnectionHandle == nullptr)
        throw ELibMCDriver_EulerInterfaceException(LIBMCDRIVER_EULER_ERROR_HASBEENDISCONNECTED);

	char * pszNewJobID = m_pLibEulerSDK->euler_connect_new_build_job(m_hConnectionHandle, sJobName.c_str(), nLayerCount);
    if (pszNewJobID != nullptr) {
        std::string sNewJobID (pszNewJobID);

        return sNewJobID;
    } else 
        throw ELibMCDriver_EulerInterfaceException(LIBMCDRIVER_EULER_ERROR_COULDNOTCREATEBUILD, "could not create build: " + sJobName);

    

}

void CEulerConnectionInstance::uploadImage(const std::string& sBuildJobID, const LibMCDriver_Euler_uint32 nLayerIndex, const LibMCDriver_Euler::eEulerImageType eImageType, LibMCEnv::PImageData pImage)
{
    if (m_hConnectionHandle == nullptr)
        throw ELibMCDriver_EulerInterfaceException(LIBMCDRIVER_EULER_ERROR_HASBEENDISCONNECTED);

    if (sBuildJobID.empty ())
        throw ELibMCDriver_EulerInterfaceException(LIBMCDRIVER_EULER_ERROR_INVALIDBUILDJOBID);

    if (pImage.get() == nullptr)
        throw ELibMCDriver_EulerInterfaceException(LIBMCDRIVER_EULER_ERROR_HASBEENDISCONNECTED);

    auto pJPEGOptions = pImage->CreateJPEGOptions();
    pJPEGOptions->ResetToDefaults();

    auto pJPEGImage = pImage->CreateJPEGImage(pJPEGOptions);

    std::vector<uint8_t> jpegBuffer;
    pJPEGImage->GetJPEGDataStream(jpegBuffer);

    if (jpegBuffer.size() == 0)
		throw ELibMCDriver_EulerInterfaceException(LIBMCDRIVER_EULER_ERROR_COULDNOTENCODEJPEGIMAGE);

    uint32_t imageTypeConst = 0;
    switch (eImageType) {
    case LibMCDriver_Euler::eEulerImageType::Recoat:
        imageTypeConst = EULERSDK_IMAGETYPE_RECOAT;
        break;
    case LibMCDriver_Euler::eEulerImageType::Exposure:
        imageTypeConst = EULERSDK_IMAGETYPE_EXPOSURE;
        break;
    default:
        throw ELibMCDriver_EulerInterfaceException(LIBMCDRIVER_EULER_ERROR_INVALIDIMAGETYPE);
    }

    bool bSuccess = m_pLibEulerSDK->euler_connect_upload_layer_image(m_hConnectionHandle, sBuildJobID.c_str(), nLayerIndex, imageTypeConst, jpegBuffer.data(), jpegBuffer.size());
    if (!bSuccess)
        throw ELibMCDriver_EulerInterfaceException(LIBMCDRIVER_EULER_ERROR_COULDNOTUPLOADLAYERIMAGE);

}

void CEulerConnectionInstance::setJobStatus(const std::string& sBuildJobID, const LibMCDriver_Euler::eEulerJobStatus eJobStatus)
{
    if (m_hConnectionHandle == nullptr)
        throw ELibMCDriver_EulerInterfaceException(LIBMCDRIVER_EULER_ERROR_HASBEENDISCONNECTED);

    if (sBuildJobID.empty())
        throw ELibMCDriver_EulerInterfaceException(LIBMCDRIVER_EULER_ERROR_INVALIDBUILDJOBID);

    uint32_t jobStatusConst = 0;
    switch (eJobStatus) {
    case LibMCDriver_Euler::eEulerJobStatus::InProgress:
        jobStatusConst = EULERSDK_JOBSTATUS_INPROGRESS;
        break;
    case LibMCDriver_Euler::eEulerJobStatus::Completed:
        jobStatusConst = EULERSDK_JOBSTATUS_COMPLETED;
        break;
    default:
        throw ELibMCDriver_EulerInterfaceException(LIBMCDRIVER_EULER_ERROR_INVALIDJOBSTATUS);
    }


    bool bSuccess = m_pLibEulerSDK->euler_connect_set_job_status(m_hConnectionHandle, sBuildJobID.c_str(), jobStatusConst);
    if (!bSuccess)
        throw ELibMCDriver_EulerInterfaceException(LIBMCDRIVER_EULER_ERROR_COULDUPDATEJOBSTATUS);

}


void CEulerConnectionInstance::closeConnection()
{
    if (m_pLibEulerSDK.get() != nullptr) {
        if (m_hConnectionHandle != nullptr)
            m_pLibEulerSDK->euler_connect_destroy(m_hConnectionHandle);
    }

    m_hConnectionHandle = nullptr;
}

