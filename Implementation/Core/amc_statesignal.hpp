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


#ifndef __AMC_STATESIGNAL
#define __AMC_STATESIGNAL

#ifndef _STATESIGNAL_HEADERPROTECTION
#error Never include amc_statesignal.hpp from outside of amc_statesignal.cpp and amc_statesignalhandler.cpp
#endif

#include "amc_statesignalparameter.hpp"
#include "amc_parametergroup.hpp"

#include <memory>
#include <string>
#include <map>
#include <list>

namespace AMC {

	class CStateSignal;
	typedef std::shared_ptr<CStateSignal> PStateSignal;


	class CStateSignal {
	private:
		std::string m_sInstanceName;
		std::string m_sName;
		bool m_bIsTriggered;
		std::string m_sCurrentParameterData;
		std::string m_sCurrentSignalUUID;

		std::list <CStateSignalParameter> m_ParameterDefinitions;
		std::list <CStateSignalParameter> m_ResultDefinitions;
		std::map <std::string, std::string> m_ResultMap;


	public:

		CStateSignal(const std::string & sInstanceName, const std::string& sName, const std::list<CStateSignalParameter>& Parameters, const std::list<CStateSignalParameter>& Results);
		virtual ~CStateSignal();

		// NOT ThreadSafe. Do not call directly!
		// Mutexes need to be added outside of the signal
		std::string getNameInternal();
		std::string getInstanceNameInternal();
		std::string getParameterDataInternal();
		bool triggerSignalInternal(const std::string& sParameterData, std::string& sNewSignalUUID);
		bool checkSignalInternal(std::string& sCurrentSignalUUID);
		bool canTriggerInternal();
		void clearSignalInternal();

		void markSignalAsHandledInternal(const std::string& sResultData);
		bool signalHasBeenHandledInternal(const std::string& sSignalUUID, bool clearAllResults, std::string& sResultData);

		std::string getCurrentUUID();

		void populateParameterGroup(CParameterGroup* pParameterGroup);
		void populateResultGroup(CParameterGroup* pResultGroup);

	};

	
}


#endif //__AMC_STATESIGNAL

