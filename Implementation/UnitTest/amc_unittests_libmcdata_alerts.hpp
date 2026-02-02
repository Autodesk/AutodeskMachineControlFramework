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
	};

}

#endif // __AMCTEST_UNITTEST_LIBMCDATA_ALERTS
