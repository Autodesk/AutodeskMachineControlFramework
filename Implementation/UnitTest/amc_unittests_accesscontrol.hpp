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

#ifndef __AMCTEST_UNITTEST_ACCESSCONTROL
#define __AMCTEST_UNITTEST_ACCESSCONTROL


#include "amc_unittests.hpp"
#include "amc_accesscontrol.hpp"
#include "amc_accessrole.hpp"

namespace AMCUnitTest {

    class CUnitTestGroup_AccessControl : public CUnitTestGroup {
    public:

        std::string getTestGroupName() override {
            return "AccessControl";
        }

        void registerTests() override {
            registerTest("AddPermission", "Add and find permission", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_AccessControl::testAddPermission, this));
            registerTest("AddDuplicatePermissionFails", "Adding duplicate permission must throw", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_AccessControl::testAddDuplicatePermissionFails, this));
            registerTest("AddAndFindRole", "Add and find role", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_AccessControl::testAddAndFindRole, this));
            registerTest("AddDuplicateRoleFails", "Adding duplicate role must throw", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_AccessControl::testAddDuplicateRoleFails, this));
            registerTest("SetDefaultRole", "Set and retrieve default role", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_AccessControl::testSetDefaultRole, this));
            registerTest("SetToNoAccessControl", "Clear all roles and permissions, and set default", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_AccessControl::testSetToNoAccessControl, this));
            registerTest("CheckPermissionInRole", "Check whether a permission is in a role", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_AccessControl::testCheckPermissionInRole, this));
            registerTest("PermissionNotFoundThrows", "Querying missing permission with bMustExist=true should throw", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_AccessControl::testPermissionNotFoundThrows, this));
            registerTest("DefaultRoleNotSetThrows", "Accessing unset default role should throw", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_AccessControl::testDefaultRoleNotSetThrows, this));
            registerTest("EmptyIdentifierThrows", "Empty permission or role identifiers must throw", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_AccessControl::testEmptyIdentifierThrows, this));
        }

        void initializeTests() override {
            // No init needed
        }

    private:

        AMC::CStringResource makeString(const std::string& s) {
            return AMC::CStringResource(s);
        }

        void testAddPermission() {
            AMC::CAccessControl control;
            auto perm = control.addPermission("perm_read", makeString("Read"), makeString("Can read"));
            auto found = control.findPermission("perm_read", true);
            assertTrue(found->getIdentifier() == "perm_read");
        }

        void testAddDuplicatePermissionFails() {
            AMC::CAccessControl control;
            control.addPermission("perm_read", makeString("Read"), makeString("Can read"));

            bool thrown = false;
            try {
                control.addPermission("perm_read", makeString("Read Again"), makeString("Should fail"));
            } catch (...) {
                thrown = true;
            }
            assertTrue(thrown, "Duplicate permission not caught");
        }

        void testAddAndFindRole() {
            AMC::CAccessControl control;
            control.addRole("admin", makeString("Admin"), makeString("Has full access"));
            auto role = control.findRole("admin", true);
            assertTrue(role->getIdentifier() == "admin");
        }

        void testAddDuplicateRoleFails() {
            AMC::CAccessControl control;
            control.addRole("admin", makeString("Admin"), makeString("Has full access"));
            bool thrown = false;
            try {
                control.addRole("admin", makeString("Admin again"), makeString("Should fail"));
            } catch (...) {
                thrown = true;
            }
            assertTrue(thrown, "Duplicate role not caught");
        }

        void testSetDefaultRole() {
            AMC::CAccessControl control;
            control.addRole("default", makeString("Default"), makeString(""));
            control.setDefaultRole("default");

            auto def = control.getDefaultRole();
            assertTrue(def->getIdentifier() == "default");
        }

        void testSetToNoAccessControl() {
            AMC::CAccessControl control;
            control.setToNoAccessControl();
            auto def = control.getDefaultRole();
            assertTrue(def->getIdentifier() == "default");
        }

        void testCheckPermissionInRole() {
            AMC::CAccessControl control;
            auto perm = control.addPermission("perm_test", makeString("Test"), makeString(""));
            auto role = control.addRole("tester", makeString("Tester"), makeString(""));

            role->addPermission(perm);
            bool hasPerm = control.checkPermissionInRole("tester", "perm_test");
            assertTrue(hasPerm);
        }

        void testPermissionNotFoundThrows() {
            AMC::CAccessControl control;
            bool thrown = false;
            try {
                control.findPermission("nonexistent", true);
            } catch (...) {
                thrown = true;
            }
            assertTrue(thrown, "Expected exception for missing permission");
        }

        void testDefaultRoleNotSetThrows() {
            AMC::CAccessControl control;
            bool thrown = false;
            try {
                control.getDefaultRole();
            } catch (...) {
                thrown = true;
            }
            assertTrue(thrown, "Expected exception when default role not set");
        }

        void testEmptyIdentifierThrows() {
            AMC::CAccessControl control;
            control.addRole("viewer", makeString("Viewer"), makeString(""));
            control.addPermission("perm_view", makeString("View"), makeString(""));
            bool thrown = false;
            try {
                control.checkPermissionInRole("", "perm_view");
            } catch (...) {
                thrown = true;
            }
            assertTrue(thrown, "Empty role identifier not rejected");

            thrown = false;
            try {
                control.checkPermissionInRole("viewer", "");
            } catch (...) {
                thrown = true;
            }
            assertTrue(thrown, "Empty permission identifier not rejected");
        }
    };

}



#endif // __AMCTEST_UNITTEST_ACCESSCONTROL