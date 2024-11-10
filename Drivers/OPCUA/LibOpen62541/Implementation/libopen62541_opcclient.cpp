/*++

Copyright (C) 2024 Autodesk Inc.

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


Abstract: This is a stub class definition of COPCClient

*/

#include "libopen62541_opcclient.hpp"
#include "libopen62541_interfaceexception.hpp"

#include <array>

using namespace LibOpen62541::Impl;

/*************************************************************************************************************************
 Class definition of COPCClient 
**************************************************************************************************************************/

COPCClient::COPCClient()
	: m_Client (nullptr),
	m_SecurityMode (LibOpen62541::eUASecurityMode::None) 
{

}

COPCClient::~COPCClient()
{
	if (m_Client != nullptr) {
		UA_Client_delete(m_Client);
		m_Client = nullptr;
	}
}

void COPCClient::EnableEncryption(const std::string& sLocalCertificate, const std::string& sPrivateKey, const LibOpen62541::eUASecurityMode eSecurityMode)
{
	m_sCertificate = sLocalCertificate;
	m_sPrivateKey = sPrivateKey;
	m_SecurityMode = eSecurityMode;
}

void COPCClient::DisableEncryption()
{
	m_sCertificate = "";
	m_sPrivateKey = "";
	m_SecurityMode = LibOpen62541::eUASecurityMode::None;
}

void COPCClient::ConnectUserName(const std::string & sEndPointURL, const std::string & sUsername, const std::string & sPassword, const std::string & sApplicationURL)
{
	UA_StatusCode statusCode;

	m_Client = UA_Client_new();
	if (m_Client == nullptr)
		throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_COULDNOTCREATEOPCUACLIENT);

	try {

		UA_ClientConfig* pConfig = UA_Client_getConfig(m_Client);

		UA_ClientConfig_setDefault(pConfig);

		switch (m_SecurityMode) {
			case LibOpen62541::eUASecurityMode::None:
				pConfig->securityMode = UA_MESSAGESECURITYMODE_NONE;
				break;
			case LibOpen62541::eUASecurityMode::Sign:
				pConfig->securityMode = UA_MESSAGESECURITYMODE_SIGN;
				break;
			case LibOpen62541::eUASecurityMode::SignAndEncrypt:
				pConfig->securityMode = UA_MESSAGESECURITYMODE_SIGNANDENCRYPT;
				break;
			default:
				throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_INVALIDSECURITYMODE);
		}

		UA_String_clear(&pConfig->clientDescription.applicationUri);
		pConfig->clientDescription.applicationUri = UA_STRING_ALLOC(sApplicationURL.c_str ());

		if (!m_sCertificate.empty()) {
			auto certificateString = UA_String_fromChars(m_sCertificate.c_str());
			auto privateKeyString = UA_String_fromChars(m_sPrivateKey.c_str());

			statusCode = UA_ClientConfig_setDefaultEncryption(pConfig, certificateString, privateKeyString, nullptr, 0, nullptr, 0);

			UA_String_clear(&certificateString);
			UA_String_clear(&privateKeyString);
		}
		else {
			statusCode = UA_ClientConfig_setDefault(pConfig);
		}

		if (statusCode != UA_STATUSCODE_GOOD)
			throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_COULDNOTSENDOPCUACLIENTCONFIG, "could not send OPCUA Client Config (" + std::to_string (statusCode) + ")");
		
		statusCode = UA_Client_connectUsername(m_Client, sEndPointURL.c_str (), sUsername.c_str (), sPassword.c_str ());
		if (statusCode != UA_STATUSCODE_GOOD)
			throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_COULDNOTCONNECT, "could not connect to OPCUA endpoint " + sEndPointURL +  " (" + std::to_string (statusCode) + ")");



	}
	catch (...)
	{
		Disconnect();
		throw;
	}


}

void COPCClient::Disconnect()
{
	if (m_Client != nullptr) {
		UA_Client_delete(m_Client);
		m_Client = nullptr;
	}
}

bool COPCClient::IsConnected()
{
	return (m_Client != nullptr);
}

