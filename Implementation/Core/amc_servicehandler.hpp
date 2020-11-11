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


#ifndef __AMC_SERVICEHANDLER
#define __AMC_SERVICEHANDLER

#include "amc_service.hpp"
#include "amc_logger.hpp"

#include <memory>
#include <string>
#include <set>
#include <queue>

#define SERVICETHREADCOUNT_MIN 2
#define SERVICETHREADCOUNT_MAX 128
#define SERVICETHREADCOUNT_DEFAULT 8

namespace AMC {

	class CServiceHandler;
	typedef std::shared_ptr<CServiceHandler> PServiceHandler;

	class CService;
	typedef std::shared_ptr<CService> PService;

	class CServiceHandler {
	private:

		uint32_t m_nMaxThreadCount;
		std::mutex m_Mutex;
		std::queue<PService> m_QueuedServices;
		std::set<PService> m_RunningServices;
		std::set<PService> m_FinishedServices;

		PLogger m_pLogger;


	public:

		CServiceHandler(PLogger pLogger);
		virtual ~CServiceHandler();

		void addServiceToQueue (PService pService);

		void handleQueue();

		void clearGarbage();

		void logMessage(const std::string& sMessage, const std::string& sSubSystem, const eLogLevel logLevel);

		void setMaxThreadCount(uint32_t nMaxThreadCount);

	};

}


#endif //__AMC_SERVICEHANDLER

