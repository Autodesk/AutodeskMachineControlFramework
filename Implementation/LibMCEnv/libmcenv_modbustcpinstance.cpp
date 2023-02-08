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

#include "libmcenv_modbustcpinstance.hpp"
#include "libmcenv_interfaceexception.hpp"

#include <iostream>
#include <iomanip>
#include <vector>
#include <chrono>
#include <thread>
#include <cstring>

#define MODBUSTCP_MAX_PAYLOADSIZE 1024
#define MODBUSTCP_MAX_COILCOUNT 1024
#define MODBUSTCP_MAX_REGISTERCOUNT 128
#define MODBUSTCP_FUNCTION_READCOILSTATUS 1
#define MODBUSTCP_FUNCTION_READINPUTSTATUS 2
#define MODBUSTCP_FUNCTION_READHOLDINGREGISTERS 3
#define MODBUSTCP_FUNCTION_READINPUTREGISTERS 4
#define MODBUSTCP_FUNCTION_DIAGNOSIS 8
#define MODBUSTCP_FUNCTION_FORCEMULTIPLECOILS 15
#define MODBUSTCP_FUNCTION_PRESETMULTIPLEREGISTERS 16

#define MODBUSTCP_DIAGNOSISFUNCTION_ECHO 0
#define MODBUSTCP_DIAGNOSISFUNCTION_COUPLERRESET 1
#define MODBUSTCP_DIAGNOSISFUNCTION_DELETECOUNTERS 10
#define MODBUSTCP_DIAGNOSISFUNCTION_BUSCOMMUNICATIONANSWERS 11
#define MODBUSTCP_DIAGNOSISFUNCTION_ERRORANSWERCOUNTER 13


#define MODBUSTCP_TRANSACTIONID_DEFAULT_SEED 41659
#define MODBUSTCP_TRANSACTIONID_DEFAULT_INCREMENT 49991


namespace LibMCEnv {
    namespace Impl {

        CModbusTCPConnectionInstance::CModbusTCPConnectionInstance(const std::string& sIPAddress, uint32_t nPort, uint32_t nTimeOutInMs)
            : m_sIPAddress(sIPAddress),
            m_nPort(nPort),
            m_nTimeoutInMs(nTimeOutInMs),
            m_nCurrentTransactionID(MODBUSTCP_TRANSACTIONID_DEFAULT_SEED),
            m_nTransactionIDIncrement(MODBUSTCP_TRANSACTIONID_DEFAULT_INCREMENT),
            m_bDebugMode (false)
        {
            CTCPIPSocketConnection::initializeNetworking();
            connect();
        }


        CModbusTCPConnectionInstance::~CModbusTCPConnectionInstance()
        {
            if (m_pSocketConnection.get() != nullptr) {
                if (m_pSocketConnection->isConnected())
                    m_pSocketConnection->disconnect();
                m_pSocketConnection = nullptr;
            }

        }

        sModbusTCPRequest CModbusTCPConnectionInstance::prepareRequest(uint8_t nFunctionCode, uint16_t nPayloadLength)
        {
            if (nPayloadLength > MODBUSTCP_MAX_PAYLOADSIZE)
                throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDMODBUSTCPPAYLOADSIZE);

            // UnitIdentifier and FunctionCode count as data in the ModBus protocol...
            uint32_t nRequestLength = (uint32_t)nPayloadLength + 2;

            // Fill modBus TCP Header
            sModbusTCPRequest modBusRequest;
            memset((void*)&modBusRequest, 0, sizeof(modBusRequest));
            modBusRequest.m_TransactionIDHigh = (m_nCurrentTransactionID >> 8) & 0xff;
            modBusRequest.m_TransactionIDLow = m_nCurrentTransactionID & 0xff;
            modBusRequest.m_ProtocolIDHigh = 0;
            modBusRequest.m_ProtocolIDLow = 0;
            modBusRequest.m_LengthHigh = (nRequestLength >> 8) & 0xff;
            modBusRequest.m_LengthLow = (nRequestLength) & 0xff;
            modBusRequest.m_UnitIdentifier = 255;
            modBusRequest.m_FunctionCode = nFunctionCode;

