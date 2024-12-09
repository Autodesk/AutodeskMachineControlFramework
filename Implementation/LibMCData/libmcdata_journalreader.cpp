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


Abstract: This is a stub class definition of CJournalReader

*/

#include "libmcdata_journalreader.hpp"
#include "libmcdata_interfaceexception.hpp"

// Include custom headers here.
#include "common_utils.hpp"
#include "amcdata_sqlhandler_sqlite.hpp"

using namespace LibMCData::Impl;

/*************************************************************************************************************************
 Class definition of CJournalReader 
**************************************************************************************************************************/

CJournalReader::CJournalReader(AMCData::PSQLHandler pSQLHandler, const std::string& sJournalUUID, const std::string& sJournalBasePath)
    : m_pSQLHandler (pSQLHandler), 
    m_sJournalUUID (AMCCommon::CUtils::normalizeUUIDString (sJournalUUID)), 
    m_sJournalBasePath (sJournalBasePath), 
    m_nSchemaVersion (0)
{
    if (pSQLHandler.get() == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

    auto pStatement = pSQLHandler->prepareStatement("SELECT starttime, logfilename, schemaversion FROM journals WHERE uuid=?");
    pStatement->setString(1, m_sJournalUUID);

    if (!pStatement->nextRow ())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_COULDNOTFINDJOURNALUUID, "could not find journal uuid: " + m_sJournalUUID);

    m_sStartTime = pStatement->getColumnString(1);
    std::string sLogFileName = pStatement->getColumnString(2);
    m_nSchemaVersion = pStatement->getColumnInt(3);

    pStatement = nullptr;

    std::string sJournalFileName = m_sJournalBasePath + "/" + sLogFileName;

    m_pJournalSQLHandler = std::make_shared<AMCData::CSQLHandler_SQLite>(sJournalFileName);

}

CJournalReader::~CJournalReader()
{
    m_pJournalSQLHandler = nullptr;
    m_pSQLHandler = nullptr;

}


std::string CJournalReader::GetJournalUUID()
{
    return m_sJournalUUID;
}

std::string CJournalReader::GetStartTime()
{
    return m_sStartTime;
}

IJournalChunkIntegerData * CJournalReader::ReadChunkIntegerData(const LibMCData_uint32 nChunkIndex)
{
	throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_NOTIMPLEMENTED);
}

