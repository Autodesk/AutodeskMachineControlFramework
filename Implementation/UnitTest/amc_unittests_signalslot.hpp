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

#ifndef __AMCTEST_UNITTEST_SIGNALSLOT
#define __AMCTEST_UNITTEST_SIGNALSLOT

#include "amc_unittests.hpp"

#define _STATESIGNAL_HEADERPROTECTION
#include "amc_statesignal.hpp"
#undef _STATESIGNAL_HEADERPROTECTION

#include <thread>
#include <sstream>
#include <iomanip>
#include <atomic>
#include <vector>
#include <list>
#include <mutex>
#include "common_utils.hpp"
#include "common_chrono.hpp"
#include "amc_telemetry.hpp"
#include "libmcdata_dynamic.hpp"
#include "amc_unittests_libmcdata.hpp"


namespace AMCUnitTest {

    class CDummyRegistry : public AMC::CStateSignalRegistry {

    public:

        CDummyRegistry()
        {
            initializeTelemetry();
        }

        void registerMessage(const std::string& sMessageUUID, AMC::CStateSignalSlot* pSignalSlot) override
        {

        }

        void unregisterMessage(const std::string& sMessageUUID) override
        {

        }

        AMC::PStateSignalSlot findSignalSlotOfMessage(const std::string& sMessageUUID)
        {
            return nullptr;
        }

        AMC::PTelemetryChannel registerTelemetryChannel(const std::string& sChannelIdentifier, const std::string& sChannelDescription, LibMCData::eTelemetryChannelType channelType) override
        {
            return m_pTelemetryHandler->registerChannel(sChannelIdentifier, sChannelDescription, channelType);
        }

    private:
        void initializeTelemetry()
        {
            std::string sRootPath = "temp";
            if (!AMCCommon::CUtils::fileOrPathExistsOnDisk(sRootPath))
                AMCCommon::CUtils::createDirectoryOnDisk(sRootPath);

            std::string sBasePath = sRootPath + "/signalslot_" + AMCCommon::CUtils::createUUID();
            if (!AMCCommon::CUtils::fileOrPathExistsOnDisk(sBasePath))
                AMCCommon::CUtils::createDirectoryOnDisk(sBasePath);

            std::string sDatabaseFile = sBasePath + "/signalslot.db";

            m_pDataWrapper = AMCUnitTest::loadLibMCDataInProcess();
            m_pDataModel = m_pDataWrapper->CreateDataModelInstance();
            m_pDataModel->InitialiseDatabase(sBasePath, LibMCData::eDataBaseType::SqLite, sDatabaseFile);
            m_pTelemetrySession = m_pDataModel->CreateTelemetrySession();

            m_pChrono = std::make_shared<AMCCommon::CChrono>();
            m_pTelemetryWriter = std::make_shared<AMC::CTelemetryWriter>(m_pTelemetrySession, m_pChrono);
            m_pTelemetryHandler = std::make_shared<AMC::CTelemetryHandler>(m_pTelemetryWriter);
        }

        LibMCData::PWrapper m_pDataWrapper;
        LibMCData::PDataModel m_pDataModel;
        LibMCData::PTelemetrySession m_pTelemetrySession;
        AMCCommon::PChrono m_pChrono;
        AMC::PTelemetryWriter m_pTelemetryWriter;
        std::shared_ptr<AMC::CTelemetryHandler> m_pTelemetryHandler;

    };


class CUnitTestGroup_SignalSlot : public CUnitTestGroup {
public:
    CUnitTestGroup_SignalSlot() = default;
    virtual ~CUnitTestGroup_SignalSlot() = default;

    std::string getTestGroupName() override {
        return "SignalSlot";
    }

    void initializeTests() override {
        // Nothing to initialize
    }

