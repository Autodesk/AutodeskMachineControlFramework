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

#include "amcdata_journal.hpp"
#include "libmcdata_interfaceexception.hpp"
#include "common_utils.hpp"
#include "amcdata_sqlhandler_sqlite.hpp"
#include "common_exportstream_native.hpp"

namespace AMCData {
		
	CJournal::CJournal(const std::string& sJournalPath, const std::string& sJournalDataPath, const std::string& sSessionUUID)
		: m_LogID(1), m_AlertID (1), m_sSessionUUID (AMCCommon::CUtils::normalizeUUIDString (sSessionUUID))
	{

		m_pJournalStream = std::make_shared<AMCCommon::CExportStream_Native>(sJournalDataPath);

		m_pSQLHandler = std::make_shared<AMCData::CSQLHandler_SQLite>(sJournalPath);

		std::string sQuery = "CREATE TABLE `logs` (";
		sQuery += "`logindex`	int DEFAULT 0, ";
		sQuery += "`loglevel`	int DEFAULT 0,";
		sQuery += "`timestamp`  varchar ( 64 ) NOT NULL, ";
		sQuery += "`subsystem`  varchar ( 8 ) NOT NULL, ";
		sQuery += "`message`	TEXT DEFAULT `` )";

		auto pLogStatement = m_pSQLHandler->prepareStatement(sQuery);
		pLogStatement->execute();
		pLogStatement = nullptr;

		std::string sJournalQuery = "CREATE TABLE `journal_chunks` (";
		sJournalQuery += "`chunkindex` int DEFAULT 0, ";
		sJournalQuery += "`starttimestamp` int DEFAULT 0, ";
		sJournalQuery += "`endtimestamp` int DEFAULT 0, ";
		sJournalQuery += "`dataoffset` int DEFAULT 0, ";
		sJournalQuery += "`datalength` int DEFAULT 0) ";

		auto pJournalStatement = m_pSQLHandler->prepareStatement(sJournalQuery);
		pJournalStatement->execute();
		pJournalStatement = nullptr;

		std::string sAlertQuery = "CREATE TABLE `alerts` (";
		sAlertQuery += "`uuid`  varchar ( 64 ) NOT NULL, ";
		sAlertQuery += "`identifier`  varchar ( 64 ) NOT NULL, ";
		sAlertQuery += "`alertindex`	int DEFAULT 0, ";
		sAlertQuery += "`alertlevel`	varchar (64) NOT NULL,";
		sAlertQuery += "`description`	TEXT DEFAULT ``,";
		sAlertQuery += "`descriptionidentifier`  varchar ( 64 ) NOT NULL, ";
		sAlertQuery += "`contextinformation`	TEXT DEFAULT ``,";
		sAlertQuery += "`active`	int DEFAULT 0,";
		sAlertQuery += "`needsacknowledgement`	int DEFAULT 0,";
		sAlertQuery += "`timestamp`  varchar ( 64 ) NOT NULL)";

		auto pAlertStatement = m_pSQLHandler->prepareStatement(sAlertQuery);
		pAlertStatement->execute();
		pAlertStatement = nullptr;

		std::string sAlertAckQuery = "CREATE TABLE `alertacknowledgements` (";
		sAlertAckQuery += "`uuid`  varchar ( 64 ) NOT NULL, ";
		sAlertAckQuery += "`alertuuid`  varchar ( 64 ) NOT NULL, ";
		sAlertAckQuery += "`useruuid`  varchar ( 64 ) NOT NULL, ";
		sAlertAckQuery += "`usercomment`  varchar ( 64 ) NOT NULL, ";
		sAlertAckQuery += "`timestamp`  varchar ( 64 ) NOT NULL)";

		auto pAlertAckStatement = m_pSQLHandler->prepareStatement(sAlertAckQuery);
		pAlertAckStatement->execute();
		pAlertAckStatement = nullptr; 
	}

	CJournal::~CJournal()
	{

	}

	std::string CJournal::getSessionUUID()
	{
		return m_sSessionUUID;
	}