            m_nCurrentTransactionID = (uint32_t)(m_nCurrentTransactionID + m_nTransactionIDIncrement) % 65536;

            return modBusRequest;
        }

        sModbusTCPRequest CModbusTCPConnectionInstance::prepareCanonicalRequest(uint8_t nFunctionCode, uint16_t nStartAddress, uint16_t nCount)
        {
            // Canonical requests have 4 bytes of payload
            sModbusTCPRequest modBusRequest = prepareRequest(nFunctionCode, 4);

            // Start Address High
            modBusRequest.m_PayloadData[0] = (nStartAddress >> 8) & 0xff;
            // Start Address Low
            modBusRequest.m_PayloadData[1] = (nStartAddress) & 0xff;
            // Count High
            modBusRequest.m_PayloadData[2] = (nCount >> 8) & 0xff;
            // Count Low
            modBusRequest.m_PayloadData[3] = (nCount) & 0xff;

            return modBusRequest;
        }

        uint32_t CModbusTCPConnectionInstance::getPayloadLength(const sModbusTCPPacket& modbusPacket)
        {
            uint32_t nRequestLength = (uint32_t)modbusPacket.m_LengthLow + (((uint32_t)modbusPacket.m_LengthHigh) << 8);

            // UnitIdentifier and FunctionCode count as data in the ModBus protocol...
            if (nRequestLength < 2)
                throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDMODBUSTCPREQUEST);
            uint32_t nPayloadLength = nRequestLength - 2;
            if (nPayloadLength > MODBUSTCP_MAX_PAYLOADSIZE)
                throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDMODBUSTCPPAYLOADSIZE);

            return nPayloadLength;

        }

        uint32_t CModbusTCPConnectionInstance::getPacketLength(const sModbusTCPPacket& modbusPacket)
        {
            uint32_t nPayloadLength = getPayloadLength(modbusPacket);

            // Modbus TCP Header Length is 6 bytes plus 2 bytes for unit identifier and function code. 
            return nPayloadLength + 8;
        }


        sModbusTCPResponse CModbusTCPConnectionInstance::sendRequest(sModbusTCPRequest modbusRequest)
        {
            uint32_t nRequestPacketLength = getPacketLength(modbusRequest);

            // Send request to server
            m_pSocketConnection->sendBuffer((uint8_t*)&modbusRequest, nRequestPacketLength);

            // Receiving response data
            std::vector<uint8_t> responseBuffer;
            m_pSocketConnection->receiveBuffer(responseBuffer, sizeof(sModbusTCPResponse), false, m_nTimeoutInMs);
            // Modbus TCP Header Length is 6 bytes plus 2 bytes for unit identifier and function code.
            size_t receivedLength = responseBuffer.size();
            if (receivedLength < 8)
                throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INSUFFICIENTMODBUSTCPRESPONSEHEADER);
            if (receivedLength > sizeof(sModbusTCPResponse))
                throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_MODBUSTCPRESPONSEOVERFLOW);

            // fill static struct with zeros
            responseBuffer.resize(sizeof(sModbusTCPResponse));
            for (size_t nIndex = receivedLength; nIndex < sizeof(sModbusTCPResponse); nIndex++)
                responseBuffer.at(nIndex) = 0;
            sModbusTCPResponse modbusResponse = *((sModbusTCPResponse*)responseBuffer.data());

            // Check response header
            if ((modbusResponse.m_TransactionIDHigh != modbusRequest.m_TransactionIDHigh) || (modbusResponse.m_TransactionIDLow != modbusRequest.m_TransactionIDLow))
                throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDMODBUSTCPTRANSACTIONIDRESPONSE);
            if ((modbusResponse.m_ProtocolIDHigh != modbusRequest.m_ProtocolIDHigh) || (modbusResponse.m_ProtocolIDLow != modbusRequest.m_ProtocolIDLow))
                throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDMODBUSTCPPROTOCOLIDRESPONSE);
            if (modbusResponse.m_UnitIdentifier != modbusRequest.m_UnitIdentifier)
                throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDMODBUSTCPUNITIDENTIFIERRESPONSE);

            // Check packet length vs. header length
            uint32_t nResponsePacketLength = getPacketLength(modbusResponse);
            if (receivedLength != nResponsePacketLength)
                throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_MODBUSTCPRESPONSELENGTHMISMATCH);

            return modbusResponse;
        }


        void CModbusTCPConnectionInstance::readCoilStatus(uint16_t nStartAddress, uint16_t nBitCount, std::vector<bool>& coilStatus)
        {
            if (nBitCount == 0)
                throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_MODBUSTCPINVALIDCOILSTATUSBITCOUNT);

            sModbusTCPRequest modBusRequest = prepareCanonicalRequest(MODBUSTCP_FUNCTION_READCOILSTATUS, nStartAddress, nBitCount);

            debugPacket(modBusRequest, "readCoilStatus request");

            sModbusTCPResponse modBusResponse = sendRequest(modBusRequest);

            debugPacket(modBusResponse, "readCoilStatus response");

            uint32_t nPayloadSize = getPayloadLength(modBusResponse);

            if (modBusResponse.m_FunctionCode != MODBUSTCP_FUNCTION_READCOILSTATUS)
                throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_MODBUSTCPINVALIDCOILSTATUSRESPONSE);
            if (nPayloadSize < 2)
                throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_MODBUSTCPCOILSTATUSRESPONSEEMPTY);

            coilStatus.resize(nBitCount);
            uint32_t nByteCount = modBusResponse.m_PayloadData[0];
            if (nPayloadSize != (nByteCount + 1))
                throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_MODBUSTCPINVALIDCOILSTATUSRESPONSESIZE);

            for (uint32_t nByteIndex = 0; nByteIndex < nByteCount; nByteIndex++) {
                uint8_t nByteValue = modBusResponse.m_PayloadData[1 + nByteIndex];
                for (uint32_t nBitIndex = 0; nBitIndex < 8; nBitIndex++) {
                    uint32_t nCoilIndex = nByteIndex * 8 + nBitIndex;
                    if (nCoilIndex < coilStatus.size()) {
                        if ((nByteValue & (1UL << nBitIndex)) != 0)
                            coilStatus.at(nCoilIndex) = true;
                        else
                            coilStatus.at(nCoilIndex) = false;
                    }

                }

            }

        }

        void CModbusTCPConnectionInstance::readInputStatus(uint16_t nStartAddress, uint16_t nBitCount, std::vector<bool>& inputStatus)
        {
            if (nBitCount == 0)
                throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_MODBUSTCPINVALIDINPUTSTATUSBITCOUNT);

            sModbusTCPRequest modBusRequest = prepareCanonicalRequest(MODBUSTCP_FUNCTION_READINPUTSTATUS, nStartAddress, nBitCount);
            debugPacket(modBusRequest, "readInputStatus request");

            sModbusTCPResponse modBusResponse = sendRequest(modBusRequest);

            debugPacket(modBusResponse, "readInputStatus response");

            uint32_t nPayloadSize = getPayloadLength(modBusResponse);

            if (modBusResponse.m_FunctionCode != MODBUSTCP_FUNCTION_READINPUTSTATUS)
                throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_MODBUSTCPINVALIDINPUTSTATUSRESPONSE);
            if (nPayloadSize < 2)
                throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_MODBUSTCPINPUTSTATUSRESPONSEEMPTY);

            inputStatus.resize(nBitCount);
            uint32_t nByteCount = modBusResponse.m_PayloadData[0];
            uint32_t nDesiredByteCount = (nBitCount + 7) / 8;

            if (nByteCount != nDesiredByteCount)
                throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_MODBUSTCPINVALIDINPUTSTATUSRESPONSESIZE);
            if (nPayloadSize != (nByteCount + 1))
                throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_MODBUSTCPINVALIDINPUTSTATUSRESPONSESIZE);

            for (uint32_t nByteIndex = 0; nByteIndex < nByteCount; nByteIndex++) {
                uint8_t nByteValue = modBusResponse.m_PayloadData[1 + nByteIndex];
                for (uint32_t nBitIndex = 0; nBitIndex < 8; nBitIndex++) {
                    uint32_t nInputIndex = nByteIndex * 8 + nBitIndex;
                    if (nInputIndex < inputStatus.size()) {
                        if ((nByteValue & (1UL << nBitIndex)) != 0)
                            inputStatus.at(nInputIndex) = true;
                        else
                            inputStatus.at(nInputIndex) = false;
                    }

                }

            }

        }

        void CModbusTCPConnectionInstance::readHoldingRegisters(uint16_t nStartAddress, uint16_t nRegisterCount, std::vector<uint16_t>& holdingRegisters)
        {
            sModbusTCPRequest modBusRequest = prepareCanonicalRequest(MODBUSTCP_FUNCTION_READHOLDINGREGISTERS, nStartAddress, nRegisterCount);
            debugPacket(modBusRequest, "readHoldingRegisters request");

            sModbusTCPResponse modBusResponse = sendRequest(modBusRequest);
            debugPacket(modBusResponse, "readHoldingRegisters response");

            uint32_t nPayloadSize = getPayloadLength(modBusResponse);

            if (modBusResponse.m_FunctionCode != MODBUSTCP_FUNCTION_READHOLDINGREGISTERS)
                throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_MODBUSTCPINVALIDHOLDINGREGISTERSRESPONSE);
            if (nPayloadSize < 2)
                throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_MODBUSTCPHOLDINGREGISTERSRESPONSEEMPTY);

            uint32_t nByteCount = modBusResponse.m_PayloadData[0];

            if (nPayloadSize != (nByteCount + 1))
                throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_MODBUSTCPINVALIDHOLDINGREGISTERSRESPONSESIZE);

            if ((nRegisterCount * 2) != nByteCount)
                throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_MODBUSTCPINVALIDHOLDINGREGISTERSRESPONSESIZE);

            holdingRegisters.resize(nRegisterCount);

            for (uint32_t nRegisterIndex = 0; nRegisterIndex < nRegisterCount; nRegisterIndex++)
            {
                uint32_t nHighByte = modBusResponse.m_PayloadData[1 + nRegisterIndex * 2];
                uint32_t nLowByte = modBusResponse.m_PayloadData[2 + nRegisterIndex * 2];
                uint32_t nValue = (nHighByte << 8) | nLowByte;
                holdingRegisters.at(nRegisterIndex) = nValue;

            }


        }

        void CModbusTCPConnectionInstance::readInputRegisters(uint16_t nStartAddress, uint16_t nRegisterCount, std::vector<uint16_t>& inputRegisters)
        {
            sModbusTCPRequest modBusRequest = prepareCanonicalRequest(MODBUSTCP_FUNCTION_READINPUTREGISTERS, nStartAddress, nRegisterCount);
            debugPacket(modBusRequest, "readInputRegisters request");

            sModbusTCPResponse modBusResponse = sendRequest(modBusRequest);
            debugPacket(modBusResponse, "readInputRegisters response");

            uint32_t nPayloadSize = getPayloadLength(modBusResponse);

            if (modBusResponse.m_FunctionCode != MODBUSTCP_FUNCTION_READINPUTREGISTERS)
                throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_MODBUSTCPINVALIDINPUTREGISTERSRESPONSE);
            if (nPayloadSize < 2)
                throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_MODBUSTCPINPUTREGISTERSRESPONSEEMPTY);

            uint32_t nByteCount = modBusResponse.m_PayloadData[0];

            if (nPayloadSize != (nByteCount + 1))
                throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_MODBUSTCPINVALIDINPUTREGISTERSRESPONSESIZE);

            if ((nRegisterCount * 2) != nByteCount)
                throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_MODBUSTCPINVALIDINPUTREGISTERSRESPONSESIZE);

            inputRegisters.resize(nRegisterCount);

            for (uint32_t nRegisterIndex = 0; nRegisterIndex < nRegisterCount; nRegisterIndex++) 
            {
               uint32_t nHighByte = modBusResponse.m_PayloadData[1 + nRegisterIndex * 2];
               uint32_t nLowByte = modBusResponse.m_PayloadData[2 + nRegisterIndex * 2];
               uint32_t nValue = (nHighByte << 8) | nLowByte;
               inputRegisters.at(nRegisterIndex) = nValue;

            }

        }



        void CModbusTCPConnectionInstance::forceMultipleCoils(uint16_t nStartAddress, const std::vector<uint8_t>& coilStatus)
        {
            if (coilStatus.size() > MODBUSTCP_MAX_COILCOUNT)
                throw std::runtime_error("modbus tcp invalid coil count");
            uint32_t nCount = (uint32_t)coilStatus.size();
            uint32_t nByteCount = (nCount + 7) / 8;

            // Payload is 2 bytes for start address, 2 bytes for Coil Count, 1 byte for byteCount
            uint32_t nPayloadSize = nByteCount + 5;

            // Canonical requests have 4 bytes of payload
            sModbusTCPRequest modBusRequest = prepareRequest(MODBUSTCP_FUNCTION_FORCEMULTIPLECOILS, nPayloadSize);


            // Start Address High
            modBusRequest.m_PayloadData[0] = (nStartAddress >> 8) & 0xff;
            // Start Address Low
            modBusRequest.m_PayloadData[1] = (nStartAddress) & 0xff;
            // Count High
            modBusRequest.m_PayloadData[2] = (nCount >> 8) & 0xff;
            // Count Low
            modBusRequest.m_PayloadData[3] = (nCount) & 0xff;

            // Byte count in request
            modBusRequest.m_PayloadData[4] = nByteCount;

            // Encode data buffer
            for (uint32_t nByteIndex = 0; nByteIndex < nByteCount; nByteIndex++) {
                uint8_t nByteValue = 0;
                for (uint32_t nBitIndex = 0; nBitIndex < 8; nBitIndex++) {
                    uint32_t nCoilIndex = nByteIndex * 8 + nBitIndex;
                    if (nCoilIndex < coilStatus.size()) {
                        if (coilStatus.at(nCoilIndex))
                            nByteValue |= 1 << nBitIndex;
                    }
                }

                modBusRequest.m_PayloadData[5 + nByteIndex] = nByteValue;

            }

            debugPacket(modBusRequest, "forceMultipleCoils request");

            sModbusTCPResponse modBusResponse = sendRequest(modBusRequest);
            debugPacket(modBusResponse, "forceMultipleCoils response");
        }

        void CModbusTCPConnectionInstance::presetMultipleRegisters(uint16_t nStartAddress, std::vector<uint16_t> registerValues)
        {
            if (registerValues.size() > MODBUSTCP_MAX_REGISTERCOUNT)
                throw std::runtime_error("modbus tcp invalid register count");
            uint32_t nCount = (uint32_t)registerValues.size();
            uint32_t nByteCount = nCount * 2;

            // Payload is 2 bytes for start address, 2 bytes for register Count, 1 byte for byteCount
            uint32_t nPayloadSize = nByteCount + 5;

            // Canonical requests have 4 bytes of payload
            sModbusTCPRequest modBusRequest = prepareRequest(MODBUSTCP_FUNCTION_PRESETMULTIPLEREGISTERS, nPayloadSize);


            // Start Address High
            modBusRequest.m_PayloadData[0] = (nStartAddress >> 8) & 0xff;
            // Start Address Low
            modBusRequest.m_PayloadData[1] = (nStartAddress) & 0xff;
            // Count High
            modBusRequest.m_PayloadData[2] = (nCount >> 8) & 0xff;
            // Count Low
            modBusRequest.m_PayloadData[3] = (nCount) & 0xff;

            // Byte count in request
            modBusRequest.m_PayloadData[4] = nByteCount;

            for (uint32_t nWordIndex = 0; nWordIndex < nCount; nWordIndex++) {
                uint16_t nRegisterValue = registerValues.at(nWordIndex);
                modBusRequest.m_PayloadData[5 + nWordIndex * 2] = (nRegisterValue >> 8) & 0xff;
                modBusRequest.m_PayloadData[6 + nWordIndex * 2] = nRegisterValue & 0xff;

            }

            debugPacket(modBusRequest, "presetMultipleRegisters request");

            sModbusTCPResponse modBusResponse = sendRequest(modBusRequest);
            debugPacket(modBusResponse, "presetMultipleRegisters response");

        }

        void CModbusTCPConnectionInstance::diagnosisCall(uint16_t nSubFunction, uint16_t nDatafield, uint16_t& nDataResponse)
        {
            // Canonical requests have 4 bytes of payload
            sModbusTCPRequest modBusRequest = prepareRequest(MODBUSTCP_FUNCTION_DIAGNOSIS, 4);


            // Start Address High
            modBusRequest.m_PayloadData[0] = (nSubFunction >> 8) & 0xff;
            // Start Address Low
            modBusRequest.m_PayloadData[1] = (nSubFunction) & 0xff;
            // Count High
            modBusRequest.m_PayloadData[2] = (nDatafield >> 8) & 0xff;
            // Count Low
            modBusRequest.m_PayloadData[3] = (nDatafield) & 0xff;

            debugPacket(modBusRequest, "diagnosisCall request");

            sModbusTCPResponse modBusResponse = sendRequest(modBusRequest);
            debugPacket(modBusResponse, "diagnosisCall response");

        }

        void CModbusTCPConnectionInstance::connect()
        {
            disconnect();
            m_pSocketConnection = std::make_shared<CTCPIPSocketConnection>(m_sIPAddress, m_nPort);
        }

        void CModbusTCPConnectionInstance::disconnect()
        {
            if (m_pSocketConnection.get() != nullptr) {
                m_pSocketConnection->disconnect();
                m_pSocketConnection = nullptr;
            }

        }

        void CModbusTCPConnectionInstance::reconnect()
        {
            connect();
        }

        std::string CModbusTCPConnectionInstance::getIPAddress()
        {
            return m_sIPAddress;
        }

        uint32_t CModbusTCPConnectionInstance::getPort()
        {
            return m_nPort;
        }

        uint32_t CModbusTCPConnectionInstance::getTimeoutInMs()
        {
            return m_nTimeoutInMs;
        }

        bool CModbusTCPConnectionInstance::isConnected()
        {
            if (m_pSocketConnection.get() != nullptr)
                return m_pSocketConnection->isConnected();

            return false;
        }


        void CModbusTCPConnectionInstance::debugPacket(const sModbusTCPPacket& modbusPacket, const std::string& sContext)
        {
            if (m_bDebugMode) {
                std::cout << "DEBUG " << sContext << ": " << std::endl;

                uint8_t* pData = (uint8_t*)&modbusPacket;
                size_t nDataSize = getPacketLength(modbusPacket);

                for (size_t nIndex = 0; nIndex < nDataSize; nIndex++) {
                    std::cout << std::hex << (uint32_t)pData[nIndex] << " ";
                }
                std::cout << std::dec << std::endl;

                std::cout << "   " << std::endl;
            }
        }


    }
}