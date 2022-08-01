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

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
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

#include "libmcdriver_ads_client.hpp"
#include "libmcdriver_ads_interfaceexception.hpp"

#include <vector>
#include <cstring>
#include <stdexcept>

using namespace LibMCDriver_ADS::Impl;

CADSClientConnection::CADSClientConnection(PADSSDK pSDK, AdsPort nPort, sAmsAddr localAddress)
	: m_pSDK (pSDK), m_Port(nPort), m_LocalAddress (localAddress)
{
	if (pSDK.get() == nullptr)
		throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_INVALIDPARAM);

}

CADSSDK* CADSClientConnection::getSDK()
{
	if (m_pSDK.get() == nullptr)
		throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_NOTCONNECTED);

	return m_pSDK.get();
}

AdsPort CADSClientConnection::getPort()
{
	return m_Port;
}

sAmsAddr* CADSClientConnection::getAddressP()
{
	return &m_LocalAddress;
}

void CADSClientConnection::disconnect()
{
	if (m_pSDK.get() != nullptr)
	{
		auto pSDK = m_pSDK;
		m_pSDK = nullptr;

		auto nPort = m_Port;
		m_Port = 0;

		pSDK->checkError (pSDK->AdsPortCloseEx(nPort));
	}

	m_Port = 0;
	memset((void*) &m_LocalAddress, sizeof(m_LocalAddress), 0);
}


CADSClientVariable::CADSClientVariable(PADSClientConnection pConnection, const std::string& sName, uint32_t Handle)
	: m_pConnection(pConnection), m_sName (sName), m_Handle (Handle)
{
	if (pConnection.get() == nullptr)
		throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_INVALIDPARAM);
	if (m_sName.empty())
		throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_INVALIDPARAM);
}

CADSClientVariable::~CADSClientVariable()
{

}

std::string CADSClientVariable::getName()
{
	return m_sName;
}


void CADSClientVariable::readBuffer(void* pData, uint32_t nLength)
{
	if (pData == nullptr)
		throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_INVALIDPARAM);
	if (nLength == 0)
		throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_INVALIDPARAM);

	auto pSDK = m_pConnection->getSDK();

	uint32_t bytesRead = 0;
	pSDK->checkError (pSDK->AdsSyncReadReqEx2(m_pConnection->getPort(), m_pConnection->getAddressP(), ADSIGRP_SYM_VALBYHND, m_Handle, nLength, pData, &bytesRead));

	if (bytesRead != nLength)
		throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_COULDNOTREADDATA, "could not read data: " + m_sName);

}

void CADSClientVariable::writeBuffer(void* pData, uint32_t nLength)
{
	if (pData == nullptr)
		throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_INVALIDPARAM);
	if (nLength == 0)
		throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_INVALIDPARAM);

	auto pSDK = m_pConnection->getSDK();

	pSDK->checkError(pSDK->AdsSyncWriteReqEx(m_pConnection->getPort(), m_pConnection->getAddressP(), ADSIGRP_SYM_VALBYHND, m_Handle, nLength, pData));
}



CADSClientIntegerVariable::CADSClientIntegerVariable(PADSClientConnection pConnection, const std::string& sName, uint32_t Handle)
	: CADSClientVariable (pConnection, sName, Handle)
{

}

CADSClientIntegerVariable::~CADSClientIntegerVariable()
{

}

CADSClientFloatVariable::CADSClientFloatVariable(PADSClientConnection pConnection, const std::string& sName, uint32_t Handle)
	: CADSClientVariable(pConnection, sName, Handle)
{

}

CADSClientFloatVariable::~CADSClientFloatVariable()
{

}



CADSClientInt8Variable::CADSClientInt8Variable(PADSClientConnection pConnection, const std::string& sName, uint32_t Handle)
	: CADSClientIntegerVariable (pConnection, sName, Handle)
{

}

CADSClientInt8Variable::~CADSClientInt8Variable()
{

}

CADSClientBoolVariable::CADSClientBoolVariable(PADSClientConnection pConnection, const std::string& sName, uint32_t Handle)
	: CADSClientIntegerVariable(pConnection, sName, Handle)
{

}
	
CADSClientBoolVariable::~CADSClientBoolVariable()
{

}

int64_t CADSClientBoolVariable::readValueFromPLC()
{
	bool nValue = 0;
	readBuffer((void*)&nValue, sizeof(nValue));
	if (nValue != 0) {
		return 1;
	}
	else {
		return 0;
	}
}

void CADSClientBoolVariable::writeValueToPLC(const int64_t nValue)
{
	int8_t nValueToWrite = (nValue != 0) ? 1 : 0;
	writeBuffer((void*)&nValueToWrite, sizeof(nValueToWrite));
}

bool CADSClientBoolVariable::readBooleanValueFromPLC()
{
	return readValueFromPLC() != 0;
}

