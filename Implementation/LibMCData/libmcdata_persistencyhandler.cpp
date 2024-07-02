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


Abstract: This is a stub class definition of CPersistencyHandler

*/

#include "libmcdata_persistencyhandler.hpp"
#include "libmcdata_interfaceexception.hpp"

#include "common_utils.hpp"
#include "common_chrono.hpp"

using namespace LibMCData::Impl;

/*************************************************************************************************************************
 Class definition of CPersistencyHandler 
**************************************************************************************************************************/

CPersistencyHandler::CPersistencyHandler(AMCData::PSQLHandler pSQLHandler)
	: m_pSQLHandler(pSQLHandler)
{
	if (pSQLHandler.get() == nullptr)
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

}

bool CPersistencyHandler::HasPersistentParameter(const std::string & sUUID)
{
	std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);
	std::lock_guard<std::mutex> lockGuard(m_Mutex);

	std::string sQuery = "SELECT uuid FROM persistentparameters WHERE uuid=? AND active=1";
	auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
	pStatement->setString(1, sNormalizedUUID);
	return pStatement->nextRow();
}

void CPersistencyHandler::GetPersistentParameterDetails(const std::string & sUUID, std::string & sName, LibMCData::eParameterDataType & eDataType)
{
	std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);
	std::lock_guard<std::mutex> lockGuard(m_Mutex);

	std::string sQuery = "SELECT name, datatype FROM persistentparameters WHERE uuid=? AND active=1";
	auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
	pStatement->setString(1, sNormalizedUUID);
	
	if (!pStatement->nextRow ()) 
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDNAMESTRING, "persistent parameter not found: " + sNormalizedUUID);

	sName = pStatement->getColumnString(1);
	eDataType = CPersistencyHandler::convertStringToDataType(pStatement->getColumnString(2));

}

bool CPersistencyHandler::DeletePersistentParameter(const std::string & sUUID)
{
	std::lock_guard<std::mutex> lockGuard(m_Mutex);

	std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);
	std::string sUpdateUUID = AMCCommon::CUtils::createUUID();

	auto pTransaction = m_pSQLHandler->beginTransaction();

	std::string sUpdateQuery = "UPDATE persistentparameters SET active=0 AND updateuuid=? WHERE uuid=? AND active=1";
	auto pUpdateStatement = pTransaction->prepareStatement(sUpdateQuery);
	pUpdateStatement->setString(1, sUpdateUUID);
	pUpdateStatement->setString(2, sNormalizedUUID);
	pUpdateStatement->execute();

	std::string sCheckQuery = "SELECT uuid FROM persistentparameters WHERE updateuuid=?";
	auto pCheckStatement = pTransaction->prepareStatement(sCheckQuery);
	pCheckStatement->setString(1, sUpdateUUID);
	bool bUpdateSuccess = pCheckStatement->nextRow();

	pTransaction->commit();

	return bUpdateSuccess;

}

