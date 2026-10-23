/*++

Copyright (C) 2026 Autodesk Inc.

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

Abstract: This is the class declaration of CJSONEventStreamInstance.
An abstract server-sent event stream. Each connection keeps its own cursor and
pulls fully framed SSE events; a change signal wakes waiting connections early.

*/


#ifndef __AMC_JSONEVENTSTREAMINSTANCE
#define __AMC_JSONEVENTSTREAMINSTANCE

#include "amc_streaminstance.hpp"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>

namespace AMC {

	// Per-connection read position in a JSON event stream.
	struct sJSONEventStreamCursor {
		// ID of the last event the consumer has received. 0 means a full snapshot is required.
		uint64_t m_nLastEventID;
		// Change counter the consumer has already reacted to.
		uint64_t m_nChangeCounter;
		bool m_bStarted;
		// The client behind the connection, as passed with its stream ticket, or empty.
		std::string m_sClientID;
		// Data change counter the last event was built from, and when it was built.
		uint64_t m_nDataChangeCounter;
		std::chrono::steady_clock::time_point m_LastBuildTime;

		sJSONEventStreamCursor();
	};

	class CJSONEventStreamInstance : public CStreamInstance {
	private:
		std::mutex m_SignalMutex;
		std::condition_variable m_SignalCondition;
		uint64_t m_nChangeCounter;
		std::atomic<bool> m_bEnded;

	protected:

		// Blocks until the change counter differs from nKnownChangeCounter, the stream ends or the timeout elapses.
		// Returns the current change counter.
		uint64_t waitForChange(uint64_t nKnownChangeCounter, uint32_t nTimeoutInMilliseconds);

		uint64_t getChangeCounter();

	public:

		CJSONEventStreamInstance(const std::string& sUUID);

		virtual ~CJSONEventStreamInstance();

		eStreamType getStreamType() const override;

		// Returns false once the stream has ended.
		bool isActive() const override;

		// Wakes all connections that are waiting for a change.
		void notifyChange();

		// Ends the stream permanently and wakes all waiting connections.
		virtual void endStream();

		// Returns the next fully framed SSE event for the consumer and advances its cursor.
		// Returns an empty string if there is nothing to send yet. Must return within a bounded time,
		// so that the caller can send heartbeats and detect closed connections.
		virtual std::string waitForNextEvent(sJSONEventStreamCursor& cursor) = 0;

		// Frames one SSE event. Line breaks in the data are split into several data lines.
		static std::string formatEvent(const std::string& sEventName, uint64_t nEventID, const std::string& sData);

		static std::string formatComment(const std::string& sComment);

	};

	typedef std::shared_ptr<CJSONEventStreamInstance> PJSONEventStreamInstance;

}


#endif //__AMC_JSONEVENTSTREAMINSTANCE