void CADSClientBoolVariable::writeBooleanValueToPLC(const bool bValue)
{
	if (bValue) {
		writeValueToPLC(1);
	}
	else
	{
		writeValueToPLC(0);
	}
}


int64_t CADSClientInt8Variable::readValueFromPLC()
{
	int8_t nValue = 0;
	readBuffer((void*)&nValue, sizeof(nValue));
	return (int64_t)nValue;
}

void CADSClientInt8Variable::writeValueToPLC(const int64_t nValue)
{
	if ((nValue < INT8_MIN) || (nValue > INT8_MAX))
		throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_INTEGERVALUEOUTOFRANGE);

	int8_t nValueToWrite = (int8_t)nValue;
	writeBuffer((void*)&nValueToWrite, sizeof(nValueToWrite));
}



CADSClientUint8Variable::CADSClientUint8Variable(PADSClientConnection pConnection, const std::string& sName, uint32_t Handle)
	: CADSClientIntegerVariable(pConnection, sName, Handle)
{

}

CADSClientUint8Variable::~CADSClientUint8Variable()
{

}

int64_t CADSClientUint8Variable::readValueFromPLC()
{
	uint8_t nValue = 0;
	readBuffer((void*)&nValue, sizeof(nValue));
	return (int64_t)nValue;
}

void CADSClientUint8Variable::writeValueToPLC(const int64_t nValue)
{
	if ((nValue < 0) || (nValue > UINT8_MAX))
		throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_INTEGERVALUEOUTOFRANGE);

	uint8_t nValueToWrite = (uint8_t)nValue;
	writeBuffer((void*)&nValueToWrite, sizeof(nValueToWrite));
}

CADSClientInt16Variable::CADSClientInt16Variable(PADSClientConnection pConnection, const std::string& sName, uint32_t Handle)
	: CADSClientIntegerVariable(pConnection, sName, Handle)
{

}

CADSClientInt16Variable::~CADSClientInt16Variable()
{

}


int64_t CADSClientInt16Variable::readValueFromPLC()
{
	int16_t nValue = 0;
	readBuffer((void*)&nValue, sizeof(nValue));
	return (int64_t)nValue;
}

void CADSClientInt16Variable::writeValueToPLC(const int64_t nValue)
{
	if ((nValue < INT16_MIN) || (nValue > INT16_MAX))
		throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_INTEGERVALUEOUTOFRANGE);

	int16_t nValueToWrite = (int16_t)nValue;
	writeBuffer((void*)&nValueToWrite, sizeof(nValueToWrite));
}



CADSClientUint16Variable::CADSClientUint16Variable(PADSClientConnection pConnection, const std::string& sName, uint32_t Handle)
	: CADSClientIntegerVariable(pConnection, sName, Handle)
{

}

CADSClientUint16Variable::~CADSClientUint16Variable()
{

}

int64_t CADSClientUint16Variable::readValueFromPLC()
{
	uint16_t nValue = 0;
	readBuffer((void*)&nValue, sizeof(nValue));
	return (int64_t)nValue;
}

void CADSClientUint16Variable::writeValueToPLC(const int64_t nValue)
{
	if ((nValue < 0) || (nValue > UINT16_MAX))
		throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_INTEGERVALUEOUTOFRANGE);

	uint16_t nValueToWrite = (uint16_t)nValue;
	writeBuffer((void*)&nValueToWrite, sizeof(nValueToWrite));
}



CADSClientInt32Variable::CADSClientInt32Variable(PADSClientConnection pConnection, const std::string& sName, uint32_t Handle)
	: CADSClientIntegerVariable(pConnection, sName, Handle)
{

}

CADSClientInt32Variable::~CADSClientInt32Variable()
{

}


int64_t CADSClientInt32Variable::readValueFromPLC()
{
	int32_t nValue = 0;
	readBuffer((void*)&nValue, sizeof(nValue));
	return (int64_t)nValue;
}

void CADSClientInt32Variable::writeValueToPLC(const int64_t nValue)
{
	if ((nValue < INT32_MIN) || (nValue > INT32_MAX))
		throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_INTEGERVALUEOUTOFRANGE);

	int32_t nValueToWrite = (int32_t)nValue;
	writeBuffer((void*)&nValueToWrite, sizeof(nValueToWrite));
}



CADSClientUint32Variable::CADSClientUint32Variable(PADSClientConnection pConnection, const std::string& sName, uint32_t Handle)
	: CADSClientIntegerVariable(pConnection, sName, Handle)
{

}

CADSClientUint32Variable::~CADSClientUint32Variable()
{

}

int64_t CADSClientUint32Variable::readValueFromPLC()
{
	uint32_t nValue = 0;
	readBuffer((void*)&nValue, sizeof(nValue));
	return (int64_t)nValue;
}

