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


Abstract: This is a stub class definition of CDriver_BuR

*/
// Include custom headers here.
#include <iostream>
#include <brynet/base/AppStatus.hpp>
#include <brynet/net/AsyncConnector.hpp>
#include <brynet/net/TcpService.hpp>
#include <brynet/net/wrapper/ConnectionBuilder.hpp>

// Include custom headers here.
#include "pugixml.hpp"

#include "libmcdriver_bur_driver_bur.hpp"
#include "libmcdriver_bur_interfaceexception.hpp"
#include "crcpp/CRC.h"
#include <random>

using namespace LibMCDriver_BuR::Impl;


#define PACKET_SIGNATURE 0xAB

#define COMMAND_DIRECT_INIT 100
#define COMMAND_DIRECT_BEGINLIST 101
#define COMMAND_DIRECT_FINISHLIST 102
#define COMMAND_DIRECT_EXECUTELIST 103
#define COMMAND_DIRECT_PAUSELIST 104
#define COMMAND_DIRECT_LISTSTATUS 105
#define COMMAND_DIRECT_ABORTLIST 106
#define COMMAND_DIRECT_RESUMELIST 107
#define COMMAND_DIRECT_MACHINESTATUS 108
#define COMMAND_DIRECT_JOURNALSTART 109
#define COMMAND_DIRECT_JOURNALSTOP 110
#define COMMAND_DIRECT_JOURNALRETRIEVE 111




CDriver_BuRValue::CDriver_BuRValue(const std::string& sName, const uint32_t nAddress)
    : m_sName(sName), m_nAddress(nAddress)
{

}

CDriver_BuRValue::~CDriver_BuRValue()
{

}

std::string CDriver_BuRValue::getName()
{
    return m_sName;
}

uint32_t CDriver_BuRValue::getAddress()
{
    return m_nAddress;
}

CDriver_BuRStringValue::CDriver_BuRStringValue(const std::string& sName, const uint32_t nAddress, const uint32_t nLength)
    : CDriver_BuRValue(sName, nAddress), m_nLength (nLength)
{

}

ePLCFieldType CDriver_BuRStringValue::getFieldType()
{
    return ePLCFieldType::ftString;
}

uint32_t CDriver_BuRStringValue::getLength()
{
    return m_nLength;
}


CDriver_BuRRealValue::CDriver_BuRRealValue(const std::string& sName, const uint32_t nAddress)
    : CDriver_BuRValue(sName, nAddress)
{

}

ePLCFieldType CDriver_BuRRealValue::getFieldType()
{
    return ePLCFieldType::ftReal;
}


CDriver_BuRLRealValue::CDriver_BuRLRealValue(const std::string& sName, const uint32_t nAddress)
    : CDriver_BuRValue(sName, nAddress)
{

}

ePLCFieldType CDriver_BuRLRealValue::getFieldType()
{
    return ePLCFieldType::ftReal;
}


CDriver_BuRDIntValue::CDriver_BuRDIntValue(const std::string& sName, const uint32_t nAddress)
    : CDriver_BuRValue(sName, nAddress)
{

}

ePLCFieldType CDriver_BuRDIntValue::getFieldType()
{
    return ePLCFieldType::ftDInt;
}



CDriver_BuRIntValue::CDriver_BuRIntValue(const std::string& sName, const uint32_t nAddress)
    : CDriver_BuRValue(sName, nAddress)
{

}


ePLCFieldType CDriver_BuRIntValue::getFieldType()
{
    return ePLCFieldType::ftInt;
}


CDriver_BuRBoolValue::CDriver_BuRBoolValue(const std::string& sName, const uint32_t nAddress)
    : CDriver_BuRValue(sName, nAddress)
{
}

ePLCFieldType CDriver_BuRBoolValue::getFieldType()
{
    return ePLCFieldType::ftBool;
}


CDriver_BuRCommandParameter::CDriver_BuRCommandParameter(const std::string& sName, const std::string& sDescription, const std::string& sField)
    : m_sName(sName), m_sDescription(sDescription), m_sField(sField)
{

}

std::string CDriver_BuRCommandParameter::getName()
{
    return m_sName;
}

std::string CDriver_BuRCommandParameter::getDescription()
{
    return m_sDescription;
}

std::string CDriver_BuRCommandParameter::getField()
{
    return m_sField;
}


