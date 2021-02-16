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


Abstract: This is a stub class definition of CDriver_S7Net

*/

#include "libmcdriver_s7net_driver_s7net.hpp"
#include "libmcdriver_s7net_interfaceexception.hpp"
#include "libmcdriver_s7net_plccommand.hpp"
#include "libs7com_abi.hpp"
#include "libs7com_interfaces.hpp"

// Include custom headers here.
#include "pugixml.hpp"

using namespace LibMCDriver_S7Net::Impl;


CDriver_S7Value::CDriver_S7Value(const std::string& sName, const uint32_t nAddress)
    : m_sName (sName), m_nAddress (nAddress)
{

}

CDriver_S7Value::~CDriver_S7Value()
{

}

std::string CDriver_S7Value::getName()
{
    return m_sName;
}


CDriver_S7RealValue::CDriver_S7RealValue(const std::string& sName, const uint32_t nAddress)
    : CDriver_S7Value (sName, nAddress)
{

}

double CDriver_S7RealValue::readValue(LibS7Com::CPLCCommunication* pCommunication)
{
    if (pCommunication == nullptr)
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_INVALIDPARAM);

    return pCommunication->ReadVariableReal(m_nAddress);
}

CDriver_S7LRealValue::CDriver_S7LRealValue(const std::string& sName, const uint32_t nAddress)
    : CDriver_S7Value(sName, nAddress)
{

}

double CDriver_S7LRealValue::readValue(LibS7Com::CPLCCommunication* pCommunication)
{
    if (pCommunication == nullptr)
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_INVALIDPARAM);

    return pCommunication->ReadVariableLReal(m_nAddress);
}


CDriver_S7DIntValue::CDriver_S7DIntValue(const std::string& sName, const uint32_t nAddress)
    : CDriver_S7Value(sName, nAddress)
{

}

int32_t CDriver_S7DIntValue::readValue(LibS7Com::CPLCCommunication* pCommunication)
{
    if (pCommunication == nullptr)
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_INVALIDPARAM);

    return pCommunication->ReadVariableInt32(m_nAddress);
}

CDriver_S7IntValue::CDriver_S7IntValue(const std::string& sName, const uint32_t nAddress)
    : CDriver_S7Value(sName, nAddress)
{

}

int32_t CDriver_S7IntValue::readValue(LibS7Com::CPLCCommunication* pCommunication)
{
    if (pCommunication == nullptr)
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_INVALIDPARAM);

    return pCommunication->ReadVariableInt16(m_nAddress);
}

CDriver_S7BoolValue::CDriver_S7BoolValue(const std::string& sName, const uint32_t nAddress, const uint32_t nBit)
    : CDriver_S7Value (sName, nAddress), m_nBit (nBit)
{
}

bool CDriver_S7BoolValue::readValue(LibS7Com::CPLCCommunication* pCommunication)
{
    if (pCommunication == nullptr)
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_INVALIDPARAM);

    return pCommunication->ReadVariableBool(m_nAddress, m_nBit);

}



CDriver_S7Command::CDriver_S7Command(const std::string& sName, const uint32_t nCommandID)
    : m_sName (sName), m_nCommandID (nCommandID)
{

}

CDriver_S7Command::~CDriver_S7Command()
{

}

std::string CDriver_S7Command::getName()
{
    return m_sName;
}

uint32_t CDriver_S7Command::getCommandID()
{
    return m_nCommandID;
}

/*************************************************************************************************************************
 Class definition of CDriver_S7Net 
**************************************************************************************************************************/

CDriver_S7Net::CDriver_S7Net(const std::string& sName, const std::string& sType, LibMCEnv::PDriverEnvironment pDriverEnvironment)
    : CDriver (sName, sType), m_pDriverEnvironment (pDriverEnvironment), m_nAMCtoPLC_DBNo (0), m_nPLCtoAMC_DBNo (0),  m_nPLCtoAMC_DBSize (0)
{
    if (pDriverEnvironment.get() == nullptr)
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_INVALIDPARAM);


}

CDriver_S7Net::~CDriver_S7Net()
{
    m_pCommunication = nullptr;
    m_pCommunicationWrapper = nullptr;
    m_pPLC = nullptr;
    m_pPLCWrapper = nullptr;
    m_pWorkingDirectory = nullptr;
}


