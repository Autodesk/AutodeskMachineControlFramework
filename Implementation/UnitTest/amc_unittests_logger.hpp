/*++

Copyright (C) 2025 Autodesk Inc.

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

#ifndef __AMCTEST_UNITTEST_LOGGER
#define __AMCTEST_UNITTEST_LOGGER


#include "amc_unittests.hpp"
#include "amc_logger.hpp"
#include "amc_logger_multi.hpp"
#include "amc_logger_callback.hpp"
#include "amc_logger_database.hpp"


namespace AMCUnitTest {

	class CLogger_TestRecorder : public AMC::CLogger {
	private:
		std::vector<AMC::CLoggerEntry> m_Entries;
		bool m_bSupportsRetrieval;
		uint32_t m_nHeadID;
	public:
		CLogger_TestRecorder(AMCCommon::PChrono pGlobalChrono, bool bSupportsRetrieval)
			: AMC::CLogger(pGlobalChrono), m_bSupportsRetrieval(bSupportsRetrieval), m_nHeadID(0)
		{
		}

		void logMessageEx(const std::string& sMessage, const std::string& sSubSystem, const AMC::eLogLevel logLevel, const std::string& sTimeStamp) override
		{
			m_nHeadID++;
			m_Entries.emplace_back(m_nHeadID, sMessage, sSubSystem, logLevel, sTimeStamp);
		}

		bool supportsLogMessagesRetrieval() override
		{
			return m_bSupportsRetrieval;
		}

		void retrieveLogMessages(std::vector<AMC::CLoggerEntry>& entryBuffer, const uint32_t startID, const uint32_t endID, const AMC::eLogLevel eMinLogLevel) override
		{
			for (const auto& entry : m_Entries) {
				if ((entry.getID() >= startID) && (entry.getID() <= endID) && (entry.getlogLevel() >= eMinLogLevel)) {
					entryBuffer.push_back(entry);
				}
			}
		}

		uint32_t getLogMessageHeadID() override
		{
			return m_nHeadID;
		}

		size_t getEntryCount() const
		{
			return m_Entries.size();
		}

		const AMC::CLoggerEntry& getEntry(size_t index) const
		{
			return m_Entries.at(index);
		}
	};

	class CUnitTestGroup_Logger : public CUnitTestGroup {
	public:

		std::string getTestGroupName() override {
			return "Logger";
		}

		void registerTests() override {
			registerTest("LoggerEntryBasics", "Logger entry getters and level strings", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_Logger::testLoggerEntryBasics, this));
			registerTest("LoggerBaseAndMulti", "Logger base helpers and multi logger routing", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_Logger::testLoggerBaseAndMulti, this));
			registerTest("LoggerConstructorValidation", "Logger database/callback constructor validation", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_Logger::testLoggerConstructorValidation, this));
		}

		void initializeTests() override {
		}

	private:

		void testLoggerEntryBasics()
		{
			AMC::CLoggerEntry entry(7, "message", "sub", AMC::eLogLevel::Warning, "time");
			assertTrue(entry.getID() == 7);
			assertTrue(entry.getMessage() == "message");
			assertTrue(entry.getSubSystem() == "sub");
			assertTrue(entry.getlogLevel() == AMC::eLogLevel::Warning);
			assertTrue(entry.getTimeStamp() == "time");
			assertTrue(entry.getlogLevelString() == "Warning");

			assertTrue(AMC::CLogger::logLevelToString(AMC::eLogLevel::FatalError) == "fatal");
			assertTrue(AMC::CLogger::logLevelToString(AMC::eLogLevel::CriticalError) == "critical");
			assertTrue(AMC::CLogger::logLevelToString(AMC::eLogLevel::Warning) == "warning");
			assertTrue(AMC::CLogger::logLevelToString(AMC::eLogLevel::Message) == "message");
			assertTrue(AMC::CLogger::logLevelToString(AMC::eLogLevel::Info) == "info");
			assertTrue(AMC::CLogger::logLevelToString(AMC::eLogLevel::Debug) == "debug");
			assertTrue(AMC::CLogger::logLevelToString(AMC::eLogLevel::Unknown) == "unknown");
		}

		void testLoggerBaseAndMulti()
		{
			auto chrono = std::make_shared<AMCCommon::CChrono>();
			auto pRecorder = std::make_shared<CLogger_TestRecorder>(chrono, false);

			pRecorder->logMessage("hello", LOG_SUBSYSTEM_TESTING, AMC::eLogLevel::Message);
			assertTrue(pRecorder->getEntryCount() == 1);
			assertTrue(pRecorder->getEntry(0).getMessage() == "hello");
			assertTrue(!pRecorder->getEntry(0).getTimeStamp().empty());

			auto pRetriever = std::make_shared<CLogger_TestRecorder>(chrono, true);
			pRetriever->logMessageEx("first", "sub", AMC::eLogLevel::Info, "t1");
			pRetriever->logMessageEx("second", "sub", AMC::eLogLevel::Warning, "t2");

			AMC::CLogger_Multi multiLogger(chrono);
			multiLogger.addLogger(pRecorder);
			multiLogger.addLogger(pRetriever);

			multiLogger.logMessageEx("multi", "sub", AMC::eLogLevel::Debug, "t3");
			assertTrue(pRecorder->getEntryCount() == 2);
			assertTrue(pRetriever->getEntryCount() == 3);

			assertTrue(multiLogger.supportsLogMessagesRetrieval());
			std::vector<AMC::CLoggerEntry> entries;
			multiLogger.retrieveLogMessages(entries, 1, 10, AMC::eLogLevel::Info);
			assertTrue(entries.size() == 2);
			assertTrue(multiLogger.getLogMessageHeadID() == pRetriever->getLogMessageHeadID());
		}

		void testLoggerConstructorValidation()
		{
			bool thrown = false;
			try {
				AMC::CLogger_Callback logger(nullptr, std::make_shared<AMCCommon::CChrono>());
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected callback logger to reject null data model");

			thrown = false;
			try {
				AMC::CLogger_Callback logger(std::shared_ptr<LibMCData::CDataModel>(), nullptr);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected callback logger to reject null chrono");

			thrown = false;
			try {
				AMC::CLogger_Database logger(nullptr, std::make_shared<AMCCommon::CChrono>());
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected database logger to reject null data model");

			thrown = false;
			try {
				AMC::CLogger_Database logger(std::shared_ptr<LibMCData::CDataModel>(), nullptr);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected database logger to reject null chrono");
		}
	};

}

#endif // __AMCTEST_UNITTEST_LOGGER