CDriver_BuRPacket::CDriver_BuRPacket(uint32_t nCommandID, uint32_t nStatusCode, uint32_t nDataLen, const uint8_t* pData)
    : m_nCommandID (nCommandID), m_nStatusCode (nStatusCode)
{
    m_Data.resize(nDataLen);

    if (pData != nullptr) {
        for (uint32_t nIndex = 0; nIndex < nDataLen; nIndex++)
            m_Data[nIndex] = pData[nIndex];
    }

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



CDriver_BuRSendInfo::CDriver_BuRSendInfo(uint32_t nCommandID, uint32_t nSequenceID, uint32_t nClientID, uint64_t nTimeStamp)
    : m_nCommandID (nCommandID), m_nSequenceID (nSequenceID), m_nClientID (nClientID), m_nTimeStamp (nTimeStamp)
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


/*************************************************************************************************************************
 Class definition of CDriver_BuR 
**************************************************************************************************************************/
#define __STRINGIZE(x) #x
#define __STRINGIZE_VALUE_OF(x) __STRINGIZE(x)

CDriver_BuR::CDriver_BuR(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment)
    : m_sName(sName), m_pDriverEnvironment (pDriverEnvironment), m_nWorkerThreadCount (1), m_nMaxReceiveBufferSize (1024*1024),
       m_nMajorVersion (0), m_nMinorVersion (0), m_nPatchVersion (0), m_nBuildVersion (0), m_nSequenceID (1), m_nMaxPacketQueueSize(1024)

{  
    if (pDriverEnvironment.get() == nullptr)
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_INVALIDPARAM);

    std::string sGitHash = __STRINGIZE_VALUE_OF(__GITHASH);
    m_nBuildVersion = CRC::Calculate(sGitHash.c_str(), sGitHash.length(), CRC::CRC_32());    

}



void CDriver_BuR::Configure(const std::string& sConfigurationString)
{

    if (sConfigurationString.length() == 0)
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_INVALIDDRIVERPROTOCOL);

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(sConfigurationString.c_str());
    if (!result)
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_COULDNOTPARSEDRIVERPROTOCOL);

    pugi::xml_node burprotocolNode = doc.child("burprotocol");
    if (burprotocolNode.empty())
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_INVALIDDRIVERPROTOCOL);

    pugi::xml_node versionNode = burprotocolNode.child("version");
    if (versionNode.empty())
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_NOVERSIONDEFINITION);

    pugi::xml_attribute majorversionAttrib = versionNode.attribute("major");
    if (majorversionAttrib.empty())
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_NOMAJORVERSION);
    m_nMajorVersion = majorversionAttrib.as_uint(0);

    pugi::xml_attribute minorversionAttrib = versionNode.attribute("minor");
    if (minorversionAttrib.empty())
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_NOMINORVERSION);
    m_nMinorVersion = minorversionAttrib.as_uint(0);

    pugi::xml_attribute patchversionAttrib = versionNode.attribute("patch");
    if (patchversionAttrib.empty())
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_NOPATCHVERSION);
    m_nPatchVersion = patchversionAttrib.as_uint(0);

    pugi::xml_node machineStatusNode = burprotocolNode.child("machinestatus");
    if (machineStatusNode.empty())
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_NOSTATUSDBDEFINITION);

    auto statusNodes = machineStatusNode.children();
    for (pugi::xml_node childNode : statusNodes)
    {
        std::string sChildName = childNode.name();
        if ((sChildName == "bool") || (sChildName == "real") || (sChildName == "lreal") || (sChildName == "dint") || (sChildName == "int")) {

            auto nameAttrib = childNode.attribute("name");
            auto addressAttrib = childNode.attribute("address");
            auto descriptionAttrib = childNode.attribute("description");

            if (nameAttrib.empty())
                throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_NONAMEATTRIBUTE);
            if (addressAttrib.empty())
                throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_NOADDRESSEATTRIBUTE);
            if (descriptionAttrib.empty())
                throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_NODESCRIPTIONATTRIBUTE);

            std::string sName = nameAttrib.as_string();
            uint32_t nAddress = addressAttrib.as_uint(1UL << 30);
            std::string sDescription = descriptionAttrib.as_string();

            if (sName.empty())
                throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_INVALIDNAMEATTRIBUTE);
            if (sDescription.empty())
                throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_INVALIDDESCRIPTIONATTRIBUTE);

            if (sChildName == "bool") {
                m_pDriverEnvironment->RegisterBoolParameter(sName, sDescription, false);
                auto pValue = std::make_shared<CDriver_BuRBoolValue>(sName, nAddress);
                m_DriverParameters.push_back(pValue);
                m_DriverParameterMap.insert(std::make_pair(sName, pValue));
            }

            if (sChildName == "string") {
                auto lengthAttrib = childNode.attribute("length");
                if (lengthAttrib.empty())
                    throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_NOLENGTHATTRIBUTE);
                uint32_t nLength = lengthAttrib.as_uint(0xffff);
                if (nLength >= 65536)
                    throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_INVALIDLENGTHATTRIBUTE);

                m_pDriverEnvironment->RegisterStringParameter(sName, sDescription, "");
                auto pValue = std::make_shared<CDriver_BuRStringValue>(sName, nAddress, nLength);
                m_DriverParameters.push_back(pValue);
                m_DriverParameterMap.insert(std::make_pair(sName, pValue));
            }

            if (sChildName == "real") {
                m_pDriverEnvironment->RegisterDoubleParameter(sName, sDescription, 0.0);
                auto pValue = std::make_shared<CDriver_BuRRealValue>(sName, nAddress);
                m_DriverParameters.push_back(pValue);
                m_DriverParameterMap.insert(std::make_pair(sName, pValue));
            }

            if (sChildName == "lreal") {
                m_pDriverEnvironment->RegisterDoubleParameter(sName, sDescription, 0.0);
                auto pValue = std::make_shared<CDriver_BuRLRealValue>(sName, nAddress);
                m_DriverParameters.push_back(pValue);
                m_DriverParameterMap.insert(std::make_pair(sName, pValue));
            }

            if (sChildName == "dint") {
                m_pDriverEnvironment->RegisterIntegerParameter(sName, sDescription, 0);
                auto pValue = std::make_shared<CDriver_BuRDIntValue>(sName, nAddress);
                m_DriverParameters.push_back(pValue);
                m_DriverParameterMap.insert(std::make_pair(sName, pValue));
            }

            if (sChildName == "int") {
                m_pDriverEnvironment->RegisterIntegerParameter(sName, sDescription, 0);
                auto pValue = std::make_shared<CDriver_BuRIntValue>(sName, nAddress);
                m_DriverParameters.push_back(pValue);
                m_DriverParameterMap.insert(std::make_pair(sName, pValue));
            }

        }
    }


    m_pTcpService = brynet::net::TcpService::Create();
    m_pTcpService->startWorkerThread(m_nWorkerThreadCount);

    m_pAsyncConnector = brynet::net::AsyncConnector::Create();
    m_pAsyncConnector->startWorkerThread();


}

