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


#include "amc_ui_frontendsnapshot.hpp"

#define __AMCIMPL_API_CONSTANTS
#include "amc_api_constants.hpp"

#include "libmc_exceptiontypes.hpp"

using namespace AMC;


/////////////////////////////////////////////////////////////////////////////////////
// CUIFrontendSnapshot
/////////////////////////////////////////////////////////////////////////////////////

uint64_t CUIFrontendSnapshot::calculateHash(const std::string& sJSON)
{
	// FNV-1a, 64 bit
	uint64_t nHash = 14695981039346656037ULL;
	for (unsigned char cChar : sJSON) {
		nHash ^= (uint64_t)cChar;
		nHash *= 1099511628211ULL;
	}
	return nHash;
}

std::string CUIFrontendSnapshot::serializeValue(const rapidjson::Value& value)
{
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	value.Accept(writer);

	return std::string(buffer.GetString(), buffer.GetSize());
}

void CUIFrontendSnapshot::readItemArray(const rapidjson::Value& document, const char* pszArrayName)
{
	auto iArray = document.FindMember(pszArrayName);
	if ((iArray == document.MemberEnd()) || (!iArray->value.IsArray()))
		return;

	for (auto iItem = iArray->value.Begin(); iItem != iArray->value.End(); iItem++) {
		if (!iItem->IsObject())
			continue;

		auto iUUID = iItem->FindMember(AMC_API_KEY_UI_UUID);
		if ((iUUID == iItem->MemberEnd()) || (!iUUID->value.IsString()))
			continue;
		std::string sUUID = iUUID->value.GetString();

		for (auto iMember = iItem->MemberBegin(); iMember != iItem->MemberEnd(); iMember++) {
			std::string sName = iMember->name.GetString();
			if (sName != AMC_API_KEY_UI_UUID)
				setValue(sUUID, sName, serializeValue(iMember->value));
		}
	}
}

void CUIFrontendSnapshot::readPageArray(const rapidjson::Value& document, const char* pszArrayName)
{
	auto iArray = document.FindMember(pszArrayName);
	if ((iArray == document.MemberEnd()) || (!iArray->value.IsArray()))
		return;

	for (auto iPage = iArray->value.Begin(); iPage != iArray->value.End(); iPage++) {
		if (!iPage->IsObject())
			continue;

		auto iUUID = iPage->FindMember(AMC_API_KEY_UI_UUID);
		if ((iUUID == iPage->MemberEnd()) || (!iUUID->value.IsString()))
			continue;
		std::string sUUID = iUUID->value.GetString();

		for (auto iMember = iPage->MemberBegin(); iMember != iPage->MemberEnd(); iMember++) {
			std::string sName = iMember->name.GetString();
			if (sName == AMC_API_KEY_UI_MODULES) {
				if (iMember->value.IsArray())
					readModuleArray(iMember->value);
			}
			else if (sName != AMC_API_KEY_UI_UUID) {
				setValue(sUUID, sName, serializeValue(iMember->value));
			}
		}
	}
}

void CUIFrontendSnapshot::readModuleArray(const rapidjson::Value& moduleArray)
{
	for (auto iModule = moduleArray.Begin(); iModule != moduleArray.End(); iModule++) {
		if (!iModule->IsObject())
			continue;

		auto iUUID = iModule->FindMember(AMC_API_KEY_UI_UUID);
		if ((iUUID != iModule->MemberEnd()) && (iUUID->value.IsString())) {
			std::string sUUID = iUUID->value.GetString();

			auto iAttributes = iModule->FindMember("attributes");
			if ((iAttributes != iModule->MemberEnd()) && (iAttributes->value.IsObject())) {
				auto& attributes = iAttributes->value;
				for (auto iAttribute = attributes.MemberBegin(); iAttribute != attributes.MemberEnd(); iAttribute++)
					setValue(sUUID, iAttribute->name.GetString(), serializeValue(iAttribute->value));
			}
		}

		auto iSubmodules = iModule->FindMember("submodules");
		if ((iSubmodules != iModule->MemberEnd()) && (iSubmodules->value.IsArray()))
			readModuleArray(iSubmodules->value);
	}
}

void CUIFrontendSnapshot::readFromFrontendJSON(const rapidjson::Value& document)
{
	m_Stores.clear();
	if (!document.IsObject())
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDJSONFORMAT, "frontend status");

	readItemArray(document, AMC_API_KEY_UI_MENUITEMS);
	readItemArray(document, AMC_API_KEY_UI_TOOLBARITEMS);
	readPageArray(document, AMC_API_KEY_UI_PAGES);
	readPageArray(document, AMC_API_KEY_UI_CUSTOMPAGES);
	readPageArray(document, AMC_API_KEY_UI_DIALOGS);
}

