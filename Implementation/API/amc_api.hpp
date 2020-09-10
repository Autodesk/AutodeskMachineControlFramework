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


#ifndef __AMC_API
#define __AMC_API

#include "header_protection.hpp"
#include "amc_api_types.hpp"


namespace AMC {

	amcDeclareDependingClass(CAPI, PAPI);
	amcDeclareDependingClass(CAPIAuth, PAPIAuth);
	amcDeclareDependingClass(CAPIHandler, PAPIHandler);
	amcDeclareDependingClass(CAPIResponse, PAPIResponse);
	amcDeclareDependingClass(CAPIFormFields, PAPIFormFields);	
	amcDeclareDependingClass(CAPIFieldDetails, PAPIFieldDetails);
	amcDeclareDependingClass(CAPISessionHandler, PAPISessionHandler);


	class CAPI {
	private:

		std::list<PAPIHandler> m_ApiHandlers;

		PAPISessionHandler m_pSessionHandler;

		PAPIResponse makeError (uint32_t nHTTPError, int32_t errorCode, const std::string & sErrorString);

		PAPIHandler getURIMatch (const std::string& sURI);
			
	public:

		CAPI();

		virtual ~CAPI();
								
		void checkAuthorizationMode (const std::string& sURI, const eAPIRequestType requestType, bool & bNeedsToBeAuthorized, bool & bCreateNewSession);

		bool expectsRawBody(const std::string& sURI, const eAPIRequestType requestType);

		PAPIResponse handleRequest(const std::string& sURI, const eAPIRequestType requestType, const uint8_t * pData, uint64_t nCount, CAPIFormFields & pFormFields, PAPIAuth pAuth);

		uint32_t getFormDataFieldCount(const std::string& sURI, const eAPIRequestType requestType);

		CAPIFieldDetails getFormDataFieldDetails(const std::string& sURI, const eAPIRequestType requestType, const uint32_t nFieldIndex);

		void registerHandler (PAPIHandler pAPIHandler);

		PAPISessionHandler getSessionHandler();

		static std::string removeLeadingSlashFromURI(const std::string& sURI);

		static eAPIRequestType getRequestTypeFromString(const std::string & sRequestType);

	};

	
}


#endif //__AMC_API

