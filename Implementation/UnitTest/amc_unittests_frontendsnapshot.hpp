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


#ifndef __AMCTEST_UNITTEST_FRONTENDSNAPSHOT
#define __AMCTEST_UNITTEST_FRONTENDSNAPSHOT


#include "amc_unittests.hpp"
#include "amc_ui_frontendsnapshot.hpp"

#include "RapidJSON/document.h"

#include <vector>


namespace AMCUnitTest {

	class CUnitTestGroup_FrontendSnapshot : public CUnitTestGroup {
	public:

		std::string getTestGroupName() override {
			return "FrontendSnapshot";
		}

		void registerTests() override {
			registerTest("ReadFrontendStatus", "Snapshot extracts items, page headers and nested module attributes", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_FrontendSnapshot::testReadFrontendStatus, this));
			registerTest("PatchTransformsSnapshot", "Applying the diff of A and B to A yields B", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_FrontendSnapshot::testPatchTransformsSnapshot, this));
			registerTest("UnchangedProducesEmptyPatch", "Unchanged attributes produce an empty patch and no new revision", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_FrontendSnapshot::testUnchangedProducesEmptyPatch, this));
			registerTest("MergedPatchCatchesUp", "A merged patch moves any buffered revision to the current one", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_FrontendSnapshot::testMergedPatchCatchesUp, this));
			registerTest("OldRevisionFallsBack", "Revisions older than the buffer, unknown revisions and scope changes require a snapshot", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_FrontendSnapshot::testOldRevisionFallsBack, this));
			registerTest("PatchJSON", "Patch serializes changed values and removals as null", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_FrontendSnapshot::testPatchJSON, this));
		}

		void initializeTests() override {
		}

	private:

		static AMC::CUIFrontendSnapshot snapshotFromJSON(const std::string& sJSON)
		{
			rapidjson::Document document;
			document.Parse(sJSON.c_str());

			AMC::CUIFrontendSnapshot snapshot;
			snapshot.readFromFrontendJSON(document);
			return snapshot;
		}

		static std::string statusJSON(const std::string& sCaption, const std::string& sLabel, int nCounter, bool bMenuVisible)
		{
			return std::string("{\"protocol\":\"p\",\"menuitems\":[{\"uuid\":\"m1\",\"caption\":\"Menu\",\"visible\":") + (bMenuVisible ? "true" : "false") + "}],"
				"\"pages\":[{\"name\":\"main\",\"uuid\":\"p1\",\"caption\":\"" + sCaption + "\",\"visible\":true,\"modules\":["
				"{\"moduletype\":\"content\",\"uuid\":\"mod1\",\"name\":\"content\",\"attributes\":{\"label\":\"" + sLabel + "\"},"
				"\"submodules\":[{\"moduletype\":\"paragraph\",\"uuid\":\"item1\",\"attributes\":{\"counter\":" + std::to_string(nCounter) + ",\"list\":[1,2]}}]}]}]}";
		}

		void testReadFrontendStatus()
		{
			auto snapshot = snapshotFromJSON(statusJSON("Main", "Hello", 3, true));

			assertTrue(snapshot.getValue("m1", "caption") == "\"Menu\"");
			assertTrue(snapshot.getValue("m1", "visible") == "true");
			assertTrue(snapshot.getValue("p1", "name") == "\"main\"");
			assertTrue(snapshot.getValue("p1", "caption") == "\"Main\"");
			assertFalse(snapshot.hasValue("p1", "modules"));
			assertFalse(snapshot.hasValue("p1", "uuid"));
			assertTrue(snapshot.getValue("mod1", "label") == "\"Hello\"");
			assertFalse(snapshot.hasValue("mod1", "moduletype"));
			assertFalse(snapshot.hasValue("mod1", "name"));
			assertTrue(snapshot.getValue("item1", "counter") == "3");
			assertTrue(snapshot.getValue("item1", "list") == "[1,2]");
			assertTrue(snapshot.getStores().size() == 4);
		}

		void testPatchTransformsSnapshot()
		{
			auto snapshotA = snapshotFromJSON(statusJSON("Main", "Hello", 3, true));
			auto snapshotB = snapshotFromJSON(statusJSON("Overview", "Hello", 4, false));
			snapshotB.removeValue("item1", "list");
			snapshotB.setValue("item2", "value", "{\"x\":1}");
			snapshotA.setValue("item3", "value", "\"gone\"");

			auto patch = AMC::CUIFrontendPatch::createDiff(snapshotA, snapshotB);
			assertFalse(patch.isEmpty());

			auto& changes = patch.getChanges();
			assertTrue(changes.find("mod1") == changes.end());
			assertTrue(changes.at("p1").size() == 1);
			assertTrue(changes.at("item1").at("list").m_bRemoved);
			assertTrue(changes.at("item3").at("value").m_bRemoved);

			auto patchedSnapshot = snapshotA;
			patch.applyTo(patchedSnapshot);
			assertTrue(patchedSnapshot.equals(snapshotB));
			assertFalse(patchedSnapshot.equals(snapshotA));
		}

		void testUnchangedProducesEmptyPatch()
		{
			auto snapshotA = snapshotFromJSON(statusJSON("Main", "Hello", 3, true));
			auto snapshotB = snapshotFromJSON(statusJSON("Main", "Hello", 3, true));
			assertTrue(AMC::CUIFrontendPatch::createDiff(snapshotA, snapshotB).isEmpty());

			AMC::CUIFrontendRevisionLog revisionLog;
			auto firstResult = revisionLog.publish(snapshotA, "main|", 0);
			assertFalse(firstResult.m_bPatchAvailable);

			auto secondResult = revisionLog.publish(snapshotB, "main|", firstResult.m_nRevision);
			assertTrue(secondResult.m_nRevision == firstResult.m_nRevision);
			assertTrue(secondResult.m_bPatchAvailable);
			assertTrue(secondResult.m_Patch.isEmpty());
		}

		void testMergedPatchCatchesUp()
		{
			AMC::CUIFrontendRevisionLog revisionLog;

			std::vector<AMC::CUIFrontendSnapshot> snapshots;
			snapshots.push_back(snapshotFromJSON(statusJSON("Main", "Hello", 1, true)));
			snapshots.push_back(snapshotFromJSON(statusJSON("Main", "World", 2, true)));
			snapshots.push_back(snapshotFromJSON(statusJSON("Other", "World", 3, false)));
			snapshots.push_back(snapshotFromJSON(statusJSON("Other", "Hello", 4, true)));

			std::vector<uint64_t> revisions;
			for (auto& snapshot : snapshots)
				revisions.push_back(revisionLog.publish(snapshot, "main|", 0).m_nRevision);

			for (size_t nIndex = 1; nIndex < revisions.size(); nIndex++)
				assertTrue(revisions[nIndex] == revisions[nIndex - 1] + 1);

			for (size_t nBaseIndex = 0; nBaseIndex < snapshots.size(); nBaseIndex++) {
				auto result = revisionLog.publish(snapshots.back(), "main|", revisions[nBaseIndex]);
				assertTrue(result.m_bPatchAvailable);
				assertTrue(result.m_nRevision == revisions.back());

				auto patchedSnapshot = snapshots[nBaseIndex];
				result.m_Patch.applyTo(patchedSnapshot);
				assertTrue(patchedSnapshot.equals(snapshots.back()));

				// Values are absolute, so the patch also applies to any later state on the same lineage.
				for (size_t nLaterIndex = nBaseIndex; nLaterIndex < snapshots.size(); nLaterIndex++) {
					auto laterSnapshot = snapshots[nLaterIndex];
					result.m_Patch.applyTo(laterSnapshot);
					assertTrue(laterSnapshot.equals(snapshots.back()));
				}
			}
		}

		void testOldRevisionFallsBack()
		{
			AMC::CUIFrontendRevisionLog revisionLog(3);

			uint64_t nRevision = 0;
			for (int nCounter = 1; nCounter <= 6; nCounter++)
				nRevision = revisionLog.publish(snapshotFromJSON(statusJSON("Main", "Hello", nCounter, true)), "main|", 0).m_nRevision;
			assertTrue(nRevision == 6);

			auto currentSnapshot = snapshotFromJSON(statusJSON("Main", "Hello", 6, true));

			assertTrue(revisionLog.publish(currentSnapshot, "main|", 3).m_bPatchAvailable);
			assertFalse(revisionLog.publish(currentSnapshot, "main|", 2).m_bPatchAvailable);
			assertFalse(revisionLog.publish(currentSnapshot, "main|", 1).m_bPatchAvailable);
			assertFalse(revisionLog.publish(currentSnapshot, "main|", 7).m_bPatchAvailable);
			assertTrue(revisionLog.getRevision() == 6);

			auto scopeResult = revisionLog.publish(currentSnapshot, "main|dialog", 6);
			assertFalse(scopeResult.m_bPatchAvailable);
			assertTrue(scopeResult.m_nRevision == 7);
			assertFalse(revisionLog.publish(currentSnapshot, "main|dialog", 6).m_bPatchAvailable);
			assertTrue(revisionLog.publish(currentSnapshot, "main|dialog", 7).m_bPatchAvailable);
		}

		void testPatchJSON()
		{
			AMC::CUIFrontendPatch patch;
			patch.setValue("store1", "number", "42");
			patch.setValue("store1", "object", "{\"a\":[1,\"b\"]}");
			patch.setRemoved("store2", "caption");

			AMC::CJSONWriter writer;
			AMC::CJSONWriterObject changedObject(writer);
			patch.writeToJSON(writer, changedObject);
			writer.addObject("changed", changedObject);

			rapidjson::Document document;
			document.Parse(writer.saveToString().c_str());
			assertFalse(document.HasParseError());

			auto& changed = document["changed"];
			assertTrue(changed["store1"]["number"].GetInt() == 42);
			assertTrue(changed["store1"]["object"]["a"][1].GetString() == std::string("b"));
			assertTrue(changed["store2"]["caption"].IsNull());

			AMC::CUIFrontendPatch laterPatch;
			laterPatch.setValue("store2", "caption", "\"back\"");
			laterPatch.setRemoved("store1", "number");
			patch.merge(laterPatch);
			assertFalse(patch.getChanges().at("store2").at("caption").m_bRemoved);
			assertTrue(patch.getChanges().at("store1").at("number").m_bRemoved);
			assertFalse(patch.getChanges().at("store1").at("object").m_bRemoved);
		}
	};

}

#endif // __AMCTEST_UNITTEST_FRONTENDSNAPSHOT