std::string CDriver_BuR::GetName()
{
    return m_sName;
}

std::string CDriver_BuR::GetType()
{
    return "bur-1.0";
}

void CDriver_BuR::GetVersion(LibMCDriver_BuR_uint32& nMajor, LibMCDriver_BuR_uint32& nMinor, LibMCDriver_BuR_uint32& nMicro, std::string& sBuild)
{
    nMajor = 1;
    nMinor = 0;
    nMicro = 0;
}

void CDriver_BuR::GetHeaderInformation(std::string& sNameSpace, std::string& sBaseName)
{
    sNameSpace = "LibMCDriver_BuR";
    sBaseName = "libmcdriver_bur";
}

void CDriver_BuR::QueryParameters()
{
    brynet::base::app_kbhit();

    if (!m_pCurrentConnection.expired()) {
        auto pConnection = m_pCurrentConnection.lock();
        sendSimpleCommandToPLC(pConnection, COMMAND_DIRECT_MACHINESTATUS);
    }

}

void CDriver_BuR::sendSimpleCommandToPLC(std::shared_ptr<brynet::net::TcpConnection> pConnection, uint32_t nCommandID)
{
    sAMCFToPLCPacketPayload payLoad;
    for (uint32_t nIndex = 0; nIndex < 24; nIndex++)
        payLoad.m_nData[nIndex] = 0;
    sendCommandToPLC(pConnection, nCommandID, payLoad);
}

void CDriver_BuR::sendCommandToPLC(std::shared_ptr<brynet::net::TcpConnection> pConnection, uint32_t nCommandID, sAMCFToPLCPacketPayload payLoad)
{
    if (pConnection.get() == nullptr)
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_INVALIDTCPCONNECTIONPOINTER);

    if (m_SentPacketQueue.size () >= m_nMaxPacketQueueSize)
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_PACKETQUEUEOVERFLOW);

    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(1, 1024 * 1024 * 1024);
    uint32_t nClientID = distribution(generator);

    uint64_t nTimeStamp = m_pDriverEnvironment->GetGlobalTimerInMilliseconds();

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
    packetToSend.m_nChecksum = CRC::Calculate(&packetToSend, ((intptr_t)(&packetToSend.m_nChecksum) - (intptr_t)(&packetToSend)), CRC::CRC_32());;

    auto pSendInfo = std::make_shared<CDriver_BuRSendInfo>(nCommandID, m_nSequenceID, nClientID, nTimeStamp);
    {        
        std::lock_guard<std::mutex> lockGuard(m_ConnectionMutex);        
        m_SentPacketQueue.insert(std::make_pair(m_nSequenceID, pSendInfo));
    }


    m_nSequenceID++;

    pConnection->send((const char*)&packetToSend, sizeof(packetToSend));
}