void CDriver_S7Net::Configure(const std::string& sConfigurationString)
{
    m_pCommunication = nullptr;
    m_pCommunicationWrapper = nullptr;
    m_pPLC = nullptr;
    m_pPLCWrapper = nullptr;
    m_pWorkingDirectory = nullptr;

    if (sConfigurationString.length() == 0)
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_INVALIDDRIVERPROTOCOL);

    m_sProtocolXML = sConfigurationString;

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(m_sProtocolXML.c_str());
    if (!result)
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_COULDNOTPARSEDRIVERPROTOCOL);

    pugi::xml_node s7protocolNode = doc.child("s7protocol");
    if (s7protocolNode.empty())
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_INVALIDDRIVERPROTOCOL);

    pugi::xml_node statusdbNode = s7protocolNode.child("statusdb");
    if (statusdbNode.empty())
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_NOSTATUSDBDEFINITION);

    pugi::xml_attribute statusnumberAttrib = statusdbNode.attribute("number");
    if (statusnumberAttrib.empty())
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_NOSTATUSDBNUMBER);

    m_nPLCtoAMC_DBNo = statusnumberAttrib.as_uint(0);
    if (m_nPLCtoAMC_DBNo == 0)
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_INVALIDSTATUSDBNUMBER);

    pugi::xml_attribute sizeAttrib = statusdbNode.attribute("size");
    if (sizeAttrib.empty())
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_NOSTATUSDBSIZE);

    m_nPLCtoAMC_DBSize = sizeAttrib.as_uint(0);
    if (m_nPLCtoAMC_DBSize == 0)
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_INVALIDSTATUSDBSIZE);

    auto statusNodes = statusdbNode.children();
    for (pugi::xml_node childNode : statusNodes)
    {
        std::string sChildName = childNode.name();
        if ((sChildName == "bool") || (sChildName == "real") || (sChildName == "lreal") || (sChildName == "dint") || (sChildName == "int")) {

            auto nameAttrib = childNode.attribute("name");
            auto addressAttrib = childNode.attribute("address");
            auto descriptionAttrib = childNode.attribute("description");

            if (nameAttrib.empty())
                throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_NONAMEATTRIBUTE);
            if (addressAttrib.empty())
                throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_NOADDRESSEATTRIBUTE);
            if (descriptionAttrib.empty())
                throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_NODESCRIPTIONATTRIBUTE);

            std::string sName = nameAttrib.as_string();
            uint32_t nAddress = addressAttrib.as_uint(0);
            std::string sDescription = descriptionAttrib.as_string();

            if (sName.empty())
                throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_INVALIDNAMEATTRIBUTE);
            if (nAddress == 0)
                throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_INVALIDADDRESSEATTRIBUTE);
            if (sDescription.empty())
                throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_INVALIDDESCRIPTIONATTRIBUTE);

            if (sChildName == "bool") {
                auto bitAttrib = childNode.attribute("bit");
                if (bitAttrib.empty())
                    throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_NOBITATTRIBUTE);
                uint32_t nBit = bitAttrib.as_uint(0xffff);
                if (nBit >= 8)
                    throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_INVALIDBITATTRIBUTE);

                m_pDriverEnvironment->RegisterBoolParameter(sName, sDescription, false);
                m_DriverParameters.push_back(std::make_shared<CDriver_S7BoolValue>(sName, nAddress, nBit));
            }

            if (sChildName == "real") {
                m_pDriverEnvironment->RegisterDoubleParameter(sName, sDescription, 0.0);
                m_DriverParameters.push_back(std::make_shared<CDriver_S7RealValue>(sName, nAddress));
            }

            if (sChildName == "lreal") {
                m_pDriverEnvironment->RegisterDoubleParameter(sName, sDescription, 0.0);
                m_DriverParameters.push_back(std::make_shared<CDriver_S7LRealValue>(sName, nAddress));
            }

            if (sChildName == "dint") {
                m_pDriverEnvironment->RegisterIntegerParameter(sName, sDescription, 0);
                m_DriverParameters.push_back(std::make_shared<CDriver_S7DIntValue>(sName, nAddress));
            }

            if (sChildName == "int") {
                m_pDriverEnvironment->RegisterIntegerParameter(sName, sDescription, 0);
                m_DriverParameters.push_back(std::make_shared<CDriver_S7IntValue>(sName, nAddress));
            }

        }
    }

    pugi::xml_node controldbNode = s7protocolNode.child("controldb");
    if (controldbNode.empty())
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_NOCONTROLDBDEFINITION);

    pugi::xml_attribute controlnumberAttrib = controldbNode.attribute("number");
    if (controlnumberAttrib.empty())
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_NOCONTROLDBNUMBER);

    m_nAMCtoPLC_DBNo = controlnumberAttrib.as_uint(0);
    if (m_nAMCtoPLC_DBNo == 0)
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_INVALIDCONTROLDBNUMBER);


    auto commandNodes = controldbNode.children("command");
    for (pugi::xml_node commandNode : commandNodes) {
        auto commandNameAttrib = commandNode.attribute("name");
        auto commandIDAttrib = commandNode.attribute("cmdid");

        if (commandNameAttrib.empty())
            throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_NOCOMMANDNAME);
        if (commandIDAttrib.empty())
            throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_NOCOMMANDID);

        std::string sName = commandNameAttrib.as_string();
        uint32_t nCmdID = commandIDAttrib.as_uint(0);

        if (sName.empty())
            throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_INVALIDCOMMANDNAME);
        if (nCmdID == 0)
            throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_INVALIDCOMMANDID);

        auto pS7Command = std::make_shared<CDriver_S7Command>(sName, nCmdID);

        m_CommandDefinitions.insert(std::make_pair(sName, pS7Command));
    }


    m_pWorkingDirectory = m_pDriverEnvironment->CreateWorkingDirectory();
    auto pLibS7NetDLL = m_pWorkingDirectory->StoreDriverData("libs7net.dll", "libs7net");
    auto pLibS7NetCOMHost = m_pWorkingDirectory->StoreDriverData("libs7net_managed.comhost.dll", "libs7net_managed.comhost");
    m_pWorkingDirectory->StoreDriverData("libs7net_managed.dll", "libs7net_managed");
    m_pWorkingDirectory->StoreDriverData("libs7net.dll", "libs7net");
    m_pWorkingDirectory->StoreDriverData("libs7net_managed.runtimeconfig.json", "libs7net_managed.runtimeconfig");
    m_pWorkingDirectory->StoreDriverData("libs7net_managed.runtimeconfig.dev.json", "libs7net_managed.runtimeconfig.dev");

    m_pPLCWrapper = LibS7Net::CWrapper::loadLibrary (pLibS7NetDLL->GetAbsoluteFileName());
    m_pPLC = m_pPLCWrapper->CreatePLC(pLibS7NetCOMHost->GetAbsoluteFileName());

    m_pCommunicationWrapper = LibS7Com::CWrapper::loadLibraryFromSymbolLookupMethod((void*) &LibS7Com::Impl::LibS7Com_GetProcAddress);
    m_pCommunicationWrapper->InjectComponent("LibS7Net", m_pPLCWrapper->GetSymbolLookupMethod());
    m_pCommunication = m_pCommunicationWrapper->CreatePLCCommunication();
    m_pCommunication->SetProtocolConfiguration(m_nPLCtoAMC_DBNo, m_nPLCtoAMC_DBSize, m_nAMCtoPLC_DBNo);
    
}

