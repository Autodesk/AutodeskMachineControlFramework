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


Abstract: This is a stub class definition of CDriver_CifX

*/

#include "libmcdriver_cifx_driver_cifx.hpp"
#include "libmcdriver_cifx_interfaceexception.hpp"

// Include custom headers here.
#define __STRINGIZE(x) #x
#define __STRINGIZE_VALUE_OF(x) __STRINGIZE(x)

#include <iostream>

#define CIFX_DEFAULT_HOSTSTATETIMEOUT 5000
#define CIFX_DEFAULT_BUSSTATETIMEOUT 2000

#define CIFX_MAXINPUTSIZE (1024 * 1024)
#define CIFX_MAXOUTPUTSIZE (1024 * 1024)

using namespace LibMCDriver_CifX::Impl;

CDriver_CifXParameter::CDriver_CifXParameter(const std::string& sName, const std::string& sDescription, eDriver_CifXParameterType Type, const uint32_t nAddress)
	: m_sName (sName), 
	m_sDescription (sDescription), 
	m_eType (Type),
	m_nAddress (nAddress)
{

}

CDriver_CifXParameter::~CDriver_CifXParameter()
{

}

std::string CDriver_CifXParameter::getName()
{
	return m_sName;
}

std::string CDriver_CifXParameter::getDescription()
{
	return m_sDescription;
}

eDriver_CifXParameterType CDriver_CifXParameter::getType()
{
	return m_eType;
}

eDriver_AbstractParameterType CDriver_CifXParameter::getAbstractType()
{
	switch (m_eType) {
		case eDriver_CifXParameterType::CifXParameter_BOOL:
			return eDriver_AbstractParameterType::CifXAbstractParameter_BOOL;

		case eDriver_CifXParameterType::CifXParameter_UINT8:
		case eDriver_CifXParameterType::CifXParameter_UINT16:
		case eDriver_CifXParameterType::CifXParameter_UINT32:
		case eDriver_CifXParameterType::CifXParameter_INT8:
		case eDriver_CifXParameterType::CifXParameter_INT16:
		case eDriver_CifXParameterType::CifXParameter_INT32:
			return eDriver_AbstractParameterType::CifXAbstractParameter_INT;

		case eDriver_CifXParameterType::CifXParameter_FLOAT:
		case eDriver_CifXParameterType::CifXParameter_DOUBLE:
			return eDriver_AbstractParameterType::CifXAbstractParameter_DOUBLE;

		default:
			return eDriver_AbstractParameterType::CifXAbstractParameter_Unknown;
	}
}


double CDriver_CifXParameter::GetActualDoubleValue()
{
	return 0.0;
}

int64_t CDriver_CifXParameter::GetActualIntegerValue()
{
	return 0;
}

bool CDriver_CifXParameter::GetActualBoolValue()
{
	return GetActualIntegerValue() != 0;
}

void CDriver_CifXParameter::SetActualDoubleValue(double dValue)
{

}

void CDriver_CifXParameter::SetActualIntegerValue(int64_t nValue)
{

}



void CDriver_CifXParameter::SetActualBoolValue(bool bValue)
{
	if (bValue)
		SetActualIntegerValue(1);
	else
		SetActualIntegerValue(0);

}

double CDriver_CifXParameter::GetTargetDoubleValue()
{
	return 0.0;
}

int64_t CDriver_CifXParameter::GetTargetIntegerValue()
{
	return 0;
}

bool CDriver_CifXParameter::GetTargetBoolValue()
{
	return GetTargetIntegerValue() != 0;
}

void CDriver_CifXParameter::SetTargetDoubleValue(double dValue)
{

}

void CDriver_CifXParameter::SetTargetIntegerValue(int64_t nValue)
{

}

void CDriver_CifXParameter::SetTargetBoolValue(bool bValue)
{
	if (bValue)
		SetTargetIntegerValue(1);
	else
		SetTargetIntegerValue(0);
}


CDriver_CifXParameter_Integer::CDriver_CifXParameter_Integer(const std::string& sName, const std::string& sDescription, eDriver_CifXParameterType Type, const uint32_t nAddress)
	: CDriver_CifXParameter (sName, sDescription, Type, nAddress), m_nActualValue (0), m_nTargetValue(0)
{

}