void CDriver_BuR::handlePacket (PDriver_BuRPacket pPacket, uint32_t nSequenceID, uint32_t nClientID)
{
    if (pPacket.get() == nullptr)
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_INVALIDPARAM);
        

    PDriver_BuRSendInfo pSendInfo;
    {
        std::lock_guard<std::mutex> lockGuard(m_ConnectionMutex);
        auto iIter = m_SentPacketQueue.find(nSequenceID);
        if (iIter != m_SentPacketQueue.end()) {
            pSendInfo = iIter->second;
            m_SentPacketQueue.erase(nSequenceID);
        }
    }

    if (pSendInfo.get() == nullptr)
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_INVALIDCLIENTSEQUENCEID);
    if (pSendInfo->getSequenceID () != nSequenceID)
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_INVALIDCLIENTSEQUENCEID);
    if (pSendInfo->getClientID () != nClientID)
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_INVALIDCLIENTID);

    auto nCommandID = pPacket->getCommandID();
    if (nCommandID == COMMAND_DIRECT_MACHINESTATUS) {

        for (auto driverParameter : m_DriverParameters) {
            auto sName = driverParameter->getName();
            
            if (dynamic_cast<CDriver_BuRDIntValue*> (driverParameter.get()) != nullptr) {
                m_pDriverEnvironment->SetIntegerParameter(sName, pPacket->readUInt32 (driverParameter->getAddress ()));
            }

            if (dynamic_cast<CDriver_BuRIntValue*> (driverParameter.get()) != nullptr) {
                m_pDriverEnvironment->SetIntegerParameter(sName, pPacket->readUInt16(driverParameter->getAddress()));
            }

            if (dynamic_cast<CDriver_BuRRealValue*> (driverParameter.get()) != nullptr) {
                m_pDriverEnvironment->SetDoubleParameter(sName, pPacket->readFloat(driverParameter->getAddress()));
            }

            if (dynamic_cast<CDriver_BuRLRealValue*> (driverParameter.get()) != nullptr) {
                m_pDriverEnvironment->SetDoubleParameter(sName, pPacket->readDouble(driverParameter->getAddress()));
            }

            if (dynamic_cast<CDriver_BuRBoolValue*> (driverParameter.get()) != nullptr) {
                m_pDriverEnvironment->SetBoolParameter(sName, pPacket->readBool(driverParameter->getAddress()));
            }

            auto pStringValue = dynamic_cast<CDriver_BuRStringValue*> (driverParameter.get());
            if (pStringValue != nullptr) {
                m_pDriverEnvironment->SetStringParameter(sName, pPacket->readString(driverParameter->getAddress(), pStringValue->getLength ()));
            }

        }

    }

}

void CDriver_BuR::enterCallback(const std::shared_ptr <brynet::net::TcpConnection> session, const std::string& sIPAddress, const uint32_t nPort)
{

    std::lock_guard<std::mutex> lockGuard(m_ConnectionMutex);

    if (!m_pCurrentConnection.expired()) {        
        auto pConnection = m_pCurrentConnection.lock();
        pConnection->postDisConnect();        
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

    m_pCurrentConnection = session->weak_from_this();
    
}

void CDriver_BuR::failedCallback()
{
    if (!m_pCurrentConnection.expired()) {
        std::lock_guard<std::mutex> lockGuard(m_ConnectionMutex);
        auto pConnection = m_pCurrentConnection.lock();
        pConnection->postDisConnect();
    }
    m_pCurrentConnection.reset();

}


void CDriver_BuR::Connect(const std::string& sIPAddress, const LibMCDriver_BuR_uint32 nPort, const LibMCDriver_BuR_uint32 nTimeout)
{
    brynet::net::wrapper::ConnectionBuilder connectionBuilder;

    connectionBuilder.WithService(m_pTcpService)
        .WithConnector(m_pAsyncConnector)
        .WithMaxRecvBufferSize(m_nMaxReceiveBufferSize)
        .AddEnterCallback([this, sIPAddress, nPort](const brynet::net::TcpConnection::Ptr& session) { enterCallback(session, sIPAddress, nPort); });

    connectionBuilder
        .WithAddr(sIPAddress, nPort)
        .WithTimeout(std::chrono::milliseconds(nTimeout))
        .WithFailedCallback([this]() { failedCallback(); })
        .AddSocketProcessCallback([](brynet::net::TcpSocket& socket) {
            socket.setNodelay();
        })
        .asyncConnect();

}

void CDriver_BuR::Disconnect()
{
    if (!m_pCurrentConnection.expired()) {
        std::lock_guard<std::mutex> lockGuard(m_ConnectionMutex);
        auto pConnection = m_pCurrentConnection.lock();
        pConnection->postDisConnect();
    }
    m_pCurrentConnection.reset();
}
