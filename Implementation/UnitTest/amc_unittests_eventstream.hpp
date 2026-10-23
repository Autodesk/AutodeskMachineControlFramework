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


#ifndef __AMCTEST_UNITTEST_EVENTSTREAM
#define __AMCTEST_UNITTEST_EVENTSTREAM


#include "amc_unittests.hpp"
#include "amc_jsoneventstreaminstance.hpp"
#include "amc_streamregistry.hpp"
#include "amc_ui_frontendsnapshot.hpp"
#include "common_utils.hpp"

#include "RapidJSON/document.h"

#include <chrono>
#include <memory>
#include <thread>


namespace AMCUnitTest {

	class CUnitTestEventStreamInstance : public AMC::CJSONEventStreamInstance {
	public:

		CUnitTestEventStreamInstance(const std::string& sUUID)
			: AMC::CJSONEventStreamInstance(sUUID)
		{
		}

		std::string waitForNextEvent(AMC::sJSONEventStreamCursor& cursor) override
		{
			cursor.m_nChangeCounter = waitForChange(cursor.m_nChangeCounter, 10);
			return "";
		}

		uint64_t testWaitForChange(uint64_t nKnownChangeCounter, uint32_t nTimeoutInMilliseconds)
		{
			return waitForChange(nKnownChangeCounter, nTimeoutInMilliseconds);
		}

		uint64_t testGetChangeCounter()
		{
			return getChangeCounter();
		}
	};

	class CUnitTestGroup_EventStream : public CUnitTestGroup {
	public:

		std::string getTestGroupName() override {
			return "EventStream";
		}

