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


Abstract: This is a stub class definition of CChannelInformation

*/

#include "libmcdriver_cifx_channel.hpp"
#include "libmcdriver_cifx_interfaceexception.hpp"

#include <iostream>

#define CIFX_MAXINPUTSIZE (1024 * 1024)
#define CIFX_MAXOUTPUTSIZE (1024 * 1024)

#define CIFX_DEFAULT_HOSTSTATETIMEOUT 5000
#define CIFX_DEFAULT_BUSSTATETIMEOUT 2000
#define CIFX_DEFAULT_SYNCDELAY 10

using namespace LibMCDriver_CifX::Impl;


PDriver_CifXParameter readParameterFromXMLNode(pugi::xml_node& node)
{
	std::string sNodeName = node.name();

	auto addressAttrib = node.attribute("address");
	auto bitAttrib = node.attribute("bit");
	auto nameAttrib = node.attribute("name");
	auto descriptionAttrib = node.attribute("description");
	auto endianessAttrib = node.attribute("endianess");

	if (addressAttrib.empty())
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_MISSINGADDRESSATTRIBUTE);
	int32_t nAddress = addressAttrib.as_int(-1);
	if (nAddress < 0)
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDADDRESSATTRIBUTE);

	int32_t nBit = bitAttrib.as_int(0);

	std::string sName = nameAttrib.as_string();
	std::string sDescription = descriptionAttrib.as_string();

	if (sName.empty())
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDNAMEATTRIBUTE);

	PDriver_CifXParameter pParameter;

	if (sNodeName == "bool")
		pParameter = std::make_shared<CDriver_CifXParameter_Bool>(sName, sDescription, eDriver_CifXParameterType::CifXParameter_BOOL, nAddress, nBit);
	if (sNodeName == "uint8")
		pParameter = std::make_shared<CDriver_CifXParameter_Integer>(sName, sDescription, eDriver_CifXParameterType::CifXParameter_UINT8, nAddress);
	if (sNodeName == "uint16")
		pParameter = std::make_shared<CDriver_CifXParameter_Integer>(sName, sDescription, eDriver_CifXParameterType::CifXParameter_UINT16, nAddress);
	if (sNodeName == "uint32")
		pParameter = std::make_shared<CDriver_CifXParameter_Integer>(sName, sDescription, eDriver_CifXParameterType::CifXParameter_UINT32, nAddress);
	if (sNodeName == "int8")
		pParameter = std::make_shared<CDriver_CifXParameter_Integer>(sName, sDescription, eDriver_CifXParameterType::CifXParameter_INT8, nAddress);
	if (sNodeName == "int16")
		pParameter = std::make_shared<CDriver_CifXParameter_Integer>(sName, sDescription, eDriver_CifXParameterType::CifXParameter_INT16, nAddress);
	if (sNodeName == "int32")
		pParameter = std::make_shared<CDriver_CifXParameter_Integer>(sName, sDescription, eDriver_CifXParameterType::CifXParameter_INT32, nAddress);

	if (pParameter.get () == nullptr)
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDPARAMETERTYPE, "invalid parameter type: " + sNodeName);

	if (!endianessAttrib.empty()) {
		std::string sEndianess = endianessAttrib.as_string();
		if (sEndianess == "littleendian") {
			pParameter->setIsBigEndian(false);
		} 
		else if (sEndianess == "bigendian") {
			pParameter->setIsBigEndian(true);
		}
		else
			throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDENDIANESSATTRIBUTE, "invalid endianess attribute: " + sNodeName);
	}

	return pParameter;

}



CDriver_CifXChannelBuffer::CDriver_CifXChannelBuffer(uint32_t nSize)
{
	m_Data.resize(nSize);
	clear();

}

CDriver_CifXChannelBuffer::~CDriver_CifXChannelBuffer()
{

}

void CDriver_CifXChannelBuffer::clear()
{
	for (auto& iIter : m_Data)
		iIter = 0;
} 

