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


Abstract: This is a stub class definition of CChannel

*/

#include "libmcdriver_tml_types.hpp"
#include "libmcdriver_tml_instance.hpp"
#include "libmcdriver_tml_interfaceexception.hpp"

// Include custom headers here.
#define TML_MAXIDENTIFIERLENGTH 64


#define TML_PROTOCOL_TMLCAN	0x00
#define TML_PROTOCOL_TECHNOCAN 0x80

#define TML_CHANNEL_RS232 0
#define TML_CHANNEL_RS485 1
#define TML_CHANNEL_IXXAT_CAN 2
#define TML_CHANNEL_SYS_TEC_USBCAN 3
#define TML_CHANNEL_PEAK_SYS_PCAN_PCI 4
#define TML_CHANNEL_ESD_CAN 5
#define TML_CHANNEL_PEAK_SYS_PCAN_ISA 6
#define TML_CHANNEL_PEAK_SYS_PCAN_USB 7
#define TML_CHANNEL_PEAK_SYS_PCAN_DONGLE 8
#define TML_CHANNEL_LAWICEL_USBCAN 9
#define TML_CHANNEL_KVASER_USBCAN 10
#define TML_CHANNEL_SOCKET_CAN 11
#define TML_CHANNEL_VIRTUAL_SERIAL 15
#define TML_CHANNEL_XPORT_IP 16
#define TML_CHANNEL_ETHERNET_CAN 17


using namespace LibMCDriver_TML::Impl;


CTMLAxisInstance::CTMLAxisInstance(const std::string& sChannelIdentifier, const std::string& sAxisIdentifier, uint8_t nHardwareID)
    :  m_sChannelIdentifier (sChannelIdentifier), m_sAxisIdentifier(sAxisIdentifier), m_nHardwareID (nHardwareID)
{

}

CTMLAxisInstance::~CTMLAxisInstance()
{

}

std::string CTMLAxisInstance::getAxisIdentifier()
{
    return m_sAxisIdentifier;
}

std::string CTMLAxisInstance::getChannelIdentifier()
{
    return m_sChannelIdentifier;
}

uint8_t CTMLAxisInstance::getHardwareID()
{
    return m_nHardwareID;
}


CTMLInstance::CTMLInstance(PTMLSDK pTMLSDK, LibMCEnv::PWorkingDirectory pWorkingDirectory)
    : m_pTMLSDK (pTMLSDK), m_pWorkingDirectory (pWorkingDirectory)
{
    if (pTMLSDK.get() == nullptr)
        throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_INVALIDPARAM);
    if (pWorkingDirectory.get() == nullptr)
        throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_INVALIDPARAM);
}

CTMLInstance::~CTMLInstance()
{

}

void CTMLInstance::openChannel(const std::string& sIdentifier, const std::string& sDeviceName, const LibMCDriver_TML::eChannelType eChannelTypeToUse, const LibMCDriver_TML::eProtocolType eProtocolTypeToUse, const LibMCDriver_TML_uint32 nHostID, const LibMCDriver_TML_uint32 nBaudrate)
{
    if (!checkIdentifierString (sIdentifier))
        throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_INVALIDCHANNELIDENTIFIER, "invalid channel identifier: " + sIdentifier);

    if (channelExists (sIdentifier))
        throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_CHANNELALREADYEXISTS, "channel already exists: " + sIdentifier);

    if (sDeviceName.empty ())
        throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_EMPTYDEVICENAME);

    uint8_t nType = 0;

    switch (eProtocolTypeToUse) {
        case LibMCDriver_TML::eProtocolType::TechnoCAN:
            nType |= TML_PROTOCOL_TECHNOCAN;
            break;

        case LibMCDriver_TML::eProtocolType::TMLCAN:
            nType |= TML_PROTOCOL_TMLCAN;
            break;

        default:
            throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_UNSUPPORTEDPROTOCOLTYPE);

    };

    switch (eChannelTypeToUse) {
    case LibMCDriver_TML::eChannelType::RS232:
        nType |= TML_CHANNEL_RS232;
        break;
    case LibMCDriver_TML::eChannelType::RS485:
        nType |= TML_CHANNEL_RS485;
        break;
    case LibMCDriver_TML::eChannelType::IXXAT_CAN:
        nType |= TML_CHANNEL_IXXAT_CAN;
        break;
    case LibMCDriver_TML::eChannelType::SysTecUSBCAN:
        nType |= TML_CHANNEL_SYS_TEC_USBCAN;
        break;

    default:
        throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_UNSUPPORTEDCHANNELTYPE);

    };

    if (nHostID > 255)
        throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_INVALIDHOSTID);

    tmlInt fileDescriptor = m_pTMLSDK->TS_OpenChannel(sDeviceName.c_str(), nType, (uint8_t)nHostID, nBaudrate);

    if (fileDescriptor < 0) 
        m_pTMLSDK->raiseLastError();

    m_ChannelFileDescriptorMap.insert (std::make_pair (sIdentifier, fileDescriptor));

}

