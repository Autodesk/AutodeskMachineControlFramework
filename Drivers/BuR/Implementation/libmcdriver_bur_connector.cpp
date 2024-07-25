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


*/

#include "libmcdriver_bur_connector.hpp"
#include "libmcdriver_bur_interfaceexception.hpp"
#include "libmcdriver_bur_sockets.hpp"

#include "crcpp/CRC.h"
#include <random>

#include "RapidJSON/rapidjson.h"
#include "RapidJSON/document.h"

using namespace LibMCDriver_BuR::Impl;


#pragma pack(push)
#pragma pack(1)


struct sAMCFToPLCPacketLegacy {
    uint32_t m_nSignature;
    uint8_t m_nMajorVersion;
    uint8_t m_nMinorVersion;
    uint8_t m_nPatchVersion;
    uint8_t m_nBuildVersion;
    uint32_t m_nClientID;
    uint32_t m_nSequenceID;
    uint32_t m_nCommandID;
    sAMCFToPLCPacketPayload m_Payload;
    uint32_t m_nChecksum;
};


struct sPLCToAMCFPacketLegacy {
    uint32_t m_nSignature;
    uint8_t m_nMajorVersion;
    uint8_t m_nMinorVersion;
    uint8_t m_nPatchVersion;
    uint8_t m_nBuildVersion;
    uint32_t m_nClientID;
    uint32_t m_nSequenceID;
    uint32_t m_nErrorCode;
    uint32_t m_nCommandID;
    uint32_t m_nMessageLen;
    uint32_t m_nHeaderChecksum;
    uint32_t m_nDataChecksum;
};


struct sAMCFToPLCPacketVersion3 {
    uint32_t m_nSignature;
    uint32_t m_nClientID;
    uint32_t m_nSequenceID;
    uint32_t m_nCommandID;
    sAMCFToPLCPacketPayload m_Payload;
    uint32_t m_nChecksum;
};


struct sPLCToAMCFPacketVersion3 {
    uint32_t m_nSignature;
    uint32_t m_nClientID;
    uint32_t m_nSequenceID;
    uint32_t m_nErrorCode;
    uint32_t m_nPayloadLength;
    uint32_t m_nHeaderChecksum;
    uint32_t m_nDataChecksum;
};

struct sAMCFToPLCPacketSendInfo {
    uint32_t m_CommandID;
    uint32_t m_SequenceID;
    uint32_t m_ClientID;
    BurPacketCallback m_Callback;
};

#pragma pack(pop)



CDriver_BuRPacket::CDriver_BuRPacket(uint32_t nCommandID, uint32_t nStatusCode)
    : m_nCommandID (nCommandID), m_nStatusCode (nStatusCode)
{
}

CDriver_BuRPacket::~CDriver_BuRPacket()
{

}

uint32_t CDriver_BuRPacket::getCommandID()
{
    return m_nCommandID;
}

uint32_t CDriver_BuRPacket::getStatusCode()
{
    return m_nStatusCode;
}

uint64_t CDriver_BuRPacket::readUInt64(uint32_t nAddress)
{
    if (((size_t)nAddress) + 8 > m_Data.size())
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_INVALIDPARAMETERADDRESS);

    return *((uint64_t*)&m_Data[nAddress]);

}


uint32_t CDriver_BuRPacket::readUInt32(uint32_t nAddress)
{
    if (((size_t)nAddress) + 4 > m_Data.size())
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_INVALIDPARAMETERADDRESS);

    return *((uint32_t*)&m_Data[nAddress]);

}

uint16_t CDriver_BuRPacket::readUInt16(uint32_t nAddress)
{
    if (((size_t)nAddress) + 2 > m_Data.size())
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_INVALIDPARAMETERADDRESS);

    return *((uint16_t*)&m_Data[nAddress]);
}

