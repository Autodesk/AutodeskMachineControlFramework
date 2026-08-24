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

#ifndef __AMCTEST_UNITTEST_LIBMCDATA_DATAMODEL
#define __AMCTEST_UNITTEST_LIBMCDATA_DATAMODEL

#include "amc_unittests.hpp"
#include "amc_unittests_libmcdata.hpp"
#include "common_utils.hpp"
#include "libmcdata_dynamic.hpp"

#include <cstdint>
#include <string>
#include <atomic>

namespace AMCUnitTest {

	// Global log callback tracking for tests
	static std::atomic<int> g_nLogCallbackCount(0);
	static std::string g_sLastLogMessage;
	static std::string g_sLastLogSubsystem;
	static LibMCData::eLogLevel g_eLastLogLevel;

	void testLogCallback(const char* pLogMessage, const char* pSubSystem, LibMCData::eLogLevel eLogLevel, const char* pTimeStamp, void* pUserData)
	{
		g_nLogCallbackCount++;
		g_sLastLogMessage = pLogMessage ? pLogMessage : "";
		g_sLastLogSubsystem = pSubSystem ? pSubSystem : "";
		g_eLastLogLevel = eLogLevel;
	}

	class CUnitTestGroup_LibMCData_DataModel : public CUnitTestGroup {
	public:
		std::string getTestGroupName() override {
			return "LibMCData_DataModel";
		}

