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


Abstract: This is the class declaration of CDriver_S7Net

*/


#ifndef __LIBMCDRIVER_S7NET_DRIVER_S7NET
#define __LIBMCDRIVER_S7NET_DRIVER_S7NET

#include "libmcdriver_s7net_interfaces.hpp"
#include <string>
#include <map>

// Parent classes
#include "libmcdriver_s7net_driver.hpp"
#include "libmcdriver_s7net_plccommand.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "libs7com_dynamic.hpp"
#include "libs7net_dynamic.hpp"


namespace LibMCDriver_S7Net {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CDriver_S7Net 
**************************************************************************************************************************/


class CDriver_S7Value {
protected:
    std::string m_sName;
    uint32_t m_nAddress;
public:

    CDriver_S7Value(const std::string& sName, const uint32_t nAddress);
    virtual ~CDriver_S7Value();

    std::string getName(); 
    uint32_t getAddress();

    virtual ePLCFieldType getFieldType () = 0;

    virtual void writeToPLCParameters (LibS7Com::CCommandParameters * pCommandParameters, const std::string & sStringValue) = 0;

};


class CDriver_S7RealValue : public CDriver_S7Value {
protected:
public:

    CDriver_S7RealValue(const std::string& sName, const uint32_t nAddress);

    double readValue(LibS7Com::CPLCCommunication * pCommunication);

    ePLCFieldType getFieldType() override;

    void writeToPLCParameters(LibS7Com::CCommandParameters* pCommandParameters, const std::string& sStringValue) override;

};

class CDriver_S7LRealValue : public CDriver_S7Value {
protected:
public:

    CDriver_S7LRealValue(const std::string& sName, const uint32_t nAddress);

    double readValue(LibS7Com::CPLCCommunication* pCommunication);

    ePLCFieldType getFieldType() override;

    void writeToPLCParameters(LibS7Com::CCommandParameters* pCommandParameters, const std::string& sStringValue) override;

};

class CDriver_S7DIntValue : public CDriver_S7Value {
protected:
public:

    CDriver_S7DIntValue(const std::string& sName, const uint32_t nAddress);

    int32_t readValue(LibS7Com::CPLCCommunication* pCommunication);

    ePLCFieldType getFieldType() override;

    void writeToPLCParameters(LibS7Com::CCommandParameters* pCommandParameters, const std::string& sStringValue) override;

};


class CDriver_S7IntValue : public CDriver_S7Value {
protected:
public:

    CDriver_S7IntValue(const std::string& sName, const uint32_t nAddress);

    int32_t readValue(LibS7Com::CPLCCommunication* pCommunication);

    ePLCFieldType getFieldType() override;

    void writeToPLCParameters(LibS7Com::CCommandParameters* pCommandParameters, const std::string& sStringValue) override;

};



class CDriver_S7BoolValue : public CDriver_S7Value {
protected:

    uint32_t m_nBit;

public:

    CDriver_S7BoolValue(const std::string& sName, const uint32_t nAddress, const uint32_t nBit);

    bool readValue(LibS7Com::CPLCCommunication* pCommunication);

    ePLCFieldType getFieldType() override;

    uint32_t getBit();

    void writeToPLCParameters(LibS7Com::CCommandParameters* pCommandParameters, const std::string& sStringValue) override;

};

class CDriver_S7StringValue : public CDriver_S7Value {
protected:

    uint32_t m_nLength;

public:

    CDriver_S7StringValue(const std::string& sName, const uint32_t nAddress, const uint32_t nLength);

    std::string readValue(LibS7Com::CPLCCommunication* pCommunication);

    ePLCFieldType getFieldType() override;

    void writeToPLCParameters(LibS7Com::CCommandParameters* pCommandParameters, const std::string& sStringValue) override;

};


typedef std::shared_ptr<CDriver_S7Value> PDriver_S7Value;

class CDriver_S7CommandParameter {
private:
    std::string m_sName;
    std::string m_sDescription;
    std::string m_sField;
public:
    CDriver_S7CommandParameter(const std::string& sName, const std::string& sDescription, const std::string& sField);

    std::string getName();
    std::string getDescription();
    std::string getField();
};

typedef std::shared_ptr<CDriver_S7CommandParameter> PDriver_S7CommandParameter;


class CDriver_S7Command {
protected:
    std::string m_sName;
    uint32_t m_nCommandID;
    std::map<std::string, PDriver_S7CommandParameter> m_Parameters;

public:

    CDriver_S7Command(const std::string& sName, const uint32_t nCommandID);
    virtual ~CDriver_S7Command();

    std::string getName();
    uint32_t getCommandID();

    PDriver_S7CommandParameter addParameter(const std::string& sName, const std::string& sDescription, const std::string& sField);
    std::list <std::string> getParameterNames();
    CDriver_S7CommandParameter* findParameter(const std::string& sName);
};

typedef std::shared_ptr<CDriver_S7Command> PDriver_S7Command;


class CDriver_S7Net : public virtual IDriver_S7Net, public virtual CDriver {
private:

protected:

    LibMCEnv::PDriverEnvironment m_pDriverEnvironment;
    LibMCEnv::PWorkingDirectory m_pWorkingDirectory;

    std::string m_sDriverType;
    std::string m_sProtocolXML;

    LibS7Net::PWrapper m_pPLCWrapper;
    LibS7Net::PPLC m_pPLC;
    LibS7Com::PWrapper m_pCommunicationWrapper;
    LibS7Com::PPLCCommunication m_pCommunication;

    uint32_t m_nPLCtoAMC_DBNo;
    uint32_t m_nPLCtoAMC_DBSize;

    std::list<PDriver_S7Value> m_DriverParameters;
    std::map<std::string, PDriver_S7Value> m_DriverParameterMap;

    std::map<std::string, PDriver_S7Command> m_CommandDefinitions;
    std::map<std::string, PDriver_S7Value> m_ControlParameterMap;

    void updateParameters ();

    uint32_t findPLCToAMCIntOffset (const std::string & sName);
    uint32_t findPLCToAMCDIntOffset(const std::string& sName);
    uint32_t findPLCToAMCStringOffset(const std::string& sName);
    uint32_t findAMCToPLCIntOffset(const std::string& sName);
    uint32_t findAMCToPLCDIntOffset(const std::string& sName);
    uint32_t findAMCToPLCStringOffset(const std::string& sName);

public:

    CDriver_S7Net(const std::string& sName, const std::string& sType, LibMCEnv::PDriverEnvironment pDriverEnvironment);
    virtual ~CDriver_S7Net();

    void Configure(const std::string& sConfigurationString) override;

    void Connect(const LibMCDriver_S7Net::eS7CPUType eCPUType, const std::string& sIPAddress, const LibMCDriver_S7Net_uint32 nRack, const LibMCDriver_S7Net_uint32 nSlot) override;

	void Disconnect() override;

	void QueryParameters() override;

    IPLCCommand* CreateCommand(const std::string& sCommand) override;

    void ExecuteCommand(IPLCCommand* pPLCCommand) override;

    bool WaitForCommand(IPLCCommand* pPLCCommand, const LibMCDriver_S7Net_uint32 nReactionTimeInMS, const LibMCDriver_S7Net_uint32 nWaitForTimeInMS) override;

};

} // namespace Impl
} // namespace LibMCDriver_S7Net

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_S7NET_DRIVER_S7NET
