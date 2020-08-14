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


#ifndef __AMC_API_HANDLER_UPLOAD
#define __AMC_API_HANDLER_UPLOAD

#include "amc_api_handler.hpp"
#include "amc_logger.hpp"
#include "amc_api_response.hpp"

#include "amc_systemstate.hpp"

namespace AMC {

	enum APIHandler_UploadType {
		utUnknown = 0,
		utInitUpload = 1,
		utStreamUpload = 2,
		utFinishUpload = 3
	};

	class CAPIHandler_Upload : public CAPIHandler {
	private:
		
		PSystemState m_pSystemState;

		std::vector <APIFieldDetails> m_StreamUploadFields;

		APIHandler_UploadType parseRequest (const std::string& sURI, const eAPIRequestType requestType, std::string & uploadUUID);

		void handleInitUploadRequest(CJSONWriter & writer, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth);
		void handleFinishUploadRequest(CJSONWriter& writer, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth);
		void handleStreamUploadRequest(CJSONWriter& writer, APIFormFields pFormFields, const std::string & sStreamUUID, PAPIAuth pAuth);

	public:

		CAPIHandler_Upload(PSystemState pSystemState);

		virtual ~CAPIHandler_Upload();
				
		virtual std::string getBaseURI () override;

		virtual PAPIResponse handleRequest(const std::string& sURI, const eAPIRequestType requestType, APIFormFields pFormFields, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth) override;

		virtual uint32_t getFormDataFieldCount(const std::string& sURI, const eAPIRequestType requestType) override;

		virtual APIFieldDetails getFormDataFieldDetails(const std::string& sURI, const eAPIRequestType requestType, const uint32_t nFieldIndex) override;

		virtual bool expectsRawBody(const std::string& sURI, const eAPIRequestType requestType) override;

	};

	
}


#endif //__AMC_API_HANDLER_UPLOAD

