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

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL AUTODESK INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Abstract: This is the class definition of CStreamRegistry.

*/

#include "amc_streamregistry.hpp"
#include "common_utils.hpp"
#include "libmc_exceptiontypes.hpp"

#include <vector>

using namespace AMC;

CStreamRegistry::CStreamRegistry()
{
}

CStreamRegistry::~CStreamRegistry()
{
}

void CStreamRegistry::registerStream(PStreamInstance pStreamInstance)
{
	if (pStreamInstance.get() == nullptr)
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDPARAM, "Stream instance is null.");

	std::string sUUID = pStreamInstance->getUUID();

	std::lock_guard<std::mutex> lock(m_Mutex);

	if (m_StreamMap.find(sUUID) != m_StreamMap.end())
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDPARAM, "Stream with UUID " + sUUID + " already registered.");

	m_StreamMap.insert(std::make_pair(sUUID, pStreamInstance));
}

PVideoStreamInstance CStreamRegistry::createVideoStream(
	uint32_t nWidth,
	uint32_t nHeight,
	uint32_t nDesiredFrameDurationInMicroseconds,
	uint32_t nPauseToleranceInMicroseconds,
	uint32_t nFrameCacheDurationInMicroseconds)
{
	std::string sUUID = AMCCommon::CUtils::createUUID();

	auto pInstance = std::make_shared<CVideoStreamInstance>(
		sUUID,
		nWidth,
		nHeight,
		nDesiredFrameDurationInMicroseconds,
		nPauseToleranceInMicroseconds,
		nFrameCacheDurationInMicroseconds
	);

	std::lock_guard<std::mutex> lock(m_Mutex);
	m_StreamMap.insert(std::make_pair(sUUID, pInstance));

	return pInstance;
}

PStreamInstance CStreamRegistry::findStream(const std::string& sUUID) const
{
	std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);

	std::lock_guard<std::mutex> lock(m_Mutex);
	auto iter = m_StreamMap.find(sNormalizedUUID);
	if (iter != m_StreamMap.end())
		return iter->second;

	return nullptr;
}

PVideoStreamInstance CStreamRegistry::findVideoStream(const std::string& sUUID) const
{
	auto pStream = findStream(sUUID);
	if (pStream.get() == nullptr)
		return nullptr;

	return std::dynamic_pointer_cast<CVideoStreamInstance>(pStream);
}

void CStreamRegistry::removeStream(const std::string& sUUID)
{
	std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);

	std::lock_guard<std::mutex> lock(m_Mutex);
	m_StreamMap.erase(sNormalizedUUID);
}

bool CStreamRegistry::hasStream(const std::string& sUUID) const
{
	std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);

	std::lock_guard<std::mutex> lock(m_Mutex);
	return m_StreamMap.find(sNormalizedUUID) != m_StreamMap.end();
}

void CStreamRegistry::removeExpiredStreamTicketsNoLock(uint64_t nTimestampInMicroseconds)
{
	for (auto iter = m_StreamTickets.begin(); iter != m_StreamTickets.end(); ) {
		if (iter->second.m_nExpiryTimestampInMicroseconds <= nTimestampInMicroseconds)
			iter = m_StreamTickets.erase(iter);
		else
			++iter;
	}
}

std::string CStreamRegistry::createStreamTicket(const std::string& sStreamUUID, const std::string& sClientID, uint64_t nTimestampInMicroseconds, uint64_t nLifetimeInMicroseconds)
{
	std::string sNormalizedStreamUUID = AMCCommon::CUtils::normalizeUUIDString(sStreamUUID);

	std::lock_guard<std::mutex> lock(m_Mutex);

	if (m_StreamMap.find(sNormalizedStreamUUID) == m_StreamMap.end())
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDPARAM, "Stream with UUID " + sNormalizedStreamUUID + " is not registered.");

	removeExpiredStreamTicketsNoLock(nTimestampInMicroseconds);
	if (m_StreamTickets.size() >= AMC_STREAMREGISTRY_MAXSTREAMTICKETS)
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDPARAM, "Too many open stream tickets.");

	std::string sTicket = AMCCommon::CUtils::createUUID();

	sStreamTicket ticket;
	ticket.m_sStreamUUID = sNormalizedStreamUUID;
	ticket.m_sClientID = sClientID;
	ticket.m_nExpiryTimestampInMicroseconds = nTimestampInMicroseconds + nLifetimeInMicroseconds;
	m_StreamTickets.insert(std::make_pair(sTicket, ticket));

	return sTicket;
}

PStreamInstance CStreamRegistry::redeemStreamTicket(const std::string& sTicket, uint64_t nTimestampInMicroseconds, std::string& sClientID)
{
	sClientID.clear();
	if (!AMCCommon::CUtils::stringIsUUIDString(sTicket))
		return nullptr;

	std::string sNormalizedTicket = AMCCommon::CUtils::normalizeUUIDString(sTicket);

	std::lock_guard<std::mutex> lock(m_Mutex);

	auto iTicketIter = m_StreamTickets.find(sNormalizedTicket);
	if (iTicketIter == m_StreamTickets.end())
		return nullptr;

	sStreamTicket ticket = iTicketIter->second;
	m_StreamTickets.erase(iTicketIter);

	if (ticket.m_nExpiryTimestampInMicroseconds <= nTimestampInMicroseconds)
		return nullptr;

	auto iStreamIter = m_StreamMap.find(ticket.m_sStreamUUID);
	if (iStreamIter == m_StreamMap.end())
		return nullptr;

	if (!iStreamIter->second->isActive())
		return nullptr;

	sClientID = ticket.m_sClientID;
	return iStreamIter->second;
}

void CStreamRegistry::notifyJSONEventStreams()
{
	std::vector<PJSONEventStreamInstance> jsonEventStreams;
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		for (auto& iter : m_StreamMap) {
			if (iter.second->getStreamType() == eStreamType::JSONEventStream) {
				auto pJSONEventStream = std::dynamic_pointer_cast<CJSONEventStreamInstance>(iter.second);
				if (pJSONEventStream.get() != nullptr)
					jsonEventStreams.push_back(pJSONEventStream);
			}
		}
	}

	for (auto& pJSONEventStream : jsonEventStreams)
		pJSONEventStream->notifyChange();
}

void CStreamRegistry::removeEndedJSONEventStreams()
{
	std::lock_guard<std::mutex> lock(m_Mutex);
	for (auto iter = m_StreamMap.begin(); iter != m_StreamMap.end(); ) {
		if ((iter->second->getStreamType() == eStreamType::JSONEventStream) && (!iter->second->isActive()))
			iter = m_StreamMap.erase(iter);
		else
			++iter;
	}
}
