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


Abstract: This is a stub class definition of CDriver_BK9xxx

*/

#include "libmcdriver_bk9xxx_driver_bk9xxx.hpp"
#include "libmcdriver_bk9xxx_interfaceexception.hpp"
#include <iostream>

// Include custom headers here.

#define B9XXX_CONFIGURATIONSCHEMA "http://schemas.autodesk.com/amc/bk9xxxprotocol/2023/01"

using namespace LibMCDriver_BK9xxx::Impl;

/*************************************************************************************************************************
 Class definition of CDriver_BK9xxx 
**************************************************************************************************************************/





CDriver_BK9xxx::CDriver_BK9xxx(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment)
	: m_sName (sName), 
	m_bIsInSimulationMode (false),
	m_nPort(0), 
	m_nTimeOutInMs(0), 
	m_nMajorVersion(0),
	m_nMinorVersion(0),
	m_nPatchVersion(0),
	m_pDriverEnvironment (pDriverEnvironment)
{
	if (pDriverEnvironment.get() == nullptr)
		throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_INVALIDPARAM);
}

CDriver_BK9xxx::~CDriver_BK9xxx()
{
	m_pModBusTCPConnection = nullptr;
	m_pDriverEnvironment = nullptr;
}
	

void CDriver_BK9xxx::Configure(const std::string& sConfigurationString)
{

    m_pDriverEnvironment->LogMessage("Configuring BK9xxx driver!");

    if (sConfigurationString.length() == 0)
        throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_INVALIDDRIVERPROTOCOL);

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(sConfigurationString.c_str());
    if (!result)
        throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_COULDNOTPARSEDRIVERPROTOCOL);

    pugi::xml_node configurationNode = doc.child("driverconfiguration");
    if (configurationNode.empty())
        throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_INVALIDDRIVERPROTOCOL);

	pugi::xml_attribute xmlnsAttrib = configurationNode.attribute("xmlns");
	if (xmlnsAttrib.empty())
		throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_NOCONFIGURATIONSCHEMA);
	std::string sXMLNs = xmlnsAttrib.as_string();
	if (sXMLNs != B9XXX_CONFIGURATIONSCHEMA)
		throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_INVALIDCONFIGURATIONSCHEMA);

    pugi::xml_node versionNode = configurationNode.child("version");
    if (versionNode.empty())
        throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_NOVERSIONDEFINITION);

    pugi::xml_attribute majorversionAttrib = versionNode.attribute("major");
    if (majorversionAttrib.empty())
        throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_NOMAJORVERSION);
    m_nMajorVersion = majorversionAttrib.as_uint(0);

    pugi::xml_attribute minorversionAttrib = versionNode.attribute("minor");
    if (minorversionAttrib.empty())
        throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_NOMINORVERSION);
    m_nMinorVersion = minorversionAttrib.as_uint(0);

    pugi::xml_attribute patchversionAttrib = versionNode.attribute("patch");
    if (patchversionAttrib.empty())
        throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_NOPATCHVERSION);
    m_nPatchVersion = patchversionAttrib.as_uint(0);

	
		auto digitalInputNodes = configurationNode.children("digital_inputs");
		for (auto digitalInput : digitalInputNodes) {

			auto pDigitalInputBlock = std::make_shared<CDriver_BK9xxx_DigitalInputsDefinition>(digitalInput);
			m_pDigitalInputBlocks.push_back(pDigitalInputBlock);

			uint32_t nInputCount = pDigitalInputBlock->getCount();
			for (uint32_t nInputIndex = 0; nInputIndex < nInputCount; nInputIndex++) {
				auto pDigitalIO = pDigitalInputBlock->getIODefinition(nInputIndex);
				std::string sName = pDigitalIO->getName();
				if (m_ReservedNames.find(sName) != m_ReservedNames.end())
					throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_DUPLICATEVARIABLENAME, "duplicate variable name: " + sName);
				m_ReservedNames.insert(sName);

				auto iIter = m_DigitalInputIOMap.find(sName);
				if (iIter != m_DigitalInputIOMap.end())
					throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_DUPLICATEDIGITALINPUT, "duplicate digital input: " + sName);

				m_DigitalInputIOMap.insert(std::make_pair(sName, pDigitalIO));

				m_pDriverEnvironment->RegisterBoolParameter(sName, pDigitalIO->getDescription(), pDigitalIO->getValue ());
			}

		}
	

	{
		auto digitalOutputNodes = configurationNode.children("digital_outputs");
		for (auto digitalOutput : digitalOutputNodes) {

			auto pDigitalOutputBlock = std::make_shared<CDriver_BK9xxx_DigitalOutputsDefinition>(digitalOutput);
			m_pDigitalOutputBlocks.push_back(pDigitalOutputBlock);

			uint32_t nOutputCount = pDigitalOutputBlock->getCount();
			for (uint32_t nOutputIndex = 0; nOutputIndex < nOutputCount; nOutputIndex++) {
				auto pDigitalIO = pDigitalOutputBlock->getIODefinition(nOutputIndex);
				std::string sName = pDigitalIO->getName();
				if (m_ReservedNames.find(sName) != m_ReservedNames.end())
					throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_DUPLICATEVARIABLENAME, "duplicate variable name: " + sName);
				m_ReservedNames.insert(sName);

				auto iIter = m_DigitalOutputIOMap.find(sName);
				if (iIter != m_DigitalOutputIOMap.end())
					throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_DUPLICATEDIGITALOUTPUT, "duplicate digital output: " + sName);

				m_DigitalOutputIOMap.insert(std::make_pair(sName, pDigitalIO));

				m_pDriverEnvironment->RegisterBoolParameter(sName, pDigitalIO->getDescription(), pDigitalIO->getValue ());
			}

		}
	}

	auto analogInputNodes = configurationNode.children("analog_inputs");
	for (auto analogInput : analogInputNodes) {

		auto pAnalogInputBlock = std::make_shared<CDriver_BK9xxx_AnalogInputsDefinition>(analogInput);
		m_pAnalogInputBlocks.push_back(pAnalogInputBlock);

		uint32_t nInputCount = pAnalogInputBlock->getCount();
		for (uint32_t nInputIndex = 0; nInputIndex < nInputCount; nInputIndex++) {
			auto pAnalogIO = pAnalogInputBlock->getIODefinition(nInputIndex);
			std::string sName = pAnalogIO->getName();
			if (m_ReservedNames.find(sName) != m_ReservedNames.end())
				throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_DUPLICATEVARIABLENAME, "duplicate variable name: " + sName);
			m_ReservedNames.insert(sName);

			auto iIter = m_AnalogInputIOMap.find(sName);
			if (iIter != m_AnalogInputIOMap.end())
				throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_DUPLICATEANALOGINPUT, "duplicate analog input: " + sName);

			m_AnalogInputIOMap.insert(std::make_pair(sName, pAnalogIO));

			m_pDriverEnvironment->RegisterDoubleParameter(sName, pAnalogIO->getDescription(), pAnalogIO->getScaledValue ());
		}

	}

	auto analogOutputNodes = configurationNode.children("analog_outputs");
	for (auto analogOutput : analogOutputNodes) {

		auto pAnalogOutputBlock = std::make_shared<CDriver_BK9xxx_AnalogOutputsDefinition>(analogOutput);
		m_pAnalogOutputBlocks.push_back(pAnalogOutputBlock);

		uint32_t nOutputCount = pAnalogOutputBlock->getCount();
		for (uint32_t nOutputIndex = 0; nOutputIndex < nOutputCount; nOutputIndex++) {
			auto pAnalogIO = pAnalogOutputBlock->getIODefinition(nOutputIndex);
			std::string sName = pAnalogIO->getName();
			if (m_ReservedNames.find(sName) != m_ReservedNames.end())
				throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_DUPLICATEVARIABLENAME, "duplicate variable name: " + sName);
			m_ReservedNames.insert(sName);

			auto iIter = m_AnalogOutputIOMap.find(sName);
			if (iIter != m_AnalogOutputIOMap.end())
				throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_DUPLICATEANALOGOUTPUT, "duplicate analog output: " + sName);

			m_AnalogOutputIOMap.insert(std::make_pair(sName, pAnalogIO));

			m_pDriverEnvironment->RegisterDoubleParameter(sName, pAnalogIO->getDescription(), pAnalogIO->getScaledValue());
		}

	}

}