uint8_t CDriver_BuRPacket::readUInt8(uint32_t nAddress)
{
    if (((size_t)nAddress) + 1 > m_Data.size())
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_INVALIDPARAMETERADDRESS);

    return *((uint8_t*)&m_Data[nAddress]);
}

float CDriver_BuRPacket::readFloat(uint32_t nAddress)
{
    if (((size_t)nAddress) + 4 > m_Data.size())
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_INVALIDPARAMETERADDRESS);

    return *((float*)&m_Data[nAddress]);
}

double CDriver_BuRPacket::readDouble(uint32_t nAddress)
{
    if (((size_t)nAddress) + 8 > m_Data.size())
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_INVALIDPARAMETERADDRESS);

    return *((double*)&m_Data[nAddress]);
}

bool CDriver_BuRPacket::readBool(uint32_t nAddress)
{
    return readUInt8(nAddress);
}

std::string CDriver_BuRPacket::readString(uint32_t nAddress, uint32_t nLength)
{
    if (((size_t)nAddress + (size_t)nLength + 2) > m_Data.size())
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_INVALIDPARAMETERADDRESS);

    uint16_t nActualLength = readUInt16(nAddress);
    if (nActualLength > nLength)
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_STRINGPARAMETEROVERFLOW);
    
    std::string stringValue(reinterpret_cast<const char*>(&m_Data[2]), nActualLength);

    return stringValue;
}

std::vector<uint8_t>& CDriver_BuRPacket::getDataBuffer()
{
    return m_Data;
}


CDriver_BuRConnector::CDriver_BuRConnector(uint32_t nWorkerThreadCount, uint32_t nMaxReceiveBufferSize, uint32_t nMajorVersion, uint32_t nMinorVersion, uint32_t nPatchVersion, uint32_t nBuildVersion, uint32_t nMaxPacketQueueSize, eDriver_BurProtocolVersion ProtocolVersion, uint32_t nPacketSignature)
    : m_nWorkerThreadCount(nWorkerThreadCount),
    m_nMaxReceiveBufferSize(nMaxReceiveBufferSize),
    m_nMajorVersion(nMajorVersion),
    m_nMinorVersion(nMinorVersion),
    m_nPatchVersion(nPatchVersion),
    m_nPacketSignature (nPacketSignature),
    m_nBuildVersion (nBuildVersion),
    m_ProtocolVersion (ProtocolVersion),
    m_StartJournaling (false),
    m_nSequenceID (1),
    m_nMaxPacketQueueSize (nMaxPacketQueueSize)

{
   

}



void CDriver_BuRConnector::queryParameters(BurPacketCallback callback)
{    
    std::shared_ptr<CDriver_BuRSocketConnection> pConnection;
    {
        std::lock_guard<std::mutex> lockGuard(m_ConnectionMutex);
        pConnection = m_pCurrentConnection;
    }

    if (pConnection.get() != nullptr) {        
        sendCommandToPLC(BUR_COMMAND_DIRECT_MACHINESTATUS, callback);
    }
}


void CDriver_BuRConnector::sendCommandsToPLC(std::vector<sAMCFToPLCPacketToSend>& packetList)
{
    switch (m_ProtocolVersion) {
        case eDriver_BurProtocolVersion::Legacy: 
            sendCommandsToPLCLegacy(packetList);
            break;
        case eDriver_BurProtocolVersion::Version3:
            sendCommandsToPLCVersion3(packetList);
            break;
        default:
            throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_UNKNOWNDRIVERPROTOCOLVERSION);
    }

}