void CDriver_CifXChannelBuffer::readEndianValue(uint32_t nAddress, uint32_t nSize, bool bIsBigEndian, CDriver_CifXChannelEndianValue& outputValue)
{
	if ((nSize == 0) || (nSize > 8))
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDPARAM);
	if (((size_t)nAddress + nSize) > m_Data.size())
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDADDRESSREAD);

	if (bIsBigEndian) {
		for (uint32_t nIndex = 0; nIndex < nSize; nIndex++)
			outputValue.at(nSize - 1 - nIndex) = m_Data.at((size_t)nAddress + nIndex);

	}
	else {
		for (uint32_t nIndex = 0; nIndex < nSize; nIndex++)
			outputValue.at(nIndex) = m_Data.at((size_t)nAddress + nIndex);
	}

}

void CDriver_CifXChannelBuffer::writeEndianValue(uint32_t nAddress, uint32_t nSize, bool bIsBigEndian, uint8_t* pValue)
{
	if (((size_t)nAddress + nSize) > m_Data.size())
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDADDRESSWRITE);

	if (bIsBigEndian) {
		for (uint32_t nIndex = 0; nIndex < nSize; nIndex++)
			m_Data.at((size_t)nAddress + nIndex) = pValue[nSize - 1 - nIndex];

	}
	else {
		for (uint32_t nIndex = 0; nIndex < nSize; nIndex++)
			m_Data.at((size_t)nAddress + nIndex) = pValue[nIndex];
	}

}




uint8_t CDriver_CifXChannelBuffer::readUint8(uint32_t nAddress)
{
	if (((size_t)nAddress + sizeof(uint8_t)) > m_Data.size())
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDADDRESSREAD);

	return *((uint8_t*)&m_Data.at(nAddress));
}

uint16_t CDriver_CifXChannelBuffer::readUint16(uint32_t nAddress, bool bReadBigEndian)
{
	CDriver_CifXChannelEndianValue outputValue;
	readEndianValue(nAddress, sizeof(uint16_t), bReadBigEndian, outputValue);

	return *((uint16_t*)outputValue.data ());

}

uint32_t CDriver_CifXChannelBuffer::readUint32(uint32_t nAddress, bool bReadBigEndian)
{
	CDriver_CifXChannelEndianValue outputValue;
	readEndianValue(nAddress, sizeof(uint32_t), bReadBigEndian, outputValue);

	return *((uint32_t*)outputValue.data());

}

int8_t CDriver_CifXChannelBuffer::readInt8(uint32_t nAddress)
{
	if (((size_t)nAddress + sizeof(int8_t)) > m_Data.size())
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDADDRESSREAD);

	return *((int8_t*)&m_Data.at(nAddress));

}

int16_t CDriver_CifXChannelBuffer::readInt16(uint32_t nAddress, bool bReadBigEndian)
{
	CDriver_CifXChannelEndianValue outputValue;
	readEndianValue(nAddress, sizeof(int16_t), bReadBigEndian, outputValue);

	return *((int16_t*)outputValue.data());

}

int32_t CDriver_CifXChannelBuffer::readInt32(uint32_t nAddress, bool bReadBigEndian)
{
	CDriver_CifXChannelEndianValue outputValue;
	readEndianValue(nAddress, sizeof(int32_t), bReadBigEndian, outputValue);

	return *((int32_t*)outputValue.data());

}

bool CDriver_CifXChannelBuffer::readBool(uint32_t nAddress, uint32_t nBit)
{
	if (((size_t)nAddress + sizeof(uint8_t)) > m_Data.size())
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDADDRESSREAD);
	if (nBit >= 8)
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDADDRESSREADBIT);

	uint8_t nValue = m_Data.at(nAddress);

	return ((nValue & (1UL << nBit)) != 0);
}

float CDriver_CifXChannelBuffer::readFloat(uint32_t nAddress, bool bReadBigEndian)
{
	CDriver_CifXChannelEndianValue outputValue;
	readEndianValue(nAddress, sizeof(float), bReadBigEndian, outputValue);

	return *((float*)outputValue.data());

}

