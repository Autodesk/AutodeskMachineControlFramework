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

#ifndef __AMCTEST_UNITTEST_ACCESSROLE
#define __AMCTEST_UNITTEST_ACCESSROLE

#include "amc_unittests.hpp"
#include "amc_accessrole.hpp"
#include "amc_accesspermission.hpp"

namespace AMCUnitTest {

	class CUnitTestGroup_AccessRole : public CUnitTestGroup {
	public:

		std::string getTestGroupName() override {
			return "AccessRole";
		}

		void registerTests() override {
			registerTest("CreateValidRole", "Creates a role with valid identifier", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_AccessRole::testCreateValidRole, this));
			registerTest("CreateInvalidRoleThrows", "Creating a role with invalid ID throws", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_AccessRole::testCreateInvalidRoleThrows, this));
			registerTest("AddPermissionToRole", "Add a permission and verify it's present", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_AccessRole::testAddPermissionToRole, this));
			registerTest("DuplicatePermissionThrows", "Adding a permission twice should throw", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_AccessRole::testDuplicatePermissionThrows, this));
			registerTest("RemovePermission", "Removes a permission from the role", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_AccessRole::testRemovePermission, this));
			registerTest("GetPermissionsList", "Returns all assigned permissions", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_AccessRole::testGetPermissionsList, this));
			registerTest("AddNullPermissionThrows", "Adding a null permission throws", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_AccessRole::testAddNullPermissionThrows, this));
		}

		void initializeTests() override {
			// No special setup required
		}

	private:

		AMC::PAccessPermission makePermission(const std::string& id, const std::string& name = "Display", const std::string& desc = "Description") {
			return std::make_shared<AMC::CAccessPermission>(id, AMC::CLanguageString("", name), AMC::CLanguageString("", desc));
		}

		void testCreateValidRole() {
			AMC::CAccessRole role("admin", AMC::CLanguageString("", "Administrator"), AMC::CLanguageString("", "Manages everything"));
			assertTrue(role.getIdentifier() == "admin");
			assertTrue(role.getDisplayName().getCustomValue() == "Administrator");
			assertTrue(role.getDescription().getCustomValue() == "Manages everything");
		}

		void testCreateInvalidRoleThrows() {
			bool thrown = false;
			try {
				AMC::CAccessRole role("invalid id!", AMC::CLanguageString("", "X"), AMC::CLanguageString("", "Y"));
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected exception on invalid identifier");
		}

		void testAddPermissionToRole() {
			AMC::CAccessRole role("user", AMC::CLanguageString("", "User"), AMC::CLanguageString("", "Standard user"));
			auto perm = makePermission("read");

			role.addPermission(perm);
			assertTrue(role.hasPermission("read"), "Permission not found after adding");
		}

		void testDuplicatePermissionThrows() {
			AMC::CAccessRole role("user", AMC::CLanguageString("", "User"), AMC::CLanguageString("", "Standard user"));
			auto perm = makePermission("read");

			role.addPermission(perm);

			bool thrown = false;
			try {
				role.addPermission(perm);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Adding duplicate permission should throw");
		}

		void testRemovePermission() {
			AMC::CAccessRole role("user", AMC::CLanguageString("", "User"), AMC::CLanguageString("", "Standard user"));
			auto perm = makePermission("read");

			role.addPermission(perm);
			assertTrue(role.hasPermission("read"));

			role.removePermission("read");
			assertFalse(role.hasPermission("read"));
		}

		void testGetPermissionsList() {
			AMC::CAccessRole role("user", AMC::CLanguageString("", "User"), AMC::CLanguageString("", "Standard user"));
			role.addPermission(makePermission("read"));
			role.addPermission(makePermission("write"));

			auto perms = role.getPermissions();
			assertTrue(perms.size() == 2, "Expected two permissions in list");

			bool foundRead = false;
			bool foundWrite = false;

			for (auto p : perms) {
				if (p->getIdentifier() == "read") foundRead = true;
				if (p->getIdentifier() == "write") foundWrite = true;
			}

			assertTrue(foundRead && foundWrite, "Missing expected permissions");
		}

		void testAddNullPermissionThrows() {
			AMC::CAccessRole role("user", AMC::CLanguageString("", "User"), AMC::CLanguageString("", "Standard user"));

			bool thrown = false;
			try {
				role.addPermission(nullptr);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Adding null permission should throw");
		}

	};

}


#endif // __AMCTEST_UNITTEST_ACCESSROLE