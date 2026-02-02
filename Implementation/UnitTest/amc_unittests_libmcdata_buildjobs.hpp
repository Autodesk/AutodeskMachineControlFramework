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

#ifndef __AMCTEST_UNITTEST_LIBMCDATA_BUILDJOBS
#define __AMCTEST_UNITTEST_LIBMCDATA_BUILDJOBS

#include "amc_unittests.hpp"
#include "amc_unittests_libmcdata.hpp"
#include "common_utils.hpp"
#include "libmcdata_dynamic.hpp"

#include <cstdint>
#include <string>
#include <vector>
#include <chrono>

namespace AMCUnitTest {

	class CUnitTestGroup_LibMCData_BuildJobs : public CUnitTestGroup {
	public:
		std::string getTestGroupName() override {
			return "LibMCData_BuildJobs";
		}

		void registerTests() override {
			// Job Handler tests
			registerTest("CreateJob", "Create build job", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_BuildJobs::testCreateJob, this));
			registerTest("JobExists", "Check job existence", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_BuildJobs::testJobExists, this));
			registerTest("RetrieveJob", "Retrieve job by UUID", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_BuildJobs::testRetrieveJob, this));
			registerTest("ListJobsByStatus", "List jobs by status", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_BuildJobs::testListJobsByStatus, this));
			
			// Job operations tests
			registerTest("JobProperties", "Job properties access", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_BuildJobs::testJobProperties, this));
			registerTest("ChangeName", "Change job name", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_BuildJobs::testChangeName, this));
			registerTest("MetaData", "Job metadata storage", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_BuildJobs::testMetaData, this));
			
			// Job execution tests
			registerTest("CreateExecution", "Create job execution", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_BuildJobs::testCreateExecution, this));
			registerTest("ExecutionStatus", "Execution status changes", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_BuildJobs::testExecutionStatus, this));
			registerTest("ExecutionDescription", "Execution description", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_BuildJobs::testExecutionDescription, this));
		}

		void initializeTests() override {
		}

	private:

		struct SBuildJobsFixture {
			LibMCData::PWrapper m_pDataWrapper;
			LibMCData::PDataModel m_pDataModel;
			LibMCData::PBuildJobHandler m_pBuildJobHandler;
			LibMCData::PStorage m_pStorage;
			LibMCData::PLoginHandler m_pLoginHandler;
			std::string m_sBasePath;
			std::string m_sJournalUUID;
			std::string m_sUserUUID;
		};

		SBuildJobsFixture createFixture(const std::string& sSuffix)
		{
			SBuildJobsFixture fixture;

			std::string sRootPath = "temp";
			if (!AMCCommon::CUtils::fileOrPathExistsOnDisk(sRootPath))
				AMCCommon::CUtils::createDirectoryOnDisk(sRootPath);

			fixture.m_sBasePath = sRootPath + "/libmcdata_buildjobs_" + sSuffix + "_" + AMCCommon::CUtils::createUUID();
			if (!AMCCommon::CUtils::fileOrPathExistsOnDisk(fixture.m_sBasePath))
				AMCCommon::CUtils::createDirectoryOnDisk(fixture.m_sBasePath);

			std::string sDatabaseFile = fixture.m_sBasePath + "/buildjobs.db";

			fixture.m_pDataWrapper = AMCUnitTest::loadLibMCDataInProcess();
			fixture.m_pDataModel = fixture.m_pDataWrapper->CreateDataModelInstance();
			fixture.m_pDataModel->InitialiseDatabase(fixture.m_sBasePath, LibMCData::eDataBaseType::SqLite, sDatabaseFile);
			fixture.m_pBuildJobHandler = fixture.m_pDataModel->CreateBuildJobHandler();
			fixture.m_pStorage = fixture.m_pDataModel->CreateStorage();
			fixture.m_pLoginHandler = fixture.m_pDataModel->CreateLoginHandler();
			
			// Get journal UUID for executions
			auto pJournalSession = fixture.m_pDataModel->CreateJournalSession();
			fixture.m_sJournalUUID = pJournalSession->GetSessionUUID();
			
			// Create a test user (required for build jobs)
			std::string sSalt = "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
			std::string sHash = "fedcba9876543210fedcba9876543210fedcba9876543210fedcba9876543210";
			fixture.m_sUserUUID = fixture.m_pLoginHandler->CreateUser("testuser", "operator", sSalt, sHash, "Test User");

			return fixture;
		}

		uint64_t getCurrentTimestamp()
		{
			return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(
				std::chrono::system_clock::now().time_since_epoch()).count());
		}

		std::string createTestStream(SBuildJobsFixture& fixture, const std::string& sContent)
		{
			std::string sUUID = AMCCommon::CUtils::createUUID();
			std::string sUserUUID = AMCCommon::CUtils::createUUID();
			std::vector<uint8_t> buffer(sContent.begin(), sContent.end());
			fixture.m_pStorage->StoreNewStream(sUUID, "testfile.bin", "application/octet-stream", buffer, sUserUUID, getCurrentTimestamp());
			return sUUID;
		}

		void testCreateJob()
		{
			auto fixture = createFixture("create_job");
			
			std::string sJobUUID = AMCCommon::CUtils::createUUID();
			std::string sStreamUUID = createTestStream(fixture, "job content");
			
			// CreateJob(sJobUUID, sName, sUserUUID, sStorageStreamUUID, nAbsoluteTimeStamp)
			auto pJob = fixture.m_pBuildJobHandler->CreateJob(
				sJobUUID,
				"Test Job",
				fixture.m_sUserUUID,
				sStreamUUID,
				getCurrentTimestamp()
			);
			
			assertAssigned(pJob.get(), "Job should be created");
			assertTrue(pJob->GetUUID() == sJobUUID, "Job UUID mismatch");
			assertTrue(pJob->GetName() == "Test Job", "Job name mismatch");
		}

		void testJobExists()
		{
			auto fixture = createFixture("job_exists");
			
			std::string sExistingUUID = AMCCommon::CUtils::createUUID();
			std::string sNonExistingUUID = AMCCommon::CUtils::createUUID();
			std::string sStreamUUID = createTestStream(fixture, "content");
			
			fixture.m_pBuildJobHandler->CreateJob(
				sExistingUUID, "Job", fixture.m_sUserUUID, sStreamUUID, getCurrentTimestamp()
			);
			
			assertTrue(fixture.m_pBuildJobHandler->JobExists(sExistingUUID), "Job should exist");
			assertFalse(fixture.m_pBuildJobHandler->JobExists(sNonExistingUUID), "Job should not exist");
		}

		void testRetrieveJob()
		{
			auto fixture = createFixture("retrieve_job");
			
			std::string sJobUUID = AMCCommon::CUtils::createUUID();
			std::string sStreamUUID = createTestStream(fixture, "content");
			
			fixture.m_pBuildJobHandler->CreateJob(
				sJobUUID, "Retrieve Test", fixture.m_sUserUUID, sStreamUUID, getCurrentTimestamp()
			);
			
			auto pJob = fixture.m_pBuildJobHandler->RetrieveJob(sJobUUID);
			
			assertAssigned(pJob.get(), "Job should be retrieved");
			assertTrue(pJob->GetName() == "Retrieve Test", "Job name mismatch");
		}

		void testListJobsByStatus()
		{
			auto fixture = createFixture("list_status");
			
			// Create jobs with different initial statuses (they start as Created)
			for (int i = 0; i < 3; i++) {
				std::string sJobUUID = AMCCommon::CUtils::createUUID();
				std::string sStreamUUID = createTestStream(fixture, "content" + std::to_string(i));
				fixture.m_pBuildJobHandler->CreateJob(
					sJobUUID, "Job " + std::to_string(i), fixture.m_sUserUUID, sStreamUUID, getCurrentTimestamp()
				);
			}
			
			auto pIterator = fixture.m_pBuildJobHandler->ListJobsByStatus(LibMCData::eBuildJobStatus::Created);
			assertTrue(pIterator->Count() == 3, "Should have 3 created jobs");
		}

		void testJobProperties()
		{
			auto fixture = createFixture("job_props");
			
			std::string sJobUUID = AMCCommon::CUtils::createUUID();
			std::string sStreamUUID = createTestStream(fixture, "content");
			
			auto pJob = fixture.m_pBuildJobHandler->CreateJob(
				sJobUUID, "Property Test", fixture.m_sUserUUID, sStreamUUID, getCurrentTimestamp()
			);
			
			assertTrue(pJob->GetUUID() == sJobUUID, "UUID mismatch");
			assertTrue(pJob->GetName() == "Property Test", "Name mismatch");
			assertTrue(pJob->GetStorageStreamUUID() == sStreamUUID, "Storage stream UUID mismatch");
		}

		void testChangeName()
		{
			auto fixture = createFixture("change_name");
			
			std::string sJobUUID = AMCCommon::CUtils::createUUID();
			std::string sStreamUUID = createTestStream(fixture, "content");
			
			auto pJob = fixture.m_pBuildJobHandler->CreateJob(
				sJobUUID, "Original Name", fixture.m_sUserUUID, sStreamUUID, getCurrentTimestamp()
			);
			
			assertTrue(pJob->GetName() == "Original Name", "Original name mismatch");
			
			pJob->ChangeName("New Name");
			assertTrue(pJob->GetName() == "New Name", "New name mismatch");
			
			// Verify change persisted
			auto pRetrieved = fixture.m_pBuildJobHandler->RetrieveJob(sJobUUID);
			assertTrue(pRetrieved->GetName() == "New Name", "Persisted name mismatch");
		}

		void testMetaData()
		{
			auto fixture = createFixture("metadata");
			
			std::string sJobUUID = AMCCommon::CUtils::createUUID();
			std::string sStreamUUID = createTestStream(fixture, "content");
			
			auto pJob = fixture.m_pBuildJobHandler->CreateJob(
				sJobUUID, "Metadata Test", fixture.m_sUserUUID, sStreamUUID, getCurrentTimestamp()
			);
			
			// Store metadata - StoreMetaDataString(sKey, sValue, nAbsoluteTimeStamp)
			pJob->StoreMetaDataString("key1", "value1", getCurrentTimestamp());
			pJob->StoreMetaDataString("key2", "value2", getCurrentTimestamp());
			
			// Check existence
			assertTrue(pJob->HasMetaDataString("key1"), "Should have key1");
			assertTrue(pJob->HasMetaDataString("key2"), "Should have key2");
			assertFalse(pJob->HasMetaDataString("key3"), "Should not have key3");
			
			// Get values
			assertTrue(pJob->GetMetaDataString("key1") == "value1", "Value1 mismatch");
			assertTrue(pJob->GetMetaDataString("key2") == "value2", "Value2 mismatch");
		}

		void testCreateExecution()
		{
			auto fixture = createFixture("create_exec");
			
			std::string sJobUUID = AMCCommon::CUtils::createUUID();
			std::string sStreamUUID = createTestStream(fixture, "content");
			
			auto pJob = fixture.m_pBuildJobHandler->CreateJob(
				sJobUUID, "Execution Test", fixture.m_sUserUUID, sStreamUUID, getCurrentTimestamp()
			);
			
			// Validate job
			pJob->StartValidating();
			pJob->FinishValidating(10);
			
			// Create execution - CreateBuildJobExecution(sDescription, sUserUUID, nAbsoluteStartTimeStamp)
			auto pExec = pJob->CreateBuildJobExecution("Test execution", fixture.m_sUserUUID, getCurrentTimestamp());
			
			assertAssigned(pExec.get(), "Execution should be created");
			assertTrue(pExec->GetJobUUID() == sJobUUID, "Job UUID mismatch");
		}

		void testExecutionStatus()
		{
			auto fixture = createFixture("exec_status");
			
			std::string sJobUUID = AMCCommon::CUtils::createUUID();
			std::string sStreamUUID = createTestStream(fixture, "content");
			
			auto pJob = fixture.m_pBuildJobHandler->CreateJob(
				sJobUUID, "Exec Status Test", fixture.m_sUserUUID, sStreamUUID, getCurrentTimestamp()
			);
			pJob->StartValidating();
			pJob->FinishValidating(10);
			
			auto pExec = pJob->CreateBuildJobExecution("Execution", fixture.m_sUserUUID, getCurrentTimestamp());
			
			// Initial status should be InProcess
			assertTrue(pExec->GetStatus() == LibMCData::eBuildJobExecutionStatus::InProcess, "Initial status should be InProcess");
			
			// Change status - ChangeStatus(eNewStatus, nAbsoluteEndTimeStamp)
			pExec->ChangeStatus(LibMCData::eBuildJobExecutionStatus::Finished, getCurrentTimestamp());
			assertTrue(pExec->GetStatus() == LibMCData::eBuildJobExecutionStatus::Finished, "Status should be Finished");
		}

		void testExecutionDescription()
		{
			auto fixture = createFixture("exec_desc");
			
			std::string sJobUUID = AMCCommon::CUtils::createUUID();
			std::string sStreamUUID = createTestStream(fixture, "content");
			
			auto pJob = fixture.m_pBuildJobHandler->CreateJob(
				sJobUUID, "Exec Desc Test", fixture.m_sUserUUID, sStreamUUID, getCurrentTimestamp()
			);
			pJob->StartValidating();
			pJob->FinishValidating(10);
			
			auto pExec = pJob->CreateBuildJobExecution("Initial description", fixture.m_sUserUUID, getCurrentTimestamp());
			
			assertTrue(pExec->GetDescription() == "Initial description", "Initial description mismatch");
			
			// Set new description
			pExec->SetDescription("Updated description");
			assertTrue(pExec->GetDescription() == "Updated description", "Updated description mismatch");
		}
	};

}

#endif // __AMCTEST_UNITTEST_LIBMCDATA_BUILDJOBS
