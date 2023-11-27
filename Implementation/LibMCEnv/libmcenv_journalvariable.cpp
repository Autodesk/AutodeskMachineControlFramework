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


Abstract: This is a stub class definition of CJournalVariable

*/

#include "libmcenv_journalvariable.hpp"
#include "libmcenv_interfaceexception.hpp"
#include "amc_statejournal.hpp"
#include "amc_statejournalstream.hpp"


// Include custom headers here.


using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CJournalVariable 
**************************************************************************************************************************/
CJournalVariable::CJournalVariable(AMC::PStateJournal pStateJournal, const std::string& sVariableName, uint64_t nStartTimeStamp, uint64_t nEndTimeStamp)
    : m_pStateJournal (pStateJournal), m_sVariableName (sVariableName), m_nStartTimeStamp (nStartTimeStamp), m_nEndTimeStamp (nEndTimeStamp)
{
    if (pStateJournal.get () == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
    if (sVariableName.empty())
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_EMPTYJOURNALVARIABLENAME);


    if (nStartTimeStamp >= nEndTimeStamp)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDJOURNALVARIABLEINTERVAL);

    //auto pValue = m_pStateJournal->findValue(sVariableName, false);
    //if (pValue.get () == nullptr)
        //throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_JOURNALVARIABLENOTFOUND, "journal variable not found: " + sVariableName);

}

CJournalVariable::~CJournalVariable()
{

}

std::string CJournalVariable::GetVariableName()
{
    return m_sVariableName;
}

LibMCEnv_uint64 CJournalVariable::GetStartTimeStamp()
{
    return m_nStartTimeStamp;
}

LibMCEnv_uint64 CJournalVariable::GetEndTimeStamp()
{
    return m_nEndTimeStamp;
}

LibMCEnv_double CJournalVariable::ComputeFullAverage()
{
    return ComputeAverage(m_nStartTimeStamp, m_nEndTimeStamp, true);
}

LibMCEnv_double CJournalVariable::ComputeAverage(const LibMCEnv_uint64 nStartTimeInMS, const LibMCEnv_uint64 nEndTimeInMS, const bool bClampInterval)
{
    uint64_t nClampedStartTimeInMS = nStartTimeInMS;
    uint64_t nClampedEndTimeInMS = nEndTimeInMS;
    if (bClampInterval) {
        if (nClampedStartTimeInMS < m_nStartTimeStamp)
            nClampedStartTimeInMS = m_nStartTimeStamp;
        if (nClampedEndTimeInMS > m_nEndTimeStamp)
            nClampedEndTimeInMS = m_nEndTimeStamp;
    }
    else {
        if (nClampedStartTimeInMS < m_nStartTimeStamp)
            throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COMPUTATIONOUTSIDEOFJOURNALDATA);
        if (nClampedEndTimeInMS > m_nEndTimeStamp)
            throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COMPUTATIONOUTSIDEOFJOURNALDATA);
    }

    auto sStatistics = m_pStateJournal->computeStatistics(m_sVariableName, nClampedStartTimeInMS, nClampedEndTimeInMS);
    return sStatistics.m_dAverageValue;

}

IUniformJournalSampling * CJournalVariable::ComputeUniformAverageSamples(const LibMCEnv_uint64 nStartTimeInMS, const LibMCEnv_uint64 nEndTimeInMS, const LibMCEnv_uint32 nNumberOfSamples, const LibMCEnv_double dMovingAverageDelta, const bool bClampInterval)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}


void CJournalVariable::ReceiveRawTimeStream(LibMCEnv_uint64 nTimeStreamEntriesBufferSize, LibMCEnv_uint64* pTimeStreamEntriesNeededCount, LibMCEnv::sTimeStreamEntry* pTimeStreamEntriesBuffer)
{
    std::vector<AMC::sJournalTimeStreamDoubleEntry> timeStream;

    m_pStateJournal->readDoubleTimeStream(m_sVariableName, m_nStartTimeStamp, m_nEndTimeStamp, timeStream);

    // Add start value as first entry!
    size_t nRawEntryCount = timeStream.size();

    if (pTimeStreamEntriesNeededCount != nullptr)
        *pTimeStreamEntriesNeededCount = nRawEntryCount;

    if (pTimeStreamEntriesBuffer != nullptr) 
    {
        if (nTimeStreamEntriesBufferSize < nRawEntryCount)
            throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_BUFFERTOOSMALL);

        auto pTargetEntry = pTimeStreamEntriesBuffer;

        for (auto& timeStreamIter : timeStream) {
            pTargetEntry->m_TimestampInMS = timeStreamIter.m_nTimeStamp;
            pTargetEntry->m_Value = timeStreamIter.m_dValue;
            pTargetEntry++;
        }
    }

}
