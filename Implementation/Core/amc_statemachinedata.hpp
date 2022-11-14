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

#ifndef __AMC_PARAMETERINSTANCES
#define __AMC_PARAMETERINSTANCES

#include "amc_parameterhandler.hpp"

#include <memory>
#include <vector>
#include <map>
#include <string>
#include <mutex>

namespace AMC {

	class CStateMachineData;
	typedef std::shared_ptr<CStateMachineData> PStateMachineData;


	class CStateMachineData {
	private:

		std::map <std::string, PParameterHandler> m_StateMachineParameters;
		std::map <std::string, PParameterGroup> m_StateMachineDataStores;
		std::map <std::string, std::string> m_StateMachineStates;

		std::mutex m_Mutex;
		
	public:
	
		CStateMachineData();
		virtual ~CStateMachineData();

		void registerParameterHandler (const std::string & sInstanceName, PParameterHandler pParameterHandler);
		PParameterHandler getParameterHandler (const std::string& sInstanceName);

		CParameterGroup* getDataStore(const std::string& sInstanceName);
		void setInstanceStateName(const std::string& sInstanceName, const std::string& sInstanceState);
		std::string getInstanceStateName(const std::string& sInstanceName);

		static void extractParameterDetailsFromDotString(const std::string& sParameterPath, std::string& sParameterInstance, std::string& sParameterGroup, std::string& sParameterName, bool allowEmptyParameterName, bool allowNonAlphaNumericNames);

	};

	
}


#endif //__AMC_PARAMETERHANDLER