	uint32_t CJournal::getSchemaVersion()
	{
		return 1;
	}


	void CJournal::AddEntry(const std::string& sMessage, const std::string& sSubSystem, const LibMCData::eLogLevel logLevel, const std::string& sTimestamp)
	{
		std::lock_guard<std::mutex> lockGuard(m_LogMutex);

		std::string sQuery = "INSERT INTO logs (logindex, loglevel, timestamp, subsystem, message) VALUES (?, ?, ?, ?, ?)";
		auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
		pStatement->setInt(1, m_LogID);
		pStatement->setInt(2, (int)logLevel);
		pStatement->setString(3, sTimestamp);
		pStatement->setString(4, sSubSystem);
		pStatement->setString(5, sMessage);
		pStatement->execute();
		pStatement = nullptr;

		m_LogID++;
	}

	LibMCData_uint32 CJournal::GetMaxLogEntryID()
	{
		return m_LogID;
	}

	AMCData::PSQLHandler CJournal::getSQLHandler()
	{
		return m_pSQLHandler;
	}


	void CJournal::WriteJournalChunkIntegerData(const LibMCData_uint32 nChunkIndex, const LibMCData_uint64 nStartTimeStamp, const LibMCData_uint64 nEndTimeStamp, const LibMCData_uint64 nVariableInfoBufferSize, const LibMCData::sJournalChunkVariableInfo* pVariableInfoBuffer, const LibMCData_uint64 nTimeStampDataBufferSize, const LibMCData_uint32* pTimeStampDataBuffer, const LibMCData_uint64 nValueDataBufferSize, const LibMCData_int64* pValueDataBuffer)
	{
		std::lock_guard<std::mutex> lockGuard(m_JournalMutex);

		uint64_t nPosition = m_pJournalStream->getPosition();
		if ((nTimeStampDataBufferSize > 0) && (nVariableInfoBufferSize > 0)) {
			if (pTimeStampDataBuffer == nullptr)
				throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);
			if (pValueDataBuffer == nullptr)
				throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);
			if (pVariableInfoBuffer == nullptr)
				throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);
			if (nTimeStampDataBufferSize != nValueDataBufferSize)
				throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

			uint64_t nVariableBufferMemSize = nVariableInfoBufferSize * sizeof(LibMCData::sJournalChunkVariableInfo);
			uint64_t nTimeStampBufferMemSize = nTimeStampDataBufferSize * sizeof(uint32_t);
			uint64_t nValueBufferMemSize = nValueDataBufferSize * sizeof(int64_t);

			uint64_t nPosition = m_pJournalStream->getPosition();
			m_pJournalStream->writeBuffer((const void*)pVariableInfoBuffer, nVariableBufferMemSize);
			m_pJournalStream->writeBuffer((const void*)pTimeStampDataBuffer, nTimeStampBufferMemSize);
			m_pJournalStream->writeBuffer((const void*)pValueDataBuffer, nValueBufferMemSize);
			m_pJournalStream->flushStream();


			std::string sQuery = "INSERT INTO journal_chunks (chunkindex, starttimestamp, endtimestamp, dataoffset, datalength) VALUES (?, ?, ?, ?, ?)";
			auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
			pStatement->setInt64(1, nChunkIndex);
			pStatement->setInt64(2, nStartTimeStamp);
			pStatement->setInt64(3, nEndTimeStamp);
			pStatement->setInt64(4, nPosition);
			pStatement->setInt64(5, nVariableBufferMemSize + nTimeStampBufferMemSize + nValueBufferMemSize);
			pStatement->execute();
			pStatement = nullptr; 
		}

	}

	std::string CJournal::convertAlertLevelToString(const LibMCData::eAlertLevel eLevel)
	{


		switch (eLevel) {
		case LibMCData::eAlertLevel::Warning:
			return "warning";
			break;

		case LibMCData::eAlertLevel::Message:
			return "message";
			break;

		case LibMCData::eAlertLevel::FatalError:
			return "fatalerror";
			break;

		case LibMCData::eAlertLevel::CriticalError:
			return "criticalerror";
			break;

		case LibMCData::eAlertLevel::Unknown:
			return "unknown";
			break;

		default:
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDALERTLEVEL, "invalid alert level: " + std::to_string((int)eLevel));

		}

	}

	LibMCData::eAlertLevel CJournal::convertStringToAlertLevel(const std::string& sValue, bool bFailIfUnknown)
	{
		if (sValue == "warning") {
			return LibMCData::eAlertLevel::Warning;
		}
		else if (sValue == "message") {
			return LibMCData::eAlertLevel::Message;
		}
		else if (sValue == "fatalerror") {
			return LibMCData::eAlertLevel::FatalError;
		}
		else if (sValue == "criticalerror") {
			return LibMCData::eAlertLevel::CriticalError;
		}
		else {
			if (bFailIfUnknown)
				throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDALERTLEVEL, "invalid alert level: " + sValue);

			return LibMCData::eAlertLevel::Unknown;
		}
	}



	void CJournal::addAlert(const std::string& sUUID, const std::string& sIdentifier, const LibMCData::eAlertLevel eLevel, const std::string& sDescription, const std::string& sDescriptionIdentifier, const std::string& sReadableContextInformation, const bool bNeedsAcknowledgement, const std::string& sTimestampUTC)
	{
		auto sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);

		if (sIdentifier.empty())
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_EMPTYALERTIDENTIFIER);

		if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sIdentifier))
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDALERTIDENTIFIER, "invalid alert identifier: " + sIdentifier);

		if (!sDescriptionIdentifier.empty()) {
			if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sDescriptionIdentifier))
				throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDALERTDESCRIPTIONIDENTIFIER, "invalid alert description identifier: " + sDescriptionIdentifier);
		}

		std::string sLevelString = convertAlertLevelToString (eLevel);


		std::lock_guard<std::mutex> lockGuard(m_LogMutex);

		std::string sQuery = "INSERT INTO alerts (uuid, identifier, alertindex, alertlevel, description, descriptionidentifier, contextinformation, active, needsacknowledgement, timestamp) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
		auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
		pStatement->setString(1, sNormalizedUUID);
		pStatement->setString(2, sIdentifier);
		pStatement->setInt(3, m_AlertID);
		pStatement->setString(4, sLevelString);
		pStatement->setString(5, sDescription);
		pStatement->setString(6, sDescriptionIdentifier);
		pStatement->setString(7, sReadableContextInformation);
		pStatement->setInt(8, 1);
		pStatement->setInt(9, bNeedsAcknowledgement);
		pStatement->setString(10, sTimestampUTC);
		pStatement->execute();
		pStatement = nullptr;

		m_AlertID++;
	}


	bool CJournal::hasAlert(const std::string& sUUID)
	{
		auto sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);

		std::lock_guard<std::mutex> lockGuard(m_LogMutex);

		std::string sQuery = "SELECT uuid FROM alerts WHERE uuid=?";
		auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
		pStatement->setString(1, sNormalizedUUID);
		return pStatement->nextRow();


	}

	void CJournal::getAlertInformation(const std::string& sUUID, std::string& sIdentifier, LibMCData::eAlertLevel& eLevel, std::string& sDescription, std::string& sDescriptionIdentifier, std::string& sReadableContextInformation, bool& bNeedsAcknowledgement, std::string& sTimestampUTC)
	{
		auto sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);

		std::lock_guard<std::mutex> lockGuard(m_LogMutex);

		std::string sQuery = "SELECT identifier, alertlevel, description, descriptionidentifier, contextinformation, needsacknowledgement, timestamp FROM alerts WHERE uuid=?";
		auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
		pStatement->setString(1, sNormalizedUUID);

		if (pStatement->nextRow()) {
			sIdentifier = pStatement->getColumnString(1);
			std::string sAlertLevel = pStatement->getColumnString(2);

			eLevel = convertStringToAlertLevel(sAlertLevel, false);

			sDescription = pStatement->getColumnString(3);
			sDescriptionIdentifier = pStatement->getColumnString(4);
			sReadableContextInformation = pStatement->getColumnString(5);
			bNeedsAcknowledgement = pStatement->getColumnInt(6) != 0;
			sTimestampUTC = pStatement->getColumnString(7);
		}
		else {
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_ALERTNOTFOUND, "alert information not found: " + sNormalizedUUID);
		}


	}

	void CJournal::acknowledgeAlert(const std::string& sUUID, const std::string& sUserUUID, const std::string& sUserComment, const std::string& sTimeStampUTC)
	{
		auto sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);
		auto sNormalizedUserUUID = AMCCommon::CUtils::normalizeUUIDString(sUserUUID);

		auto pTransaction = m_pSQLHandler->beginTransaction();

		std::string sQuery = "SELECT uuid FROM alerts WHERE uuid=?";
		auto pStatement = pTransaction->prepareStatement(sQuery);
		pStatement->setString(1, sNormalizedUUID);
		if (pStatement->nextRow()) {

			std::string sNewAckUUID = AMCCommon::CUtils::createUUID();

			std::string sAlertQuery = "UPDATE alerts SET active=0 WHERE uuid=?";
			auto pAlertStatement = m_pSQLHandler->prepareStatement(sAlertQuery);
			pAlertStatement->setString(1, sNormalizedUUID);
			pAlertStatement->execute();
			pAlertStatement = nullptr;

			std::string sAckQuery = "INSERT INTO alertacknowledgements (uuid, alertuuid, useruuid, usercomment, timestamp) VALUES (?, ?, ?, ?, ?)";
			auto pStatement = m_pSQLHandler->prepareStatement(sAckQuery);
			pStatement->setString(1, sNewAckUUID);
			pStatement->setString(2, sNormalizedUUID);
			pStatement->setString(3, sNormalizedUserUUID);
			pStatement->setString(4, sUserComment);
			pStatement->setString(5, sTimeStampUTC);
			pStatement->execute();
			pStatement = nullptr;

			pTransaction->commit();

		}
		else {
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_ALERTNOTFOUND, "alert not found for acknowledgment: " + sNormalizedUUID);
		}


	}

	bool CJournal::alertHasBeenAcknowledged(const std::string& sUUID)
	{
		auto sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);

		std::lock_guard<std::mutex> lockGuard(m_LogMutex);

		std::string sQuery = "SELECT uuid FROM alertacknowledgements WHERE alertuuid=? ORDER BY timestamp LIMIT 1";
		auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
		pStatement->setString(1, sNormalizedUUID);
		return pStatement->nextRow();
	}

	bool CJournal::alertIsActive(const std::string& sUUID)
	{
		auto sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);

		std::lock_guard<std::mutex> lockGuard(m_LogMutex);

		std::string sQuery = "SELECT active FROM alerts WHERE uuid=?";
		auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
		pStatement->setString(1, sNormalizedUUID);
		if (pStatement->nextRow()) {
			return pStatement->getColumnInt(1) != 0;
		}

		return false;

	}


	void CJournal::getAcknowledgementInformation(const std::string& sUUID, std::string& sUserUUID, std::string& sUserComment, std::string& sTimestampUTC)
	{
		auto sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);

		std::lock_guard<std::mutex> lockGuard(m_LogMutex);

		std::string sQuery = "SELECT useruuid, usercomment, timestamp FROM alertacknowledgements WHERE alertuuid=? ORDER BY timestamp LIMIT 1";
		auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
		pStatement->setString(1, sNormalizedUUID);
		if (pStatement->nextRow()) {
			sUserUUID = pStatement->getColumnString(1);
			sUserComment = pStatement->getColumnString(2);
			sTimestampUTC = pStatement->getColumnString(3);
		}
		else {
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_ALERTNOTACKNOWLEDGED, "alert has not been acknowledged: " + sNormalizedUUID);

		}

	}

	void CJournal::retrieveAlerts(std::vector<std::string>& alertUUIDs)
	{
		auto pTransaction = m_pSQLHandler->beginTransaction();
		auto sQuery = "SELECT uuid FROM alerts ORDER BY timestamp";
		auto pStatement = pTransaction->prepareStatement(sQuery);

		while (pStatement->nextRow()) 
			alertUUIDs.push_back(pStatement->getColumnString(1));
	}

	void CJournal::retrieveActiveAlerts(std::vector<std::string>& alertUUIDs)
	{
		auto pTransaction = m_pSQLHandler->beginTransaction();
		auto sQuery = "SELECT uuid FROM alerts WHERE active=? ORDER BY timestamp";
		auto pStatement = pTransaction->prepareStatement(sQuery);
		pStatement->setInt(1, 1);

		while (pStatement->nextRow())
			alertUUIDs.push_back(pStatement->getColumnString(1));
	}

	void CJournal::retrieveAlertsByType(std::vector<std::string>& alertUUIDs, const std::string& sTypeIdentifier)
	{
		auto pTransaction = m_pSQLHandler->beginTransaction();
		auto sQuery = "SELECT uuid FROM alerts WHERE identifier=? ORDER BY timestamp";
		auto pStatement = pTransaction->prepareStatement(sQuery);
		pStatement->setString(1, sTypeIdentifier);

		while (pStatement->nextRow())
			alertUUIDs.push_back(pStatement->getColumnString(1));
	}

	void CJournal::retrieveActiveAlertsByType(std::vector<std::string>& alertUUIDs, const std::string& sTypeIdentifier)
	{
		auto pTransaction = m_pSQLHandler->beginTransaction();
		auto sQuery = "SELECT uuid FROM alerts WHERE active=? AND identifier=? ORDER BY timestamp";
		auto pStatement = pTransaction->prepareStatement(sQuery);
		pStatement->setInt(1, 1);
		pStatement->setString(2, sTypeIdentifier);

		while (pStatement->nextRow())
			alertUUIDs.push_back(pStatement->getColumnString(1));
	}

	void CJournal::acknowledgeAlertForUser(const std::string& sAlertUUID, const std::string& sUserUUID, const std::string& sUserComment, const std::string& sTimeStampUTC)
	{
		auto pTransaction = m_pSQLHandler->beginTransaction();
		auto sDeactivateQuery = "UPDATE alerts SET active=0 WHERE uuid=?";
		auto pDeactivateStatement = pTransaction->prepareStatement(sDeactivateQuery);
		pDeactivateStatement->setString(1, AMCCommon::CUtils::normalizeUUIDString(sAlertUUID));
		pDeactivateStatement->execute();

		std::string sAcknowledgeUUID = AMCCommon::CUtils::createUUID();

		auto sInsertAckQuery = "INSERT INTO alertacknowledgements (uuid, alertuuid, useruuid, usercomment, timestamp) VALUES (?, ?, ?, ?, ?)";

		auto pInsertAckStatement = pTransaction->prepareStatement(sInsertAckQuery);
		pInsertAckStatement->setString(1, sAcknowledgeUUID);
		pInsertAckStatement->setString(2, AMCCommon::CUtils::normalizeUUIDString(sAlertUUID));
		pInsertAckStatement->setString(3, AMCCommon::CUtils::normalizeUUIDString(sUserUUID));
		pInsertAckStatement->setString(4, sUserComment);
		pInsertAckStatement->setString(5, sTimeStampUTC);
		pInsertAckStatement->execute();

		pTransaction->commit();

	}

	void CJournal::deactivateAlert(const std::string& sAlertUUID)
	{
		auto pTransaction = m_pSQLHandler->beginTransaction();
		auto sQuery = "UPDATE alerts SET active=0 WHERE uuid=?";
		auto pStatement = pTransaction->prepareStatement(sQuery);
		pStatement->setString(1, AMCCommon::CUtils::normalizeUUIDString (sAlertUUID));
		pStatement->execute();

		pTransaction->commit();

	}

}


