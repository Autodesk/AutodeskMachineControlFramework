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


Abstract: This is a stub class definition of CTelemetryChannelStatistics

*/

#include "libmcenv_telemetrychannelstatistics.hpp"
#include "libmcenv_interfaceexception.hpp"

// Include custom headers here.

using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CTelemetryChannelStatistics 
**************************************************************************************************************************/

CTelemetryChannelStatistics::CTelemetryChannelStatistics(const std::string& sChannelIdentifier, uint64_t nIntervalCount, uint64_t nInstantMarkerCount, uint64_t nTotalDuration, uint64_t nMinDuration, uint64_t nMaxDuration, uint64_t nAvgDuration)
	: m_sChannelIdentifier(sChannelIdentifier),
	m_nIntervalCount(nIntervalCount),
	m_nInstantMarkerCount(nInstantMarkerCount),
	m_nTotalDuration(nTotalDuration),
	m_nMinDuration(nMinDuration),
	m_nMaxDuration(nMaxDuration),
	m_nAvgDuration(nAvgDuration)
{
}

CTelemetryChannelStatistics::~CTelemetryChannelStatistics()
{
}

std::string CTelemetryChannelStatistics::GetChannelIdentifier()
{
	return m_sChannelIdentifier;
}

LibMCEnv_uint64 CTelemetryChannelStatistics::GetIntervalCount()
{
	return m_nIntervalCount;
}

LibMCEnv_uint64 CTelemetryChannelStatistics::GetInstantMarkerCount()
{
	return m_nInstantMarkerCount;
}

LibMCEnv_uint64 CTelemetryChannelStatistics::GetTotalDuration()
{
	return m_nTotalDuration;
}

LibMCEnv_uint64 CTelemetryChannelStatistics::GetMinDuration()
{
	return m_nMinDuration;
}

LibMCEnv_uint64 CTelemetryChannelStatistics::GetMaxDuration()
{
	return m_nMaxDuration;
}

LibMCEnv_uint64 CTelemetryChannelStatistics::GetAverageDuration()
{
	return m_nAvgDuration;
}
