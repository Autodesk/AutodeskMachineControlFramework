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


#ifndef __AMC_UI_FRONTENDSTATE
#define __AMC_UI_FRONTENDSTATE

#include "amc_parameterhandler.hpp"
#include "amc_ui_frontenddefinition.hpp"
#include "amc_ui_frontendsnapshot.hpp"
#include "amc_jsoneventstreaminstance.hpp"

#include "common_chrono.hpp"

#include <functional>
#include <mutex>

namespace AMC {

	class CUserInformation;
	typedef std::shared_ptr<CUserInformation> PUserInformation;

	class CAccessControl;
	typedef std::shared_ptr<CAccessControl> PAccessControl;

	class CUIFrontendState : public CUIExpressionSessionContext {
	private:

		PParameterHandler m_pLegacyParameterHandler;
	
		PUIFrontendDefinition m_pFrontendDefinition;

		std::mutex m_SessionMutex;
		PParameterGroup m_pSessionVariables;
		uint64_t m_nAppliedBroadcastCounter;

		std::string m_sSessionUUID;
		PUserInformation m_pUserInformation;
		PAccessControl m_pAccessControl;

		CUIFrontendRevisionLog m_RevisionLog;

		std::mutex m_EventStreamMutex;
		PJSONEventStreamInstance m_pEventStream;

		// Returns the session variable group after applying broadcasts that arrived since the last access.
		CParameterGroup* getSessionVariables();

		CParameterGroup* findSessionVariableGroup(const std::string& sName);

		std::string evaluateBuiltinReference(const std::string& sReference);

	public:

		CUIFrontendState (PUIFrontendDefinition pFrontendDefinition);

		virtual ~CUIFrontendState();

		PParameterHandler getLegacyParameterHandler ();

		void setSessionIdentity(const std::string& sSessionUUID, PUserInformation pUserInformation, PAccessControl pAccessControl);

		CUIFrontendRevisionLog& getRevisionLog();

		// Returns the session's event stream. Calls createStream if there is none or the previous one has ended.
		PJSONEventStreamInstance getOrCreateEventStream(const std::function<PJSONEventStreamInstance()>& createStream);

		// Ends and releases the session's event stream, if any. Must be called when the session closes,
		// since the stream keeps a reference to the session's authentication.
		void endEventStream();

		bool hasSessionVariable(const std::string& sName);

		std::string getSessionVariable(const std::string& sName);
		std::string getSessionVariableAsUUID(const std::string& sName);
		double getSessionVariableAsDouble(const std::string& sName);
		int64_t getSessionVariableAsInteger(const std::string& sName);
		bool getSessionVariableAsBool(const std::string& sName);

		void setSessionVariable(const std::string& sName, const std::string& sValue);
		void setSessionVariableAsUUID(const std::string& sName, const std::string& sValue);
		void setSessionVariableAsDouble(const std::string& sName, double dValue);
		void setSessionVariableAsInteger(const std::string& sName, int64_t nValue);
		void setSessionVariableAsBool(const std::string& sName, bool bValue);

		virtual std::string evaluateSessionReference(const std::string& sReference) override;
		virtual bool evaluateSessionReferenceAsBool(const std::string& sReference) override;
		virtual int64_t evaluateSessionReferenceAsInteger(const std::string& sReference) override;
		virtual double evaluateSessionReferenceAsNumber(const std::string& sReference) override;

		// Throws if a session reference is malformed, refers to an undeclared session variable or an unknown permission.
		static void validateSessionReference(const std::string& sReference, CUIFrontendDefinition* pFrontendDefinition, CAccessControl* pAccessControl);

		void writeModuleAttributesToJSON(CJSONWriter& writer, CJSONWriterObject &attributesObject, CUIFrontendDefinitionModuleStore * pModuleStore, CStateMachineData* pStateMachineData, const sUIFrontendBuildEpoch& epoch);

		// Recursive serialization: writes moduletype, uuid, attributes, and submodules (if children exist).
		// The definition tree owns the hierarchy; the stores hold flat attribute values.
		void writeModuleStoreToJSON(CJSONWriter& writer, CJSONWriterObject& moduleObject, CUIFrontendDefinitionModuleStore* pModuleStore, CStateMachineData* pStateMachineData, const sUIFrontendBuildEpoch& epoch);

	};

}

#endif //__AMC_UI_FRONTENDSTATE