CDriver_CifXParameter_Integer::~CDriver_CifXParameter_Integer()
{

}


int64_t CDriver_CifXParameter_Integer::GetActualIntegerValue()
{
	return m_nActualValue;
}

void CDriver_CifXParameter_Integer::SetActualIntegerValue(int64_t nValue)
{
	m_nActualValue = nValue;
}

int64_t CDriver_CifXParameter_Integer::GetTargetIntegerValue()
{
	return m_nTargetValue;
}

void CDriver_CifXParameter_Integer::SetTargetIntegerValue(int64_t nValue)
{
	m_nTargetValue = nValue;
}

CDriver_CifXParameter_Double::CDriver_CifXParameter_Double(const std::string& sName, const std::string& sDescription, eDriver_CifXParameterType Type, const uint32_t nAddress)
	: CDriver_CifXParameter(sName, sDescription, Type, nAddress), m_dActualValue(0.0), m_dTargetValue(0.0)
{

}

CDriver_CifXParameter_Double::~CDriver_CifXParameter_Double()
{

}

double CDriver_CifXParameter_Double::GetActualDoubleValue()
{
	return m_dActualValue;
}

void CDriver_CifXParameter_Double::SetActualDoubleValue(double dValue)
{
	m_dActualValue = dValue;
}

double CDriver_CifXParameter_Double::GetTargetDoubleValue()
{
	return m_dTargetValue;
}

void CDriver_CifXParameter_Double::SetTargetDoubleValue(double dValue)
{
	m_dTargetValue = dValue;
}

CDriver_CifXParameter_Bool::CDriver_CifXParameter_Bool(const std::string& sName, const std::string& sDescription, eDriver_CifXParameterType Type, const uint32_t nAddress, const uint32_t nBit)
	: CDriver_CifXParameter_Integer (sName, sDescription, Type, nAddress), m_nBit (0)
{

}

CDriver_CifXParameter_Bool::~CDriver_CifXParameter_Bool()
{

}



PDriver_CifXParameter readParameterFromXMLNode(pugi::xml_node & node)
{
	std::string sNodeName = node.name();

	auto& addressAttrib = node.attribute("address");
	auto& bitAttrib = node.attribute("bit");
	auto& nameAttrib = node.attribute("name");
	auto& descriptionAttrib = node.attribute("description");
	auto& defaultAttrib = node.attribute("default");

	if (addressAttrib.empty ())
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_MISSINGADDRESSATTRIBUTE);
	int32_t nAddress = addressAttrib.as_int(-1);
	if (nAddress < 0)
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDADDRESSATTRIBUTE);

	int32_t nBit = bitAttrib.as_int(0);

	std::string sName = nameAttrib.as_string();
	std::string sDescription = descriptionAttrib.as_string();

	if (sNodeName == "bool")
		return std::make_shared<CDriver_CifXParameter_Bool>(sName, sDescription, eDriver_CifXParameterType::CifXParameter_BOOL, nAddress, nBit);
	if (sNodeName == "uint8")
		return std::make_shared<CDriver_CifXParameter_Integer>(sName, sDescription, eDriver_CifXParameterType::CifXParameter_UINT8, nAddress);
	if (sNodeName == "uint16")
		return std::make_shared<CDriver_CifXParameter_Integer>(sName, sDescription, eDriver_CifXParameterType::CifXParameter_UINT16, nAddress);
	if (sNodeName == "uint32")
		return std::make_shared<CDriver_CifXParameter_Integer>(sName, sDescription, eDriver_CifXParameterType::CifXParameter_UINT32, nAddress);
	if (sNodeName == "int8")
		return std::make_shared<CDriver_CifXParameter_Integer>(sName, sDescription, eDriver_CifXParameterType::CifXParameter_INT8, nAddress);
	if (sNodeName == "int16")
		return std::make_shared<CDriver_CifXParameter_Integer>(sName, sDescription, eDriver_CifXParameterType::CifXParameter_INT16, nAddress);
	if (sNodeName == "int32")
		return std::make_shared<CDriver_CifXParameter_Integer>(sName, sDescription, eDriver_CifXParameterType::CifXParameter_INT32, nAddress);

	return nullptr;

}


