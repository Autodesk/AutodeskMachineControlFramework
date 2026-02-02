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
	};

}

#endif // __AMCTEST_UNITTEST_LIBMCDATA_LOGGING
