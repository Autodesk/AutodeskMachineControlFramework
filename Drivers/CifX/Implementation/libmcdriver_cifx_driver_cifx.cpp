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
#include "libmcdriver_cifx_channelinformation.hpp"

// Include custom headers here.
#define __STRINGIZE(x) #x
#define __STRINGIZE_VALUE_OF(x) __STRINGIZE(x)

#include <iostream>


#define CIFX_CONFIGURATIONSCHEMA "http://schemas.autodesk.com/amc/cifxprotocol/2023/01"


using namespace LibMCDriver_CifX::Impl;

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

	pugi::xml_node configurationNode = doc.child("driverconfiguration");

	if (configurationNode.empty())
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDDRIVERPROTOCOL);

	pugi::xml_attribute xmlnsAttrib = configurationNode.attribute("xmlns");
	if (xmlnsAttrib.empty())
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_NOCONFIGURATIONSCHEMA);
	std::string sXMLNs = xmlnsAttrib.as_string();
	if (sXMLNs != CIFX_CONFIGURATIONSCHEMA)
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDCONFIGURATIONSCHEMA);

	pugi::xml_node versionNode = configurationNode.child("version");
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

	auto channelNodes = configurationNode.children("channel");
	for (pugi::xml_node channelNode : channelNodes)
	{
		PDriver_CifXChannel pChannel = std::make_shared<CDriver_CifXChannel>(channelNode);
		m_Channels.push_back(pChannel);

		pChannel->RegisterVariables(m_pDriverEnvironment);

		uint32_t nInputCount = pChannel->getInputCount();
		for (uint32_t nInputIndex = 0; nInputIndex < nInputCount; nInputIndex++) {
			auto pInput = pChannel->getInputByIndex(nInputIndex);
			std::string sName = pInput->getName();
			if (m_GlobalInputMap.find (sName) != m_GlobalInputMap.end ())
				throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_DUPLICATEINPUTIO, "duplicate input io: " + sName);
			m_GlobalInputMap.insert(std::make_pair (sName, pInput));
		}

		uint32_t nOutputCount = pChannel->getOutputCount();
		for (uint32_t nOutputIndex = 0; nOutputIndex < nOutputCount; nOutputIndex++) {
			auto pOutput = pChannel->getOutputByIndex(nOutputIndex);

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


void CDriver_CifX::QueryParameters()
{
	QueryParametersEx(m_pDriverEnvironment->CreateStatusUpdateSession());
}


void CDriver_CifX::QueryParametersEx(LibMCEnv::PDriverStatusUpdateSession pDriverUpdateInstance) 
{
	if (pDriverUpdateInstance.get() == nullptr)
		return;

	if (IsConnected()) {
		for (auto pOutputValueIter : m_GlobalOutputMap) {
			auto pOutputValue = pOutputValueIter.second;
			switch (pOutputValue->getAbstractType()) {
			case eDriver_AbstractParameterType::CifXAbstractParameter_BOOL:
				pDriverUpdateInstance->SetBoolParameter(pOutputValue->getName(), pOutputValue->GetActualBoolValue());
				break;

			case eDriver_AbstractParameterType::CifXAbstractParameter_INT:
				pDriverUpdateInstance->SetIntegerParameter(pOutputValue->getName(), pOutputValue->GetActualIntegerValue());
				break;

			case eDriver_AbstractParameterType::CifXAbstractParameter_DOUBLE:
				pDriverUpdateInstance->SetDoubleParameter(pOutputValue->getName(), pOutputValue->GetActualDoubleValue());
				break;

			}
			
		}
		
		for (auto pInputValueIter : m_GlobalInputMap) {
			auto pInputValue = pInputValueIter.second;
			switch (pInputValue->getAbstractType()) {
				case eDriver_AbstractParameterType::CifXAbstractParameter_BOOL:
					pDriverUpdateInstance->SetBoolParameter(pInputValue->getName(), pInputValue->GetActualBoolValue());
					break;

				case eDriver_AbstractParameterType::CifXAbstractParameter_INT:
					pDriverUpdateInstance->SetIntegerParameter(pInputValue->getName(), pInputValue->GetActualIntegerValue());
					break;

				case eDriver_AbstractParameterType::CifXAbstractParameter_DOUBLE:
					pDriverUpdateInstance->SetDoubleParameter(pInputValue->getName(), pInputValue->GetActualDoubleValue());
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


LibMCDriver_CifX_uint32 CDriver_CifX::GetChannelCount()
{
	return (uint32_t)m_Channels.size();
}

IChannelInformation* CDriver_CifX::GetChannelInformation(const LibMCDriver_CifX_uint32 nChannelIndex)
{
	if (nChannelIndex >= m_Channels.size ())
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDCHANNELINDEX);

	return new CChannelInformation(m_Channels.at (nChannelIndex));
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





void CDriver_CifX::Connect()
{
	LoadSDKIfNeeded();

	Disconnect();

	if (!m_bIsSimulationMode) {
		m_pCifXSDK->checkError(m_pCifXSDK->xDriverOpen(&m_hDriverHandle));

		for (auto pChannel : m_Channels) {
			pChannel->startSyncThread(m_pCifXSDK, m_hDriverHandle);
		}
	}

}

void CDriver_CifX::Reconnect()
{
	Disconnect();

	Connect();
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
	if (m_bIsSimulationMode)
		return true;

	if (m_hDriverHandle != nullptr) {
		for (auto pChannel : m_Channels) {
			if (!pChannel->isConnected())
				return false;
		}

		return true;

	}
	else {
		return false;
	}


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

void CDriver_CifX::GetValueType(const std::string& sName, LibMCDriver_CifX::eValueType& eValueType, bool& bIsInput, bool& bIsOutput)
{

}

void CDriver_CifX::GetIntegerValueRange(const std::string& sName, LibMCDriver_CifX_int64& nMinValue, LibMCDriver_CifX_int64& nMaxValue) 
{
	nMinValue = 0;
	nMaxValue = 0;

	auto iOutputIter = m_GlobalOutputMap.find(sName);
	if (iOutputIter != m_GlobalOutputMap.end()) {
		auto pValue = iOutputIter->second;
		pValue->getValueRange (nMinValue, nMaxValue);
		return;
	}


	auto iInputIter = m_GlobalInputMap.find(sName);
	if (iInputIter != m_GlobalInputMap.end()) {
		auto pValue = iInputIter->second;
		pValue->getValueRange(nMinValue, nMaxValue);
		return;
	}

	throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_VALUENOTFOUND, "value not found: " + sName);

}

void CDriver_CifX::WriteIntegerValue(const std::string& sName, const LibMCDriver_CifX_int64 nValue, const bool bClampToRange, const LibMCDriver_CifX_uint32 nTimeOutInMs) 
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