CDriver_CifXChannelThreadState::CDriver_CifXChannelThreadState(PCifXSDK pCifXSDK, uint32_t nInputSize, uint32_t nOutputSize, cifxHandle hChannel)
	: m_pCifXSDK (pCifXSDK), m_hChannel (hChannel), m_bDebugMode (true)
{
	if (pCifXSDK.get() == nullptr)
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDPARAM);
	if (hChannel == nullptr)
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDPARAM);

	if (nInputSize > CIFX_MAXINPUTSIZE)
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDINPUTIOSIZEATTRIBUTE);
	if (nOutputSize > CIFX_MAXOUTPUTSIZE)
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDOUTPUTIOSIZEATTRIBUTE);


	m_InputData.resize(nInputSize);
	m_OutputData.resize(nOutputSize);

	for (auto& iInputIter : m_InputData)
		iInputIter = 0;
	for (auto& iOutputIter : m_OutputData)
		iOutputIter = 0;
}

CDriver_CifXChannelThreadState::~CDriver_CifXChannelThreadState()
{

}

void CDriver_CifXChannelThreadState::executeThread(uint32_t nReadTimeOut, uint32_t nWriteTimeOut)
{
	std::lock_guard<std::mutex> lockGuard(m_Mutex);
	if (m_InputData.size() > 0) 
		m_pCifXSDK->checkError(m_pCifXSDK->xChannelIORead(m_hChannel, 0, 0, (uint32_t) m_InputData.size(), m_InputData.data(), nReadTimeOut));
	if (m_OutputData.size() > 0)
		m_pCifXSDK->checkError(m_pCifXSDK->xChannelIOWrite(m_hChannel, 0, 0, (uint32_t) m_OutputData.size(), m_OutputData.data(), nWriteTimeOut));

}

void CDriver_CifXChannelThreadState::handleException(uint32_t nErrorCode, const std::string& sMessage)
{
	if (m_bDebugMode) {
		std::cout << "B9XXX: An exception occured: " << sMessage << std::endl;
	}
	m_Exceptions.push_back(std::make_pair(nErrorCode, sMessage));

}

bool CDriver_CifXChannelThreadState::threadShallBeCanceled()
{
	return m_CancelFlag;
}


void CDriver_CifXChannelThreadState::stopThread(uint32_t nHostStateTimeOut, uint32_t nBusStateTimeOut)
{
	m_CancelFlag = true;

	std::lock_guard<std::mutex> lockGuard(m_Mutex);
	if ((m_hChannel != nullptr) && (m_pCifXSDK.get () != nullptr)) {

		uint32_t nState = 0;
		m_pCifXSDK->xChannelBusState(m_hChannel, CIFX_BUS_STATE_OFF, &nState, nHostStateTimeOut);
		m_pCifXSDK->xChannelHostState(m_hChannel, CIFX_HOST_STATE_NOT_READY, &nState, nBusStateTimeOut);
		m_pCifXSDK->xChannelClose(m_hChannel);
		m_hChannel = nullptr;
	}
}


CDriver_CifXChannel::CDriver_CifXChannel(pugi::xml_node& channelNode)
	: m_nChannelIndex(0), m_nInputSize(0), m_nOutputSize(0), m_nHostStateTimeOut(CIFX_DEFAULT_HOSTSTATETIMEOUT), m_nBusStateTimeOut(CIFX_DEFAULT_BUSSTATETIMEOUT)

