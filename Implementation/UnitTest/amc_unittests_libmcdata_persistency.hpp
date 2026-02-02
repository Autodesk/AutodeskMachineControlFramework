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
	};

}

#endif // __AMCTEST_UNITTEST_LIBMCDATA_PERSISTENCY