LibOpen62541_int64 COPCClient::ReadInteger(const LibOpen62541_uint32 nNameSpace, const std::string & sNodeName, const LibOpen62541::eUAIntegerType eNodeType)
{
	UA_StatusCode statusCode;
	int64_t nResultValue = 0;

	if (m_Client == nullptr)
		throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_NOTCONNECTED, "OPCUA client is not connected");

	UA_Variant* variantValue = UA_Variant_new();
	try {
		std::vector<char> charBuffer (sNodeName.begin (), sNodeName.end ());
		charBuffer.push_back(0);

		statusCode = UA_Client_readValueAttribute(m_Client, UA_NODEID_STRING(nNameSpace, charBuffer.data()), variantValue);
		if (statusCode != UA_STATUSCODE_GOOD) 
			throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_COULDNOTREADVALUE, "could not read value " + sNodeName + " (" + std::to_string(statusCode) + ")");

		switch (eNodeType) {
			case eUAIntegerType::UAUInt8: {
				if (!UA_Variant_hasScalarType(variantValue, &UA_TYPES[UA_TYPES_BYTE]))
					throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_READVALUEISOFINCORRECTTYPE, "read value is of incorrect type. " + sNodeName + " was supposed to be Uint8 (" + std::to_string(statusCode) + ")");

				UA_Byte* pTypedValue = (UA_Byte*)variantValue->data;
				if (pTypedValue == nullptr)
					throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_OPCUAVARIANTDATAISNULL, "OPCUA variant data is null for " + sNodeName);

				nResultValue = *pTypedValue;
				break;
			}

			case eUAIntegerType::UAUInt16: {

				if (!UA_Variant_hasScalarType(variantValue, &UA_TYPES[UA_TYPES_UINT16])) 
					throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_READVALUEISOFINCORRECTTYPE, "read value is of incorrect type. " + sNodeName + " was supposed to be Uint16 (" + std::to_string(statusCode) + ")");

				UA_UInt16* pTypedValue = (UA_UInt16*)variantValue->data;
				if (pTypedValue == nullptr)
					throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_OPCUAVARIANTDATAISNULL, "OPCUA variant data is null for " + sNodeName);

				nResultValue = *pTypedValue;
				break;
			}

			case eUAIntegerType::UAUInt32: {
				if (!UA_Variant_hasScalarType(variantValue, &UA_TYPES[UA_TYPES_UINT32]))
					throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_READVALUEISOFINCORRECTTYPE, "read value is of incorrect type. " + sNodeName + " was supposed to be Uint32 (" + std::to_string(statusCode) + ")");

				UA_UInt32* pTypedValue = (UA_UInt32*)variantValue->data;
				if (pTypedValue == nullptr)
					throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_OPCUAVARIANTDATAISNULL, "OPCUA variant data is null for " + sNodeName);

				nResultValue = *pTypedValue;
				break;
			}

			case eUAIntegerType::UAUInt64: {
				if (!UA_Variant_hasScalarType(variantValue, &UA_TYPES[UA_TYPES_UINT64]))
					throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_READVALUEISOFINCORRECTTYPE, "read value is of incorrect type. " + sNodeName + " was supposed to be Uint64 (" + std::to_string(statusCode) + ")");

				UA_UInt64* pTypedValue = (UA_UInt64*)variantValue->data;
				if (pTypedValue == nullptr)
					throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_OPCUAVARIANTDATAISNULL, "OPCUA variant data is null for " + sNodeName);

				nResultValue = *pTypedValue;
				break;
			}

			case eUAIntegerType::UAInt8: {
				if (!UA_Variant_hasScalarType(variantValue, &UA_TYPES[UA_TYPES_SBYTE]))
					throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_READVALUEISOFINCORRECTTYPE, "read value is of incorrect type. " + sNodeName + " was supposed to be Int8 (" + std::to_string(statusCode) + ")");

				UA_SByte* pTypedValue = (UA_SByte*)variantValue->data;
				if (pTypedValue == nullptr)
					throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_OPCUAVARIANTDATAISNULL, "OPCUA variant data is null for " + sNodeName);

				nResultValue = *pTypedValue;
				break;
			}

			case eUAIntegerType::UAInt16: {
				if (!UA_Variant_hasScalarType(variantValue, &UA_TYPES[UA_TYPES_INT16]))
					throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_READVALUEISOFINCORRECTTYPE, "read value is of incorrect type. " + sNodeName + " was supposed to be Int16 (" + std::to_string(statusCode) + ")");

				UA_Int16* pTypedValue = (UA_Int16*)variantValue->data;
				if (pTypedValue == nullptr)
					throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_OPCUAVARIANTDATAISNULL, "OPCUA variant data is null for " + sNodeName);

				nResultValue = *pTypedValue;
				break;
			}

			case eUAIntegerType::UAInt32: {
				if (!UA_Variant_hasScalarType(variantValue, &UA_TYPES[UA_TYPES_INT32]))
					throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_READVALUEISOFINCORRECTTYPE, "read value is of incorrect type. " + sNodeName + " was supposed to be Int32 (" + std::to_string(statusCode) + ")");

				UA_Int32* pTypedValue = (UA_Int32*)variantValue->data;
				if (pTypedValue == nullptr)
					throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_OPCUAVARIANTDATAISNULL, "OPCUA variant data is null for " + sNodeName);

				nResultValue = *pTypedValue;
				break;
			}

			case eUAIntegerType::UAInt64: {
				if (!UA_Variant_hasScalarType(variantValue, &UA_TYPES[UA_TYPES_INT64]))
					throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_READVALUEISOFINCORRECTTYPE, "read value is of incorrect type. " + sNodeName + " was supposed to be Int64 (" + std::to_string(statusCode) + ")");

				UA_Int64* pTypedValue = (UA_Int64*)variantValue->data;
				if (pTypedValue == nullptr)
					throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_OPCUAVARIANTDATAISNULL, "OPCUA variant data is null for " + sNodeName);

				nResultValue = *pTypedValue;
				break;
			}

			default:
				throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_INVALIDINTEGERNODETYPE);

		}

		UA_Variant* valueToDelete = variantValue;
		variantValue = nullptr;

		UA_Variant_delete(valueToDelete);

	}
	catch (...) {
		if (variantValue != nullptr)
			UA_Variant_delete(variantValue);

		throw;
	}

	return nResultValue;


}

