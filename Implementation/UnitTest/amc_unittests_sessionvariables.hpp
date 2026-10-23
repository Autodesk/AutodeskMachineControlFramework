/*++

Copyright (C) 2026 Autodesk Inc.

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

#ifndef __AMCTEST_UNITTEST_SESSIONVARIABLES
#define __AMCTEST_UNITTEST_SESSIONVARIABLES


#include "amc_unittests.hpp"
#include "amc_ui_expression.hpp"
#include "amc_ui_frontenddefinition.hpp"
#include "amc_ui_frontendstate.hpp"
#include "amc_statemachinedata.hpp"
#include "amc_userinformation.hpp"
#include "amc_accesscontrol.hpp"
#include "amc_accessrole.hpp"
#include "common_chrono.hpp"
#include "common_utils.hpp"


namespace AMCUnitTest {

	class CUnitTestGroup_SessionVariables : public CUnitTestGroup {
	public:

		std::string getTestGroupName() override {
			return "SessionVariables";
		}

		void registerTests() override {
			registerTest("ExpressionWithoutContext", "Session references evaluate to empty/false without a session context", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_SessionVariables::testExpressionWithoutContext, this));
			registerTest("Declarations", "Session variable declarations reject invalid and duplicate names", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_SessionVariables::testDeclarations, this));
			registerTest("SessionIsolation", "Session variables of different sessions are independent", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_SessionVariables::testSessionIsolation, this));
			registerTest("Broadcast", "Broadcasts reach existing sessions only", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_SessionVariables::testBroadcast, this));
			registerTest("BuiltinReferences", "Built-in session and user references", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_SessionVariables::testBuiltinReferences, this));
			registerTest("ReferenceValidation", "Startup validation of session references", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_SessionVariables::testReferenceValidation, this));
		}

		void initializeTests() override {
		}

	private:

		AMC::PUIFrontendDefinition createDefinition()
		{
			auto pDefinition = std::make_shared<AMC::CUIFrontendDefinition>(std::make_shared<AMCCommon::CChrono>(), std::make_shared<AMC::CFrontendChangeCounter>());
			pDefinition->addSessionVariable("flag", "bool", "Flag", "0");
			pDefinition->addSessionVariable("selecteduuid", "uuid", "Selected UUID", AMCCommon::CUtils::createEmptyUUID());
			pDefinition->addSessionVariable("counter", "int", "Counter", "5");
			pDefinition->addSessionVariable("caption", "string", "Caption", "default");
			return pDefinition;
		}

		AMC::CUIExpression createSyncExpression(const std::string& sExpression)
		{
			AMC::CUIExpression expression;
			expression.setSyncValue(sExpression);
			return expression;
		}

		template <typename F> bool throwsException(F function)
		{
			try {
				function();
			}
			catch (...) {
				return true;
			}
			return false;
		}

		void testExpressionWithoutContext()
		{
			AMC::CStateMachineData stateMachineData;

			assertTrue(AMC::CUIExpression::isSessionReference("session.flag"));
			assertTrue(AMC::CUIExpression::isSessionReference("$session.uuid"));
			assertTrue(AMC::CUIExpression::isSessionReference("$user.role"));
			assertFalse(AMC::CUIExpression::isSessionReference("main.ui.flag"));

			auto flagExpression = createSyncExpression("session.flag");
			auto invertedExpression = createSyncExpression("!session.flag");
			auto captionExpression = createSyncExpression("session.caption");

			assertTrue(flagExpression.getSessionReference() == "session.flag");
			assertTrue(invertedExpression.getSessionReference() == "session.flag");
			assertTrue(createSyncExpression("main.ui.flag").getSessionReference().empty());

			assertFalse(flagExpression.evaluateBoolValue(&stateMachineData));
			assertTrue(invertedExpression.evaluateBoolValue(&stateMachineData));
			assertTrue(captionExpression.evaluateStringValue(&stateMachineData).empty());
			assertTrue(createSyncExpression("session.counter").evaluateIntegerValue(&stateMachineData) == 0);
		}

		void testDeclarations()
		{
			auto pDefinition = createDefinition();

			assertTrue(pDefinition->hasSessionVariable("flag"));
			assertFalse(pDefinition->hasSessionVariable("unknown"));

			assertTrue(throwsException([&]() { pDefinition->addSessionVariable("flag", "bool", "", "0"); }), "duplicate session variable must be rejected");
			assertTrue(throwsException([&]() { pDefinition->addSessionVariable("invalid.name", "bool", "", "0"); }), "invalid session variable name must be rejected");
			assertTrue(throwsException([&]() { pDefinition->addSessionVariable("", "bool", "", "0"); }), "empty session variable name must be rejected");
			assertTrue(throwsException([&]() { pDefinition->addSessionVariable("badtype", "invalid", "", "0"); }), "invalid session variable type must be rejected");
			assertTrue(throwsException([&]() { pDefinition->broadcastSessionVariable("unknown", "1"); }), "broadcast of undeclared session variable must be rejected");
		}

		void testSessionIsolation()
		{
			AMC::CStateMachineData stateMachineData;
			auto pDefinition = createDefinition();

			AMC::CUIFrontendState sessionA(pDefinition);
			AMC::CUIFrontendState sessionB(pDefinition);

			assertFalse(sessionA.getSessionVariableAsBool("flag"));
			assertTrue(sessionA.getSessionVariableAsInteger("counter") == 5);
			assertTrue(sessionA.getSessionVariable("caption") == "default");

			std::string sBuildUUID = AMCCommon::CUtils::createUUID();
			sessionA.setSessionVariableAsBool("flag", true);
			sessionA.setSessionVariableAsUUID("selecteduuid", sBuildUUID);
			sessionA.setSessionVariableAsInteger("counter", 42);

			assertTrue(sessionA.getSessionVariableAsBool("flag"));
			assertTrue(sessionA.getSessionVariableAsUUID("selecteduuid") == AMCCommon::CUtils::normalizeUUIDString(sBuildUUID));
			assertTrue(sessionA.getSessionVariableAsInteger("counter") == 42);

			assertFalse(sessionB.getSessionVariableAsBool("flag"));
			assertTrue(sessionB.getSessionVariableAsUUID("selecteduuid") == AMCCommon::CUtils::createEmptyUUID());
			assertTrue(sessionB.getSessionVariableAsInteger("counter") == 5);

			auto flagExpression = createSyncExpression("session.flag");
			auto invertedExpression = createSyncExpression("!session.flag");
			assertTrue(flagExpression.evaluateBoolValue(&stateMachineData, &sessionA));
			assertFalse(flagExpression.evaluateBoolValue(&stateMachineData, &sessionB));
			assertFalse(invertedExpression.evaluateBoolValue(&stateMachineData, &sessionA));
			assertTrue(invertedExpression.evaluateBoolValue(&stateMachineData, &sessionB));

			auto uuidExpression = createSyncExpression("session.selecteduuid");
			assertTrue(uuidExpression.evaluateUUIDValue(&stateMachineData, &sessionA) == AMCCommon::CUtils::normalizeUUIDString(sBuildUUID));

			sessionA.setSessionVariableAsUUID("selecteduuid", "");
			assertTrue(sessionA.getSessionVariableAsUUID("selecteduuid") == AMCCommon::CUtils::createEmptyUUID());

			assertTrue(throwsException([&]() { sessionA.getSessionVariable("unknown"); }), "access to undeclared session variable must be rejected");
			assertTrue(throwsException([&]() { sessionA.setSessionVariable("unknown", "1"); }), "write to undeclared session variable must be rejected");
		}

		void testBroadcast()
		{
			auto pDefinition = createDefinition();

			AMC::CUIFrontendState sessionA(pDefinition);
			AMC::CUIFrontendState sessionB(pDefinition);
			sessionB.setSessionVariableAsInteger("counter", 7);

			pDefinition->broadcastSessionVariableAsBool("flag", true);

			assertTrue(sessionA.getSessionVariableAsBool("flag"));
			assertTrue(sessionB.getSessionVariableAsBool("flag"));
			assertTrue(sessionB.getSessionVariableAsInteger("counter") == 7, "broadcast must not touch other variables");

			AMC::CUIFrontendState sessionC(pDefinition);
			assertFalse(sessionC.getSessionVariableAsBool("flag"), "sessions created after a broadcast start with the declared default");

			auto pChangeCounter = pDefinition->getFrontendChangeCounter();
			uint64_t nCounterBeforeWrite = pChangeCounter->get();
			sessionA.setSessionVariableAsBool("flag", false);
			assertFalse(sessionA.getSessionVariableAsBool("flag"), "a local write after a broadcast must win");
			assertTrue(sessionB.getSessionVariableAsBool("flag"));
			assertTrue(pChangeCounter->get() > nCounterBeforeWrite, "a changed session variable must bump the frontend change counter");

			pDefinition->broadcastSessionVariableAsInteger("counter", 99);
			assertFalse(sessionA.getSessionVariableAsBool("flag"), "an older broadcast must not be applied again");
			assertTrue(sessionA.getSessionVariableAsInteger("counter") == 99);
			assertTrue(sessionC.getSessionVariableAsInteger("counter") == 99);
			assertFalse(sessionC.getSessionVariableAsBool("flag"));

			uint64_t nCounterBeforeRebroadcast = pChangeCounter->get();
			pDefinition->broadcastSessionVariableAsBool("flag", true);
			assertTrue(pChangeCounter->get() > nCounterBeforeRebroadcast, "a broadcast of an unchanged value must still bump the frontend change counter");
			assertTrue(sessionA.getSessionVariableAsBool("flag"), "a repeated broadcast must reach sessions that changed the value locally");
		}

		void testBuiltinReferences()
		{
			AMC::CStateMachineData stateMachineData;
			auto pDefinition = createDefinition();

			auto pAccessControl = std::make_shared<AMC::CAccessControl>();
			auto pPermission = pAccessControl->addPermission("perm_edit", AMC::CLanguageString("", "Edit"), AMC::CLanguageString("", ""));
			pAccessControl->addPermission("perm_admin", AMC::CLanguageString("", "Admin"), AMC::CLanguageString("", ""));
			auto pRole = pAccessControl->addRole("operator", AMC::CLanguageString("", "Operator"), AMC::CLanguageString("", ""));
			pRole->addPermission(pPermission);

			std::string sUserUUID = AMCCommon::CUtils::createUUID();
			auto pUserInformation = std::make_shared<AMC::CUserInformation>(sUserUUID, "jdoe", "John Doe", "operator", "en");

			AMC::CUIFrontendState session(pDefinition);

			assertTrue(createSyncExpression("$user.login").evaluateStringValue(&stateMachineData, &session).empty(), "no user identity yet");
			assertFalse(createSyncExpression("$user.permission.perm_edit").evaluateBoolValue(&stateMachineData, &session), "no user identity yet");

			std::string sSessionUUID = AMCCommon::CUtils::createUUID();
			session.setSessionIdentity(sSessionUUID, pUserInformation, pAccessControl);

			assertTrue(createSyncExpression("$session.uuid").evaluateStringValue(&stateMachineData, &session) == AMCCommon::CUtils::normalizeUUIDString(sSessionUUID));
			assertTrue(createSyncExpression("$user.login").evaluateStringValue(&stateMachineData, &session) == "jdoe");
			assertTrue(createSyncExpression("$user.role").evaluateStringValue(&stateMachineData, &session) == "operator");
			assertTrue(createSyncExpression("$user.description").evaluateStringValue(&stateMachineData, &session) == "John Doe");
			assertTrue(createSyncExpression("$user.language").evaluateStringValue(&stateMachineData, &session) == "en");

			assertTrue(createSyncExpression("$user.permission.perm_edit").evaluateBoolValue(&stateMachineData, &session));
			assertFalse(createSyncExpression("$user.permission.perm_admin").evaluateBoolValue(&stateMachineData, &session));
			assertTrue(createSyncExpression("!$user.permission.perm_admin").evaluateBoolValue(&stateMachineData, &session));
		}

		void testReferenceValidation()
		{
			auto pDefinition = createDefinition();

			AMC::CAccessControl accessControl;
			accessControl.addPermission("perm_edit", AMC::CLanguageString("", "Edit"), AMC::CLanguageString("", ""));

			AMC::CUIFrontendState::validateSessionReference("session.flag", pDefinition.get(), &accessControl);
			AMC::CUIFrontendState::validateSessionReference("$session.uuid", pDefinition.get(), &accessControl);
			AMC::CUIFrontendState::validateSessionReference("$user.role", pDefinition.get(), &accessControl);
			AMC::CUIFrontendState::validateSessionReference("$user.permission.perm_edit", pDefinition.get(), &accessControl);

			assertTrue(throwsException([&]() { AMC::CUIFrontendState::validateSessionReference("session.unknown", pDefinition.get(), &accessControl); }), "undeclared session variable must be rejected");
			assertTrue(throwsException([&]() { AMC::CUIFrontendState::validateSessionReference("$user.permission.perm_unknown", pDefinition.get(), &accessControl); }), "unknown permission must be rejected");
			assertTrue(throwsException([&]() { AMC::CUIFrontendState::validateSessionReference("$user.password", pDefinition.get(), &accessControl); }), "unknown user reference must be rejected");
			assertTrue(throwsException([&]() { AMC::CUIFrontendState::validateSessionReference("$session.token", pDefinition.get(), &accessControl); }), "unknown session reference must be rejected");
		}

	};

}

#endif // __AMCTEST_UNITTEST_SESSIONVARIABLES
