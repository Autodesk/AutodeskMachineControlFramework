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
#include "common_exportstream_native.hpp"

#include "amcdata_journal.hpp"

using namespace LibMCData::Impl;

/*************************************************************************************************************************
 Class definition of CLogSession 
**************************************************************************************************************************/

CLogSession::CLogSession(AMCData::PJournal pJournal)
	: m_pJournal (pJournal)
{
	if (pJournal.get() == nullptr)
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);
}

CLogSession::~CLogSession()
{

}

void CLogSession::AddEntry(const std::string & sMessage, const std::string & sSubSystem, const LibMCData::eLogLevel logLevel, const std::string & sTimestamp)
{
	m_pJournal->AddEntry(sMessage, sSubSystem, logLevel, sTimestamp);

}

LibMCData_uint32 CLogSession::GetMaxLogEntryID()
{
	return m_pJournal->GetMaxLogEntryID();
}

ILogEntryList* CLogSession::RetrieveLogEntriesByID(const LibMCData_uint32 nMinLogID, const LibMCData_uint32 nMaxLogID, const LibMCData::eLogLevel eMinLogLevel)
{
	auto pSQLHandler = m_pJournal->getSQLHandler();

	auto pLogEntryList = std::make_unique<CLogEntryList>();

	std::string sQuery = "SELECT logindex, loglevel, timestamp, subsystem, message FROM logs WHERE logindex >= ? AND logindex <= ? AND loglevel <= ?";
	auto pStatement = pSQLHandler->prepareStatement(sQuery);
	pStatement->setInt(1, nMinLogID);
	pStatement->setInt(2, nMaxLogID);
	pStatement->setInt(3, (int)eMinLogLevel);
	while (pStatement->nextRow()) {
		int32_t nLogID = pStatement->getColumnInt(1);
		int32_t nLogLevel = pStatement->getColumnInt(2);
		std::string sTimeStamp = pStatement->getColumnString(3);
		std::string sSubSystem = pStatement->getColumnString(4);
		std::string sMessage = pStatement->getColumnString(5);

		pLogEntryList->addEntry(nLogID, sMessage, sSubSystem, (eLogLevel)nLogLevel, sTimeStamp);
	}
	pStatement = nullptr;

	return pLogEntryList.release();
}

std::string CLogSession::GetSessionUUID()
{
	return m_pJournal->getSessionUUID();
}
