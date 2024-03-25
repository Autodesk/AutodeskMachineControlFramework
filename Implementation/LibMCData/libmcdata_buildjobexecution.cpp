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


Abstract: This is a stub class definition of CBuildJobExecution

*/

#include "libmcdata_buildjobexecution.hpp"
#include "libmcdata_interfaceexception.hpp"

// Include custom headers here.
#include "common_utils.hpp"
#include "common_chrono.hpp"

using namespace LibMCData::Impl;

/*************************************************************************************************************************
 Class definition of CBuildJobExecution 
**************************************************************************************************************************/

CBuildJobExecution::CBuildJobExecution(AMCData::PSQLHandler pSQLHandler, const std::string& sExecutionUUID, AMCData::PStorageState pStorageState)
	: m_pSQLHandler (pSQLHandler),
	m_sExecutionUUID (AMCCommon::CUtils::normalizeUUIDString (sExecutionUUID)),
	m_nStartJournalTimestamp (0),
	m_pStorageState (pStorageState)
{
	if (pSQLHandler.get() == nullptr)
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);
	if (pStorageState.get () == nullptr)
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

	// Retrieve non-mutable values at the beginning
	std::string sSelectQuery = "SELECT jobuuid, journaluuid, useruuid, startjournaltimestamp FROM buildjobexecutions WHERE uuid=? AND active=1";
	auto pSelectStatement = m_pSQLHandler->prepareStatement(sSelectQuery);
	pSelectStatement->setString(1, m_sExecutionUUID);

	if (!pSelectStatement->nextRow())
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_BUILDJOBEXECUTIONNOTFOUND, "build job execution not found: " + m_sExecutionUUID);

	m_sJobUUID = AMCCommon::CUtils::normalizeUUIDString (pSelectStatement->getColumnString(1));
	m_sJournalUUID = AMCCommon::CUtils::normalizeUUIDString(pSelectStatement->getColumnString(2));
	m_sUserUUID = AMCCommon::CUtils::normalizeUUIDString(pSelectStatement->getColumnString(3));
	int64_t nStartTimeStamp = pSelectStatement->getColumnInt64(4);
	if (nStartTimeStamp < 0)
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDBUILDJOBEXECUTIONSTART, "invalid build job execution start: " + m_sExecutionUUID);
	if (!AMCCommon::CChrono::timeStampIsWithinAMillionYears ((uint64_t)nStartTimeStamp))
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDBUILDJOBEXECUTIONSTART, "invalid build job execution start: " + m_sExecutionUUID);

	m_nStartJournalTimestamp = (uint64_t)nStartTimeStamp;

	pSelectStatement = nullptr;
}

CBuildJobExecution::~CBuildJobExecution()
{

}

std::string CBuildJobExecution::GetExecutionUUID()
{
	return m_sExecutionUUID;
}

std::string CBuildJobExecution::GetJobUUID()
{
	return m_sJobUUID;
}

LibMCData::eBuildJobExecutionStatus CBuildJobExecution::GetStatus()
{
	std::string sSelectQuery = "SELECT status FROM buildjobexecutions WHERE uuid=? AND active=1";
	auto pSelectStatement = m_pSQLHandler->prepareStatement(sSelectQuery);
	pSelectStatement->setString(1, m_sExecutionUUID);

	if (!pSelectStatement->nextRow())
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_BUILDJOBEXECUTIONNOTFOUND, "build job execution not found: " + m_sExecutionUUID);

	return CBuildJobExecution::convertStringToBuildJobExecutionStatus (pSelectStatement->getColumnString(1));
}

