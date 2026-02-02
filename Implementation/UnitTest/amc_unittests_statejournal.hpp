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

#ifndef __AMCTEST_UNITTEST_STATEJOURNAL
#define __AMCTEST_UNITTEST_STATEJOURNAL

#include "amc_unittests.hpp"
#include "amc_statejournal.hpp"
#include "amc_statejournalstream.hpp"
#include "common_utils.hpp"
#include "common_chrono.hpp"
#include "libmc_exceptiontypes.hpp"
#include "libmc_interfaceexception.hpp"
#include "libmcdata_dynamic.hpp"
#include "amc_unittests_libmcdata.hpp"

#include <memory>
#include <string>

namespace AMCUnitTest {

	class CUnitTestGroup_StateJournal : public CUnitTestGroup {
	public:
		CUnitTestGroup_StateJournal() = default;
		virtual ~CUnitTestGroup_StateJournal() = default;

		std::string getTestGroupName() override {
			return "StateJournal";
		}

		void registerTests() override {
			registerTest("BasicRecordAndSample", "Records bool/int/double values and samples them by timestamp", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_StateJournal::test_BasicRecordAndSample, this));
			registerTest("AliasAndStringErrors", "Uses aliases for sampling and rejects non-numeric variables", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_StateJournal::test_AliasAndStringErrors, this));
			registerTest("LifecycleErrors", "Validates state journal lifecycle errors", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_StateJournal::test_LifecycleErrors, this));
		}

		void initializeTests() override {
		}

	private:
