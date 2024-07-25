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


// Include custom headers here.
#include "pugixml.hpp"

#include "libmcdriver_bur_driver_bur.hpp"
#include "libmcdriver_bur_plccommand.hpp"
#include "libmcdriver_bur_plccommandlist.hpp"
#include "libmcdriver_bur_interfaceexception.hpp"
#include "crcpp/CRC.h"
#include <random>

using namespace LibMCDriver_BuR::Impl;

#define BURDRIVER_PROTOCOLVERSION_LEGACY "http://schemas.autodesk.com/amc/burprotocol/2021/01"
#define BURDRIVER_PROTOCOLVERSION_VERSION3 "http://schemas.autodesk.com/amc/burprotocol/2023/06"

/*************************************************************************************************************************
 Class definition of CDriver_BuR 
**************************************************************************************************************************/
#define __STRINGIZE(x) #x
#define __STRINGIZE_VALUE_OF(x) __STRINGIZE(x)


PDriver_BuRValue readParameterFromXMLNode(pugi::xml_node & xmlNode)
{
    PDriver_BuRValue pValue;

    std::string sChildName = xmlNode.name();
    if ((sChildName == "bool") || (sChildName == "real") || (sChildName == "lreal") || (sChildName == "dint") || (sChildName == "int")) {

        auto nameAttrib = xmlNode.attribute("name");
        auto addressAttrib = xmlNode.attribute("address");
        auto descriptionAttrib = xmlNode.attribute("description");

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
            pValue = std::make_shared<CDriver_BuRBoolValue>(sName, sDescription, nAddress);
        }

        if (sChildName == "string") {
            auto lengthAttrib = xmlNode.attribute("length");
            if (lengthAttrib.empty())
                throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_NOLENGTHATTRIBUTE);
            uint32_t nLength = lengthAttrib.as_uint(0xffff);
            if (nLength >= 65536)
                throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_INVALIDLENGTHATTRIBUTE);
            
            pValue = std::make_shared<CDriver_BuRStringValue>(sName, sDescription, nAddress, nLength);
        }

        if (sChildName == "real") {            
            pValue = std::make_shared<CDriver_BuRRealValue>(sName, sDescription, nAddress);
        }

        if (sChildName == "lreal") {
            pValue = std::make_shared<CDriver_BuRLRealValue>(sName, sDescription, nAddress);
        }

        if (sChildName == "dint") {
            pValue = std::make_shared<CDriver_BuRDIntValue>(sName, sDescription, nAddress);
        }

        if (sChildName == "int") {
            pValue = std::make_shared<CDriver_BuRIntValue>(sName, sDescription, nAddress);
        }

    }

    return pValue;

}