		void registerTests() override {
			registerTest("EventFraming", "SSE events carry event name, id and one data line per payload line", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_EventStream::testEventFraming, this));
			registerTest("ChangeSignal", "A change notification or the end of the stream wakes waiting connections", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_EventStream::testChangeSignal, this));
			registerTest("StreamTickets", "Stream tickets are single-use, expire and do not open ended streams", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_EventStream::testStreamTickets, this));
			registerTest("RemoveEndedStreams", "Ended JSON event streams are unregistered", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_EventStream::testRemoveEndedStreams, this));
			registerTest("InitialRevision", "A revision log with an initial revision ignores revisions below it", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_EventStream::testInitialRevision, this));
			registerTest("SnapshotJSON", "Snapshot serializes all stores with their raw attribute values", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_EventStream::testSnapshotJSON, this));
		}

		void initializeTests() override {
		}

	private:

		void testEventFraming()
		{
			std::string sEvent = AMC::CJSONEventStreamInstance::formatEvent("patch", 42, "{\"a\":1}");
			assertTrue(sEvent == "event: patch\nid: 42\ndata: {\"a\":1}\n\n");

			std::string sMultiLineEvent = AMC::CJSONEventStreamInstance::formatEvent("snapshot", 7, "line1\r\nline2\n");
			assertTrue(sMultiLineEvent == "event: snapshot\nid: 7\ndata: line1\ndata: line2\ndata: \n\n");

			std::string sUnnamedEvent = AMC::CJSONEventStreamInstance::formatEvent("", 1, "x");
			assertTrue(sUnnamedEvent == "id: 1\ndata: x\n\n");

			assertTrue(AMC::CJSONEventStreamInstance::formatComment("keep\nalive") == ": keepalive\n\n");
		}

		void testChangeSignal()
		{
			auto pStream = std::make_shared<CUnitTestEventStreamInstance>(AMCCommon::CUtils::createUUID());
			assertTrue(pStream->getStreamType() == AMC::eStreamType::JSONEventStream);
			assertTrue(pStream->isActive());

			uint64_t nCounter = pStream->testGetChangeCounter();
			assertTrue(pStream->testWaitForChange(nCounter, 10) == nCounter);

			pStream->notifyChange();
			auto startTime = std::chrono::steady_clock::now();
			uint64_t nNewCounter = pStream->testWaitForChange(nCounter, 10000);
			assertTrue(nNewCounter != nCounter);

			std::thread notifyThread([pStream]() {
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
				pStream->endStream();
			});
			pStream->testWaitForChange(nNewCounter, 10000);
			notifyThread.join();

			auto nElapsedInMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime).count();
			assertTrue(nElapsedInMs < 5000);
			assertFalse(pStream->isActive());

			// An ended stream does not block anymore.
			uint64_t nEndedCounter = pStream->testGetChangeCounter();
			startTime = std::chrono::steady_clock::now();
			pStream->testWaitForChange(nEndedCounter, 10000);
			nElapsedInMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime).count();
			assertTrue(nElapsedInMs < 5000);
		}

		void testStreamTickets()
		{
			AMC::CStreamRegistry registry;
			auto pStream = std::make_shared<CUnitTestEventStreamInstance>(AMCCommon::CUtils::createUUID());
			registry.registerStream(pStream);

			uint64_t nNow = 1000000000;
			uint64_t nLifetime = 30000000;

			std::string sClientID = AMCCommon::CUtils::createUUID();
			std::string sRedeemedClientID;

			std::string sTicket = registry.createStreamTicket(pStream->getUUID(), sClientID, nNow, nLifetime);
			assertTrue(AMCCommon::CUtils::stringIsUUIDString(sTicket));
			assertTrue(sTicket != pStream->getUUID());

			auto pRedeemed = registry.redeemStreamTicket(sTicket, nNow + 1, sRedeemedClientID);
			assertTrue(pRedeemed.get() == pStream.get());
			assertTrue(sRedeemedClientID == sClientID);
			assertTrue(registry.redeemStreamTicket(sTicket, nNow + 2, sRedeemedClientID).get() == nullptr);
			assertTrue(sRedeemedClientID.empty());

			std::string sExpiredTicket = registry.createStreamTicket(pStream->getUUID(), "", nNow, nLifetime);
			assertTrue(registry.redeemStreamTicket(sExpiredTicket, nNow + nLifetime, sRedeemedClientID).get() == nullptr);

			assertTrue(registry.redeemStreamTicket("not-a-ticket", nNow, sRedeemedClientID).get() == nullptr);
			assertTrue(registry.redeemStreamTicket(AMCCommon::CUtils::createUUID(), nNow, sRedeemedClientID).get() == nullptr);

			std::string sEndedTicket = registry.createStreamTicket(pStream->getUUID(), "", nNow, nLifetime);
			pStream->endStream();
			assertTrue(registry.redeemStreamTicket(sEndedTicket, nNow + 1, sRedeemedClientID).get() == nullptr);

			bool bThrown = false;
			try {
				registry.createStreamTicket(AMCCommon::CUtils::createUUID(), "", nNow, nLifetime);
			}
			catch (...) {
				bThrown = true;
			}
			assertTrue(bThrown, "ticket for an unregistered stream");
		}

		void testRemoveEndedStreams()
		{
			AMC::CStreamRegistry registry;
			auto pActiveStream = std::make_shared<CUnitTestEventStreamInstance>(AMCCommon::CUtils::createUUID());
			auto pEndedStream = std::make_shared<CUnitTestEventStreamInstance>(AMCCommon::CUtils::createUUID());
			registry.registerStream(pActiveStream);
			registry.registerStream(pEndedStream);

			pEndedStream->endStream();
			registry.removeEndedJSONEventStreams();

			assertTrue(registry.hasStream(pActiveStream->getUUID()));
			assertFalse(registry.hasStream(pEndedStream->getUUID()));
		}

		void testInitialRevision()
		{
			uint64_t nInitialRevision = 5ULL << 32;
			AMC::CUIFrontendRevisionLog revisionLog(AMC_UI_FRONTEND_REVISIONLOG_DEPTH, nInitialRevision);

			AMC::CUIFrontendSnapshot snapshotA;
			snapshotA.setValue("store1", "value", "1");
			auto firstResult = revisionLog.publish(snapshotA, "*", 0);
			assertTrue(firstResult.m_nRevision == nInitialRevision + 1);
			assertFalse(firstResult.m_bPatchAvailable);

			AMC::CUIFrontendSnapshot snapshotB;
			snapshotB.setValue("store1", "value", "2");
			auto secondResult = revisionLog.publish(snapshotB, "*", firstResult.m_nRevision);
			assertTrue(secondResult.m_nRevision == nInitialRevision + 2);
			assertTrue(secondResult.m_bPatchAvailable);

			assertFalse(revisionLog.publish(snapshotB, "*", 3).m_bPatchAvailable);
			assertFalse(revisionLog.publish(snapshotB, "*", nInitialRevision).m_bPatchAvailable);
		}

		void testSnapshotJSON()
		{
			AMC::CUIFrontendSnapshot snapshot;
			snapshot.setValue("store1", "number", "42");
			snapshot.setValue("store1", "caption", "\"text\"");
			snapshot.setValue("store2", "list", "[1,2]");

			AMC::CJSONWriter writer;
			AMC::CJSONWriterObject storesObject(writer);
			snapshot.writeToJSON(writer, storesObject);
			writer.addObject("stores", storesObject);

			rapidjson::Document document;
			document.Parse(writer.saveToString().c_str());
			assertFalse(document.HasParseError());

			auto& stores = document["stores"];
			assertTrue(stores["store1"]["number"].GetInt() == 42);
			assertTrue(stores["store1"]["caption"].GetString() == std::string("text"));
			assertTrue(stores["store2"]["list"][1].GetInt() == 2);
			assertTrue(stores.MemberCount() == 2);
		}
	};

}

#endif // __AMCTEST_UNITTEST_EVENTSTREAM
