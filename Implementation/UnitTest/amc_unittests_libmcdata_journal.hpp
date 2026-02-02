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
	};

}

#endif // __AMCTEST_UNITTEST_LIBMCDATA_JOURNAL
