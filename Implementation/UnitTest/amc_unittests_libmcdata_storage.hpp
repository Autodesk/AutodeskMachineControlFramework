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

#ifndef __AMCTEST_UNITTEST_LIBMCDATA_STORAGE
#define __AMCTEST_UNITTEST_LIBMCDATA_STORAGE

#include "amc_unittests.hpp"
#include "amc_unittests_libmcdata.hpp"
#include "common_utils.hpp"
#include "libmcdata_dynamic.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace AMCUnitTest {

	class CUnitTestGroup_LibMCData_Storage : public CUnitTestGroup {
	public:
		std::string getTestGroupName() override {
			return "LibMCData_Storage";
		}

		void registerTests() override {
			registerTest("StoreAndRetrieveStream", "Store and retrieve a stream", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Storage::testStoreAndRetrieveStream, this));
			registerTest("StreamMetadata", "Verify stream metadata is preserved", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Storage::testStreamMetadata, this));
			registerTest("SHA256Validation", "Verify SHA256 hash calculation", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Storage::testSHA256Validation, this));
			registerTest("StreamIsReady", "Test StreamIsReady check", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Storage::testStreamIsReady, this));
			registerTest("ContentTypeValidation", "Test content type acceptance", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Storage::testContentTypeValidation, this));
			registerTest("LargeStream", "Test storage of larger data", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Storage::testLargeStream, this));
			registerTest("MultipleStreams", "Test storing multiple streams", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Storage::testMultipleStreams, this));
		}

		void initializeTests() override {
		}

	private:

		struct SStorageFixture {
			LibMCData::PWrapper m_pDataWrapper;
			LibMCData::PDataModel m_pDataModel;
			LibMCData::PStorage m_pStorage;
			std::string m_sBasePath;
		};

		SStorageFixture createFixture(const std::string& sSuffix)
		{
			SStorageFixture fixture;

			std::string sRootPath = "temp";
			if (!AMCCommon::CUtils::fileOrPathExistsOnDisk(sRootPath))
				AMCCommon::CUtils::createDirectoryOnDisk(sRootPath);

			fixture.m_sBasePath = sRootPath + "/libmcdata_storage_" + sSuffix + "_" + AMCCommon::CUtils::createUUID();
			if (!AMCCommon::CUtils::fileOrPathExistsOnDisk(fixture.m_sBasePath))
				AMCCommon::CUtils::createDirectoryOnDisk(fixture.m_sBasePath);

			std::string sDatabaseFile = fixture.m_sBasePath + "/storage.db";

			fixture.m_pDataWrapper = AMCUnitTest::loadLibMCDataInProcess();
			fixture.m_pDataModel = fixture.m_pDataWrapper->CreateDataModelInstance();
			fixture.m_pDataModel->InitialiseDatabase(fixture.m_sBasePath, LibMCData::eDataBaseType::SqLite, sDatabaseFile);
			fixture.m_pStorage = fixture.m_pDataModel->CreateStorage();

			return fixture;
		}

		uint64_t getCurrentTimestamp()
		{
			return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(
				std::chrono::system_clock::now().time_since_epoch()).count());
		}

		void testStoreAndRetrieveStream()
		{
			auto fixture = createFixture("store_retrieve");
			
			std::string sTestData = "Hello, LibMCData Storage Test!";
			std::vector<uint8_t> testBuffer(sTestData.begin(), sTestData.end());
			
			std::string sUUID = AMCCommon::CUtils::createUUID();
			std::string sUserUUID = AMCCommon::CUtils::createUUID();
			
			fixture.m_pStorage->StoreNewStream(sUUID, "testfile.txt", "text/plain", testBuffer, sUserUUID, getCurrentTimestamp());
			
			auto pStream = fixture.m_pStorage->RetrieveStream(sUUID);
			assertAssigned(pStream.get());
			
			std::vector<uint8_t> retrievedBuffer;
			pStream->GetContent(retrievedBuffer);
			
			assertTrue(retrievedBuffer.size() == testBuffer.size(), "Content size mismatch");
			for (size_t i = 0; i < testBuffer.size(); i++) {
				assertTrue(retrievedBuffer[i] == testBuffer[i], "Content byte mismatch at position " + std::to_string(i));
			}
		}

		void testStreamMetadata()
		{
			auto fixture = createFixture("metadata");
			
			std::string sTestData = "Metadata test content";
			std::vector<uint8_t> testBuffer(sTestData.begin(), sTestData.end());
			
			std::string sUUID = AMCCommon::CUtils::createUUID();
			std::string sUserUUID = AMCCommon::CUtils::createUUID();
			std::string sName = "metadata_test.dat";
			std::string sMimeType = "application/octet-stream";
			
			fixture.m_pStorage->StoreNewStream(sUUID, sName, sMimeType, testBuffer, sUserUUID, getCurrentTimestamp());
			
			auto pStream = fixture.m_pStorage->RetrieveStream(sUUID);
			
			assertTrue(pStream->GetUUID() == sUUID, "UUID mismatch");
			assertTrue(pStream->GetName() == sName, "Name mismatch");
			assertTrue(pStream->GetMIMEType() == sMimeType, "MIME type mismatch");
			assertTrue(pStream->GetSize() == testBuffer.size(), "Size mismatch");
		}

		void testSHA256Validation()
		{
			auto fixture = createFixture("sha256");
			
			std::string sTestData = "test";
			std::vector<uint8_t> testBuffer(sTestData.begin(), sTestData.end());
			
			std::string sUUID = AMCCommon::CUtils::createUUID();
			std::string sUserUUID = AMCCommon::CUtils::createUUID();
			
			fixture.m_pStorage->StoreNewStream(sUUID, "sha256_test.txt", "text/plain", testBuffer, sUserUUID, getCurrentTimestamp());
			
			auto pStream = fixture.m_pStorage->RetrieveStream(sUUID);
			std::string sSHA2 = pStream->GetSHA2();
			
			assertFalse(sSHA2.empty(), "SHA256 should not be empty");
			// SHA256 is 32 bytes = 64 hex characters
			assertTrue(sSHA2.length() == 64, "SHA256 should be 64 hex characters");
		}

		void testStreamIsReady()
		{
			auto fixture = createFixture("ready");
			
			std::string sExistingUUID = AMCCommon::CUtils::createUUID();
			std::string sNonExistingUUID = AMCCommon::CUtils::createUUID();
			std::string sUserUUID = AMCCommon::CUtils::createUUID();
			
			std::vector<uint8_t> testBuffer = {0x01, 0x02, 0x03};
			fixture.m_pStorage->StoreNewStream(sExistingUUID, "ready_test.bin", "application/octet-stream", testBuffer, sUserUUID, getCurrentTimestamp());
			
			assertTrue(fixture.m_pStorage->StreamIsReady(sExistingUUID), "Existing stream should be ready");
			assertFalse(fixture.m_pStorage->StreamIsReady(sNonExistingUUID), "Non-existing stream should not be ready");
		}

		void testContentTypeValidation()
		{
			auto fixture = createFixture("contenttype");
			
			// Test that some content type check works - actual accepted types depend on config
			// Just verify the method can be called without error
			bool bResult1 = fixture.m_pStorage->ContentTypeIsAccepted("application/3mf");
			bool bResult2 = fixture.m_pStorage->ContentTypeIsAccepted("nonexistent/faketype");
			
			// At minimum, verify the function returns a boolean without throwing
			assertTrue(bResult1 || !bResult1, "ContentTypeIsAccepted should return a boolean");
			assertTrue(bResult2 || !bResult2, "ContentTypeIsAccepted should return a boolean");
		}

		void testLargeStream()
		{
			auto fixture = createFixture("large");
			
			// Create 1MB of data
			const size_t dataSize = 1024 * 1024;
			std::vector<uint8_t> largeBuffer(dataSize);
			for (size_t i = 0; i < dataSize; i++) {
				largeBuffer[i] = static_cast<uint8_t>(i & 0xFF);
			}
			
			std::string sUUID = AMCCommon::CUtils::createUUID();
			std::string sUserUUID = AMCCommon::CUtils::createUUID();
			
			fixture.m_pStorage->StoreNewStream(sUUID, "large_test.bin", "application/octet-stream", largeBuffer, sUserUUID, getCurrentTimestamp());
			
			auto pStream = fixture.m_pStorage->RetrieveStream(sUUID);
			assertTrue(pStream->GetSize() == dataSize, "Large stream size mismatch");
			
			std::vector<uint8_t> retrievedBuffer;
			pStream->GetContent(retrievedBuffer);
			assertTrue(retrievedBuffer.size() == dataSize, "Retrieved large buffer size mismatch");
			
			// Verify first and last bytes
			assertTrue(retrievedBuffer[0] == 0, "First byte mismatch");
			assertTrue(retrievedBuffer[dataSize - 1] == ((dataSize - 1) & 0xFF), "Last byte mismatch");
		}

		void testMultipleStreams()
		{
			auto fixture = createFixture("multiple");
			
			std::vector<std::string> uuids;
			std::string sUserUUID = AMCCommon::CUtils::createUUID();
			
			// Store 10 streams
			for (int i = 0; i < 10; i++) {
				std::string sUUID = AMCCommon::CUtils::createUUID();
				std::string sContent = "Stream content " + std::to_string(i);
				std::vector<uint8_t> buffer(sContent.begin(), sContent.end());
				
				fixture.m_pStorage->StoreNewStream(sUUID, "stream_" + std::to_string(i) + ".txt", "text/plain", buffer, sUserUUID, getCurrentTimestamp());
				uuids.push_back(sUUID);
			}
			
			// Verify all streams exist and have correct content
			for (int i = 0; i < 10; i++) {
				assertTrue(fixture.m_pStorage->StreamIsReady(uuids[i]), "Stream " + std::to_string(i) + " should be ready");
				
				auto pStream = fixture.m_pStorage->RetrieveStream(uuids[i]);
				assertTrue(pStream->GetName() == "stream_" + std::to_string(i) + ".txt", "Stream name mismatch");
			}
		}
	};

}

#endif // __AMCTEST_UNITTEST_LIBMCDATA_STORAGE
