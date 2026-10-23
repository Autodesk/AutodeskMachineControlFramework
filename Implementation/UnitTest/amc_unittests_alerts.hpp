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

#ifndef __AMCTEST_UNITTEST_ALERTS
#define __AMCTEST_UNITTEST_ALERTS


#include "amc_unittests.hpp"
#include "amc_alerthandler.hpp"
#include "amc_languagedefinition.hpp"
#include "amc_languagestring.hpp"

#include "PugiXML/pugixml.hpp"


namespace AMCUnitTest {

	class CUnitTestGroup_Alerts : public CUnitTestGroup {
	public:

		std::string getTestGroupName() override {
			return "Alerts";
		}

		void registerTests() override {
			registerTest("AlertDefinitionBasics", "Alert definition properties and translations", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_Alerts::testAlertDefinitionBasics, this));
			registerTest("LanguageFallback", "Translation lookup through parent languages, inline text and key", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_Alerts::testLanguageFallback, this));
			registerTest("LanguageStringFromXML", "Language strings read the requested attribute and its i18n key", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_Alerts::testLanguageStringFromXML, this));
			registerTest("AlertLevelConversions", "Alert level string conversions", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_Alerts::testAlertLevelConversions, this));
			registerTest("AlertHandlerDefinitions", "Alert handler add/find/validate definitions", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_Alerts::testAlertHandlerDefinitions, this));
		}

		void initializeTests() override {
		}

	private:

		void testAlertDefinitionBasics()
		{
			AMC::CLanguageString description("test_desc", "Custom");
			AMC::CAlertDefinition definition("AlertOne", LibMCData::eAlertLevel::Warning, description, true);
			assertTrue(definition.getIdentifier() == "AlertOne");
			assertTrue(definition.getAlertLevel() == LibMCData::eAlertLevel::Warning);
			assertTrue(definition.needsAcknowledgement());
			assertTrue(definition.getDescription().getStringIdentifier() == "test_desc");
			assertTrue(definition.getDescription().getCustomValue() == "Custom");

			auto pLanguage = std::make_shared<AMC::CLanguageDefinition>("en", nullptr);
			pLanguage->addTranslation("test_desc", "Translated");
			assertTrue(definition.getTranslatedDescription(pLanguage.get()) == "Translated");

			AMC::CLanguageString customOnly("", "Fallback");
			AMC::CAlertDefinition customDefinition("AlertTwo", LibMCData::eAlertLevel::Message, customOnly, false);
			assertFalse(customDefinition.needsAcknowledgement());
			assertTrue(customDefinition.getTranslatedDescription(pLanguage.get()) == "Fallback");

			definition.setAckPermissionIdentifier("ack_permission");
			assertTrue(definition.getAckPermissionIdentifier() == "ack_permission");
		}

		void testLanguageFallback()
		{
			auto pEnglish = std::make_shared<AMC::CLanguageDefinition>("en", nullptr);
			pEnglish->addTranslation("alerts.dooropen", "Door is open");
			pEnglish->addTranslation("alerts.empty", "");

			auto pGerman = std::make_shared<AMC::CLanguageDefinition>("de", pEnglish);
			pGerman->addTranslation("alerts.doorclosed", "Tuer ist geschlossen");

			auto pSwiss = std::make_shared<AMC::CLanguageDefinition>("de-CH", pGerman);

			AMC::CLanguageString doorClosed("alerts.doorclosed", "Door is closed");
			AMC::CLanguageString doorOpen("alerts.dooropen", "Inline door open");
			AMC::CLanguageString missing("alerts.missing", "Inline missing");
			AMC::CLanguageString missingWithoutText("alerts.missingnotext", "");
			AMC::CLanguageString emptyTranslation("alerts.empty", "Inline empty");

			assertTrue(doorClosed.getTranslatedString(pSwiss.get()) == "Tuer ist geschlossen");
			assertTrue(doorOpen.getTranslatedString(pSwiss.get()) == "Door is open");
			assertTrue(missing.getTranslatedString(pSwiss.get()) == "Inline missing");
			assertTrue(missingWithoutText.getTranslatedString(pSwiss.get()) == "alerts.missingnotext");
			assertTrue(emptyTranslation.getTranslatedString(pSwiss.get()) == "");

			auto misses = pSwiss->getTranslationMisses();
			assertTrue(misses.size() == 2);
			assertTrue(misses.find("alerts.missing") != misses.end());
			assertTrue(misses.find("alerts.missingnotext") != misses.end());
			assertTrue(pEnglish->getTranslationMisses().empty());

			bool thrown = false;
			try {
				AMC::CLanguageDefinition invalidLanguage("de_CH", nullptr);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected CLanguageDefinition to reject non-BCP 47 identifiers");

			thrown = false;
			try {
				AMC::CLanguageString invalidKey("alerts..dooropen", "Text");
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected CLanguageString to reject invalid keys");
		}

		void testLanguageStringFromXML()
		{
			pugi::xml_document document;
			auto parseResult = document.load_string("<permission displayname=\"Edit settings\" i18n:displayname=\"permissions.editsettings\" description=\"Allows editing\" i18n:description=\"permissions.editsettings_desc\" />");
			assertTrue(parseResult.status == pugi::status_ok);

			auto permissionNode = document.child("permission");

			AMC::CLanguageString displayName(permissionNode, "displayname");
			assertTrue(displayName.getCustomValue() == "Edit settings");
			assertTrue(displayName.getStringIdentifier() == "permissions.editsettings");

			AMC::CLanguageString description(permissionNode, "description");
			assertTrue(description.getCustomValue() == "Allows editing");
			assertTrue(description.getStringIdentifier() == "permissions.editsettings_desc");

			AMC::CLanguageString caption(permissionNode, "caption");
			assertTrue(caption.getCustomValue().empty());
			assertTrue(caption.getStringIdentifier().empty());
		}

		void testAlertLevelConversions()
		{
			assertTrue(AMC::CAlertDefinition::stringToAlertLevel("fatalerror", true) == LibMCData::eAlertLevel::FatalError);
			assertTrue(AMC::CAlertDefinition::stringToAlertLevel("criticalerror", true) == LibMCData::eAlertLevel::CriticalError);
			assertTrue(AMC::CAlertDefinition::stringToAlertLevel("warning", true) == LibMCData::eAlertLevel::Warning);
			assertTrue(AMC::CAlertDefinition::stringToAlertLevel("message", true) == LibMCData::eAlertLevel::Message);
			assertTrue(AMC::CAlertDefinition::stringToAlertLevel("unknown", false) == LibMCData::eAlertLevel::Unknown);

			bool thrown = false;
			try {
				AMC::CAlertDefinition::stringToAlertLevel("unknown", true);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected stringToAlertLevel to throw on unknown input");

			assertTrue(AMC::CAlertDefinition::alertLevelToString(LibMCData::eAlertLevel::FatalError) == "fatalerror");
			assertTrue(AMC::CAlertDefinition::alertLevelToString(LibMCData::eAlertLevel::CriticalError) == "criticalerror");
			assertTrue(AMC::CAlertDefinition::alertLevelToString(LibMCData::eAlertLevel::Warning) == "warning");
			assertTrue(AMC::CAlertDefinition::alertLevelToString(LibMCData::eAlertLevel::Message) == "message");
			assertTrue(AMC::CAlertDefinition::alertLevelToString(LibMCData::eAlertLevel::Unknown) == "unknown");
		}

		void testAlertHandlerDefinitions()
		{
			AMC::CAlertHandler handler;
			AMC::CLanguageString description("desc", "Desc");

			assertFalse(handler.hasDefinition("AlertOne"));
			auto pDefinition = handler.addDefinition("AlertOne", LibMCData::eAlertLevel::Warning, description, true);
			assertTrue(handler.hasDefinition("AlertOne"));
			assertTrue(pDefinition.get() != nullptr);

			auto pFound = handler.findDefinition("AlertOne", true);
			assertTrue(pFound.get() == pDefinition.get());
			assertTrue(pFound->needsAcknowledgement());

			auto pMissing = handler.findDefinition("Missing", false);
			assertTrue(pMissing.get() == nullptr);

			bool thrown = false;
			try {
				handler.findDefinition("Missing", true);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected findDefinition to throw on missing definition");

			thrown = false;
			try {
				handler.addDefinition("AlertOne", LibMCData::eAlertLevel::Warning, description, false);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected addDefinition to throw on duplicate identifier");

			thrown = false;
			try {
				handler.addDefinition("", LibMCData::eAlertLevel::Warning, description, false);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected addDefinition to throw on empty identifier");

			thrown = false;
			try {
				handler.addDefinition("bad-char", LibMCData::eAlertLevel::Warning, description, false);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected addDefinition to throw on invalid identifier");

			thrown = false;
			try {
				handler.hasDefinition("");
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected hasDefinition to throw on empty identifier");

			thrown = false;
			try {
				handler.hasDefinition("bad-char");
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected hasDefinition to throw on invalid identifier");
		}
	};

}

#endif // __AMCTEST_UNITTEST_ALERTS
