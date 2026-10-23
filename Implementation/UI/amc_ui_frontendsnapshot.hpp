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


#ifndef __AMC_UI_FRONTENDSNAPSHOT
#define __AMC_UI_FRONTENDSNAPSHOT

#include "amc_jsonwriter.hpp"

#include <cstdint>
#include <deque>
#include <map>
#include <mutex>
#include <string>

#define AMC_UI_FRONTEND_REVISIONLOG_DEPTH 64

namespace AMC {

	// Flat view of a frontend status: store UUID -> attribute name -> compact JSON value.
	// Stores are menu items, toolbar items, pages, custom pages, dialogs and module stores.
	class CUIFrontendSnapshot {
	public:

		struct sAttributeValue {
			std::string m_sJSON;
			uint64_t m_nHash;
		};

		typedef std::map<std::string, sAttributeValue> AttributeMap;
		typedef std::map<std::string, AttributeMap> StoreMap;

	private:

		StoreMap m_Stores;

		void readItemArray(const rapidjson::Value& document, const char* pszArrayName);
		void readPageArray(const rapidjson::Value& document, const char* pszArrayName);
		void readModuleArray(const rapidjson::Value& moduleArray);

	public:

		static uint64_t calculateHash(const std::string& sJSON);

		static std::string serializeValue(const rapidjson::Value& value);

		void readFromFrontendJSON(const rapidjson::Value& document);

		void setValue(const std::string& sStoreUUID, const std::string& sAttributeName, const std::string& sJSON);

		// Removes the store once its last attribute is gone, so that snapshots compare structurally.
		void removeValue(const std::string& sStoreUUID, const std::string& sAttributeName);

		bool hasValue(const std::string& sStoreUUID, const std::string& sAttributeName) const;

		std::string getValue(const std::string& sStoreUUID, const std::string& sAttributeName) const;

		const StoreMap& getStores() const;

		bool equals(const CUIFrontendSnapshot& otherSnapshot) const;

		void writeToJSON(CJSONWriter& writer, CJSONWriterObject& storesObject) const;

	};


	// Attribute-level change set between two snapshots. A removed attribute is serialized as JSON null.
	class CUIFrontendPatch {
	public:

		struct sAttributeChange {
			bool m_bRemoved;
			std::string m_sJSON;
		};

		typedef std::map<std::string, sAttributeChange> AttributeChangeMap;
		typedef std::map<std::string, AttributeChangeMap> StoreChangeMap;

	private:

		StoreChangeMap m_Changes;

	public:

		static CUIFrontendPatch createDiff(const CUIFrontendSnapshot& fromSnapshot, const CUIFrontendSnapshot& toSnapshot);

		void setValue(const std::string& sStoreUUID, const std::string& sAttributeName, const std::string& sJSON);

		void setRemoved(const std::string& sStoreUUID, const std::string& sAttributeName);

		// Folds a patch that follows this one into it; the later value wins per attribute.
		void merge(const CUIFrontendPatch& laterPatch);

		void applyTo(CUIFrontendSnapshot& snapshot) const;

		bool isEmpty() const;

		const StoreChangeMap& getChanges() const;

		void writeToJSON(CJSONWriter& writer, CJSONWriterObject& changedObject) const;

	};


	// Revision history of one session. Every published snapshot that differs from the previous one
	// becomes a new revision; the last AMC_UI_FRONTEND_REVISIONLOG_DEPTH patches are kept so clients
	// can catch up from any recent revision. A change of scope (the active pages and dialogs the
	// snapshot was built for) starts a new lineage, since patches across scopes are meaningless.
	class CUIFrontendRevisionLog {
	public:

		struct sPublishResult {
			uint64_t m_nRevision;
			bool m_bPatchAvailable;
			CUIFrontendPatch m_Patch;
		};

	private:

		std::mutex m_Mutex;

		uint64_t m_nRevision;
		bool m_bHasSnapshot;
		std::string m_sScope;
		CUIFrontendSnapshot m_Snapshot;

		// Each patch is stored with the revision it leads to.
		std::deque<std::pair<uint64_t, CUIFrontendPatch>> m_Patches;
		size_t m_nMaxPatchCount;

	public:

		// The first published snapshot gets revision nInitialRevision + 1.
		CUIFrontendRevisionLog(size_t nMaxPatchCount = AMC_UI_FRONTEND_REVISIONLOG_DEPTH, uint64_t nInitialRevision = 0);

		// Records the snapshot and, if nBaseRevision is non-zero and still reachable, returns the merged
		// patch from nBaseRevision to the resulting revision. Otherwise the caller has to send the snapshot.
		sPublishResult publish(const CUIFrontendSnapshot& snapshot, const std::string& sScope, uint64_t nBaseRevision);

		uint64_t getRevision();

	};

}

#endif //__AMC_UI_FRONTENDSNAPSHOT