void CPersistencyHandler::StorePersistentParameter(const std::string& sUUID, const std::string& sName, const LibMCData::eParameterDataType eDataType, const std::string& sValue, const LibMCData_uint64 nAbsoluteTimeStamp)
{

	// Check parameters
	auto sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);
	auto sNormalizedName = AMCCommon::CUtils::trimString(AMCCommon::CUtils::toLowerString(sName));
	auto sDataType = CPersistencyHandler::convertDataTypeToString(eDataType);

	if (!AMCCommon::CUtils::stringIsValidAlphanumericPathString(sNormalizedName))
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDNAMESTRING, "invalid name string: " + sName);

	// Check Value parameter
	std::string sNormalizedValue;

	switch (eDataType) {
	case LibMCData::eParameterDataType::String:
		sNormalizedValue = sValue;
		break;
	case LibMCData::eParameterDataType::Integer:
		sNormalizedValue = std::to_string(AMCCommon::CUtils::stringToInteger(sValue));
		break;
	case LibMCData::eParameterDataType::Bool:
		if (AMCCommon::CUtils::stringToInteger(sValue) != 0)
			sNormalizedValue = "1";
		else
			sNormalizedValue = "0";

		break;
	case LibMCData::eParameterDataType::Double:
		sNormalizedValue = std::to_string(AMCCommon::CUtils::stringToDouble(sValue));
		break;
	case LibMCData::eParameterDataType::UUID:
		sNormalizedValue = AMCCommon::CUtils::normalizeUUIDString(sValue);
		break;

	default:
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDDATATYPE, "invalid data type: " + std::to_string((int64_t)eDataType));

	}

	std::string sTimestamp = AMCCommon::CChrono::convertToISO8601TimeUTC(nAbsoluteTimeStamp);


	auto pTransaction = m_pSQLHandler->beginTransaction();

	std::string sStoredName;
	std::string sStoredDataType;
	std::string sCheckQuery = "SELECT name, datatype, active FROM persistentparameters WHERE uuid=?";
	auto pCheckStatement = pTransaction->prepareStatement(sCheckQuery);
	pCheckStatement->setString(1, sNormalizedUUID);
	bool bParameterExists = pCheckStatement->nextRow();
	if (bParameterExists) {
		sStoredName = pCheckStatement->getColumnString(1);
		sStoredDataType = pCheckStatement->getColumnString(2);
	}
	pCheckStatement = nullptr;
	if (bParameterExists) {
		// parameter is already stored...		
		if (sStoredName != sNormalizedName)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_NAMESTRINGMISMATCH, "Name string mismatch: " + sNormalizedName + " != " + sStoredName);
		if (sStoredDataType != sDataType)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_DATATYPEMISMATCH, "Datatype mismatch for " + sStoredName + ": " + sDataType + " != " + sStoredDataType);

		std::string sUpdateQuery = "UPDATE persistentparameters SET value=?, timestamp=?, active=1 WHERE uuid=?";
		auto pUpdateStatement = pTransaction->prepareStatement(sUpdateQuery);
		pUpdateStatement->setString(1, sNormalizedValue);
		pUpdateStatement->setString(2, sTimestamp);
		pUpdateStatement->setString(3, sNormalizedUUID);
		pUpdateStatement->execute();


	}
	else {
		// parameter has never been stored

		std::string sInsertQuery = "INSERT INTO persistentparameters (uuid, name, datatype, value, timestamp, active) VALUES (?, ?, ?, ?, ?, 1)";
		auto pInsertStatement = pTransaction->prepareStatement(sInsertQuery);
		pInsertStatement->setString(1, sNormalizedUUID);
		pInsertStatement->setString(2, sNormalizedName);
		pInsertStatement->setString(3, sDataType);
		pInsertStatement->setString(4, sNormalizedValue);
		pInsertStatement->setString(5, sTimestamp);
		pInsertStatement->execute();
	}

	pTransaction->commit();


}

void CPersistencyHandler::StorePersistentStringParameter(const std::string & sUUID, const std::string & sName, const std::string & sValue, const LibMCData_uint64 nAbsoluteTimeStamp)
{
	StorePersistentParameter(sUUID, sName, LibMCData::eParameterDataType::String, sValue, nAbsoluteTimeStamp);
}

void CPersistencyHandler::StorePersistentUUIDParameter(const std::string & sUUID, const std::string & sName, const std::string & sValue, const LibMCData_uint64 nAbsoluteTimeStamp)
{
	std::string sFormattedUUID = AMCCommon::CUtils::normalizeUUIDString(sValue);
	StorePersistentParameter(sUUID, sName, LibMCData::eParameterDataType::UUID, sFormattedUUID, nAbsoluteTimeStamp);

}

void CPersistencyHandler::StorePersistentDoubleParameter(const std::string & sUUID, const std::string & sName, const LibMCData_double dValue, const LibMCData_uint64 nAbsoluteTimeStamp)
{
	StorePersistentParameter(sUUID, sName, LibMCData::eParameterDataType::Double, std::to_string (dValue), nAbsoluteTimeStamp);
}

void CPersistencyHandler::StorePersistentIntegerParameter(const std::string & sUUID, const std::string & sName, const LibMCData_int64 nValue, const LibMCData_uint64 nAbsoluteTimeStamp)
{
	StorePersistentParameter(sUUID, sName, LibMCData::eParameterDataType::Integer, std::to_string(nValue), nAbsoluteTimeStamp);
}

