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


Abstract: This is a stub class definition of CDriver_OPCUA

*/

// Include custom headers here.
#include "libmcdriver_opcua_driver_opcua.hpp"
#include "libmcdriver_opcua_interfaceexception.hpp"

#include "pugixml.hpp"
#include <iostream>

#define __STRINGIZE(x) #x
#define __STRINGIZE_VALUE_OF(x) __STRINGIZE(x)


using namespace LibMCDriver_OPCUA::Impl;

/*************************************************************************************************************************
 Class definition of CDriver_OPCUA 
**************************************************************************************************************************/

CDriver_OPCUA::CDriver_OPCUA(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment)
	: m_sName (sName), m_bSimulationMode (false), m_pDriverEnvironment (pDriverEnvironment), m_nMajorVersion (0), m_nMinorVersion (0), m_nPatchVersion (0)
{
	if (pDriverEnvironment.get() == nullptr)
		throw ELibMCDriver_OPCUAInterfaceException(LIBMCDRIVER_OPCUA_ERROR_INVALIDPARAM);
}


CDriver_OPCUA::~CDriver_OPCUA()
{
    m_pClient = nullptr;
    m_pOpen62541DLL = nullptr;
    m_pWorkingDirectory = nullptr;
}

void CDriver_OPCUA::Configure(const std::string& sConfigurationString)
{
    if (sConfigurationString.length() == 0)
        throw ELibMCDriver_OPCUAInterfaceException(LIBMCDRIVER_OPCUA_ERROR_INVALIDDRIVERPROTOCOL);

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(sConfigurationString.c_str());
    if (!result)
        throw ELibMCDriver_OPCUAInterfaceException(LIBMCDRIVER_OPCUA_ERROR_COULDNOTPARSEDRIVERPROTOCOL);

    pugi::xml_node burprotocolNode = doc.child("opcuaprotocol");
    if (burprotocolNode.empty())
        throw ELibMCDriver_OPCUAInterfaceException(LIBMCDRIVER_OPCUA_ERROR_INVALIDDRIVERPROTOCOL);

    pugi::xml_node versionNode = burprotocolNode.child("version");
    if (versionNode.empty())
        throw ELibMCDriver_OPCUAInterfaceException(LIBMCDRIVER_OPCUA_ERROR_NOVERSIONDEFINITION);

    pugi::xml_attribute majorversionAttrib = versionNode.attribute("major");
    if (majorversionAttrib.empty())
        throw ELibMCDriver_OPCUAInterfaceException(LIBMCDRIVER_OPCUA_ERROR_NOMAJORVERSION);
    m_nMajorVersion = majorversionAttrib.as_uint(0);

    pugi::xml_attribute minorversionAttrib = versionNode.attribute("minor");
    if (minorversionAttrib.empty())
        throw ELibMCDriver_OPCUAInterfaceException(LIBMCDRIVER_OPCUA_ERROR_NOMINORVERSION);
    m_nMinorVersion = minorversionAttrib.as_uint(0);

    pugi::xml_attribute patchversionAttrib = versionNode.attribute("patch");
    if (patchversionAttrib.empty())
        throw ELibMCDriver_OPCUAInterfaceException(LIBMCDRIVER_OPCUA_ERROR_NOPATCHVERSION);
    m_nPatchVersion = patchversionAttrib.as_uint(0);

    pugi::xml_node machineStatusNode = burprotocolNode.child("machinestatus");
    if (machineStatusNode.empty())
        throw ELibMCDriver_OPCUAInterfaceException(LIBMCDRIVER_OPCUA_ERROR_NOMACHINESTATUSDEFINITION);

    /*auto statusNodes = machineStatusNode.children();
    for (pugi::xml_node childNode : statusNodes)
    {
        auto pValue = readParameterFromXMLNode(childNode);

        if (dynamic_cast<CDriver_BuRBoolValue*> (pValue.get()) != nullptr)
            m_pDriverEnvironment->RegisterBoolParameter(pValue->getName(), pValue->getDescription(), false);

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

    } */


    /*pugi::xml_node commandsNode = burprotocolNode.child("commands");
    if (commandsNode.empty())
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_NOCOMMANDSLIST);

    auto commandNodes = commandsNode.children("command");
    for (pugi::xml_node commandNode : commandNodes)
    {

        auto nameAttrib = commandNode.attribute("name");
        auto cmdIdAttrib = commandNode.attribute("id");

        std::string sName = nameAttrib.as_string();
        if (sName.empty())
            throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_COMMANDNAMEMISSING);

        if (cmdIdAttrib.empty())
            throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_COMMANDIDMISSING);

        uint32_t nCommandID = cmdIdAttrib.as_int(0);
        if ((nCommandID < BUR_MINCUSTOMCOMMANDID) || (nCommandID > BUR_MAXCUSTOMCOMMANDID))
            throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_INVALIDCOMMANDID);

        auto pDefinition = std::make_shared<CDriver_BuRCommandDefinition>(sName, nCommandID);
        m_CommandDefinitions.insert(std::make_pair(sName, pDefinition));

        auto parametersNodes = commandNode.children();
        for (pugi::xml_node parameterNode : parametersNodes)
        {
            auto pValue = readParameterFromXMLNode(parameterNode);
            pDefinition->addParameter(pValue);
        }


    } */


    m_pWorkingDirectory = m_pDriverEnvironment->CreateWorkingDirectory();
    m_pOpen62541DLL = m_pWorkingDirectory->StoreDriverData("open62541.dll", "open62541_win64");
    
    auto pSDK = std::make_shared<COpen62541SDK>(m_pOpen62541DLL->GetAbsoluteFileName());
    m_pClient = std::make_shared <COpen62541Client>(pSDK, this);

    m_pClient->connect("localhost:4840");

}

