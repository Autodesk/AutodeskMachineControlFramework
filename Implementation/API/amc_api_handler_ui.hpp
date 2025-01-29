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


#ifndef __AMC_API_HANDLER_UI
#define __AMC_API_HANDLER_UI

#include "amc_api_handler.hpp"
#include "amc_logger.hpp"
#include "amc_api_response.hpp"

#include "amc_systemstate.hpp"

namespace AMC {

	enum class APIHandler_UIType {
		utUnknown = 0,
		utConfiguration = 1,
		utState = 2,
		utImage = 3,
		utContentItem = 4,
		utEvent = 5,
		utChart = 6,
		utMeshGeometry = 7,
		utMeshEdges = 8,
		utDownload = 9,
		utPointCloud = 10,
		utWidgetRequest = 11
	};

	class CAPIHandler_UI : public CAPIHandler {
	private:
		
		PSystemState m_pSystemState;

		APIHandler_UIType parseRequest(const std::string& sURI, const eAPIRequestType requestType, std::string & sParameterUUID, std::string & sAdditionalParameter);

		void handleConfigurationRequest(CJSONWriter& writer, PAPIAuth pAuth);
		void handleStateRequest(CJSONWriter& writer, PAPIAuth pAuth);
		void handleContentItemRequest(CJSONWriter& writer, const std::string& sParameterUUID, PAPIAuth pAuth, uint32_t nStateID);
		PAPIResponse handleImageRequest(const std::string & sParameterUUID, PAPIAuth pAuth);
		PAPIResponse handleChartRequest(const std::string& sParameterUUID, PAPIAuth pAuth);
		PAPIResponse handleDownloadRequest(const std::string& sParameterUUID, PAPIAuth pAuth);

		void handleEventRequest(CJSONWriter& writer, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth);
		void handleWidgetRequest(CJSONWriter& writer, const std::string & sWidgetUUID, const std::string& sRequestType, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth);

	public:

		CAPIHandler_UI(PSystemState pSystemState);

		virtual ~CAPIHandler_UI();
				
		virtual void checkAuthorizationMode(const std::string& sURI, const eAPIRequestType requestType, bool& bNeedsToBeAuthorized, bool& bCreateNewSession) override;

		virtual std::string getBaseURI () override;

		virtual bool expectsRawBody(const std::string& sURI, const eAPIRequestType requestType) override;

		virtual PAPIResponse handleRequest(const std::string& sURI, const eAPIRequestType requestType, CAPIFormFields & pFormFields, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth) override;

	};

	
}


#endif //__AMC_API_HANDLER_UI