void CPersistencyHandler::StorePersistentBoolParameter(const std::string & sUUID, const std::string & sName, const bool bValue, const LibMCData_uint64 nAbsoluteTimeStamp)
{
	StorePersistentParameter(sUUID, sName, LibMCData::eParameterDataType::Bool, std::to_string((int32_t)bValue), nAbsoluteTimeStamp);
}

std::string CPersistencyHandler::RetrievePersistentStringParameter(const std::string & sUUID)
{
	return retrievePersistentParameter(sUUID, LibMCData::eParameterDataType::String);
}

std::string CPersistencyHandler::RetrievePersistentUUIDParameter(const std::string & sUUID)
{
	auto sStringValue = retrievePersistentParameter(sUUID, LibMCData::eParameterDataType::UUID);
	return AMCCommon::CUtils::normalizeUUIDString(sStringValue);
}

LibMCData_double CPersistencyHandler::RetrievePersistentDoubleParameter(const std::string & sUUID)
{
	auto sStringValue = retrievePersistentParameter(sUUID, LibMCData::eParameterDataType::Double);
	return AMCCommon::CUtils::stringToDouble(sStringValue);
}

LibMCData_int64 CPersistencyHandler::RetrievePersistentIntegerParameter(const std::string & sUUID)
{
	auto sStringValue = retrievePersistentParameter(sUUID, LibMCData::eParameterDataType::Integer);
	return AMCCommon::CUtils::stringToInteger(sStringValue);
}

bool CPersistencyHandler::RetrievePersistentBoolParameter(const std::string & sUUID)
{
	auto sStringValue = retrievePersistentParameter(sUUID, LibMCData::eParameterDataType::Bool);
	return (AMCCommon::CUtils::stringToInteger(sStringValue) != 0);
}

std::string CPersistencyHandler::retrievePersistentParameter(const std::string& sUUID, const LibMCData::eParameterDataType eDataType)
{
	std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);
	std::lock_guard<std::mutex> lockGuard(m_Mutex);
	auto sDataType = CPersistencyHandler::convertDataTypeToString(eDataType);

	std::string sQuery = "SELECT name, value, datatype FROM persistentparameters WHERE uuid=? AND active=1";
	auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
	pStatement->setString(1, sNormalizedUUID);

	if (!pStatement->nextRow())
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDNAMESTRING, "persistent parameter not found: " + sNormalizedUUID);

	auto sStoredName = pStatement->getColumnString(1);
	auto sStoredValue = pStatement->getColumnString(2);
	auto sStoredDataType = pStatement->getColumnString(3);

	if (sDataType != sStoredDataType)
	   throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_DATATYPEMISMATCH, "Datatype mismatch for " + sStoredName + ": " + sDataType + " != " + sStoredDataType);

	return sStoredValue;
}


std::string CPersistencyHandler::convertDataTypeToString(const LibMCData::eParameterDataType eDataType)
{
	switch (eDataType) {
		case LibMCData::eParameterDataType::Bool: return "bool";
		case LibMCData::eParameterDataType::Double: return "double";
		case LibMCData::eParameterDataType::String: return "string";
		case LibMCData::eParameterDataType::UUID: return "uuid";
		case LibMCData::eParameterDataType::Integer: return "integer";
		default:
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDDATATYPE, "invalid data type: " + std::to_string ((int64_t) eDataType));
	}
}

LibMCData::eParameterDataType CPersistencyHandler::convertStringToDataType(const std::string& sValue)
{
	auto sNormalizedValue = AMCCommon::CUtils::trimString(AMCCommon::CUtils::toLowerString(sValue));

	if (sNormalizedValue == "bool")
		return LibMCData::eParameterDataType::Bool;
	if (sNormalizedValue == "double")
		return LibMCData::eParameterDataType::Double;
	if (sNormalizedValue == "string")
		return LibMCData::eParameterDataType::String;
	if (sNormalizedValue == "uuid")
		return LibMCData::eParameterDataType::UUID;
	if (sNormalizedValue == "integer")
		return LibMCData::eParameterDataType::Integer;
	
	throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDDATATYPE, "invalid data type: " + sValue);

}
