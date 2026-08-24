/*++

Copyright (C) 2020 Autodesk Inc.

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


#ifndef AMC_STATESIGNAL
#define AMC_STATESIGNAL

#include "amc_statesignalparameter.hpp"
#include "amc_statesignaltypes.hpp"
#include "amc_statesignalregistry.hpp"
#include "amc_parametergroup.hpp"

#include <memory>
#include <string>
#include <map>
#include <unordered_map>
#include <deque>
#include <unordered_set>
#include <vector>
#include <list>

#include <queue>
#include <thread>

namespace AMC {

	class CStateSignalSlot;
	typedef std::shared_ptr<CStateSignalSlot> PStateSignalSlot;

	class CStateSignalMessage;
	typedef std::shared_ptr<CStateSignalMessage> PStateSignalMessage;

	class CTelemetryChannel;
	typedef std::shared_ptr<CTelemetryChannel> PTelemetryChannel;

	class CTelemetryMarker;
	typedef std::shared_ptr<CTelemetryMarker> PTelemetryMarker;

	class CStateSignalArchiveWriter {
	public:

		virtual ~CStateSignalArchiveWriter() 
		{
		}

		virtual void writeSignalMessageToArchive(const std::string& sInstanceName, const std::string& sSignalName, const CStateSignalMessage* pMessage) = 0;

	};


	class CStateSignalMessage
	{
		private:

			std::string m_sUUID;

			AMC::eAMCSignalPhase m_MessagePhase;

			uint32_t m_nReactionTimeoutInMS;

			std::string m_sResultDataJSON;

			std::string m_sParameterDataJSON;

			std::string m_sErrorMessage;	

			uint64_t m_nCreationTimestamp;
			uint64_t m_nTerminalTimestamp;

			AMC::PTelemetryMarker m_pTelemetryInQueueMarker;
			AMC::PTelemetryMarker m_pTelemetryInProcessMarker;
			AMC::PTelemetryMarker m_pTelemetryAcknowledgedMarker;

			PTelemetryChannel m_pInProcessTelemetryChannel;
			PTelemetryChannel m_pAcknowledgeTelemetryChannel;

		public:

			CStateSignalMessage(const std::string & sUUID, uint32_t nReactionTimeoutInMS, PTelemetryChannel pQueueTelemetryChannel, PTelemetryChannel pInProcessTelemetryChannel, PTelemetryChannel pAcknowledgeTelemetryChannel);

			virtual ~CStateSignalMessage();

			std::string getUUID() const;

			uint64_t getCreationTimestamp() const;

			// Returns terminal timestamp (handled/failed/timedout/cleared), 0 if not in terminal state
			uint64_t getTerminalTimestamp() const;

			bool setPhase (AMC::eAMCSignalPhase messagePhase);

			AMC::eAMCSignalPhase getPhase() const;

			uint32_t getReactionTimeoutInMS() const;

			std::string getResultDataJSON() const;

			std::string getParameterDataJSON() const;

			std::string getErrorMessage() const;

			void setErrorMessage(const std::string & sErrorMessage);

			void setResultDataJSON(const std::string& sResultDataJSON);

			void setParameterDataJSON(const std::string& sParameterDataJSON);

			bool hadReactionTimeout(uint64_t nGlobalTimestamp);

	};


	class CStateSignalSlot {
	private:
		std::string m_sInstanceName;
		std::string m_sName;

		std::vector <CStateSignalParameter> m_ParameterDefinitions;
		std::vector <CStateSignalParameter> m_ResultDefinitions;

		uint32_t m_nSignalDefaultReactionTimeOutInMS; // Time until a handler needs to pick up the signal until it times out
		uint32_t m_nSignalAutomaticArchiveTimeInMS; // Time until a handled/failed/cleared/timedout signal is automatically archived
		uint32_t m_nSignalQueueSize;

		std::unordered_map<std::string, PStateSignalMessage> m_MessageMap;

		CStateSignalRegistry* m_pRegistry;

		std::list<PStateSignalMessage> m_Queue;
		std::unordered_map<std::string, std::list<PStateSignalMessage>::iterator> m_QueueMap;

		std::unordered_set<std::string> m_InProcess;
		std::unordered_set<std::string> m_Handled;
		std::unordered_set<std::string> m_Failed;
		std::unordered_set<std::string> m_TimedOut;
		std::unordered_set<std::string> m_Cleared;
		std::deque<PStateSignalMessage> m_MessagesToArchive;

		uint64_t m_nTriggerCount;
		uint64_t m_nHandledCount;
		uint64_t m_nFailedCount;
		uint64_t m_nTimedOutCount;
		uint64_t m_nArchivedCount;
		uint64_t m_nMaxReactionTime;

		PTelemetryChannel m_pQueueTelemetryChannel;
		PTelemetryChannel m_pProcessingTelemetryChannel;
		PTelemetryChannel m_pAcknowledgementTelemetryChannel;

		void increaseTriggerCount();
		void increaseHandledCount();
		void increaseFailedCount();
		void increaseTimeoutCount();
		void increaseArchivedCount();
		void updateTimingStatistics();

		PParameterGroup m_pSignalInformationGroup;

		std::mutex m_Mutex;

		CStateSignalMessage* getMessageByUUIDNoMutex (const std::string& sMessageUUIDNormalized);
		bool queueIsFullNoMutex();


		void checkForReactionTimeoutsNoMutex(uint64_t nGlobalTimestamp);

	public:

		CStateSignalSlot(const std::string & sInstanceName, const std::string& sName, const std::vector<CStateSignalParameter>& Parameters, const std::vector<CStateSignalParameter>& Results, uint32_t nSignalDefaultReactionTimeOutInMS, uint32_t nSignalAutomaticArchiveTimeInMS, uint32_t nSignalQueueSize, PParameterGroup pSignalInformationGroup, CStateSignalRegistry* pRegistry);
		virtual ~CStateSignalSlot();

		std::string getNameInternal() const;
		std::string getInstanceNameInternal() const;

		std::string getSignalTelemetryQueueIdentifier() const;
		std::string getSignalTelemetryProcessingIdentifier() const;
		std::string getSignalTelemetryAcknowledgementIdentifier() const;

		bool queueIsFull();
		bool queueIsEmpty();
		size_t clearQueueInternal();
		bool eraseMessage(const std::string& sUUID);

		PStateSignalMessage addNewInQueueSignalInternal(const std::string& sMessageUUID, const std::string& sParameterData, uint32_t nReactionTimeoutInMS, uint64_t nTimeStamp);
		bool changeSignalPhaseToHandledInternal(const std::string& sMessageUUID, const std::string& sResultData, uint64_t nTimeStamp);
		bool changeSignalPhaseToInFailedInternal(const std::string& sMessageUUID, const std::string& sResultData, const std::string& sErrorMessage, uint64_t nTimeStamp);
		bool changeSignalPhaseToInProcessInternal(const std::string& sMessageUUID, uint64_t nTimeStamp);
		bool changeSignalPhaseToArchivedInternal(const std::string& sMessageUUID, uint64_t nTimeStamp, bool bFailIfNotExisting);
		AMC::eAMCSignalPhase getSignalPhaseInternal(const std::string& sMessageUUID);

		uint32_t getAvailableSignalQueueEntriesInternal ();

		uint32_t getTotalSignalQueueSizeInternal();

		uint32_t getDefaultReactionTimeoutInternal();

		uint32_t getReactionTimeoutInternal(const std::string& sMessageUUID);

		void checkForReactionTimeouts(uint64_t nGlobalTimestamp);

		void autoArchiveMessages (uint64_t nGlobalTimestamp);

		void writeMessagesToArchive (CStateSignalArchiveWriter * pArchiveWriter);

		PStateSignalMessage claimMessageFromQueueInternal(bool bCheckForReactionTimeout, uint64_t nGlobalTimestamp, uint64_t nTimeStamp, bool bChangePhaseToInprocess);
		
		std::string getResultDataJSONInternal(const std::string& sMessageUUID);
		std::string getParameterDataJSONInternal(const std::string& sMessageUUID);

		bool getParameterPropertiesInternal(const std::string& sMessageUUID, std::string & sInstanceName, std::string & sSignalName, std::string & sParameterDataJSON);

		void populateParameterGroup(CParameterGroup* pParameterGroup);
		void populateResultGroup(CParameterGroup* pResultGroup);

	};

	
}


#endif // AMC_STATESIGNAL
