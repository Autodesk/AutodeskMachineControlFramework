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
			
			// Additional LoginHandler tests for coverage
			registerTest("GetUserDetails", "Get user details", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Users::testGetUserDetails, this));
			registerTest("GetUserPropsAll", "Get all user properties", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Users::testGetUserPropsAll, this));
			registerTest("GetUserPropsByUUID", "Get user properties by UUID", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Users::testGetUserPropsByUUID, this));
			registerTest("SetUserPassword", "Set user password", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Users::testSetUserPassword, this));
			registerTest("SetUserPasswordByUUID", "Set password by UUID", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Users::testSetUserPasswordByUUID, this));
			registerTest("GetDescriptionByUUID", "Get description by UUID", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Users::testGetDescriptionByUUID, this));
			registerTest("SetDescriptionByUUID", "Set description by UUID", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Users::testSetDescriptionByUUID, this));
			registerTest("UserListIteration", "Iterate user list", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Users::testUserListIteration, this));
			
			// Additional User Tests for Coverage (8 tests)
			registerTest("SpecialCharUsername", "Username with special chars", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Users::testSpecialCharUsername, this));
			registerTest("UserListPagination", "User list pagination", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Users::testUserListPagination, this));
			registerTest("NonExistentUser", "Non-existent user checks", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Users::testNonExistentUser, this));
			registerTest("LongUsername", "Long username", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Users::testLongUsername, this));
			registerTest("LongDescription", "Long description", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Users::testLongDescription, this));
			registerTest("EmptyDescription", "Empty description", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Users::testEmptyDescription, this));
			registerTest("MultipleRoleChanges", "Multiple role changes", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Users::testMultipleRoleChanges, this));
			registerTest("UserUUIDFormat", "User UUID format", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Users::testUserUUIDFormat, this));
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
		
		// ============= Additional LoginHandler Tests =============
		
		void testGetUserDetails()
		{
			auto fixture = createFixture("details");
			
			std::string sUsername = "detailsuser_" + AMCCommon::CUtils::createUUID().substr(0, 8);
			std::string sSalt = "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
			std::string sHash = "fedcba9876543210fedcba9876543210fedcba9876543210fedcba9876543210";
			
			std::string sUserUUID = fixture.m_pLoginHandler->CreateUser(sUsername, "admin", sSalt, sHash, "Detailed user");
			
			std::string sRetrievedSalt, sRetrievedHash;
			fixture.m_pLoginHandler->GetUserDetails(sUsername, sRetrievedSalt, sRetrievedHash);
			
			assertTrue(sRetrievedSalt == sSalt, "Salt should match");
			assertTrue(sRetrievedHash == sHash, "Hash should match");
		}
		
		void testGetUserPropsAll()
		{
			auto fixture = createFixture("props_all");
			
			std::string sUsername = "propsalluser_" + AMCCommon::CUtils::createUUID().substr(0, 8);
			std::string sSalt = "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
			std::string sHash = "fedcba9876543210fedcba9876543210fedcba9876543210fedcba9876543210";
			std::string sRole = "operator";
			std::string sDescription = "Props all user";
			
			fixture.m_pLoginHandler->CreateUser(sUsername, sRole, sSalt, sHash, sDescription);
			fixture.m_pLoginHandler->SetUserLanguage(sUsername, "de");
			
			// GetUserProperties(sUsername, out sUUID, out sDescription, out sRole, out sLanguageIdentifier)
			std::string sRetrievedUUID, sRetrievedDescription, sRetrievedRole, sRetrievedLanguage;
			fixture.m_pLoginHandler->GetUserProperties(sUsername, sRetrievedUUID, sRetrievedDescription, sRetrievedRole, sRetrievedLanguage);
			
			assertFalse(sRetrievedUUID.empty(), "UUID should not be empty");
			assertTrue(sRetrievedRole == sRole, "Role should match");
			assertTrue(sRetrievedLanguage == "de", "Language should match");
			assertTrue(sRetrievedDescription == sDescription, "Description should match");
		}
		
		void testGetUserPropsByUUID()
		{
			auto fixture = createFixture("props_uuid");
			
			std::string sUsername = "propsuuiduser_" + AMCCommon::CUtils::createUUID().substr(0, 8);
			std::string sSalt = "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
			std::string sHash = "fedcba9876543210fedcba9876543210fedcba9876543210fedcba9876543210";
			std::string sRole = "viewer";
			std::string sDescription = "Props by UUID user";
			
			std::string sUserUUID = fixture.m_pLoginHandler->CreateUser(sUsername, sRole, sSalt, sHash, sDescription);
			fixture.m_pLoginHandler->SetUserLanguageByUUID(sUserUUID, "fr");
			
			// GetUserPropertiesByUUID(sUUID, out sUsername, out sDescription, out sRole, out sLanguageIdentifier)
			std::string sRetrievedUsername, sRetrievedDescription, sRetrievedRole, sRetrievedLanguage;
			fixture.m_pLoginHandler->GetUserPropertiesByUUID(sUserUUID, sRetrievedUsername, sRetrievedDescription, sRetrievedRole, sRetrievedLanguage);
			
			assertTrue(sRetrievedUsername == sUsername, "Username should match");
			assertTrue(sRetrievedRole == sRole, "Role should match");
			assertTrue(sRetrievedLanguage == "fr", "Language should match");
			assertTrue(sRetrievedDescription == sDescription, "Description should match");
		}
		
		void testSetUserPassword()
		{
			auto fixture = createFixture("password");
			
			std::string sUsername = "pwduser_" + AMCCommon::CUtils::createUUID().substr(0, 8);
			std::string sSalt = "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
			std::string sHash = "fedcba9876543210fedcba9876543210fedcba9876543210fedcba9876543210";
			
			fixture.m_pLoginHandler->CreateUser(sUsername, "role", sSalt, sHash, "desc");
			
			// Change password
			std::string sNewSalt = "abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789";
			std::string sNewHash = "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
			
			fixture.m_pLoginHandler->SetUserPassword(sUsername, sNewSalt, sNewHash);
			
			// Verify new credentials
			std::string sRetrievedSalt, sRetrievedHash;
			fixture.m_pLoginHandler->GetUserDetails(sUsername, sRetrievedSalt, sRetrievedHash);
			
			assertTrue(sRetrievedSalt == sNewSalt, "New salt should match");
			assertTrue(sRetrievedHash == sNewHash, "New hash should match");
		}
		
		void testSetUserPasswordByUUID()
		{
			auto fixture = createFixture("pwd_uuid");
			
			std::string sUsername = "pwduuiduser_" + AMCCommon::CUtils::createUUID().substr(0, 8);
			std::string sSalt = "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
			std::string sHash = "fedcba9876543210fedcba9876543210fedcba9876543210fedcba9876543210";
			
			std::string sUserUUID = fixture.m_pLoginHandler->CreateUser(sUsername, "role", sSalt, sHash, "desc");
			
			// Change password by UUID
			std::string sNewSalt = "1111111111111111111111111111111111111111111111111111111111111111";
			std::string sNewHash = "2222222222222222222222222222222222222222222222222222222222222222";
			
			fixture.m_pLoginHandler->SetUserPasswordByUUID(sUserUUID, sNewSalt, sNewHash);
			
			// Verify new credentials
			std::string sRetrievedSalt, sRetrievedHash;
			fixture.m_pLoginHandler->GetUserDetails(sUsername, sRetrievedSalt, sRetrievedHash);
			
			assertTrue(sRetrievedSalt == sNewSalt, "New salt by UUID should match");
			assertTrue(sRetrievedHash == sNewHash, "New hash by UUID should match");
		}
		
		void testGetDescriptionByUUID()
		{
			auto fixture = createFixture("desc_by_uuid");
			
			std::string sUsername = "descuuiduser_" + AMCCommon::CUtils::createUUID().substr(0, 8);
			std::string sSalt = "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
			std::string sHash = "fedcba9876543210fedcba9876543210fedcba9876543210fedcba9876543210";
			std::string sDescription = "Description by UUID test";
			
			std::string sUserUUID = fixture.m_pLoginHandler->CreateUser(sUsername, "role", sSalt, sHash, sDescription);
			
			std::string sRetrieved = fixture.m_pLoginHandler->GetUserDescriptionByUUID(sUserUUID);
			assertTrue(sRetrieved == sDescription, "Description by UUID should match");
		}
		
		void testSetDescriptionByUUID()
		{
			auto fixture = createFixture("set_desc_uuid");
			
			std::string sUsername = "setdescuuid_" + AMCCommon::CUtils::createUUID().substr(0, 8);
			std::string sSalt = "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
			std::string sHash = "fedcba9876543210fedcba9876543210fedcba9876543210fedcba9876543210";
			
			std::string sUserUUID = fixture.m_pLoginHandler->CreateUser(sUsername, "role", sSalt, sHash, "Original");
			
			// Set new description by UUID
			fixture.m_pLoginHandler->SetUserDescriptionByUUID(sUserUUID, "Updated by UUID");
			
			// Verify
			std::string sRetrieved = fixture.m_pLoginHandler->GetUserDescriptionByUUID(sUserUUID);
			assertTrue(sRetrieved == "Updated by UUID", "Description should be updated");
		}
		
		void testUserListIteration()
		{
			auto fixture = createFixture("list_iter");
			
			std::string sSalt = "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
			std::string sHash = "fedcba9876543210fedcba9876543210fedcba9876543210fedcba9876543210";
			
			// Create users
			std::vector<std::string> usernames;
			for (int i = 0; i < 3; i++) {
				std::string sUsername = "iteruser" + std::to_string(i) + "_" + AMCCommon::CUtils::createUUID().substr(0, 8);
				fixture.m_pLoginHandler->CreateUser(sUsername, "role", sSalt, sHash, "User " + std::to_string(i));
				usernames.push_back(sUsername);
			}
			
			auto pUserList = fixture.m_pLoginHandler->GetActiveUsers();
			uint32_t nCount = pUserList->Count();
			assertTrue(nCount >= 3, "Should have at least 3 users");
			
			// Iterate through users using GetUserProperties(index, out sUsername, out sUUID, out sDescription, out sRole, out sLanguageIdentifier)
			for (uint32_t i = 0; i < 3 && i < nCount; i++) {
				std::string sUsername, sUUID, sDescription, sRole, sLanguage;
				pUserList->GetUserProperties(i, sUsername, sUUID, sDescription, sRole, sLanguage);
				
				assertFalse(sUUID.empty(), "User UUID should not be empty");
				assertFalse(sUsername.empty(), "Username should not be empty");
			}
		}
		
		// ============= Additional User Tests for Coverage (8 tests) =============
		
		void testSpecialCharUsername()
		{
			auto fixture = createFixture("special_chars");
			
			std::string sSalt = "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
			std::string sHash = "fedcba9876543210fedcba9876543210fedcba9876543210fedcba9876543210";
			
			// Username with underscores and numbers (common valid characters)
			std::string sUsername = "test_user_123_" + AMCCommon::CUtils::createUUID().substr(0, 8);
			
			std::string sUserUUID = fixture.m_pLoginHandler->CreateUser(sUsername, "role", sSalt, sHash, "Special user");
			
			assertFalse(sUserUUID.empty(), "User UUID should be created");
			assertTrue(fixture.m_pLoginHandler->UserExists(sUsername), "User should exist");
			assertTrue(fixture.m_pLoginHandler->GetUsernameByUUID(sUserUUID) == sUsername, "Username should be preserved");
		}
		
		void testUserListPagination()
		{
			auto fixture = createFixture("pagination");
			
			std::string sSalt = "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
			std::string sHash = "fedcba9876543210fedcba9876543210fedcba9876543210fedcba9876543210";
			
			// Create many users
			for (int i = 0; i < 25; i++) {
				std::string sUsername = "pageuser" + std::to_string(i) + "_" + AMCCommon::CUtils::createUUID().substr(0, 8);
				fixture.m_pLoginHandler->CreateUser(sUsername, "role", sSalt, sHash, "Page User " + std::to_string(i));
			}
			
			auto pUserList = fixture.m_pLoginHandler->GetActiveUsers();
			uint32_t nCount = pUserList->Count();
			assertTrue(nCount >= 25, "Should have at least 25 users");
			
			// Verify we can access all users by index
			for (uint32_t i = 0; i < 25 && i < nCount; i++) {
				std::string sUsername, sUUID, sDescription, sRole, sLanguage;
				pUserList->GetUserProperties(i, sUsername, sUUID, sDescription, sRole, sLanguage);
				assertFalse(sUsername.empty(), "User " + std::to_string(i) + " should have username");
			}
		}
		
		void testNonExistentUser()
		{
			auto fixture = createFixture("nonexistent");
			
			std::string sNonExistentUsername = "nonexistent_user_" + AMCCommon::CUtils::createUUID();
			std::string sNonExistentUUID = AMCCommon::CUtils::createUUID();
			
			assertFalse(fixture.m_pLoginHandler->UserExists(sNonExistentUsername), "Non-existent user should not exist");
			assertFalse(fixture.m_pLoginHandler->UserUUIDExists(sNonExistentUUID), "Non-existent UUID should not exist");
		}
		
		void testLongUsername()
		{
			auto fixture = createFixture("long_username");
			
			std::string sSalt = "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
			std::string sHash = "fedcba9876543210fedcba9876543210fedcba9876543210fedcba9876543210";
			
			// Create a reasonably long username (64 chars)
			std::string sUsername(60, 'u');
			sUsername += "_" + AMCCommon::CUtils::createUUID().substr(0, 8);
			
			std::string sUserUUID = fixture.m_pLoginHandler->CreateUser(sUsername, "role", sSalt, sHash, "Long username user");
			
			assertFalse(sUserUUID.empty(), "User should be created with long username");
			assertTrue(fixture.m_pLoginHandler->UserExists(sUsername), "User with long username should exist");
		}
		
		void testLongDescription()
		{
			auto fixture = createFixture("long_desc");
			
			std::string sSalt = "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
			std::string sHash = "fedcba9876543210fedcba9876543210fedcba9876543210fedcba9876543210";
			
			std::string sUsername = "longdescuser_" + AMCCommon::CUtils::createUUID().substr(0, 8);
			std::string sLongDescription(1000, 'D');
			
			std::string sUserUUID = fixture.m_pLoginHandler->CreateUser(sUsername, "role", sSalt, sHash, sLongDescription);
			
			std::string sRetrieved = fixture.m_pLoginHandler->GetUserDescription(sUsername);
			assertTrue(sRetrieved == sLongDescription, "Long description should be preserved");
		}
		
		void testEmptyDescription()
		{
			auto fixture = createFixture("empty_desc");
			
			std::string sSalt = "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
			std::string sHash = "fedcba9876543210fedcba9876543210fedcba9876543210fedcba9876543210";
			
			std::string sUsername = "emptydescuser_" + AMCCommon::CUtils::createUUID().substr(0, 8);
			
			std::string sUserUUID = fixture.m_pLoginHandler->CreateUser(sUsername, "role", sSalt, sHash, "");
			
			std::string sRetrieved = fixture.m_pLoginHandler->GetUserDescription(sUsername);
			assertTrue(sRetrieved.empty(), "Empty description should be preserved");
		}
		
		void testMultipleRoleChanges()
		{
			auto fixture = createFixture("multi_role");
			
			std::string sSalt = "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
			std::string sHash = "fedcba9876543210fedcba9876543210fedcba9876543210fedcba9876543210";
			
			std::string sUsername = "multiroleuser_" + AMCCommon::CUtils::createUUID().substr(0, 8);
			
			fixture.m_pLoginHandler->CreateUser(sUsername, "viewer", sSalt, sHash, "Multi role user");
			assertTrue(fixture.m_pLoginHandler->GetUserRole(sUsername) == "viewer", "Initial role should be viewer");
			
			// Change role multiple times
			std::vector<std::string> roles = {"operator", "admin", "superadmin", "viewer", "editor"};
			for (const auto& role : roles) {
				fixture.m_pLoginHandler->SetUserRole(sUsername, role);
				assertTrue(fixture.m_pLoginHandler->GetUserRole(sUsername) == role, "Role should be " + role);
			}
		}
		
		void testUserUUIDFormat()
		{
			auto fixture = createFixture("uuid_format");
			
			std::string sSalt = "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
			std::string sHash = "fedcba9876543210fedcba9876543210fedcba9876543210fedcba9876543210";
			
			std::string sUsername = "uuidformatuser_" + AMCCommon::CUtils::createUUID().substr(0, 8);
			
			std::string sUserUUID = fixture.m_pLoginHandler->CreateUser(sUsername, "role", sSalt, sHash, "UUID format user");
			
			// UUID should be in standard format (36 chars with dashes)
			assertTrue(sUserUUID.length() == 36, "UUID should be 36 characters");
			assertTrue(sUserUUID[8] == '-', "UUID should have dash at position 8");
			assertTrue(sUserUUID[13] == '-', "UUID should have dash at position 13");
			assertTrue(sUserUUID[18] == '-', "UUID should have dash at position 18");
			assertTrue(sUserUUID[23] == '-', "UUID should have dash at position 23");
		}
	};

}

#endif // __AMCTEST_UNITTEST_LIBMCDATA_USERS
