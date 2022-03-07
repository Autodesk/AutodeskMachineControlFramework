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


#include "amc_logger_database.hpp"
#include "libmc_exceptiontypes.hpp"

#define AMC_MAXLOGMESSAGE_REQUESTCOUNT 1024

namespace AMC {
		
	CLogger_Database::CLogger_Database(LibMCData::PLogSession pLogSession)
		: m_pLogSession (pLogSession),
		m_MaxLogMessageRequestCount (AMC_MAXLOGMESSAGE_REQUESTCOUNT)
	{
		LibMCAssertNotNull(pLogSession.get());

	}
	
	CLogger_Database::~CLogger_Database()
	{
	}

	void CLogger_Database::logMessageEx(const std::string& sMessage, const std::string& sSubSystem, const eLogLevel logLevel, const std::string& sTimeStamp)
	{

		std::lock_guard<std::mutex> lockGuard(m_DBMutex);
		try {
			m_pLogSession->AddEntry(sMessage, sSubSystem, logLevel, sTimeStamp);
		}
		catch (...) {
			// Fallback logging needs to take care on this.
		}

	}



	void CLogger_Database::retrieveLogMessages(std::vector<CLoggerEntry>& entryBuffer, const uint32_t startID, const uint32_t endID, const eLogLevel eMinLogLevel)
	{
		std::lock_guard<std::mutex> lockGuard(m_DBMutex);

		if (startID > endID)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

		if (startID > (endID + m_MaxLogMessageRequestCount))
			throw ELibMCInterfaceException(LIBMC_ERROR_TOOMANYREQUESTEDLOGS);

/*		std::string sQuery = "SELECT logindex, loglevel, timestamp, subsystem, message FROM logs WHERE logindex >= ? AND logindex <= ? AND loglevel <= ?";
		auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
		pStatement->setInt(1, startID);
		pStatement->setInt(2, endID);
		pStatement->setInt(3, (int) eMinLogLevel);
		while (pStatement->nextRow()) {
			int nLogID = pStatement->getColumnInt(0);
			int nLogLevel = pStatement->getColumnInt(1);
			std::string sTimeStamp = pStatement->getColumnString(2);
			std::string sSubSystem = pStatement->getColumnString(3);
			std::string sMessage = pStatement->getColumnString(4);

			entryBuffer.push_back(CLoggerEntry (nLogID, sMessage, sSubSystem, (AMC::eLogLevel) nLogLevel, sTimeStamp));
		} 
		pStatement = nullptr; */

	}

	bool CLogger_Database::supportsLogMessagesRetrieval()
	{
		return true;
	}


}


