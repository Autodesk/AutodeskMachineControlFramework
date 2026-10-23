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

#ifndef __AMCTEST_UNITTEST_LIBMCDATA_CUSTOMDATA
#define __AMCTEST_UNITTEST_LIBMCDATA_CUSTOMDATA

#include "amc_unittests.hpp"
#include "amc_unittests_libmcdata.hpp"
#include "common_utils.hpp"
#include "libmcdata_dynamic.hpp"

#include <cstdint>
#include <string>
#include <vector>
#include <chrono>

namespace AMCUnitTest {

	class CUnitTestGroup_LibMCData_CustomData : public CUnitTestGroup {
	public:
		std::string getTestGroupName() override {
			return "LibMCData_CustomData";
		}

		void registerTests() override {
			registerTest("AddJobData", "Add custom data to job", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_CustomData::testAddJobData, this));
			registerTest("GetDataUUID", "Get custom data UUID", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_CustomData::testGetDataUUID, this));
			registerTest("GetIdentifier", "Get custom data identifier", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_CustomData::testGetIdentifier, this));
			registerTest("GetName", "Get custom data name", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_CustomData::testGetName, this));
			registerTest("GetTimeStamp", "Get custom data timestamp", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_CustomData::testGetTimeStamp, this));
			registerTest("GetStorageStreamUUID", "Get storage stream UUID", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_CustomData::testGetStorageStreamUUID, this));
			registerTest("GetStorageStreamSHA2", "Get storage stream SHA256", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_CustomData::testGetStorageStreamSHA2, this));
			registerTest("GetStorageStreamSize", "Get storage stream size", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_CustomData::testGetStorageStreamSize, this));
			registerTest("GetDataType", "Get data type enum", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_CustomData::testGetDataType, this));
			registerTest("GetDataTypeAsString", "Get data type as string", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_CustomData::testGetDataTypeAsString, this));
		}

		void initializeTests() override {
		}

	private:

		struct SCustomDataFixture {
			LibMCData::PWrapper m_pDataWrapper;
			LibMCData::PDataModel m_pDataModel;
			LibMCData::PBuildJobHandler m_pBuildJobHandler;
			LibMCData::PStorage m_pStorage;
			LibMCData::PLoginHandler m_pLoginHandler;
			std::string m_sBasePath;
			std::string m_sUserUUID;
		};

		SCustomDataFixture createFixture(const std::string& sSuffix)
		{
			SCustomDataFixture fixture;

			std::string sRootPath = "temp";
			if (!AMCCommon::CUtils::fileOrPathExistsOnDisk(sRootPath))
				AMCCommon::CUtils::createDirectoryOnDisk(sRootPath);

			fixture.m_sBasePath = sRootPath + "/libmcdata_customdata_" + sSuffix + "_" + AMCCommon::CUtils::createUUID();
			if (!AMCCommon::CUtils::fileOrPathExistsOnDisk(fixture.m_sBasePath))
				AMCCommon::CUtils::createDirectoryOnDisk(fixture.m_sBasePath);

			std::string sDatabaseFile = fixture.m_sBasePath + "/customdata.db";

			fixture.m_pDataWrapper = AMCUnitTest::loadLibMCDataInProcess();
			fixture.m_pDataModel = fixture.m_pDataWrapper->CreateDataModelInstance();
			fixture.m_pDataModel->InitialiseDatabase(fixture.m_sBasePath, LibMCData::eDataBaseType::SqLite, sDatabaseFile);
			fixture.m_pBuildJobHandler = fixture.m_pDataModel->CreateBuildJobHandler();
			fixture.m_pStorage = fixture.m_pDataModel->CreateStorage();
			fixture.m_pLoginHandler = fixture.m_pDataModel->CreateLoginHandler();
			
			// Create a test user
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

		std::string createTestStream(SCustomDataFixture& fixture, const std::string& sContent)
		{
			std::string sUUID = AMCCommon::CUtils::createUUID();
			std::vector<uint8_t> buffer(sContent.begin(), sContent.end());
			fixture.m_pStorage->StoreNewStream(sUUID, "testfile.bin", "application/octet-stream", buffer, fixture.m_sUserUUID, getCurrentTimestamp());
			return sUUID;
		}

		LibMCData::PBuildJob createTestJob(SCustomDataFixture& fixture, const std::string& sName)
		{
			std::string sJobUUID = AMCCommon::CUtils::createUUID();
			std::string sStreamUUID = createTestStream(fixture, "job content for " + sName);
			return fixture.m_pBuildJobHandler->CreateJob(sJobUUID, sName, fixture.m_sUserUUID, sStreamUUID, getCurrentTimestamp());
		}

		void testAddJobData()
		{
			auto fixture = createFixture("add_data");
			
			auto pJob = createTestJob(fixture, "Job With Data");
			
			// Create a storage stream for the custom data
			std::string sDataContent = "Custom binary data content";
			std::string sStorageStreamUUID = createTestStream(fixture, sDataContent);
			auto pStream = fixture.m_pStorage->RetrieveStream(sStorageStreamUUID);
			
			// Add custom data to the job
			std::string sIdentifier = "customdata.test";
			std::string sName = "Test Custom Data";
			
			pJob->AddJobData(sIdentifier, sName, pStream.get(), LibMCData::eCustomDataType::CustomBinaryData, fixture.m_sUserUUID, getCurrentTimestamp());
			
			// Verify data was added
			assertTrue(pJob->HasJobDataIdentifier(sIdentifier), "Job should have custom data");
		}

		void testGetDataUUID()
		{
			auto fixture = createFixture("data_uuid");
			
			auto pJob = createTestJob(fixture, "Job For Data UUID");
			
			std::string sDataContent = "Data content";
			std::string sStorageStreamUUID = createTestStream(fixture, sDataContent);
			auto pStream = fixture.m_pStorage->RetrieveStream(sStorageStreamUUID);
			std::string sIdentifier = "datauuid.test";
			
			pJob->AddJobData(sIdentifier, "Test Data", pStream.get(), LibMCData::eCustomDataType::CustomBinaryData, fixture.m_sUserUUID, getCurrentTimestamp());
			
			auto pCustomData = pJob->RetrieveJobDataByIdentifier(sIdentifier);
			
			assertAssigned(pCustomData.get(), "Custom data should be retrieved");
			
			std::string sDataUUID = pCustomData->GetDataUUID();
			assertFalse(sDataUUID.empty(), "Data UUID should not be empty");
			assertTrue(sDataUUID.length() == 36, "Data UUID should be 36 characters");
		}

		void testGetIdentifier()
		{
			auto fixture = createFixture("identifier");
			
			auto pJob = createTestJob(fixture, "Job For Identifier");
			
			std::string sStorageStreamUUID = createTestStream(fixture, "Content");
			auto pStream = fixture.m_pStorage->RetrieveStream(sStorageStreamUUID);
			std::string sIdentifier = "myidentifier.test";
			
			pJob->AddJobData(sIdentifier, "Identifier Test", pStream.get(), LibMCData::eCustomDataType::CustomBinaryData, fixture.m_sUserUUID, getCurrentTimestamp());
			
			auto pCustomData = pJob->RetrieveJobDataByIdentifier(sIdentifier);
			
			assertTrue(pCustomData->GetIdentifier() == sIdentifier, "Identifier should match");
		}

		void testGetName()
		{
			auto fixture = createFixture("name");
			
			auto pJob = createTestJob(fixture, "Job For Name");
			
			std::string sStorageStreamUUID = createTestStream(fixture, "Content");
			auto pStream = fixture.m_pStorage->RetrieveStream(sStorageStreamUUID);
			std::string sIdentifier = "nametest.test";
			std::string sName = "My Custom Data Name";
			
			pJob->AddJobData(sIdentifier, sName, pStream.get(), LibMCData::eCustomDataType::CustomBinaryData, fixture.m_sUserUUID, getCurrentTimestamp());
			
			auto pCustomData = pJob->RetrieveJobDataByIdentifier(sIdentifier);
			
			assertTrue(pCustomData->GetName() == sName, "Name should match");
		}

		void testGetTimeStamp()
		{
			auto fixture = createFixture("timestamp");
			
			auto pJob = createTestJob(fixture, "Job For Timestamp");
			
			std::string sStorageStreamUUID = createTestStream(fixture, "Content");
			auto pStream = fixture.m_pStorage->RetrieveStream(sStorageStreamUUID);
			std::string sIdentifier = "timestamp.test";
			
			pJob->AddJobData(sIdentifier, "Timestamp Test", pStream.get(), LibMCData::eCustomDataType::CustomBinaryData, fixture.m_sUserUUID, getCurrentTimestamp());
			
			auto pCustomData = pJob->RetrieveJobDataByIdentifier(sIdentifier);
			
			std::string sTimestamp = pCustomData->GetTimeStamp();
			assertFalse(sTimestamp.empty(), "Timestamp should not be empty");
		}

		void testGetStorageStreamUUID()
		{
			auto fixture = createFixture("stream_uuid");
			
			auto pJob = createTestJob(fixture, "Job For Stream UUID");
			
			std::string sStorageStreamUUID = createTestStream(fixture, "Content");
			auto pStream = fixture.m_pStorage->RetrieveStream(sStorageStreamUUID);
			std::string sIdentifier = "streamuuid.test";
			
			pJob->AddJobData(sIdentifier, "Stream UUID Test", pStream.get(), LibMCData::eCustomDataType::CustomBinaryData, fixture.m_sUserUUID, getCurrentTimestamp());
			
			auto pCustomData = pJob->RetrieveJobDataByIdentifier(sIdentifier);
			
			std::string sRetrievedUUID = pCustomData->GetStorageStreamUUID();
			
			// Normalize both for comparison
			std::string sNormalized1 = AMCCommon::CUtils::normalizeUUIDString(sStorageStreamUUID);
			std::string sNormalized2 = AMCCommon::CUtils::normalizeUUIDString(sRetrievedUUID);
			
			assertTrue(sNormalized1 == sNormalized2, "Storage stream UUID should match");
		}

		void testGetStorageStreamSHA2()
		{
			auto fixture = createFixture("sha256");
			
			auto pJob = createTestJob(fixture, "Job For SHA256");
			
			std::string sContent = "Content for SHA256 test";
			std::string sStorageStreamUUID = createTestStream(fixture, sContent);
			auto pStream = fixture.m_pStorage->RetrieveStream(sStorageStreamUUID);
			std::string sIdentifier = "sha256.test";
			
			pJob->AddJobData(sIdentifier, "SHA256 Test", pStream.get(), LibMCData::eCustomDataType::CustomBinaryData, fixture.m_sUserUUID, getCurrentTimestamp());
			
			auto pCustomData = pJob->RetrieveJobDataByIdentifier(sIdentifier);
			
			std::string sSHA2 = pCustomData->GetStorageStreamSHA2();
			
			assertFalse(sSHA2.empty(), "SHA256 should not be empty");
			assertTrue(sSHA2.length() == 64, "SHA256 should be 64 hex characters");
		}

		void testGetStorageStreamSize()
		{
			auto fixture = createFixture("stream_size");
			
			auto pJob = createTestJob(fixture, "Job For Stream Size");
			
			std::string sContent = "This is the content for size test";
			std::string sStorageStreamUUID = createTestStream(fixture, sContent);
			auto pStream = fixture.m_pStorage->RetrieveStream(sStorageStreamUUID);
			std::string sIdentifier = "streamsize.test";
			
			pJob->AddJobData(sIdentifier, "Stream Size Test", pStream.get(), LibMCData::eCustomDataType::CustomBinaryData, fixture.m_sUserUUID, getCurrentTimestamp());
			
			auto pCustomData = pJob->RetrieveJobDataByIdentifier(sIdentifier);
			
			uint64_t nSize = pCustomData->GetStorageStreamSize();
			assertTrue(nSize == sContent.size(), "Stream size should match content size");
		}

		void testGetDataType()
		{
			auto fixture = createFixture("data_type");
			
			auto pJob = createTestJob(fixture, "Job For Data Type");
			
			// Test different data types
			struct DataTypeTest {
				std::string identifier;
				LibMCData::eCustomDataType type;
			};
			
			std::vector<DataTypeTest> tests = {
				{"type.toolpath", LibMCData::eCustomDataType::Toolpath},
				{"type.timeline", LibMCData::eCustomDataType::Timeline},
				{"type.custombinary", LibMCData::eCustomDataType::CustomBinaryData},
				{"type.pngimage", LibMCData::eCustomDataType::PNGImage},
				{"type.jpegimage", LibMCData::eCustomDataType::JPEGImage},
				{"type.thumbnail", LibMCData::eCustomDataType::Thumbnail}
			};
			
			for (const auto& test : tests) {
				std::string sStreamUUID = createTestStream(fixture, "Content for " + test.identifier);
				auto pStream = fixture.m_pStorage->RetrieveStream(sStreamUUID);
				pJob->AddJobData(test.identifier, "Type Test", pStream.get(), test.type, fixture.m_sUserUUID, getCurrentTimestamp());
				
				auto pCustomData = pJob->RetrieveJobDataByIdentifier(test.identifier);
				assertTrue(pCustomData->GetDataType() == test.type, "Data type should match for " + test.identifier);
			}
		}

		void testGetDataTypeAsString()
		{
			auto fixture = createFixture("data_type_str");
			
			auto pJob = createTestJob(fixture, "Job For Data Type String");
			
			struct DataTypeStringTest {
				std::string identifier;
				LibMCData::eCustomDataType type;
				std::string expectedString;
			};
			
			std::vector<DataTypeStringTest> tests = {
				{"typestr.toolpath", LibMCData::eCustomDataType::Toolpath, "toolpath"},
				{"typestr.timeline", LibMCData::eCustomDataType::Timeline, "timeline"},
				{"typestr.custombinary", LibMCData::eCustomDataType::CustomBinaryData, "custombinarydata"},
				{"typestr.pngimage", LibMCData::eCustomDataType::PNGImage, "pngimage"},
				{"typestr.jpegimage", LibMCData::eCustomDataType::JPEGImage, "jpegimage"},
				{"typestr.thumbnail", LibMCData::eCustomDataType::Thumbnail, "thumbnail"}
			};
			
			for (const auto& test : tests) {
				std::string sStreamUUID = createTestStream(fixture, "Content for " + test.identifier);
				auto pStream = fixture.m_pStorage->RetrieveStream(sStreamUUID);
				pJob->AddJobData(test.identifier, "Type String Test", pStream.get(), test.type, fixture.m_sUserUUID, getCurrentTimestamp());
				
				auto pCustomData = pJob->RetrieveJobDataByIdentifier(test.identifier);
				assertTrue(pCustomData->GetDataTypeAsString() == test.expectedString, "Data type string should be '" + test.expectedString + "' for " + test.identifier);
			}
		}
	};

}

#endif // __AMCTEST_UNITTEST_LIBMCDATA_CUSTOMDATA
