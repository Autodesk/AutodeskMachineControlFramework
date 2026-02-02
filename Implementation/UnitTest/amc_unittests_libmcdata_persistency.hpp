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

#ifndef __AMCTEST_UNITTEST_LIBMCDATA_PERSISTENCY
#define __AMCTEST_UNITTEST_LIBMCDATA_PERSISTENCY

#include "amc_unittests.hpp"
#include "amc_unittests_libmcdata.hpp"
#include "common_utils.hpp"
#include "libmcdata_dynamic.hpp"

#include <cstdint>
#include <string>
#include <cmath>
#include <chrono>

namespace AMCUnitTest {

	class CUnitTestGroup_LibMCData_Persistency : public CUnitTestGroup {
	public:
		std::string getTestGroupName() override {
			return "LibMCData_Persistency";
		}

		void registerTests() override {
			registerTest("StoreAndRetrieveString", "Store and retrieve string parameter", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Persistency::testStoreAndRetrieveString, this));
			registerTest("StoreAndRetrieveInteger", "Store and retrieve integer parameter", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Persistency::testStoreAndRetrieveInteger, this));
			registerTest("StoreAndRetrieveDouble", "Store and retrieve double parameter", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Persistency::testStoreAndRetrieveDouble, this));
			registerTest("StoreAndRetrieveBool", "Store and retrieve bool parameter", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Persistency::testStoreAndRetrieveBool, this));
			registerTest("StoreAndRetrieveUUID", "Store and retrieve UUID parameter", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Persistency::testStoreAndRetrieveUUID, this));
			registerTest("HasParameter", "Check parameter existence", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Persistency::testHasParameter, this));
			registerTest("DeleteParameter", "Delete parameter", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Persistency::testDeleteParameter, this));
			
			// Additional Persistency tests for coverage
			registerTest("UpdateStringParam", "Update existing string parameter", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Persistency::testUpdateStringParam, this));
			registerTest("UpdateIntegerParam", "Update existing integer parameter", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Persistency::testUpdateIntegerParam, this));
			registerTest("UpdateDoubleParam", "Update existing double parameter", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Persistency::testUpdateDoubleParam, this));
			registerTest("UpdateBoolParam", "Update existing bool parameter", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Persistency::testUpdateBoolParam, this));
			registerTest("MultipleParameters", "Store multiple parameters", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Persistency::testMultipleParameters, this));
			registerTest("EmptyStringParam", "Store empty string parameter", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Persistency::testEmptyStringParam, this));
			registerTest("LargeStringParam", "Store large string parameter", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Persistency::testLargeStringParam, this));
			registerTest("EdgeCaseIntegers", "Test edge case integers", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Persistency::testEdgeCaseIntegers, this));
			registerTest("EdgeCaseDoubles", "Test edge case doubles", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Persistency::testEdgeCaseDoubles, this));
			registerTest("DeleteNonExistent", "Delete non-existent parameter", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Persistency::testDeleteNonExistent, this));
			registerTest("RetrieveAfterUpdate", "Retrieve after multiple updates", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Persistency::testRetrieveAfterUpdate, this));
		}

		void initializeTests() override {
		}

	private:

		struct SPersistencyFixture {
			LibMCData::PWrapper m_pDataWrapper;
			LibMCData::PDataModel m_pDataModel;
			LibMCData::PPersistencyHandler m_pPersistencyHandler;
			std::string m_sBasePath;
		};

		SPersistencyFixture createFixture(const std::string& sSuffix)
		{
			SPersistencyFixture fixture;

			std::string sRootPath = "temp";
			if (!AMCCommon::CUtils::fileOrPathExistsOnDisk(sRootPath))
				AMCCommon::CUtils::createDirectoryOnDisk(sRootPath);

			fixture.m_sBasePath = sRootPath + "/libmcdata_persistency_" + sSuffix + "_" + AMCCommon::CUtils::createUUID();
			if (!AMCCommon::CUtils::fileOrPathExistsOnDisk(fixture.m_sBasePath))
				AMCCommon::CUtils::createDirectoryOnDisk(fixture.m_sBasePath);

			std::string sDatabaseFile = fixture.m_sBasePath + "/persistency.db";

			fixture.m_pDataWrapper = AMCUnitTest::loadLibMCDataInProcess();
			fixture.m_pDataModel = fixture.m_pDataWrapper->CreateDataModelInstance();
			fixture.m_pDataModel->InitialiseDatabase(fixture.m_sBasePath, LibMCData::eDataBaseType::SqLite, sDatabaseFile);
			fixture.m_pPersistencyHandler = fixture.m_pDataModel->CreatePersistencyHandler();

			return fixture;
		}

		uint64_t getCurrentTimestamp()
		{
			return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(
				std::chrono::system_clock::now().time_since_epoch()).count());
		}

		void testStoreAndRetrieveString()
		{
			auto fixture = createFixture("string");
			
			std::string sUUID = AMCCommon::CUtils::createUUID();
			std::string sName = "test_string_param";
			std::string sValue = "Hello, Persistency!";
			
			// StorePersistentStringParameter(sUUID, sName, sValue, nAbsoluteTimeStamp)
			fixture.m_pPersistencyHandler->StorePersistentStringParameter(sUUID, sName, sValue, getCurrentTimestamp());
			
			std::string sRetrieved = fixture.m_pPersistencyHandler->RetrievePersistentStringParameter(sUUID);
			assertTrue(sRetrieved == sValue, "String value mismatch");
		}

		void testStoreAndRetrieveInteger()
		{
			auto fixture = createFixture("integer");
			
			std::string sUUID = AMCCommon::CUtils::createUUID();
			std::string sName = "test_integer_param";
			int64_t nValue = 1234567890123LL;
			
			fixture.m_pPersistencyHandler->StorePersistentIntegerParameter(sUUID, sName, nValue, getCurrentTimestamp());
			
			int64_t nRetrieved = fixture.m_pPersistencyHandler->RetrievePersistentIntegerParameter(sUUID);
			assertTrue(nRetrieved == nValue, "Integer value mismatch");
			
			// Test negative value
			std::string sUUID2 = AMCCommon::CUtils::createUUID();
			fixture.m_pPersistencyHandler->StorePersistentIntegerParameter(sUUID2, "neg_int", -999, getCurrentTimestamp());
			assertTrue(fixture.m_pPersistencyHandler->RetrievePersistentIntegerParameter(sUUID2) == -999, "Negative integer mismatch");
		}

		void testStoreAndRetrieveDouble()
		{
			auto fixture = createFixture("double");
			
			std::string sUUID = AMCCommon::CUtils::createUUID();
			std::string sName = "test_double_param";
			double dValue = 3.14159265358979;
			
			fixture.m_pPersistencyHandler->StorePersistentDoubleParameter(sUUID, sName, dValue, getCurrentTimestamp());
			
			double dRetrieved = fixture.m_pPersistencyHandler->RetrievePersistentDoubleParameter(sUUID);
			
			// Compare with larger epsilon for floating point - database may have different precision
			double dDiff = std::abs(dRetrieved - dValue);
			assertTrue(dDiff < 0.001, "Double value mismatch");
		}

		void testStoreAndRetrieveBool()
		{
			auto fixture = createFixture("bool");
			
			// Test true
			std::string sUUID1 = AMCCommon::CUtils::createUUID();
			fixture.m_pPersistencyHandler->StorePersistentBoolParameter(sUUID1, "bool_true", true, getCurrentTimestamp());
			assertTrue(fixture.m_pPersistencyHandler->RetrievePersistentBoolParameter(sUUID1) == true, "True value mismatch");
			
			// Test false
			std::string sUUID2 = AMCCommon::CUtils::createUUID();
			fixture.m_pPersistencyHandler->StorePersistentBoolParameter(sUUID2, "bool_false", false, getCurrentTimestamp());
			assertTrue(fixture.m_pPersistencyHandler->RetrievePersistentBoolParameter(sUUID2) == false, "False value mismatch");
		}

		void testStoreAndRetrieveUUID()
		{
			auto fixture = createFixture("uuid");
			
			std::string sParamUUID = AMCCommon::CUtils::createUUID();
			std::string sName = "test_uuid_param";
			std::string sValue = AMCCommon::CUtils::createUUID();
			
			fixture.m_pPersistencyHandler->StorePersistentUUIDParameter(sParamUUID, sName, sValue, getCurrentTimestamp());
			
			std::string sRetrieved = fixture.m_pPersistencyHandler->RetrievePersistentUUIDParameter(sParamUUID);
			
			// Normalize for comparison (UUIDs might be stored differently)
			std::string sNormalizedValue = AMCCommon::CUtils::normalizeUUIDString(sValue);
			std::string sNormalizedRetrieved = AMCCommon::CUtils::normalizeUUIDString(sRetrieved);
			assertTrue(sNormalizedRetrieved == sNormalizedValue, "UUID value mismatch");
		}

		void testHasParameter()
		{
			auto fixture = createFixture("has");
			
			std::string sExistingUUID = AMCCommon::CUtils::createUUID();
			std::string sNonExistingUUID = AMCCommon::CUtils::createUUID();
			
			assertFalse(fixture.m_pPersistencyHandler->HasPersistentParameter(sExistingUUID), "Should not exist initially");
			
			fixture.m_pPersistencyHandler->StorePersistentStringParameter(sExistingUUID, "test_param", "value", getCurrentTimestamp());
			
			assertTrue(fixture.m_pPersistencyHandler->HasPersistentParameter(sExistingUUID), "Should exist after storing");
			assertFalse(fixture.m_pPersistencyHandler->HasPersistentParameter(sNonExistingUUID), "Non-existing should not exist");
		}

		void testDeleteParameter()
		{
			auto fixture = createFixture("delete");
			
			std::string sUUID = AMCCommon::CUtils::createUUID();
			
			// Store then delete
			fixture.m_pPersistencyHandler->StorePersistentStringParameter(sUUID, "delete_param", "to_delete", getCurrentTimestamp());
			assertTrue(fixture.m_pPersistencyHandler->HasPersistentParameter(sUUID), "Should exist before delete");
			
			// Delete and verify - just check the parameter is gone
			fixture.m_pPersistencyHandler->DeletePersistentParameter(sUUID);
			assertFalse(fixture.m_pPersistencyHandler->HasPersistentParameter(sUUID), "Should not exist after delete");
		}
		
		// ============= Additional Persistency Tests =============
		
		void testUpdateStringParam()
		{
			auto fixture = createFixture("update_string");
			
			std::string sUUID = AMCCommon::CUtils::createUUID();
			
			fixture.m_pPersistencyHandler->StorePersistentStringParameter(sUUID, "update_param", "original", getCurrentTimestamp());
			assertTrue(fixture.m_pPersistencyHandler->RetrievePersistentStringParameter(sUUID) == "original", "Original value mismatch");
			
			fixture.m_pPersistencyHandler->StorePersistentStringParameter(sUUID, "update_param", "updated", getCurrentTimestamp());
			assertTrue(fixture.m_pPersistencyHandler->RetrievePersistentStringParameter(sUUID) == "updated", "Updated value mismatch");
		}
		
		void testUpdateIntegerParam()
		{
			auto fixture = createFixture("update_integer");
			
			std::string sUUID = AMCCommon::CUtils::createUUID();
			
			fixture.m_pPersistencyHandler->StorePersistentIntegerParameter(sUUID, "update_int", 100, getCurrentTimestamp());
			assertTrue(fixture.m_pPersistencyHandler->RetrievePersistentIntegerParameter(sUUID) == 100, "Original integer mismatch");
			
			fixture.m_pPersistencyHandler->StorePersistentIntegerParameter(sUUID, "update_int", 200, getCurrentTimestamp());
			assertTrue(fixture.m_pPersistencyHandler->RetrievePersistentIntegerParameter(sUUID) == 200, "Updated integer mismatch");
		}
		
		void testUpdateDoubleParam()
		{
			auto fixture = createFixture("update_double");
			
			std::string sUUID = AMCCommon::CUtils::createUUID();
			
			fixture.m_pPersistencyHandler->StorePersistentDoubleParameter(sUUID, "update_dbl", 1.5, getCurrentTimestamp());
			double dRetrieved1 = fixture.m_pPersistencyHandler->RetrievePersistentDoubleParameter(sUUID);
			assertTrue(std::abs(dRetrieved1 - 1.5) < 0.001, "Original double mismatch");
			
			fixture.m_pPersistencyHandler->StorePersistentDoubleParameter(sUUID, "update_dbl", 2.5, getCurrentTimestamp());
			double dRetrieved2 = fixture.m_pPersistencyHandler->RetrievePersistentDoubleParameter(sUUID);
			assertTrue(std::abs(dRetrieved2 - 2.5) < 0.001, "Updated double mismatch");
		}
		
		void testUpdateBoolParam()
		{
			auto fixture = createFixture("update_bool");
			
			std::string sUUID = AMCCommon::CUtils::createUUID();
			
			fixture.m_pPersistencyHandler->StorePersistentBoolParameter(sUUID, "update_bool", true, getCurrentTimestamp());
			assertTrue(fixture.m_pPersistencyHandler->RetrievePersistentBoolParameter(sUUID) == true, "Original bool mismatch");
			
			fixture.m_pPersistencyHandler->StorePersistentBoolParameter(sUUID, "update_bool", false, getCurrentTimestamp());
			assertTrue(fixture.m_pPersistencyHandler->RetrievePersistentBoolParameter(sUUID) == false, "Updated bool mismatch");
		}
		
		void testMultipleParameters()
		{
			auto fixture = createFixture("multiple");
			
			// Store multiple different parameters
			for (int i = 0; i < 10; i++) {
				std::string sUUID = AMCCommon::CUtils::createUUID();
				std::string sName = "param_" + std::to_string(i);
				std::string sValue = "value_" + std::to_string(i);
				
				fixture.m_pPersistencyHandler->StorePersistentStringParameter(sUUID, sName, sValue, getCurrentTimestamp());
				assertTrue(fixture.m_pPersistencyHandler->HasPersistentParameter(sUUID), "Parameter should exist");
				assertTrue(fixture.m_pPersistencyHandler->RetrievePersistentStringParameter(sUUID) == sValue, "Value mismatch for param " + std::to_string(i));
			}
		}
		
		void testEmptyStringParam()
		{
			auto fixture = createFixture("empty_string");
			
			std::string sUUID = AMCCommon::CUtils::createUUID();
			
			fixture.m_pPersistencyHandler->StorePersistentStringParameter(sUUID, "empty_param", "", getCurrentTimestamp());
			assertTrue(fixture.m_pPersistencyHandler->HasPersistentParameter(sUUID), "Empty string param should exist");
			
			std::string sRetrieved = fixture.m_pPersistencyHandler->RetrievePersistentStringParameter(sUUID);
			assertTrue(sRetrieved.empty(), "Retrieved empty string should be empty");
		}
		
		void testLargeStringParam()
		{
			auto fixture = createFixture("large_string");
			
			std::string sUUID = AMCCommon::CUtils::createUUID();
			
			// Create a large string (10KB)
			std::string sLargeValue(10240, 'X');
			
			fixture.m_pPersistencyHandler->StorePersistentStringParameter(sUUID, "large_param", sLargeValue, getCurrentTimestamp());
			
			std::string sRetrieved = fixture.m_pPersistencyHandler->RetrievePersistentStringParameter(sUUID);
			assertTrue(sRetrieved.size() == sLargeValue.size(), "Large string size mismatch");
			assertTrue(sRetrieved == sLargeValue, "Large string content mismatch");
		}
		
		void testEdgeCaseIntegers()
		{
			auto fixture = createFixture("edge_integers");
			
			// Test zero
			std::string sUUID1 = AMCCommon::CUtils::createUUID();
			fixture.m_pPersistencyHandler->StorePersistentIntegerParameter(sUUID1, "zero", 0, getCurrentTimestamp());
			assertTrue(fixture.m_pPersistencyHandler->RetrievePersistentIntegerParameter(sUUID1) == 0, "Zero mismatch");
			
			// Test max int64
			std::string sUUID2 = AMCCommon::CUtils::createUUID();
			int64_t nMaxInt = 9223372036854775807LL;
			fixture.m_pPersistencyHandler->StorePersistentIntegerParameter(sUUID2, "max_int", nMaxInt, getCurrentTimestamp());
			assertTrue(fixture.m_pPersistencyHandler->RetrievePersistentIntegerParameter(sUUID2) == nMaxInt, "Max int mismatch");
			
			// Test min int64
			std::string sUUID3 = AMCCommon::CUtils::createUUID();
			int64_t nMinInt = -9223372036854775807LL;
			fixture.m_pPersistencyHandler->StorePersistentIntegerParameter(sUUID3, "min_int", nMinInt, getCurrentTimestamp());
			assertTrue(fixture.m_pPersistencyHandler->RetrievePersistentIntegerParameter(sUUID3) == nMinInt, "Min int mismatch");
		}
		
		void testEdgeCaseDoubles()
		{
			auto fixture = createFixture("edge_doubles");
			
			// Test zero
			std::string sUUID1 = AMCCommon::CUtils::createUUID();
			fixture.m_pPersistencyHandler->StorePersistentDoubleParameter(sUUID1, "zero", 0.0, getCurrentTimestamp());
			double dRetrieved1 = fixture.m_pPersistencyHandler->RetrievePersistentDoubleParameter(sUUID1);
			assertTrue(std::abs(dRetrieved1) < 0.0001, "Zero double mismatch");
			
			// Test very small value
			std::string sUUID2 = AMCCommon::CUtils::createUUID();
			double dSmall = 0.000001;
			fixture.m_pPersistencyHandler->StorePersistentDoubleParameter(sUUID2, "small", dSmall, getCurrentTimestamp());
			double dRetrieved2 = fixture.m_pPersistencyHandler->RetrievePersistentDoubleParameter(sUUID2);
			assertTrue(std::abs(dRetrieved2 - dSmall) < 0.0000001, "Small double mismatch");
			
			// Test negative value
			std::string sUUID3 = AMCCommon::CUtils::createUUID();
			fixture.m_pPersistencyHandler->StorePersistentDoubleParameter(sUUID3, "negative", -123.456, getCurrentTimestamp());
			double dRetrieved3 = fixture.m_pPersistencyHandler->RetrievePersistentDoubleParameter(sUUID3);
			assertTrue(std::abs(dRetrieved3 - (-123.456)) < 0.001, "Negative double mismatch");
		}
		
		void testDeleteNonExistent()
		{
			auto fixture = createFixture("delete_nonexist");
			
			std::string sNonExistentUUID = AMCCommon::CUtils::createUUID();
			
			// Delete non-existent - should not throw
			fixture.m_pPersistencyHandler->DeletePersistentParameter(sNonExistentUUID);
			
			// Verify still doesn't exist
			assertFalse(fixture.m_pPersistencyHandler->HasPersistentParameter(sNonExistentUUID), "Non-existent should not exist");
		}
		
		void testRetrieveAfterUpdate()
		{
			auto fixture = createFixture("retrieve_update");
			
			std::string sUUID = AMCCommon::CUtils::createUUID();
			
			// Multiple updates
			for (int i = 0; i < 5; i++) {
				std::string sValue = "iteration_" + std::to_string(i);
				fixture.m_pPersistencyHandler->StorePersistentStringParameter(sUUID, "multi_update", sValue, getCurrentTimestamp());
			}
			
			// Final value should be the last one
			std::string sFinal = fixture.m_pPersistencyHandler->RetrievePersistentStringParameter(sUUID);
			assertTrue(sFinal == "iteration_4", "Final value should be iteration_4");
		}
	};

}

#endif // __AMCTEST_UNITTEST_LIBMCDATA_PERSISTENCY