std::string CDriver_BK9xxx::GetName()
{
	return m_sName;
}


void CDriver_BK9xxx::GetVersion(LibMCDriver_BK9xxx_uint32& nMajor, LibMCDriver_BK9xxx_uint32& nMinor, LibMCDriver_BK9xxx_uint32& nMicro, std::string& sBuild)
{
	nMajor = LIBMCDRIVER_BK9XXX_VERSION_MAJOR;
	nMinor = LIBMCDRIVER_BK9XXX_VERSION_MINOR;
	nMicro = LIBMCDRIVER_BK9XXX_VERSION_MICRO;
}

void CDriver_BK9xxx::GetHeaderInformation(std::string& sNameSpace, std::string& sBaseName)
{
	sNameSpace = "LibMCDriver_BK9xxx";
	sBaseName = "libmcdriver_bk9xxx";
}

void CDriver_BK9xxx::QueryParameters()
{
	
	if (IsConnected()) {

		for (auto pDigitalInputBlock : m_pDigitalInputBlocks) {
			auto pIOStatus = m_pModBusTCPConnection->ReadInputStatus(pDigitalInputBlock->getStartAddress(), pDigitalInputBlock->getBitCount());
			uint32_t nCount = pDigitalInputBlock->getCount();
			for (uint32_t nIndex = 0; nIndex < nCount; nIndex++) {
				auto pIODefinition = pDigitalInputBlock->getIODefinition(nIndex);
				bool bValue = pIOStatus->GetValue(pIODefinition->getOffset());

				pIODefinition->setValue(bValue);
			}
			
		}

		for (auto pAnalogInputBlock : m_pAnalogInputBlocks) {
			auto pIOStatus = m_pModBusTCPConnection->ReadHoldingRegisters(pAnalogInputBlock->getStartAddress(), pAnalogInputBlock->getRegisterCount ());
			uint32_t nCount = pAnalogInputBlock->getCount();
			for (uint32_t nIndex = 0; nIndex < nCount; nIndex++) {
				auto pIODefinition = pAnalogInputBlock->getIODefinition(nIndex);
				uint32_t nRawValue = pIOStatus->GetValue(pIODefinition->getOffset());

				pIODefinition->setRawValue(nRawValue);
			}

		}


	}


}