void CBuildJobExecution::ChangeStatus(const LibMCData::eBuildJobExecutionStatus eNewExecutionStatus)
{
	auto pTransaction = m_pSQLHandler->beginTransaction();

	std::string sSelectQuery = "SELECT status FROM buildjobexecutions WHERE uuid=? AND active=1";
	auto pSelectStatement = pTransaction->prepareStatement(sSelectQuery);
	pSelectStatement->setString(1, m_sExecutionUUID);

	if (!pSelectStatement->nextRow())
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_BUILDJOBEXECUTIONNOTFOUND, "build job execution not found: " + m_sExecutionUUID);

	auto eOldStatus = convertStringToBuildJobExecutionStatus(pSelectStatement->getColumnString(1));

	pSelectStatement = nullptr;

	if (eOldStatus != eBuildJobExecutionStatus::InProcess)
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_BUILDJOBEXECUTIONISNOTINPROCESS, "build job execution is not in process: " + m_sExecutionUUID);

	std::string sUpdateQuery = "UPDATE buildjobexecutions SET status=? WHERE uuid=? AND active=1";
	auto pUpdateStatement = pTransaction->prepareStatement(sUpdateQuery);
	pUpdateStatement->setString(1, convertBuildJobExecutionStatusToString (eNewExecutionStatus));
	pUpdateStatement->setString(2, m_sExecutionUUID);
	pUpdateStatement->execute();

	pTransaction->commit();
}

std::string CBuildJobExecution::GetDescription()
{
	std::string sSelectQuery = "SELECT description FROM buildjobexecutions WHERE uuid=? AND active=1";
	auto pSelectStatement = m_pSQLHandler->prepareStatement(sSelectQuery);
	pSelectStatement->setString(1, m_sExecutionUUID);

	if (!pSelectStatement->nextRow())
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_BUILDJOBEXECUTIONNOTFOUND, "build job execution not found: " + m_sExecutionUUID);

	return pSelectStatement->getColumnString(1);
}

void CBuildJobExecution::SetDescription(const std::string & sNewDescription)
{
	std::string sUpdateQuery = "UPDATE buildjobexecutions SET description=? WHERE uuid=? AND active=1";
	auto pUpdateStatement = m_pSQLHandler->prepareStatement(sUpdateQuery);
	pUpdateStatement->setString(1, sNewDescription);
	pUpdateStatement->setString(2, m_sExecutionUUID);
	pUpdateStatement->execute();
}

std::string CBuildJobExecution::GetJournalUUID()
{
	return m_sJournalUUID;
}

std::string CBuildJobExecution::GetUserUUID()
{
	return m_sUserUUID;
}

LibMCData_uint64 CBuildJobExecution::GetStartTimeStampInMicroseconds()
{
	return m_nStartJournalTimestamp;
}

LibMCData_uint64 CBuildJobExecution::GetEndTimeStampInMicroseconds()
{
	std::string sSelectQuery = "SELECT status, endjournaltimestamp FROM buildjobexecutions WHERE uuid=? AND active=1";
	auto pSelectStatement = m_pSQLHandler->prepareStatement(sSelectQuery);
	pSelectStatement->setString(1, m_sExecutionUUID);

	if (!pSelectStatement->nextRow())
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_BUILDJOBEXECUTIONNOTFOUND, "build job execution not found: " + m_sExecutionUUID);

	auto eStatus = convertStringToBuildJobExecutionStatus(pSelectStatement->getColumnString(1));
	if ((eStatus != eBuildJobExecutionStatus::Finished) && (eStatus != eBuildJobExecutionStatus::Failed))
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_BUILDJOBEXECUTIONENDNOTAVAILABLE, "build job execution end is not available: " + m_sExecutionUUID);

	int64_t nEndTimeStamp = pSelectStatement->getColumnInt64(4);
	if (nEndTimeStamp < 0)
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDBUILDJOBEXECUTIONEND, "invalid build job execution end: " + m_sExecutionUUID);
	if (!AMCCommon::CChrono::timeStampIsWithinAMillionYears((uint64_t)nEndTimeStamp))
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDBUILDJOBEXECUTIONEND, "invalid build job execution end: " + m_sExecutionUUID);

	if ((uint64_t) (nEndTimeStamp) < m_nStartJournalTimestamp)
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_BUILDJOBEXECUTIONENDISBEFORESTART, "Build job execution end is before start: " + m_sExecutionUUID);

	return (uint64_t)nEndTimeStamp;

}

