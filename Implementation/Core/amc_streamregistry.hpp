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

Abstract: This is the class declaration of CStreamRegistry.
A thread-safe registry that maps stream UUIDs to CStreamInstance objects.
Supports multiple stream types (video, JSON events, etc.).

*/


#ifndef __AMC_STREAMREGISTRY
#define __AMC_STREAMREGISTRY

#include <memory>
#include <mutex>
#include <map>
#include <string>

#include "amc_streaminstance.hpp"
#include "amc_videostreaminstance.hpp"
#include "amc_jsoneventstreaminstance.hpp"

#define AMC_STREAMREGISTRY_MAXSTREAMTICKETS 1024

namespace AMC {

	class CStreamRegistry {
	private:

		struct sStreamTicket {
			std::string m_sStreamUUID;
			std::string m_sClientID;
			uint64_t m_nExpiryTimestampInMicroseconds;
		};

		std::map<std::string, PStreamInstance> m_StreamMap;
		std::map<std::string, sStreamTicket> m_StreamTickets;
		mutable std::mutex m_Mutex;

		void removeExpiredStreamTicketsNoLock(uint64_t nTimestampInMicroseconds);

	public:
		CStreamRegistry();
		virtual ~CStreamRegistry();

		// Registers an existing stream instance.
		void registerStream(PStreamInstance pStreamInstance);

		// Creates a new video stream and registers it.
		PVideoStreamInstance createVideoStream(
			uint32_t nWidth,
			uint32_t nHeight,
			uint32_t nDesiredFrameDurationInMicroseconds,
			uint32_t nPauseToleranceInMicroseconds,
			uint32_t nFrameCacheDurationInMicroseconds
		);

		// Finds a stream by UUID. Returns nullptr if not found.
		PStreamInstance findStream(const std::string& sUUID) const;

		// Finds a video stream by UUID. Returns nullptr if not found or not a video stream.
		PVideoStreamInstance findVideoStream(const std::string& sUUID) const;

		// Removes a stream by UUID.
		void removeStream(const std::string& sUUID);

		// Returns true if a stream with the given UUID exists.
		bool hasStream(const std::string& sUUID) const;

		// Creates a single-use ticket (a UUID) that grants access to a registered stream until it expires.
		// sClientID identifies the consumer within the stream and is handed back on redemption; it may be empty.
		std::string createStreamTicket(const std::string& sStreamUUID, const std::string& sClientID, uint64_t nTimestampInMicroseconds, uint64_t nLifetimeInMicroseconds);

		// Consumes a ticket. Returns nullptr if the ticket is unknown or expired, or its stream is gone or inactive.
		// Otherwise sClientID receives the client ID the ticket was created for.
		PStreamInstance redeemStreamTicket(const std::string& sTicket, uint64_t nTimestampInMicroseconds, std::string& sClientID);

		// Wakes all connections of all JSON event streams.
		void notifyJSONEventStreams();

		// Unregisters JSON event streams that have ended.
		void removeEndedJSONEventStreams();
	};

	typedef std::shared_ptr<CStreamRegistry> PStreamRegistry;

}


#endif //__AMC_STREAMREGISTRY
