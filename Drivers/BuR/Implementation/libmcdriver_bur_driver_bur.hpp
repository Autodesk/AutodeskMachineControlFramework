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


#ifndef __LIBMCDRIVER_BUR_DRIVER_BUR
#define __LIBMCDRIVER_BUR_DRIVER_BUR

#include "libmcdriver_bur_interfaces.hpp"
#include "libmcdriver_bur_plccommand.hpp"

// Parent classes
#include "libmcdriver_bur_driver.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include <mutex>
#include <map>
#include <memory>

namespace brynet {
    namespace net {
        class TcpService;
        class TcpConnection;
        class AsyncConnector;
    }
}

namespace LibMCDriver_BuR {
namespace Impl {

#pragma pack (1)

    struct sAMCFToPLCPacketPayload {
        uint8_t m_nData[24];
    };

    struct sAMCFToPLCPacket {
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


    struct sPLCToAMCFPacket {
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


/*************************************************************************************************************************
 Class declaration of CDriver_BuR 
**************************************************************************************************************************/

class CDriver_BuRPacket {
private:
    uint32_t m_nCommandID;
    uint32_t m_nStatusCode;
    std::vector<uint8_t> m_Data;
public:
    CDriver_BuRPacket(uint32_t nCommandID, uint32_t nStatusCode, uint32_t nDataLen, const uint8_t* pData);
    ~CDriver_BuRPacket();

    uint32_t getCommandID();
    uint32_t getStatusCode();

    uint32_t readUInt32(uint32_t nAddress);
    uint16_t readUInt16(uint32_t nAddress);
    uint8_t readUInt8(uint32_t nAddress);
    float readFloat(uint32_t nAddress);
    double readDouble(uint32_t nAddress);
    bool readBool(uint32_t nAddress);
    std::string readString(uint32_t nAddress, uint32_t nLength);
};

typedef std::shared_ptr<CDriver_BuRPacket> PDriver_BuRPacket;


class CDriver_BuRSendInfo {
private:
    uint32_t m_nCommandID;
    uint32_t m_nSequenceID;  
    uint32_t m_nClientID;
    uint64_t m_nTimeStamp;
public:
    CDriver_BuRSendInfo(uint32_t nCommandID, uint32_t nSequenceID, uint32_t nClientID, uint64_t nTimeStamp);
    ~CDriver_BuRSendInfo();

    uint32_t getCommandID();
    uint32_t getSequenceID();
    uint32_t getClientID();
    uint64_t getTimeStamp();
};

typedef std::shared_ptr<CDriver_BuRSendInfo> PDriver_BuRSendInfo;



class CDriver_BuRValue {
protected:
    std::string m_sName;
    uint32_t m_nAddress;
public:

    CDriver_BuRValue(const std::string& sName, const uint32_t nAddress);
    virtual ~CDriver_BuRValue();

    std::string getName(); 
    uint32_t getAddress();

    virtual ePLCFieldType getFieldType () = 0;

    //virtual void writeToPLCParameters (LibS7Com::CCommandParameters * pCommandParameters, const std::string & sStringValue) = 0;

};


class CDriver_BuRRealValue : public CDriver_BuRValue {
protected:
public:

    CDriver_BuRRealValue(const std::string& sName, const uint32_t nAddress);

    //double readValue(LibS7Com::CPLCCommunication * pCommunication);

    ePLCFieldType getFieldType() override;

    //void writeToPLCParameters(LibS7Com::CCommandParameters* pCommandParameters, const std::string& sStringValue) override;

};


class CDriver_BuRStringValue : public CDriver_BuRValue {
protected:
    uint32_t m_nLength;
public:

    CDriver_BuRStringValue(const std::string& sName, const uint32_t nAddress, const uint32_t nLength);

    //double readValue(LibS7Com::CPLCCommunication * pCommunication);

    ePLCFieldType getFieldType() override;

    uint32_t getLength();

    //void writeToPLCParameters(LibS7Com::CCommandParameters* pCommandParameters, const std::string& sStringValue) override;

};


class CDriver_BuRLRealValue : public CDriver_BuRValue {
protected:
public:

    CDriver_BuRLRealValue(const std::string& sName, const uint32_t nAddress);

    //double readValue(LibS7Com::CPLCCommunication* pCommunication);

    ePLCFieldType getFieldType() override;

    //void writeToPLCParameters(LibS7Com::CCommandParameters* pCommandParameters, const std::string& sStringValue) override;

};

class CDriver_BuRDIntValue : public CDriver_BuRValue {
protected:
public:

    CDriver_BuRDIntValue(const std::string& sName, const uint32_t nAddress);

    //int32_t readValue(LibS7Com::CPLCCommunication* pCommunication);

    ePLCFieldType getFieldType() override;