void CDriver_BuRConnector::sendCommandsToPLCVersion3(std::vector<sAMCFToPLCPacketToSend>& packetList)
{
    if (packetList.empty())
        return;

    if (m_pCurrentConnection == nullptr)
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_NOTCONNECTED);

    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(1, 1024 * 1024 * 1024);

    std::vector<sAMCFToPLCPacketVersion3> TCPPacketList;
    std::vector<sAMCFToPLCPacketSendInfo> SendInfoList;

    for (auto& packet : packetList) {
        uint32_t nClientID = distribution(generator);

        sAMCFToPLCPacketVersion3 TCPpacket;
        TCPpacket.m_nSignature = m_nPacketSignature;
        TCPpacket.m_nCommandID = packet.m_CommandID;
        TCPpacket.m_nClientID = nClientID;
        TCPpacket.m_nSequenceID = m_nSequenceID;
        TCPpacket.m_Payload = packet.m_Payload;
        TCPpacket.m_nChecksum = CRC::Calculate(&TCPpacket, ((intptr_t)(&TCPpacket.m_nChecksum) - (intptr_t)(&TCPpacket)), CRC::CRC_32());
        TCPPacketList.push_back(TCPpacket);

        // Store send info
        sAMCFToPLCPacketSendInfo sendInfo;
        sendInfo.m_CommandID = TCPpacket.m_nCommandID;
        sendInfo.m_SequenceID = TCPpacket.m_nSequenceID;
        sendInfo.m_ClientID = TCPpacket.m_nClientID;
        sendInfo.m_Callback = packet.m_Callback;
        SendInfoList.push_back(sendInfo);

        m_nSequenceID++;
    }

    if (TCPPacketList.empty())
        throw ELibMCDriver_BuRInterfaceException(LIBMCENV_ERROR_INTERNALERROR);

    int index = 0;
    for (auto& packetToSend : TCPPacketList) {
        auto sendInfo = SendInfoList.at(index);

        if (packetToSend.m_nSequenceID != sendInfo.m_SequenceID)
            throw ELibMCDriver_BuRInterfaceException(LIBMCENV_ERROR_INTERNALERROR);

        m_pCurrentConnection->sendBuffer((uint8_t*)&packetToSend, sizeof(sAMCFToPLCPacketVersion3));
        index++;


    }

    //index = 0;
    for (auto& sendInfo : SendInfoList) {

        //auto sendInfo = SendInfoList.at(index);

        std::vector<uint8_t> recvBuffer;
        m_pCurrentConnection->receiveBuffer(recvBuffer, sizeof(sPLCToAMCFPacketVersion3), true);

        if (recvBuffer.size() != sizeof(sPLCToAMCFPacketVersion3))
            throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_RECEIVEERROR);

        const sPLCToAMCFPacketVersion3* receivedPacket = (const sPLCToAMCFPacketVersion3*)recvBuffer.data();
        if (receivedPacket->m_nSignature != m_nPacketSignature) {
            m_pCurrentConnection->disconnect();
            throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_RECEIVEDINVALIDPACKETSIGNATURE);
        }

        auto pPacket = std::make_shared<CDriver_BuRPacket>(sendInfo.m_CommandID, receivedPacket->m_nErrorCode);

        if (receivedPacket->m_nPayloadLength > BUR_MAX_PAYLOADLENGTH)
            throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_RECEIVEDINVALIDPACKETLENGTH);

        if (receivedPacket->m_nPayloadLength > 0) {
            m_pCurrentConnection->receiveBuffer(pPacket->getDataBuffer(), receivedPacket->m_nPayloadLength, true);
        }

        if (sendInfo.m_SequenceID != receivedPacket->m_nSequenceID)
            throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_INVALIDCLIENTSEQUENCEID);
        if (sendInfo.m_ClientID != receivedPacket->m_nClientID)
            throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_INVALIDCLIENTID);

        if (sendInfo.m_Callback != nullptr) {
            auto callback = sendInfo.m_Callback;
            callback(pPacket.get());
        }

        //index++;
    }

}

