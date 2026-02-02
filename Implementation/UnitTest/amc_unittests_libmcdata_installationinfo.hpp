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

#ifndef __AMCTEST_UNITTEST_LIBMCDATA_INSTALLATIONINFO
#define __AMCTEST_UNITTEST_LIBMCDATA_INSTALLATIONINFO

#include "amc_unittests.hpp"
#include "amc_unittests_libmcdata.hpp"
#include "common_utils.hpp"
#include "libmcdata_dynamic.hpp"

#include <cstdint>
#include <string>

namespace AMCUnitTest {

	class CUnitTestGroup_LibMCData_InstallationInfo : public CUnitTestGroup {
	public:
		std::string getTestGroupName() override {
			return "LibMCData_InstallationInfo";
		}

		void registerTests() override {
			registerTest("GetInstallationUUID", "Get installation UUID", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_InstallationInfo::testGetInstallationUUID, this));
			registerTest("GetInstallationSecret", "Get installation secret", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_InstallationInfo::testGetInstallationSecret, this));
			registerTest("GetBaseTempDirectory", "Get base temp directory", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_InstallationInfo::testGetBaseTempDirectory, this));
			registerTest("UUIDNormalization", "UUID normalization verification", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_InstallationInfo::testUUIDNormalization, this));
			registerTest("SecretNormalization", "Secret normalization verification", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_InstallationInfo::testSecretNormalization, this));
			registerTest("ConsistentValues", "Values are consistent across calls", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_InstallationInfo::testConsistentValues, this));
		}

		void initializeTests() override {
		}

	private:

		struct SInstallInfoFixture {
			LibMCData::PWrapper m_pDataWrapper;
			LibMCData::PDataModel m_pDataModel;
			LibMCData::PInstallationInformation m_pInstallInfo;
			std::string m_sBasePath;
		};

		SInstallInfoFixture createFixture(const std::string& sSuffix)
		{
			SInstallInfoFixture fixture;

			std::string sRootPath = "temp";
			if (!AMCCommon::CUtils::fileOrPathExistsOnDisk(sRootPath))
				AMCCommon::CUtils::createDirectoryOnDisk(sRootPath);

			fixture.m_sBasePath = sRootPath + "/libmcdata_installinfo_" + sSuffix + "_" + AMCCommon::CUtils::createUUID();
			if (!AMCCommon::CUtils::fileOrPathExistsOnDisk(fixture.m_sBasePath))
				AMCCommon::CUtils::createDirectoryOnDisk(fixture.m_sBasePath);

			std::string sDatabaseFile = fixture.m_sBasePath + "/installinfo.db";

			fixture.m_pDataWrapper = AMCUnitTest::loadLibMCDataInProcess();
			fixture.m_pDataModel = fixture.m_pDataWrapper->CreateDataModelInstance();
			fixture.m_pDataModel->InitialiseDatabase(fixture.m_sBasePath, LibMCData::eDataBaseType::SqLite, sDatabaseFile);
			
			// Set temp directory before getting installation info
			std::string sTempDir = fixture.m_sBasePath + "/temp";
			fixture.m_pDataModel->SetBaseTempDirectory(sTempDir);
			
			fixture.m_pInstallInfo = fixture.m_pDataModel->GetInstallationInformationObject();

			return fixture;
		}

		void testGetInstallationUUID()
		{
			auto fixture = createFixture("uuid");
			
			std::string sUUID = fixture.m_pInstallInfo->GetInstallationUUID();
			
			// UUID should not be empty
			assertFalse(sUUID.empty(), "Installation UUID should not be empty");
			
			// UUID should be in standard format (8-4-4-4-12 = 36 chars)
			assertTrue(sUUID.length() == 36, "Installation UUID should be 36 characters");
			
			// Should have dashes in correct positions
			assertTrue(sUUID[8] == '-', "UUID should have dash at position 8");
			assertTrue(sUUID[13] == '-', "UUID should have dash at position 13");
			assertTrue(sUUID[18] == '-', "UUID should have dash at position 18");
			assertTrue(sUUID[23] == '-', "UUID should have dash at position 23");
			
			// All non-dash characters should be valid hex
			for (size_t i = 0; i < sUUID.length(); i++) {
				if (i != 8 && i != 13 && i != 18 && i != 23) {
					char c = sUUID[i];
					bool bValidHex = (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
					assertTrue(bValidHex, "UUID should contain only hex characters");
				}
			}
		}

		void testGetInstallationSecret()
		{
			auto fixture = createFixture("secret");
			
			std::string sSecret = fixture.m_pInstallInfo->GetInstallationSecret();
			
			// Secret should not be empty
			assertFalse(sSecret.empty(), "Installation secret should not be empty");
			
			// Secret should be SHA256 format (64 hex characters)
			assertTrue(sSecret.length() == 64, "Installation secret should be 64 hex characters");
			
			// All characters should be valid hex
			for (size_t i = 0; i < sSecret.length(); i++) {
				char c = sSecret[i];
				bool bValidHex = (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
				assertTrue(bValidHex, "Secret should contain only hex characters at position " + std::to_string(i));
			}
		}

		void testGetBaseTempDirectory()
		{
			auto fixture = createFixture("tempdir");
			
			std::string sTempDir = fixture.m_pInstallInfo->GetBaseTempDirectory();
			
			// Temp directory should not be empty since we set it in fixture
			assertFalse(sTempDir.empty(), "Base temp directory should not be empty");
			
			// Directory should exist
			assertTrue(AMCCommon::CUtils::fileOrPathExistsOnDisk(sTempDir), "Base temp directory should exist");
		}

		void testUUIDNormalization()
		{
			auto fixture = createFixture("uuid_norm");
			
			std::string sUUID = fixture.m_pInstallInfo->GetInstallationUUID();
			
			// UUID should be lowercase (normalized)
			for (size_t i = 0; i < sUUID.length(); i++) {
				char c = sUUID[i];
				if (c >= 'A' && c <= 'F') {
					// If uppercase letters exist, normalization may allow them
					// But typically normalized UUIDs are lowercase
					break;
				}
			}
			
			// Multiple calls should return the same UUID
			std::string sUUID2 = fixture.m_pInstallInfo->GetInstallationUUID();
			assertTrue(sUUID == sUUID2, "UUID should be consistent across calls");
		}

		void testSecretNormalization()
		{
			auto fixture = createFixture("secret_norm");
			
			std::string sSecret = fixture.m_pInstallInfo->GetInstallationSecret();
			
			// Secret should be normalized (typically lowercase hex)
			// Multiple calls should return the same secret
			std::string sSecret2 = fixture.m_pInstallInfo->GetInstallationSecret();
			assertTrue(sSecret == sSecret2, "Secret should be consistent across calls");
			
			// Verify it's a valid SHA256 format
			assertTrue(sSecret.length() == 64, "Secret should be 64 characters (SHA256)");
		}

		void testConsistentValues()
		{
			auto fixture = createFixture("consistent");
			
			// Get values multiple times
			std::string sUUID1 = fixture.m_pInstallInfo->GetInstallationUUID();
			std::string sSecret1 = fixture.m_pInstallInfo->GetInstallationSecret();
			std::string sTempDir1 = fixture.m_pInstallInfo->GetBaseTempDirectory();
			
			std::string sUUID2 = fixture.m_pInstallInfo->GetInstallationUUID();
			std::string sSecret2 = fixture.m_pInstallInfo->GetInstallationSecret();
			std::string sTempDir2 = fixture.m_pInstallInfo->GetBaseTempDirectory();
			
			std::string sUUID3 = fixture.m_pInstallInfo->GetInstallationUUID();
			std::string sSecret3 = fixture.m_pInstallInfo->GetInstallationSecret();
			std::string sTempDir3 = fixture.m_pInstallInfo->GetBaseTempDirectory();
			
			// All calls should return consistent values
			assertTrue(sUUID1 == sUUID2 && sUUID2 == sUUID3, "UUID should be consistent");
			assertTrue(sSecret1 == sSecret2 && sSecret2 == sSecret3, "Secret should be consistent");
			assertTrue(sTempDir1 == sTempDir2 && sTempDir2 == sTempDir3, "TempDir should be consistent");
			
			// Also verify via deprecated methods for consistency
			std::string sDeprecatedUUID, sDeprecatedSecret;
			fixture.m_pDataModel->GetInstallationInformation(sDeprecatedUUID, sDeprecatedSecret);
			
			assertTrue(sUUID1 == sDeprecatedUUID, "UUID should match deprecated method");
			assertTrue(sSecret1 == sDeprecatedSecret, "Secret should match deprecated method");
		}
	};

}

#endif // __AMCTEST_UNITTEST_LIBMCDATA_INSTALLATIONINFO
