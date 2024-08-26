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


Abstract: This is a stub class definition of CJournalChunkIntegerData

*/

#include "libmcdata_journalchunkintegerdata.hpp"
#include "libmcdata_interfaceexception.hpp"

// Include custom headers here.


using namespace LibMCData::Impl;

/*************************************************************************************************************************
 Class definition of CJournalChunkIntegerData 
**************************************************************************************************************************/

CJournalChunkIntegerData::CJournalChunkIntegerData(uint32_t nChunkIndex)
    : m_nChunkIndex (nChunkIndex),
    m_nStartTimeStamp (0),
    m_nEndTimeStamp (0)
{

}


CJournalChunkIntegerData::~CJournalChunkIntegerData()
{

}

LibMCData_uint32 CJournalChunkIntegerData::GetChunkIndex()
{
    return m_nChunkIndex;
}

LibMCData_uint64 CJournalChunkIntegerData::GetStartTimeStamp()
{
    return m_nStartTimeStamp;
}

LibMCData_uint64 CJournalChunkIntegerData::GetEndTimeStamp()
{
    return m_nEndTimeStamp;
}

void CJournalChunkIntegerData::GetVariableInfo(LibMCData_uint64 nVariableInfoBufferSize, LibMCData_uint64* pVariableInfoNeededCount, LibMCData::sJournalChunkVariableInfo * pVariableInfoBuffer)
{
    if (pVariableInfoNeededCount != nullptr)
        *pVariableInfoNeededCount = m_VariableInfo.size();

    if (pVariableInfoBuffer != nullptr) {
        if (nVariableInfoBufferSize < m_VariableInfo.size())
            throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_BUFFERTOOSMALL);

        LibMCData::sJournalChunkVariableInfo* pTarget = pVariableInfoBuffer;
        for (auto& info : m_VariableInfo) {
            *pTarget = info;
            pTarget++;
        }
    }
}

void CJournalChunkIntegerData::GetTimeStampData(LibMCData_uint64 nTimeStampDataBufferSize, LibMCData_uint64* pTimeStampDataNeededCount, LibMCData_uint32 * pTimeStampDataBuffer)
{
    if (pTimeStampDataNeededCount != nullptr)
        *pTimeStampDataNeededCount = m_TimeStamps.size();

    if (pTimeStampDataBuffer != nullptr) {
        if (nTimeStampDataBufferSize < m_TimeStamps.size())
            throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_BUFFERTOOSMALL);

        uint32_t* pTarget = pTimeStampDataBuffer;
        for (auto timeStamp : m_TimeStamps) {
            *pTarget = timeStamp;
            pTarget++;
        }
    }
}

void CJournalChunkIntegerData::GetValueData(LibMCData_uint64 nValueDataBufferSize, LibMCData_uint64* pValueDataNeededCount, LibMCData_int64 * pValueDataBuffer)
{
    if (pValueDataNeededCount != nullptr)
        *pValueDataNeededCount = m_ValueData.size();

    if (pValueDataBuffer != nullptr) {
        if (nValueDataBufferSize < m_ValueData.size())
            throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_BUFFERTOOSMALL);

        int64_t* pTarget = pValueDataBuffer;
        for (auto timeStamp : m_ValueData) {
            *pTarget = timeStamp;
            pTarget++;
        }
    }
}

std::vector<LibMCData::sJournalChunkVariableInfo>& CJournalChunkIntegerData::getVariableInfoInternal()
{
    return m_VariableInfo;
}

std::vector<uint32_t>& CJournalChunkIntegerData::getTimeStampsInternal()
{
    return m_TimeStamps;
}

std::vector<int64_t>& CJournalChunkIntegerData::getValueDataInternal()
{
    return m_ValueData;
}

void CJournalChunkIntegerData::setTimeInterval(uint64_t nStartTimeStamp, uint64_t nEndTimeStamp)
{
    m_nStartTimeStamp = nStartTimeStamp;
    m_nEndTimeStamp = nEndTimeStamp;
}
