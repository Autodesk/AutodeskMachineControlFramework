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

#ifndef __AMCTEST_UNITTEST_PARAMETERS
#define __AMCTEST_UNITTEST_PARAMETERS


#include "amc_unittests.hpp"
#include "amc_parameterhandler.hpp"
#include "amc_parametergroup.hpp"
#include "common_utils.hpp"


namespace AMCUnitTest {

	class CUnitTestGroup_Parameters : public CUnitTestGroup {
	public:

		std::string getTestGroupName() override {
			return "Parameters";
		}

		void registerTests() override {
			registerTest("ParameterGroupBasics", "Parameter group add/get/set operations", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_Parameters::testParameterGroupBasics, this));
			registerTest("DerivedParameters", "Derived parameter behavior and readonly checks", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_Parameters::testDerivedParameters, this));
			registerTest("ParameterSerialization", "Parameter group JSON serialization", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_Parameters::testParameterSerialization, this));
			registerTest("ParameterHandlerBasics", "Parameter handler group management and duplication", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_Parameters::testParameterHandlerBasics, this));
			registerTest("ParameterGroupEnumeration", "Parameter group enumeration by index (count/name/description)", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_Parameters::testParameterGroupEnumeration, this));
		}

		void initializeTests() override {
		}

	private:

		void testParameterGroupBasics()
		{
			auto chrono = std::make_shared<AMCCommon::CChrono>();
			AMC::CParameterGroup group("group", "desc", chrono);
			group.setJournal(nullptr, "instance");

			assertFalse(group.hasParameter("name"));
			group.addNewStringParameter("name", "Name", "default");
			group.addNewDoubleParameter("dbl", "Double", 1.5, 0.1);
			group.addNewIntParameter("int", "Int", 5);
			group.addNewBoolParameter("flag", "Flag", true);
			group.addNewUUIDParameter("uuid", "UUID", AMCCommon::CUtils::createUUID());
			group.addNewTypedParameter("typed", "bool", "Typed", "true", "");

			assertTrue(group.hasParameter("name"));
			assertTrue(group.getParameterCount() == 6);

			std::string desc, defValue;
			group.getParameterInfoByName("name", desc, defValue);
			assertTrue(defValue == "default");

			assertTrue(group.getParameterValueByName("name") == "default");
			assertTrue(group.getDoubleParameterValueByName("dbl") == 1.5);
			assertTrue(group.getIntParameterValueByName("int") == 5);
			assertTrue(group.getBoolParameterValueByName("flag"));

			group.setParameterValueByName("name", "updated");
			group.setDoubleParameterValueByName("dbl", 2.5);
			group.setIntParameterValueByName("int", 7);
			group.setBoolParameterValueByName("flag", false);

			assertTrue(group.getParameterValueByName("name") == "updated");
			assertTrue(group.getDoubleParameterValueByName("dbl") == 2.5);
			assertTrue(group.getIntParameterValueByName("int") == 7);
			assertFalse(group.getBoolParameterValueByName("flag"));

			std::string uuidValue = group.getUUIDParameterValueByName("uuid");
			assertTrue(AMCCommon::CUtils::stringIsUUIDString(uuidValue));

			group.setParameterValueByName("uuid", "not-a-uuid");
			assertTrue(group.getUUIDParameterValueByName("uuid") == AMCCommon::CUtils::createEmptyUUID());

			uint64_t counterBefore = group.getChangeCounterOf("name");
			group.setParameterValueByName("name", "changed");
			uint64_t counterAfter = group.getChangeCounterOf("name");
			assertTrue(counterAfter > counterBefore);

			std::string localPath = group.getLocalParameterPath("name");
			assertTrue(localPath == "instance.group.name");
			assertTrue(group.getOriginalParameterPath("name") == localPath);

			group.removeValue("typed");
			assertFalse(group.hasParameter("typed"));

			bool thrown = false;
			try {
				group.addNewStringParameter("_bad", "Bad", "x");
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected addNewStringParameter to reject invalid name");

			thrown = false;
			try {
				group.addNewTypedParameter("badtype", "invalid", "Bad", "", "");
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected addNewTypedParameter to reject invalid type");

			thrown = false;
			try {
				group.addNewTypedParameter("badunits", "double", "Bad", "1.0", "1e10");
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected addNewTypedParameter to reject invalid units");
		}

		void testDerivedParameters()
		{
			auto chrono = std::make_shared<AMCCommon::CChrono>();
			auto pSourceGroup = std::make_shared<AMC::CParameterGroup>("source", "src", chrono);
			pSourceGroup->setJournal(nullptr, "instance");
			pSourceGroup->addNewStringParameter("value", "Value", "a");
			pSourceGroup->setParameterValueByName("value", "b");

			AMC::CParameterGroup derived("derived", "drv", chrono);
			derived.setJournal(nullptr, "instance");
			derived.addNewDerivedParameter("value_copy", pSourceGroup, "value");

			assertTrue(derived.getParameterValueByName("value_copy") == "b");
			pSourceGroup->setParameterValueByName("value", "c");
			assertTrue(derived.getParameterValueByName("value_copy") == "c");

			assertTrue(derived.getOriginalParameterPath("value_copy") == pSourceGroup->getOriginalParameterPath("value"));

			bool thrown = false;
			try {
				derived.setParameterValueByName("value_copy", "x");
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected derived parameter to be read-only");
		}

		void testParameterSerialization()
		{
			auto chrono = std::make_shared<AMCCommon::CChrono>();
			AMC::CParameterGroup group("group", "desc", chrono);
			group.setJournal(nullptr, "instance");
			group.addNewStringParameter("name", "Name", "default");
			group.addNewIntParameter("count", "Count", 2);
			group.setParameterValueByName("name", "serialized");
			group.setIntParameterValueByName("count", 5);

			std::string json = group.serializeToJSON();

			AMC::CParameterGroup target("group", "desc", chrono);
			target.setJournal(nullptr, "instance");
			target.addNewStringParameter("name", "Name", "default");
			target.addNewIntParameter("count", "Count", 2);
			target.deserializeJSON(json, 0);

			assertTrue(target.getParameterValueByName("name") == "serialized");
			assertTrue(target.getIntParameterValueByName("count") == 5);

			bool thrown = false;
			try {
				target.deserializeJSON("[]", 0);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected deserializeJSON to reject non-object");

			thrown = false;
			try {
				target.deserializeJSON("{\"name\":5}", 0);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected deserializeJSON to reject non-string values");
		}

		void testParameterHandlerBasics()
		{
			auto chrono = std::make_shared<AMCCommon::CChrono>();
			AMC::CParameterHandler handler("handler", chrono);

			assertFalse(handler.hasGroup("group"));
			bool thrown = false;
			try {
				handler.addGroup("bad..name", "desc");
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected addGroup to reject invalid name");

			auto pGroup = handler.addGroup("group", "desc");
			pGroup->setJournal(nullptr, "instance");
			pGroup->addNewStringParameter("name", "Name", "default");
			pGroup->setParameterValueByName("name", "value");

			assertTrue(handler.getGroupCount() == 1);
			assertTrue(handler.getGroup(0).get() == pGroup.get());
			assertTrue(handler.findGroup("group", true).get() == pGroup.get());

			thrown = false;
			try {
				handler.getGroup(2);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected getGroup to throw on invalid index");

			auto duplicate = handler.duplicate();
			auto dupGroup = duplicate->findGroup("group", true);
			assertTrue(dupGroup->getParameterValueByName("name") == "value");

			pGroup->setParameterValueByName("name", "changed");
			assertTrue(dupGroup->getParameterValueByName("name") == "value");

			duplicate->loadPersistentParameters(nullptr, 0);
		}

		void testParameterGroupEnumeration()
		{
			// Verifies index-based enumeration (getParameterCount / getParameterInfo), which backs the
			// StateEnvironment / UIEnvironment parameter-group enumeration methods used for dynamic dropdowns.
			auto chrono = std::make_shared<AMCCommon::CChrono>();
			AMC::CParameterGroup group("sequences", "Selectable Operation Sequences", chrono);
			group.setJournal(nullptr, "main");

			assertTrue(group.getParameterCount() == 0);

			group.addNewStringParameter("warmup", "Warmup Sequence", "sequence_warmup");
			group.addNewStringParameter("layerloop", "Layer Loop Sequence", "sequence_layerloop");
			group.addNewStringParameter("cooldown", "Cooldown Sequence", "sequence_cooldown");

			assertTrue(group.getParameterCount() == 3);

			// Enumeration preserves insertion order and returns matching name/description/default triples.
			const char* expectedNames[3] = { "warmup", "layerloop", "cooldown" };
			const char* expectedDescriptions[3] = { "Warmup Sequence", "Layer Loop Sequence", "Cooldown Sequence" };
			const char* expectedDefaults[3] = { "sequence_warmup", "sequence_layerloop", "sequence_cooldown" };

			for (uint32_t nIndex = 0; nIndex < group.getParameterCount(); nIndex++) {
				std::string sName, sDescription, sDefaultValue;
				group.getParameterInfo(nIndex, sName, sDescription, sDefaultValue);
				assertTrue(sName == expectedNames[nIndex], "Unexpected parameter name during enumeration");
				assertTrue(sDescription == expectedDescriptions[nIndex], "Unexpected parameter description during enumeration");
				assertTrue(sDefaultValue == expectedDefaults[nIndex], "Unexpected parameter default during enumeration");
			}

			// Out-of-range indices must be rejected.
			bool thrown = false;
			try {
				std::string sName, sDescription, sDefaultValue;
				group.getParameterInfo(group.getParameterCount(), sName, sDescription, sDefaultValue);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected getParameterInfo to throw on out-of-range index");
		}
	};

}

#endif // __AMCTEST_UNITTEST_PARAMETERS