void CUIFrontendSnapshot::setValue(const std::string& sStoreUUID, const std::string& sAttributeName, const std::string& sJSON)
{
	auto& attributeValue = m_Stores[sStoreUUID][sAttributeName];
	attributeValue.m_sJSON = sJSON;
	attributeValue.m_nHash = calculateHash(sJSON);
}

void CUIFrontendSnapshot::removeValue(const std::string& sStoreUUID, const std::string& sAttributeName)
{
	auto iStore = m_Stores.find(sStoreUUID);
	if (iStore == m_Stores.end())
		return;

	iStore->second.erase(sAttributeName);
	if (iStore->second.empty())
		m_Stores.erase(iStore);
}

bool CUIFrontendSnapshot::hasValue(const std::string& sStoreUUID, const std::string& sAttributeName) const
{
	auto iStore = m_Stores.find(sStoreUUID);
	if (iStore == m_Stores.end())
		return false;

	return (iStore->second.find(sAttributeName) != iStore->second.end());
}

std::string CUIFrontendSnapshot::getValue(const std::string& sStoreUUID, const std::string& sAttributeName) const
{
	auto iStore = m_Stores.find(sStoreUUID);
	if (iStore != m_Stores.end()) {
		auto iAttribute = iStore->second.find(sAttributeName);
		if (iAttribute != iStore->second.end())
			return iAttribute->second.m_sJSON;
	}

	throw ELibMCCustomException(LIBMC_ERROR_MODULEITEMNOTFOUND, sStoreUUID + "/" + sAttributeName);
}

const CUIFrontendSnapshot::StoreMap& CUIFrontendSnapshot::getStores() const
{
	return m_Stores;
}

bool CUIFrontendSnapshot::equals(const CUIFrontendSnapshot& otherSnapshot) const
{
	if (m_Stores.size() != otherSnapshot.m_Stores.size())
		return false;

	auto iOtherStore = otherSnapshot.m_Stores.begin();
	for (auto& store : m_Stores) {
		if ((store.first != iOtherStore->first) || (store.second.size() != iOtherStore->second.size()))
			return false;

		auto iOtherAttribute = iOtherStore->second.begin();
		for (auto& attribute : store.second) {
			if ((attribute.first != iOtherAttribute->first) || (attribute.second.m_sJSON != iOtherAttribute->second.m_sJSON))
				return false;
			iOtherAttribute++;
		}

		iOtherStore++;
	}

	return true;
}

void CUIFrontendSnapshot::writeToJSON(CJSONWriter& writer, CJSONWriterObject& storesObject) const
{
	for (auto& store : m_Stores) {
		CJSONWriterObject storeObject(writer);
		for (auto& attribute : store.second)
			storeObject.addRawJSON(attribute.first, attribute.second.m_sJSON);
		storesObject.addObject(store.first, storeObject);
	}
}


/////////////////////////////////////////////////////////////////////////////////////
// CUIFrontendPatch
/////////////////////////////////////////////////////////////////////////////////////

CUIFrontendPatch CUIFrontendPatch::createDiff(const CUIFrontendSnapshot& fromSnapshot, const CUIFrontendSnapshot& toSnapshot)
{
	CUIFrontendPatch patch;

	auto& fromStores = fromSnapshot.getStores();
	auto& toStores = toSnapshot.getStores();

	for (auto& toStore : toStores) {
		auto iFromStore = fromStores.find(toStore.first);

		for (auto& toAttribute : toStore.second) {
			bool bChanged = true;
			if (iFromStore != fromStores.end()) {
				auto iFromAttribute = iFromStore->second.find(toAttribute.first);
				if (iFromAttribute != iFromStore->second.end())
					bChanged = (iFromAttribute->second.m_nHash != toAttribute.second.m_nHash) || (iFromAttribute->second.m_sJSON != toAttribute.second.m_sJSON);
			}

			if (bChanged)
				patch.setValue(toStore.first, toAttribute.first, toAttribute.second.m_sJSON);
		}

		if (iFromStore != fromStores.end()) {
			for (auto& fromAttribute : iFromStore->second) {
				if (toStore.second.find(fromAttribute.first) == toStore.second.end())
					patch.setRemoved(toStore.first, fromAttribute.first);
			}
		}
	}

	for (auto& fromStore : fromStores) {
		if (toStores.find(fromStore.first) == toStores.end()) {
			for (auto& fromAttribute : fromStore.second)
				patch.setRemoved(fromStore.first, fromAttribute.first);
		}
	}

	return patch;
}