void CDriver_BuRConnector::sendCommandsToPLCLegacy(std::vector<sAMCFToPLCPacketToSend>& packetList)
{
    if (packetList.empty())
        return;

    if (m_pCurrentConnection == nullptr)
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_NOTCONNECTED);

    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(1, 1024 * 1024 * 1024);

    std::vector<sAMCFToPLCPacketLegacy> TCPPacketList;
    std::vector<sAMCFToPLCPacketSendInfo> SendInfoList;

    for (auto& packet : packetList) {
        uint32_t nClientID = distribution(generator);

        sAMCFToPLCPacketLegacy TCPpacket;
        TCPpacket.m_nSignature = m_nPacketSignature;
        TCPpacket.m_nCommandID = packet.m_CommandID;
        TCPpacket.m_nMajorVersion = m_nMajorVersion;
        TCPpacket.m_nMinorVersion = m_nMinorVersion;
        TCPpacket.m_nPatchVersion = m_nPatchVersion;
        TCPpacket.m_nBuildVersion = m_nBuildVersion;
        TCPpacket.m_nClientID = nClientID;
        TCPpacket.m_nSequenceID = m_nSequenceID;
        TCPpacket.m_Payload = packet.m_Payload;
        TCPpacket.m_nChecksum = CRC::Calculate(&TCPpacket, ((intptr_t)(&TCPpacket.m_nChecksum) - (intptr_t)(&TCPpacket)), CRC::CRC_32());
        TCPPacketList.push_back(TCPpacket);

        // Store send info
        sAMCFToPLCPacketSendInfo sendInfo;
        sendInfo.m_CommandID = TCPpacket.m_nCommandID;
        sendInfo.m_SequenceID = TCPpacket.m_nSequenceID;
        sendInfo.m_ClientID = TCPpacket.m_nClientID;
        sendInfo.m_Callback = packet.m_Callback;
        SendInfoList.push_back(sendInfo);

        m_nSequenceID++;
    }

    if (TCPPacketList.empty())
        throw ELibMCDriver_BuRInterfaceException(LIBMCENV_ERROR_INTERNALERROR);

    int index = 0;
    for (auto& packetToSend : TCPPacketList) {
        auto sendInfo = SendInfoList.at(index);

        if (packetToSend.m_nSequenceID != sendInfo.m_SequenceID)
            throw ELibMCDriver_BuRInterfaceException(LIBMCENV_ERROR_INTERNALERROR);

        m_pCurrentConnection->sendBuffer((uint8_t*)&packetToSend, sizeof(sAMCFToPLCPacketLegacy));
        index++;

        
    }

    //index = 0;
    for (auto& sendInfo : SendInfoList) {

        //auto sendInfo = SendInfoList.at(index);

        std::vector<uint8_t> recvBuffer;
        m_pCurrentConnection->receiveBuffer(recvBuffer, sizeof(sPLCToAMCFPacketLegacy), true);

        if (recvBuffer.size() != sizeof(sPLCToAMCFPacketLegacy))
            throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_RECEIVEERROR);

        const sPLCToAMCFPacketLegacy* receivedPacket = (const sPLCToAMCFPacketLegacy*)recvBuffer.data();
        if (receivedPacket->m_nSignature != m_nPacketSignature) {
            m_pCurrentConnection->disconnect();
            throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_RECEIVEDINVALIDPACKETSIGNATURE);
        }

        auto pPacket = std::make_shared<CDriver_BuRPacket>(receivedPacket->m_nCommandID, receivedPacket->m_nErrorCode);

        if (receivedPacket->m_nMessageLen < sizeof(sPLCToAMCFPacketLegacy))
            throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_RECEIVEDINVALIDPACKETLENGTH);

        uint32_t nDataLen = (receivedPacket->m_nMessageLen - sizeof(sPLCToAMCFPacketLegacy));
        if (nDataLen > 0) {
            m_pCurrentConnection->receiveBuffer(pPacket->getDataBuffer(), nDataLen, true);
        }

        if (sendInfo.m_SequenceID != receivedPacket->m_nSequenceID)
            throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_INVALIDCLIENTSEQUENCEID);
        if (sendInfo.m_ClientID != receivedPacket->m_nClientID)
            throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_INVALIDCLIENTID);

        if (sendInfo.m_Callback != nullptr) {
            auto callback = sendInfo.m_Callback;
            callback(pPacket.get());
        }

        //index++;
    }
}