void CDriver_BK9xxx::SetToSimulationMode()
{
	if (IsConnected())
		throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_SETSIMULATIONMODEBEFORECONNECTING);

	m_bIsInSimulationMode = true;
}

bool CDriver_BK9xxx::IsSimulationMode()
{
	return m_bIsInSimulationMode;
}

void CDriver_BK9xxx::Connect(const std::string & sIPAddress, const LibMCDriver_BK9xxx_uint32 nPort, const LibMCDriver_BK9xxx_uint32 nTimeOutInMs)
{
	Disconnect();

	m_sIPAddress = sIPAddress;
	m_nPort = nPort;
	m_nTimeOutInMs = nTimeOutInMs;

	Reconnect();
}

std::string CDriver_BK9xxx::GetIPAddress()
{
	return m_sIPAddress;
}

LibMCDriver_BK9xxx_uint32 CDriver_BK9xxx::GetPort()
{
	return m_nPort;
}

LibMCDriver_BK9xxx_uint32 CDriver_BK9xxx::GetTimeOut()
{
	return m_nTimeOutInMs;
}

void CDriver_BK9xxx::SetErrorRecoveryMode(const LibMCDriver_BK9xxx_uint32 nReconnectionDelayInMs, const LibMCDriver_BK9xxx_uint32 nReconnectionCount)
{
	throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_NOTIMPLEMENTED);
}

