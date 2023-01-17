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

#include "libmcdata_logsession.hpp"
#include "libmcdata_interfaceexception.hpp"
#include "libmcdata_logentrylist.hpp"

// Include custom headers here.
#include "amcdata_sqlhandler_sqlite.hpp"

using namespace LibMCData::Impl;

/*************************************************************************************************************************
 Class definition of CLogSession 
**************************************************************************************************************************/

CLogSession::CLogSession(const std::string & sLogPath)
	: m_LogID (1)
{
	m_pSQLHandler = std::make_shared<AMCData::CSQLHandler_SQLite>(sLogPath);

	std::string sQuery = "CREATE TABLE `logs` (";
	sQuery += "`logindex`	int DEFAULT 0, ";
	sQuery += "`loglevel`	int DEFAULT 0,";
	sQuery += "`timestamp`  varchar ( 64 ) NOT NULL, ";
	sQuery += "`subsystem`  varchar ( 8 ) NOT NULL, ";
	sQuery += "`message`	TEXT DEFAULT `` )";

	auto pStatement = m_pSQLHandler->prepareStatement (sQuery);
	pStatement->execute ();
	pStatement = nullptr; 

}

void CLogSession::AddEntry(const std::string & sMessage, const std::string & sSubSystem, const LibMCData::eLogLevel logLevel, const std::string & sTimestamp)
{
	std::lock_guard<std::mutex> lockGuard(m_Mutex);

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

LibMCData_uint32 CLogSession::GetMaxLogEntryID()
{
	return m_LogID;
}

ILogEntryList* CLogSession::RetrieveLogEntriesByID(const LibMCData_uint32 nMinLogID, const LibMCData_uint32 nMaxLogID, const LibMCData::eLogLevel eMinLogLevel)
{
	auto pLogEntryList = std::make_unique<CLogEntryList> ();

	std::string sQuery = "SELECT logindex, loglevel, timestamp, subsystem, message FROM logs WHERE logindex >= ? AND logindex <= ? AND loglevel <= ?";
	auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
	pStatement->setInt(1, nMinLogID);
	pStatement->setInt(2, nMaxLogID);
	pStatement->setInt(3, (int)eMinLogLevel);
	while (pStatement->nextRow()) {
		int32_t nLogID = pStatement->getColumnInt(1);
		int32_t nLogLevel = pStatement->getColumnInt(2);
		std::string sTimeStamp = pStatement->getColumnString(3);
		std::string sSubSystem = pStatement->getColumnString(4);
		std::string sMessage = pStatement->getColumnString(5);

		pLogEntryList->addEntry (nLogID, sMessage, sSubSystem, (eLogLevel) nLogLevel, sTimeStamp);
	}
	pStatement = nullptr;

	return pLogEntryList.release();
}

