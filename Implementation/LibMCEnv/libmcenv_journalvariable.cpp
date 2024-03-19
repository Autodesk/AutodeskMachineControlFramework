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
CJournalVariable::CJournalVariable(AMC::PStateJournal pStateJournal, const std::string& sVariableName, AMC::sStateJournalInterval interval)
    : m_pStateJournal (pStateJournal), m_sVariableName (sVariableName), m_Interval (interval)
{
    if (pStateJournal.get () == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
    if (sVariableName.empty())
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_EMPTYJOURNALVARIABLENAME);


    if (interval.m_nStartTimeInMicroSeconds >= interval.m_nEndTimeInMicroSeconds)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDJOURNALVARIABLEINTERVAL);

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
    return m_Interval.m_nStartTimeInMicroSeconds;
}

LibMCEnv_uint64 CJournalVariable::GetEndTimeStamp()
{
    return m_Interval.m_nEndTimeInMicroSeconds;
}

LibMCEnv_double CJournalVariable::ComputeFullAverage()
{
    return ComputeAverage(m_Interval.m_nStartTimeInMicroSeconds, m_Interval.m_nEndTimeInMicroSeconds, true);
}

LibMCEnv_double CJournalVariable::ComputeAverage(const LibMCEnv_uint64 nStartTimeInMicroSeconds, const LibMCEnv_uint64 nEndTimeInMicroSeconds, const bool bClampInterval)
{
    uint64_t nClampedStartTimeInMicroSeconds = nStartTimeInMicroSeconds;
    uint64_t nClampedEndTimeInMicroSeconds = nEndTimeInMicroSeconds;
    if (bClampInterval) {
        if (nClampedStartTimeInMicroSeconds < m_Interval.m_nStartTimeInMicroSeconds)
            nClampedStartTimeInMicroSeconds = m_Interval.m_nStartTimeInMicroSeconds;
        if (nClampedEndTimeInMicroSeconds > m_Interval.m_nEndTimeInMicroSeconds)
            nClampedEndTimeInMicroSeconds = m_Interval.m_nEndTimeInMicroSeconds;
    }
    else {
        if (nClampedStartTimeInMicroSeconds < m_Interval.m_nStartTimeInMicroSeconds)
            throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COMPUTATIONOUTSIDEOFJOURNALDATA);
        if (nClampedEndTimeInMicroSeconds > m_Interval.m_nEndTimeInMicroSeconds)
            throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COMPUTATIONOUTSIDEOFJOURNALDATA);
    }

    AMC::sStateJournalInterval interval;
    interval.m_nStartTimeInMicroSeconds = nClampedStartTimeInMicroSeconds;
    interval.m_nEndTimeInMicroSeconds = nClampedEndTimeInMicroSeconds;
    auto sStatistics = m_pStateJournal->computeStatistics(m_sVariableName, interval);
    return sStatistics.m_dAverageValue;

}

LibMCEnv_double CJournalVariable::ComputeSample(const LibMCEnv_uint64 nTimeInMicroSeconds)
{
    throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}


IUniformJournalSampling * CJournalVariable::ComputeUniformAverageSamples(const LibMCEnv_uint64 nStartTimeInMicroSeconds, const LibMCEnv_uint64 nEndTimeInMicroSeconds, const LibMCEnv_uint32 nNumberOfSamples, const LibMCEnv_double dMovingAverageDelta, const bool bClampInterval)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

IUniformJournalSampling* CJournalVariable::ComputeEquidistantSamples(const LibMCEnv_uint64 nStartTimeInMicroSeconds, const LibMCEnv_uint64 nEndTimeInMicroSeconds, const LibMCEnv_uint32 nNumberOfSamples)
{
    throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}



void CJournalVariable::ReceiveRawTimeStream(LibMCEnv_uint64 nTimeStreamEntriesBufferSize, LibMCEnv_uint64* pTimeStreamEntriesNeededCount, LibMCEnv::sTimeStreamEntry* pTimeStreamEntriesBuffer)
{
    std::vector<AMC::sJournalTimeStreamDoubleEntry> timeStream;

    m_pStateJournal->readDoubleTimeStream(m_sVariableName, m_Interval, timeStream);

    size_t nRawEntryCount = timeStream.size();

    if (pTimeStreamEntriesNeededCount != nullptr)
        *pTimeStreamEntriesNeededCount = nRawEntryCount;

    if (pTimeStreamEntriesBuffer != nullptr) 
    {
        if (nTimeStreamEntriesBufferSize < nRawEntryCount)
            throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_BUFFERTOOSMALL);

        auto pTargetEntry = pTimeStreamEntriesBuffer;

        for (auto& timeStreamIter : timeStream) {
            pTargetEntry->m_TimestampInMicroSeconds = timeStreamIter.m_nTimeStampInMicroSeconds;
            pTargetEntry->m_Value = timeStreamIter.m_dValue;
            pTargetEntry++;
        }
    }

}
