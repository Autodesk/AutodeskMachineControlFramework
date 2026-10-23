/*++

Copyright (C) 2025 Autodesk Inc.

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

*/


#ifndef __AMC_API_HANDLER_FRONTEND
#define __AMC_API_HANDLER_FRONTEND

#include "amc_api_handler.hpp"
#include "amc_logger.hpp"
#include "amc_api_response.hpp"

#include "amc_systemstate.hpp"
#include "amc_api_frontendeventstream.hpp"

#define AMC_API_FRONTEND_STREAMTICKET_LIFETIME_SECONDS 30
// Limits for the comma separated page and dialog names of a stream scope.
#define AMC_API_FRONTEND_STREAMSCOPE_MAXLENGTH 16384
#define AMC_API_FRONTEND_STREAMSCOPE_MAXNAMES 256

namespace AMC {

	class CAPISessionHandler;
	typedef std::shared_ptr<CAPISessionHandler> PAPISessionHandler;

	enum class APIHandler_FrontendType {
		ftUnknown = 0,
		ftStatus = 1,
		ftPageStatus = 2,
		ftPageSubscription = 3,
		ftModuleStatus = 4,
		ftModuleSubscription = 5,
		ftSubscriptionUpdate = 6,
		ftSubscriptionKeepAlive = 7,
		ftSubscriptionReset = 8,
		ftTriggerEvent = 2,
		ftMetrics = 9,
		ftParameterListDefinition = 10,
		ftParameterListValues = 11,
		ftStreamTicket = 12,
		ftStreamScope = 13,
	};

	class CAPIHandler_Frontend : public CAPIHandler {
	private:
		
		PSystemState m_pSystemState;
		PAPISessionHandler m_pSessionHandler;

		APIHandler_FrontendType parseRequest(const std::string& sURI, const eAPIRequestType requestType, std::string & sParameterUUID, std::string & sAdditionalParameter);

		// Clients that pass the "pages" query parameter only receive modules for the listed pages and the dialogs in "dialogs".
		// Clients that pass "since" (a revision of the same pages and dialogs) receive only the changed attributes
		// if that revision is still in the session's revision log, and the full status otherwise.
		std::string handleStatusRequest(CAPIFormFields& pFormFields, PAPIAuth pAuth);

		// Optional query parameters: "definitionhash" and "valueshash" (last known hashes) and "rows" (row indices/ranges, e.g. "0-24,30").
		void handleParameterListRequest(CJSONWriter& writer, const std::string& sItemUUID, bool bValuesOnly, CAPIFormFields& pFormFields, PAPIAuth pAuth);

		// Records an aggregated frontend reactivity metrics window pushed by the client.
		// The session UUID is stamped server-side from the authenticated session, never trusted from the body.
		void handleMetricsRequest(CJSONWriter& writer, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth);

		PAPIFrontendEventStream getOrCreateEventStream(PAPIAuth pAuth);

		// Reads the optional "client", "pages" and "dialogs" members of a stream request body and applies the scope
		// to the event stream. Returns the normalized client ID, or an empty string if the body carries none.
		std::string applyStreamScope(PAPIFrontendEventStream pEventStream, const uint8_t* pBodyData, const size_t nBodyDataSize, bool bClientIsMandatory);

		// Issues a short-lived single-use ticket for the session's event stream, to be opened as /stream/{ticket}.
		// The optional body {client, pages, dialogs} identifies the browser tab and its active pages and dialogs.
		void handleStreamTicketRequest(CJSONWriter& writer, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth);

		// Updates the active pages and dialogs of a client of the session's event stream. Body: {client, pages, dialogs}.
		void handleStreamScopeRequest(CJSONWriter& writer, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth);

	public:

		CAPIHandler_Frontend(PSystemState pSystemState, PAPISessionHandler pSessionHandler);

		virtual ~CAPIHandler_Frontend();
				
		virtual void checkAuthorizationMode(const std::string& sURI, const eAPIRequestType requestType, bool& bNeedsToBeAuthorized, bool& bCreateNewSession) override;

		virtual std::string getBaseURI () override;

		virtual bool expectsRawBody(const std::string& sURI, const eAPIRequestType requestType) override;

		virtual PAPIResponse handleRequest(const std::string& sURI, const eAPIRequestType requestType, CAPIFormFields & pFormFields, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth) override;

	};

	
}


#endif //__AMC_API_HANDLER_FRONTEND

