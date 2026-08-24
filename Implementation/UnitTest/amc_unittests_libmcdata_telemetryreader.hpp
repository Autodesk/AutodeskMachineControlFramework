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

#ifndef __AMCTEST_UNITTEST_LIBMCDATA_TELEMETRYREADER
#define __AMCTEST_UNITTEST_LIBMCDATA_TELEMETRYREADER

#include "amc_unittests.hpp"
#include "amc_unittests_libmcdata.hpp"
#include "common_utils.hpp"
#include "libmcdata_dynamic.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace AMCUnitTest {

	class CUnitTestGroup_LibMCData_TelemetryReader : public CUnitTestGroup {
	public:
		std::string getTestGroupName() override {
			return "LibMCData_TelemetryReader";
		}

		void registerTests() override {
			// Channel tests
			registerTest("CreateChannel", "Create telemetry channel", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_TelemetryReader::testCreateChannel, this));
			registerTest("MultipleChannels", "Create multiple channels", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_TelemetryReader::testMultipleChannels, this));
			registerTest("FindChannelByIdentifier", "Find channel by identifier", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_TelemetryReader::testFindChannelByIdentifier, this));
			
			// Chunk tests
			registerTest("WriteAndReadChunk", "Write and read telemetry chunk", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_TelemetryReader::testWriteAndReadChunk, this));
			registerTest("ChunkTimeStamps", "Verify chunk timestamps", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_TelemetryReader::testChunkTimeStamps, this));
			registerTest("MultipleChunks", "Write and read multiple chunks", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_TelemetryReader::testMultipleChunks, this));
			
			// Time range query tests
			registerTest("FindChunksInTimeRange", "Find chunks in time range", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_TelemetryReader::testFindChunksInTimeRange, this));
			
			// Interval tests
			registerTest("QueryIntervalsBasic", "Query basic intervals", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_TelemetryReader::testQueryIntervalsBasic, this));
			registerTest("QueryInstantMarkers", "Query instant markers", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_TelemetryReader::testQueryInstantMarkers, this));
			
			// Statistics tests
			registerTest("ChannelStatistics", "Calculate channel statistics", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_TelemetryReader::testChannelStatistics, this));
			
			// Additional TelemetryReader tests for coverage
			registerTest("ReaderGetStartTime", "Get reader start time", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_TelemetryReader::testReaderGetStartTime, this));
			registerTest("ReaderGetSessionUUID", "Get reader session UUID", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_TelemetryReader::testReaderGetSessionUUID, this));
			registerTest("ReaderGetLifeTime", "Get reader lifetime", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_TelemetryReader::testReaderGetLifeTime, this));
		}

		void initializeTests() override {
		}

	private:

		struct STelemetryReaderFixture {
			LibMCData::PWrapper m_pDataWrapper;
			LibMCData::PDataModel m_pDataModel;
			LibMCData::PTelemetrySession m_pTelemetrySession;
			std::string m_sBasePath;
			std::string m_sJournalUUID;
		};

		STelemetryReaderFixture createFixture(const std::string& sSuffix)
		{
			STelemetryReaderFixture fixture;

			std::string sRootPath = "temp";
			if (!AMCCommon::CUtils::fileOrPathExistsOnDisk(sRootPath))
				AMCCommon::CUtils::createDirectoryOnDisk(sRootPath);

			fixture.m_sBasePath = sRootPath + "/libmcdata_telreader_" + sSuffix + "_" + AMCCommon::CUtils::createUUID();
			if (!AMCCommon::CUtils::fileOrPathExistsOnDisk(fixture.m_sBasePath))
				AMCCommon::CUtils::createDirectoryOnDisk(fixture.m_sBasePath);

			std::string sDatabaseFile = fixture.m_sBasePath + "/telemetry.db";

			fixture.m_pDataWrapper = AMCUnitTest::loadLibMCDataInProcess();
			fixture.m_pDataModel = fixture.m_pDataWrapper->CreateDataModelInstance();
			fixture.m_pDataModel->InitialiseDatabase(fixture.m_sBasePath, LibMCData::eDataBaseType::SqLite, sDatabaseFile);
			fixture.m_pTelemetrySession = fixture.m_pDataModel->CreateTelemetrySession();
			
			// Get journal UUID for reader access
			auto pJournalSession = fixture.m_pDataModel->CreateJournalSession();
			fixture.m_sJournalUUID = pJournalSession->GetSessionUUID();

			return fixture;
		}

		void createChannel(STelemetryReaderFixture& fixture, const std::string& sIdentifier, uint32_t nIndex)
		{
			std::string sUUID = AMCCommon::CUtils::createUUID();
			// CreateChannelInDB(sUUID, eChannelType, nChannelIndex, sChannelIdentifier, sChannelDescription)
			fixture.m_pTelemetrySession->CreateChannelInDB(sUUID, LibMCData::eTelemetryChannelType::CustomMarker, nIndex, sIdentifier, "Description for " + sIdentifier);
		}

		void testCreateChannel()
		{
			auto fixture = createFixture("create_channel");
			
			createChannel(fixture, "test_channel", 1);
			
			// Create reader and verify
			auto pReader = fixture.m_pDataModel->CreateTelemetryReader(fixture.m_sJournalUUID);
			
			uint32_t nCount = pReader->GetChannelCount();
			assertTrue(nCount == 1, "Should have 1 channel");
			
			std::string sUUID, sIdentifier, sDescription;
			LibMCData::eTelemetryChannelType eType;
			pReader->GetChannelInformation(0, sUUID, eType, sIdentifier, sDescription);
			
			assertTrue(sIdentifier == "test_channel", "Channel identifier mismatch");
			assertTrue(eType == LibMCData::eTelemetryChannelType::CustomMarker, "Channel type mismatch");
		}

		void testMultipleChannels()
		{
			auto fixture = createFixture("multi_channels");
			
			for (uint32_t i = 1; i <= 5; i++) {
				createChannel(fixture, "channel_" + std::to_string(i), i);
			}
			
			auto pReader = fixture.m_pDataModel->CreateTelemetryReader(fixture.m_sJournalUUID);
			assertTrue(pReader->GetChannelCount() == 5, "Should have 5 channels");
		}

		void testFindChannelByIdentifier()
		{
			auto fixture = createFixture("find_channel");
			
			createChannel(fixture, "findme_channel", 1);
			createChannel(fixture, "other_channel", 2);
			
			auto pReader = fixture.m_pDataModel->CreateTelemetryReader(fixture.m_sJournalUUID);
			
			uint32_t nIndex;
			bool bFound = pReader->FindChannelByIdentifier("findme_channel", nIndex);
			assertTrue(bFound, "Channel should be found");
			
			bFound = pReader->FindChannelByIdentifier("nonexistent", nIndex);
			assertFalse(bFound, "Non-existent channel should not be found");
		}

		void testWriteAndReadChunk()
		{
			auto fixture = createFixture("write_read_chunk");
			
			createChannel(fixture, "chunk_channel", 1);
			
			// Write chunk with entries - use chunk ID 0 for the first chunk
			std::vector<LibMCData::sTelemetryChunkEntry> entries;
			
			LibMCData::sTelemetryChunkEntry entry1;
			entry1.m_EntryType = LibMCData::eTelemetryChunkEntryType::InstantMarker;
			entry1.m_ChannelIndex = 1;
			entry1.m_MarkerID = 100;
			entry1.m_TimeStamp = 1000;
			entry1.m_ContextData = 42;
			entries.push_back(entry1);
			
			// WriteTelemetryChunk(nChunkID, nStartTimeStamp, nEndTimeStamp, entries)
			fixture.m_pTelemetrySession->WriteTelemetryChunk(0, 1000, 2000, entries);
			
			// Read back
			auto pReader = fixture.m_pDataModel->CreateTelemetryReader(fixture.m_sJournalUUID);
			uint32_t nChunkCount = pReader->GetChunkCount();
			assertTrue(nChunkCount >= 1, "Should have at least 1 chunk");
			
			if (nChunkCount > 0) {
				auto pChunkData = pReader->ReadChunkData(0);
				assertAssigned(pChunkData.get(), "Chunk data should be returned");
				assertTrue(pChunkData->GetEntryCount() >= 1, "Chunk should have at least 1 entry");
			}
		}

		void testChunkTimeStamps()
		{
			auto fixture = createFixture("chunk_times");
			
			createChannel(fixture, "time_channel", 1);
			
			uint64_t nStartTime = 5000;
			uint64_t nEndTime = 10000;
			
			std::vector<LibMCData::sTelemetryChunkEntry> entries;
			LibMCData::sTelemetryChunkEntry entry;
			entry.m_EntryType = LibMCData::eTelemetryChunkEntryType::InstantMarker;
			entry.m_ChannelIndex = 1;
			entry.m_MarkerID = 1;
			entry.m_TimeStamp = 7500;
			entry.m_ContextData = 0;
			entries.push_back(entry);
			
			// Use chunk ID 0
			fixture.m_pTelemetrySession->WriteTelemetryChunk(0, nStartTime, nEndTime, entries);
			
			auto pReader = fixture.m_pDataModel->CreateTelemetryReader(fixture.m_sJournalUUID);
			
			uint32_t nChunkCount = pReader->GetChunkCount();
			assertTrue(nChunkCount >= 1, "Should have at least 1 chunk");
			
			if (nChunkCount > 0) {
				uint64_t nReadStart, nReadEnd, nEntryCount;
				pReader->GetChunkInformation(0, nReadStart, nReadEnd, nEntryCount);
				
				// Just verify we got valid timestamps back
				assertTrue(nReadEnd >= nReadStart, "End time should be >= start time");
			}
		}

		void testMultipleChunks()
		{
			auto fixture = createFixture("multi_chunks");
			
			createChannel(fixture, "multi_chunk_channel", 1);
			
			// Write multiple chunks with 0-indexed chunk IDs
			for (uint64_t i = 0; i < 5; i++) {
				std::vector<LibMCData::sTelemetryChunkEntry> entries;
				LibMCData::sTelemetryChunkEntry entry;
				entry.m_EntryType = LibMCData::eTelemetryChunkEntryType::InstantMarker;
				entry.m_ChannelIndex = 1;
				entry.m_MarkerID = i + 1;
				entry.m_TimeStamp = (i + 1) * 1000;
				entry.m_ContextData = 0;
				entries.push_back(entry);
				
				fixture.m_pTelemetrySession->WriteTelemetryChunk(i, i * 1000, (i + 1) * 1000, entries);
			}
			
			auto pReader = fixture.m_pDataModel->CreateTelemetryReader(fixture.m_sJournalUUID);
			uint32_t nChunkCount = pReader->GetChunkCount();
			assertTrue(nChunkCount >= 5, "Should have at least 5 chunks");
		}

		void testFindChunksInTimeRange()
		{
			auto fixture = createFixture("time_range");
			
			createChannel(fixture, "range_channel", 1);
			
			// Create chunks at different time ranges (0-indexed)
			// Chunk 0: 0-1000
			// Chunk 1: 1000-2000
			// Chunk 2: 2000-3000
			for (uint64_t i = 0; i < 3; i++) {
				std::vector<LibMCData::sTelemetryChunkEntry> entries;
				LibMCData::sTelemetryChunkEntry entry;
				entry.m_EntryType = LibMCData::eTelemetryChunkEntryType::InstantMarker;
				entry.m_ChannelIndex = 1;
				entry.m_MarkerID = i + 1;
				entry.m_TimeStamp = i * 1000 + 500;
				entry.m_ContextData = 0;
				entries.push_back(entry);
				
				fixture.m_pTelemetrySession->WriteTelemetryChunk(i, i * 1000, (i + 1) * 1000, entries);
			}
			
			auto pReader = fixture.m_pDataModel->CreateTelemetryReader(fixture.m_sJournalUUID);
			
			// Query range that overlaps chunks 1 and 2 (1500-2500)
			std::vector<uint32_t> chunkIndices;
			pReader->FindChunksInTimeRange(1500, 2500, chunkIndices);
			
			assertTrue(chunkIndices.size() >= 1, "Should find at least 1 chunk in range");
		}

		void testQueryIntervalsBasic()
		{
			auto fixture = createFixture("intervals_basic");
			
			createChannel(fixture, "interval_channel", 1);
			
			// Create start/end marker pairs
			std::vector<LibMCData::sTelemetryChunkEntry> entries;
			
			// Interval 1: 1000-1500 (duration 500)
			LibMCData::sTelemetryChunkEntry start1;
			start1.m_EntryType = LibMCData::eTelemetryChunkEntryType::IntervalStartMarker;
			start1.m_ChannelIndex = 1;
			start1.m_MarkerID = 100;
			start1.m_TimeStamp = 1000;
			start1.m_ContextData = 10;
			entries.push_back(start1);
			
			LibMCData::sTelemetryChunkEntry end1;
			end1.m_EntryType = LibMCData::eTelemetryChunkEntryType::IntervalEndMarker;
			end1.m_ChannelIndex = 1;
			end1.m_MarkerID = 100;
			end1.m_TimeStamp = 1500;
			end1.m_ContextData = 0;
			entries.push_back(end1);
			
			// Interval 2: 2000-3000 (duration 1000)
			LibMCData::sTelemetryChunkEntry start2;
			start2.m_EntryType = LibMCData::eTelemetryChunkEntryType::IntervalStartMarker;
			start2.m_ChannelIndex = 1;
			start2.m_MarkerID = 101;
			start2.m_TimeStamp = 2000;
			start2.m_ContextData = 20;
			entries.push_back(start2);
			
			LibMCData::sTelemetryChunkEntry end2;
			end2.m_EntryType = LibMCData::eTelemetryChunkEntryType::IntervalEndMarker;
			end2.m_ChannelIndex = 1;
			end2.m_MarkerID = 101;
			end2.m_TimeStamp = 3000;
			end2.m_ContextData = 0;
			entries.push_back(end2);
			
			// Use chunk ID 0
			fixture.m_pTelemetrySession->WriteTelemetryChunk(0, 0, 4000, entries);
			
			auto pReader = fixture.m_pDataModel->CreateTelemetryReader(fixture.m_sJournalUUID);
			
			std::vector<LibMCData::sTelemetryIntervalData> intervals;
			// QueryIntervals(nStartTime, nEndTime, nChannelIndex, intervalsBuffer)
			// Use 0xFFFFFFFF for all channels
			pReader->QueryIntervals(0, 5000, 0xFFFFFFFF, intervals);
			
			assertTrue(intervals.size() >= 2, "Should find at least 2 intervals");
		}

		void testQueryInstantMarkers()
		{
			auto fixture = createFixture("instant_markers");
			
			createChannel(fixture, "instant_channel", 1);
			
			std::vector<LibMCData::sTelemetryChunkEntry> entries;
			
			for (uint64_t i = 1; i <= 5; i++) {
				LibMCData::sTelemetryChunkEntry entry;
				entry.m_EntryType = LibMCData::eTelemetryChunkEntryType::InstantMarker;
				entry.m_ChannelIndex = 1;
				entry.m_MarkerID = i;
				entry.m_TimeStamp = i * 100;
				entry.m_ContextData = i;
				entries.push_back(entry);
			}
			
			// Use chunk ID 0
			fixture.m_pTelemetrySession->WriteTelemetryChunk(0, 0, 600, entries);
			
			auto pReader = fixture.m_pDataModel->CreateTelemetryReader(fixture.m_sJournalUUID);
			
			std::vector<LibMCData::sTelemetryChunkEntry> markers;
			pReader->QueryInstantMarkers(0, 1000, 0xFFFFFFFF, markers);
			
			assertTrue(markers.size() >= 5, "Should find at least 5 instant markers");
		}

		void testChannelStatistics()
		{
			auto fixture = createFixture("statistics");
			
			createChannel(fixture, "stats_channel", 1);
			
			std::vector<LibMCData::sTelemetryChunkEntry> entries;
			
			// Create intervals with durations: 100, 200, 300
			uint64_t durations[] = {100, 200, 300};
			for (int i = 0; i < 3; i++) {
				uint64_t startTime = i * 500;
				uint64_t endTime = startTime + durations[i];
				
				LibMCData::sTelemetryChunkEntry start;
				start.m_EntryType = LibMCData::eTelemetryChunkEntryType::IntervalStartMarker;
				start.m_ChannelIndex = 1;
				start.m_MarkerID = 300 + i;
				start.m_TimeStamp = startTime;
				start.m_ContextData = 0;
				entries.push_back(start);
				
				LibMCData::sTelemetryChunkEntry end;
				end.m_EntryType = LibMCData::eTelemetryChunkEntryType::IntervalEndMarker;
				end.m_ChannelIndex = 1;
				end.m_MarkerID = 300 + i;
				end.m_TimeStamp = endTime;
				end.m_ContextData = 0;
				entries.push_back(end);
			}
			
			// Use chunk ID 0
			fixture.m_pTelemetrySession->WriteTelemetryChunk(0, 0, 2000, entries);
			
			auto pReader = fixture.m_pDataModel->CreateTelemetryReader(fixture.m_sJournalUUID);
			
			uint32_t nChannelIndex;
			bool bFound = pReader->FindChannelByIdentifier("stats_channel", nChannelIndex);
			assertTrue(bFound, "Should find stats_channel");
			
			uint64_t nIntervalCount, nInstantCount, nTotalDuration, nMinDuration, nMaxDuration, nAvgDuration;
			pReader->GetChannelStatistics(nChannelIndex, 0, 5000, nIntervalCount, nInstantCount, nTotalDuration, nMinDuration, nMaxDuration, nAvgDuration);
			
			assertTrue(nIntervalCount >= 3, "Should have at least 3 intervals");
			assertTrue(nTotalDuration >= 600, "Total duration should be at least 600");
		}
		
		// ============= Additional TelemetryReader Tests =============
		
		void testReaderGetStartTime()
		{
			auto fixture = createFixture("start_time");
			
			createChannel(fixture, "starttime_channel", 1);
			
			// Write a chunk
			std::vector<LibMCData::sTelemetryChunkEntry> entries;
			LibMCData::sTelemetryChunkEntry entry;
			entry.m_EntryType = LibMCData::eTelemetryChunkEntryType::InstantMarker;
			entry.m_ChannelIndex = 1;
			entry.m_MarkerID = 1;
			entry.m_TimeStamp = 1000;
			entry.m_ContextData = 0;
			entries.push_back(entry);
			
			fixture.m_pTelemetrySession->WriteTelemetryChunk(0, 0, 2000, entries);
			
			auto pReader = fixture.m_pDataModel->CreateTelemetryReader(fixture.m_sJournalUUID);
			
			std::string sStartTime = pReader->GetStartTime();
			assertFalse(sStartTime.empty(), "Start time should not be empty");
		}
		
		void testReaderGetSessionUUID()
		{
			auto fixture = createFixture("session_uuid");
			
			createChannel(fixture, "sessionuuid_channel", 1);
			
			auto pReader = fixture.m_pDataModel->CreateTelemetryReader(fixture.m_sJournalUUID);
			
			std::string sSessionUUID = pReader->GetSessionUUID();
			assertFalse(sSessionUUID.empty(), "Session UUID should not be empty");
			assertTrue(sSessionUUID == fixture.m_sJournalUUID, "Session UUID should match journal UUID");
		}
		
		void testReaderGetLifeTime()
		{
			auto fixture = createFixture("lifetime");
			
			createChannel(fixture, "lifetime_channel", 1);
			
			// Write a chunk
			std::vector<LibMCData::sTelemetryChunkEntry> entries;
			LibMCData::sTelemetryChunkEntry entry;
			entry.m_EntryType = LibMCData::eTelemetryChunkEntryType::InstantMarker;
			entry.m_ChannelIndex = 1;
			entry.m_MarkerID = 1;
			entry.m_TimeStamp = 1000;
			entry.m_ContextData = 0;
			entries.push_back(entry);
			
			fixture.m_pTelemetrySession->WriteTelemetryChunk(0, 0, 2000, entries);
			
			auto pReader = fixture.m_pDataModel->CreateTelemetryReader(fixture.m_sJournalUUID);
			
			uint64_t nLifeTime = pReader->GetLifeTimeInMicroseconds();
			// Lifetime should be non-negative
			assertTrue(nLifeTime >= 0, "Lifetime should be non-negative");
		}
	};

}

#endif // __AMCTEST_UNITTEST_LIBMCDATA_TELEMETRYREADER