CDriver_BuR::CDriver_BuR(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment)
    :  m_sName(sName),
       m_pDriverEnvironment (pDriverEnvironment), 
       m_nWorkerThreadCount (1), 
       m_ProtocolVersion (eDriver_BurProtocolVersion::Unknown),
       m_nPacketSignature (0),
       m_nMajorVersion (0),
       m_nMinorVersion (0),
       m_nBuildVersion (0),
       m_nPatchVersion (0),
       m_nMaxReceiveBufferSize (1024*1024),
       m_bIsQueryingParameters (false),
       m_SimulationMode (false),
       m_nMaxPacketQueueSize(1024),
       m_InitMachineCommandID(DRIVER_BUR_DEFAULT_INITMACHINECOMMANDID)

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

    
    pugi::xml_node burprotocolNode = doc.child("driverconfiguration");
    // Depreciated fallback: root Node was called "burprotocol" in earlier versions.
    // Going forward, the driver configurations should all be "driverconfiguration"
    if (burprotocolNode.empty())
        burprotocolNode = doc.child("burprotocol");

    if (burprotocolNode.empty())
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_INVALIDDRIVERPROTOCOL);

    auto xmlNSAttribute = burprotocolNode.attribute ("xmlns");
    std::string sXMLNS = xmlNSAttribute.as_string ();
    if (sXMLNS == BURDRIVER_PROTOCOLVERSION_VERSION3) {
        m_ProtocolVersion = eDriver_BurProtocolVersion::Version3;

        pugi::xml_node protocolNode = burprotocolNode.child("protocol");
        if (protocolNode.empty())
            throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_NOTCPPROTOCOLNODE);

        pugi::xml_attribute packetSignatureAttrib = protocolNode.attribute("packetsignature");
        m_nPacketSignature = packetSignatureAttrib.as_uint(0);               
        if (m_nPacketSignature == 0)
            throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_INVALIDPACKETSIGNATURE);

    }
    
    if (sXMLNS == BURDRIVER_PROTOCOLVERSION_LEGACY) {
        m_ProtocolVersion = eDriver_BurProtocolVersion::Legacy;

        m_nPacketSignature = PACKET_SIGNATURE_LEGACY;

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

    }

    if (m_ProtocolVersion == eDriver_BurProtocolVersion::Unknown)
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_UNKNOWNDRIVERPROTOCOLVERSION);

    pugi::xml_node machineStatusNode = burprotocolNode.child("machinestatus");
    if (machineStatusNode.empty())
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_NOMACHINESTATUSDEFINITION);

    auto statusNodes = machineStatusNode.children();
    for (pugi::xml_node childNode : statusNodes)
    {
        auto pValue = readParameterFromXMLNode(childNode);

        if (dynamic_cast<CDriver_BuRBoolValue*> (pValue.get()) != nullptr)
            m_pDriverEnvironment->RegisterBoolParameter(pValue->getName (), pValue->getDescription (), false);

        if (dynamic_cast<CDriver_BuRStringValue*> (pValue.get()) != nullptr)
            m_pDriverEnvironment->RegisterStringParameter(pValue->getName(), pValue->getDescription(), "");

        if (dynamic_cast<CDriver_BuRLRealValue*> (pValue.get()) != nullptr)
            m_pDriverEnvironment->RegisterDoubleParameter(pValue->getName(), pValue->getDescription(), 0.0);

        if (dynamic_cast<CDriver_BuRRealValue*> (pValue.get()) != nullptr)
            m_pDriverEnvironment->RegisterDoubleParameter(pValue->getName(), pValue->getDescription(), 0.0);

        if (dynamic_cast<CDriver_BuRIntValue*> (pValue.get()) != nullptr)
            m_pDriverEnvironment->RegisterIntegerParameter(pValue->getName(), pValue->getDescription(), 0);

        if (dynamic_cast<CDriver_BuRDIntValue*> (pValue.get()) != nullptr)
            m_pDriverEnvironment->RegisterIntegerParameter(pValue->getName(), pValue->getDescription(), 0);

        m_DriverParameters.push_back(pValue);
        m_DriverParameterMap.insert(std::make_pair(pValue->getName(), pValue));

    }

    pugi::xml_node commandsNode = burprotocolNode.child("commands");
    if (commandsNode.empty())
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_NOCOMMANDSLIST);

    auto initMachineAttrib = commandsNode.attribute("initmachine");
    if (!initMachineAttrib.empty())
        m_InitMachineCommandID = initMachineAttrib.as_uint();

    auto commandNodes = commandsNode.children("command");
    for (pugi::xml_node commandNode : commandNodes)
    {

        auto nameAttrib = commandNode.attribute("name");
        auto cmdIdAttrib = commandNode.attribute("id");

        std::string sName = nameAttrib.as_string();
        if (sName.empty ())
            throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_COMMANDNAMEMISSING);

        if (cmdIdAttrib.empty ())
            throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_COMMANDIDMISSING);

        uint32_t nCommandID = cmdIdAttrib.as_int(0);
        if ((nCommandID < BUR_MINCUSTOMCOMMANDID) || (nCommandID > BUR_MAXCUSTOMCOMMANDID))
            throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_INVALIDCOMMANDID);

        auto pDefinition = std::make_shared<CDriver_BuRCommandDefinition>(sName, nCommandID);
        m_CommandDefinitions.insert(std::make_pair (sName, pDefinition));

        auto parametersNodes = commandNode.children();
        for (pugi::xml_node parameterNode : parametersNodes)
        {
            auto pValue = readParameterFromXMLNode(parameterNode);
            pDefinition->addParameter(pValue);
        }


    }

    m_pConnector = std::make_shared<CDriver_BuRConnector>(m_nWorkerThreadCount, m_nMaxReceiveBufferSize, m_nMajorVersion, m_nMinorVersion, m_nPatchVersion, m_nBuildVersion, m_nMaxPacketQueueSize, m_ProtocolVersion, m_nPacketSignature); 


}

void CDriver_BuR::SetToSimulationMode()
{
    m_SimulationMode = true;
}

bool CDriver_BuR::IsSimulationMode()
{
    return m_SimulationMode;
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
    nMajor = LIBMCDRIVER_BUR_VERSION_MAJOR;
    nMinor = LIBMCDRIVER_BUR_VERSION_MINOR;
    nMicro = LIBMCDRIVER_BUR_VERSION_MICRO;
    sBuild = __STRINGIZE_VALUE_OF(__GITHASH);
}

