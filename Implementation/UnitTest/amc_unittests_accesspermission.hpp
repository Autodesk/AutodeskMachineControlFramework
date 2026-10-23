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

#ifndef __AMCTEST_UNITTEST_ACCESSPERMISSION
#define __AMCTEST_UNITTEST_ACCESSPERMISSION

#include "amc_unittests.hpp"
#include "amc_accesspermission.hpp"

namespace AMCUnitTest {

	class CUnitTestGroup_AccessPermission : public CUnitTestGroup {
	public:

		std::string getTestGroupName() override {
			return "AccessPermission";
		}

		void registerTests() override {
			registerTest("CreateValidPermission", "Creates a permission with a valid identifier", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_AccessPermission::testCreateValidPermission, this));
			registerTest("EmptyIdentifierThrows", "Throws on empty permission identifier", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_AccessPermission::testEmptyIdentifierThrows, this));
			registerTest("InvalidIdentifierThrows", "Throws on invalid characters in permission ID", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_AccessPermission::testInvalidIdentifierThrows, this));
			registerTest("AccessorsReturnExpectedValues", "Check display name and description accessors", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_AccessPermission::testAccessorsReturnExpectedValues, this));
		}

		void initializeTests() override {
			// No special setup required
		}

	private:

		void testCreateValidPermission() {
			AMC::CAccessPermission perm("editfiles", AMC::CLanguageString("", "Edit Files"), AMC::CLanguageString("", "Allows file editing."));
			assertTrue(perm.getIdentifier() == "editfiles");
			assertTrue(perm.getDisplayName().getCustomValue() == "Edit Files");
			assertTrue(perm.getDescription().getCustomValue() == "Allows file editing.");
		}

		void testEmptyIdentifierThrows() {
			bool thrown = false;
			try {
				AMC::CAccessPermission perm("", AMC::CLanguageString("", "Invalid"), AMC::CLanguageString("", "Should throw"));
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Creating permission with empty identifier should throw");
		}

		void testInvalidIdentifierThrows() {
			bool thrown = false;
			try {
				AMC::CAccessPermission perm("bad id!", AMC::CLanguageString("", "Bad ID"), AMC::CLanguageString("", "Invalid characters"));
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Invalid identifier with special characters should throw");
		}

		void testAccessorsReturnExpectedValues() {
			AMC::CLanguageString displayName("permissions.manageaccess", "Manage Access");
			AMC::CLanguageString description("permissions.manageaccess_desc", "Grants full access control");
			AMC::CAccessPermission perm("manageaccess", displayName, description);

			assertTrue(perm.getDisplayName().getStringIdentifier() == "permissions.manageaccess");
			assertTrue(perm.getDisplayName().getCustomValue() == "Manage Access");
			assertTrue(perm.getDescription().getStringIdentifier() == "permissions.manageaccess_desc");
			assertTrue(perm.getDescription().getCustomValue() == "Grants full access control");
		}

	};

}


#endif // __AMCTEST_UNITTEST_ACCESSPERMISSION