std::string CDriver_OPCUA::GetName()
{
	return m_sName;
}

std::string CDriver_OPCUA::GetType()
{
	return "opcua";
}

void CDriver_OPCUA::GetVersion(LibMCDriver_OPCUA_uint32& nMajor, LibMCDriver_OPCUA_uint32& nMinor, LibMCDriver_OPCUA_uint32& nMicro, std::string& sBuild)
{
	nMajor = LIBMCDRIVER_OPCUA_VERSION_MAJOR;
	nMinor = LIBMCDRIVER_OPCUA_VERSION_MINOR;
	nMicro = LIBMCDRIVER_OPCUA_VERSION_MICRO;
	sBuild = __STRINGIZE_VALUE_OF(__GITHASH);;
}

void CDriver_OPCUA::GetHeaderInformation(std::string& sNameSpace, std::string& sBaseName)
{
	sNameSpace = "LibMCDriver_OPCUA";
	sBaseName = "libmcdriver_opcua";
}

void CDriver_OPCUA::QueryParameters()
{
}


void CDriver_OPCUA::SetToSimulationMode()
{
	m_bSimulationMode = true;
}

bool CDriver_OPCUA::IsSimulationMode()
{
	return m_bSimulationMode;
}

void CDriver_OPCUA::Connect(const std::string & sIPAddress, const LibMCDriver_OPCUA_uint32 nPort, const LibMCDriver_OPCUA_uint32 nTimeout)
{
	


}

void CDriver_OPCUA::Disconnect()
{
}

IPLCCommandList * CDriver_OPCUA::CreateCommandList()
{
	return nullptr;
}

IPLCCommand * CDriver_OPCUA::CreateCommand(const std::string & sCommandName)
{
	return nullptr;
}

void CDriver_OPCUA::StartJournaling()
{
}

void CDriver_OPCUA::StopJournaling()
{
}

void CDriver_OPCUA::RefreshJournal()
{
}

void CDriver_OPCUA::onLog(const std::string& sMessage, opcUA_LogLevel level, opcUA_LogCategory category)
{
    std::string sCategory;
    switch (category) {
    case UA_LOGCATEGORY_NETWORK: sCategory = "network"; break;
    case UA_LOGCATEGORY_SECURECHANNEL: sCategory = "securechannel"; break;
    case UA_LOGCATEGORY_SESSION: sCategory = "session"; break;
    case UA_LOGCATEGORY_SERVER: sCategory = "server"; break;
    case UA_LOGCATEGORY_CLIENT: sCategory = "client"; break;
    case UA_LOGCATEGORY_USERLAND: sCategory = "userland"; break;
    case UA_LOGCATEGORY_SECURITYPOLICY: sCategory = "securitypolicy"; break;
    case UA_LOGCATEGORY_EVENTLOOP: sCategory = "eventloop"; break;
    default: sCategory = "unknown";
    }

    switch (level) {
    case UA_LOGLEVEL_DEBUG:
    case UA_LOGLEVEL_INFO:
        m_pDriverEnvironment->LogInfo(sCategory + " | " + sMessage);
        break;
    case UA_LOGLEVEL_WARNING:
        m_pDriverEnvironment->LogWarning(sCategory + " | " + sMessage);
        break;
    case UA_LOGLEVEL_ERROR: 
    case UA_LOGLEVEL_FATAL:
        m_pDriverEnvironment->LogMessage(sCategory + " | " + sMessage);
        break;
    }
}