{
	auto& boardAttrib = channelNode.attribute("board");
	auto& channelIndexAttrib = channelNode.attribute("channelindex");

	m_sBoardName = boardAttrib.as_string();
	if (m_sBoardName.empty())
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_NOBOARDATTRIBUTE);
	if (channelIndexAttrib.empty())
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_NOCHANNELINDEXATTRIBUTE);

	int nChannelIndex = channelIndexAttrib.as_int(-1);
	if (nChannelIndex < 0)
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDCHANNELINDEXATTRIBUTE);

	m_nChannelIndex = (uint32_t)nChannelIndex;
	
	auto& hostStateTimeoutAttrib = channelNode.attribute("hoststatetimeout");
	if (!hostStateTimeoutAttrib.empty()) {
		int nHostStateTimeout = hostStateTimeoutAttrib.as_int(0);
		if (nHostStateTimeout <= 0)
			throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDHOSTSTATETIMEOUT);
		m_nHostStateTimeOut = nHostStateTimeout;
	}

	 
	auto& busStateTimeoutAttrib = channelNode.attribute("busstatetimeout");
	if (!busStateTimeoutAttrib.empty()) {
		int nBusStateTimeout = busStateTimeoutAttrib.as_int(0);
		if (nBusStateTimeout <= 0)
			throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDBUSSTATETIMEOUT);
		m_nBusStateTimeOut = nBusStateTimeout;
	}

	auto& inputIONode = channelNode.child("input_io");
	if (!inputIONode.empty()) {

		auto sizeAttrib = inputIONode.attribute("size");
		if (sizeAttrib.empty ())
			throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_NOINPUTIOSIZEATTRIBUTE);
		int nSize = sizeAttrib.as_int(0);
		if (nSize <= 0)
			throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDINPUTIOSIZEATTRIBUTE);
		m_nInputSize = (uint32_t)nSize;

		auto ioNodes = inputIONode.children();
		for (auto ioNode : ioNodes) {
			PDriver_CifXParameter pParameter = readParameterFromXMLNode(ioNode);
			if (pParameter.get() != nullptr) {
				m_Inputs.push_back(pParameter);
				m_InputMap.insert(std::make_pair (pParameter->getName (), pParameter));
			}
		}
	}

	auto& outputIONode = channelNode.child("output_io");
	if (!outputIONode.empty()) {

		auto sizeAttrib = outputIONode.attribute("size");
		if (sizeAttrib.empty())
			throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_NOOUTPUTIOSIZEATTRIBUTE);
		int nSize = sizeAttrib.as_int(0);
		if (nSize <= 0)
			throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDOUTPUTIOSIZEATTRIBUTE);
		m_nOutputSize = (uint32_t)nSize;

		auto ioNodes = outputIONode.children();
		for (auto ioNode : ioNodes) {
			PDriver_CifXParameter pParameter = readParameterFromXMLNode(ioNode);
			if (pParameter.get() != nullptr) {
				m_Outputs.push_back(pParameter);
				m_OutputMap.insert(std::make_pair(pParameter->getName(), pParameter));
			}
		}
	}


}

CDriver_CifXChannel::~CDriver_CifXChannel()
{

}

std::string CDriver_CifXChannel::getBoardName()
{
	return m_sBoardName;
}

uint32_t CDriver_CifXChannel::getChannelIndex()
{
	return m_nChannelIndex;
}

void CDriver_CifXChannel::RegisterVariables(LibMCEnv::PDriverEnvironment pDriverEnvironment)
{
	if (pDriverEnvironment.get() == nullptr)
	{
		for (auto pInputParameter : m_Inputs) {
			switch (pInputParameter->getAbstractType()) {
				case eDriver_AbstractParameterType::CifXAbstractParameter_BOOL:
					pDriverEnvironment->RegisterBoolParameter(pInputParameter->getName(), pInputParameter->getDescription(), pInputParameter->GetActualBoolValue());
					break;

				case eDriver_AbstractParameterType::CifXAbstractParameter_INT:
					pDriverEnvironment->RegisterIntegerParameter(pInputParameter->getName(), pInputParameter->getDescription(), pInputParameter->GetActualIntegerValue());
					break;

				case eDriver_AbstractParameterType::CifXAbstractParameter_DOUBLE:
					pDriverEnvironment->RegisterDoubleParameter(pInputParameter->getName(), pInputParameter->getDescription(), pInputParameter->GetActualDoubleValue());
					break;

			}

		}

		for (auto pOutputParameter : m_Outputs) {
			switch (pOutputParameter->getAbstractType()) {
			case eDriver_AbstractParameterType::CifXAbstractParameter_BOOL:
				pDriverEnvironment->RegisterBoolParameter(pOutputParameter->getName(), pOutputParameter->getDescription(), pOutputParameter->GetActualBoolValue());
				break;

			case eDriver_AbstractParameterType::CifXAbstractParameter_INT:
				pDriverEnvironment->RegisterIntegerParameter(pOutputParameter->getName(), pOutputParameter->getDescription(), pOutputParameter->GetActualIntegerValue());
				break;

			case eDriver_AbstractParameterType::CifXAbstractParameter_DOUBLE:
				pDriverEnvironment->RegisterDoubleParameter(pOutputParameter->getName(), pOutputParameter->getDescription(), pOutputParameter->GetActualDoubleValue());
				break;

			}
		}
	}
}

