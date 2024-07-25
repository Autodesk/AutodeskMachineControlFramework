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


#include "amc_logger_multi.hpp"
#include "libmc_exceptiontypes.hpp"

namespace AMC {
		
	CLogger_Multi::CLogger_Multi(AMCCommon::PChrono pGlobalChrono)
		: CLogger (pGlobalChrono)
	{
		if (pGlobalChrono.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
	}
	
	CLogger_Multi::~CLogger_Multi()
	{
	}


	void CLogger_Multi::addLogger(PLogger pLogger)
	{
		LibMCAssertNotNull(pLogger.get());

		std::lock_guard<std::mutex> lockguard(m_LoggersMutex);
		m_Loggers.push_back(pLogger);

		if (pLogger->supportsLogMessagesRetrieval())
			m_RetrievalLogger = pLogger;

	}

	void CLogger_Multi::logMessageEx(const std::string& sMessage, const std::string& sSubSystem, const eLogLevel logLevel, const std::string& sTimeStamp)
	{
		std::lock_guard<std::mutex> lockguard(m_LoggersMutex);
		for (auto logger : m_Loggers)
			logger->logMessageEx(sMessage, sSubSystem, logLevel, sTimeStamp);
	}

	bool CLogger_Multi::supportsLogMessagesRetrieval()
	{
		std::lock_guard<std::mutex> lockguard(m_LoggersMutex);		
		return (m_RetrievalLogger.get () != nullptr);
	}


	void CLogger_Multi::retrieveLogMessages(std::vector<CLoggerEntry>& entryBuffer, const uint32_t startID, const uint32_t endID, const eLogLevel eMinLogLevel)
	{

		std::lock_guard<std::mutex> lockguard(m_LoggersMutex);
		if (m_RetrievalLogger.get() != nullptr)
		{
			m_RetrievalLogger->retrieveLogMessages(entryBuffer, startID, endID, eMinLogLevel);			
		}

	}

	uint32_t CLogger_Multi::getLogMessageHeadID()
	{
		std::lock_guard<std::mutex> lockguard(m_LoggersMutex);
		if (m_RetrievalLogger.get() != nullptr)
		{
			return m_RetrievalLogger->getLogMessageHeadID();
		}

		return 0;
	}


}


