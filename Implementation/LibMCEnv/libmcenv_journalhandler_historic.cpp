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


Abstract: This is a stub class definition of CJournalHandler

*/

#include "libmcenv_journalhandler_historic.hpp"
#include "libmcenv_interfaceexception.hpp"
#include "libmcenv_journalvariable.hpp"

// Include custom headers here.
#include "libmcenv_datetime.hpp"


using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CJournalHandler_Historic 
**************************************************************************************************************************/

CJournalHandler_Historic::CJournalHandler_Historic(LibMCData::PJournalReader pJournalReader)
	: m_pJournalReader (pJournalReader)
{
	if (pJournalReader.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
}

CJournalHandler_Historic::~CJournalHandler_Historic()
{

}


IJournalVariable * CJournalHandler_Historic::RetrieveJournalVariable(const std::string & sVariableName, const LibMCEnv_uint64 nTimeDeltaInMicroseconds)
{
	std::lock_guard<std::mutex> lockGuard(m_JournalReaderMutex);
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

IJournalVariable* CJournalHandler_Historic::RetrieveJournalVariableFromTimeInterval(const std::string& sVariableName, const LibMCEnv_uint64 nStartTimeInMicroseconds, const LibMCEnv_uint64 nEndTimeInMicroseconds)
{
	std::lock_guard<std::mutex> lockGuard(m_JournalReaderMutex);
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}


IDateTime* CJournalHandler_Historic::GetStartTime()
{
	std::lock_guard<std::mutex> lockGuard(m_JournalReaderMutex);
	return CDateTime::makefromUTC(m_pJournalReader->GetStartTime ());
}

ILogEntryList* CJournalHandler_Historic::RetrieveLogEntries(const LibMCEnv_uint64 nTimeDeltaInMicroseconds, LibMCEnv::eLogLevel& eMinLogLevel)
{
	std::lock_guard<std::mutex> lockGuard(m_JournalReaderMutex);
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

ILogEntryList* CJournalHandler_Historic::RetrieveLogEntriesFromTimeInterval(const LibMCEnv_uint64 nStartTimeInMicroseconds, const LibMCEnv_uint64 nEndTimeInMicroseconds, LibMCEnv::eLogLevel& eMinLogLevel)
{
	std::lock_guard<std::mutex> lockGuard(m_JournalReaderMutex);
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

IAlertIterator* CJournalHandler_Historic::RetrieveAlerts(const LibMCEnv_uint64 nTimeDeltaInMicroseconds)
{
	std::lock_guard<std::mutex> lockGuard(m_JournalReaderMutex);
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

IAlertIterator* CJournalHandler_Historic::RetrieveAlertsFromTimeInterval(const LibMCEnv_uint64 nStartTimeInMicroseconds, const LibMCEnv_uint64 nEndTimeInMicroseconds)
{
	std::lock_guard<std::mutex> lockGuard(m_JournalReaderMutex);
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

