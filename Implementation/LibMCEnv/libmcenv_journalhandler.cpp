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

#include "libmcenv_journalhandler.hpp"
#include "libmcenv_interfaceexception.hpp"
#include "libmcenv_journalvariable.hpp"

// Include custom headers here.


using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CJournalHandler 
**************************************************************************************************************************/

CJournalHandler::CJournalHandler(AMC::PStateJournal pStateJournal)
	: m_pStateJournal (pStateJournal)
{
	if (pStateJournal.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
}

CJournalHandler::~CJournalHandler()
{

}


IJournalVariable * CJournalHandler::RetrieveJournalVariable(const std::string & sVariableName, const LibMCEnv_uint64 nTimeDeltaInMilliseconds)
{
	uint64_t nStartTimeStamp = 0;
	uint64_t nEndTimeStamp = 0;
	m_pStateJournal->retrieveRecentInterval(nTimeDeltaInMilliseconds, nStartTimeStamp, nEndTimeStamp);

	return new CJournalVariable(m_pStateJournal, sVariableName, nStartTimeStamp, nEndTimeStamp);
}

IJournalVariable * CJournalHandler::RetrieveJournalVariableFromTimeInterval(const std::string & sVariableName, const LibMCEnv_uint64 nStartTimeInMilliseconds, const LibMCEnv_uint64 nEndTimeInMilliseconds)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

LibMCEnv_uint64 CJournalHandler::StoreJournalMarker(const std::string & sMarkerType, const std::string & sMarkerName, const bool bMustBeUnique)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

bool CJournalHandler::HasJournalMarker(const std::string & sMarkerType, const std::string & sMarkerName)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

LibMCEnv_uint64 CJournalHandler::RetrieveJournalMarker(const std::string & sMarkerType, const std::string & sMarkerName, const bool bMustBeUnique)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CJournalHandler::RetrieveJournalMarkers(const std::string & sMarkerType, const std::string & sMarkerName, LibMCEnv_uint64 nTimeStampsBufferSize, LibMCEnv_uint64* pTimeStampsNeededCount, LibMCEnv_uint64 * pTimeStampsBuffer)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

