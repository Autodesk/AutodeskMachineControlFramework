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


#ifndef __AMC_UI_FRONTENDDEFINITION
#define __AMC_UI_FRONTENDDEFINITION

#include <cstdint>
#include "common_chrono.hpp"

#include "amc_ui_expression.hpp"
#include "amc_jsonwriter.hpp"
#include "amc_frontendchangecounter.hpp"

#include <memory>
#include <map>
#include <vector>
#include <mutex>
#include <functional>

namespace AMC {

	class CParameterGroup;
	typedef std::shared_ptr<CParameterGroup> PParameterGroup;

	class CUIFrontendState;
	class CUIFrontendDefinition;

	enum class eUIFrontendDefinitionAttributeType : uint32_t {
		atUnknown = 0,
		atString = 1,
		atNumber = 2,
		atInteger = 3,
		atBoolean = 4,
		atUUID = 5,
		atArray = 6,
		atObject = 7
	};

	enum class eUIFrontendDefinitionAttributeScope : uint32_t {
		asGlobal = 0,
		asSession = 1
	};

	// Values of global attributes are re-read at least once per slot, since values from outside the
	// core (database heads, logs, data series) do not bump the frontend change counter.
	#define AMC_UI_FRONTEND_EPOCH_TIMESLOT_MS 1000

	// Identifies the state a status build reads: the frontend change counter, read before the build,
	// and a time slot. Builds of the same epoch share the values of global attributes.
	struct sUIFrontendBuildEpoch {
		uint64_t m_nChangeCounter;
		uint64_t m_nTimeSlot;

		sUIFrontendBuildEpoch();
		sUIFrontendBuildEpoch(uint64_t nChangeCounter, uint64_t nTimeSlot);

		bool operator==(const sUIFrontendBuildEpoch& other) const;

		static uint64_t currentTimeSlot();
	};

	// Writes the value sName into object. Providers must only write under sName (or nothing).
	typedef std::function<void(CJSONWriter& writer, CJSONWriterObject& object, const std::string& sName, CStateMachineData* pStateMachineData, CUIFrontendState* pFrontendState)> UIFrontendDefinitionProvider;

	class CUIFrontendDefinitionAttribute {
	private:

		std::string m_sName;
		eUIFrontendDefinitionAttributeType m_AttributeType;

		std::mutex m_SharedValueMutex;
		sUIFrontendBuildEpoch m_SharedValueEpoch;
		// The members written by the last evaluation (none or one); replaced as a whole, since the document allocator never frees.
		std::unique_ptr<rapidjson::Document> m_pSharedValue;

	public:

		CUIFrontendDefinitionAttribute(const std::string& sName, eUIFrontendDefinitionAttributeType attributeType);

		virtual ~CUIFrontendDefinitionAttribute();

		std::string getName();

		eUIFrontendDefinitionAttributeType getAttributeType();

		virtual void writeToFrontendJSON(CJSONWriter& writer, CJSONWriterObject& attributesObject, CStateMachineData* pStateMachineData, CUIFrontendState* pFrontendState) = 0;

		// Like writeToFrontendJSON, but global attributes are evaluated only once per epoch and the
		// result is shared by all sessions.
		void writeToFrontendJSONForEpoch(CJSONWriter& writer, CJSONWriterObject& attributesObject, CStateMachineData* pStateMachineData, CUIFrontendState* pFrontendState, const sUIFrontendBuildEpoch& epoch);

		// Returns the session reference of the attribute value, or an empty string.
		virtual std::string getSessionReference();

		// Returns true if the value can differ between client sessions.
		virtual bool isSessionScoped() = 0;
	};

	typedef std::shared_ptr<CUIFrontendDefinitionAttribute> PUIFrontendDefinitionAttribute;


	class CUIFrontendDefinitionExpressionAttribute : public CUIFrontendDefinitionAttribute {
	private:
		CUIExpression m_ValueExpression;

	public: 

		CUIFrontendDefinitionExpressionAttribute(const std::string& sName, eUIFrontendDefinitionAttributeType attributeType, const CUIExpression& valueExpression);

		virtual ~CUIFrontendDefinitionExpressionAttribute();

		virtual void writeToFrontendJSON(CJSONWriter& writer, CJSONWriterObject& attributesObject, CStateMachineData * pStateMachineData, CUIFrontendState* pFrontendState) override;

		virtual std::string getSessionReference() override;

		virtual bool isSessionScoped() override;

	};


	class CUIFrontendDefinitionProviderAttribute : public CUIFrontendDefinitionAttribute {
	private:
		eUIFrontendDefinitionAttributeScope m_Scope;
		UIFrontendDefinitionProvider m_Provider;

	public:

		CUIFrontendDefinitionProviderAttribute(const std::string& sName, eUIFrontendDefinitionAttributeType attributeType, eUIFrontendDefinitionAttributeScope scope, UIFrontendDefinitionProvider provider);

		virtual ~CUIFrontendDefinitionProviderAttribute();

		virtual void writeToFrontendJSON(CJSONWriter& writer, CJSONWriterObject& attributesObject, CStateMachineData* pStateMachineData, CUIFrontendState* pFrontendState) override;

		virtual bool isSessionScoped() override;

	};