void CADSClientUint32Variable::writeValueToPLC(const int64_t nValue)
{
	if ((nValue < 0) || (nValue > UINT32_MAX))
		throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_INTEGERVALUEOUTOFRANGE);

	uint32_t nValueToWrite = (uint32_t)nValue;
	writeBuffer((void*)&nValueToWrite, sizeof(nValueToWrite));
}

CADSClientFloat32Variable::CADSClientFloat32Variable(PADSClientConnection pConnection, const std::string& sName, uint32_t Handle)
	: CADSClientFloatVariable(pConnection, sName, Handle)
{

}

CADSClientFloat32Variable::~CADSClientFloat32Variable()
{

}


double CADSClientFloat32Variable::readValueFromPLC()
{
	float fValue = 0.0;
	readBuffer((void*)&fValue, sizeof(fValue));
	return (double)fValue;
}

void CADSClientFloat32Variable::writeValueToPLC(const double dValue)
{
	float fValueToWrite = (float)dValue;
	writeBuffer((void*)&fValueToWrite, sizeof(fValueToWrite));
}

CADSClientFloat64Variable::CADSClientFloat64Variable(PADSClientConnection pConnection, const std::string& sName, uint32_t Handle)
	: CADSClientFloatVariable(pConnection, sName, Handle)
{

}

CADSClientFloat64Variable::~CADSClientFloat64Variable()
{

}


double CADSClientFloat64Variable::readValueFromPLC()
{
	double dValue = 0.0;
	readBuffer((void*)&dValue, sizeof(dValue));
	return (double)dValue;
}

void CADSClientFloat64Variable::writeValueToPLC(const double dValue)
{
	double dValueToWrite = (double)dValue;
	writeBuffer((void*)&dValueToWrite, sizeof(dValueToWrite));
}




CADSClient::CADSClient(PADSSDK pSDK)
	: m_pSDK(pSDK), m_Version({0, 0, 0})
{
	if (pSDK.get() == nullptr)
		throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_INVALIDPARAM);


	uint32_t version = pSDK->AdsGetDllVersion();
	m_Version = *((AdsVersion*)&version);
}

CADSClient::~CADSClient()
{

}

void CADSClient::connect(uint32_t nPortNumber)
{

	if (m_pCurrentConnection.get() != nullptr)
		disconnect();

	m_pCurrentConnection = nullptr;
	AdsPort nPortHandle = m_pSDK->AdsPortOpenEx();

	sAmsAddr localAddress;
	memset(&localAddress, sizeof(localAddress), 0);


	m_pSDK->checkError (m_pSDK->AdsGetLocalAddressEx(nPortHandle, &localAddress));
	 
	localAddress.m_Port = nPortNumber; 

	m_pCurrentConnection = std::make_shared<CADSClientConnection> (m_pSDK, nPortHandle, localAddress);
}

void CADSClient::disconnect()
{
	if (m_pCurrentConnection.get() != nullptr) {
		m_pCurrentConnection->disconnect();
		m_pCurrentConnection = nullptr;
	}
}


std::string CADSClient::getVersionString()
{
	return std::to_string(m_Version.m_Version) + "." + std::to_string(m_Version.m_Revision) + "." + std::to_string(m_Version.m_Build);
}


uint32_t CADSClient::getVariableHandle(const std::string& sName)
{
	if (m_pCurrentConnection.get() == nullptr)
		throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_NOTCONNECTED);

	uint32_t HandleValue = 0;
	uint32_t bytesRead = 0;

	if (sName.length() > ADS_MAXNAMELENGTH)
		throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_NAMELENGTHEXCEEDSMAXIMUM);

	auto pSDK = m_pCurrentConnection->getSDK();
	pSDK->checkError (pSDK->AdsSyncReadWriteReqEx2(m_pCurrentConnection->getPort(), m_pCurrentConnection->getAddressP(), ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(HandleValue), &HandleValue, (uint32_t)sName.length(), (void*)sName.c_str(), &bytesRead));

	if (bytesRead != sizeof(HandleValue))
		throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_COULDNOTREADHANDLEVALUE);

	return HandleValue;

}

PADSClientBoolVariable CADSClient::registerBoolVariable(const std::string& sName)
{
	if (m_pCurrentConnection.get() == nullptr)
		throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_NOTCONNECTED);

	auto pVariable = std::make_shared<CADSClientBoolVariable>(m_pCurrentConnection, sName, getVariableHandle(sName));
	registerVariable(pVariable);
	return pVariable;
}


PADSClientInt8Variable CADSClient::registerInt8Variable(const std::string& sName)
{
	if (m_pCurrentConnection.get() == nullptr)
		throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_NOTCONNECTED);

	auto pVariable = std::make_shared<CADSClientInt8Variable>(m_pCurrentConnection, sName, getVariableHandle (sName));
	registerVariable(pVariable);
	return pVariable;
}

