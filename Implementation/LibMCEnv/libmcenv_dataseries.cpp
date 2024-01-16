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


Abstract: This is a stub class definition of CDataSeries

*/

#include "libmcenv_dataseries.hpp"
#include "libmcenv_interfaceexception.hpp"
#include "libmcenv_journalvariable.hpp"

// Include custom headers here.


using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CDataSeries 
**************************************************************************************************************************/
CDataSeries::CDataSeries(AMC::PDataSeries pDataSeries)
	: m_pDataSeries (pDataSeries)
{
	if (pDataSeries.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
}

CDataSeries::~CDataSeries()
{

}


std::string CDataSeries::GetName()
{
	return m_pDataSeries->getName();
}

std::string CDataSeries::GetUUID()
{
	return m_pDataSeries->getUUID();
}

void CDataSeries::Clear()
{
	m_pDataSeries->increaseVersion();
	m_pDataSeries->clearData ();
}

bool CDataSeries::IsEmpty()
{
	return m_pDataSeries->isEmpty();
}

LibMCEnv_uint64 CDataSeries::GetMinimum()
{
	return m_pDataSeries->getMinimum();
}

LibMCEnv_uint64 CDataSeries::GetMaximum()
{
	return m_pDataSeries->getMaximum();
}

void CDataSeries::GetAllEntries(LibMCEnv_uint64 nEntryArrayBufferSize, LibMCEnv_uint64* pEntryArrayNeededCount, LibMCEnv::sTimeStreamEntry * pEntryArrayBuffer)
{
	auto& entries = m_pDataSeries->getEntries();
	uint64_t nEntryCount = entries.size();

	if (pEntryArrayNeededCount != nullptr)
		*pEntryArrayNeededCount = nEntryCount;

	if (pEntryArrayBuffer != nullptr) {

		if (nEntryArrayBufferSize < nEntryCount)
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_BUFFERTOOSMALL);

		if (nEntryCount > 0) {
			LibMCEnv::sTimeStreamEntry* pTarget = pEntryArrayBuffer;
			AMC::sDataSeriesEntry* pSource = entries.data();

			for (uint64_t nIndex = 0; nIndex < nEntryCount; nIndex++) {
				pTarget->m_TimestampInMicroSeconds = pSource->m_nTimeStampInMicroSeconds;
				pTarget->m_Value = pSource->m_dValue;
				pTarget++;
				pSource++;
			}
		}

	}

}

void CDataSeries::SetAllEntries(const LibMCEnv_uint64 nEntryArrayBufferSize, const LibMCEnv::sTimeStreamEntry * pEntryArrayBuffer)
{
	m_pDataSeries->increaseVersion();

	auto& entries = m_pDataSeries->getEntries();
	entries.clear();

	if (pEntryArrayBuffer != nullptr) {
		const LibMCEnv::sTimeStreamEntry* pSource = pEntryArrayBuffer;
		const LibMCEnv::sTimeStreamEntry* pPrev = nullptr;

		entries.resize(nEntryArrayBufferSize);
		if (nEntryArrayBufferSize > 0) {

			AMC::sDataSeriesEntry* pTarget = entries.data();

			for (uint64_t nIndex = 0; nIndex < nEntryArrayBufferSize; nIndex++) {

				if (pPrev != nullptr) {
					if (pPrev->m_TimestampInMicroSeconds >= pSource->m_TimestampInMicroSeconds)
						throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_DATASERIESTIMESTAMPSNOTINCREMENTING, "Data series time stamps not incrementing: " + m_pDataSeries->getName());
				}

				pPrev = pSource;

				pTarget->m_dValue = pSource->m_Value;
				pTarget->m_nTimeStampInMicroSeconds = pSource->m_TimestampInMicroSeconds;
				pTarget++;
				pSource++;
			}
		}

	}
}

void CDataSeries::SampleJournalVariable(IJournalVariable* pJournalVariable, const LibMCEnv_uint32 nNumberOfSamples, const LibMCEnv_double dMovingAverageDelta)
{
	m_pDataSeries->increaseVersion();
	auto& entries = m_pDataSeries->getEntries();
	entries.clear();

	if (pJournalVariable == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

	if (nNumberOfSamples < 2)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDNUMBEROFSAMPLES);

	auto pJournalVariableImpl = dynamic_cast<CJournalVariable*> (pJournalVariable);
	if (pJournalVariableImpl == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDCAST);

	uint64_t nStartTimeStamp = pJournalVariableImpl->GetStartTimeStamp();
	uint64_t nEndTimeStamp = pJournalVariableImpl->GetEndTimeStamp();
	uint64_t nDeltaTime = nEndTimeStamp - nStartTimeStamp;

	if (nDeltaTime > 0) {
		//auto pSampling = pJournalVariableImpl->ComputeUniformAverageSamples(nStartTimeStamp, nEndTimeStamp, nNumberOfSamples, dMovingAverageDelta, true);
		entries.resize(nNumberOfSamples);

		for (uint32_t nIndex = 0; nIndex < nNumberOfSamples; nIndex++) {

			double dValue = 0.0;

			uint64_t nMovingAverageDeltaInMicroSeconds = 0;
			if (dMovingAverageDelta > 0.0)
				nMovingAverageDeltaInMicroSeconds = (uint64_t) round(dMovingAverageDelta * 1000000);			
			if (nMovingAverageDeltaInMicroSeconds < 1)
				nMovingAverageDeltaInMicroSeconds = 1;

			uint64_t nTimeStampInMicroSeconds = nStartTimeStamp + (nIndex * nDeltaTime + (nDeltaTime / 2)) / nNumberOfSamples;
			uint64_t nIntervalStartTimeStampInMicroSeconds = 0;
			if (nMovingAverageDeltaInMicroSeconds < nTimeStampInMicroSeconds)
				nIntervalStartTimeStampInMicroSeconds = nTimeStampInMicroSeconds - nMovingAverageDeltaInMicroSeconds;

			int64_t nIntervalEndTimeStampInMicroSeconds = nTimeStampInMicroSeconds + nMovingAverageDeltaInMicroSeconds;

			dValue = pJournalVariableImpl->ComputeAverage(nIntervalStartTimeStampInMicroSeconds, nIntervalEndTimeStampInMicroSeconds, true);

			auto& entry = entries.at(nIndex);

			entry.m_nTimeStampInMicroSeconds = nTimeStampInMicroSeconds;
			entry.m_dValue = dValue;
		}

	}
}


LibMCEnv_uint32 CDataSeries::GetVersion()
{
	return m_pDataSeries->getVersion();
}

void CDataSeries::IncreaseVersion()
{
	m_pDataSeries->increaseVersion();

}