double CDriver_CifXChannelBuffer::readDouble(uint32_t nAddress, bool bReadBigEndian)
{
	CDriver_CifXChannelEndianValue outputValue;
	readEndianValue(nAddress, sizeof(double), bReadBigEndian, outputValue);

	return *((double*)outputValue.data());

}

void CDriver_CifXChannelBuffer::writeUint8(uint32_t nAddress, uint8_t nValue)
{
	writeEndianValue(nAddress, sizeof(int8_t), false, (uint8_t*)&nValue);


}

void CDriver_CifXChannelBuffer::writeUint16(uint32_t nAddress, uint16_t nValue, bool bWriteBigEndian)
{
	writeEndianValue(nAddress, sizeof(uint16_t), bWriteBigEndian, (uint8_t*)&nValue);

}

void CDriver_CifXChannelBuffer::writeUint32(uint32_t nAddress, uint32_t nValue, bool bWriteBigEndian)
{
	writeEndianValue(nAddress, sizeof(uint32_t), bWriteBigEndian, (uint8_t*)&nValue);
}

void CDriver_CifXChannelBuffer::writeInt8(uint32_t nAddress, int8_t nValue)
{
	writeEndianValue(nAddress, sizeof(int8_t), false, (uint8_t*)&nValue);

}

void CDriver_CifXChannelBuffer::writeInt16(uint32_t nAddress, int16_t nValue, bool bWriteBigEndian)
{
	writeEndianValue(nAddress, sizeof(int16_t), bWriteBigEndian, (uint8_t*)&nValue);

}

void CDriver_CifXChannelBuffer::writeInt32(uint32_t nAddress, int32_t nValue, bool bWriteBigEndian)
{
	writeEndianValue(nAddress, sizeof(int32_t), bWriteBigEndian, (uint8_t*)&nValue);

}

void CDriver_CifXChannelBuffer::writeBool(uint32_t nAddress, uint32_t nBit, bool bValue)
{
	if (((size_t)nAddress + sizeof(uint8_t)) > m_Data.size())
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDADDRESSWRITE);
	if (nBit >= 8)
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDADDRESSWRITEBIT);

	if (bValue) {
		m_Data.at(nAddress) |= (1UL << nBit);
	}
	else {
		m_Data.at(nAddress) &= (0xff ^ (1UL << nBit));
	}
	

}

void CDriver_CifXChannelBuffer::writeFloat(uint32_t nAddress, float fValue, bool bWriteBigEndian)
{
	writeEndianValue(nAddress, sizeof(float), bWriteBigEndian, (uint8_t*)&fValue);

}

void CDriver_CifXChannelBuffer::writeDouble(uint32_t nAddress, double dValue, bool bWriteBigEndian)
{
	writeEndianValue(nAddress, sizeof(double), bWriteBigEndian, (uint8_t*)&dValue);

}



std::vector<uint8_t>& CDriver_CifXChannelBuffer::getBuffer()
{
	return m_Data;
}


CDriver_CifXChannelThreadState::CDriver_CifXChannelThreadState(PCifXSDK pCifXSDK, uint32_t nInputSize, uint32_t nOutputSize, cifxHandle hChannel)
	: m_pCifXSDK(pCifXSDK), m_hChannel(hChannel), m_bDebugMode(true), m_InputBuffer(nInputSize), m_OutputBuffer(nOutputSize), m_bThreadIsRunning (false)
{
	if (pCifXSDK.get() == nullptr)
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDPARAM);
	if (hChannel == nullptr)
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDPARAM);

	if (nInputSize > CIFX_MAXINPUTSIZE)
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDINPUTIOSIZEATTRIBUTE);
	if (nOutputSize > CIFX_MAXOUTPUTSIZE)
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDOUTPUTIOSIZEATTRIBUTE);


}

