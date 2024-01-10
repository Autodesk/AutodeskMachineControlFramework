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


#ifndef __AMC_ALERTHANDLER
#define __AMC_ALERTHANDLER

#include "amc_alertdefinition.hpp"
#include <mutex>
#include <map>

namespace LibMCData {

	class CAlertSession;
	typedef std::shared_ptr<CAlertSession> PAlertSession;

}


namespace AMC {

	class CAlertHandler;
	typedef std::shared_ptr<CAlertHandler> PAlertHandler;


	class CAlertHandler {
	private:
	
		std::mutex m_Mutex;
		std::map<std::string, PAlertDefinition> m_AlertDefinitions;
		
	public:

		CAlertHandler ();
		
		virtual ~CAlertHandler();
		
		PAlertDefinition findDefinition (const std::string & sIdentifier, bool bFailIfNotExisting);
		
		bool hasDefinition (const std::string & sIdentifier);
		
		PAlertDefinition addDefinition (const std::string& sIdentifier, LibMCData::eAlertLevel alertLevel, const CLanguageString& sDescription, bool bNeedsAcknowledgement);

	};

	
}


#endif //__AMC_ALERTHANDLER