LibMCData_uint64 CBuildJobExecution::ComputeElapsedTimeInMicroseconds(const LibMCData_uint64 nGlobalTimerInMicroseconds) 
{

	if (!AMCCommon::CChrono::timeStampIsWithinAMillionYears(nGlobalTimerInMicroseconds))
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

	std::string sSelectQuery = "SELECT status, endjournaltimestamp FROM buildjobexecutions WHERE uuid=? AND active=1";
	auto pSelectStatement = m_pSQLHandler->prepareStatement(sSelectQuery);
	pSelectStatement->setString(1, m_sExecutionUUID);

	if (!pSelectStatement->nextRow())
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_BUILDJOBEXECUTIONNOTFOUND, "build job execution not found: " + m_sExecutionUUID);

	auto eStatus = convertStringToBuildJobExecutionStatus(pSelectStatement->getColumnString(1));

	switch (eStatus) {

		case eBuildJobExecutionStatus::InProcess:
		{
			if (m_sJournalUUID != m_pStorageState->getSessionUUID ())
				throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_BUILDJOBEXECUTIONISFROMPASTJOURNAL, "build job execution is from past journal: " + m_sExecutionUUID);

			if (nGlobalTimerInMicroseconds < m_nStartJournalTimestamp)
				throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_BUILDJOBEXECUTIONSTARTISINTHEFUTURE, "build job execution start is in the future: " + m_sExecutionUUID);
			
			return nGlobalTimerInMicroseconds - m_nStartJournalTimestamp;
		}

		case eBuildJobExecutionStatus::Finished:
		case eBuildJobExecutionStatus::Failed:
		{
			int64_t nEndTimeStamp = pSelectStatement->getColumnInt64(4);
			if (nEndTimeStamp < 0)
				throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDBUILDJOBEXECUTIONEND, "invalid build job execution end: " + m_sExecutionUUID);
			if (!AMCCommon::CChrono::timeStampIsWithinAMillionYears((uint64_t)nEndTimeStamp))
				throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDBUILDJOBEXECUTIONEND, "invalid build job execution end: " + m_sExecutionUUID);

			if ((uint64_t)(nEndTimeStamp) < m_nStartJournalTimestamp)
				throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_BUILDJOBEXECUTIONENDISBEFORESTART, "Build job execution end is before start: " + m_sExecutionUUID);

			return (uint64_t)nEndTimeStamp - m_nStartJournalTimestamp;
		}

		default:
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_BUILDJOBDURATIONNOTAVAILABLE, "build job duration is not available: " + m_sExecutionUUID);

	}
}

std::string CBuildJobExecution::convertBuildJobExecutionStatusToString(const LibMCData::eBuildJobExecutionStatus eStatus)
{
	switch (eStatus) {
	case LibMCData::eBuildJobExecutionStatus::InProcess: return "inprocess";
	case LibMCData::eBuildJobExecutionStatus::Finished: return "finished";
	case LibMCData::eBuildJobExecutionStatus::Failed: return "failed";
	case LibMCData::eBuildJobExecutionStatus::Unknown: return "unknown";
	default:
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDBUILDJOBEXECUTIONSTATUS);
	}
}

LibMCData::eBuildJobExecutionStatus CBuildJobExecution::convertStringToBuildJobExecutionStatus(const std::string& sValue)
{
	if (sValue == "inprocess")
		return LibMCData::eBuildJobExecutionStatus::InProcess;
	if (sValue == "finished")
		return LibMCData::eBuildJobExecutionStatus::Finished;
	if (sValue == "failed")
		return LibMCData::eBuildJobExecutionStatus::Failed;
	if (sValue == "unknown")
		return LibMCData::eBuildJobExecutionStatus::Unknown;

	throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDBUILDJOBEXECUTIONSTATUS);
}