LibOpen62541_double COPCClient::ReadDouble(const LibOpen62541_uint32 nNameSpace, const std::string & sNodeName, const LibOpen62541::eUADoubleType eNodeType)
{
	UA_StatusCode statusCode;
	double dResultValue = 0;

	if (m_Client == nullptr)
		throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_NOTCONNECTED, "OPCUA client is not connected");

	UA_Variant* variantValue = UA_Variant_new();
	try {
		std::vector<char> charBuffer(sNodeName.begin(), sNodeName.end());
		charBuffer.push_back(0);

		statusCode = UA_Client_readValueAttribute(m_Client, UA_NODEID_STRING(nNameSpace, charBuffer.data()), variantValue);
		if (statusCode != UA_STATUSCODE_GOOD)
			throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_COULDNOTREADVALUE, "could not read value " + sNodeName + " (" + std::to_string(statusCode) + ")");

		switch (eNodeType) {
			case eUADoubleType::UAFloat32: {
				if (!UA_Variant_hasScalarType(variantValue, &UA_TYPES[UA_TYPES_FLOAT]))
					throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_READVALUEISOFINCORRECTTYPE, "read value is of incorrect type. " + sNodeName + " was supposed to be Float (" + std::to_string(statusCode) + ")");

				UA_Float* pTypedValue = (UA_Float*) variantValue->data;
				if (pTypedValue == nullptr)
					throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_OPCUAVARIANTDATAISNULL, "OPCUA variant data is null for " + sNodeName);

				dResultValue = *pTypedValue;
				break;
			}

			case eUADoubleType::UADouble64: {
				if (!UA_Variant_hasScalarType(variantValue, &UA_TYPES[UA_TYPES_FLOAT]))
					throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_READVALUEISOFINCORRECTTYPE, "read value is of incorrect type. " + sNodeName + " was supposed to be Float (" + std::to_string(statusCode) + ")");

				UA_Float* pTypedValue = (UA_Float*)variantValue->data;
				if (pTypedValue == nullptr)
					throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_OPCUAVARIANTDATAISNULL, "OPCUA variant data is null for " + sNodeName);

				dResultValue = *pTypedValue;
				break;
			}

			default:
				throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_INVALIDFLOATNODETYPE);

		}

		UA_Variant* valueToDelete = variantValue;
		variantValue = nullptr;

		UA_Variant_delete(valueToDelete);

	}
	catch (...) {
		if (variantValue != nullptr)
			UA_Variant_delete(variantValue);

		throw;
	}

	return dResultValue;


}