void CDriver_S7Net::Connect(const LibMCDriver_S7Net::eS7CPUType eCPUType, const std::string& sIPAddress, const LibMCDriver_S7Net_uint32 nRack, const LibMCDriver_S7Net_uint32 nSlot)
{
    if (m_pPLC.get() == nullptr)
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_DRIVERNOTINITIALISED);
    if (m_pCommunication.get() == nullptr)
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_DRIVERNOTINITIALISED);

    LibS7Net::eCPUType eS7NetType;
    switch (eCPUType) {
    case LibMCDriver_S7Net::eS7CPUType::S71200:
        eS7NetType = LibS7Net::eCPUType::S71200;
        break;
    case LibMCDriver_S7Net::eS7CPUType::S71500:
        eS7NetType = LibS7Net::eCPUType::S71500;
        break;
    case LibMCDriver_S7Net::eS7CPUType::S7200:
        eS7NetType = LibS7Net::eCPUType::S7200;
        break;
    case LibMCDriver_S7Net::eS7CPUType::S7300:
        eS7NetType = LibS7Net::eCPUType::S7300;
        break;
    case LibMCDriver_S7Net::eS7CPUType::S7400:
        eS7NetType = LibS7Net::eCPUType::S7400;
        break;
    default:
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_INVALIDCPUTYPE);
    }

    m_pPLC->Connect(eS7NetType, sIPAddress, nRack, nSlot);

    m_pCommunication->StartCommunication(m_pPLC);
}


void CDriver_S7Net::Disconnect()
{	
    if (m_pCommunication.get () != nullptr)
        m_pCommunication->StopCommunication();

}

void CDriver_S7Net::updateParameters()
{
    if (m_pCommunication.get() != nullptr) {
        for (auto pParameter : m_DriverParameters) {
            auto sName = pParameter->getName();

            auto pRealParameter = std::dynamic_pointer_cast<CDriver_S7RealValue> (pParameter);
            if (pRealParameter.get() != nullptr) {
                double dValue = pRealParameter->readValue(m_pCommunication.get());
                m_pDriverEnvironment->SetDoubleParameter(sName, dValue);
            }

            auto pLRealParameter = std::dynamic_pointer_cast<CDriver_S7LRealValue> (pParameter);
            if (pLRealParameter.get() != nullptr) {
                double dValue = pRealParameter->readValue(m_pCommunication.get());
                m_pDriverEnvironment->SetDoubleParameter(sName, dValue);
            }

            auto pBoolParameter = std::dynamic_pointer_cast<CDriver_S7BoolValue> (pParameter);
            if (pBoolParameter.get() != nullptr) {
                bool bValue = pBoolParameter->readValue(m_pCommunication.get());
                m_pDriverEnvironment->SetBoolParameter(sName, bValue);
            }

            auto pDintParameter = std::dynamic_pointer_cast<CDriver_S7DIntValue> (pParameter);
            if (pDintParameter.get() != nullptr) {
                int32_t nValue = pDintParameter->readValue(m_pCommunication.get());
                m_pDriverEnvironment->SetIntegerParameter(sName, nValue);
            }

            auto pIntParameter = std::dynamic_pointer_cast<CDriver_S7IntValue> (pParameter);
            if (pIntParameter.get() != nullptr) {
                int32_t nValue = pIntParameter->readValue(m_pCommunication.get());
                m_pDriverEnvironment->SetIntegerParameter(sName, nValue);
            }
        }
    }
}