void CDriver_BK9xxx::GetErrorRecoveryMode(LibMCDriver_BK9xxx_uint32 & nReconnectionDelayInMs, const LibMCDriver_BK9xxx_uint32 nReconnectionCount)
{
	throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_NOTIMPLEMENTED);
}

void CDriver_BK9xxx::Disconnect()
{
	if (m_pModBusTCPConnection.get() != nullptr) {
		m_pModBusTCPConnection->Disconnect();
		m_pModBusTCPConnection = nullptr;
	}
}

void CDriver_BK9xxx::ResetDevice()
{
}

void CDriver_BK9xxx::Reconnect()
{
	Disconnect();

	//std::cout << "Connecting to BK9xxx " << m_sIPAddress << ": " << m_nPort << std::endl;

	m_pModBusTCPConnection = m_pDriverEnvironment->CreateModbusTCPConnection(m_sIPAddress, m_nPort, m_nTimeOutInMs);
}

bool CDriver_BK9xxx::IsConnected()
{
	if (m_pModBusTCPConnection.get() != nullptr) {
		return m_pModBusTCPConnection->IsConnected();
	}

	return false;
}

LibMCDriver_BK9xxx::eErrorState CDriver_BK9xxx::GetErrorState()
{
	throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_NOTIMPLEMENTED);
}

bool CDriver_BK9xxx::VariableExists(const std::string & sName)
{
	auto iDigitalInputIter = m_DigitalInputIOMap.find(sName);
	if (iDigitalInputIter != m_DigitalInputIOMap.end())
		return true;
	auto iDigitalOutputIter = m_DigitalOutputIOMap.find(sName);
	if (iDigitalOutputIter != m_DigitalOutputIOMap.end())
		return true;
	auto iAnalogInputIter = m_AnalogInputIOMap.find(sName);
	if (iAnalogInputIter != m_AnalogInputIOMap.end())
		return true;
	auto iAnalogOutputIter = m_AnalogOutputIOMap.find(sName);
	if (iAnalogOutputIter != m_AnalogOutputIOMap.end())
		return true;

	return false;
}

LibMCDriver_BK9xxx::eVariableType CDriver_BK9xxx::GetVariableType(const std::string & sName)
{
	auto iDigitalInputIter = m_DigitalInputIOMap.find(sName);
	if (iDigitalInputIter != m_DigitalInputIOMap.end())
		return LibMCDriver_BK9xxx::eVariableType::DigitalInput;
	auto iDigitalOutputIter = m_DigitalOutputIOMap.find(sName);
	if (iDigitalOutputIter != m_DigitalOutputIOMap.end())
		return LibMCDriver_BK9xxx::eVariableType::DigitalOutput;
	auto iAnalogInputIter = m_AnalogInputIOMap.find(sName);
	if (iAnalogInputIter != m_AnalogInputIOMap.end())
		return LibMCDriver_BK9xxx::eVariableType::AnalogInput;
	auto iAnalogOutputIter = m_AnalogOutputIOMap.find(sName);
	if (iAnalogOutputIter != m_AnalogOutputIOMap.end())
		return LibMCDriver_BK9xxx::eVariableType::AnalogOutput;

	throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_VARIABLENOTFOUND);
}

bool CDriver_BK9xxx::DigitalInputExists(const std::string & sName)
{
	auto iIter = m_DigitalInputIOMap.find(sName);
	return iIter != m_DigitalInputIOMap.end();
}

bool CDriver_BK9xxx::DigitalOutputExists(const std::string & sName)
{
	auto iIter = m_DigitalOutputIOMap.find(sName);
	return iIter != m_DigitalOutputIOMap.end();
}