std::string COPCClient::ReadString(const LibOpen62541_uint32 nNameSpace, const std::string & sNodeName)
{
	UA_StatusCode statusCode;
	std::string sResultValue;

	if (m_Client == nullptr)
		throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_NOTCONNECTED, "OPCUA client is not connected");

	UA_Variant* variantValue = UA_Variant_new();
	try {
		std::vector<char> charBuffer(sNodeName.begin(), sNodeName.end());
		charBuffer.push_back(0);

		statusCode = UA_Client_readValueAttribute(m_Client, UA_NODEID_STRING(nNameSpace, charBuffer.data()), variantValue);
		if (statusCode != UA_STATUSCODE_GOOD)
			throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_COULDNOTREADVALUE, "could not read value " + sNodeName + " (" + std::to_string(statusCode) + ")");

		if (!UA_Variant_hasScalarType(variantValue, &UA_TYPES[UA_TYPES_STRING]))
				throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_READVALUEISOFINCORRECTTYPE, "read value is of incorrect type. " + sNodeName + " was supposed to be String (" + std::to_string(statusCode) + ")");

		UA_String* pTypedValue = (UA_String*)variantValue->data;
		if (pTypedValue == nullptr)
			throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_OPCUAVARIANTDATAISNULL, "OPCUA variant data is null for " + sNodeName);

		const char * pDataChar = (const char*) pTypedValue->data;
		if (pDataChar == nullptr)
			throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_OPCUASTRINGDATAISNULL, "OPCUA String data is null: " + sNodeName);

		sResultValue = std::string(pDataChar, pDataChar + pTypedValue->length);

		UA_Variant* valueToDelete = variantValue;
		variantValue = nullptr;

		UA_Variant_delete(valueToDelete);

	}
	catch (...) {
		if (variantValue != nullptr)
			UA_Variant_delete(variantValue);

		throw;
	}

	return sResultValue;
}

