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


Abstract: This is a stub class definition of CJournalSession

*/

#include "libmcdata_journalsession.hpp"
#include "libmcdata_interfaceexception.hpp"

// Include custom headers here.
#include "libmcdata_journalchunkintegerdata.hpp"


using namespace LibMCData::Impl;

/*************************************************************************************************************************
 Class definition of CJournalSession 
**************************************************************************************************************************/

CJournalSession::CJournalSession(AMCData::PJournal pJournal)
    : m_pJournal (pJournal)
{
    if (pJournal.get() == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

}

CJournalSession::~CJournalSession()
{

}


void CJournalSession::WriteJournalChunkIntegerData(const LibMCData_uint32 nChunkIndex, const LibMCData_uint64 nStartTimeStamp, const LibMCData_uint64 nEndTimeStamp, const LibMCData_uint64 nVariableInfoBufferSize, const LibMCData::sJournalChunkVariableInfo* pVariableInfoBuffer, const LibMCData_uint64 nTimeStampDataBufferSize, const LibMCData_uint32* pTimeStampDataBuffer, const LibMCData_uint64 nValueDataBufferSize, const LibMCData_int64* pValueDataBuffer) 
{
    m_pJournal->WriteJournalChunkIntegerData(nChunkIndex, nStartTimeStamp, nEndTimeStamp, nVariableInfoBufferSize, pVariableInfoBuffer, nTimeStampDataBufferSize, pTimeStampDataBuffer, nValueDataBufferSize, pValueDataBuffer);
}

void CJournalSession::CreateVariableInJournalDB(const std::string& sName, const LibMCData_uint32 nID, const LibMCData_uint32 nIndex, const LibMCData::eParameterDataType eDataType)
{
    m_pJournal->CreateVariableInJournalDB(sName, nID, nIndex, eDataType);
}


IJournalChunkIntegerData* CJournalSession::ReadChunkIntegerData(const LibMCData_uint32 nChunkIndex)
{
    auto pResult = std::make_unique<CJournalChunkIntegerData>(nChunkIndex);

    uint64_t nStartTimeStamp = 0;
    uint64_t nEndTimeStamp = 0;

    m_pJournal->ReadJournalChunkIntegerData(nChunkIndex, nStartTimeStamp, nEndTimeStamp, pResult->getVariableInfoInternal(), pResult->getTimeStampsInternal(), pResult->getValueDataInternal());
    pResult->setTimeInterval(nStartTimeStamp, nEndTimeStamp);

    return pResult.release();
}


LibMCData_uint64 CJournalSession::GetChunkCacheQuota()
{
    return m_pJournal->getMaxMemoryQuotaInBytes();
}

LibMCData_uint32 CJournalSession::GetChunkIntervalInMicroseconds()
{
    return m_pJournal->getChunkIntervalInMicroseconds();
}

std::string CJournalSession::GetSessionUUID()
{
    return m_pJournal->getSessionUUID();
}