sAMCFToPLCPacketToSend CDriver_BuRConnector::makePacket(uint32_t nCommandID, BurPacketCallback callback)
{
    return makePacket(nCommandID, 0, 0, 0, 0, 0, callback);
}

sAMCFToPLCPacketToSend CDriver_BuRConnector::makePacket(uint32_t nCommandID, uint32_t nParameter0, BurPacketCallback callback)
{
    return makePacket(nCommandID, nParameter0, 0, 0, 0, 0, callback);
}

sAMCFToPLCPacketToSend CDriver_BuRConnector::makePacket(uint32_t nCommandID, uint32_t nParameter0, uint32_t nParameter1, BurPacketCallback callback)
{
    return makePacket(nCommandID, nParameter0, nParameter1, 0, 0, 0, callback);
}


sAMCFToPLCPacketToSend CDriver_BuRConnector::makePacket(uint32_t nCommandID, uint32_t nParameter0, uint32_t nParameter1, uint32_t nParameter2, BurPacketCallback callback)
{
    return makePacket(nCommandID, nParameter0, nParameter1, nParameter2, 0, 0, callback);
}

sAMCFToPLCPacketToSend CDriver_BuRConnector::makePacket(uint32_t nCommandID, uint32_t nParameter0, uint32_t nParameter1, uint32_t nParameter2, uint32_t nParameter3, BurPacketCallback callback)
{
    return makePacket(nCommandID, nParameter0, nParameter1, nParameter2, nParameter3, 0, callback);
}

sAMCFToPLCPacketToSend CDriver_BuRConnector::makePacket(uint32_t nCommandID, uint32_t nParameter0, uint32_t nParameter1, uint32_t nParameter2, uint32_t nParameter3, uint32_t nParameter4, BurPacketCallback callback)
{
    sAMCFToPLCPacketToSend packetToSend;
    for (uint32_t nIndex = 0; nIndex < 24; nIndex++)
        packetToSend.m_Payload.m_nData[nIndex] = 0;
    packetToSend.m_CommandID = nCommandID;
    packetToSend.m_Callback = callback;

    uint32_t* pParameter = (uint32_t*)&packetToSend.m_Payload.m_nData[0];
    pParameter[0] = nParameter0;
    pParameter[1] = nParameter1;
    pParameter[2] = nParameter2;
    pParameter[3] = nParameter3;
    pParameter[4] = nParameter4;

    return packetToSend;
}

sAMCFToPLCPacketToSend CDriver_BuRConnector::makePacket(uint32_t nCommandID, sAMCFToPLCPacketPayload payLoad, BurPacketCallback callback)
{
    sAMCFToPLCPacketToSend packetToSend;
    packetToSend.m_CommandID = nCommandID;
    packetToSend.m_Payload = payLoad;
    packetToSend.m_Callback = callback;
    return packetToSend;
}


void CDriver_BuRConnector::sendCommandToPLC(uint32_t nCommandID, BurPacketCallback callback)
{
    sendCommandToPLC(nCommandID, 0, 0, 0, 0, 0, callback);
}

void CDriver_BuRConnector::sendCommandToPLC(uint32_t nCommandID, uint32_t nParameter0, BurPacketCallback callback)
{
    sendCommandToPLC(nCommandID, nParameter0, 0, 0, 0, 0, callback);
}
void CDriver_BuRConnector::sendCommandToPLC(uint32_t nCommandID, uint32_t nParameter0, uint32_t nParameter1, BurPacketCallback callback)
{
    sendCommandToPLC(nCommandID, nParameter0, nParameter1, 0, 0, 0, callback);
}