void COPCClient::WriteInteger(const LibOpen62541_uint32 nNameSpace, const std::string & sNodeName, const LibOpen62541::eUAIntegerType eNodeType, const LibOpen62541_int64 nValue)
{
	std::array<uint8_t, 8> writeDataBuffer;
	for (auto & value : writeDataBuffer)
		value = 0;

	if (m_Client == nullptr)
		throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_NOTCONNECTED, "OPCUA client is not connected");

	UA_DataType* pDataType;
	switch (eNodeType) {
		case eUAIntegerType::UAUInt8: {
			pDataType = &UA_TYPES[UA_TYPES_BYTE];
			if ((nValue < 0) || (nValue >= 256))
				throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_OPCUAWRITEINTEGEROUTOFBOUNDS, "OPCUA write integer out of bounds: " + std::to_string (nValue));

			uint8_t* pTargetPtr = writeDataBuffer.data();
			*pTargetPtr = (uint8_t) nValue;

			break;
		}

		case eUAIntegerType::UAUInt16: {
			pDataType = &UA_TYPES[UA_TYPES_UINT16];
			if ((nValue < 0) || (nValue >= 65536))
				throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_OPCUAWRITEINTEGEROUTOFBOUNDS, "OPCUA write integer out of bounds" + std::to_string(nValue));

			uint16_t* pTargetPtr = (uint16_t *) writeDataBuffer.data();
			*pTargetPtr = (uint16_t)nValue;

			break;
		}

		case eUAIntegerType::UAUInt32: {
			pDataType = &UA_TYPES[UA_TYPES_UINT32];
			if ((nValue < 0) || (nValue > 0x100000000LL))
				throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_OPCUAWRITEINTEGEROUTOFBOUNDS, "OPCUA write integer out of bounds" + std::to_string(nValue));

			uint32_t* pTargetPtr = (uint32_t*)writeDataBuffer.data();
			*pTargetPtr = (uint32_t)nValue;

			break;
		}

		case eUAIntegerType::UAUInt64: {
			pDataType = &UA_TYPES[UA_TYPES_UINT64];

			if (nValue < 0)
				throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_OPCUAWRITEINTEGEROUTOFBOUNDS, "OPCUA write integer out of bounds" + std::to_string(nValue));

			uint64_t* pTargetPtr = (uint64_t*)writeDataBuffer.data();
			*pTargetPtr = (uint64_t)nValue;

			break;
		}

		case eUAIntegerType::UAInt8: {
			pDataType = &UA_TYPES[UA_TYPES_BYTE];
			if ((nValue < -127) || (nValue >= 128))
				throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_OPCUAWRITEINTEGEROUTOFBOUNDS, "OPCUA write integer out of bounds: " + std::to_string(nValue));

			int8_t* pTargetPtr = (int8_t*)writeDataBuffer.data();
			*pTargetPtr = (int8_t)nValue;

			break;
		}

		case eUAIntegerType::UAInt16: {
			pDataType = &UA_TYPES[UA_TYPES_INT16];
			if ((nValue < -32767) || (nValue >= 32768))
				throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_OPCUAWRITEINTEGEROUTOFBOUNDS, "OPCUA write integer out of bounds" + std::to_string(nValue));

			int16_t* pTargetPtr = (int16_t*)writeDataBuffer.data();
			*pTargetPtr = (int16_t)nValue;

			break;
		}

		case eUAIntegerType::UAInt32: {
			pDataType = &UA_TYPES[UA_TYPES_INT32];
			if ((nValue < INT32_MIN) || (nValue > INT32_MAX))
				throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_OPCUAWRITEINTEGEROUTOFBOUNDS, "OPCUA write integer out of bounds" + std::to_string(nValue));

			int32_t* pTargetPtr = (int32_t*)writeDataBuffer.data();
			*pTargetPtr = (int32_t)nValue;

			break;
		}

		case eUAIntegerType::UAInt64: {
			pDataType = &UA_TYPES[UA_TYPES_INT64];

			int64_t* pTargetPtr = (int64_t*)writeDataBuffer.data();
			*pTargetPtr = (int64_t)nValue;

			break;
		}

		default:
			throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_INVALIDINTEGERNODETYPE);

	}

	UA_WriteRequest wReq;
	UA_WriteRequest_init(&wReq);
	wReq.nodesToWrite = UA_WriteValue_new();
	wReq.nodesToWriteSize = 1;
	wReq.nodesToWrite[0].nodeId = UA_NODEID_STRING_ALLOC(nNameSpace, sNodeName.c_str ());
	wReq.nodesToWrite[0].attributeId = UA_ATTRIBUTEID_VALUE;
	wReq.nodesToWrite[0].value.hasValue = true;
	wReq.nodesToWrite[0].value.value.storageType = UA_VARIANT_DATA_NODELETE; /* do not free the integer on deletion */

	wReq.nodesToWrite[0].value.value.type = pDataType;
	wReq.nodesToWrite[0].value.value.data = writeDataBuffer.data();

	UA_WriteResponse wResp = UA_Client_Service_write(m_Client, wReq);

	auto serviceResult = wResp.responseHeader.serviceResult;
	UA_WriteRequest_clear(&wReq);
	UA_WriteResponse_clear(&wResp);

	if (serviceResult != UA_STATUSCODE_GOOD)
		throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_COULDNOTWRITEVALUE, "could not write value to " + sNodeName);

}