CDriver_CifXChannelThreadState::~CDriver_CifXChannelThreadState()
{

}

void CDriver_CifXChannelThreadState::executeThread(uint32_t nReadTimeOut, uint32_t nWriteTimeOut)
{
	std::lock_guard<std::mutex> lockGuard(m_Mutex);
	auto& inputBuffer = m_InputBuffer.getBuffer();
	if (inputBuffer.size() > 0)
		m_pCifXSDK->checkError(m_pCifXSDK->xChannelIORead(m_hChannel, 0, 0, (uint32_t)inputBuffer.size(), inputBuffer.data(), nReadTimeOut));
	auto& outputBuffer = m_OutputBuffer.getBuffer();
	if (outputBuffer.size() > 0)
		m_pCifXSDK->checkError(m_pCifXSDK->xChannelIOWrite(m_hChannel, 0, 0, (uint32_t)outputBuffer.size(), outputBuffer.data(), nWriteTimeOut));

}

void CDriver_CifXChannelThreadState::handleException(uint32_t nErrorCode, const std::string& sMessage)
{
	if (m_bDebugMode) {
		std::cout << "CifX: An exception occured: " << sMessage << std::endl;
	}
	m_Exceptions.push_back(std::make_pair(nErrorCode, sMessage));

}

bool CDriver_CifXChannelThreadState::threadShallBeCanceled()
{
	return m_bCancelFlag;
}

void CDriver_CifXChannelThreadState::setThreadIsRunning(bool bThreadIsRunning)
{
	m_bThreadIsRunning = bThreadIsRunning;
}

bool CDriver_CifXChannelThreadState::isConnected()
{
	return m_bThreadIsRunning;
}


void CDriver_CifXChannelThreadState::stopThread(uint32_t nHostStateTimeOut, uint32_t nBusStateTimeOut)
{
	m_bCancelFlag = true;

	std::lock_guard<std::mutex> lockGuard(m_Mutex);
	if ((m_hChannel != nullptr) && (m_pCifXSDK.get() != nullptr)) {

		uint32_t nState = 0;
		m_pCifXSDK->xChannelBusState(m_hChannel, CIFX_BUS_STATE_OFF, &nState, nHostStateTimeOut);
		m_pCifXSDK->xChannelHostState(m_hChannel, CIFX_HOST_STATE_NOT_READY, &nState, nBusStateTimeOut);
		m_pCifXSDK->xChannelClose(m_hChannel);
		m_hChannel = nullptr;
	}
	
}


void CDriver_CifXChannelThreadState::readInputParameter(CDriver_CifXParameter* pParameter)
{
	if (pParameter == nullptr)
		return;

	std::lock_guard<std::mutex> lockGuard(m_Mutex);
	uint32_t nAddress = pParameter->getAddress();
	CDriver_CifXParameter_Bool* pBoolParameter;

	switch (pParameter->getType()) {
	case eDriver_CifXParameterType::CifXParameter_BOOL:
		pBoolParameter = dynamic_cast<CDriver_CifXParameter_Bool*> (pParameter);
		if (pBoolParameter == nullptr)
			throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDBOOLPARAMETERCAST);
		pParameter->SetActualBoolValue(m_InputBuffer.readBool(nAddress, pBoolParameter->getBit()));
		break;
	case eDriver_CifXParameterType::CifXParameter_UINT8:
		pParameter->SetActualIntegerValue(m_InputBuffer.readUint8(nAddress));
		break;

	case eDriver_CifXParameterType::CifXParameter_UINT16:
		pParameter->SetActualIntegerValue(m_InputBuffer.readUint16(nAddress, pParameter->isBigEndian()));
		break;

	case eDriver_CifXParameterType::CifXParameter_UINT32:
		pParameter->SetActualIntegerValue(m_InputBuffer.readUint32(nAddress, pParameter->isBigEndian()));
		break;

	case eDriver_CifXParameterType::CifXParameter_INT8:
		pParameter->SetActualIntegerValue(m_InputBuffer.readInt8(nAddress));
		break;

	case eDriver_CifXParameterType::CifXParameter_INT16:
		pParameter->SetActualIntegerValue(m_InputBuffer.readInt16(nAddress, pParameter->isBigEndian()));
		break;

	case eDriver_CifXParameterType::CifXParameter_INT32:
		pParameter->SetActualIntegerValue(m_InputBuffer.readInt32(nAddress, pParameter->isBigEndian()));
		break;

	case eDriver_CifXParameterType::CifXParameter_FLOAT:
		pParameter->SetActualDoubleValue(m_InputBuffer.readFloat(nAddress, pParameter->isBigEndian()));
		break;

	case eDriver_CifXParameterType::CifXParameter_DOUBLE:
		pParameter->SetActualDoubleValue(m_InputBuffer.readDouble(nAddress, pParameter->isBigEndian()));
		break;
	}


}

