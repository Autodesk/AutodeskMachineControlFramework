/*++

Copyright (C) 2020 Autodesk Inc.

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


Abstract: This is a stub class definition of CDeviceConfiguration

*/

#include "libmcdriver_scanlaboie_deviceconfiguration.hpp"
#include "libmcdriver_scanlaboie_interfaceexception.hpp"

// Include custom headers here.


using namespace LibMCDriver_ScanLabOIE::Impl;

#define OIE_MAX_SIGNALCOUNT 1024

/*************************************************************************************************************************
 Class definition of CDeviceConfiguration 
**************************************************************************************************************************/

CDeviceConfiguration::CDeviceConfiguration(PScanLabOIESDK pSDK, const std::string& sDeviceConfigString, LibMCEnv::PDriverEnvironment pDriverEnvironment)
    : m_pSDK (pSDK), m_RTCDeviceType (LibMCDriver_ScanLabOIE::eRTCDeviceType::Unknown)
{
    if (pSDK.get () == nullptr)
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDPARAM);
    if (pDriverEnvironment.get() == nullptr)
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDPARAM);

    auto pWorkingDirectory = pDriverEnvironment->CreateWorkingDirectory();

    {

        auto pWorkingFile = pWorkingDirectory->StoreCustomStringInTempFile("ini", sDeviceConfigString);
        std::string sTempFileName = pWorkingFile->GetAbsoluteFileName();

        int32_t rtcType = -1;
        m_pSDK->checkError(m_pSDK->oie_get_rtc_type(sTempFileName.c_str(), &rtcType));
        // See OIE Documentation
        if (rtcType == 0)
            m_RTCDeviceType = eRTCDeviceType::RTC5;
        if (rtcType == 1)
            m_RTCDeviceType = eRTCDeviceType::RTC6;

        std::vector<uint32_t> buffer;
        buffer.resize(OIE_MAX_SIGNALCOUNT);
        uint32_t nRTCSignalBufferSize = OIE_MAX_SIGNALCOUNT;
        uint32_t nSensorSignalBufferSize = OIE_MAX_SIGNALCOUNT;

        m_pSDK->checkError(m_pSDK->oie_get_rtc_signals(sTempFileName.c_str(), buffer.data(), &nRTCSignalBufferSize));
        for (uint32_t nIndex = 0; nIndex < nRTCSignalBufferSize; nIndex++)
            m_RTCSignalIDs.push_back(buffer.at (nIndex));

        m_pSDK->checkError(m_pSDK->oie_get_sensor_signals(sTempFileName.c_str(), buffer.data(), &nSensorSignalBufferSize));
        for (uint32_t nIndex = 0; nIndex < nSensorSignalBufferSize; nIndex++)
            m_SensorSignalIDs.push_back(buffer.at(nIndex));

        pWorkingFile = nullptr;
        pWorkingDirectory = nullptr;
    }
}

CDeviceConfiguration::~CDeviceConfiguration()
{

}

LibMCDriver_ScanLabOIE::eRTCDeviceType CDeviceConfiguration::GetDeviceType()
{
    return m_RTCDeviceType;
}

void CDeviceConfiguration::GetRTCSignalIDs(LibMCDriver_ScanLabOIE_uint64 nSignalIDsBufferSize, LibMCDriver_ScanLabOIE_uint64* pSignalIDsNeededCount, LibMCDriver_ScanLabOIE_uint32 * pSignalIDsBuffer)
{
    if (pSignalIDsNeededCount != nullptr)
        *pSignalIDsNeededCount = m_RTCSignalIDs.size();

    if (pSignalIDsBuffer != nullptr) {
        if (nSignalIDsBufferSize < m_RTCSignalIDs.size ())
            throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_BUFFERTOOSMALL);

        for (size_t nIndex = 0; nIndex < m_RTCSignalIDs.size(); nIndex++)
            pSignalIDsBuffer[nIndex] = m_RTCSignalIDs.at (nIndex);

    }

}

void CDeviceConfiguration::GetSensorSignalIDs(LibMCDriver_ScanLabOIE_uint64 nSignalIDsBufferSize, LibMCDriver_ScanLabOIE_uint64* pSignalIDsNeededCount, LibMCDriver_ScanLabOIE_uint32 * pSignalIDsBuffer)
{
    if (pSignalIDsNeededCount != nullptr)
        *pSignalIDsNeededCount = m_SensorSignalIDs.size();

    if (pSignalIDsBuffer != nullptr) {
        if (nSignalIDsBufferSize < m_SensorSignalIDs.size())
            throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_BUFFERTOOSMALL);

        for (size_t nIndex = 0; nIndex < m_SensorSignalIDs.size(); nIndex++)
            pSignalIDsBuffer[nIndex] = m_SensorSignalIDs.at(nIndex);

    }
}