std::vector<PDriver_CifXParameter> CDriver_CifXChannel::getInputs()
{
	return m_Inputs;
}

std::vector<PDriver_CifXParameter> CDriver_CifXChannel::getOutputs()
{
	return m_Outputs;
}

void CDriver_CifXChannel::startSyncThread(PCifXSDK pCifXSDK, cifxHandle hDriverHandle)
{
	if (pCifXSDK.get() == nullptr)
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDPARAM);

	stopSyncThread();

	// Open channel
	cifxHandle hChannel = 0;
	pCifXSDK->checkError(pCifXSDK->xChannelOpen(hDriverHandle, m_sBoardName.c_str(), m_nChannelIndex, &hChannel));

	// Create Thread State
	auto pThreadState = std::make_shared<CDriver_CifXChannelThreadState>(pCifXSDK, m_nInputSize, m_nOutputSize, hChannel);
	m_pThreadState = pThreadState;

	// Set Host state to ready
	uint32_t nHostState = 0;
	pCifXSDK->checkError(pCifXSDK->xChannelHostState(hChannel, CIFX_HOST_STATE_READY, &nHostState, m_nHostStateTimeOut));

	// Set bus state to ready
	uint32_t nBusState = 0;
	pCifXSDK->checkError(pCifXSDK->xChannelBusState(hChannel, CIFX_BUS_STATE_ON, &nBusState, m_nBusStateTimeOut));

	uint32_t nReadTimeout = 1000;
	uint32_t nWriteTimeout = 1000;

	auto pInputs = m_Inputs;
	auto pOutputs = m_Outputs;

	m_SyncThread = std::thread([pThreadState, pInputs, pOutputs, nReadTimeout, nWriteTimeout]() {

		try
		{
			while (!pThreadState->threadShallBeCanceled()) {
				pThreadState->executeThread(nReadTimeout, nWriteTimeout);

				// TODO: Sync Variables
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}
		}
		catch (ELibMCDriver_CifXInterfaceException& DriverException) {
			std::cout << "Fatal Error: " << DriverException.what() << std::endl;

			pThreadState->handleException(DriverException.getErrorCode(), DriverException.what());
		}
		catch (std::exception& Exception) {
			std::cout << "Fatal Error: " << Exception.what() << std::endl;

			pThreadState->handleException(LIBMCDRIVER_CIFX_ERROR_GENERICEXCEPTION, Exception.what());
		}
		catch (...) {
			std::cout << "Fatal Error: " << std::endl;

			pThreadState->handleException(LIBMCDRIVER_CIFX_ERROR_UNKNOWNEXCEPTION, "an unknown fatal error occured");
		}

	});

}

void CDriver_CifXChannel::stopSyncThread()
{
	if (m_pThreadState.get() != nullptr) {
		m_pThreadState->stopThread(m_nHostStateTimeOut, m_nBusStateTimeOut);
		m_pThreadState = nullptr;

		if (m_SyncThread.joinable())
			m_SyncThread.join();
	}
}

/*************************************************************************************************************************
 Class definition of CDriver_CifX 
**************************************************************************************************************************/

CDriver_CifX::CDriver_CifX(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment)
	: m_sName (sName), m_bIsSimulationMode (false), m_pDriverEnvironment (pDriverEnvironment), m_hDriverHandle (nullptr),
	  m_nMajorVersion (0), m_nMinorVersion (0), m_nPatchVersion (0)

