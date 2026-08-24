/*++

Copyright (C) 2023 Autodesk Inc.

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


#ifndef AMC_TELEMETRY
#define AMC_TELEMETRY

#include <memory>
#include <string>
#include <vector>
#include <mutex>
#include <deque>
#include <atomic>
#include <unordered_map>
#include <map>
#include <queue>

#include "libmcdata_types.hpp"

#include <common_chrono.hpp>

namespace LibMCData {
	class CTelemetrySession;
	typedef std::shared_ptr<CTelemetrySession> PTelemetrySession;
}

namespace AMC {

	class CTelemetryWriter;

	struct sTelemetryOpenIntervalMarker {
		uint32_t m_nChunkID;
		uint32_t m_nEntryIndex;
	};

	class CTelemetryDataChunk {
	private:
		CTelemetryWriter* m_pWriter;

		uint64_t m_nChunkStartTimestamp;
		uint64_t m_nChunkEndTimestamp;

		uint64_t m_nMinTimestamp; // Should be within chunk time interval
		uint64_t m_nMaxTimestamp; // Should be within chunk time interval
		uint64_t m_nMinMarkerID;
		uint64_t m_nMaxMarkerID;

		uint64_t m_nTelemetryChunkID;

		std::vector<LibMCData::sTelemetryChunkEntry> m_Entries;

		bool m_bChunkIsReadonly;

		bool m_bChunkHasBeenArchived;

		mutable std::mutex m_ChunkMutex;

	public:

		CTelemetryDataChunk (CTelemetryWriter* pWriter, uint64_t nTelemetryChunkID, uint64_t nChunkStartTimestamp, uint64_t nChunkEndTimestamp);

		virtual ~CTelemetryDataChunk();

		uint64_t getChunkID() const;

		bool isEmpty() const;

		void writeEntry(const LibMCData::sTelemetryChunkEntry& entry);

		void makeReadOnly();

		bool isReadOnly() const;

		bool hasBeenArchived() const;

		void writeToArchive(LibMCData::PTelemetrySession pTelemetrySession);

	};

	typedef std::shared_ptr<CTelemetryDataChunk> PTelemetryDataChunk;

	class CTelemetryWriter
	{
		private:
			std::mutex m_ChunkMutex;
			std::vector<PTelemetryDataChunk> m_Chunks;

			std::mutex m_ArchiveQueueMutex;
			std::queue<PTelemetryDataChunk> m_ArchiveQueue;

			uint64_t m_nChunkIntervalInMicroseconds;

			std::mutex m_OpenIntervalMutex;
			std::unordered_map<uint64_t, sTelemetryOpenIntervalMarker> m_OpenIntervalMarkers;

			std::mutex m_DataMutex;
			LibMCData::PTelemetrySession m_pTelemetrySession;

			AMCCommon::PChrono m_pGlobalChrono;

			std::atomic<uint64_t> m_nNextMarkerID;

			void extendChunksUntil(uint64_t nMaxChunkIndexOneBased);

		public:

			CTelemetryWriter(LibMCData::PTelemetrySession pTelemetrySession, AMCCommon::PChrono pGlobalChrono);

			virtual ~CTelemetryWriter();

			PTelemetryDataChunk getOrCreateChunkByTimestamp(uint64_t nTimestamp);

			void writeEntry(const LibMCData::sTelemetryChunkEntry& entry);

			void registerOpenInterval (uint64_t nMarkerID, uint32_t nChunkID, uint32_t nChunkEntryIndex);

			void eraseOpenInterval (uint64_t nMarkerID);

			void createChannelInDB(const std::string & sUUID, LibMCData::eTelemetryChannelType channelType, uint32_t nChannelIndex, const std::string & sChannelIdentifier, const std::string & sChannelDescription);

			void archiveOldChunksToDB();

			uint64_t createMarkerID();

			uint64_t getCurrentTimestamp();

	};

	typedef std::shared_ptr<CTelemetryWriter> PTelemetryWriter;

	class CTelemetryChannel;
	typedef std::shared_ptr<CTelemetryChannel> PTelemetryChannel;
	typedef std::weak_ptr<CTelemetryChannel> WTelemetryChannel;


	class CTelemetryHandler;
	class CTelemetryScope;

	class CTelemetryMarker {
		private:

			WTelemetryChannel m_pChannel;

			uint64_t m_nMarkerID;

			uint64_t m_nStartTimestamp;

			uint64_t m_nContextData;

			std::atomic<uint64_t> m_nFinishTimestamp;

		public:

			CTelemetryMarker(WTelemetryChannel pChannel, uint64_t nMarkerID, bool bMarkInstant, uint64_t nContextData);

			virtual ~CTelemetryMarker();

			WTelemetryChannel getChannel() const;

			void finishMarker();

			bool isFinished() const;

			uint64_t getDuration() const;

			uint64_t getMarkerID() const;

			uint64_t getStartTimestamp() const;

			uint64_t getFinishTimestamp() const;

			uint64_t getContextData () const;

	};

	typedef std::shared_ptr<CTelemetryMarker> PTelemetryMarker;
	typedef std::weak_ptr<CTelemetryMarker> WTelemetryMarker;

	class CTelemetryScope {
		private:
			PTelemetryMarker m_pMarker;

		public:

			CTelemetryScope(PTelemetryMarker pMarker);
			CTelemetryScope(const CTelemetryScope&) = delete;
			CTelemetryScope& operator=(const CTelemetryScope&) = delete;
			CTelemetryScope(CTelemetryScope&&) noexcept = default;
			CTelemetryScope& operator=(CTelemetryScope&&) noexcept = default;

			virtual ~CTelemetryScope();

			uint64_t getMarkerID() const; 

			uint64_t getStartTimeStamp() const;

	};

	class CTelemetryChannel : public std::enable_shared_from_this<CTelemetryChannel> {
	private:
		std::string m_sChannelUUID;
		uint32_t m_nChannelIndex;
		std::string m_sChannelIdentifier;
		std::string m_sChannelDescription;
		LibMCData::eTelemetryChannelType m_eChannelType;

		std::mutex m_MarkerMutex;
		std::unordered_map<uint64_t, PTelemetryMarker> m_Markers;
		uint64_t m_nTotalMarkersCreated;
		uint64_t m_nMaxDurationInMicroseconds;

		PTelemetryWriter m_pTelemetryWriter;

		PTelemetryMarker createMarkerEx(bool bMarkInstant, uint64_t nContextData);

	public:
	
		CTelemetryChannel(const std::string& sChannelUUID, uint32_t nChannelIndex, const std::string& sChannelIdentifier, const std::string& sChannelDescription, LibMCData::eTelemetryChannelType channelType, PTelemetryWriter pTelemetryWriter);

		virtual ~CTelemetryChannel();

		std::string getChannelUUID() const;

		uint32_t getChannelIndex() const;

		std::string getChannelIdentifier() const;

		std::string getChannelDescription() const;

		LibMCData::eTelemetryChannelType getChannelType () const;

		uint64_t createInstantMarker (uint64_t nContextData);

		[[nodiscard]] CTelemetryScope startIntervalScope(uint64_t nContextData);

		PTelemetryMarker startIntervalMarker(uint64_t nContextData);

		void unregisterMarker(uint64_t markerID, uint64_t nDurationInMicroseconds);

		uint64_t getCurrentTimestamp();	

		uint64_t getTotalMarkersCreated();

		uint64_t getMaxDurationInMicroseconds();

		void getStatistics(uint64_t& nTotalMarkersCreated, uint64_t& nMaxDurationInMicroseconds);

		PTelemetryWriter getWriter() const;		

		static std::string mapDataChannelTypeToTypeString(LibMCData::eTelemetryChannelType dataChannelType);

		static LibMCData::eTelemetryChannelType mapChannelTypeStringToDataChannelType(const std::string& sTypeString);


	};
	
	class CTelemetryHandler {
	private:

		std::unordered_map<std::string, PTelemetryChannel> m_ChannelsByUUID;
		std::unordered_map<std::string, PTelemetryChannel> m_ChannelsByIdentifier;
		uint32_t m_nNextChannelIndex;

		std::mutex m_ChannelMutex;

		PTelemetryWriter m_pTelemetryWriter;

	public:

		CTelemetryHandler (PTelemetryWriter pWriter);
		
		virtual ~CTelemetryHandler ();
		
		PTelemetryChannel registerChannel(const std::string& sChannelIdentifier, const std::string& sChannelDescription, LibMCData::eTelemetryChannelType channelType);

		PTelemetryChannel findChannelByUUID(const std::string& sChannelUUID);

		PTelemetryChannel findChannelByIdentifier(const std::string& sChannelIdentifier, bool bFailIfNotExisting);

		void archiveOldChunksToDB();


	};

	typedef std::shared_ptr<CTelemetryHandler> PTelemetryHandler;

}


#endif //AMC_TELEMETRY

