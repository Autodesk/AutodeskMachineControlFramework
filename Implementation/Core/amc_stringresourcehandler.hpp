/*++

Copyright (C) 2023 Autodesk Inc.

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


#ifndef __AMC_STRINGRESOURCEHANDLER
#define __AMC_STRINGRESOURCEHANDLER

#include "amc_stringresource.hpp"
#include "amc_stringresourcetable.hpp"
#include <mutex>

namespace AMC {


	// https://en.cppreference.com/w/cpp/memory/enable_shared_from_this

	class CStringResourceHandler : public std::enable_shared_from_this<CStringResourceHandler>{
	private:
	
		WStringResourceHandler m_pStringResourceHandler;
		uint64_t m_nIDCounter;

		PStringResourceTable m_pDefaultResourceTable;
		std::map<StringLanguageID, PStringResourceTable> m_pLanguageResourceTables;

		std::map<StringResourceID, std::string> m_ResourceIDMap;
		std::map<std::string, StringResourceID> m_ResourceNameMap;

		std::mutex m_Mutex;
	
	public:

		CStringResourceHandler ();
		
		virtual ~CStringResourceHandler();
		
		CStringResource registerString (const std::string & sResourceName, const std::string & sDefaultValue);

		std::string retrieveStringValue (const StringLanguageID nLanguageID, const StringResourceID nResourceID);

	};

	
}


#endif //__AMC_STRINGRESOURCEHANDLER