{
	if (pDriverEnvironment.get() == nullptr)
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDPARAM);

}

CDriver_CifX::~CDriver_CifX()
{
	Disconnect();
}


void CDriver_CifX::Configure(const std::string& sConfigurationString)
{
	m_pDriverEnvironment->LogMessage("Configuring CifX driver!");

	if (sConfigurationString.length() == 0)
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDDRIVERPROTOCOL);

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_string(sConfigurationString.c_str());
	if (!result)
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_COULDNOTPARSEDRIVERPROTOCOL);

	pugi::xml_node protocolNode = doc.child("driverconfiguration");

	if (protocolNode.empty())
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDDRIVERPROTOCOL);

	pugi::xml_node versionNode = protocolNode.child("version");
	if (versionNode.empty())
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_NOVERSIONDEFINITION);

	pugi::xml_attribute majorversionAttrib = versionNode.attribute("major");
	if (majorversionAttrib.empty())
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_NOMAJORVERSION);
	m_nMajorVersion = majorversionAttrib.as_uint(0);

	pugi::xml_attribute minorversionAttrib = versionNode.attribute("minor");
	if (minorversionAttrib.empty())
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_NOMINORVERSION);
	m_nMinorVersion = minorversionAttrib.as_uint(0);

	pugi::xml_attribute patchversionAttrib = versionNode.attribute("patch");
	if (patchversionAttrib.empty())
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_NOPATCHVERSION);
	m_nPatchVersion = patchversionAttrib.as_uint(0);

	auto channelNodes = protocolNode.children("channel");
	for (pugi::xml_node channelNode : channelNodes)
	{
		PDriver_CifXChannel pChannel = std::make_shared<CDriver_CifXChannel>(channelNode);
		m_Channels.push_back(pChannel);

		pChannel->RegisterVariables(m_pDriverEnvironment);

		auto pInputs = pChannel->getInputs();
		for (auto pInput : pInputs) {
			std::string sName = pInput->getName();
			if (m_GlobalInputMap.find (sName) != m_GlobalInputMap.end ())
				throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_DUPLICATEINPUTIO, "duplicate input io: " + sName);
			m_GlobalInputMap.insert(std::make_pair (sName, pInput));
		}

		auto pOutputs = pChannel->getOutputs();
		for (auto pOutput : pOutputs) {
			std::string sName = pOutput->getName();
			if (m_GlobalOutputMap.find(sName) != m_GlobalOutputMap.end())
				throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_DUPLICATEOUTPUTIO, "duplicate output io: " + sName);
			m_GlobalOutputMap.insert(std::make_pair(sName, pOutput));
		}

	}


}

std::string CDriver_CifX::GetName()
{
	return m_sName;
}

std::string CDriver_CifX::GetType()
{
	return "cifx-1.0";
}

void CDriver_CifX::GetVersion(LibMCDriver_CifX_uint32& nMajor, LibMCDriver_CifX_uint32& nMinor, LibMCDriver_CifX_uint32& nMicro, std::string& sBuild)
{
	nMajor = LIBMCDRIVER_CIFX_VERSION_MAJOR;
	nMinor = LIBMCDRIVER_CIFX_VERSION_MINOR;
	nMicro = LIBMCDRIVER_CIFX_VERSION_MICRO;
	sBuild = __STRINGIZE_VALUE_OF(__GITHASH);

}

void CDriver_CifX::GetHeaderInformation(std::string& sNameSpace, std::string& sBaseName)
{
	sNameSpace = "LibMCDriver_CifX";
	sBaseName = "libmcdriver_cifx";
}

