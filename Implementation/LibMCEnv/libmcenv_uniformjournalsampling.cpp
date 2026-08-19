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


Abstract: This is a stub class definition of CUniformJournalSampling

*/

#include "libmcenv_uniformjournalsampling.hpp"
#include "libmcenv_interfaceexception.hpp"

// Include custom headers here.


using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CUniformJournalSampling 
**************************************************************************************************************************/

CUniformJournalSampling::CUniformJournalSampling(const std::string& sVariableName, LibMCEnv_uint64 nStartTimeStamp, LibMCEnv_uint64 nEndTimeStamp, const std::vector<AMC::sJournalEnvelopeSample>& samples)
	: m_sVariableName(sVariableName), m_nStartTimeStamp(nStartTimeStamp), m_nEndTimeStamp(nEndTimeStamp), m_Samples(samples)
{
}

CUniformJournalSampling::~CUniformJournalSampling()
{
}

std::string CUniformJournalSampling::GetVariableName()
{
	return m_sVariableName;
}

LibMCEnv_uint32 CUniformJournalSampling::GetNumberOfSamples()
{
	return (LibMCEnv_uint32)m_Samples.size();
}

LibMCEnv_uint64 CUniformJournalSampling::GetStartTimeStamp()
{
	return m_nStartTimeStamp;
}

LibMCEnv_uint64 CUniformJournalSampling::GetEndTimeStamp()
{
	return m_nEndTimeStamp;
}

void CUniformJournalSampling::GetSample(const LibMCEnv_uint32 nIndex, LibMCEnv_uint64 & nTimeStamp, LibMCEnv_double & dValue)
{
	if (nIndex >= m_Samples.size())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

	auto& sample = m_Samples.at(nIndex);
	nTimeStamp = sample.m_nTimeStampInMicroSeconds;
	dValue = sample.m_dAverageValue;
}

void CUniformJournalSampling::GetAllSamples(LibMCEnv_uint64 nSamplesBufferSize, LibMCEnv_uint64* pSamplesNeededCount, LibMCEnv::sTimeStreamEntry* pSamplesBuffer)
{
	uint64_t nCount = (uint64_t)m_Samples.size();

	if (pSamplesNeededCount != nullptr)
		*pSamplesNeededCount = nCount;

	if (pSamplesBuffer != nullptr) {
		if (nSamplesBufferSize < nCount)
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_BUFFERTOOSMALL);

		LibMCEnv::sTimeStreamEntry* pTarget = pSamplesBuffer;
		for (auto& sample : m_Samples) {
			pTarget->m_TimestampInMicroSeconds = sample.m_nTimeStampInMicroSeconds;
			pTarget->m_Value = sample.m_dAverageValue;
			pTarget++;
		}
	}
}

void CUniformJournalSampling::GetAllSamplesWithBounds(LibMCEnv_uint64 nSamplesBufferSize, LibMCEnv_uint64* pSamplesNeededCount, LibMCEnv::sTimeStreamEnvelopeEntry* pSamplesBuffer)
{
	uint64_t nCount = (uint64_t)m_Samples.size();

	if (pSamplesNeededCount != nullptr)
		*pSamplesNeededCount = nCount;

	if (pSamplesBuffer != nullptr) {
		if (nSamplesBufferSize < nCount)
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_BUFFERTOOSMALL);

		LibMCEnv::sTimeStreamEnvelopeEntry* pTarget = pSamplesBuffer;
		for (auto& sample : m_Samples) {
			pTarget->m_TimestampInMicroSeconds = sample.m_nTimeStampInMicroSeconds;
			pTarget->m_MinValue = sample.m_dMinValue;
			pTarget->m_MaxValue = sample.m_dMaxValue;
			pTarget->m_AverageValue = sample.m_dAverageValue;
			pTarget->m_LastValue = sample.m_dLastValue;
			pTarget++;
		}
	}
}


