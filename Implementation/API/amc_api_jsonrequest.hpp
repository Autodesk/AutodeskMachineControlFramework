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


#ifndef __AMC_API_JSONREQUEST
#define __AMC_API_JSONREQUEST

#include <memory>
#include <string>
#include <map>

namespace AMC {

	class CAPIJSONRequest;
	typedef std::shared_ptr<CAPIJSONRequest> PAPIJSONRequest;
	
	class CAPIJSONImpl;

	class CAPIJSONRequest {
	private:

		std::unique_ptr<CAPIJSONImpl> m_pImpl;
		static CAPIJSONImpl* __construct(const uint8_t* pBodyData, const size_t nBodySize);
			
	public:

		CAPIJSONRequest (const uint8_t * pBodyData, const size_t nBodySize);

		virtual ~CAPIJSONRequest();
							
		bool hasValue(const std::string& sKeyName);
		std::string getRawString(const std::string& sKeyName, uint32_t nErrorCode);

		std::string getUUID (const std::string & sKeyName, uint32_t nErrorCode);
		std::string getNameString(const std::string& sKeyName, uint32_t nErrorCode);
		uint64_t getUint64 (const std::string& sKeyName, const uint64_t nMinValue, const uint64_t nMaxValue, uint32_t nErrorCode);
		std::string getSHA256(const std::string& sKeyName, uint32_t nErrorCode);
	};

	
}


#endif //__AMC_API_JSONREQUEST