void CUIFrontendPatch::setValue(const std::string& sStoreUUID, const std::string& sAttributeName, const std::string& sJSON)
{
	auto& change = m_Changes[sStoreUUID][sAttributeName];
	change.m_bRemoved = false;
	change.m_sJSON = sJSON;
}

void CUIFrontendPatch::setRemoved(const std::string& sStoreUUID, const std::string& sAttributeName)
{
	auto& change = m_Changes[sStoreUUID][sAttributeName];
	change.m_bRemoved = true;
	change.m_sJSON.clear();
}

void CUIFrontendPatch::merge(const CUIFrontendPatch& laterPatch)
{
	for (auto& store : laterPatch.m_Changes) {
		auto& targetStore = m_Changes[store.first];
		for (auto& change : store.second)
			targetStore[change.first] = change.second;
	}
}

void CUIFrontendPatch::applyTo(CUIFrontendSnapshot& snapshot) const
{
	for (auto& store : m_Changes) {
		for (auto& change : store.second) {
			if (change.second.m_bRemoved)
				snapshot.removeValue(store.first, change.first);
			else
				snapshot.setValue(store.first, change.first, change.second.m_sJSON);
		}
	}
}

bool CUIFrontendPatch::isEmpty() const
{
	return m_Changes.empty();
}

const CUIFrontendPatch::StoreChangeMap& CUIFrontendPatch::getChanges() const
{
	return m_Changes;
}

void CUIFrontendPatch::writeToJSON(CJSONWriter& writer, CJSONWriterObject& changedObject) const
{
	for (auto& store : m_Changes) {
		CJSONWriterObject storeObject(writer);
		for (auto& change : store.second) {
			if (change.second.m_bRemoved)
				storeObject.addNull(change.first);
			else
				storeObject.addRawJSON(change.first, change.second.m_sJSON);
		}
		changedObject.addObject(store.first, storeObject);
	}
}


/////////////////////////////////////////////////////////////////////////////////////
// CUIFrontendRevisionLog
/////////////////////////////////////////////////////////////////////////////////////

CUIFrontendRevisionLog::CUIFrontendRevisionLog(size_t nMaxPatchCount, uint64_t nInitialRevision)
	: m_nRevision(nInitialRevision), m_bHasSnapshot(false), m_nMaxPatchCount(nMaxPatchCount)
{
	if (m_nMaxPatchCount == 0)
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDPARAM, "revision log depth");
}

CUIFrontendRevisionLog::sPublishResult CUIFrontendRevisionLog::publish(const CUIFrontendSnapshot& snapshot, const std::string& sScope, uint64_t nBaseRevision)
{
	std::lock_guard<std::mutex> lockGuard(m_Mutex);

	if ((!m_bHasSnapshot) || (m_sScope != sScope)) {
		m_nRevision++;
		m_bHasSnapshot = true;
		m_sScope = sScope;
		m_Snapshot = snapshot;
		m_Patches.clear();
	}
	else {
		CUIFrontendPatch patch = CUIFrontendPatch::createDiff(m_Snapshot, snapshot);
		if (!patch.isEmpty()) {
			m_nRevision++;
			m_Patches.push_back(std::make_pair(m_nRevision, std::move(patch)));
			while (m_Patches.size() > m_nMaxPatchCount)
				m_Patches.pop_front();
			m_Snapshot = snapshot;
		}
	}

	sPublishResult result;
	result.m_nRevision = m_nRevision;
	result.m_bPatchAvailable = false;

	if ((nBaseRevision != 0) && (nBaseRevision <= m_nRevision)) {
		if (nBaseRevision == m_nRevision) {
			result.m_bPatchAvailable = true;
		}
		else if ((!m_Patches.empty()) && (m_Patches.front().first <= nBaseRevision + 1)) {
			for (auto& revisionPatch : m_Patches) {
				if (revisionPatch.first > nBaseRevision)
					result.m_Patch.merge(revisionPatch.second);
			}
			result.m_bPatchAvailable = true;
		}
	}

	return result;
}

uint64_t CUIFrontendRevisionLog::getRevision()
{
	std::lock_guard<std::mutex> lockGuard(m_Mutex);
	return m_nRevision;
}
