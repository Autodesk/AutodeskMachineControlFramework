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


Abstract: This is the class declaration of CDriver_BuR

*/


#ifndef __LIBMCDRIVER_BUR_CONNECTOR
#define __LIBMCDRIVER_BUR_CONNECTOR

#include "libmcdriver_bur_connector.hpp"
#include "libmcdriver_bur_definitions.hpp"
#include "libmcdriver_bur_journal.hpp"

// Include custom headers here.
#include <mutex>
#include <map>
#include <memory>
#include <queue>
#include <functional>

#define BUR_MINCUSTOMCOMMANDID 1024
#define BUR_MAXCUSTOMCOMMANDID 65535

#define BUR_COMMAND_DIRECT_INIT 100
#define BUR_COMMAND_DIRECT_BEGINLIST 101
#define BUR_COMMAND_DIRECT_FINISHLIST 102
#define BUR_COMMAND_DIRECT_EXECUTELIST 103
#define BUR_COMMAND_DIRECT_PAUSELIST 104
#define BUR_COMMAND_DIRECT_LISTSTATUS 105
#define BUR_COMMAND_DIRECT_ABORTLIST 106
#define BUR_COMMAND_DIRECT_RESUMELIST 107
#define BUR_COMMAND_DIRECT_MACHINESTATUSLEGACY 108
#define BUR_COMMAND_DIRECT_DELETELIST 112

#define BUR_COMMAND_DIRECT_CURRENTJOURNALSTATUS 120
#define BUR_COMMAND_DIRECT_CURRENTJOURNALSCHEMA 121
#define BUR_COMMAND_DIRECT_RETRIEVEJOURNALVARIABLE 122
#define BUR_COMMAND_DIRECT_RETRIEVEJOURNALHISTORY 123

#define PACKET_SIGNATURE_LEGACY 0xAB

#define BUR_MAX_PAYLOADLENGTH (1024 * 1024)

namespace LibMCDriver_BuR {
namespace Impl {


    enum class eDriver_BurProtocolVersion {
        Unknown = 0,
        Legacy = 1,
        Version3 = 3
    };

/*************************************************************************************************************************
 Class declaration of CDriver_BuR 
**************************************************************************************************************************/

class CDriver_BuRPacket {
private:
    uint32_t m_nCommandID;
    uint32_t m_nStatusCode;
    std::vector<uint8_t> m_Data;

public:
    CDriver_BuRPacket(uint32_t nCommandID, uint32_t nStatusCode);
    ~CDriver_BuRPacket();

    uint32_t getCommandID();
    uint32_t getStatusCode();

    uint64_t readUInt64(uint32_t nAddress);
    uint32_t readUInt32(uint32_t nAddress);
    uint16_t readUInt16(uint32_t nAddress);
    uint8_t readUInt8(uint32_t nAddress);
    float readFloat(uint32_t nAddress);
    double readDouble(uint32_t nAddress);
    bool readBool(uint32_t nAddress);
    std::string readString(uint32_t nAddress, uint32_t nLength);

    std::vector<uint8_t> & getDataBuffer ();
};

typedef std::shared_ptr<CDriver_BuRPacket> PDriver_BuRPacket;

class CDriver_BuRConnector {
private:

protected:

    LibMCEnv::PDriverEnvironment m_pDriverEnvironment;

    uint32_t m_nWorkerThreadCount;
    uint32_t m_nMaxReceiveBufferSize;

    eDriver_BurProtocolVersion m_ProtocolVersion;

    uint32_t m_nMajorVersion;
    uint32_t m_nMinorVersion;
    uint32_t m_nPatchVersion;
    uint32_t m_nBuildVersion;

    uint32_t m_nMaxPacketQueueSize;
    uint32_t m_nSequenceID;
    uint32_t m_nPacketSignature;

    uint32_t m_nReceiveTimeoutInMS;

    bool m_StartJournaling;

    LibMCEnv::PTCPIPConnection m_pCurrentConnection;

    std::mutex m_ConnectionOrJournalMutex;
    std::mutex m_SequenceMapMutex;

    std::list<PDriver_BuRValue> m_DriverParameters;
    std::map<std::string, PDriver_BuRValue> m_DriverParameterMap;

