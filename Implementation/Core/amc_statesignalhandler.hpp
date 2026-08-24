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


#ifndef AMC_STATESIGNALHANDLER
#define AMC_STATESIGNALHANDLER

#include <memory>
#include <string>
#include <map>
#include <list>
#include <mutex>
#include <unordered_map>

#include "amc_statesignalparameter.hpp"
#include "amc_statesignaltypes.hpp"
#include "amc_statesignalregistry.hpp"
#include "amc_parametergroup.hpp"


namespace AMC {

	class CStateSignalHandler;
	typedef std::shared_ptr<CStateSignalHandler> PStateSignalHandler;

	class CTelemetryHandler;
	typedef std::shared_ptr<CTelemetryHandler> PTelemetryHandler;

	// Do not include StateSignal.hpp outside of amc_statesignalhandler.cpp for threadsafety!
	class CStateSignalSlot;
	typedef std::shared_ptr<CStateSignalSlot> PStateSignalSlot;

	class CStateSignalArchiveWriter;

	class CStateSignalInstance {
	private:
		std::string m_sInstanceName;

		std::mutex m_SlotMutex;

		CStateSignalRegistry* m_pRegistry;

		std::unordered_map<std::string, PStateSignalSlot> m_Slots;

	public:

		CStateSignalInstance (const std::string & sInstanceName, CStateSignalRegistry* pRegistry);

		virtual ~CStateSignalInstance();

		std::string getInstanceName () const;

		PStateSignalSlot getSignalSlot (const std::string& sSignalName);

		PStateSignalSlot getSignalSlotOrNull (const std::string& sSignalName);

		PStateSignalSlot addSignalDefinition(const std::string& sSignalName, const std::vector<CStateSignalParameter>& Parameters, const std::vector<CStateSignalParameter>& Results, uint32_t nSignalReactionTimeOutInMS, uint32_t nAutomaticArchiveTimeInMS, uint32_t nSignalQueueSize, PParameterGroup pSignalInformationGroup);

		void clearUnhandledSignals(uint64_t nTimestamp);

		void clearUnhandledSignalsOfType(const std::string& sSignalTypeName, uint64_t nTimestamp);

		bool canTrigger(const std::string& sSignalName);

		bool queueIsEmpty(const std::string& sSignalName);

		bool hasSignalDefinition(const std::string& sSignalName);

		bool claimSignalMessage(const std::string& sSignalName, bool bCheckForReactionTimeout, uint64_t nGlobalTimestamp, uint64_t nTimeStamp, std::string& sSignalUUID, std::string& sParameterDataJSON, bool bChangePhaseToInprocess);

		bool addNewInQueueSignal(const std::string& sSignalName, const std::string& sSignalUUID, const std::string& sParameterData, uint32_t nResponseTimeOutInMS, uint64_t nTimestamp);

		uint32_t getAvailableSignalQueueEntryCount(const std::string& sSignalName);

		uint32_t getTotalSignalQueueSize(const std::string& sSignalName);

		uint32_t getDefaultReactionTimeout(const std::string& sSignalName);

		void populateParameterGroup(const std::string& sSignalName, CParameterGroup* pParameterGroup);

		void populateResultGroup(const std::string& sSignalName, CParameterGroup* pResultGroup);

		void checkForReactionTimeouts(uint64_t nGlobalTimestamp);

		void autoArchiveMessages(uint64_t nGlobalTimestamp);

		void writeMessagesToArchive(CStateSignalArchiveWriter* pArchiveWriter);

	};

	typedef std::shared_ptr<CStateSignalInstance> PStateSignalInstance;

	class CStateSignalHandler : public CStateSignalRegistry {
	private:
		
		std::unordered_map<std::string, PStateSignalInstance> m_Instances;

		std::mutex m_SignalInstanceMutex;


		std::unordered_map<std::string, std::weak_ptr<CStateSignalSlot>> m_MessageSlotMap;

		std::mutex m_MessageMapMutex;

		PTelemetryHandler m_pTelemetryHandler;

	public:

		CStateSignalHandler(PTelemetryHandler pTelemetryHandler);

		virtual ~CStateSignalHandler();

		PStateSignalInstance registerInstance (const std::string& sInstanceName);

		PStateSignalInstance getInstance (const std::string & sInstanceName);

		void registerMessage(const std::string& sMessageUUID, CStateSignalSlot* pSignalSlot) override;

		void unregisterMessage(const std::string& sMessageUUID) override;

		PStateSignalSlot findSignalSlotOfMessage(const std::string& sMessageUUID) override;

		PTelemetryChannel registerTelemetryChannel(const std::string& sChannelIdentifier, const std::string& sChannelDescription, LibMCData::eTelemetryChannelType channelType) override;

		bool finalizeSignal(const std::string& sUUID);

		bool findSignalPropertiesByUUID(const std::string& sSignalUUID, std::string & sInstanceName, std::string& sSignalName, std::string& sParameterData);

		AMC::eAMCSignalPhase getSignalPhase (const std::string& sSignalUUID);

		void checkForReactionTimeouts(uint64_t nGlobalTimestamp);

		void autoArchiveMessages(uint64_t nGlobalTimestamp);

		void writeMessagesToArchive(CStateSignalArchiveWriter* pArchiveWriter);

		void changeSignalPhaseToHandled(const std::string& sSignalUUID, const std::string& sResultData, uint64_t nTimestamp);

		void changeSignalPhaseToInProcess(const std::string& sSignalUUID, uint64_t nTimestamp);
		
		void changeSignalPhaseToFailed(const std::string& sSignalUUID, const std::string& sResultData, const std::string & sErrorMessage, uint64_t nTimestamp);

		uint32_t getReactionTimeout(const std::string& sSignalUUID);

		std::string getResultDataJSON(const std::string& sSignalUUID);

	};

	
}


#endif //AMC_STATESIGNALHANDLER
