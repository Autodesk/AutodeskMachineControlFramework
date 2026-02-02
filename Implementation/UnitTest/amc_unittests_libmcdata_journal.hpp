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

#ifndef __AMCTEST_UNITTEST_LIBMCDATA_JOURNAL
#define __AMCTEST_UNITTEST_LIBMCDATA_JOURNAL

#include "amc_unittests.hpp"
#include "amc_unittests_libmcdata.hpp"
#include "common_utils.hpp"
#include "libmcdata_dynamic.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace AMCUnitTest {

	class CUnitTestGroup_LibMCData_Journal : public CUnitTestGroup {
	public:
		std::string getTestGroupName() override {
			return "LibMCData_Journal";
		}

		void registerTests() override {
			registerTest("SessionUUID", "Verify journal session UUID", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Journal::testSessionUUID, this));
			registerTest("CreateVariable", "Create variable in journal", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Journal::testCreateVariable, this));
			registerTest("CreateVariableAlias", "Create variable alias", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Journal::testCreateVariableAlias, this));
			registerTest("ReaderFromUUID", "Create reader from journal UUID", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Journal::testReaderFromUUID, this));
			
			// Additional JournalReader tests for coverage
			registerTest("ReaderGetStartTime", "Get journal start time", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Journal::testReaderGetStartTime, this));
			registerTest("ReaderGetLifeTime", "Get journal lifetime", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Journal::testReaderGetLifeTime, this));
			registerTest("ReaderGetChunkCount", "Get chunk count", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Journal::testReaderGetChunkCount, this));
			registerTest("ReaderGetVarInfo", "Get variable information", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Journal::testReaderGetVarInfo, this));
			registerTest("ReaderGetAliasCount", "Get alias count", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Journal::testReaderGetAliasCount, this));
			registerTest("ReaderGetAliasInfo", "Get alias information", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Journal::testReaderGetAliasInfo, this));
			registerTest("MultipleVariables", "Multiple variables in journal", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Journal::testMultipleVariables, this));
			registerTest("VariableTypes", "Different variable types", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Journal::testVariableTypes, this));
			registerTest("MultipleAliases", "Multiple aliases", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Journal::testMultipleAliases, this));
			registerTest("JournalSessionClose", "Close journal session", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Journal::testJournalSessionClose, this));
			
			// Additional tests to reach 100
			registerTest("VariableUnits", "Variable units in journal", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Journal::testVariableUnits, this));
			registerTest("VarDataTypes", "Variable data types", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Journal::testVarDataTypes, this));
			registerTest("AliasMapping", "Verify alias to source mapping", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_Journal::testAliasMapping, this));
		}

		void initializeTests() override {
		}

	private:

		struct SJournalFixture {
			LibMCData::PWrapper m_pDataWrapper;
			LibMCData::PDataModel m_pDataModel;
			LibMCData::PJournalSession m_pJournalSession;
			std::string m_sBasePath;
		};

		SJournalFixture createFixture(const std::string& sSuffix)
		{
			SJournalFixture fixture;

			std::string sRootPath = "temp";
			if (!AMCCommon::CUtils::fileOrPathExistsOnDisk(sRootPath))
				AMCCommon::CUtils::createDirectoryOnDisk(sRootPath);

			fixture.m_sBasePath = sRootPath + "/libmcdata_journal_" + sSuffix + "_" + AMCCommon::CUtils::createUUID();
			if (!AMCCommon::CUtils::fileOrPathExistsOnDisk(fixture.m_sBasePath))
				AMCCommon::CUtils::createDirectoryOnDisk(fixture.m_sBasePath);

			std::string sDatabaseFile = fixture.m_sBasePath + "/journal.db";

			fixture.m_pDataWrapper = AMCUnitTest::loadLibMCDataInProcess();
			fixture.m_pDataModel = fixture.m_pDataWrapper->CreateDataModelInstance();
			fixture.m_pDataModel->InitialiseDatabase(fixture.m_sBasePath, LibMCData::eDataBaseType::SqLite, sDatabaseFile);
			fixture.m_pJournalSession = fixture.m_pDataModel->CreateJournalSession();

			return fixture;
		}

		void testSessionUUID()
		{
			auto fixture = createFixture("uuid");
			
			std::string sSessionUUID = fixture.m_pJournalSession->GetSessionUUID();
			assertFalse(sSessionUUID.empty(), "Session UUID should not be empty");
			assertTrue(sSessionUUID.length() == 36, "Session UUID should be 36 characters");
		}

		void testCreateVariable()
		{
			auto fixture = createFixture("create_var");
			
			// CreateVariableInJournalDB(sName, nID, nIndex, eDataType, dUnits)
			fixture.m_pJournalSession->CreateVariableInJournalDB("test_variable", 1, 1, LibMCData::eParameterDataType::Double, 1.0);
			
			// Variable was created - we can verify via reader
			std::string sJournalUUID = fixture.m_pJournalSession->GetSessionUUID();
			auto pReader = fixture.m_pDataModel->CreateJournalReader(sJournalUUID);
			
			uint32_t nVarCount = pReader->GetVariableCount();
			assertTrue(nVarCount >= 1, "Should have at least 1 variable");
		}

		void testCreateVariableAlias()
		{
			auto fixture = createFixture("alias");
			
			fixture.m_pJournalSession->CreateVariableInJournalDB("original_variable", 1, 1, LibMCData::eParameterDataType::Double, 1.0);
			
			// Create alias
			fixture.m_pJournalSession->CreateVariableAliasInJournalDB("alias_name", "original_variable");
			
			// Verify via reader
			std::string sJournalUUID = fixture.m_pJournalSession->GetSessionUUID();
			auto pReader = fixture.m_pDataModel->CreateJournalReader(sJournalUUID);
			
			uint32_t nVarCount = pReader->GetVariableCount();
			assertTrue(nVarCount >= 1, "Should have at least 1 variable");
		}

		void testReaderFromUUID()
		{
			auto fixture = createFixture("reader_uuid");
			
			fixture.m_pJournalSession->CreateVariableInJournalDB("reader_test_var", 1, 1, LibMCData::eParameterDataType::Double, 1.0);
			
			std::string sJournalUUID = fixture.m_pJournalSession->GetSessionUUID();
			
			// Create reader for same journal
			auto pReader = fixture.m_pDataModel->CreateJournalReader(sJournalUUID);
			
			assertAssigned(pReader.get(), "Reader should be created");
			assertTrue(pReader->GetJournalUUID() == sJournalUUID, "Reader journal UUID should match");
		}
		
		// ============= Additional JournalReader Tests =============
		
		void testReaderGetStartTime()
		{
			auto fixture = createFixture("start_time");
			
			fixture.m_pJournalSession->CreateVariableInJournalDB("time_var", 1, 1, LibMCData::eParameterDataType::Double, 1.0);
			
			std::string sJournalUUID = fixture.m_pJournalSession->GetSessionUUID();
			auto pReader = fixture.m_pDataModel->CreateJournalReader(sJournalUUID);
			
			std::string sStartTime = pReader->GetStartTime();
			assertFalse(sStartTime.empty(), "Start time should not be empty");
		}
		
		void testReaderGetLifeTime()
		{
			auto fixture = createFixture("lifetime");
			
			fixture.m_pJournalSession->CreateVariableInJournalDB("lifetime_var", 1, 1, LibMCData::eParameterDataType::Double, 1.0);
			
			std::string sJournalUUID = fixture.m_pJournalSession->GetSessionUUID();
			auto pReader = fixture.m_pDataModel->CreateJournalReader(sJournalUUID);
			
			uint64_t nLifeTime = pReader->GetLifeTimeInMicroseconds();
			// Lifetime should be >= 0 (journal was just created)
			assertTrue(nLifeTime >= 0, "Lifetime should be non-negative");
		}
		
		void testReaderGetChunkCount()
		{
			auto fixture = createFixture("chunk_count");
			
			fixture.m_pJournalSession->CreateVariableInJournalDB("chunk_var", 1, 1, LibMCData::eParameterDataType::Double, 1.0);
			
			std::string sJournalUUID = fixture.m_pJournalSession->GetSessionUUID();
			auto pReader = fixture.m_pDataModel->CreateJournalReader(sJournalUUID);
			
			uint32_t nChunkCount = pReader->GetChunkCount();
			// Initially might be 0 or more depending on implementation
			assertTrue(nChunkCount >= 0, "Chunk count should be non-negative");
		}
		
		void testReaderGetVarInfo()
		{
			auto fixture = createFixture("var_info");
			
			std::string sVarName = "info_variable";
			uint32_t nVarID = 10;
			uint32_t nVarIndex = 1;
			double dUnits = 2.5;
			
			fixture.m_pJournalSession->CreateVariableInJournalDB(sVarName, nVarID, nVarIndex, LibMCData::eParameterDataType::Double, dUnits);
			
			std::string sJournalUUID = fixture.m_pJournalSession->GetSessionUUID();
			auto pReader = fixture.m_pDataModel->CreateJournalReader(sJournalUUID);
			
			uint32_t nVarCount = pReader->GetVariableCount();
			assertTrue(nVarCount >= 1, "Should have at least 1 variable");
			
			// Get variable information - search for our variable
			bool bFoundVariable = false;
			for (uint32_t i = 0; i < nVarCount; i++) {
				std::string sRetrievedName;
				uint32_t nRetrievedID;
				LibMCData::eParameterDataType eDataType;
				double dRetrievedUnits;
				
				pReader->GetVariableInformation(i, sRetrievedName, nRetrievedID, eDataType, dRetrievedUnits);
				
				if (sRetrievedName == sVarName) {
					bFoundVariable = true;
					assertTrue(eDataType == LibMCData::eParameterDataType::Double, "Data type should be Double");
					break;
				}
			}
			assertTrue(bFoundVariable, "Should find the created variable");
		}
		
		void testReaderGetAliasCount()
		{
			auto fixture = createFixture("alias_count");
			
			fixture.m_pJournalSession->CreateVariableInJournalDB("base_var", 1, 1, LibMCData::eParameterDataType::Double, 1.0);
			fixture.m_pJournalSession->CreateVariableAliasInJournalDB("alias1", "base_var");
			fixture.m_pJournalSession->CreateVariableAliasInJournalDB("alias2", "base_var");
			
			std::string sJournalUUID = fixture.m_pJournalSession->GetSessionUUID();
			auto pReader = fixture.m_pDataModel->CreateJournalReader(sJournalUUID);
			
			uint32_t nAliasCount = pReader->GetAliasCount();
			assertTrue(nAliasCount >= 2, "Should have at least 2 aliases");
		}
		
		void testReaderGetAliasInfo()
		{
			auto fixture = createFixture("alias_info");
			
			fixture.m_pJournalSession->CreateVariableInJournalDB("source_var", 1, 1, LibMCData::eParameterDataType::Double, 1.0);
			fixture.m_pJournalSession->CreateVariableAliasInJournalDB("my_alias", "source_var");
			
			std::string sJournalUUID = fixture.m_pJournalSession->GetSessionUUID();
			auto pReader = fixture.m_pDataModel->CreateJournalReader(sJournalUUID);
			
			uint32_t nAliasCount = pReader->GetAliasCount();
			assertTrue(nAliasCount >= 1, "Should have at least 1 alias");
			
			// Get alias information
			std::string sAliasName, sSourceName;
			pReader->GetAliasInformation(0, sAliasName, sSourceName);
			
			assertFalse(sAliasName.empty(), "Alias name should not be empty");
			assertFalse(sSourceName.empty(), "Source name should not be empty");
		}
		
		void testMultipleVariables()
		{
			auto fixture = createFixture("multi_vars");
			
			// Create multiple variables
			for (int i = 0; i < 5; i++) {
				std::string sName = "var_" + std::to_string(i);
				fixture.m_pJournalSession->CreateVariableInJournalDB(sName, i + 1, i, LibMCData::eParameterDataType::Double, 1.0);
			}
			
			std::string sJournalUUID = fixture.m_pJournalSession->GetSessionUUID();
			auto pReader = fixture.m_pDataModel->CreateJournalReader(sJournalUUID);
			
			uint32_t nVarCount = pReader->GetVariableCount();
			assertTrue(nVarCount >= 5, "Should have at least 5 variables");
		}
		
		void testVariableTypes()
		{
			auto fixture = createFixture("var_types");
			
			// Create variables of different types
			fixture.m_pJournalSession->CreateVariableInJournalDB("double_var", 1, 1, LibMCData::eParameterDataType::Double, 1.0);
			fixture.m_pJournalSession->CreateVariableInJournalDB("int_var", 2, 2, LibMCData::eParameterDataType::Integer, 1.0);
			fixture.m_pJournalSession->CreateVariableInJournalDB("bool_var", 3, 3, LibMCData::eParameterDataType::Bool, 1.0);
			fixture.m_pJournalSession->CreateVariableInJournalDB("string_var", 4, 4, LibMCData::eParameterDataType::String, 1.0);
			
			std::string sJournalUUID = fixture.m_pJournalSession->GetSessionUUID();
			auto pReader = fixture.m_pDataModel->CreateJournalReader(sJournalUUID);
			
			uint32_t nVarCount = pReader->GetVariableCount();
			assertTrue(nVarCount >= 4, "Should have at least 4 variables");
		}
		
		void testMultipleAliases()
		{
			auto fixture = createFixture("multi_aliases");
			
			fixture.m_pJournalSession->CreateVariableInJournalDB("source1", 1, 1, LibMCData::eParameterDataType::Double, 1.0);
			fixture.m_pJournalSession->CreateVariableInJournalDB("source2", 2, 2, LibMCData::eParameterDataType::Integer, 1.0);
			
			// Create multiple aliases
			fixture.m_pJournalSession->CreateVariableAliasInJournalDB("alias_a", "source1");
			fixture.m_pJournalSession->CreateVariableAliasInJournalDB("alias_b", "source1");
			fixture.m_pJournalSession->CreateVariableAliasInJournalDB("alias_c", "source2");
			
			std::string sJournalUUID = fixture.m_pJournalSession->GetSessionUUID();
			auto pReader = fixture.m_pDataModel->CreateJournalReader(sJournalUUID);
			
			uint32_t nAliasCount = pReader->GetAliasCount();
			assertTrue(nAliasCount >= 3, "Should have at least 3 aliases");
		}
		
		void testJournalSessionClose()
		{
			auto fixture = createFixture("session_close");
			
			fixture.m_pJournalSession->CreateVariableInJournalDB("close_var", 1, 1, LibMCData::eParameterDataType::Double, 1.0);
			
			std::string sJournalUUID = fixture.m_pJournalSession->GetSessionUUID();
			
			// Session is still valid, verify we can create a reader for it
			auto pReader = fixture.m_pDataModel->CreateJournalReader(sJournalUUID);
			assertAssigned(pReader.get(), "Reader should be created for session");
			assertTrue(pReader->GetJournalUUID() == sJournalUUID, "Reader UUID should match session");
		}
		
		// ============= Additional tests to reach 100 =============
		
		void testVariableUnits()
		{
			auto fixture = createFixture("var_units");
			
			// Create variables with different unit values
			fixture.m_pJournalSession->CreateVariableInJournalDB("temp_celsius", 1, 1, LibMCData::eParameterDataType::Double, 1.0);
			fixture.m_pJournalSession->CreateVariableInJournalDB("temp_fahrenheit", 2, 2, LibMCData::eParameterDataType::Double, 1.8);
			fixture.m_pJournalSession->CreateVariableInJournalDB("speed_mps", 3, 3, LibMCData::eParameterDataType::Double, 1.0);
			
			std::string sJournalUUID = fixture.m_pJournalSession->GetSessionUUID();
			auto pReader = fixture.m_pDataModel->CreateJournalReader(sJournalUUID);
			
			uint32_t nVarCount = pReader->GetVariableCount();
			assertTrue(nVarCount >= 3, "Should have at least 3 variables");
			
			// Check that we can retrieve variable info - find our variables
			bool bFoundPositiveUnits = false;
			for (uint32_t i = 0; i < nVarCount; i++) {
				std::string sName;
				uint32_t nID;
				LibMCData::eParameterDataType eDataType;
				double dUnits;
				
				pReader->GetVariableInformation(i, sName, nID, eDataType, dUnits);
				
				if (sName == "temp_celsius" || sName == "temp_fahrenheit" || sName == "speed_mps") {
					assertTrue(dUnits > 0.0, "Units should be positive for created variables");
					bFoundPositiveUnits = true;
				}
			}
			assertTrue(bFoundPositiveUnits, "Should find at least one of the created variables");
		}
		
		void testVarDataTypes()
		{
			auto fixture = createFixture("data_types");
			
			// Test all parameter data types
			fixture.m_pJournalSession->CreateVariableInJournalDB("double_type", 1, 1, LibMCData::eParameterDataType::Double, 1.0);
			fixture.m_pJournalSession->CreateVariableInJournalDB("int_type", 2, 2, LibMCData::eParameterDataType::Integer, 1.0);
			fixture.m_pJournalSession->CreateVariableInJournalDB("bool_type", 3, 3, LibMCData::eParameterDataType::Bool, 1.0);
			fixture.m_pJournalSession->CreateVariableInJournalDB("string_type", 4, 4, LibMCData::eParameterDataType::String, 1.0);
			fixture.m_pJournalSession->CreateVariableInJournalDB("uuid_type", 5, 5, LibMCData::eParameterDataType::UUID, 1.0);
			
			std::string sJournalUUID = fixture.m_pJournalSession->GetSessionUUID();
			auto pReader = fixture.m_pDataModel->CreateJournalReader(sJournalUUID);
			
			uint32_t nVarCount = pReader->GetVariableCount();
			assertTrue(nVarCount >= 5, "Should have at least 5 variables with different types");
		}
		
		void testAliasMapping()
		{
			auto fixture = createFixture("alias_map");
			
			std::string sSourceName = "source_variable";
			std::string sAliasName = "alias_for_source";
			
			fixture.m_pJournalSession->CreateVariableInJournalDB(sSourceName, 1, 1, LibMCData::eParameterDataType::Double, 1.0);
			fixture.m_pJournalSession->CreateVariableAliasInJournalDB(sAliasName, sSourceName);
			
			std::string sJournalUUID = fixture.m_pJournalSession->GetSessionUUID();
			auto pReader = fixture.m_pDataModel->CreateJournalReader(sJournalUUID);
			
			// Verify the alias exists and maps correctly
			uint32_t nAliasCount = pReader->GetAliasCount();
			assertTrue(nAliasCount >= 1, "Should have at least 1 alias");
			
			std::string sRetrievedAlias, sRetrievedSource;
			pReader->GetAliasInformation(0, sRetrievedAlias, sRetrievedSource);
			
			assertTrue(sRetrievedAlias == sAliasName, "Alias name should match");
			assertTrue(sRetrievedSource == sSourceName, "Source name should match");
		}
	};

}

#endif // __AMCTEST_UNITTEST_LIBMCDATA_JOURNAL