bool CDriver_BK9xxx::AnalogInputExists(const std::string & sName)
{
	auto iIter = m_AnalogInputIOMap.find(sName);
	return iIter != m_AnalogInputIOMap.end();
}

bool CDriver_BK9xxx::AnalogOutputExists(const std::string & sName)
{
	auto iIter = m_AnalogOutputIOMap.find(sName);
	return iIter != m_AnalogOutputIOMap.end();
}

bool CDriver_BK9xxx::GetDigitalInput(const std::string & sVariableName)
{
	auto iIter = m_DigitalInputIOMap.find(sVariableName);
	if (iIter == m_DigitalInputIOMap.end ())
		throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_DIGITALIONOTFOUND);

	return iIter->second->getValue();

}

bool CDriver_BK9xxx::GetDigitalOutput(const std::string & sVariableName)
{
	auto iIter = m_DigitalOutputIOMap.find(sVariableName);
	if (iIter == m_DigitalOutputIOMap.end())
		throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_DIGITALIONOTFOUND);

	return iIter->second->getValue();
}

LibMCDriver_BK9xxx_uint32 CDriver_BK9xxx::GetAnalogInputRaw(const std::string & sVariableName)
{
	auto iIter = m_AnalogInputIOMap.find(sVariableName);
	if (iIter == m_AnalogInputIOMap.end())
		throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_ANALOGIONOTFOUND);

	return iIter->second->getRawValue();

}

LibMCDriver_BK9xxx_uint32 CDriver_BK9xxx::GetAnalogOutputRaw(const std::string & sVariableName)
{
	auto iIter = m_AnalogOutputIOMap.find(sVariableName);
	if (iIter == m_AnalogOutputIOMap.end())
		throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_ANALOGIONOTFOUND);

	return iIter->second->getRawValue();
}

LibMCDriver_BK9xxx_double CDriver_BK9xxx::GetAnalogInput(const std::string & sVariableName)
{
	auto iIter = m_AnalogInputIOMap.find(sVariableName);
	if (iIter == m_AnalogInputIOMap.end())
		throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_ANALOGIONOTFOUND);

	return iIter->second->getScaledValue();
}

LibMCDriver_BK9xxx_double CDriver_BK9xxx::GetAnalogOutput(const std::string & sVariableName)
{
	auto iIter = m_AnalogOutputIOMap.find(sVariableName);
	if (iIter == m_AnalogOutputIOMap.end())
		throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_ANALOGIONOTFOUND);

	return iIter->second->getScaledValue();
}

void CDriver_BK9xxx::SetDigitalOutput(const std::string & sVariableName, const bool bValue)
{
	throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_NOTIMPLEMENTED);
}

void CDriver_BK9xxx::SetAnalogOutputRaw(const std::string & sVariableName, const LibMCDriver_BK9xxx_uint32 nValue)
{
	throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_NOTIMPLEMENTED);
}

void CDriver_BK9xxx::SetAnalogOutput(const std::string & sVariableName, const LibMCDriver_BK9xxx_double dValue)
{
	throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_NOTIMPLEMENTED);
}


CDriver_BK9000::CDriver_BK9000(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment)
	: CDriver_BK9xxx (sName, pDriverEnvironment)
{

}

CDriver_BK9000::~CDriver_BK9000()
{

}

std::string CDriver_BK9000::GetType()
{
	return "bk9000";
}


CDriver_BK9050::CDriver_BK9050(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment)
	: CDriver_BK9xxx(sName, pDriverEnvironment)
{

}

CDriver_BK9050::~CDriver_BK9050()
{

}

std::string CDriver_BK9050::GetType()
{
	return "bk9050";
}


CDriver_BK9100::CDriver_BK9100(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment)
	: CDriver_BK9xxx(sName, pDriverEnvironment)
{

}

CDriver_BK9100::~CDriver_BK9100()
{

}

std::string CDriver_BK9100::GetType()
{
	return "bk9100";
}