    //void writeToPLCParameters(LibS7Com::CCommandParameters* pCommandParameters, const std::string& sStringValue) override;

};


class CDriver_BuRIntValue : public CDriver_BuRValue {
protected:
public:

    CDriver_BuRIntValue(const std::string& sName, const uint32_t nAddress);

    //int32_t readValue(LibS7Com::CPLCCommunication* pCommunication);

    ePLCFieldType getFieldType() override;

    //void writeToPLCParameters(LibS7Com::CCommandParameters* pCommandParameters, const std::string& sStringValue) override;

};



class CDriver_BuRBoolValue : public CDriver_BuRValue {
protected:

public:

    CDriver_BuRBoolValue(const std::string& sName, const uint32_t nAddress);

    //bool readValue(LibS7Com::CPLCCommunication* pCommunication);

    ePLCFieldType getFieldType() override;

    //void writeToPLCParameters(LibS7Com::CCommandParameters* pCommandParameters, const std::string& sStringValue) override;

};


typedef std::shared_ptr<CDriver_BuRValue> PDriver_BuRValue;

class CDriver_BuRCommandParameter {
private:
    std::string m_sName;
    std::string m_sDescription;
    std::string m_sField;
public:
    CDriver_BuRCommandParameter(const std::string& sName, const std::string& sDescription, const std::string& sField);

    std::string getName();
    std::string getDescription();
    std::string getField();
};

typedef std::shared_ptr<CDriver_BuRCommandParameter> PDriver_BuRCommandParameter;


class CDriver_BuRCommand {
protected:
    std::string m_sName;
    uint32_t m_nCommandID;
    std::map<std::string, PDriver_BuRCommandParameter> m_Parameters;

public:

    CDriver_BuRCommand(const std::string& sName, const uint32_t nCommandID);
    virtual ~CDriver_BuRCommand();

    std::string getName();
    uint32_t getCommandID();

    PDriver_BuRCommandParameter addParameter(const std::string& sName, const std::string& sDescription, const std::string& sField);
    std::list <std::string> getParameterNames();
    CDriver_BuRCommandParameter* findParameter(const std::string& sName);
};

typedef std::shared_ptr<CDriver_BuRCommand> PDriver_BuRCommand;



class CDriver_BuR : public virtual IDriver_BuR, public virtual CDriver {
private:

protected:
    LibMCEnv::PDriverEnvironment m_pDriverEnvironment;
    std::string m_sName;

    uint32_t m_nWorkerThreadCount;
    uint32_t m_nMaxReceiveBufferSize;

    uint32_t m_nMajorVersion;
    uint32_t m_nMinorVersion;
    uint32_t m_nPatchVersion;
    uint32_t m_nBuildVersion;

    uint32_t m_nMaxPacketQueueSize;
    uint32_t m_nSequenceID;

    std::shared_ptr<brynet::net::TcpService> m_pTcpService;
    std::shared_ptr <brynet::net::AsyncConnector> m_pAsyncConnector;    

    std::map<uint32_t, PDriver_BuRSendInfo> m_SentPacketQueue;

    std::mutex m_ConnectionMutex;

    void enterCallback(const std::shared_ptr <brynet::net::TcpConnection> session, const std::string & sIPAddress, const uint32_t nPort);
    void failedCallback();

    std::weak_ptr<brynet::net::TcpConnection> m_pCurrentConnection;

    std::list<PDriver_BuRValue> m_DriverParameters;
    std::map<std::string, PDriver_BuRValue> m_DriverParameterMap;

    std::map<std::string, PDriver_BuRCommand> m_CommandDefinitions;
    std::map<std::string, PDriver_BuRValue> m_ControlParameterMap;

    void sendSimpleCommandToPLC (std::shared_ptr<brynet::net::TcpConnection> pConnection, uint32_t nCommandID);
    void sendCommandToPLC(std::shared_ptr<brynet::net::TcpConnection> pConnection, uint32_t nCommandID, sAMCFToPLCPacketPayload payLoad);

    void handlePacket (PDriver_BuRPacket pPacket, uint32_t nSequenceID, uint32_t nClientID);

public:

    CDriver_BuR (const std::string & sName, LibMCEnv::PDriverEnvironment pDriverEnvironment);

    void Configure(const std::string& sConfigurationString) override;

    std::string GetName() override;

    std::string GetType() override;

    void GetVersion(LibMCDriver_BuR_uint32& nMajor, LibMCDriver_BuR_uint32& nMinor, LibMCDriver_BuR_uint32& nMicro, std::string& sBuild) override;

    void GetHeaderInformation(std::string& sNameSpace, std::string& sBaseName) override;

    void QueryParameters() override;

    void Connect(const std::string& sIPAddress, const LibMCDriver_BuR_uint32 nPort, const LibMCDriver_BuR_uint32 nTimeout) override;

    void Disconnect() override;

};

} // namespace Impl
} // namespace LibMCDriver_BuR

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_BUR_DRIVER_BUR
