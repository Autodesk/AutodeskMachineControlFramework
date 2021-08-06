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


#include "amc_servicehandler.hpp"
#include "common_utils.hpp"
#include "libmc_exceptiontypes.hpp"


namespace AMC {
	

	CServiceHandler::CServiceHandler(PLogger pLogger)
		:  m_nMaxThreadCount(SERVICETHREADCOUNT_DEFAULT), m_pLogger (pLogger)
	{
		LibMCAssertNotNull(pLogger.get());
	}

	CServiceHandler::~CServiceHandler()
	{

	}

	void CServiceHandler::addServiceToQueue(PService pService)
	{
		LibMCAssertNotNull(pService.get());
		if (pService->getServiceHandler() != this)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDSERVICEHANDLER);

		{
			// lock guard only for the push
			std::lock_guard<std::mutex> lockGuard(m_Mutex);
			m_QueuedServices.push(pService);			
		}

		handleQueue();
		clearGarbage();
	}

	void CServiceHandler::handleQueue()
	{
		{
			std::lock_guard<std::mutex> lockGuard(m_Mutex);

			for (auto iter = m_RunningServices.begin(); iter != m_RunningServices.end();)
			{
				auto pService = (*iter);

				if (!pService->getIsRunning())
				{
					// Store service in Garbage collector, as *iter might call handleQueue.
					iter = m_RunningServices.erase(iter);
					m_FinishedServices.insert(pService);
				}
				else
				{
					iter++;
				}
			}
		}

		// Start new services
		while ((m_RunningServices.size() < (size_t)m_nMaxThreadCount) && (!m_QueuedServices.empty ())) {

			PService pService; 

			{
				std::lock_guard<std::mutex> lockGuard(m_Mutex);

				pService = m_QueuedServices.front();
				m_QueuedServices.pop();
				m_RunningServices.insert(pService);
			}

			logMessage("Executing service " + pService->getName(), LOG_SUBSYSTEM_SYSTEM, AMC::eLogLevel::Message);
			pService->executeThreaded();
		}


	}


	void CServiceHandler::logMessage(const std::string& sMessage, const std::string& sSubSystem, const eLogLevel logLevel)
	{
		m_pLogger->logMessage(sMessage, sSubSystem, logLevel);
	}

	void CServiceHandler::setMaxThreadCount(uint32_t nMaxThreadCount)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);

		if (nMaxThreadCount == 0)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDMAXTHREADCOUNT);

		m_nMaxThreadCount = nMaxThreadCount;
	}

	void CServiceHandler::clearGarbage()
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);
		m_FinishedServices.clear();
	}


}