void CDriver_BuRConnector::sendCommandToPLC(uint32_t nCommandID, uint32_t nParameter0, uint32_t nParameter1, uint32_t nParameter2, BurPacketCallback callback)
{
    sendCommandToPLC(nCommandID, nParameter0, nParameter1, nParameter2, 0, 0, callback);
}

void CDriver_BuRConnector::sendCommandToPLC(uint32_t nCommandID, uint32_t nParameter0, uint32_t nParameter1, uint32_t nParameter2, uint32_t nParameter3, BurPacketCallback callback)
{
    sendCommandToPLC(nCommandID, nParameter0, nParameter1, nParameter2, nParameter3, 0, callback);
}

void CDriver_BuRConnector::sendCommandToPLC(uint32_t nCommandID, uint32_t nParameter0, uint32_t nParameter1, uint32_t nParameter2, uint32_t nParameter3, uint32_t nParameter4, BurPacketCallback callback)
{
    std::vector<sAMCFToPLCPacketToSend> packetList;
    packetList.push_back(makePacket(nCommandID, nParameter0, nParameter1, nParameter2, nParameter3, nParameter4, callback));
    sendCommandsToPLC(packetList);
}

void CDriver_BuRConnector::sendCommandToPLC(uint32_t nCommandID, sAMCFToPLCPacketPayload payLoad, BurPacketCallback callback)
{
    std::vector<sAMCFToPLCPacketToSend> packetList;
    packetList.push_back(makePacket(nCommandID, payLoad, callback));
    sendCommandsToPLC(packetList);
}


/*uint32_t CDriver_BuRConnector::sendSimpleCommandToPLC(uint32_t nCommandID, uint64_t nTimeStamp, BurSendCallback pCallback, uint32_t nParameter0, uint32_t nParameter1, uint32_t nParameter2)
{
    sAMCFToPLCPacketPayload payLoad;
    for (uint32_t nIndex = 0; nIndex < 24; nIndex++)
        payLoad.m_nData[nIndex] = 0;

    uint32_t* pData = (uint32_t*)&payLoad;
    pData[0] = nParameter0;
    pData[1] = nParameter1;
    pData[2] = nParameter2;

    return sendCommandToPLC(nCommandID, payLoad, nTimeStamp, pCallback);

} */


/*uint32_t CDriver_BuRConnector::sendCommandToPLC(uint32_t nCommandID, sAMCFToPLCPacketPayload payLoad, uint64_t nTimeStamp, BurSendCallback pCallback)
{

    if (m_SentPacketQueue.size () >= m_nMaxPacketQueueSize)
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_PACKETQUEUEOVERFLOW);

    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(1, 1024 * 1024 * 1024);
    uint32_t nClientID = distribution(generator); 

    sAMCFToPLCPacket packetToSend;

    packetToSend.m_nSignature = PACKET_SIGNATURE;
    packetToSend.m_nCommandID = nCommandID;
    packetToSend.m_nMajorVersion = m_nMajorVersion;
    packetToSend.m_nMinorVersion = m_nMinorVersion;
    packetToSend.m_nPatchVersion = m_nPatchVersion;
    packetToSend.m_nBuildVersion = m_nBuildVersion;
    packetToSend.m_nClientID = nClientID;
    packetToSend.m_nSequenceID = m_nSequenceID;
    packetToSend.m_Payload = payLoad;
    packetToSend.m_nChecksum = CRC::Calculate(&packetToSend, ((intptr_t)(&packetToSend.m_nChecksum) - (intptr_t)(&packetToSend)), CRC::CRC_32());

    auto pSendInfo = std::make_shared<CDriver_BuRSendInfo>(nCommandID, m_nSequenceID, nClientID, nTimeStamp, pCallback);
    {        
        m_PacketsToSend.push(packetToSend);

        std::lock_guard<std::mutex> lockGuard(m_SequenceMapMutex);
        m_SentPacketQueue.insert(std::make_pair(pSendInfo->getSequenceID(), pSendInfo));
        m_nSequenceID++;
    } 


    handlePacket();

    return pSendInfo->getSequenceID(); 

} */







