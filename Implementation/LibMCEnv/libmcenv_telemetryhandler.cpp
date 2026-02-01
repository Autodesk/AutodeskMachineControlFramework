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


Abstract: This is a stub class definition of CTelemetryHandler

*/

#include "libmcenv_telemetryhandler.hpp"
#include "libmcenv_interfaceexception.hpp"
#include "libmcenv_telemetryinterval.hpp"
#include "libmcenv_telemetryintervaliterator.hpp"
#include "libmcenv_telemetrychannelstatistics.hpp"
#include "libmcenv_datetime.hpp"

// Include custom headers here.
#include "common_utils.hpp"

#include <map>

using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CTelemetryHandler 
**************************************************************************************************************************/

CTelemetryHandler::CTelemetryHandler(LibMCData::PTelemetryReader pTelemetryReader, AMCCommon::PChrono pGlobalChrono, const std::string& sStartTimeUTC)
	: m_pTelemetryReader(pTelemetryReader), m_pGlobalChrono(pGlobalChrono), m_sStartTimeUTC(sStartTimeUTC)
{
	if (pTelemetryReader.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
	if (pGlobalChrono.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
}

CTelemetryHandler::~CTelemetryHandler()
{
}

std::string CTelemetryHandler::GetSessionUUID()
{
	return m_pTelemetryReader->GetSessionUUID();
}

IDateTime* CTelemetryHandler::GetStartTime()
{
	return CDateTime::makefromUTC(m_pTelemetryReader->GetStartTime());
}

IDateTime* CTelemetryHandler::GetEndTime()
{
	// End time is start time + lifetime
	auto pStartTime = CDateTime::makefromUTC(m_pTelemetryReader->GetStartTime());
	uint64_t nLifeTime = m_pTelemetryReader->GetLifeTimeInMicroseconds();
	
	// Add lifetime to start time
	pStartTime->ShiftByMicroseconds(nLifeTime);
	
	return pStartTime;
}

LibMCEnv_uint64 CTelemetryHandler::GetLifeTimeInMicroseconds()
{
	return m_pTelemetryReader->GetLifeTimeInMicroseconds();
}

LibMCEnv_uint32 CTelemetryHandler::GetChannelCount()
{
	return m_pTelemetryReader->GetChannelCount();
}

std::string CTelemetryHandler::GetChannelIdentifier(const LibMCEnv_uint32 nChannelIndex)
{
	std::string sUUID, sIdentifier, sDescription;
	LibMCData::eTelemetryChannelType eType;
	m_pTelemetryReader->GetChannelInformation(nChannelIndex, sUUID, eType, sIdentifier, sDescription);
	return sIdentifier;
}

bool CTelemetryHandler::HasChannel(const std::string& sIdentifier)
{
	uint32_t nIndex;
	return m_pTelemetryReader->FindChannelByIdentifier(sIdentifier, nIndex);
}

std::string CTelemetryHandler::GetChannelDescription(const std::string& sIdentifier)
{
	uint32_t nIndex;
	if (!m_pTelemetryReader->FindChannelByIdentifier(sIdentifier, nIndex))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_TELEMETRYCHANNELNOTFOUND, "Telemetry channel not found: " + sIdentifier);

	std::string sUUID, sIdent, sDescription;
	LibMCData::eTelemetryChannelType eType;
	m_pTelemetryReader->GetChannelInformation(nIndex, sUUID, eType, sIdent, sDescription);
	return sDescription;
}

ITelemetryIntervalIterator* CTelemetryHandler::QueryIntervalsFromTimeDelta(const std::string& sChannelIdentifier, const LibMCEnv_uint64 nTimeDeltaInMicroseconds)
{
	uint64_t nLifeTime = m_pTelemetryReader->GetLifeTimeInMicroseconds();
	uint64_t nStartTime = 0;
	if (nLifeTime > nTimeDeltaInMicroseconds)
		nStartTime = nLifeTime - nTimeDeltaInMicroseconds;

	return QueryIntervalsFromTimeRange(sChannelIdentifier, nStartTime, nLifeTime);
}

ITelemetryIntervalIterator* CTelemetryHandler::QueryIntervalsFromTimeRange(const std::string& sChannelIdentifier, const LibMCEnv_uint64 nStartTimeInMicroseconds, const LibMCEnv_uint64 nEndTimeInMicroseconds)
{
	auto pIterator = std::make_unique<CTelemetryIntervalIterator>();

	// Find channel index
	uint32_t nChannelIndex = 0xFFFFFFFF;
	if (!sChannelIdentifier.empty()) {
		if (!m_pTelemetryReader->FindChannelByIdentifier(sChannelIdentifier, nChannelIndex))
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_TELEMETRYCHANNELNOTFOUND, "Telemetry channel not found: " + sChannelIdentifier);
	}

	// Query intervals
	std::vector<LibMCData::sTelemetryIntervalData> intervals;
	m_pTelemetryReader->QueryIntervals(nStartTimeInMicroseconds, nEndTimeInMicroseconds, nChannelIndex, intervals);

	// Build channel index to identifier map
	std::map<uint32_t, std::string> channelIdentifiers;
	uint32_t nChannelCount = m_pTelemetryReader->GetChannelCount();
	for (uint32_t i = 0; i < nChannelCount; i++) {
		std::string sUUID, sIdent, sDesc;
		LibMCData::eTelemetryChannelType eType;
		m_pTelemetryReader->GetChannelInformation(i, sUUID, eType, sIdent, sDesc);
		channelIdentifiers[i + 1] = sIdent;  // Channel indices are 1-based in data
	}

	// Convert to TelemetryInterval objects
	for (auto& intervalData : intervals) {
		std::string sIdent;
		auto iIter = channelIdentifiers.find(intervalData.m_ChannelIndex);
		if (iIter != channelIdentifiers.end())
			sIdent = iIter->second;

		auto pInterval = std::make_shared<CTelemetryInterval>(
			intervalData.m_MarkerID,
			sIdent,
			intervalData.m_StartTimeStamp,
			intervalData.m_EndTimeStamp,
			intervalData.m_ContextData
		);
		pIterator->AddInterval(pInterval);
	}

	return pIterator.release();
}

ITelemetryChannelStatistics* CTelemetryHandler::GetChannelStatistics(const std::string& sChannelIdentifier, const LibMCEnv_uint64 nStartTimeInMicroseconds, const LibMCEnv_uint64 nEndTimeInMicroseconds)
{
	// Find channel index
	uint32_t nChannelIndex;
	if (!m_pTelemetryReader->FindChannelByIdentifier(sChannelIdentifier, nChannelIndex))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_TELEMETRYCHANNELNOTFOUND, "Telemetry channel not found: " + sChannelIdentifier);

	// Determine time range
	uint64_t nStart = nStartTimeInMicroseconds;
	uint64_t nEnd = nEndTimeInMicroseconds;
	if (nEnd == 0)
		nEnd = m_pTelemetryReader->GetLifeTimeInMicroseconds();

	// Get statistics
	uint64_t nIntervalCount, nInstantMarkerCount, nTotalDuration, nMinDuration, nMaxDuration, nAvgDuration;
	m_pTelemetryReader->GetChannelStatistics(nChannelIndex, nStart, nEnd, nIntervalCount, nInstantMarkerCount, nTotalDuration, nMinDuration, nMaxDuration, nAvgDuration);

	return new CTelemetryChannelStatistics(sChannelIdentifier, nIntervalCount, nInstantMarkerCount, nTotalDuration, nMinDuration, nMaxDuration, nAvgDuration);
}
