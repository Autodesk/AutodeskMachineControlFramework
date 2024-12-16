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


Abstract: This is a stub class definition of CJournalHandler_Current

*/

#include "libmcenv_journalhandler_current.hpp"
#include "libmcenv_interfaceexception.hpp"
#include "libmcenv_journalvariable_current.hpp"

// Include custom headers here.
#include "libmcenv_datetime.hpp"


using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CJournalHandler_Current 
**************************************************************************************************************************/

CJournalHandler_Current::CJournalHandler_Current(AMC::PStateJournal pStateJournal)
	: m_pStateJournal (pStateJournal)
{
	if (pStateJournal.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
}

CJournalHandler_Current::~CJournalHandler_Current()
{

}


IJournalVariable* CJournalHandler_Current::RetrieveJournalVariableFromTimeInterval(const std::string& sVariableName, const LibMCEnv_uint64 nStartTimeInMicroseconds, const LibMCEnv_uint64 nEndTimeInMicroseconds)
{
	AMC::sStateJournalInterval interval;
	interval.m_nStartTimeInMicroSeconds = nStartTimeInMicroseconds;
	interval.m_nEndTimeInMicroSeconds = nEndTimeInMicroseconds;

	return new CJournalVariable_Current(m_pStateJournal, sVariableName, interval);
}


IDateTime* CJournalHandler_Current::GetStartTime()
{
	return CDateTime::makefromUTC(m_pStateJournal->getStartTimeAsUTC ());
}


IDateTime* CJournalHandler_Current::GetEndTime()
{
	std::unique_ptr<CDateTime> endTime (CDateTime::makefromUTC(m_pStateJournal->getStartTimeAsUTC()));

	endTime->ShiftByMicroseconds(m_pStateJournal->getLifeTimeInMicroseconds());

	return endTime.release();
}

LibMCEnv_uint64 CJournalHandler_Current::GetJournalLifeTimeInMicroseconds()
{
	return m_pStateJournal->getLifeTimeInMicroseconds();
}

ILogEntryList* CJournalHandler_Current::RetrieveLogEntries(const LibMCEnv_uint64 nTimeDeltaInMicroseconds, LibMCEnv::eLogLevel& eMinLogLevel)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

ILogEntryList* CJournalHandler_Current::RetrieveLogEntriesFromTimeInterval(const LibMCEnv_uint64 nStartTimeInMicroseconds, const LibMCEnv_uint64 nEndTimeInMicroseconds, LibMCEnv::eLogLevel& eMinLogLevel)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

IAlertIterator* CJournalHandler_Current::RetrieveAlerts(const LibMCEnv_uint64 nTimeDeltaInMicroseconds)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

IAlertIterator* CJournalHandler_Current::RetrieveAlertsFromTimeInterval(const LibMCEnv_uint64 nStartTimeInMicroseconds, const LibMCEnv_uint64 nEndTimeInMicroseconds)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