void CDriver_CifX::QueryParameters()
{

	if (IsConnected()) {
		for (auto pOutputValueIter : m_GlobalOutputMap) {
			auto pOutputValue = pOutputValueIter.second;
			switch (pOutputValue->getAbstractType()) {
			case eDriver_AbstractParameterType::CifXAbstractParameter_BOOL:
				m_pDriverEnvironment->SetBoolParameter(pOutputValue->getName(), pOutputValue->GetActualBoolValue());
				break;

			case eDriver_AbstractParameterType::CifXAbstractParameter_INT:
				m_pDriverEnvironment->SetIntegerParameter(pOutputValue->getName(), pOutputValue->GetActualIntegerValue());
				break;

			case eDriver_AbstractParameterType::CifXAbstractParameter_DOUBLE:
				m_pDriverEnvironment->SetDoubleParameter(pOutputValue->getName(), pOutputValue->GetActualDoubleValue());
				break;

			}
			
		}
		
		for (auto pInputValueIter : m_GlobalInputMap) {
			auto pInputValue = pInputValueIter.second;
			switch (pInputValue->getAbstractType()) {
				case eDriver_AbstractParameterType::CifXAbstractParameter_BOOL:
					m_pDriverEnvironment->SetBoolParameter(pInputValue->getName(), pInputValue->GetActualBoolValue());
					break;

				case eDriver_AbstractParameterType::CifXAbstractParameter_INT:
					m_pDriverEnvironment->SetIntegerParameter(pInputValue->getName(), pInputValue->GetActualIntegerValue());
					break;

				case eDriver_AbstractParameterType::CifXAbstractParameter_DOUBLE:
					m_pDriverEnvironment->SetDoubleParameter(pInputValue->getName(), pInputValue->GetActualDoubleValue());
					break;

			}
			
		}

	}
}


void CDriver_CifX::SetToSimulationMode()
{
	if (m_pCifXSDK.get() != nullptr)
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_SDKALREADYLOADED);

	m_bIsSimulationMode = true;

}

bool CDriver_CifX::IsSimulationMode()
{
	return m_bIsSimulationMode;
}

void CDriver_CifX::SetCustomSDKResource(const std::string & sResourceName)
{
	if (m_pCifXSDK.get () != nullptr)
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_SDKALREADYLOADED);

	m_sCustomSDKResource = sResourceName;
}

void CDriver_CifX::LoadSDKIfNeeded()
{
	if (m_bIsSimulationMode)
		return;

	if (m_pCifXSDK.get() == nullptr) {

		std::string sResourceName = "cifxsdk";
		std::string sFileNameOnDisk;

#ifdef _WIN32
		sFileNameOnDisk = "cifxsdk.dll";
#else
		sFileNameOnDisk = "cifxsdk.so";
#endif //_WIN32
		if (!m_sCustomSDKResource.empty())
			sResourceName = m_sCustomSDKResource;

		m_pWorkingDirectory = m_pDriverEnvironment->CreateWorkingDirectory();
		std::vector<uint8_t> DataBuffer;

		if (m_pDriverEnvironment->MachineHasResourceData(sResourceName)) {

			m_pDriverEnvironment->RetrieveMachineResourceData(sResourceName, DataBuffer);

		}
		else {

			m_pDriverEnvironment->RetrieveDriverResourceData(sResourceName, DataBuffer);
		}

		if (DataBuffer.empty())
			throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDSDKRESOURCE);

		m_pCifXDLLFile = m_pWorkingDirectory->StoreCustomData(sFileNameOnDisk, DataBuffer);

		m_pCifXSDK = std::make_shared<CCifXSDK>(m_pCifXDLLFile->GetAbsoluteFileName ());

	}
}


LibMCDriver_CifX_uint32 CDriver_CifX::EnumerateBoards()
{
	LoadSDKIfNeeded();

	throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_NOTIMPLEMENTED);
}

IBoardInformation * CDriver_CifX::GetBoardInformation(const LibMCDriver_CifX_uint32 nBoardIndex)
{
	throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_NOTIMPLEMENTED);
}



void CDriver_CifX::Connect()
{
	LoadSDKIfNeeded();

	Disconnect();
	m_pCifXSDK->checkError (m_pCifXSDK->xDriverOpen (&m_hDriverHandle));

	for (auto pChannel : m_Channels) {
		pChannel->startSyncThread (m_pCifXSDK, m_hDriverHandle);
	}

}