void CDriver_S7Net::QueryParameters()
{
    if (m_pCommunication.get() != nullptr) {
        m_pCommunication->RetrieveStatus();

        updateParameters();
    }
}


IPLCCommand* CDriver_S7Net::CreateCommand(const std::string& sCommand)
{
    return new CPLCCommand(sCommand);
}

void CDriver_S7Net::ExecuteCommand(IPLCCommand* pPLCCommand)
{
    if (m_pPLC.get() == nullptr)
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_DRIVERNOTINITIALISED);
    if (m_pCommunication.get() == nullptr)
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_DRIVERNOTINITIALISED);

    if (pPLCCommand == nullptr)
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_INVALIDPARAM);

    CPLCCommand* pPLCCommandInstance = dynamic_cast<CPLCCommand*> (pPLCCommand);
    if (pPLCCommandInstance == nullptr)
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_INVALIDCAST);

    auto nSequenceID = pPLCCommandInstance->getSequenceID();
    if (nSequenceID != 0)
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_COMMANDHASALREADYBEENEXECUTED);

    std::string sCommandName = pPLCCommandInstance->getName();

    auto iIter = m_CommandDefinitions.find(sCommandName);
    if (iIter == m_CommandDefinitions.end ())
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_UNKNOWNCOMMANDNAME, "unknown command name: " + sCommandName);

    auto pCommandDefinition = iIter->second;

    if (m_pCommunication.get() == nullptr)
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_DRIVERNOTCONNECTED);
    nSequenceID = m_pCommunication->ExecuteCommand(pCommandDefinition->getCommandID());

    if (nSequenceID == 0)
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_INVALIDSEQUENCEID);

    pPLCCommandInstance->setSequenceID(nSequenceID);

    this->QueryParameters(); 
}

bool CDriver_S7Net::WaitForCommand(IPLCCommand* pPLCCommand, const LibMCDriver_S7Net_uint32 nReactionTimeInMS, const LibMCDriver_S7Net_uint32 nWaitForTimeInMS)
{
    if (m_pPLC.get() == nullptr)
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_DRIVERNOTINITIALISED);
    if (m_pCommunication.get() == nullptr)
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_DRIVERNOTINITIALISED);

    if (pPLCCommand == nullptr)
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_INVALIDPARAM);

    CPLCCommand* pPLCCommandInstance = dynamic_cast<CPLCCommand*> (pPLCCommand);
    if (pPLCCommandInstance == nullptr)
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_INVALIDCAST);

    auto nSequenceID = pPLCCommandInstance->getSequenceID();
    if (nSequenceID == 0) 
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_COMMANDHASNOTBEENEXECUTED);

    uint64_t nStartTimer = m_pDriverEnvironment->GetGlobalTimerInMilliseconds();

    bool bSequenceHasBeenActive = false;
    bool bWaitTimeOutOccured = false;
    while (!bWaitTimeOutOccured) {
        uint64_t nDeltaTime = 0;
        uint64_t nCurrentTimer = m_pDriverEnvironment->GetGlobalTimerInMilliseconds();
        if (nCurrentTimer >= nStartTimer)
            nDeltaTime = (nCurrentTimer - nStartTimer);

        uint32_t nErrorCode;
        bool bSequenceIsActive = false;
        bool bSequenceIsFinished = false;
        m_pCommunication->CheckCommandExecution(nSequenceID, bSequenceIsActive, bSequenceIsFinished, nErrorCode);

        if (!bSequenceIsActive) {
            if (bSequenceHasBeenActive)
                throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_COMMANDSEQUENCEERROR);

            if (nDeltaTime > nReactionTimeInMS)
                throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_COMMANDREACTIONTIMEOUT);
        }
        else {
            bSequenceHasBeenActive = true;
            if (bSequenceIsFinished)
                return true;
        }

        if (nDeltaTime > nWaitForTimeInMS)
            bWaitTimeOutOccured = true;

    }

    return false;

}
