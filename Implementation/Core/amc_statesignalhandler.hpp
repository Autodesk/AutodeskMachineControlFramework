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


#ifndef __AMC_STATESIGNALHANDLER
#define __AMC_STATESIGNALHANDLER

#include <memory>
#include <string>
#include <map>
#include <list>
#include <mutex>

#include "amc_statesignalparameter.hpp"
#include "amc_parametergroup.hpp"

#define DEFAULT_WAITFOR_SLEEP_MS 1

namespace AMC {

	class CStateSignalHandler;
	typedef std::shared_ptr<CStateSignalHandler> PStateSignalHandler;

	// Do not include StateSignal.hpp anywhere for threadsafety!
	class CStateSignal;
	typedef std::shared_ptr<CStateSignal> PStateSignal;

	class CStateSignalHandler {
	private:
		
		std::map<std::pair <std::string, std::string>, PStateSignal> m_SignalMap;
		std::map<std::string, PStateSignal> m_SignalUUIDLookupMap;
		std::mutex m_Mutex;

	public:

		CStateSignalHandler();
		virtual ~CStateSignalHandler();

		void addSignalDefinition(const std::string & sInstanceName, const std::string & sSignalName, const std::list<CStateSignalParameter> & Parameters, const std::list<CStateSignalParameter> & Results);

		bool triggerSignal(const std::string& sInstanceName, const std::string& sSignalName, const std::string& sParameterData, std::string& sNewSignalUUID);

		bool checkSignal(const std::string& sInstanceName, const std::string& sSignalName, std::string& sCurrentSignalUUID);

		bool checkSignalUUID(const std::string& sInstanceName, std::string sCurrentSignalUUID);

		bool canTrigger(const std::string& sInstanceName, const std::string& sSignalName);

		bool hasSignalDefinition(const std::string& sInstanceName, const std::string& sSignalName);

		bool findSignalPropertiesByUUID(const std::string& sSignalUUID, std::string & sInstanceName, std::string& sSignalName, std::string& sParameterData);

		void markSignalAsHandled(const std::string& sSignalUUID, const std::string& sResultData);

		bool signalHasBeenHandled(const std::string& sSignalUUID, const bool clearAllResults, std::string& sResultData);

		void populateParameterGroup(const std::string& sInstanceName, const std::string& sSignalName, CParameterGroup * pParameterGroup);
		void populateResultGroup(const std::string& sInstanceName, const std::string& sSignalName, CParameterGroup* pResultGroup);

	};

	
}


#endif //__AMC_STATESIGNALHANDLER