void CDriver_CifX::Disconnect()
{
	if (m_hDriverHandle != nullptr) {
		for (auto pChannel : m_Channels) {
			pChannel->stopSyncThread();
		}

		m_pCifXSDK->xDriverClose(m_hDriverHandle);
		m_hDriverHandle = nullptr;
	}

}

bool CDriver_CifX::IsConnected()
{
	return (m_hDriverHandle != nullptr);
}

bool CDriver_CifX::ValueExists(const std::string& sName)
{
	auto iInputIter = m_GlobalInputMap.find(sName);
	if (iInputIter != m_GlobalInputMap.end())
		return true;

	auto iOutputIter = m_GlobalOutputMap.find(sName);
	if (iOutputIter != m_GlobalOutputMap.end())
		return true;

	return false;
}

void CDriver_CifX::WriteIntegerValue(const std::string& sName, const LibMCDriver_CifX_int64 nValue, const LibMCDriver_CifX_uint32 nTimeOutInMs)
{
	auto iOutputIter = m_GlobalOutputMap.find(sName);
	if (iOutputIter == m_GlobalOutputMap.end())
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_OUTPUTVALUENOTFOUND, "output value not found: " + sName);

	auto pValue = iOutputIter->second;
	pValue->SetTargetIntegerValue (nValue);

}

void CDriver_CifX::WriteBoolValue(const std::string& sName, const bool bValue, const LibMCDriver_CifX_uint32 nTimeOutInMs)
{
	auto iOutputIter = m_GlobalOutputMap.find(sName);
	if (iOutputIter == m_GlobalOutputMap.end())
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_OUTPUTVALUENOTFOUND, "output value not found: " + sName);

	auto pValue = iOutputIter->second;
	if (bValue)
		pValue->SetTargetIntegerValue(1);
	else
		pValue->SetTargetIntegerValue(0);

}

void CDriver_CifX::WriteDoubleValue(const std::string& sName, const LibMCDriver_CifX_double dValue, const LibMCDriver_CifX_uint32 nTimeOutInMs)
{
	auto iOutputIter = m_GlobalOutputMap.find(sName);
	if (iOutputIter == m_GlobalOutputMap.end())
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_OUTPUTVALUENOTFOUND, "output value not found: " + sName);

	auto pValue = iOutputIter->second;
	pValue->SetTargetDoubleValue(dValue);

}

LibMCDriver_CifX_int64 CDriver_CifX::ReadIntegerValue(const std::string& sName)
{
	auto iOutputIter = m_GlobalOutputMap.find(sName);
	if (iOutputIter != m_GlobalOutputMap.end()) {
		auto pValue = iOutputIter->second;
		return pValue->GetActualIntegerValue();
	}

	auto iInputIter = m_GlobalInputMap.find(sName);
	if (iInputIter != m_GlobalInputMap.end()) {
		auto pValue = iInputIter->second;
		return pValue->GetActualIntegerValue();
	}

	throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_VALUENOTFOUND, "value not found: " + sName);
}

bool CDriver_CifX::ReadBoolValue(const std::string& sName)
{
	auto iOutputIter = m_GlobalOutputMap.find(sName);
	if (iOutputIter != m_GlobalOutputMap.end()) {
		auto pValue = iOutputIter->second;
		return pValue->GetActualIntegerValue() != 0;
	}

	auto iInputIter = m_GlobalInputMap.find(sName);
	if (iInputIter != m_GlobalInputMap.end()) {
		auto pValue = iInputIter->second;
		return pValue->GetActualIntegerValue() != 0;
	}

	throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_VALUENOTFOUND, "value not found: " + sName);
}

LibMCDriver_CifX_double CDriver_CifX::ReadDoubleValue(const std::string& sName)
{
	auto iOutputIter = m_GlobalOutputMap.find(sName);
	if (iOutputIter != m_GlobalOutputMap.end()) {
		auto pValue = iOutputIter->second;
		return pValue->GetActualDoubleValue();
	}

	auto iInputIter = m_GlobalInputMap.find(sName);
	if (iInputIter != m_GlobalInputMap.end()) {
		auto pValue = iInputIter->second;
		return pValue->GetActualDoubleValue();
	}

	throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_VALUENOTFOUND, "value not found: " + sName);
}

