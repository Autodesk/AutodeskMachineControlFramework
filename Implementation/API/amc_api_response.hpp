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


#ifndef __AMC_API_RESPONSE
#define __AMC_API_RESPONSE

#include <vector>
#include <memory>
#include <string>



namespace AMC {


	class CAPIResponse {
	protected:

		std::vector<uint8_t> m_StreamData;

		std::string m_sContentType;
			
	public:

		CAPIResponse (const std::string & sContentType);
		
		size_t getStreamSize () const;
		
		const uint8_t * getStreamData () const;
		
		std::string getContentType () const;

	};


	class CAPIStringResponse : public CAPIResponse {
	private:
			
	public:

		CAPIStringResponse(const std::string & sContentType, const std::string & sStringValue);

	};


	class CAPIFixedBufferResponse : public CAPIResponse {
	private:

	public:

		CAPIFixedBufferResponse(const std::string& sContentType);

		std::vector<uint8_t>& getBuffer();

	};


	typedef std::shared_ptr<CAPIResponse> PAPIResponse;

	
}


#endif //__AMC_API_RESPONSE

