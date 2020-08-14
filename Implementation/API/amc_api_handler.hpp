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


#ifndef __AMC_API_HANDLER
#define __AMC_API_HANDLER

#include "amc_jsonwriter.hpp"
#include "amc_api_constants.hpp"
#include "amc_api_response.hpp"
#include "amc_api_auth.hpp"

#include <memory>
#include <map>
#include <vector>
#include <string>

namespace AMC {



	class CAPIHandler;
	typedef std::shared_ptr<CAPIHandler> PAPIHandler;

	class APIFieldDetails {
	public:
		std::string m_sFieldName;
		bool m_bIsFileData;
		bool m_bIsMandatory;

		APIFieldDetails();
		APIFieldDetails(const std::string & sFieldName, const bool bIsFileData, const bool bIsMandatory);
	};

	class APIFormFields {
	private:

		std::map<std::string, std::shared_ptr <std::vector<uint8_t>>> m_FileData;
		std::map<std::string, std::string> m_StringData;

	public:
		APIFormFields();

		void addDataField(const std::string& sName, std::shared_ptr<std::vector<uint8_t>> pData);
		bool hasDataField(const std::string& sName);
		std::shared_ptr<std::vector<uint8_t>> getDataField(const std::string& sName);

		void addStringField(const std::string& sName, const std::string& sValue);
		bool hasStringField(const std::string& sName);
		std::string getStringField(const std::string& sName);
		uint64_t getUint64Field(const std::string& sName);


	};

	class CAPIHandler {
	private:

	protected:
			
	public:

		CAPIHandler();

		virtual ~CAPIHandler();
				
		virtual std::string getBaseURI () = 0;

		virtual PAPIResponse handleRequest(const std::string& sURI, const eAPIRequestType requestType, APIFormFields pFormFields, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth) = 0;

		virtual bool expectsRawBody(const std::string& sURI, const eAPIRequestType requestType);

		virtual uint32_t getFormDataFieldCount(const std::string& sURI, const eAPIRequestType requestType);

		virtual APIFieldDetails getFormDataFieldDetails(const std::string& sURI, const eAPIRequestType requestType, const uint32_t nFieldIndex);

		virtual void writeJSONHeader(CJSONWriter& writer, const std::string& sProtocol);

	};

	
}


#endif //__AMC_API_HANDLER

