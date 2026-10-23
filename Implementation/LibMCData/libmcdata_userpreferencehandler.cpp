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


Abstract: This is a stub class definition of CUserPreferenceHandler

*/

#include "libmcdata_userpreferencehandler.hpp"
#include "libmcdata_interfaceexception.hpp"

#include "common_utils.hpp"
#include "common_chrono.hpp"

using namespace LibMCData::Impl;

/*************************************************************************************************************************
 Class definition of CUserPreferenceHandler 
**************************************************************************************************************************/

CUserPreferenceHandler::CUserPreferenceHandler(AMCData::PSQLHandler pSQLHandler)
	: m_pSQLHandler(pSQLHandler)
{
	if (pSQLHandler.get() == nullptr)
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

}

void CUserPreferenceHandler::normalizeIdentifier(const std::string& sUserUUID, const std::string& sDomain, const std::string& sKey, std::string& sNormalizedUserUUID, std::string& sNormalizedDomain, std::string& sNormalizedKey)
{
	sNormalizedUserUUID = AMCCommon::CUtils::normalizeUUIDString(sUserUUID);

	sNormalizedDomain = AMCCommon::CUtils::trimString(sDomain);
	if (sNormalizedDomain.empty())
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM, "user preference domain must not be empty");

	sNormalizedKey = AMCCommon::CUtils::trimString(sKey);
	if (sNormalizedKey.empty())
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM, "user preference key must not be empty");
}

bool CUserPreferenceHandler::HasUserPreference(const std::string & sUserUUID, const std::string & sDomain, const std::string & sKey)
{
	std::string sNormalizedUserUUID, sNormalizedDomain, sNormalizedKey;
	normalizeIdentifier(sUserUUID, sDomain, sKey, sNormalizedUserUUID, sNormalizedDomain, sNormalizedKey);

	std::lock_guard<std::mutex> lockGuard(m_Mutex);

	std::string sQuery = "SELECT uuid FROM userpreferences WHERE useruuid=? AND prefdomain=? AND prefkey=? AND active=1";
	auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
	pStatement->setString(1, sNormalizedUserUUID);
	pStatement->setString(2, sNormalizedDomain);
	pStatement->setString(3, sNormalizedKey);
	return pStatement->nextRow();
}

std::string CUserPreferenceHandler::RetrieveUserPreference(const std::string & sUserUUID, const std::string & sDomain, const std::string & sKey)
{
	std::string sNormalizedUserUUID, sNormalizedDomain, sNormalizedKey;
	normalizeIdentifier(sUserUUID, sDomain, sKey, sNormalizedUserUUID, sNormalizedDomain, sNormalizedKey);

	std::lock_guard<std::mutex> lockGuard(m_Mutex);

	std::string sQuery = "SELECT value FROM userpreferences WHERE useruuid=? AND prefdomain=? AND prefkey=? AND active=1";
	auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
	pStatement->setString(1, sNormalizedUserUUID);
	pStatement->setString(2, sNormalizedDomain);
	pStatement->setString(3, sNormalizedKey);

	if (!pStatement->nextRow())
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM, "user preference not found: " + sNormalizedDomain + "/" + sNormalizedKey);

	return pStatement->getColumnString(1);
}

void CUserPreferenceHandler::StoreUserPreference(const std::string & sUserUUID, const std::string & sDomain, const std::string & sKey, const std::string & sValue, const LibMCData_uint64 nAbsoluteTimeStamp)
{
	std::string sNormalizedUserUUID, sNormalizedDomain, sNormalizedKey;
	normalizeIdentifier(sUserUUID, sDomain, sKey, sNormalizedUserUUID, sNormalizedDomain, sNormalizedKey);

	std::string sTimestamp = AMCCommon::CChrono::convertToISO8601TimeUTC(nAbsoluteTimeStamp);
	std::string sUpdateUUID = AMCCommon::CUtils::createUUID();

	std::lock_guard<std::mutex> lockGuard(m_Mutex);

	auto pTransaction = m_pSQLHandler->beginTransaction();

	std::string sCheckQuery = "SELECT uuid FROM userpreferences WHERE useruuid=? AND prefdomain=? AND prefkey=?";
	auto pCheckStatement = pTransaction->prepareStatement(sCheckQuery);
	pCheckStatement->setString(1, sNormalizedUserUUID);
	pCheckStatement->setString(2, sNormalizedDomain);
	pCheckStatement->setString(3, sNormalizedKey);
	bool bPreferenceExists = pCheckStatement->nextRow();
	pCheckStatement = nullptr;

	if (bPreferenceExists) {
		std::string sUpdateQuery = "UPDATE userpreferences SET value=?, timestamp=?, updateuuid=?, active=1 WHERE useruuid=? AND prefdomain=? AND prefkey=?";
		auto pUpdateStatement = pTransaction->prepareStatement(sUpdateQuery);
		pUpdateStatement->setString(1, sValue);
		pUpdateStatement->setString(2, sTimestamp);
		pUpdateStatement->setString(3, sUpdateUUID);
		pUpdateStatement->setString(4, sNormalizedUserUUID);
		pUpdateStatement->setString(5, sNormalizedDomain);
		pUpdateStatement->setString(6, sNormalizedKey);
		pUpdateStatement->execute();
	}
	else {
		std::string sNewUUID = AMCCommon::CUtils::createUUID();
		std::string sInsertQuery = "INSERT INTO userpreferences (uuid, useruuid, prefdomain, prefkey, value, timestamp, updateuuid, active) VALUES (?, ?, ?, ?, ?, ?, ?, 1)";
		auto pInsertStatement = pTransaction->prepareStatement(sInsertQuery);
		pInsertStatement->setString(1, sNewUUID);
		pInsertStatement->setString(2, sNormalizedUserUUID);
		pInsertStatement->setString(3, sNormalizedDomain);
		pInsertStatement->setString(4, sNormalizedKey);
		pInsertStatement->setString(5, sValue);
		pInsertStatement->setString(6, sTimestamp);
		pInsertStatement->setString(7, sUpdateUUID);
		pInsertStatement->execute();
	}

	pTransaction->commit();
}

bool CUserPreferenceHandler::DeleteUserPreference(const std::string & sUserUUID, const std::string & sDomain, const std::string & sKey)
{
	std::string sNormalizedUserUUID, sNormalizedDomain, sNormalizedKey;
	normalizeIdentifier(sUserUUID, sDomain, sKey, sNormalizedUserUUID, sNormalizedDomain, sNormalizedKey);

	std::string sUpdateUUID = AMCCommon::CUtils::createUUID();

	std::lock_guard<std::mutex> lockGuard(m_Mutex);

	auto pTransaction = m_pSQLHandler->beginTransaction();

	std::string sUpdateQuery = "UPDATE userpreferences SET active=0, updateuuid=? WHERE useruuid=? AND prefdomain=? AND prefkey=? AND active=1";
	auto pUpdateStatement = pTransaction->prepareStatement(sUpdateQuery);
	pUpdateStatement->setString(1, sUpdateUUID);
	pUpdateStatement->setString(2, sNormalizedUserUUID);
	pUpdateStatement->setString(3, sNormalizedDomain);
	pUpdateStatement->setString(4, sNormalizedKey);
	pUpdateStatement->execute();

	std::string sCheckQuery = "SELECT uuid FROM userpreferences WHERE updateuuid=?";
	auto pCheckStatement = pTransaction->prepareStatement(sCheckQuery);
	pCheckStatement->setString(1, sUpdateUUID);
	bool bPreferenceExisted = pCheckStatement->nextRow();

	pTransaction->commit();

	return bPreferenceExisted;
}
