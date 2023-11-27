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
		
	CJournal::CJournal(const std::string& sJournalPath, const std::string& sJournalDataPath)
		: m_LogID(1)
	{

		m_pJournalStream = std::make_shared<AMCCommon::CExportStream_Native>(sJournalDataPath);

		m_pSQLHandler = std::make_shared<AMCData::CSQLHandler_SQLite>(sJournalPath);

		std::string sQuery = "CREATE TABLE `logs` (";
		sQuery += "`logindex`	int DEFAULT 0, ";
		sQuery += "`loglevel`	int DEFAULT 0,";
		sQuery += "`timestamp`  varchar ( 64 ) NOT NULL, ";
		sQuery += "`subsystem`  varchar ( 8 ) NOT NULL, ";
		sQuery += "`message`	TEXT DEFAULT `` )";

		auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
		pStatement->execute();
		pStatement = nullptr;

		std::string sJournalQuery = "CREATE TABLE `journal_chunks` (";
		sJournalQuery += "`chunkindex` int DEFAULT 0, ";
		sJournalQuery += "`starttimestamp` int DEFAULT 0, ";
		sJournalQuery += "`endtimestamp` int DEFAULT 0, ";
		sJournalQuery += "`dataoffset` int DEFAULT 0, ";
		sJournalQuery += "`datalength` int DEFAULT 0) ";

		auto pJournalStatement = m_pSQLHandler->prepareStatement(sJournalQuery);
		pJournalStatement->execute();
		pJournalStatement = nullptr;
	}

	CJournal::~CJournal()
	{

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

	

	void CJournal::WriteJournalChunkData(const LibMCData_uint32 nChunkIndex, const LibMCData_uint64 nStartTimeStamp, const LibMCData_uint64 nEndTimeStamp, const LibMCData_uint64 nDataBufferSize, const LibMCData_uint8* pDataBuffer)
	{
		std::lock_guard<std::mutex> lockGuard(m_JournalMutex);

		uint64_t nPosition = m_pJournalStream->getPosition();
		if (nDataBufferSize > 0) {
			if (pDataBuffer == nullptr)
				throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

			uint64_t nPosition = m_pJournalStream->getPosition();
			m_pJournalStream->writeBuffer(pDataBuffer, nDataBufferSize);


			std::string sQuery = "INSERT INTO journal_chunks (chunkindex, starttimestamp, endtimestamp, dataoffset, datalength) VALUES (?, ?, ?, ?, ?)";
			auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
			pStatement->setInt64(1, nChunkIndex);
			pStatement->setInt64(2, nStartTimeStamp);
			pStatement->setInt64(3, nEndTimeStamp);
			pStatement->setInt64(4, nPosition);
			pStatement->setInt64(5, nDataBufferSize);
			pStatement->execute();
			pStatement = nullptr; 
		}

	}



}