/*void CDriver_BuRConnector::enterCallback(const std::shared_ptr <brynet::net::TcpConnection> session, const std::string& sIPAddress, const uint32_t nPort)
{

    std::lock_guard<std::mutex> lockGuard(m_ConnectionMutex);

    if (m_pCurrentConnection.get() != nullptr) {        
        m_pCurrentConnection->postDisConnect();
        m_pCurrentConnection = nullptr;
    }


    session->setDataCallback([session, this](brynet::base::BasePacketReader& reader) 
    {
        auto nLen = reader.size();
        if (nLen >= sizeof(sPLCToAMCFPacket)) {
            const uint8_t * pBuffer = (const uint8_t*) reader.currentBuffer();
            const sPLCToAMCFPacket * receivedPacket = (const sPLCToAMCFPacket *) pBuffer; 
            if (receivedPacket->m_nSignature != PACKET_SIGNATURE) {
                session->postDisConnect();
                throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_RECEIVEDINVALIDPACKETSIGNATURE);
            }
            

            if (nLen >= receivedPacket->m_nMessageLen) {

                const uint8_t * pData = nullptr;
                uint32_t nDataLen = receivedPacket->m_nMessageLen - sizeof(sPLCToAMCFPacket);
                if (nDataLen > 0)
                    pData = pBuffer + sizeof(sPLCToAMCFPacket);

                auto pPacket = std::make_shared<CDriver_BuRPacket>(receivedPacket->m_nCommandID, receivedPacket->m_nErrorCode, nDataLen, pData);
                handlePacket (pPacket, receivedPacket->m_nSequenceID, receivedPacket->m_nClientID);

                reader.addPos(receivedPacket->m_nMessageLen);
                reader.savePos();
            }

        }

    });

    m_pCurrentConnection = session;
    
} */

/*void CDriver_BuRConnector::handlePacket()
{
    if (m_pCurrentConnection.get() == nullptr)
        return;

    while (!m_PacketsToSend.empty()) {
        auto& packetToSend = m_PacketsToSend.front();
        m_PacketsToSend.pop();

        m_pCurrentConnection->sendBuffer((uint8_t*)&packetToSend, sizeof(packetToSend));
        receiveCommandFromPLCEx(m_pCurrentConnection.get());
    }

} */

void CDriver_BuRConnector::connect(const std::string& sIPAddress, const uint32_t nPort, const uint32_t nTimeout)
{
    {
        std::lock_guard<std::mutex> lockGuard(m_ConnectionMutex);
        m_pCurrentConnection = nullptr;
    }

    CDriver_BuRSocketConnection::initializeNetworking();
    auto pConnection = std::make_shared <CDriver_BuRSocketConnection>(sIPAddress, nPort);

    {
        std::lock_guard<std::mutex> lockGuard(m_ConnectionMutex);
        m_pCurrentConnection = pConnection;
    }

    retrieveJournalSchema();
    
/*    std::thread connectionThread([this, sIPAddress, nPort] {

        try {
            

            while (pConnection->isConnected()) {

                while (!m_PacketsToSend.empty()) {
                    auto& packetToSend = m_PacketsToSend.front();
                    m_PacketsToSend.pop();

                    pConnection->sendBuffer((uint8_t*)&packetToSend, sizeof(packetToSend));
                    receiveCommandFromPLCEx(pConnection.get());
                }
                    
            }

        }
        catch (...) {

        }

    }); 

    connectionThread.detach(); */

}

void CDriver_BuRConnector::disconnect()
{
    std::lock_guard<std::mutex> lockGuard(m_ConnectionMutex);

    if (m_pCurrentConnection.get() != nullptr) {
        m_pCurrentConnection->disconnect();
        m_pCurrentConnection = nullptr;
    } 
}

