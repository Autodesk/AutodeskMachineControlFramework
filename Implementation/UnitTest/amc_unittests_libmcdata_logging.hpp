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

#ifndef __AMCTEST_UNITTEST_LIBMCDATA_LOGGING
#define __AMCTEST_UNITTEST_LIBMCDATA_LOGGING

#include "amc_unittests.hpp"
#include "amc_unittests_libmcdata.hpp"
#include "common_utils.hpp"
#include "libmcdata_dynamic.hpp"

#include <cstdint>
#include <string>

namespace AMCUnitTest {

	class CUnitTestGroup_LibMCData_Logging : public CUnitTestGroup {
	public:
		std::string getTestGroupName() override {
			return "LibMCData_Logging";
		}

		void registerTests() override {
			registerTest("SessionUUID", "Verify log session UUID", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Logging::testSessionUUID, this));
			registerTest("AddAndRetrieveEntries", "Add and retrieve log entries", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Logging::testAddAndRetrieveEntries, this));
			registerTest("MaxLogEntryID", "Verify max entry ID tracking", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Logging::testMaxLogEntryID, this));
			registerTest("RetrieveByIDRange", "Retrieve entries by ID range", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Logging::testRetrieveByIDRange, this));
			
			// Additional Logging tests for coverage
			registerTest("AllLogLevels", "Test all log levels", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Logging::testAllLogLevels, this));
			registerTest("FilterByLogLevel", "Filter entries by log level", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Logging::testFilterByLogLevel, this));
			registerTest("MultipleSubsystems", "Entries from multiple subsystems", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Logging::testMultipleSubsystems, this));
			registerTest("LongMessage", "Long log message", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Logging::testLongMessage, this));
			registerTest("EmptyMessage", "Empty log message", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Logging::testEmptyMessage, this));
			registerTest("ManyEntries", "Many log entries", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Logging::testManyEntries, this));
			registerTest("EntryListIteration", "Iterate through entry list", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Logging::testEntryListIteration, this));
			registerTest("RetrieveSpecificRange", "Retrieve specific ID range", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Logging::testRetrieveSpecificRange, this));
			registerTest("FilterWarnings", "Filter warnings only", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Logging::testFilterWarnings, this));
			registerTest("FilterErrors", "Filter errors only", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Logging::testFilterErrors, this));
			registerTest("SequentialTimestamps", "Sequential timestamps", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Logging::testSequentialTimestamps, this));
			registerTest("SpecialCharacters", "Special characters in messages", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Logging::testSpecialCharacters, this));
		}

		void initializeTests() override {
		}

	private:

		struct SLoggingFixture {
			LibMCData::PWrapper m_pDataWrapper;
			LibMCData::PDataModel m_pDataModel;
			LibMCData::PLogSession m_pLogSession;
			std::string m_sBasePath;
		};

		SLoggingFixture createFixture(const std::string& sSuffix)
		{
			SLoggingFixture fixture;

			std::string sRootPath = "temp";
			if (!AMCCommon::CUtils::fileOrPathExistsOnDisk(sRootPath))
				AMCCommon::CUtils::createDirectoryOnDisk(sRootPath);

			fixture.m_sBasePath = sRootPath + "/libmcdata_logging_" + sSuffix + "_" + AMCCommon::CUtils::createUUID();
			if (!AMCCommon::CUtils::fileOrPathExistsOnDisk(fixture.m_sBasePath))
				AMCCommon::CUtils::createDirectoryOnDisk(fixture.m_sBasePath);

			std::string sDatabaseFile = fixture.m_sBasePath + "/logging.db";

			fixture.m_pDataWrapper = AMCUnitTest::loadLibMCDataInProcess();
			fixture.m_pDataModel = fixture.m_pDataWrapper->CreateDataModelInstance();
			fixture.m_pDataModel->InitialiseDatabase(fixture.m_sBasePath, LibMCData::eDataBaseType::SqLite, sDatabaseFile);
			fixture.m_pLogSession = fixture.m_pDataModel->CreateNewLogSession();

			return fixture;
		}

		void testSessionUUID()
		{
			auto fixture = createFixture("uuid");
			
			std::string sSessionUUID = fixture.m_pLogSession->GetSessionUUID();
			assertFalse(sSessionUUID.empty(), "Session UUID should not be empty");
			assertTrue(sSessionUUID.length() == 36, "Session UUID should be 36 characters (with dashes)");
		}

		void testAddAndRetrieveEntries()
		{
			auto fixture = createFixture("add_retrieve");
			
			// Add some log entries
			fixture.m_pLogSession->AddEntry("Test message 1", "subsystem1", LibMCData::eLogLevel::Message, "2025-01-01T12:00:00Z");
			fixture.m_pLogSession->AddEntry("Test message 2", "subsystem2", LibMCData::eLogLevel::Warning, "2025-01-01T12:00:01Z");
			fixture.m_pLogSession->AddEntry("Test message 3", "subsystem1", LibMCData::eLogLevel::CriticalError, "2025-01-01T12:00:02Z");
			
			// Retrieve entries - third parameter is minimum log level
			auto pEntryList = fixture.m_pLogSession->RetrieveLogEntriesByID(1, 3, LibMCData::eLogLevel::Debug);
			
			assertTrue(pEntryList->Count() == 3, "Should have 3 entries");
		}

		void testMaxLogEntryID()
		{
			auto fixture = createFixture("maxid");
			
			// Get initial max - might be 0 or might have some initial entries
			uint32_t nInitialMax = fixture.m_pLogSession->GetMaxLogEntryID();
			
			// Add entries
			fixture.m_pLogSession->AddEntry("Message 1", "subsystem", LibMCData::eLogLevel::Message, "2025-01-01T12:00:00Z");
			uint32_t nAfterFirst = fixture.m_pLogSession->GetMaxLogEntryID();
			assertTrue(nAfterFirst == nInitialMax + 1, "Max ID should increase by 1");
			
			fixture.m_pLogSession->AddEntry("Message 2", "subsystem", LibMCData::eLogLevel::Message, "2025-01-01T12:00:01Z");
			fixture.m_pLogSession->AddEntry("Message 3", "subsystem", LibMCData::eLogLevel::Message, "2025-01-01T12:00:02Z");
			
			uint32_t nAfterThree = fixture.m_pLogSession->GetMaxLogEntryID();
			assertTrue(nAfterThree == nInitialMax + 3, "Max ID should increase by 3 total");
		}

		void testRetrieveByIDRange()
		{
			auto fixture = createFixture("range");
			
			// Add 10 entries
			for (int i = 0; i < 10; i++) {
				std::string sTimestamp = "2025-01-01T12:00:" + std::to_string(10 + i) + "Z";
				fixture.m_pLogSession->AddEntry("Message " + std::to_string(i), "subsystem", LibMCData::eLogLevel::Message, sTimestamp);
			}
			
			// Retrieve entries 3-7 (5 entries)
			auto pEntryList = fixture.m_pLogSession->RetrieveLogEntriesByID(3, 7, LibMCData::eLogLevel::Debug);
			assertTrue(pEntryList->Count() == 5, "Should have 5 entries in range 3-7");
			
			// Verify first entry in range - GetEntryByIndex returns (index, nID, message, subsystem, level, timestamp)
			uint32_t nID;
			std::string sMessage, sSubsystem, sTimestamp;
			LibMCData::eLogLevel level;
			pEntryList->GetEntryByIndex(0, nID, sMessage, sSubsystem, level, sTimestamp);
			assertTrue(nID == 3, "First entry in range should have ID 3");
		}
		
		// ============= Additional Logging Tests =============
		
		void testAllLogLevels()
		{
			auto fixture = createFixture("all_levels");
			
			fixture.m_pLogSession->AddEntry("Debug message", "subsystem", LibMCData::eLogLevel::Debug, "2025-01-01T12:00:00Z");
			fixture.m_pLogSession->AddEntry("Message", "subsystem", LibMCData::eLogLevel::Message, "2025-01-01T12:00:01Z");
			fixture.m_pLogSession->AddEntry("Warning", "subsystem", LibMCData::eLogLevel::Warning, "2025-01-01T12:00:02Z");
			fixture.m_pLogSession->AddEntry("CriticalError", "subsystem", LibMCData::eLogLevel::CriticalError, "2025-01-01T12:00:03Z");
			fixture.m_pLogSession->AddEntry("FatalError", "subsystem", LibMCData::eLogLevel::FatalError, "2025-01-01T12:00:04Z");
			
			auto pEntryList = fixture.m_pLogSession->RetrieveLogEntriesByID(1, 5, LibMCData::eLogLevel::Debug);
			assertTrue(pEntryList->Count() == 5, "Should have all 5 log level entries");
		}
		
		void testFilterByLogLevel()
		{
			auto fixture = createFixture("filter_level");
			
			fixture.m_pLogSession->AddEntry("Debug 1", "subsystem", LibMCData::eLogLevel::Debug, "2025-01-01T12:00:00Z");
			fixture.m_pLogSession->AddEntry("Message 1", "subsystem", LibMCData::eLogLevel::Message, "2025-01-01T12:00:01Z");
			fixture.m_pLogSession->AddEntry("Warning 1", "subsystem", LibMCData::eLogLevel::Warning, "2025-01-01T12:00:02Z");
			fixture.m_pLogSession->AddEntry("Message 2", "subsystem", LibMCData::eLogLevel::Message, "2025-01-01T12:00:03Z");
			fixture.m_pLogSession->AddEntry("CriticalError 1", "subsystem", LibMCData::eLogLevel::CriticalError, "2025-01-01T12:00:04Z");
			
			// Filter for Warning and above
			auto pWarningsUp = fixture.m_pLogSession->RetrieveLogEntriesByID(1, 5, LibMCData::eLogLevel::Warning);
			assertTrue(pWarningsUp->Count() == 2, "Should have 2 warnings and above");
		}
		
		void testMultipleSubsystems()
		{
			auto fixture = createFixture("multi_subsys");
			
			fixture.m_pLogSession->AddEntry("Message from core", "core", LibMCData::eLogLevel::Message, "2025-01-01T12:00:00Z");
			fixture.m_pLogSession->AddEntry("Message from driver", "driver", LibMCData::eLogLevel::Message, "2025-01-01T12:00:01Z");
			fixture.m_pLogSession->AddEntry("Message from plugin", "plugin", LibMCData::eLogLevel::Message, "2025-01-01T12:00:02Z");
			fixture.m_pLogSession->AddEntry("Another from core", "core", LibMCData::eLogLevel::Warning, "2025-01-01T12:00:03Z");
			
			auto pEntryList = fixture.m_pLogSession->RetrieveLogEntriesByID(1, 4, LibMCData::eLogLevel::Debug);
			assertTrue(pEntryList->Count() == 4, "Should have 4 entries");
			
			// Check subsystems
			uint32_t nID;
			std::string sMessage, sSubsystem, sTimestamp;
			LibMCData::eLogLevel level;
			
			pEntryList->GetEntryByIndex(0, nID, sMessage, sSubsystem, level, sTimestamp);
			assertTrue(sSubsystem == "core", "First should be from core");
			
			pEntryList->GetEntryByIndex(1, nID, sMessage, sSubsystem, level, sTimestamp);
			assertTrue(sSubsystem == "driver", "Second should be from driver");
		}
		
		void testLongMessage()
		{
			auto fixture = createFixture("long_msg");
			
			// Create a long message (2KB)
			std::string sLongMessage(2048, 'L');
			
			fixture.m_pLogSession->AddEntry(sLongMessage, "subsystem", LibMCData::eLogLevel::Message, "2025-01-01T12:00:00Z");
			
			auto pEntryList = fixture.m_pLogSession->RetrieveLogEntriesByID(1, 1, LibMCData::eLogLevel::Debug);
			assertTrue(pEntryList->Count() == 1, "Should have 1 entry");
			
			uint32_t nID;
			std::string sMessage, sSubsystem, sTimestamp;
			LibMCData::eLogLevel level;
			pEntryList->GetEntryByIndex(0, nID, sMessage, sSubsystem, level, sTimestamp);
			assertTrue(sMessage.size() == sLongMessage.size(), "Long message size should match");
		}
		
		void testEmptyMessage()
		{
			auto fixture = createFixture("empty_msg");
			
			fixture.m_pLogSession->AddEntry("", "subsystem", LibMCData::eLogLevel::Message, "2025-01-01T12:00:00Z");
			
			auto pEntryList = fixture.m_pLogSession->RetrieveLogEntriesByID(1, 1, LibMCData::eLogLevel::Debug);
			assertTrue(pEntryList->Count() == 1, "Should have 1 entry");
			
			uint32_t nID;
			std::string sMessage, sSubsystem, sTimestamp;
			LibMCData::eLogLevel level;
			pEntryList->GetEntryByIndex(0, nID, sMessage, sSubsystem, level, sTimestamp);
			assertTrue(sMessage.empty(), "Message should be empty");
		}
		
		void testManyEntries()
		{
			auto fixture = createFixture("many_entries");
			
			// Add 100 entries
			for (int i = 0; i < 100; i++) {
				std::string sTimestamp = "2025-01-01T12:" + std::to_string((i / 60) % 60) + ":" + std::to_string(i % 60) + "Z";
				fixture.m_pLogSession->AddEntry("Message " + std::to_string(i), "subsystem", LibMCData::eLogLevel::Message, sTimestamp);
			}
			
			uint32_t nMaxID = fixture.m_pLogSession->GetMaxLogEntryID();
			assertTrue(nMaxID >= 100, "Should have at least 100 entries");
		}
		
		void testEntryListIteration()
		{
			auto fixture = createFixture("iterate");
			
			// Add entries
			for (int i = 0; i < 5; i++) {
				std::string sTimestamp = "2025-01-01T12:00:0" + std::to_string(i) + "Z";
				fixture.m_pLogSession->AddEntry("Entry " + std::to_string(i), "subsystem", LibMCData::eLogLevel::Message, sTimestamp);
			}
			
			auto pEntryList = fixture.m_pLogSession->RetrieveLogEntriesByID(1, 5, LibMCData::eLogLevel::Debug);
			
			// Iterate through all entries
			for (uint32_t i = 0; i < pEntryList->Count(); i++) {
				uint32_t nID;
				std::string sMessage, sSubsystem, sTimestamp;
				LibMCData::eLogLevel level;
				pEntryList->GetEntryByIndex(i, nID, sMessage, sSubsystem, level, sTimestamp);
				
				assertFalse(sMessage.empty(), "Message should not be empty");
				assertFalse(sTimestamp.empty(), "Timestamp should not be empty");
			}
		}
		
		void testRetrieveSpecificRange()
		{
			auto fixture = createFixture("specific_range");
			
			// Add 20 entries
			for (int i = 0; i < 20; i++) {
				std::string sTimestamp = "2025-01-01T12:00:" + std::to_string(10 + i) + "Z";
				fixture.m_pLogSession->AddEntry("Message " + std::to_string(i), "subsystem", LibMCData::eLogLevel::Message, sTimestamp);
			}
			
			// Retrieve middle range (10-15)
			auto pEntryList = fixture.m_pLogSession->RetrieveLogEntriesByID(10, 15, LibMCData::eLogLevel::Debug);
			assertTrue(pEntryList->Count() == 6, "Should have 6 entries in range 10-15");
		}
		
		void testFilterWarnings()
		{
			auto fixture = createFixture("filter_warn");
			
			fixture.m_pLogSession->AddEntry("Message 1", "subsystem", LibMCData::eLogLevel::Message, "2025-01-01T12:00:00Z");
			fixture.m_pLogSession->AddEntry("Warning 1", "subsystem", LibMCData::eLogLevel::Warning, "2025-01-01T12:00:01Z");
			fixture.m_pLogSession->AddEntry("Warning 2", "subsystem", LibMCData::eLogLevel::Warning, "2025-01-01T12:00:02Z");
			fixture.m_pLogSession->AddEntry("Message 2", "subsystem", LibMCData::eLogLevel::Message, "2025-01-01T12:00:03Z");
			fixture.m_pLogSession->AddEntry("Warning 3", "subsystem", LibMCData::eLogLevel::Warning, "2025-01-01T12:00:04Z");
			
			// Get only warnings (minimum level = Warning)
			auto pWarnings = fixture.m_pLogSession->RetrieveLogEntriesByID(1, 5, LibMCData::eLogLevel::Warning);
			assertTrue(pWarnings->Count() == 3, "Should have 3 warnings");
		}
		
		void testFilterErrors()
		{
			auto fixture = createFixture("filter_err");
			
			fixture.m_pLogSession->AddEntry("Message 1", "subsystem", LibMCData::eLogLevel::Message, "2025-01-01T12:00:00Z");
			fixture.m_pLogSession->AddEntry("Warning 1", "subsystem", LibMCData::eLogLevel::Warning, "2025-01-01T12:00:01Z");
			fixture.m_pLogSession->AddEntry("Error 1", "subsystem", LibMCData::eLogLevel::CriticalError, "2025-01-01T12:00:02Z");
			fixture.m_pLogSession->AddEntry("Fatal 1", "subsystem", LibMCData::eLogLevel::FatalError, "2025-01-01T12:00:03Z");
			fixture.m_pLogSession->AddEntry("Error 2", "subsystem", LibMCData::eLogLevel::CriticalError, "2025-01-01T12:00:04Z");
			
			// Get only critical errors and above
			auto pErrors = fixture.m_pLogSession->RetrieveLogEntriesByID(1, 5, LibMCData::eLogLevel::CriticalError);
			assertTrue(pErrors->Count() == 3, "Should have 3 critical errors and fatals");
		}
		
		void testSequentialTimestamps()
		{
			auto fixture = createFixture("seq_timestamps");
			
			fixture.m_pLogSession->AddEntry("First", "subsystem", LibMCData::eLogLevel::Message, "2025-01-01T12:00:00Z");
			fixture.m_pLogSession->AddEntry("Second", "subsystem", LibMCData::eLogLevel::Message, "2025-01-01T12:00:01Z");
			fixture.m_pLogSession->AddEntry("Third", "subsystem", LibMCData::eLogLevel::Message, "2025-01-01T12:00:02Z");
			
			auto pEntryList = fixture.m_pLogSession->RetrieveLogEntriesByID(1, 3, LibMCData::eLogLevel::Debug);
			assertTrue(pEntryList->Count() == 3, "Should have 3 entries");
			
			// Verify timestamps are in order
			uint32_t nID;
			std::string sMessage, sSubsystem, sTimestamp;
			LibMCData::eLogLevel level;
			
			pEntryList->GetEntryByIndex(0, nID, sMessage, sSubsystem, level, sTimestamp);
			assertTrue(sTimestamp == "2025-01-01T12:00:00Z", "First timestamp mismatch");
			
			pEntryList->GetEntryByIndex(2, nID, sMessage, sSubsystem, level, sTimestamp);
			assertTrue(sTimestamp == "2025-01-01T12:00:02Z", "Third timestamp mismatch");
		}
		
		void testSpecialCharacters()
		{
			auto fixture = createFixture("special_chars");
			
			std::string sSpecialMsg = "Message with special chars: \"quotes\", 'apostrophe', <angle>, &ampersand, newline\n, tab\t";
			
			fixture.m_pLogSession->AddEntry(sSpecialMsg, "subsystem", LibMCData::eLogLevel::Message, "2025-01-01T12:00:00Z");
			
			auto pEntryList = fixture.m_pLogSession->RetrieveLogEntriesByID(1, 1, LibMCData::eLogLevel::Debug);
			
			uint32_t nID;
			std::string sMessage, sSubsystem, sTimestamp;
			LibMCData::eLogLevel level;
			pEntryList->GetEntryByIndex(0, nID, sMessage, sSubsystem, level, sTimestamp);
			
			assertTrue(sMessage == sSpecialMsg, "Special characters message mismatch");
		}
	};

}

#endif // __AMCTEST_UNITTEST_LIBMCDATA_LOGGING
