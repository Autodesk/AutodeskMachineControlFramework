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


#ifndef __AMC_API_SESSION
#define __AMC_API_SESSION

#ifndef __AMCIMPL_API_SESSION
#error this header is protected and should only be included in the corresponding implementation CPP files.
#endif

#include "header_protection.hpp"
#include "amc_api_types.hpp"

#include <mutex>

namespace AMC {

	amcDeclareDependingClass(CAPISession, PAPISession);
	
	class CAPISession {
	private:
	
		std::mutex m_Mutex;
	
		std::string m_sUUID;
		std::string m_sKey;
		std::string m_sUserName;
		std::string m_sHashedPassword;
		std::string m_sToken;
		bool m_bAuthenticated;		
					
	public:

		CAPISession();
		virtual ~CAPISession();

		std::string getUUID ();		
		std::string getKey ();
		std::string getUserName ();
		std::string getToken ();
		bool isAuthenticated ();
		
		void authorizeSessionByPassword(const std::string & sSaltedPasswordHash, const std::string & sClientKey);
		void setUserDetails(const std::string& sUserName, const std::string & sHashedPassword);
								
	};

	
}


#endif //__AMC_API

