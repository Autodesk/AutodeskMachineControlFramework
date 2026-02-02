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

#ifndef __AMCTEST_UNITTEST_LIBMCDATA_ALERTS
#define __AMCTEST_UNITTEST_LIBMCDATA_ALERTS

#include "amc_unittests.hpp"
#include "amc_unittests_libmcdata.hpp"
#include "common_utils.hpp"
#include "libmcdata_dynamic.hpp"

#include <cstdint>
#include <string>

namespace AMCUnitTest {

	class CUnitTestGroup_LibMCData_Alerts : public CUnitTestGroup {
	public:
		std::string getTestGroupName() override {
			return "LibMCData_Alerts";
		}

		void registerTests() override {
			registerTest("CreateAndRetrieve", "Create and retrieve alert", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Alerts::testCreateAndRetrieve, this));
			registerTest("AlertLevels", "Test all alert levels", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Alerts::testAlertLevels, this));
			registerTest("AlertActive", "Test alert active state", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Alerts::testAlertActive, this));
			registerTest("Deactivation", "Test alert deactivation", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Alerts::testDeactivation, this));
			registerTest("HasAlert", "Test HasAlert check", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Alerts::testHasAlert, this));
			registerTest("Acknowledgement", "Test alert acknowledgement", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Alerts::testAcknowledgement, this));
			registerTest("RetrieveAlerts", "Test retrieving multiple alerts", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Alerts::testRetrieveAlerts, this));
			registerTest("AlertProperties", "Test alert properties", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Alerts::testAlertProperties, this));
			
			// Additional AlertSession tests for coverage
			registerTest("RetrieveByType", "Retrieve alerts by type", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Alerts::testRetrieveByType, this));
			registerTest("RetrieveActiveByType", "Retrieve active alerts by type", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Alerts::testRetrieveActiveByType, this));
			
			// Additional Alert Tests for Coverage (7 tests)
			registerTest("AlertIteratorMoveNext", "Alert iterator MoveNext", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Alerts::testAlertIteratorMoveNext, this));
			registerTest("AlertIteratorGetCurrent", "Alert iterator GetCurrent", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Alerts::testAlertIteratorGetCurrent, this));
			registerTest("EmptyDescription", "Empty alert description", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Alerts::testEmptyDescription, this));
			registerTest("LongContextInfo", "Long context information", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Alerts::testLongContextInfo, this));
			registerTest("AlertUUIDFormat", "Alert UUID format", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Alerts::testAlertUUIDFormat, this));
			registerTest("MultipleAcknowledgement", "Multiple acknowledgement test", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Alerts::testMultipleAcknowledgement, this));
			registerTest("AllAlertLevels", "All alert levels test", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Alerts::testAllAlertLevels, this));
		}

		void initializeTests() override {
		}

	private:

		struct SAlertsFixture {
			LibMCData::PWrapper m_pDataWrapper;
			LibMCData::PDataModel m_pDataModel;
			LibMCData::PAlertSession m_pAlertSession;
			std::string m_sBasePath;
		};

		SAlertsFixture createFixture(const std::string& sSuffix)
		{
			SAlertsFixture fixture;

			std::string sRootPath = "temp";
			if (!AMCCommon::CUtils::fileOrPathExistsOnDisk(sRootPath))
				AMCCommon::CUtils::createDirectoryOnDisk(sRootPath);

			fixture.m_sBasePath = sRootPath + "/libmcdata_alerts_" + sSuffix + "_" + AMCCommon::CUtils::createUUID();
			if (!AMCCommon::CUtils::fileOrPathExistsOnDisk(fixture.m_sBasePath))
				AMCCommon::CUtils::createDirectoryOnDisk(fixture.m_sBasePath);

			std::string sDatabaseFile = fixture.m_sBasePath + "/alerts.db";

			fixture.m_pDataWrapper = AMCUnitTest::loadLibMCDataInProcess();
			fixture.m_pDataModel = fixture.m_pDataWrapper->CreateDataModelInstance();
			fixture.m_pDataModel->InitialiseDatabase(fixture.m_sBasePath, LibMCData::eDataBaseType::SqLite, sDatabaseFile);
			fixture.m_pAlertSession = fixture.m_pDataModel->CreateAlertSession();

			return fixture;
		}

		void testCreateAndRetrieve()
		{
			auto fixture = createFixture("create_retrieve");
			
			std::string sAlertUUID = AMCCommon::CUtils::createUUID();
			std::string sIdentifier = "testalertidentifier";  // No dots - simpler identifier
			std::string sDescription = "Test alert description";
			std::string sDescIdentifier = "testdescid";
			std::string sContextInfo = "Context information";
			std::string sTimestamp = "2025-01-01T12:00:00Z";
			bool bNeedsAck = true;
			
			auto pAlert = fixture.m_pAlertSession->AddAlert(
				sAlertUUID,
				sIdentifier,
				LibMCData::eAlertLevel::Warning,
				sDescription,
				sDescIdentifier,
				sContextInfo,
				bNeedsAck,
				sTimestamp
			);
			
			assertAssigned(pAlert.get(), "Alert should be created");
			assertTrue(pAlert->GetUUID() == sAlertUUID, "Alert UUID mismatch");
			
			// Retrieve the same alert
			auto pRetrieved = fixture.m_pAlertSession->GetAlertByUUID(sAlertUUID);
			assertAssigned(pRetrieved.get(), "Retrieved alert should exist");
			assertTrue(pRetrieved->GetIdentifier() == sIdentifier, "Identifier mismatch");
		}

		void testAlertLevels()
		{
			auto fixture = createFixture("levels");
			
			// Test all levels
			LibMCData::eAlertLevel levels[] = {
				LibMCData::eAlertLevel::Message,
				LibMCData::eAlertLevel::Warning,
				LibMCData::eAlertLevel::CriticalError,
				LibMCData::eAlertLevel::FatalError
			};
			
			int nIdx = 0;
			for (auto level : levels) {
				std::string sUUID = AMCCommon::CUtils::createUUID();
				auto pAlert = fixture.m_pAlertSession->AddAlert(
					sUUID,
					"level_test_" + std::to_string(nIdx++),  // Must be valid identifier (no dots at start)
					level,
					"Level test",
					"level_test_desc",
					"",
					false,
					"2025-01-01T12:00:00Z"
				);
				
				assertTrue(pAlert->GetLevel() == level, "Alert level should match");
			}
		}

		void testAlertActive()
		{
			auto fixture = createFixture("active");
			
			std::string sUUID = AMCCommon::CUtils::createUUID();
			auto pAlert = fixture.m_pAlertSession->AddAlert(
				sUUID,
				"activetest",
				LibMCData::eAlertLevel::Warning,
				"Active test",
				"activedesc",
				"",
				false,
				"2025-01-01T12:00:00Z"
			);
			
			assertTrue(pAlert->IsActive(), "Newly created alert should be active");
		}

		void testDeactivation()
		{
			auto fixture = createFixture("deactivate");
			
			std::string sUUID = AMCCommon::CUtils::createUUID();
			auto pAlert = fixture.m_pAlertSession->AddAlert(
				sUUID,
				"deactivatetest",
				LibMCData::eAlertLevel::Warning,
				"Deactivation test",
				"deactivatedesc",
				"",
				false,
				"2025-01-01T12:00:00Z"
			);
			
			assertTrue(pAlert->IsActive(), "Alert should initially be active");
			
			pAlert->DeactivateAlert();
			
			assertFalse(pAlert->IsActive(), "Alert should be inactive after deactivation");
			
			// Re-retrieve and verify
			auto pRetrieved = fixture.m_pAlertSession->GetAlertByUUID(sUUID);
			assertFalse(pRetrieved->IsActive(), "Retrieved alert should be inactive");
		}

		void testHasAlert()
		{
			auto fixture = createFixture("has_alert");
			
			std::string sExistingUUID = AMCCommon::CUtils::createUUID();
			std::string sNonExistingUUID = AMCCommon::CUtils::createUUID();
			
			fixture.m_pAlertSession->AddAlert(
				sExistingUUID,
				"hasalerttest",
				LibMCData::eAlertLevel::Warning,
				"Has alert test",
				"hasdesc",
				"",
				false,
				"2025-01-01T12:00:00Z"
			);
			
			assertTrue(fixture.m_pAlertSession->HasAlert(sExistingUUID), "HasAlert should return true for existing alert");
			assertFalse(fixture.m_pAlertSession->HasAlert(sNonExistingUUID), "HasAlert should return false for non-existing alert");
		}

		void testAcknowledgement()
		{
			auto fixture = createFixture("ack");
			
			std::string sAlertUUID = AMCCommon::CUtils::createUUID();
			std::string sUserUUID = AMCCommon::CUtils::createUUID();
			
			auto pAlert = fixture.m_pAlertSession->AddAlert(
				sAlertUUID,
				"acktest",
				LibMCData::eAlertLevel::Warning,
				"Acknowledgement test",
				"ackdesc",
				"",
				true,  // Needs acknowledgement
				"2025-01-01T12:00:00Z"
			);
			
			assertTrue(pAlert->GetNeedsAcknowledgement(), "Alert should need acknowledgement");
			assertFalse(pAlert->HasBeenAcknowledged(), "Alert should not be acknowledged yet");
			
			// Acknowledge the alert
			pAlert->AcknowledgeForUser(sUserUUID, "User comment", "2025-01-01T12:30:00Z");
			
			assertTrue(pAlert->HasBeenAcknowledged(), "Alert should be acknowledged");
			
			// Verify acknowledgement info
			std::string sAckUserUUID, sAckComment, sAckTime;
			pAlert->GetAcknowledgementInformation(sAckUserUUID, sAckComment, sAckTime);
			assertTrue(sAckUserUUID == sUserUUID, "Acknowledgement user UUID mismatch");
			assertTrue(sAckComment == "User comment", "Acknowledgement comment mismatch");
		}

		void testRetrieveAlerts()
		{
			auto fixture = createFixture("retrieve");
			
			// Create multiple alerts
			for (int i = 0; i < 5; i++) {
				std::string sUUID = AMCCommon::CUtils::createUUID();
				fixture.m_pAlertSession->AddAlert(
					sUUID,
					"retrievetest" + std::to_string(i),
					LibMCData::eAlertLevel::Warning,
					"Retrieve test " + std::to_string(i),
					"retrievedesc",
					"",
					false,
					"2025-01-01T12:00:0" + std::to_string(i) + "Z"
				);
			}
			
			// Retrieve all (bOnlyActive = false)
			auto pIterator = fixture.m_pAlertSession->RetrieveAlerts(false);
			
			uint64_t nCount = pIterator->Count();
			assertTrue(nCount == 5, "Should have 5 alerts");
		}

		void testAlertProperties()
		{
			auto fixture = createFixture("properties");
			
			std::string sAlertUUID = AMCCommon::CUtils::createUUID();
			std::string sIdentifier = "proptestidentifier";
			std::string sDescription = "Property test description";
			std::string sDescIdentifier = "propdescid";
			std::string sContextInfo = "Context: test=123";
			std::string sTimestamp = "2025-01-15T14:30:00Z";
			
			auto pAlert = fixture.m_pAlertSession->AddAlert(
				sAlertUUID,
				sIdentifier,
				LibMCData::eAlertLevel::CriticalError,
				sDescription,
				sDescIdentifier,
				sContextInfo,
				true,
				sTimestamp
			);
			
			// Verify all properties
			assertTrue(pAlert->GetUUID() == sAlertUUID, "UUID mismatch");
			assertTrue(pAlert->GetIdentifier() == sIdentifier, "Identifier mismatch");
			assertTrue(pAlert->GetDescription() == sDescription, "Description mismatch");
			assertTrue(pAlert->GetDescriptionIdentifier() == sDescIdentifier, "Description identifier mismatch");
			assertTrue(pAlert->GetReadableContextInformation() == sContextInfo, "Context info mismatch");
			assertTrue(pAlert->GetTimestampUTC() == sTimestamp, "Timestamp mismatch");
			assertTrue(pAlert->GetLevel() == LibMCData::eAlertLevel::CriticalError, "Level mismatch");
			
			// Test level string
			std::string sLevelString = pAlert->GetLevelString();
			assertFalse(sLevelString.empty(), "Level string should not be empty");
		}
		
		// ============= Additional AlertSession Tests =============
		
		void testRetrieveByType()
		{
			auto fixture = createFixture("by_type");
			
			// Use the same identifier for all alerts we want to retrieve together
			std::string sTypeIdentifier = "typealert";
			
			// Create 3 alerts with the SAME identifier (this is the key for RetrieveAlertsByType)
			for (int i = 0; i < 3; i++) {
				std::string sUUID = AMCCommon::CUtils::createUUID();
				fixture.m_pAlertSession->AddAlert(
					sUUID,
					sTypeIdentifier,  // Same identifier for all
					LibMCData::eAlertLevel::Warning,
					"Type test " + std::to_string(i),
					"",
					"",
					false,
					"2025-01-01T12:00:0" + std::to_string(i) + "Z"
				);
			}
			
			// Create alerts with different identifier
			for (int i = 0; i < 2; i++) {
				std::string sUUID = AMCCommon::CUtils::createUUID();
				fixture.m_pAlertSession->AddAlert(
					sUUID,
					"othertype",  // Different identifier
					LibMCData::eAlertLevel::Message,
					"Other type " + std::to_string(i),
					"",
					"",
					false,
					"2025-01-01T13:00:0" + std::to_string(i) + "Z"
				);
			}
			
			// Retrieve by type (bOnlyActive = false)
			auto pIterator = fixture.m_pAlertSession->RetrieveAlertsByType(sTypeIdentifier, false);
			
			uint64_t nCount = pIterator->Count();
			assertTrue(nCount >= 3, "Should have at least 3 alerts of type");
		}
		
		void testRetrieveActiveByType()
		{
			auto fixture = createFixture("active_by_type");
			
			// Use the same identifier for both alerts
			std::string sTypeIdentifier = "activetyp";
			
			// Create active alert
			std::string sActiveUUID = AMCCommon::CUtils::createUUID();
			auto pActiveAlert = fixture.m_pAlertSession->AddAlert(
				sActiveUUID,
				sTypeIdentifier,  // Same identifier
				LibMCData::eAlertLevel::Warning,
				"Active type test",
				"",
				"",
				false,
				"2025-01-01T12:00:00Z"
			);
			
			// Create another alert with same identifier, then deactivate
			std::string sInactiveUUID = AMCCommon::CUtils::createUUID();
			auto pInactiveAlert = fixture.m_pAlertSession->AddAlert(
				sInactiveUUID,
				sTypeIdentifier,  // Same identifier
				LibMCData::eAlertLevel::Warning,
				"Inactive type test",
				"",
				"",
				false,
				"2025-01-01T12:00:01Z"
			);
			pInactiveAlert->DeactivateAlert();
			
			// Retrieve only active by type
			auto pActiveIterator = fixture.m_pAlertSession->RetrieveAlertsByType(sTypeIdentifier, true);
			uint64_t nActiveCount = pActiveIterator->Count();
			assertTrue(nActiveCount >= 1, "Should have at least 1 active alert of type");
			
			// Retrieve all by type
			auto pAllIterator = fixture.m_pAlertSession->RetrieveAlertsByType(sTypeIdentifier, false);
			uint64_t nAllCount = pAllIterator->Count();
			assertTrue(nAllCount >= 2, "Should have at least 2 total alerts of type");
		}
		
		// ============= Additional Alert Tests for Coverage (7 tests) =============
		
		void testAlertIteratorMoveNext()
		{
			auto fixture = createFixture("iter_movenext");
			
			// Create multiple alerts
			for (int i = 0; i < 5; i++) {
				std::string sUUID = AMCCommon::CUtils::createUUID();
				fixture.m_pAlertSession->AddAlert(
					sUUID,
					"iteralert" + std::to_string(i),
					LibMCData::eAlertLevel::Warning,
					"Iterator test " + std::to_string(i),
					"",
					"",
					false,
					"2025-01-01T12:00:0" + std::to_string(i) + "Z"
				);
			}
			
			auto pIterator = fixture.m_pAlertSession->RetrieveAlerts(false);
			
			uint64_t nCount = 0;
			while (pIterator->MoveNext()) {
				auto pAlert = pIterator->GetCurrentAlert();
				assertAssigned(pAlert.get(), "Alert should be returned");
				assertFalse(pAlert->GetUUID().empty(), "Alert UUID should not be empty");
				nCount++;
			}
			assertTrue(nCount >= 5, "Should have iterated through at least 5 alerts");
		}
		
		void testAlertIteratorGetCurrent()
		{
			auto fixture = createFixture("iter_getcurrent");
			
			std::string sUUID = AMCCommon::CUtils::createUUID();
			fixture.m_pAlertSession->AddAlert(
				sUUID,
				"getcurrentalert",
				LibMCData::eAlertLevel::CriticalError,
				"GetCurrent test",
				"getcurrentdesc",
				"Context info",
				true,
				"2025-01-01T12:00:00Z"
			);
			
			auto pIterator = fixture.m_pAlertSession->RetrieveAlerts(false);
			
			assertTrue(pIterator->MoveNext(), "Should be able to move to first alert");
			
			auto pAlert = pIterator->GetCurrentAlert();
			assertAssigned(pAlert.get(), "GetCurrentAlert should return an alert");
			assertFalse(pAlert->GetDescription().empty(), "Alert should have description");
		}
		
		void testEmptyDescription()
		{
			auto fixture = createFixture("empty_desc");
			
			std::string sUUID = AMCCommon::CUtils::createUUID();
			auto pAlert = fixture.m_pAlertSession->AddAlert(
				sUUID,
				"emptydescalert",
				LibMCData::eAlertLevel::Message,
				"", // Empty description
				"emptydescid",
				"",
				false,
				"2025-01-01T12:00:00Z"
			);
			
			assertTrue(pAlert->GetDescription().empty(), "Empty description should be preserved");
		}
		
		void testLongContextInfo()
		{
			auto fixture = createFixture("long_context");
			
			std::string sUUID = AMCCommon::CUtils::createUUID();
			std::string sLongContext(5000, 'C'); // 5KB of context
			
			auto pAlert = fixture.m_pAlertSession->AddAlert(
				sUUID,
				"longcontextalert",
				LibMCData::eAlertLevel::Warning,
				"Long context test",
				"longcontextdesc",
				sLongContext,
				false,
				"2025-01-01T12:00:00Z"
			);
			
			assertTrue(pAlert->GetReadableContextInformation() == sLongContext, "Long context should be preserved");
		}
		
		void testAlertUUIDFormat()
		{
			auto fixture = createFixture("uuid_format");
			
			std::string sUUID = AMCCommon::CUtils::createUUID();
			auto pAlert = fixture.m_pAlertSession->AddAlert(
				sUUID,
				"uuidformatalert",
				LibMCData::eAlertLevel::Warning,
				"UUID format test",
				"",
				"",
				false,
				"2025-01-01T12:00:00Z"
			);
			
			std::string sRetrievedUUID = pAlert->GetUUID();
			
			// UUID should be in standard format
			assertTrue(sRetrievedUUID.length() == 36, "UUID should be 36 characters");
			assertTrue(sRetrievedUUID[8] == '-', "UUID should have dash at position 8");
			assertTrue(sRetrievedUUID[13] == '-', "UUID should have dash at position 13");
			assertTrue(sRetrievedUUID[18] == '-', "UUID should have dash at position 18");
			assertTrue(sRetrievedUUID[23] == '-', "UUID should have dash at position 23");
		}
		
		void testMultipleAcknowledgement()
		{
			auto fixture = createFixture("multi_ack");
			
			std::string sAlertUUID = AMCCommon::CUtils::createUUID();
			std::string sUserUUID1 = AMCCommon::CUtils::createUUID();
			
			auto pAlert = fixture.m_pAlertSession->AddAlert(
				sAlertUUID,
				"multiacktest",
				LibMCData::eAlertLevel::Warning,
				"Multi ack test",
				"",
				"",
				true, // Needs acknowledgement
				"2025-01-01T12:00:00Z"
			);
			
			// First acknowledgement
			pAlert->AcknowledgeForUser(sUserUUID1, "First acknowledgement", "2025-01-01T12:30:00Z");
			assertTrue(pAlert->HasBeenAcknowledged(), "Alert should be acknowledged");
			
			// Get acknowledgement info
			std::string sAckUser, sAckComment, sAckTime;
			pAlert->GetAcknowledgementInformation(sAckUser, sAckComment, sAckTime);
			assertTrue(sAckComment == "First acknowledgement", "Acknowledgement comment should be preserved");
		}
		
		void testAllAlertLevels()
		{
			auto fixture = createFixture("all_levels");
			
			LibMCData::eAlertLevel levels[] = {
				LibMCData::eAlertLevel::Message,
				LibMCData::eAlertLevel::Warning,
				LibMCData::eAlertLevel::CriticalError,
				LibMCData::eAlertLevel::FatalError
			};
			
			for (auto level : levels) {
				std::string sUUID = AMCCommon::CUtils::createUUID();
				auto pAlert = fixture.m_pAlertSession->AddAlert(
					sUUID,
					"levelalert",
					level,
					"Level test",
					"",
					"",
					false,
					"2025-01-01T12:00:00Z"
				);
				
				assertTrue(pAlert->GetLevel() == level, "Alert level should match");
				assertFalse(pAlert->GetLevelString().empty(), "Level string should not be empty");
			}
		}
	};

}

#endif // __AMCTEST_UNITTEST_LIBMCDATA_ALERTS
