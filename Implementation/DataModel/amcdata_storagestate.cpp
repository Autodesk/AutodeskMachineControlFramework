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

#include "amcdata_storagestate.hpp"
#include "libmcdata_interfaceexception.hpp"
#include "amcdata_storagewriter.hpp"

#include "common_utils.hpp"
#include "common_chrono.hpp"

namespace AMCData {
		
	CStorageState::CStorageState(const std::string& sDataPath, const std::string& sSessionUUID)
		: m_sSessionUUID (AMCCommon::CUtils::normalizeUUIDString (sSessionUUID))
	{
		if (sDataPath.empty())
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

		m_sDataPath = sDataPath;

		// Replace all backslashes with slashes
		for (char& c : m_sDataPath) {
			if (c == '\\') {
				c = '/';
			}
		}

		// Remove trailing slash if it exists
		if (m_sDataPath.back() == '/') {
			m_sDataPath.pop_back();
		}

	}

	CStorageState::~CStorageState()
	{

	}

	std::string CStorageState::getStreamPath(const std::string& sStreamUUID)
	{
		return m_sDataPath + "/" + AMCCommon::CUtils::normalizeUUIDString(sStreamUUID) + ".stream";
	}

	std::string CStorageState::getJournalBasePath(const std::string& sTimeFileName)
	{
		return m_sDataPath + "/";
	}

	std::string CStorageState::getJournalFileName(const std::string& sTimeFileName)
	{
		return "journal_" + sTimeFileName + ".db";
	}

	std::string CStorageState::getJournalChunkBaseName(const std::string& sTimeFileName)
	{
		return "journal_" + sTimeFileName + "_chunk";
	}

	std::string CStorageState::getSessionUUID()
	{
		return m_sSessionUUID;
	}


	std::string CStorageState::storageStreamStatusToString(eStorageStreamStatus eStatus)
	{
		switch (eStatus) {
		case eStorageStreamStatus::sssNew: return "new";
		case eStorageStreamStatus::sssValidated: return "validated";
		case eStorageStreamStatus::sssArchived: return "archived";
		default: 
			throw ELibMCDataInterfaceException (LIBMCDATA_ERROR_INVALIDSTORAGESTREAMSTATUS);
		}
	}

	eStorageStreamStatus CStorageState::stringToStorageStreamStatus(const std::string& sStatus)
	{
		if (sStatus == "new")
			return eStorageStreamStatus::sssNew;
		if (sStatus == "validated")
			return eStorageStreamStatus::sssValidated;
		if (sStatus == "archived")
			return eStorageStreamStatus::sssArchived;

		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDSTORAGESTREAMSTATUS);
	}

	void CStorageState::addAcceptedContent(const std::string& sContentType)
	{
		m_AcceptedContentTypes.insert(sContentType);
	}

	void CStorageState::addImageContent(const std::string& sContentType)
	{
		m_ImageContentTypes.insert(sContentType);
	}

	bool CStorageState::isAcceptedContent(const std::string& sContentType)
	{
		auto iIter = m_AcceptedContentTypes.find(sContentType);
		return (iIter != m_AcceptedContentTypes.end());
	}
	
	bool CStorageState::isImageContent(const std::string& sContentType)
	{
		auto iIter = m_ImageContentTypes.find(sContentType);
		return (iIter != m_ImageContentTypes.end());
	}

	void CStorageState::addPartialWriter(PStorageWriter pWriter)
	{
		if (pWriter.get() == nullptr)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

		std::lock_guard<std::mutex> lockGuard(m_WriterMutex);

		std::string sUUID = pWriter->getUUID();
		auto iIter = m_PartialWriters.find(sUUID);
		if (iIter != m_PartialWriters.end ())
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_STORAGEWRITERALREADYEXISTS, "storage writer already exists: " + sUUID);

		m_PartialWriters.insert (std::make_pair (sUUID, pWriter));
	}

	bool CStorageState::hasPartialWriter(const std::string& sUUID)
	{
		std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);
		std::lock_guard<std::mutex> lockGuard(m_WriterMutex);

		auto iIter = m_PartialWriters.find(sNormalizedUUID);
		return (iIter != m_PartialWriters.end());
	}

	PStorageWriter CStorageState::findPartialWriter(const std::string& sUUID, bool bFailIfNotExisting)
	{
		std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);
		std::lock_guard<std::mutex> lockGuard(m_WriterMutex);

		auto iIter = m_PartialWriters.find(sUUID);
		if (iIter == m_PartialWriters.end()) {
			if (bFailIfNotExisting)
				throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_STORAGEWRITERDOESNOTEXIST, "storage writer does not exist: " + sNormalizedUUID);

			return nullptr;
		}

		return iIter->second;
	}

	void CStorageState::deletePartialWriter(const std::string& sUUID)
	{
		std::lock_guard<std::mutex> lockGuard(m_WriterMutex);
		m_PartialWriters.erase(sUUID);

	}

}