    void registerTests() override {
        registerTest("CreateSignalSlotBasic", "Creates a signal slot with parameters", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_SignalSlot::test_CreateSignalSlotBasic, this));
        registerTest("AddSignalToQueue", "Adds a signal to the queue and verifies availability", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_SignalSlot::test_AddSignalToQueue, this));
        registerTest("SignalPhaseTransition", "Tests InQueue → InProcess → Handled", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_SignalSlot::test_SignalPhaseTransition, this));
        registerTest("SignalFailureTransition", "Tests InQueue → Failed", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_SignalSlot::test_SignalFailureTransition, this));
        registerTest("QueueOverflow", "Tests rejection of signal if queue is full", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_SignalSlot::test_QueueOverflow, this));
        registerTest("ParameterResultAccess", "Tests getting parameter and result JSON", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_SignalSlot::test_ParameterResultAccess, this));
        registerTest("ClearQueueWorks", "Clears the queue and marks signals as cleared", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_SignalSlot::test_ClearQueueWorks, this));
        registerTest("TimeoutAndOverflowTest", "Simulates queue overflow and timeout scenarios", eUnitTestCategory::utOptionalPass, std::bind(&CUnitTestGroup_SignalSlot::test_TimeoutAndOverflowTest, this));
    }

private:

    void test_CreateSignalSlotBasic() {
        std::vector<AMC::CStateSignalParameter> params;
        std::vector<AMC::CStateSignalParameter> results;
        params.emplace_back("p1", "string", true);
        results.emplace_back("r1", "int", true);

        CDummyRegistry registry;

        AMC::CStateSignalSlot slot("instance", "signal", params, results, 500, 5000, 10, nullptr, &registry);

        assertTrue(slot.getNameInternal() == "signal");
        assertTrue(slot.getInstanceNameInternal() == "instance");
        assertTrue(slot.getDefaultReactionTimeoutInternal() == 500);
        assertTrue(slot.getTotalSignalQueueSizeInternal() == 10);
    }

    void test_AddSignalToQueue() {
        CDummyRegistry registry;

        AMC::CStateSignalSlot slot("instance", "signal", {}, {}, 1000, 5000, 5, nullptr, &registry);
        std::string uuid = "11111111-2222-3333-4444-555555555555";

       
        AMCCommon::CChrono chrono;

        chrono.sleepMicroseconds(500);

        bool added = slot.addNewInQueueSignalInternal(uuid, "{\"param\":\"value\"}", 500, chrono.getElapsedMicroseconds ()) != nullptr;
        assertTrue(added);

        assertIntegerRange(slot.getAvailableSignalQueueEntriesInternal(), 0, 4);
        assertTrue(slot.getSignalPhaseInternal(uuid) == AMC::eAMCSignalPhase::InQueue);
    }

    void test_SignalPhaseTransition() {

        CDummyRegistry registry;

        AMC::CStateSignalSlot slot("instance", "signal", {}, {}, 1000, 5000, 3, nullptr, &registry);
        std::string uuid = "77777777-8888-9999-aaaa-bbbbbbbbbbbb";

        AMCCommon::CChrono chrono;

        chrono.sleepMicroseconds(500);

        auto pMessage = slot.addNewInQueueSignalInternal(uuid, "{}", 500, chrono.getElapsedMicroseconds());
        assertTrue(pMessage != nullptr);
        chrono.sleepMicroseconds(500);

        assertTrue(slot.changeSignalPhaseToInProcessInternal(uuid, chrono.getElapsedMicroseconds()));
        assertTrue(slot.getSignalPhaseInternal(uuid) == AMC::eAMCSignalPhase::InProcess);

        chrono.sleepMicroseconds(1000);

        assertTrue(slot.changeSignalPhaseToHandledInternal(uuid, "{\"result\":true}", chrono.getElapsedMicroseconds()));
        assertTrue(slot.getSignalPhaseInternal(uuid) == AMC::eAMCSignalPhase::Handled);

        assertTrue(slot.getResultDataJSONInternal(uuid) == "{\"result\":true}");
    }

    void test_SignalFailureTransition() {

        CDummyRegistry registry;

        AMC::CStateSignalSlot slot("instance", "signal", {}, {}, 1000, 5000, 2, nullptr, &registry);
        std::string uuid = "deadbeef-dead-beef-dead-beefdeadbeef";

        AMCCommon::CChrono chrono;
        chrono.sleepMicroseconds(500);

        auto pMessage = slot.addNewInQueueSignalInternal(uuid, "{}", 500, chrono.getElapsedMicroseconds());
        assertTrue(pMessage != nullptr);

        chrono.sleepMicroseconds(500);
        assertTrue(slot.changeSignalPhaseToInFailedInternal(uuid, "{\"ok\":false}", "error", chrono.getElapsedMicroseconds()));
        assertTrue(slot.getSignalPhaseInternal(uuid) == AMC::eAMCSignalPhase::Failed);
    }

    void test_QueueOverflow() {

        CDummyRegistry registry;

        AMCCommon::CChrono chrono;
        chrono.sleepMicroseconds(500);

        AMC::CStateSignalSlot slot("instance", "signal", {}, {}, 1000, 5000, 1, nullptr, &registry);

        auto pMessage1 = slot.addNewInQueueSignalInternal("00000001-0000-0000-0000-000000000001", "{}", 500, chrono.getElapsedMicroseconds());
        assertTrue(pMessage1 != nullptr);
        chrono.sleepMicroseconds(500);

        auto pMessage2 = slot.addNewInQueueSignalInternal("00000002-0000-0000-0000-000000000002", "{}", 500, chrono.getElapsedMicroseconds());
        assertFalse(pMessage2 != nullptr); // queue full
    }


    void test_ParameterResultAccess() {

        CDummyRegistry registry;

        AMCCommon::CChrono chrono;
        chrono.sleepMicroseconds(500);

        AMC::CStateSignalSlot slot("instance", "signal", {}, {}, 1000, 5000, 2, nullptr, &registry);
        std::string uuid = "e5e57000-0000-0000-0000-000000000001";

        chrono.sleepMicroseconds(500);
        slot.addNewInQueueSignalInternal(uuid, "{\"param\":\"abc\"}", 1000, chrono.getElapsedMicroseconds());

        chrono.sleepMicroseconds(500);
        assertTrue(slot.getParameterDataJSONInternal(uuid) == "{\"param\":\"abc\"}");

        chrono.sleepMicroseconds(500);
        slot.changeSignalPhaseToHandledInternal(uuid, "{\"result\":123}", chrono.getElapsedMicroseconds());
        assertTrue(slot.getResultDataJSONInternal(uuid) == "{\"result\":123}");
    }

    void test_ClearQueueWorks() {

        CDummyRegistry registry;

        AMCCommon::CChrono chrono;
        chrono.sleepMicroseconds(500);

        AMC::CStateSignalSlot slot("instance", "signal", {}, {}, 1000, 5000, 2, nullptr, &registry);

        chrono.sleepMicroseconds(500);
        slot.addNewInQueueSignalInternal("11110001-0000-0000-0000-000000000001", "{}", 500, chrono.getElapsedMicroseconds());

        chrono.sleepMicroseconds(500);
        slot.addNewInQueueSignalInternal("22220002-0000-0000-0000-000000000002", "{}", 500, chrono.getElapsedMicroseconds());

        chrono.sleepMicroseconds(500);
        slot.clearQueueInternal();
        assertTrue(slot.getAvailableSignalQueueEntriesInternal() == 2);
    }


    void test_TimeoutAndOverflowTest() {

        CDummyRegistry registry;

        AMCCommon::CChrono chrono;

        chrono.sleepMicroseconds(500);

        const int capacity = 10;
        const int total = 15;
        AMC::CStateSignalSlot slot("overflowInstance", "overflowSignal", {}, {}, 50, 5000, capacity, nullptr, &registry);

        std::vector<std::string> accepted, rejected;

        for (int i = 0; i < total; ++i) {
            std::stringstream ss;
            ss << "f10" << std::setfill('0') << std::setw(5) << i << "-0000-0000-0000-000000000000";
            std::string uuid = ss.str();

            auto pMessage = slot.addNewInQueueSignalInternal(uuid, "{}", 50, chrono.getElapsedMicroseconds());
			bool ok = pMessage != nullptr;
            if (ok) accepted.push_back(uuid);
            else rejected.push_back(uuid);
        }

        chrono.sleepMilliseconds(200);

        int timedOut = 0;
        for (const auto& uuid : accepted) {
            if (slot.getSignalPhaseInternal(uuid) == AMC::eAMCSignalPhase::InQueue) {
                timedOut++;
            }
        }

        assertTrue(accepted.size() == capacity);
        assertTrue(rejected.size() == total - capacity);
        assertTrue(timedOut > 0);
    }
};


}


#endif // __AMCTEST_UNITTEST_SIGNALSLOT