PADSClientUint8Variable CADSClient::registerUint8Variable(const std::string& sName)
{
	if (m_pCurrentConnection.get() == nullptr)
		throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_NOTCONNECTED);

	auto pVariable = std::make_shared<CADSClientUint8Variable>(m_pCurrentConnection, sName, getVariableHandle(sName));
	registerVariable(pVariable);
	return pVariable;
}

PADSClientInt16Variable CADSClient::registerInt16Variable(const std::string& sName)
{
	if (m_pCurrentConnection.get() == nullptr)
		throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_NOTCONNECTED);

	auto pVariable = std::make_shared<CADSClientInt16Variable>(m_pCurrentConnection, sName, getVariableHandle(sName));
	registerVariable(pVariable);
	return pVariable;
}

PADSClientUint16Variable CADSClient::registerUint16Variable(const std::string& sName)
{
	if (m_pCurrentConnection.get() == nullptr)
		throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_NOTCONNECTED);

	auto pVariable = std::make_shared<CADSClientUint16Variable>(m_pCurrentConnection, sName, getVariableHandle(sName));
	registerVariable(pVariable);
	return pVariable;
}

PADSClientInt32Variable CADSClient::registerInt32Variable(const std::string& sName)
{
	if (m_pCurrentConnection.get() == nullptr)
		throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_NOTCONNECTED);

	auto pVariable = std::make_shared<CADSClientInt32Variable>(m_pCurrentConnection, sName, getVariableHandle(sName));
	registerVariable(pVariable);
	return pVariable;
}

PADSClientUint32Variable CADSClient::registerUint32Variable(const std::string& sName)
{
	if (m_pCurrentConnection.get() == nullptr)
		throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_NOTCONNECTED);

	auto pVariable = std::make_shared<CADSClientUint32Variable>(m_pCurrentConnection, sName, getVariableHandle(sName));
	registerVariable(pVariable);
	return pVariable;

}

PADSClientFloat32Variable CADSClient::registerFloat32Variable(const std::string& sName)
{
	if (m_pCurrentConnection.get() == nullptr)
		throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_NOTCONNECTED);

	auto pVariable = std::make_shared<CADSClientFloat32Variable>(m_pCurrentConnection, sName, getVariableHandle(sName));
	registerVariable(pVariable);
	return pVariable;
}

PADSClientFloat64Variable CADSClient::registerFloat64Variable(const std::string& sName)
{
	if (m_pCurrentConnection.get() == nullptr)
		throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_NOTCONNECTED);

	auto pVariable = std::make_shared<CADSClientFloat64Variable>(m_pCurrentConnection, sName, getVariableHandle(sName));
	registerVariable(pVariable);
	return pVariable;

}


void CADSClient::registerVariable(PADSClientVariable pVariable)
{
	if (pVariable.get () == nullptr)
		throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_INVALIDPARAM);

	m_Variables.push_back(pVariable);
	m_VariableMap.insert(std::make_pair(pVariable->getName (), pVariable));
}

CADSClientVariable* CADSClient::findVariable(const std::string& sName, bool bFailIfNotExisting)
{
	auto iIter = m_VariableMap.find(sName);
	if (iIter == m_VariableMap.end()) {
		if (bFailIfNotExisting)
			throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_VARIABLENOTFOUND);

		return nullptr;
	}

	return iIter->second.get();
}


CADSClientIntegerVariable* CADSClient::findIntegerVariable(const std::string& sName, bool bFailIfNotExisting)
{
	auto pVariable = findVariable(sName, bFailIfNotExisting);
	auto pIntegerVariable = dynamic_cast<CADSClientIntegerVariable*> (pVariable);
	if (pIntegerVariable == nullptr) {
		if (bFailIfNotExisting)
			throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_VARIABLEISNOTINTEGER);
	}

	return pIntegerVariable;

}


CADSClientBoolVariable* CADSClient::findBoolVariable(const std::string& sName, bool bFailIfNotExisting)
{
	auto pVariable = findVariable(sName, bFailIfNotExisting);
	auto pBoolVariable = dynamic_cast<CADSClientBoolVariable*> (pVariable);
	if (pBoolVariable == nullptr) {
		if (bFailIfNotExisting)
			throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_VARIABLEISNOTINTEGER);
	}

	return pBoolVariable;

}

CADSClientFloatVariable* CADSClient::findFloatVariable(const std::string& sName, bool bFailIfNotExisting)
{
	auto pVariable = findVariable(sName, bFailIfNotExisting);
	auto pFloatVariable = dynamic_cast<CADSClientFloatVariable*> (pVariable);
	if (pFloatVariable == nullptr) {
		if (bFailIfNotExisting)
			throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_VARIABLEISNOTINTEGER);
	}

	return pFloatVariable;

}