	class CUIFrontendDefinitionModuleStore;
	typedef std::shared_ptr<CUIFrontendDefinitionModuleStore> PUIFrontendDefinitionModuleStore;

	class CUIFrontendDefinitionModuleStore {
	private:

		CUIFrontendDefinition* m_pFrontendDefinition;

		std::string m_sPath;
		std::string m_sUUID;
		std::string m_sModuleType;

		std::map<std::string, PUIFrontendDefinitionAttribute> m_Attributes;
		std::vector<PUIFrontendDefinitionModuleStore> m_ChildStores;

		// Static object level properties (like name or grid placement), written before the attributes.
		std::vector<std::pair<std::string, UIFrontendDefinitionProvider>> m_StructureProperties;

		bool m_bAlwaysWriteSubmodules;

		void checkNewAttributeName(const std::string& sName);

	public:
		CUIFrontendDefinitionModuleStore(CUIFrontendDefinition* pFrontendDefinition, const std::string& sModuleUUID, const std::string & sModulePath, const std::string& sModuleType);

		virtual ~CUIFrontendDefinitionModuleStore();

		PUIFrontendDefinitionAttribute registerValue (const std::string& sName, eUIFrontendDefinitionAttributeType attributeType, const CUIExpression & valueExpression);

		PUIFrontendDefinitionAttribute registerProvider(const std::string& sName, eUIFrontendDefinitionAttributeType attributeType, eUIFrontendDefinitionAttributeScope scope, UIFrontendDefinitionProvider provider);

		void registerStructureProperty(const std::string& sName, UIFrontendDefinitionProvider provider);

		std::vector<PUIFrontendDefinitionAttribute> getAttributes();

		const std::vector<std::pair<std::string, UIFrontendDefinitionProvider>>& getStructureProperties();

		// Tree structure: the definition layer owns the hierarchy
		PUIFrontendDefinitionModuleStore addChildStore(const std::string& sChildUUID, const std::string& sChildPath, const std::string& sChildModuleType);

		void addChildStore(PUIFrontendDefinitionModuleStore pChildStore);

		std::vector<PUIFrontendDefinitionModuleStore> getChildStores();

		bool hasChildren();

		// Containers write an empty submodules array, leaves omit it.
		void setAlwaysWriteSubmodules(bool bAlwaysWriteSubmodules);

		bool getAlwaysWriteSubmodules();

		std::string getModuleType();

		void setModuleType(const std::string& sModuleType);

		std::string getUUID();

		std::string getPath();

		// Collects the session references of the attributes of this store (not of its children).
		void collectSessionReferences(std::vector<std::string>& references);

	};

	class CUIFrontendDefinition {
	private:

		std::vector<PUIFrontendDefinitionModuleStore> m_ModuleStores;
		std::map<std::string, PUIFrontendDefinitionModuleStore> m_ModuleStoreUUIDMap;
		AMCCommon::PChrono m_pGlobalChrono;
		PFrontendChangeCounter m_pFrontendChangeCounter;

		// Declared session variables with their default values. Every client session gets a copy.
		PParameterGroup m_pSessionVariableDeclarations;

		// Values broadcast to all sessions; applied lazily by each session state.
		std::mutex m_BroadcastMutex;
		PParameterGroup m_pSessionVariableBroadcasts;
		std::map<std::string, uint64_t> m_SessionVariableBroadcastCounters;
		uint64_t m_nSessionVariableBroadcastCounter;

	public:

		CUIFrontendDefinition (AMCCommon::PChrono pGlobalChrono, PFrontendChangeCounter pFrontendChangeCounter);

		virtual ~CUIFrontendDefinition ();

		PUIFrontendDefinitionModuleStore registerModuleStore (const std::string& sModuleUUID, const std::string& sPath, const std::string& sModuleType = "");

		PUIFrontendDefinitionModuleStore findModuleStore(const std::string& sModuleUUID, bool bMustExist);

		AMCCommon::PChrono getGlobalChrono();	

		PFrontendChangeCounter getFrontendChangeCounter();

		void addSessionVariable(const std::string& sName, const std::string& sType, const std::string& sDescription, const std::string& sDefaultValue);

		bool hasSessionVariable(const std::string& sName);

		PParameterGroup getSessionVariableDeclarations();

		// Sets a session variable in all current sessions. Sessions created afterwards start with the declared default.
		void broadcastSessionVariable(const std::string& sName, const std::string& sValue);
		void broadcastSessionVariableAsDouble(const std::string& sName, double dValue);
		void broadcastSessionVariableAsInteger(const std::string& sName, int64_t nValue);
		void broadcastSessionVariableAsBool(const std::string& sName, bool bValue);

		uint64_t getSessionVariableBroadcastCounter();

		// Returns all broadcast values that were set after nSinceCounter.
		void getSessionVariableBroadcastsSince(uint64_t nSinceCounter, std::vector<std::pair<std::string, std::string>>& values);

		// Collects the session references of all registered module attributes.
		void collectSessionReferences(std::vector<std::string>& references);

	};

	typedef std::shared_ptr<CUIFrontendDefinition> PUIFrontendDefinition;

}

#endif //__AMC_UI_FRONTENDDEFINITION

