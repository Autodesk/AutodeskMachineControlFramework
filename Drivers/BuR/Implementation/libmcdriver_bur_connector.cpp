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

using namespace LibMCDriver_BuR::Impl;

#define PACKET_SIGNATURE 0xAB



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

CDriver_BuRSendInfo::CDriver_BuRSendInfo(uint32_t nCommandID, uint32_t nSequenceID, uint32_t nClientID, uint64_t nTimeStamp, BurSendCallback sendCallback)
    : m_nCommandID (nCommandID), m_nSequenceID (nSequenceID), m_nClientID (nClientID), m_nTimeStamp (nTimeStamp), m_Callback (sendCallback)
{

}

CDriver_BuRSendInfo::~CDriver_BuRSendInfo()
{

}

uint32_t CDriver_BuRSendInfo::getCommandID()
{
    return m_nCommandID;
}

uint32_t CDriver_BuRSendInfo::getSequenceID()
{
    return m_nSequenceID;
}

uint32_t CDriver_BuRSendInfo::getClientID()
{
    return m_nClientID;
}

uint64_t CDriver_BuRSendInfo::getTimeStamp()
{
    return m_nTimeStamp;
}

void CDriver_BuRSendInfo::resetCallback()
{
    m_Callback = nullptr;
}

void CDriver_BuRSendInfo::triggerCallback(CDriver_BuRPacket* pPacket)
{
    if (pPacket == nullptr)
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_INVALIDPARAM);
    if (m_Callback)
        m_Callback(this, pPacket);
}


CDriver_BuRConnector::CDriver_BuRConnector(uint32_t nWorkerThreadCount, uint32_t nMaxReceiveBufferSize, uint32_t nMajorVersion, uint32_t nMinorVersion, uint32_t nPatchVersion, uint32_t nBuildVersion, uint32_t nMaxPacketQueueSize)
    : m_nWorkerThreadCount(nWorkerThreadCount),
    m_nMaxReceiveBufferSize(nMaxReceiveBufferSize),
    m_nMajorVersion(nMajorVersion),
    m_nMinorVersion(nMinorVersion),
    m_nPatchVersion(nPatchVersion),
    m_nBuildVersion (nBuildVersion),
    m_nSequenceID (1),
    m_nMaxPacketQueueSize (nMaxPacketQueueSize)

{
   

}





void CDriver_BuRConnector::queryParameters(uint64_t nTimeStamp, BurSendCallback pCallback)
{    
    std::shared_ptr<CDriver_BuRSocketConnection> pConnection;
    {
        std::lock_guard<std::mutex> lockGuard(m_ConnectionMutex);
        pConnection = m_pCurrentConnection;
    }

    if (pConnection.get() != nullptr) {
        sAMCFToPLCPacketPayload payLoad;
        for (uint32_t nIndex = 0; nIndex < 24; nIndex++)
            payLoad.m_nData[nIndex] = 0;

        sendCommandToPLC(BUR_COMMAND_DIRECT_MACHINESTATUS, payLoad, nTimeStamp, pCallback);

        
            
    }

}



uint32_t CDriver_BuRConnector::sendSimpleCommandToPLC(uint32_t nCommandID, uint64_t nTimeStamp, BurSendCallback pCallback, uint32_t nParameter0, uint32_t nParameter1, uint32_t nParameter2)
{
    sAMCFToPLCPacketPayload payLoad;
    for (uint32_t nIndex = 0; nIndex < 24; nIndex++)
        payLoad.m_nData[nIndex] = 0;

    uint32_t* pData = (uint32_t*)&payLoad;
    pData[0] = nParameter0;
    pData[1] = nParameter1;
    pData[2] = nParameter2;

    return sendCommandToPLC(nCommandID, payLoad, nTimeStamp, pCallback);

}


uint32_t CDriver_BuRConnector::sendCommandToPLC(uint32_t nCommandID, sAMCFToPLCPacketPayload payLoad, uint64_t nTimeStamp, BurSendCallback pCallback)
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

    return pSendInfo->getSequenceID(); 

}



PDriver_BuRPacket CDriver_BuRConnector::receiveCommandFromPLCEx(CDriver_BuRSocketConnection* pConnection)
{
    if (pConnection == nullptr)
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_INVALIDTCPCONNECTIONPOINTER);

    std::vector<uint8_t> Buffer;
    pConnection->receiveBuffer(Buffer, sizeof(sPLCToAMCFPacket), true);

    if (Buffer.size () != sizeof(sPLCToAMCFPacket))
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_RECEIVEERROR);

    const sPLCToAMCFPacket* receivedPacket = (const sPLCToAMCFPacket*)Buffer.data ();
    if (receivedPacket->m_nSignature != PACKET_SIGNATURE) {
        pConnection->disconnect();
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_RECEIVEDINVALIDPACKETSIGNATURE);   
    }

    auto pPacket = std::make_shared<CDriver_BuRPacket>(receivedPacket->m_nCommandID, receivedPacket->m_nErrorCode);

    if (receivedPacket->m_nMessageLen < sizeof(sPLCToAMCFPacket))
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_RECEIVEDINVALIDPACKETLENGTH);

    uint32_t nDataLen = (receivedPacket->m_nMessageLen - sizeof(sPLCToAMCFPacket));
    if (nDataLen > 0) {
        pConnection->receiveBuffer(pPacket->getDataBuffer(), nDataLen, true);
    }
    uint32_t nSequenceID = receivedPacket->m_nSequenceID;
    uint32_t nClientID = receivedPacket->m_nClientID;

    PDriver_BuRSendInfo pSendInfo;
    {
        std::lock_guard<std::mutex> lockGuard(m_SequenceMapMutex);
        auto iIter = m_SentPacketQueue.find(nSequenceID);
        if (iIter != m_SentPacketQueue.end()) {
            pSendInfo = iIter->second;
            m_SentPacketQueue.erase(nSequenceID);
        }
    }

    if (pSendInfo.get() == nullptr)
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_INVALIDCLIENTSEQUENCEID);
    if (pSendInfo->getSequenceID() != nSequenceID)
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_INVALIDCLIENTSEQUENCEID);
    if (pSendInfo->getClientID() != nClientID)
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_INVALIDCLIENTID);

    pSendInfo->triggerCallback(pPacket.get()); 

    return pPacket;
}



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



void CDriver_BuRConnector::connect(const std::string& sIPAddress, const uint32_t nPort, const uint32_t nTimeout)
{
    {
        std::lock_guard<std::mutex> lockGuard(m_ConnectionMutex);
        m_pCurrentConnection = nullptr;
    }

    CDriver_BuRSocketConnection::initializeNetworking();
    
    std::thread connectionThread([this, sIPAddress, nPort] {

        try {
            auto pConnection = std::make_shared <CDriver_BuRSocketConnection>(sIPAddress, nPort);

            {
                std::lock_guard<std::mutex> lockGuard(m_ConnectionMutex);
                m_pCurrentConnection = pConnection;
            }

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

    connectionThread.detach();

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

void CDriver_BuRConnector::unregisterSendCallback(uint32_t nSequenceID)
{
    std::lock_guard<std::mutex> lockGuard(m_ConnectionMutex);
    auto iIter = m_SentPacketQueue.find(nSequenceID);
    if (iIter != m_SentPacketQueue.end())
        iIter->second->resetCallback();

}
