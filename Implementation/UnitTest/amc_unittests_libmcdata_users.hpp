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

#ifndef __AMCTEST_UNITTEST_LIBMCDATA_USERS
#define __AMCTEST_UNITTEST_LIBMCDATA_USERS

#include "amc_unittests.hpp"
#include "amc_unittests_libmcdata.hpp"
#include "common_utils.hpp"
#include "libmcdata_dynamic.hpp"

#include <cstdint>
#include <string>

namespace AMCUnitTest {

	class CUnitTestGroup_LibMCData_Users : public CUnitTestGroup {
	public:
		std::string getTestGroupName() override {
			return "LibMCData_Users";
		}

		void registerTests() override {
			registerTest("CreateUser", "Create user", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Users::testCreateUser, this));
			registerTest("UserExists", "Check user existence", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Users::testUserExists, this));
			registerTest("UserProperties", "Get user properties", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Users::testUserProperties, this));
			registerTest("SetLanguage", "Set user language", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Users::testSetLanguage, this));
			registerTest("SetRole", "Set user role", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Users::testSetRole, this));
			registerTest("SetDescription", "Set user description", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Users::testSetDescription, this));
			registerTest("GetActiveUsers", "Get active users list", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Users::testGetActiveUsers, this));
		}

		void initializeTests() override {
		}

	private:

		struct SUsersFixture {
			LibMCData::PWrapper m_pDataWrapper;
			LibMCData::PDataModel m_pDataModel;
			LibMCData::PLoginHandler m_pLoginHandler;
			std::string m_sBasePath;
		};

		SUsersFixture createFixture(const std::string& sSuffix)
		{
			SUsersFixture fixture;

			std::string sRootPath = "temp";
			if (!AMCCommon::CUtils::fileOrPathExistsOnDisk(sRootPath))
				AMCCommon::CUtils::createDirectoryOnDisk(sRootPath);

			fixture.m_sBasePath = sRootPath + "/libmcdata_users_" + sSuffix + "_" + AMCCommon::CUtils::createUUID();
			if (!AMCCommon::CUtils::fileOrPathExistsOnDisk(fixture.m_sBasePath))
				AMCCommon::CUtils::createDirectoryOnDisk(fixture.m_sBasePath);

			std::string sDatabaseFile = fixture.m_sBasePath + "/users.db";

			fixture.m_pDataWrapper = AMCUnitTest::loadLibMCDataInProcess();
			fixture.m_pDataModel = fixture.m_pDataWrapper->CreateDataModelInstance();
			fixture.m_pDataModel->InitialiseDatabase(fixture.m_sBasePath, LibMCData::eDataBaseType::SqLite, sDatabaseFile);
			fixture.m_pLoginHandler = fixture.m_pDataModel->CreateLoginHandler();

			return fixture;
		}

		void testCreateUser()
		{
			auto fixture = createFixture("create");
			
			std::string sUsername = "testuser_" + AMCCommon::CUtils::createUUID().substr(0, 8);
			// Salt and password must be valid SHA256 hex strings (64 hex characters)
			std::string sSalt = "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
			std::string sHashedPassword = "fedcba9876543210fedcba9876543210fedcba9876543210fedcba9876543210";
			std::string sRole = "operator";
			std::string sDescription = "Test user description";
			
			// CreateUser(sUsername, sRole, sSalt, sHashedPassword, sDescription) returns UUID
			std::string sUserUUID = fixture.m_pLoginHandler->CreateUser(sUsername, sRole, sSalt, sHashedPassword, sDescription);
			
			assertFalse(sUserUUID.empty(), "User UUID should not be empty");
			assertTrue(fixture.m_pLoginHandler->UserExists(sUsername), "User should exist");
		}

		void testUserExists()
		{
			auto fixture = createFixture("exists");
			
			std::string sUsername = "existsuser_" + AMCCommon::CUtils::createUUID().substr(0, 8);
			std::string sSalt = "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
			std::string sHash = "fedcba9876543210fedcba9876543210fedcba9876543210fedcba9876543210";
			
			assertFalse(fixture.m_pLoginHandler->UserExists(sUsername), "User should not exist initially");
			
			std::string sUserUUID = fixture.m_pLoginHandler->CreateUser(sUsername, "role", sSalt, sHash, "desc");
			
			assertTrue(fixture.m_pLoginHandler->UserExists(sUsername), "User should exist after creation");
			assertTrue(fixture.m_pLoginHandler->UserUUIDExists(sUserUUID), "User UUID should exist");
		}

		void testUserProperties()
		{
			auto fixture = createFixture("properties");
			
			std::string sUsername = "propsuser_" + AMCCommon::CUtils::createUUID().substr(0, 8);
			std::string sRole = "admin";
			std::string sDescription = "Admin user";
			std::string sSalt = "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
			std::string sHash = "fedcba9876543210fedcba9876543210fedcba9876543210fedcba9876543210";
			
			std::string sUserUUID = fixture.m_pLoginHandler->CreateUser(sUsername, sRole, sSalt, sHash, sDescription);
			
			// Get properties by username
			assertTrue(fixture.m_pLoginHandler->GetUserRole(sUsername) == sRole, "Role mismatch");
			assertTrue(fixture.m_pLoginHandler->GetUserDescription(sUsername) == sDescription, "Description mismatch");
			
			// Get UUID by username
			assertTrue(fixture.m_pLoginHandler->GetUserUUID(sUsername) == sUserUUID, "UUID mismatch");
			
			// Get username by UUID
			assertTrue(fixture.m_pLoginHandler->GetUsernameByUUID(sUserUUID) == sUsername, "Username by UUID mismatch");
		}

		void testSetLanguage()
		{
			auto fixture = createFixture("language");
			
			std::string sUsername = "languser_" + AMCCommon::CUtils::createUUID().substr(0, 8);
			std::string sSalt = "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
			std::string sHash = "fedcba9876543210fedcba9876543210fedcba9876543210fedcba9876543210";
			
			std::string sUserUUID = fixture.m_pLoginHandler->CreateUser(sUsername, "role", sSalt, sHash, "desc");
			
			fixture.m_pLoginHandler->SetUserLanguage(sUsername, "en");
			assertTrue(fixture.m_pLoginHandler->GetUserLanguage(sUsername) == "en", "Initial language mismatch");
			
			fixture.m_pLoginHandler->SetUserLanguage(sUsername, "fr");
			assertTrue(fixture.m_pLoginHandler->GetUserLanguage(sUsername) == "fr", "Updated language mismatch");
			
			// Also test by UUID
			fixture.m_pLoginHandler->SetUserLanguageByUUID(sUserUUID, "es");
			assertTrue(fixture.m_pLoginHandler->GetUserLanguageByUUID(sUserUUID) == "es", "Language by UUID mismatch");
		}

		void testSetRole()
		{
			auto fixture = createFixture("role");
			
			std::string sUsername = "roleuser_" + AMCCommon::CUtils::createUUID().substr(0, 8);
			std::string sSalt = "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
			std::string sHash = "fedcba9876543210fedcba9876543210fedcba9876543210fedcba9876543210";
			
			std::string sUserUUID = fixture.m_pLoginHandler->CreateUser(sUsername, "operator", sSalt, sHash, "desc");
			
			assertTrue(fixture.m_pLoginHandler->GetUserRole(sUsername) == "operator", "Initial role mismatch");
			
			fixture.m_pLoginHandler->SetUserRole(sUsername, "admin");
			assertTrue(fixture.m_pLoginHandler->GetUserRole(sUsername) == "admin", "Updated role mismatch");
			
			// Also test by UUID
			fixture.m_pLoginHandler->SetUserRoleByUUID(sUserUUID, "superadmin");
			assertTrue(fixture.m_pLoginHandler->GetUserRoleByUUID(sUserUUID) == "superadmin", "Role by UUID mismatch");
		}

		void testSetDescription()
		{
			auto fixture = createFixture("description");
			
			std::string sUsername = "descuser_" + AMCCommon::CUtils::createUUID().substr(0, 8);
			std::string sSalt = "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
			std::string sHash = "fedcba9876543210fedcba9876543210fedcba9876543210fedcba9876543210";
			
			fixture.m_pLoginHandler->CreateUser(sUsername, "role", sSalt, sHash, "Original description");
			
			assertTrue(fixture.m_pLoginHandler->GetUserDescription(sUsername) == "Original description", "Initial description mismatch");
			
			fixture.m_pLoginHandler->SetUserDescription(sUsername, "New description");
			assertTrue(fixture.m_pLoginHandler->GetUserDescription(sUsername) == "New description", "Updated description mismatch");
		}

		void testGetActiveUsers()
		{
			auto fixture = createFixture("active");
			
			std::string sSalt = "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
			std::string sHash = "fedcba9876543210fedcba9876543210fedcba9876543210fedcba9876543210";
			
			// Get initial count (there might be default users)
			auto pInitialList = fixture.m_pLoginHandler->GetActiveUsers();
			uint64_t nInitialCount = pInitialList->Count();
			
			// Create multiple users
			for (int i = 0; i < 5; i++) {
				std::string sUsername = "activeuser" + std::to_string(i) + "_" + AMCCommon::CUtils::createUUID().substr(0, 8);
				fixture.m_pLoginHandler->CreateUser(sUsername, "role", sSalt, sHash, "desc");
			}
			
			auto pUserList = fixture.m_pLoginHandler->GetActiveUsers();
			assertTrue(pUserList->Count() == nInitialCount + 5, "Should have 5 more active users than initial");
		}
	};

}

#endif // __AMCTEST_UNITTEST_LIBMCDATA_USERS