void CDriver_BuRConnector::refreshJournal()
{

}

void CDriver_BuRConnector::retrieveJournalSchema()
{
    if (m_ProtocolVersion != eDriver_BurProtocolVersion::Version3)
        return;

    bool bSuccessfulParsedJournalSchema = false;

    sendCommandToPLC(BUR_COMMAND_DIRECT_CURRENTJOURNALSCHEMA, [&bSuccessfulParsedJournalSchema](CDriver_BuRPacket* pPacket) {

            auto schemaBuffer = pPacket->getDataBuffer();

            std::string schemaString(schemaBuffer.begin(), schemaBuffer.end());

            rapidjson::Document document;
            document.Parse(schemaString.c_str());

            if (!document.IsObject())
                throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_COULDNOTPARSEBURSCHEMAJSON);

            if (!document.HasMember("schema"))
                throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALSCHEMAHASNOVERSIONIDENTIFIER);

            if (!document["schema"].IsString ())
                throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALSCHEMAHASINVALIDVERSIONIDENTIFIER);

            std::string sSchemaVersion = document["schema"].GetString();

            if (!document.HasMember("groups"))
                throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALSCHEMAHASNOGROUPINFORMATION);

            if (!document["groups"].IsArray ())
                throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALSCHEMAHASINVALIDGROUPINFORMATION);

            auto groupArray = document["groups"].GetArray();
            for (auto groupIter = groupArray.Begin(); groupIter != groupArray.End(); groupIter++) {

                auto& group = *groupIter;
                if (!group.HasMember ("groupname"))
                    throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALSCHEMAMISSESGROUPNAME);
                if (!group.HasMember("groupid"))
                    throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALSCHEMAMISSESGROUPID);
                if (!group.HasMember("values"))
                    throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALSCHEMAMISSESGROUPVALUES);

                auto& groupNameMember = group["groupname"];
                auto& groupIDMember = group["groupid"];
                auto& groupValuesMember = group["values"];

                if (!(groupNameMember.IsString()))
                    throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALSCHEMAHASINVALIDGROUPNAME);
                if (!(groupIDMember.IsInt()))
                    throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALSCHEMAHASINVALIDGROUPID);
                if (!(groupValuesMember.IsArray()))
                    throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALSCHEMAHASINVALIDGROUPVALUES);

                std::string sGroupName = groupNameMember.GetString();
                int32_t nGroupID = groupIDMember.GetInt ();

                auto groupValues = groupValuesMember.GetArray();
                for (auto valueIter = groupValues.Begin(); valueIter != groupValues.End(); valueIter++) {
                    auto& value = *valueIter;
                    if (!value.HasMember("type"))
                        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALSCHEMAMISSESVALUETYPE);
                    if (!value.HasMember("name"))
                        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALSCHEMAMISSESVALUENAME);
                    if (!value.HasMember("id"))
                        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALSCHEMAMISSESVALUEID);
                    if (!value.HasMember("size"))
                        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALSCHEMAMISSESVALUESIZE);

                    auto& valueTypeMember = value["type"];
                    auto& valueNameMember = value["name"];
                    auto& valueIDMember = value["id"];
                    auto& valueSizeMember = value["size"];

                    if (!(valueTypeMember.IsString()))
                        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALSCHEMAHASINVALIDVALUETYPE);
                    if (!(valueNameMember.IsString()))
                        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALSCHEMAHASINVALIDVALUENAME);
                    if (!(valueIDMember.IsInt()))
                        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALSCHEMAHASINVALIDVALUEID);
                    if (!(valueSizeMember.IsInt()))
                        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_JOURNALSCHEMAHASINVALIDVALUESIZE);
                }

            }


            bSuccessfulParsedJournalSchema = true;

        });

    if (!bSuccessfulParsedJournalSchema)
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_COULDNOTPARSEJOURNALSCHEMA);

}