void CDriver_BuR::QueryParameters()
{
    QueryParametersEx(m_pDriverEnvironment->CreateStatusUpdateSession());
}

void CDriver_BuR::QueryParametersEx(LibMCEnv::PDriverStatusUpdateSession pDriverUpdateInstance)
{

    if (pDriverUpdateInstance.get() == nullptr)
        return;

    if (m_bIsQueryingParameters)
        return;   

    if (!m_SimulationMode) {

        if (m_pConnector.get() != nullptr) {
            m_bIsQueryingParameters = true;

            m_pConnector->queryParameters(
                [this, pDriverUpdateInstance](CDriver_BuRPacket* pPacket) {

                m_bIsQueryingParameters = false;


                for (auto driverParameter : m_DriverParameters) {
                    std::lock_guard<std::mutex> driverLock(m_driverEnvironmentMutex);
                    auto sName = driverParameter->getName();

                    if (dynamic_cast<CDriver_BuRDIntValue*> (driverParameter.get()) != nullptr) {
                        pDriverUpdateInstance->SetIntegerParameter(sName, pPacket->readUInt32(driverParameter->getAddress()));
                    }

                    if (dynamic_cast<CDriver_BuRIntValue*> (driverParameter.get()) != nullptr) {
                        pDriverUpdateInstance->SetIntegerParameter(sName, pPacket->readUInt16(driverParameter->getAddress()));
                    }

                    if (dynamic_cast<CDriver_BuRRealValue*> (driverParameter.get()) != nullptr) {
                        pDriverUpdateInstance->SetDoubleParameter(sName, pPacket->readFloat(driverParameter->getAddress()));
                    }

                    if (dynamic_cast<CDriver_BuRLRealValue*> (driverParameter.get()) != nullptr) {
                        pDriverUpdateInstance->SetDoubleParameter(sName, pPacket->readDouble(driverParameter->getAddress()));
                    }

                    if (dynamic_cast<CDriver_BuRBoolValue*> (driverParameter.get()) != nullptr) {
                        pDriverUpdateInstance->SetBoolParameter(sName, pPacket->readBool(driverParameter->getAddress()));
                    }

                    auto pStringValue = dynamic_cast<CDriver_BuRStringValue*> (driverParameter.get());
                    if (pStringValue != nullptr) {
                        pDriverUpdateInstance->SetStringParameter(sName, pPacket->readString(driverParameter->getAddress(), pStringValue->getLength()));
                    }

                }


            });
        }
    }

}



void CDriver_BuR::Connect(const std::string& sIPAddress, const LibMCDriver_BuR_uint32 nPort, const LibMCDriver_BuR_uint32 nTimeout)
{        
    if (!m_SimulationMode) {

        if (m_pConnector.get() != nullptr)
            m_pConnector->connect(sIPAddress, nPort, nTimeout);

    }
}

void CDriver_BuR::Disconnect()
{
    if (m_pConnector.get() != nullptr)
        m_pConnector->disconnect(); 
}

void CDriver_BuR::ReinitializeMachine()
{
    if (!m_SimulationMode) {
        if (m_pConnector.get() == nullptr)
            throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_NOTCONNECTED);

        bool resetSuccessful = false;
        
        m_pConnector->sendCommandToPLC(m_InitMachineCommandID, [&resetSuccessful](CDriver_BuRPacket* pPacket) {
            resetSuccessful = true;
        });

        if (!resetSuccessful)
            throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_COULDNOTREINITIALIZEMACHINE);
    }
}



IPLCCommandList* CDriver_BuR::CreateCommandList()
{
    return new CPLCCommandList(m_pConnector, this, m_SimulationMode);
    
}


IPLCCommand* CDriver_BuR::CreateCommand(const std::string& sCommandName)
{
    auto iIter = m_CommandDefinitions.find(sCommandName);
    if (iIter == m_CommandDefinitions.end())
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_COMMANDDEFINITIONNOTFOUND, "command definition not found: " + sCommandName);

    auto pCommandDefinition = iIter->second;

    return new CPLCCommand(pCommandDefinition.get()); 
} 


void CDriver_BuR::StartJournaling()
{
}

void CDriver_BuR::StopJournaling()
{

}

void CDriver_BuR::RefreshJournal()
{
/*    if (m_pConnector.get() != nullptr)
        m_pConnector->refreshJournal(); */

}

uint64_t CDriver_BuR::generateTimeStamp()
{
    std::lock_guard<std::mutex> driverLock(m_driverEnvironmentMutex);
    return m_pDriverEnvironment->GetGlobalTimerInMilliseconds();

}