    std::map<std::string, PDriver_BuRCommandDefinition> m_CommandDefinitions;
    //std::map<std::string, PDriver_BuRValue> m_ControlParameterMap;

    PDriver_BuRJournal m_pJournal;

    //PDriver_BuRPacket receiveCommandFromPLCEx (CDriver_BuRSocketConnection* pConnection);

    void sendCommandsToPLCLegacy(std::vector<sAMCFToPLCPacketToSend>& packetList);
    void sendCommandsToPLCVersion3(std::vector<sAMCFToPLCPacketToSend>& packetList);


public:

	CDriver_BuRConnector (LibMCEnv::PDriverEnvironment pDriverEnvironment, uint32_t nWorkerThreadCount, uint32_t nMaxReceiveBufferSize, uint32_t nMajorVersion, uint32_t nMinorVersion, uint32_t nPatchVersion, uint32_t nBuildVersion, uint32_t nMaxPacketQueueSize, eDriver_BurProtocolVersion ProtocolVersion, uint32_t nPacketSignature);

    virtual ~CDriver_BuRConnector();

    void queryParametersLegacy (BurPacketCallback callback);

    void queryParametersVersion3 (LibMCEnv::PDriverStatusUpdateSession pDriverUpdateInstance);

    void refreshJournal();

    void connect(const std::string& sIPAddress, const uint32_t nPort, const uint32_t nTimeout);

    void disconnect();

    void sendCommandsToPLC(std::vector<sAMCFToPLCPacketToSend>& packetList);

    void sendCommandToPLC(uint32_t nCommandID, BurPacketCallback callback);
    void sendCommandToPLC(uint32_t nCommandID, uint32_t nParameter0, BurPacketCallback callback);
    void sendCommandToPLC(uint32_t nCommandID, uint32_t nParameter0, uint32_t nParameter1, BurPacketCallback callback);
    void sendCommandToPLC(uint32_t nCommandID, uint32_t nParameter0, uint32_t nParameter1, uint32_t nParameter2, BurPacketCallback callback);
    void sendCommandToPLC(uint32_t nCommandID, uint32_t nParameter0, uint32_t nParameter1, uint32_t nParameter2, uint32_t nParameter3, BurPacketCallback callback);
    void sendCommandToPLC(uint32_t nCommandID, uint32_t nParameter0, uint32_t nParameter1, uint32_t nParameter2, uint32_t nParameter3, uint32_t nParameter4, BurPacketCallback callback);
    void sendCommandToPLC(uint32_t nCommandID, sAMCFToPLCPacketPayload payLoad, BurPacketCallback callback);

    sAMCFToPLCPacketToSend makePacket(uint32_t nCommandID, BurPacketCallback callback);
    sAMCFToPLCPacketToSend makePacket(uint32_t nCommandID, uint32_t nParameter0, BurPacketCallback callback);
    sAMCFToPLCPacketToSend makePacket(uint32_t nCommandID, uint32_t nParameter0, uint32_t nParameter1, BurPacketCallback callback);
    sAMCFToPLCPacketToSend makePacket(uint32_t nCommandID, uint32_t nParameter0, uint32_t nParameter1, uint32_t nParameter2, BurPacketCallback callback);
    sAMCFToPLCPacketToSend makePacket(uint32_t nCommandID, uint32_t nParameter0, uint32_t nParameter1, uint32_t nParameter2, uint32_t nParameter3, BurPacketCallback callback);
    sAMCFToPLCPacketToSend makePacket(uint32_t nCommandID, uint32_t nParameter0, uint32_t nParameter1, uint32_t nParameter2, uint32_t nParameter3, uint32_t nParameter4, BurPacketCallback callback);
    sAMCFToPLCPacketToSend makePacket(uint32_t nCommandID, sAMCFToPLCPacketPayload payLoad, BurPacketCallback callback);

    PDriver_BuRJournal retrieveJournalSchema();

    bool isLegacy();

    bool isVersion3();

};

typedef std::shared_ptr<CDriver_BuRConnector> PDriver_BuRConnector;

} // namespace Impl
} // namespace LibMCDriver_BuR

#endif // __LIBMCDRIVER_BUR_CONNECTOR