void CDriver_CifXChannelThreadState::readOutputParameter(CDriver_CifXParameter* pParameter)
{
	if (pParameter == nullptr)
		return;

	std::lock_guard<std::mutex> lockGuard(m_Mutex);
	uint32_t nAddress = pParameter->getAddress();
	CDriver_CifXParameter_Bool* pBoolParameter;

	switch (pParameter->getType()) {
	case eDriver_CifXParameterType::CifXParameter_BOOL:
		pBoolParameter = dynamic_cast<CDriver_CifXParameter_Bool*> (pParameter);
		if (pBoolParameter == nullptr)
			throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDBOOLPARAMETERCAST);
		pParameter->SetActualBoolValue(m_OutputBuffer.readBool(nAddress, pBoolParameter->getBit()));
		break;
	case eDriver_CifXParameterType::CifXParameter_UINT8:
		pParameter->SetActualIntegerValue(m_OutputBuffer.readUint8(nAddress));
		break;

	case eDriver_CifXParameterType::CifXParameter_UINT16:
		pParameter->SetActualIntegerValue(m_OutputBuffer.readUint16(nAddress, pParameter->isBigEndian()));
		break;

	case eDriver_CifXParameterType::CifXParameter_UINT32:
		pParameter->SetActualIntegerValue(m_OutputBuffer.readUint32(nAddress, pParameter->isBigEndian()));
		break;

	case eDriver_CifXParameterType::CifXParameter_INT8:
		pParameter->SetActualIntegerValue(m_OutputBuffer.readInt8(nAddress));
		break;

	case eDriver_CifXParameterType::CifXParameter_INT16:
		pParameter->SetActualIntegerValue(m_OutputBuffer.readInt16(nAddress, pParameter->isBigEndian()));
		break;

	case eDriver_CifXParameterType::CifXParameter_INT32:
		pParameter->SetActualIntegerValue(m_OutputBuffer.readInt32(nAddress, pParameter->isBigEndian()));
		break;

	case eDriver_CifXParameterType::CifXParameter_FLOAT:
		pParameter->SetActualDoubleValue(m_OutputBuffer.readFloat(nAddress, pParameter->isBigEndian()));
		break;

	case eDriver_CifXParameterType::CifXParameter_DOUBLE:
		pParameter->SetActualDoubleValue(m_OutputBuffer.readDouble(nAddress, pParameter->isBigEndian()));
		break;
	}

}