void COPCClient::WriteDouble(const LibOpen62541_uint32 nNameSpace, const std::string & sNodeName, const LibOpen62541::eUADoubleType eNodeType, const LibOpen62541_double dValue)
{
	std::array<uint8_t, 8> writeDataBuffer;
	for (auto& value : writeDataBuffer)
		value = 0;

	if (m_Client == nullptr)
		throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_NOTCONNECTED, "OPCUA client is not connected");

	UA_DataType* pDataType;
	switch (eNodeType) {
	case eUADoubleType::UAFloat32: {
		pDataType = &UA_TYPES[UA_TYPES_FLOAT];
		float* pTargetPtr = (float*)writeDataBuffer.data();
		*pTargetPtr = (float)dValue;
		break;
	}

	case eUADoubleType::UADouble64: {
		pDataType = &UA_TYPES[UA_TYPES_DOUBLE];
		double* pTargetPtr = (double*)writeDataBuffer.data();
		*pTargetPtr = (double)dValue;
		break;
	}


	default:
		throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_INVALIDFLOATNODETYPE);
	}

	UA_WriteRequest wReq;
	UA_WriteRequest_init(&wReq);
	wReq.nodesToWrite = UA_WriteValue_new();
	wReq.nodesToWriteSize = 1;
	wReq.nodesToWrite[0].nodeId = UA_NODEID_STRING_ALLOC(nNameSpace, sNodeName.c_str());
	wReq.nodesToWrite[0].attributeId = UA_ATTRIBUTEID_VALUE;
	wReq.nodesToWrite[0].value.hasValue = true;
	wReq.nodesToWrite[0].value.value.storageType = UA_VARIANT_DATA_NODELETE; /* do not free the float on deletion */

	wReq.nodesToWrite[0].value.value.type = pDataType;
	wReq.nodesToWrite[0].value.value.data = writeDataBuffer.data();

	UA_WriteResponse wResp = UA_Client_Service_write(m_Client, wReq);

	auto serviceResult = wResp.responseHeader.serviceResult;
	UA_WriteRequest_clear(&wReq);
	UA_WriteResponse_clear(&wResp);

	if (serviceResult != UA_STATUSCODE_GOOD)
		throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_COULDNOTWRITEVALUE, "could not write value to " + sNodeName);
}

void COPCClient::WriteString(const LibOpen62541_uint32 nNameSpace, const std::string & sNodeName, const std::string & sValue)
{

	if (m_Client == nullptr)
		throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_NOTCONNECTED, "OPCUA client is not connected");

	UA_String stringValue = UA_String_fromChars(sValue.c_str());

	UA_WriteRequest wReq;
	UA_WriteRequest_init(&wReq);
	wReq.nodesToWrite = UA_WriteValue_new();
	wReq.nodesToWriteSize = 1;
	wReq.nodesToWrite[0].nodeId = UA_NODEID_STRING_ALLOC(nNameSpace, sNodeName.c_str());
	wReq.nodesToWrite[0].attributeId = UA_ATTRIBUTEID_VALUE;
	wReq.nodesToWrite[0].value.hasValue = true;
	
	UA_Variant_setScalar(&wReq.nodesToWrite[0].value.value, &stringValue, &UA_TYPES[UA_TYPES_STRING]);

	UA_WriteResponse wResp = UA_Client_Service_write(m_Client, wReq);

	auto serviceResult = wResp.responseHeader.serviceResult;
	UA_WriteRequest_clear(&wReq);
	UA_WriteResponse_clear(&wResp);

	if (serviceResult != UA_STATUSCODE_GOOD)
		throw ELibOpen62541InterfaceException(LIBOPEN62541_ERROR_COULDNOTWRITEVALUE, "could not write value to " + sNodeName);

}