void CTMLInstance::closeChannel(const std::string& sIdentifier)
{
    if (!checkIdentifierString(sIdentifier))
        throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_INVALIDCHANNELIDENTIFIER, "invalid channel identifier: " + sIdentifier);

    m_ChannelFileDescriptorMap.erase(sIdentifier);

}

bool CTMLInstance::channelExists(const std::string& sIdentifier)
{
    auto iIter = m_ChannelFileDescriptorMap.find(sIdentifier);
    return (iIter != m_ChannelFileDescriptorMap.end());
}

bool CTMLInstance::checkIdentifierString(const std::string& sIdentifier)
{
    if (sIdentifier.empty())
        return false;

    if (sIdentifier.size() > TML_MAXIDENTIFIERLENGTH)
        return false;

    for (char ch : sIdentifier)
        if (!(((ch >= '0') && (ch <= '9')) || ((ch >= 'a') && (ch <= 'z')) || ((ch >= 'A') && (ch <= 'Z')) || (ch == '_')))
            return false;

    return true;
}

bool CTMLInstance::axisExistsInChannel(const std::string& sChannelIdentifier, const std::string& sAxisIdentifier)
{
    if (!checkIdentifierString(sChannelIdentifier))
        throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_INVALIDCHANNELIDENTIFIER, "invalid channel identifier: " + sChannelIdentifier);
    if (!checkIdentifierString(sAxisIdentifier))
        throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_INVALIDAXISIDENTIFIER, "invalid axis identifier: " + sAxisIdentifier);

    auto iIter = m_AxisMap.find(sAxisIdentifier);
    if (iIter != m_AxisMap.end()) {
        return (iIter->second.getChannelIdentifier() == sChannelIdentifier);
    }
    else {
        return false;
    }

}

bool CTMLInstance::axisExists(const std::string& sAxisIdentifier)
{
    if (!checkIdentifierString(sAxisIdentifier))
        throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_INVALIDAXISIDENTIFIER, "invalid axis identifier: " + sAxisIdentifier);

    auto iIter = m_AxisMap.find(sAxisIdentifier);
    return (iIter != m_AxisMap.end());
}


void CTMLInstance::setupAxis(const std::string& sChannelIdentifier, const std::string& sAxisIdentifier, uint32_t nAxisID, size_t nConfigurationBufferSize, const uint8_t* pConfigurationBuffer)
{
    if (!checkIdentifierString(sChannelIdentifier))
        throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_INVALIDCHANNELIDENTIFIER, "invalid channel identifier: " + sChannelIdentifier);
    if (!checkIdentifierString(sAxisIdentifier))
        throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_INVALIDAXISIDENTIFIER, "invalid axis identifier: " + sAxisIdentifier);

    if (nAxisID > 255)
        throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_INVALIDAXISHARDWAREID, "invalid axis hardware ID: " + std::to_string (nAxisID));


    auto iIter = m_AxisMap.find(sAxisIdentifier);
    if (iIter != m_AxisMap.end())
        throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_AXISALREADYEXISTS, "axis already exists: " + sAxisIdentifier);

    if (nConfigurationBufferSize == 0)
        throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_INVALIDAXISCONFIGURATIONBUFFER);

    auto pTempFile = m_pWorkingDirectory->StoreCustomDataInTempFile("zip", LibMCEnv::CInputVector<uint8_t>(pConfigurationBuffer, nConfigurationBufferSize));

    tmlInt nSetupID = m_pTMLSDK->TS_LoadSetup(pTempFile->GetAbsoluteFileName().c_str());
    if (nSetupID < 0)
        m_pTMLSDK->raiseLastError();

    m_pTMLSDK->checkError(m_pTMLSDK->TS_SetupAxis((uint8_t)nAxisID, nSetupID));

    m_AxisMap.insert(std::make_pair(sAxisIdentifier, CTMLAxisInstance(sChannelIdentifier, sAxisIdentifier, (uint8_t)nAxisID)));

    selectAxisInternal(sAxisIdentifier);

    m_pTMLSDK->checkError(m_pTMLSDK->TS_DriveInitialisation());


}

void CTMLInstance::selectAxisInternal(const std::string& sAxisIdentifier)
{
    if (!checkIdentifierString(sAxisIdentifier))
        throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_INVALIDAXISIDENTIFIER, "invalid axis identifier: " + sAxisIdentifier);

    auto iIter = m_AxisMap.find(sAxisIdentifier);
    if (iIter == m_AxisMap.end())
        throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_AXISNOTFOUND, "axis not found: " + sAxisIdentifier);

    m_pTMLSDK->checkError(m_pTMLSDK->TS_SelectAxis(iIter->second.getHardwareID ()));
}