void CDriver_CifXChannelThreadState::writeOutputParameter(CDriver_CifXParameter* pParameter)
{
	if (pParameter == nullptr)
		return;

	std::lock_guard<std::mutex> lockGuard(m_Mutex);
	uint32_t nAddress = pParameter->getAddress();
	CDriver_CifXParameter_Bool* pBoolParameter;
	int64_t nValue;
	int64_t nMinValue = 0;
	int64_t nMaxValue = 0;
	double dValue;

	pParameter->getValueRange(nMinValue, nMaxValue);

	switch (pParameter->getType()) {
	case eDriver_CifXParameterType::CifXParameter_BOOL:
		pBoolParameter = dynamic_cast<CDriver_CifXParameter_Bool*> (pParameter);
		if (pBoolParameter == nullptr)
			throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDBOOLPARAMETERCAST);
		m_OutputBuffer.writeBool(nAddress, pBoolParameter->getBit(), pParameter->GetTargetBoolValue());
		break;
	case eDriver_CifXParameterType::CifXParameter_UINT8:
		nValue = pParameter->GetTargetIntegerValue();
		if ((nValue >= nMinValue) && (nValue <= nMaxValue))
			m_OutputBuffer.writeUint8(nAddress, (uint8_t)nValue);
		break;

	case eDriver_CifXParameterType::CifXParameter_UINT16:
		nValue = pParameter->GetTargetIntegerValue();
		if ((nValue >= nMinValue) && (nValue <= nMaxValue))
			m_OutputBuffer.writeUint16(nAddress, (uint16_t)nValue, pParameter->isBigEndian());
		break;

	case eDriver_CifXParameterType::CifXParameter_UINT32:
		nValue = pParameter->GetTargetIntegerValue();
		if ((nValue >= nMinValue) && (nValue <= nMaxValue))
			m_OutputBuffer.writeUint32(nAddress, (uint32_t)nValue, pParameter->isBigEndian());
		break;

	case eDriver_CifXParameterType::CifXParameter_INT8:
		nValue = pParameter->GetTargetIntegerValue();
		if ((nValue >= nMinValue) && (nValue <= nMaxValue))
			m_OutputBuffer.writeInt8(nAddress, (int8_t)nValue);
		break;

	case eDriver_CifXParameterType::CifXParameter_INT16:
		nValue = pParameter->GetTargetIntegerValue();
		if ((nValue >= nMinValue) && (nValue <= nMaxValue))
			m_OutputBuffer.writeInt16(nAddress, (int16_t)nValue, pParameter->isBigEndian());
		break;

	case eDriver_CifXParameterType::CifXParameter_INT32:
		nValue = pParameter->GetTargetIntegerValue();
		if ((nValue >= nMinValue) && (nValue <= nMaxValue))
			m_OutputBuffer.writeInt32(nAddress, (int32_t)nValue, pParameter->isBigEndian());
		break;

	case eDriver_CifXParameterType::CifXParameter_FLOAT:
		dValue = pParameter->GetTargetDoubleValue();
		m_OutputBuffer.writeFloat(nAddress, (float)dValue, pParameter->isBigEndian());
		break;

	case eDriver_CifXParameterType::CifXParameter_DOUBLE:
		dValue = pParameter->GetTargetDoubleValue();
		m_OutputBuffer.writeDouble(nAddress, dValue, pParameter->isBigEndian());
		break;
	}

}



CDriver_CifXChannel::CDriver_CifXChannel(pugi::xml_node& channelNode)
	: m_nChannelIndex(0), m_nInputSize(0), m_nOutputSize(0), m_nHostStateTimeOut(CIFX_DEFAULT_HOSTSTATETIMEOUT), m_nBusStateTimeOut(CIFX_DEFAULT_BUSSTATETIMEOUT), m_SyncDelay (CIFX_DEFAULT_SYNCDELAY)

