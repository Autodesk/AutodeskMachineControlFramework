/*++

Copyright (C) 2023 Autodesk Inc.

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


#ifndef __LIBMCDRIVER_MODBUSTCPINSTANCE
#define __LIBMCDRIVER_MODBUSTCPINSTANCE

#include <string>
#include <vector>
#include <memory>

#include "libmcenv_tcpipinstance.hpp"

#define MODBUSTCP_MAX_PAYLOADSIZE 1024
#define MODBUSTCP_MAX_COILCOUNT 1024
#define MODBUSTCP_MAX_REGISTERCOUNT 128

namespace LibMCEnv {
namespace Impl {


#pragma pack(push)
#pragma pack(1)
		typedef struct _sModbusTCPPacket
        {
            uint8_t m_TransactionIDHigh;
            uint8_t m_TransactionIDLow;
            uint8_t m_ProtocolIDHigh;
            uint8_t m_ProtocolIDLow;
            uint8_t m_LengthHigh;
            uint8_t m_LengthLow;
            uint8_t m_UnitIdentifier;
            uint8_t m_FunctionCode;
            uint8_t m_PayloadData[MODBUSTCP_MAX_PAYLOADSIZE];
        } sModbusTCPPacket;
#pragma pack(pop)

        typedef sModbusTCPPacket sModbusTCPRequest;
        typedef sModbusTCPPacket sModbusTCPResponse;



        class CModbusTCPConnectionInstance {
        private:
            std::shared_ptr<CTCPIPSocketConnection> m_pSocketConnection;
            uint32_t m_nCurrentTransactionID;
            uint32_t m_nTransactionIDIncrement;
            std::string m_sIPAddress;
            uint32_t m_nPort;
            uint32_t m_nTimeoutInMs;

            bool m_bDebugMode;

            // Prepares a Modbus TCP request header with an arbitrary payload
            sModbusTCPRequest prepareRequest(uint8_t nFunctionCode, uint16_t nPayloadLength);
            // Prepares a Modbus TCP request of the canonical form (start address + count)
            sModbusTCPRequest prepareCanonicalRequest(uint8_t nFunctionCode, uint16_t nStartAddress, uint16_t nCount);
            // sends a Modbus TCP request and awaits a response
            sModbusTCPResponse sendRequest(sModbusTCPRequest modbusRequest);
            // get and check length from modbus TCP packet
            uint32_t getPayloadLength (const sModbusTCPPacket & modbusPacket);
            // get and check packet length from modbus TCP packet
            uint32_t getPacketLength(const sModbusTCPPacket& modbusPacket);


        public:

            CModbusTCPConnectionInstance(const std::string& sIPAddress, uint32_t nPort, uint32_t nTimeOutInMs);

            virtual ~CModbusTCPConnectionInstance();

            void readCoilStatus (uint16_t nStartAddress, uint16_t nBitCount, std::vector<bool> & coilStatus);
            void readInputStatus(uint16_t nStartAddress, uint16_t nBitCount, std::vector<bool>& inputStatus);
            void readHoldingRegisters(uint16_t nStartAddress, uint16_t nRegisterCount, std::vector<uint16_t>& holdingRegisters);
            void readInputRegisters(uint16_t nStartAddress, uint16_t nRegisterCount, std::vector<uint16_t>& inputRegisters);

            void forceMultipleCoils(uint16_t nStartAddress, const std::vector<uint8_t> & coilStatus);
            void presetMultipleRegisters(uint16_t nStartAddress, std::vector<uint16_t> registerValues);

            void diagnosisCall(uint16_t nSubFunction, uint16_t nDatafield, uint16_t & nDataResponse);

            void connect();
            void disconnect();
            void reconnect ();

            std::string getIPAddress ();
            uint32_t getPort ();
            uint32_t getTimeoutInMs ();

            bool isConnected();

            void debugPacket (const sModbusTCPPacket& modbusPacket, const std::string & sContext);

        };

} // namespace Impl
} // namespace LibMCEnv

#endif // __LIBMCDRIVER_MODBUSTCPINSTANCE
