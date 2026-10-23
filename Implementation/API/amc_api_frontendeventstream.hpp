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

Abstract: This is the class declaration of CAPIFrontendEventStream.
The server-sent event stream of one session. It pushes the unfiltered frontend
status as a snapshot event and every later change as a patch event.

*/


#ifndef __AMC_API_FRONTENDEVENTSTREAM
#define __AMC_API_FRONTENDEVENTSTREAM

#include "amc_jsoneventstreaminstance.hpp"
#include "amc_ui_frontendsnapshot.hpp"
#include "amc_ui_frontenddefinition.hpp"
#include "amc_api_auth.hpp"

#include <chrono>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <string>

// A connection checks for changes at this interval. It only builds the status if the frontend change
// counter moved, the stream was notified, or the last build is older than the maximum build interval.
#define AMC_API_FRONTENDEVENTSTREAM_BUILDINTERVAL_MS 100
// Values from outside the core do not bump the change counter; this bounds how late they arrive.
#define AMC_API_FRONTENDEVENTSTREAM_MAXBUILDINTERVAL_MS AMC_UI_FRONTEND_EPOCH_TIMESLOT_MS
#define AMC_API_FRONTENDEVENTSTREAM_SESSIONREFRESHINTERVAL_MS 30000
#define AMC_API_FRONTENDEVENTSTREAM_METRICSWINDOW_MS 5000

// Clients (browser tabs) of one session; the scope of a client without a connection is kept for a while, so that it can reconnect.
#define AMC_API_FRONTENDEVENTSTREAM_MAXCLIENTS 32
#define AMC_API_FRONTENDEVENTSTREAM_CLIENTEXPIRY_MS 600000

#define AMC_API_FRONTENDEVENTSTREAM_EVENT_SNAPSHOT "snapshot"
#define AMC_API_FRONTENDEVENTSTREAM_EVENT_PATCH "patch"

#define AMC_API_FRONTENDEVENTSTREAM_METRICSLABEL "stream"

namespace AMC {

	class CSystemState;
	class CAPISessionHandler;

	// Session metrics of the stream over one window, in the columns of the polling metrics:
	// the event count, the build time and size of each sent event, and the build time of all
	// status builds including those without a change.
	typedef struct _sFrontendEventStreamMetrics {
		std::chrono::steady_clock::time_point m_WindowStart;
		uint64_t m_nWindowStartMicros;
		uint32_t m_nEventCount;
		double m_dEventSumMS;
		double m_dEventMinMS;
		double m_dEventMaxMS;
		double m_dEventSumSqMS;
		uint64_t m_nPayloadSumBytes;
		uint64_t m_nPayloadMaxBytes;
		double m_dBuildSumMS;

		_sFrontendEventStreamMetrics();
	} sFrontendEventStreamMetrics;

	// One client (browser tab) of the session. A scoped client receives modules only for its active
	// pages and dialogs. Each client has its own revision log, since its snapshots depend on its scope.
	typedef struct _sFrontendEventStreamClient {
		bool m_bScoped;
		std::set<std::string> m_ActivePageNames;
		std::set<std::string> m_ActiveDialogNames;
		std::string m_sScope;
		std::shared_ptr<CUIFrontendRevisionLog> m_pRevisionLog;
		std::chrono::steady_clock::time_point m_LastUseTime;

		_sFrontendEventStreamClient();
	} sFrontendEventStreamClient;

	class CAPIFrontendEventStream : public CJSONEventStreamInstance {
	private:

		std::mutex m_Mutex;
		PAPIAuth m_pAuth;
		std::weak_ptr<CSystemState> m_pSystemState;
		std::weak_ptr<CAPISessionHandler> m_pSessionHandler;
		std::chrono::steady_clock::time_point m_LastSessionRefresh;

		// Keyed by client ID; the empty ID stands for clients that did not send one and receive all modules.
		// Owned by the stream rather than the session, since polling clients publish scoped snapshots into the session's log.
		std::map<std::string, sFrontendEventStreamClient> m_Clients;

		// Shared by all connections of the session; guarded by m_Mutex.
		bool m_bMetricsWindowStarted;
		sFrontendEventStreamMetrics m_Metrics;

		PAPIAuth getAuth();

		// Returns false if the session does not exist anymore.
		bool refreshSessionIfDue(const std::string& sSessionUUID);

		// Returns a copy of the client's entry and marks it as used. Creates an unscoped entry for an unknown client.
		sFrontendEventStreamClient useClient(const std::string& sClientID);

		sFrontendEventStreamClient& findOrCreateClientNoLock(const std::string& sClientID, std::chrono::steady_clock::time_point now);

		void removeExpiredClientsNoLock(std::chrono::steady_clock::time_point now);

		std::string buildNextEvent(sJSONEventStreamCursor& cursor, PAPIAuth pAuth, std::shared_ptr<CSystemState> pSystemState, const sUIFrontendBuildEpoch& epoch);

		void recordMetrics(double dBuildMS, size_t nEventBytes, const std::string& sSessionUUID, std::shared_ptr<CSystemState> pSystemState);

		static uint64_t createInitialRevision();

		static std::string joinNames(const std::set<std::string>& names);

	public:

		CAPIFrontendEventStream(const std::string& sUUID, PAPIAuth pAuth, std::shared_ptr<CSystemState> pSystemState, std::shared_ptr<CAPISessionHandler> pSessionHandler);

		virtual ~CAPIFrontendEventStream();

		void endStream() override;

		std::string waitForNextEvent(sJSONEventStreamCursor& cursor) override;

		// Restricts the modules a client receives to its active pages and dialogs, and wakes its connections.
		// sClientID must be a normalized UUID.
		void setClientScope(const std::string& sClientID, const std::set<std::string>& activePageNames, const std::set<std::string>& activeDialogNames);

	};

	typedef std::shared_ptr<CAPIFrontendEventStream> PAPIFrontendEventStream;

}


#endif //__AMC_API_FRONTENDEVENTSTREAM