{
	auto boardAttrib = channelNode.attribute("board");
	auto channelIndexAttrib = channelNode.attribute("channelindex");

	m_sBoardName = boardAttrib.as_string();
	if (m_sBoardName.empty())
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_NOBOARDATTRIBUTE);
	if (channelIndexAttrib.empty())
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_NOCHANNELINDEXATTRIBUTE);

	int nChannelIndex = channelIndexAttrib.as_int(-1);
	if (nChannelIndex < 0)
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDCHANNELINDEXATTRIBUTE);

	m_nChannelIndex = (uint32_t)nChannelIndex;

	auto hostStateTimeoutAttrib = channelNode.attribute("hoststatetimeout");
	if (!hostStateTimeoutAttrib.empty()) {
		int nHostStateTimeout = hostStateTimeoutAttrib.as_int(0);
		if (nHostStateTimeout <= 0)
			throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDHOSTSTATETIMEOUT);
		m_nHostStateTimeOut = nHostStateTimeout;
	}


	auto busStateTimeoutAttrib = channelNode.attribute("busstatetimeout");
	if (!busStateTimeoutAttrib.empty()) {
		int nBusStateTimeout = busStateTimeoutAttrib.as_int(0);
		if (nBusStateTimeout <= 0)
			throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDBUSSTATETIMEOUT);
		m_nBusStateTimeOut = nBusStateTimeout;
	}

	auto inputIONode = channelNode.child("input_io");
	if (!inputIONode.empty()) {

		auto sizeAttrib = inputIONode.attribute("size");
		if (sizeAttrib.empty())
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
				m_InputMap.insert(std::make_pair(pParameter->getName(), pParameter));
			}
		}
	}

	auto outputIONode = channelNode.child("output_io");
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
	if (pDriverEnvironment.get() != nullptr)
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

uint32_t CDriver_CifXChannel::getInputCount()
{
	return (uint32_t)m_Inputs.size();
}

uint32_t CDriver_CifXChannel::getOutputCount()
{
	return (uint32_t)m_Outputs.size();
}

PDriver_CifXParameter CDriver_CifXChannel::getInputByIndex(uint32_t nIndex)
{
	if ((size_t)nIndex >= m_Inputs.size())
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDINPUTINDEX);

	return m_Inputs.at(nIndex);
}

PDriver_CifXParameter CDriver_CifXChannel::getOutputByIndex(uint32_t nIndex)
{
	if ((size_t)nIndex >= m_Outputs.size())
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDOUTPUTINDEX);

	return m_Outputs.at(nIndex);

}

bool CDriver_CifXChannel::isConnected()
{
	if (m_pThreadState.get() != nullptr)
	{
		return m_pThreadState->isConnected();
	}

	return false;
}

PDriver_CifXParameter CDriver_CifXChannel::findInputValue(const std::string& sName)
{
	auto iIter = m_InputMap.find(sName);
	if (iIter != m_InputMap.end())
		return iIter->second;
	return nullptr;
}

PDriver_CifXParameter CDriver_CifXChannel::findOutputValue(const std::string& sName)
{
	auto iIter = m_OutputMap.find(sName);
	if (iIter != m_OutputMap.end())
		return iIter->second;
	return nullptr;
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
	uint32_t nSyncDelay = m_SyncDelay;

	auto pInputs = m_Inputs;
	auto pOutputs = m_Outputs;

	// First call should be in connection thread
	pThreadState->executeThread(nReadTimeout, nWriteTimeout);

	m_SyncThread = std::thread([pThreadState, pInputs, pOutputs, nReadTimeout, nWriteTimeout, nSyncDelay]() {

		pThreadState->setThreadIsRunning(true);

		try
		{
			while (!pThreadState->threadShallBeCanceled()) {

				for (auto pOutput : pOutputs)
					pThreadState->writeOutputParameter(pOutput.get());

				pThreadState->executeThread(nReadTimeout, nWriteTimeout);

				for (auto pInput : pInputs)
					pThreadState->readInputParameter(pInput.get());
				for (auto pOutput : pOutputs)
					pThreadState->readOutputParameter(pOutput.get());

				std::this_thread::sleep_for(std::chrono::milliseconds(nSyncDelay));
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

		pThreadState->setThreadIsRunning(false);

	});

}

void CDriver_CifXChannel::stopSyncThread()
{
	if (m_pThreadState.get() != nullptr) {
		m_pThreadState->stopThread(m_nHostStateTimeOut, m_nBusStateTimeOut);

		if (m_SyncThread.joinable())
			m_SyncThread.join();

		m_pThreadState = nullptr;
	}
}