#define __STRINGIZE(x) #x
#define __STRINGIZE_VALUE_OF(x) __STRINGIZE(x)

		typedef struct _sStateJournalFixture {
			LibMCData::PWrapper m_pDataWrapper;
			LibMCData::PDataModel m_pDataModel;
			LibMCData::PJournalSession m_pJournalSession;
			AMCCommon::PChrono m_pChrono;
			AMC::PStateJournalStream m_pStream;
			AMC::PStateJournal m_pJournal;
		} sStateJournalFixture;

		sStateJournalFixture createFixture(const std::string& sSuffix) {
			sStateJournalFixture fixture;

			std::string sRootPath = "temp";
			if (!AMCCommon::CUtils::fileOrPathExistsOnDisk(sRootPath))
				AMCCommon::CUtils::createDirectoryOnDisk(sRootPath);

			std::string sBasePath = sRootPath + "/statejournal_" + sSuffix;
			if (!AMCCommon::CUtils::fileOrPathExistsOnDisk(sBasePath))
				AMCCommon::CUtils::createDirectoryOnDisk(sBasePath);

			std::string sDatabaseFile = sBasePath + "/statejournal.db";

			fixture.m_pDataWrapper = AMCUnitTest::loadLibMCDataInProcess();
			fixture.m_pDataModel = fixture.m_pDataWrapper->CreateDataModelInstance();
			fixture.m_pDataModel->InitialiseDatabase(sBasePath, LibMCData::eDataBaseType::SqLite, sDatabaseFile);

			fixture.m_pJournalSession = fixture.m_pDataModel->CreateJournalSession();
			fixture.m_pChrono = std::make_shared<AMCCommon::CChrono>();
			fixture.m_pStream = std::make_shared<AMC::CStateJournalStream>(fixture.m_pJournalSession, nullptr, false);
			fixture.m_pJournal = std::make_shared<AMC::CStateJournal>(fixture.m_pStream, fixture.m_pChrono);

			return fixture;
		}

		void test_BasicRecordAndSample() {
			auto fixture = createFixture(AMCCommon::CUtils::createUUID());

			uint32_t nBoolID = fixture.m_pJournal->registerBooleanValue("enabled", false);
			uint32_t nIntegerID = fixture.m_pJournal->registerIntegerValue("count", 0);
			uint32_t nDoubleID = fixture.m_pJournal->registerDoubleValue("temperature", 0.0, 0.1);

			fixture.m_pJournal->startRecording();

			AMCCommon::CChrono::sleepMicroseconds(1000);
			fixture.m_pJournal->updateBoolValue(nBoolID, true);
			uint64_t nBoolTime = fixture.m_pJournal->getLifeTimeInMicroseconds();
			assertDoubleRange(fixture.m_pJournal->computeSample("enabled", nBoolTime), 1.0, 1.0, "Bool sample must be 1 after update");

			AMCCommon::CChrono::sleepMicroseconds(1000);
			fixture.m_pJournal->updateIntegerValue(nIntegerID, 42);
			uint64_t nIntTime = fixture.m_pJournal->getLifeTimeInMicroseconds();
			assertDoubleRange(fixture.m_pJournal->computeSample("count", nIntTime), 42.0, 42.0, "Integer sample must match update");

			AMCCommon::CChrono::sleepMicroseconds(1000);
			fixture.m_pJournal->updateDoubleValue(nDoubleID, 12.3);
			uint64_t nDoubleTime = fixture.m_pJournal->getLifeTimeInMicroseconds();
			double dSample = fixture.m_pJournal->computeSample("temperature", nDoubleTime);
			assertDoubleRange(dSample, 12.29, 12.31, "Double sample must match update within units");

			fixture.m_pJournal->finishRecording();
		}

		void test_AliasAndStringErrors() {
			auto fixture = createFixture(AMCCommon::CUtils::createUUID());

			uint32_t nSpeedID = fixture.m_pJournal->registerIntegerValue("speed", 0);
			uint32_t nStatusID = fixture.m_pJournal->registerStringValue("status", "idle");
			fixture.m_pJournal->registerAlias("speed_alias", "speed");

			fixture.m_pJournal->startRecording();

			AMCCommon::CChrono::sleepMicroseconds(1000);
			fixture.m_pJournal->updateIntegerValue(nSpeedID, 250);
			uint64_t nSpeedTime = fixture.m_pJournal->getLifeTimeInMicroseconds();
			assertDoubleRange(fixture.m_pJournal->computeSample("speed_alias", nSpeedTime), 250.0, 250.0, "Alias sampling must match source variable");

			fixture.m_pJournal->updateStringValue(nStatusID, "running");

			bool bThrewOnStringSample = false;
			try {
				fixture.m_pJournal->computeSample("status", nSpeedTime);
			}
			catch (const std::exception&) {
				bThrewOnStringSample = true;
			}

			assertTrue(bThrewOnStringSample, "Sampling a string variable must throw");

			fixture.m_pJournal->finishRecording();
		}

		void test_LifecycleErrors() {
			auto fixture = createFixture(AMCCommon::CUtils::createUUID());

			uint32_t nCountID = fixture.m_pJournal->registerIntegerValue("count", 0);

			bool bThrewOnUpdateBeforeStart = false;
			try {
				fixture.m_pJournal->updateIntegerValue(nCountID, 1);
			}
			catch (const std::exception&) {
				bThrewOnUpdateBeforeStart = true;
			}
			assertTrue(bThrewOnUpdateBeforeStart, "Updating before startRecording must throw");

			fixture.m_pJournal->startRecording();

			bool bThrewOnRegisterAfterStart = false;
			try {
				fixture.m_pJournal->registerIntegerValue("late", 1);
			}
			catch (const std::exception&) {
				bThrewOnRegisterAfterStart = true;
			}
			assertTrue(bThrewOnRegisterAfterStart, "Registering after startRecording must throw");

			fixture.m_pJournal->finishRecording();

			bool bThrewOnUpdateAfterFinish = false;
			try {
				fixture.m_pJournal->updateIntegerValue(nCountID, 2);
			}
			catch (const std::exception&) {
				bThrewOnUpdateAfterFinish = true;
			}
			assertTrue(bThrewOnUpdateAfterFinish, "Updating after finishRecording must throw");
		}

	};

}

#endif // __AMCTEST_UNITTEST_STATEJOURNAL
