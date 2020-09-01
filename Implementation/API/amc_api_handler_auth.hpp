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

*/


#ifndef __AMC_API_HANDLER_AUTH
#define __AMC_API_HANDLER_AUTH

#include "amc_api_handler.hpp"
#include "amc_api_sessionhandler.hpp"
#include "amc_logger.hpp"
#include "amc_api_response.hpp"

namespace AMC {

	enum class APIHandler_AuthType {
		atUnknown = 0,
		atNewSession = 1,
		atAuthorize = 2,
	};

	class CAPIHandler_Auth : public CAPIHandler {
	private:
	
		std::string m_sInstallationSecret;
		std::string m_sGitHash;

		PAPISessionHandler m_pSessionHandler;
		
		APIHandler_AuthType parseRequest(const std::string& sURI, const eAPIRequestType requestType, std::string & sSessionUUID);

		void handleNewSessionRequest(const uint8_t* pBodyData, const size_t nBodyDataSize, CJSONWriter& writer, PAPIAuth pAuth);
		void handleAuthorizeRequest(const uint8_t* pBodyData, const size_t nBodyDataSize, CJSONWriter& writer, std::string & sSessionUUID, PAPIAuth pAuth);

	public:

		CAPIHandler_Auth(PAPISessionHandler pSessionHandler, const std::string & sInstallationSecret, const std::string& sGitHash);

		virtual ~CAPIHandler_Auth();
				
		virtual void checkAuthorizationMode(const std::string& sURI, const eAPIRequestType requestType, bool& bNeedsToBeAuthorized, bool& bCreateNewSession) override;
	
		virtual std::string getBaseURI () override;

		virtual bool expectsRawBody(const std::string& sURI, const eAPIRequestType requestType) override;

		virtual PAPIResponse handleRequest(const std::string& sURI, const eAPIRequestType requestType, CAPIFormFields & pFormFields, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth) override;

	};

	
}


#endif //__AMC_API_HANDLER_AUTH