		void registerTests() override {
			registerTest("GetDataModelVersion", "Get data model schema version", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_DataModel::testGetDataModelVersion, this));
			registerTest("GetInstallationInfo", "Get installation information", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_DataModel::testGetInstallationInfo, this));
			registerTest("GetInstallationInfoObject", "Get installation info object", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_DataModel::testGetInstallationInfoObject, this));
			registerTest("SetBaseTempDirectory", "Set base temp directory", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_DataModel::testSetBaseTempDirectory, this));
			registerTest("GetBaseTempDirectory", "Get base temp directory", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_DataModel::testGetBaseTempDirectory, this));
			registerTest("SetLogCallback", "Set log callback function", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_DataModel::testSetLogCallback, this));
			registerTest("ClearLogCallback", "Clear log callback function", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_DataModel::testClearLogCallback, this));
			registerTest("HasLogCallback", "Check log callback existence", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_DataModel::testHasLogCallback, this));
			registerTest("TriggerLogCallback", "Trigger log callback", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_DataModel::testTriggerLogCallback, this));
			registerTest("MultipleHandlerCreation", "Create multiple handlers", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_DataModel::testMultipleHandlerCreation, this));
		}

		void initializeTests() override {
		}

	private:

		struct SDataModelFixture {
			LibMCData::PWrapper m_pDataWrapper;
			LibMCData::PDataModel m_pDataModel;
			std::string m_sBasePath;
		};

		SDataModelFixture createFixture(const std::string& sSuffix)
		{
			SDataModelFixture fixture;

			std::string sRootPath = "temp";
			if (!AMCCommon::CUtils::fileOrPathExistsOnDisk(sRootPath))
				AMCCommon::CUtils::createDirectoryOnDisk(sRootPath);

			fixture.m_sBasePath = sRootPath + "/libmcdata_datamodel_" + sSuffix + "_" + AMCCommon::CUtils::createUUID();
			if (!AMCCommon::CUtils::fileOrPathExistsOnDisk(fixture.m_sBasePath))
				AMCCommon::CUtils::createDirectoryOnDisk(fixture.m_sBasePath);

			std::string sDatabaseFile = fixture.m_sBasePath + "/datamodel.db";

			fixture.m_pDataWrapper = AMCUnitTest::loadLibMCDataInProcess();
			fixture.m_pDataModel = fixture.m_pDataWrapper->CreateDataModelInstance();
			fixture.m_pDataModel->InitialiseDatabase(fixture.m_sBasePath, LibMCData::eDataBaseType::SqLite, sDatabaseFile);

			return fixture;
		}

		void testGetDataModelVersion()
		{
			auto fixture = createFixture("version");
			
			uint32_t nVersion = fixture.m_pDataModel->GetDataModelVersion();
			
			// Version should be a positive number
			assertTrue(nVersion > 0, "Data model version should be positive");
			
			// Version should be reasonable (not some garbage value)
			assertTrue(nVersion < 10000, "Data model version should be reasonable");
		}

		void testGetInstallationInfo()
		{
			auto fixture = createFixture("install_info");
			
			std::string sInstallationUUID, sInstallationSecret;
			fixture.m_pDataModel->GetInstallationInformation(sInstallationUUID, sInstallationSecret);
			
			// UUID should be valid format (36 chars with dashes)
			assertFalse(sInstallationUUID.empty(), "Installation UUID should not be empty");
			assertTrue(sInstallationUUID.length() == 36, "Installation UUID should be 36 characters");
			
			// Secret should be SHA256 format (64 hex characters)
			assertFalse(sInstallationSecret.empty(), "Installation secret should not be empty");
			assertTrue(sInstallationSecret.length() == 64, "Installation secret should be 64 hex characters");
		}

		void testGetInstallationInfoObject()
		{
			auto fixture = createFixture("install_obj");
			
			auto pInstallInfo = fixture.m_pDataModel->GetInstallationInformationObject();
			
			assertAssigned(pInstallInfo.get(), "Installation info object should be created");
			
			std::string sUUID = pInstallInfo->GetInstallationUUID();
			std::string sSecret = pInstallInfo->GetInstallationSecret();
			
			assertFalse(sUUID.empty(), "Installation UUID should not be empty");
			assertTrue(sUUID.length() == 36, "Installation UUID should be 36 characters");
			assertFalse(sSecret.empty(), "Installation secret should not be empty");
			assertTrue(sSecret.length() == 64, "Installation secret should be 64 hex characters");
		}

		void testSetBaseTempDirectory()
		{
			auto fixture = createFixture("set_temp");
			
			std::string sTempDir = fixture.m_sBasePath + "/temp_subdir";
			
			// SetBaseTempDirectory should create the directory if it doesn't exist
			fixture.m_pDataModel->SetBaseTempDirectory(sTempDir);
			
			// Verify directory was created
			assertTrue(AMCCommon::CUtils::fileOrPathExistsOnDisk(sTempDir), "Temp directory should be created");
			
			// Verify we can retrieve it
			std::string sRetrieved = fixture.m_pDataModel->GetBaseTempDirectory();
			assertFalse(sRetrieved.empty(), "Retrieved temp directory should not be empty");
		}

		void testGetBaseTempDirectory()
		{
			auto fixture = createFixture("get_temp");
			
			std::string sTempDir = fixture.m_sBasePath + "/temp_get";
			
			// Initially might be empty
			std::string sInitial = fixture.m_pDataModel->GetBaseTempDirectory();
			
			// Set and verify
			fixture.m_pDataModel->SetBaseTempDirectory(sTempDir);
			std::string sAfterSet = fixture.m_pDataModel->GetBaseTempDirectory();
			
			assertFalse(sAfterSet.empty(), "Temp directory should not be empty after setting");
			
			// Test setting empty string
			fixture.m_pDataModel->SetBaseTempDirectory("");
			std::string sAfterClear = fixture.m_pDataModel->GetBaseTempDirectory();
			assertTrue(sAfterClear.empty(), "Temp directory should be empty after clearing");
		}

		void testSetLogCallback()
		{
			auto fixture = createFixture("set_callback");
			
			// Reset global counter
			g_nLogCallbackCount = 0;
			
			// Initially should not have callback
			assertFalse(fixture.m_pDataModel->HasLogCallback(), "Should not have callback initially");
			
			// Set callback
			fixture.m_pDataModel->SetLogCallback(testLogCallback, nullptr);
			
			// Now should have callback
			assertTrue(fixture.m_pDataModel->HasLogCallback(), "Should have callback after setting");
		}

		void testClearLogCallback()
		{
			auto fixture = createFixture("clear_callback");
			
			// Set callback first
			fixture.m_pDataModel->SetLogCallback(testLogCallback, nullptr);
			assertTrue(fixture.m_pDataModel->HasLogCallback(), "Should have callback after setting");
			
			// Clear callback
			fixture.m_pDataModel->ClearLogCallback();
			
			// Should no longer have callback
			assertFalse(fixture.m_pDataModel->HasLogCallback(), "Should not have callback after clearing");
		}

		void testHasLogCallback()
		{
			auto fixture = createFixture("has_callback");
			
			// Initially false
			assertFalse(fixture.m_pDataModel->HasLogCallback(), "HasLogCallback should be false initially");
			
			// Set callback
			fixture.m_pDataModel->SetLogCallback(testLogCallback, nullptr);
			assertTrue(fixture.m_pDataModel->HasLogCallback(), "HasLogCallback should be true after setting");
			
			// Clear callback
			fixture.m_pDataModel->ClearLogCallback();
			assertFalse(fixture.m_pDataModel->HasLogCallback(), "HasLogCallback should be false after clearing");
		}

		void testTriggerLogCallback()
		{
			auto fixture = createFixture("trigger_callback");
			
			// Reset globals
			g_nLogCallbackCount = 0;
			g_sLastLogMessage = "";
			g_sLastLogSubsystem = "";
			
			// Set callback
			fixture.m_pDataModel->SetLogCallback(testLogCallback, nullptr);
			
			// Trigger callback
			std::string sMessage = "Test log message";
			std::string sSubsystem = "TestSubsystem";
			std::string sTimestamp = "2025-01-01T12:00:00Z";
			
			fixture.m_pDataModel->TriggerLogCallback(sMessage, sSubsystem, LibMCData::eLogLevel::Warning, sTimestamp);
			
			// Verify callback was called
			assertTrue(g_nLogCallbackCount == 1, "Callback should be called once");
			assertTrue(g_sLastLogMessage == sMessage, "Log message should match");
			assertTrue(g_sLastLogSubsystem == sSubsystem, "Log subsystem should match");
			assertTrue(g_eLastLogLevel == LibMCData::eLogLevel::Warning, "Log level should match");
			
			// Trigger again
			fixture.m_pDataModel->TriggerLogCallback("Another message", "AnotherSub", LibMCData::eLogLevel::CriticalError, sTimestamp);
			assertTrue(g_nLogCallbackCount == 2, "Callback should be called twice");
		}

		void testMultipleHandlerCreation()
		{
			auto fixture = createFixture("multi_handlers");
			
			// Create multiple handlers of different types
			auto pStorage1 = fixture.m_pDataModel->CreateStorage();
			auto pStorage2 = fixture.m_pDataModel->CreateStorage();
			auto pLoginHandler1 = fixture.m_pDataModel->CreateLoginHandler();
			auto pLoginHandler2 = fixture.m_pDataModel->CreateLoginHandler();
			auto pBuildJobHandler1 = fixture.m_pDataModel->CreateBuildJobHandler();
			auto pPersistencyHandler1 = fixture.m_pDataModel->CreatePersistencyHandler();
			auto pLogSession1 = fixture.m_pDataModel->CreateNewLogSession();
			auto pJournalSession1 = fixture.m_pDataModel->CreateJournalSession();
			auto pAlertSession1 = fixture.m_pDataModel->CreateAlertSession();
			auto pTelemetrySession1 = fixture.m_pDataModel->CreateTelemetrySession();
			
			// All should be valid
			assertAssigned(pStorage1.get(), "Storage1 should be created");
			assertAssigned(pStorage2.get(), "Storage2 should be created");
			assertAssigned(pLoginHandler1.get(), "LoginHandler1 should be created");
			assertAssigned(pLoginHandler2.get(), "LoginHandler2 should be created");
			assertAssigned(pBuildJobHandler1.get(), "BuildJobHandler1 should be created");
			assertAssigned(pPersistencyHandler1.get(), "PersistencyHandler1 should be created");
			assertAssigned(pLogSession1.get(), "LogSession1 should be created");
			assertAssigned(pJournalSession1.get(), "JournalSession1 should be created");
			assertAssigned(pAlertSession1.get(), "AlertSession1 should be created");
			assertAssigned(pTelemetrySession1.get(), "TelemetrySession1 should be created");
		}
	};

}

#endif // __AMCTEST_UNITTEST_LIBMCDATA_DATAMODEL
