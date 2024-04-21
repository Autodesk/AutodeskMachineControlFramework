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

#include "amcdata_storagewriter.hpp"
#include "libmcdata_interfaceexception.hpp"

#include "common_utils.hpp"
#include "common_exportstream_native.hpp"
#include "common_importstream_native.hpp"

#include "PicoSHA2/picosha2.h"

#define STORAGE_ZIPSTREAM_MAXENTRIES (1024*1024*1024)

namespace AMCData {

	CStorageWriter::CStorageWriter()
	{

	}

	CStorageWriter::~CStorageWriter()
	{

	}


	CStorageWriter_Partial::CStorageWriter_Partial(const std::string& sUUID, const std::string& sPath, uint64_t nSize)
		: CStorageWriter(), m_nSize (nSize), m_sUUID (AMCCommon::CUtils::normalizeUUIDString (sUUID)), m_sPath (sPath)
	{
		m_pExportStream = std::make_shared<AMCCommon::CExportStream_Native>(sPath);
	}

	CStorageWriter_Partial::~CStorageWriter_Partial()
	{
		if (m_pExportStream.get() != nullptr) {
			m_pExportStream = nullptr;
			AMCCommon::CUtils::deleteFileFromDisk (m_sPath, false);
		}
	}

	std::string CStorageWriter_Partial::getUUID()
	{
		return m_sUUID;
	}


	void CStorageWriter_Partial::writeChunkAsync(const uint8_t* pChunkData, const uint64_t nChunkSize, const uint64_t nOffset)
	{

		std::lock_guard<std::mutex> lockGuard(m_WriteMutex);

		if (nChunkSize > 0) {
			m_pExportStream->seekFromEnd(0, true);
			auto nSize = m_pExportStream->getPosition();

			if (nOffset > nSize) {
				m_pExportStream->writeZeros(nOffset - nSize);
			}

			if (nOffset < nSize) {
				m_pExportStream->seekPosition(nOffset, true);
			}


			m_pExportStream->writeBuffer(pChunkData, nChunkSize);
		}

	}

	void CStorageWriter_Partial::finalize(const std::string& sNeededSHA256, const std::string& sNeededBlockSHA256, std::string & sCalculatedSHA256, std::string & sCalculatedBlockSHA256)
	{
		std::lock_guard<std::mutex> lockGuard(m_WriteMutex);

		if (m_pExportStream.get() == nullptr)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_NOCURRENTUPLOAD);
		
		try {	

			m_pExportStream->seekFromEnd(0, true);
			auto nSize = m_pExportStream->getPosition();
			if (m_nSize != nSize)
				throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_UPLOADSIZEMISMATCH);

			// Free ExportStream and close file
			m_pExportStream = nullptr;

			sCalculatedSHA256 = AMCCommon::CUtils::calculateSHA256FromFile(m_sPath);
			sCalculatedBlockSHA256 = AMCCommon::CUtils::calculateBlockwiseSHA256FromFile(m_sPath, 65536);

			if (!sNeededSHA256.empty()) {
				auto sNeededSHA256Normalized = AMCCommon::CUtils::normalizeSHA256String(sNeededSHA256);
				if (sCalculatedSHA256 != sNeededSHA256Normalized)
					throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_UPLOADCHECKSUMMISMATCH);
			}

			if (!sNeededBlockSHA256.empty()) {
				auto sNeededBlockSHA256Normalized = AMCCommon::CUtils::normalizeSHA256String(sNeededBlockSHA256);
				if (sCalculatedBlockSHA256 != sNeededBlockSHA256Normalized)
					throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_UPLOADCHECKSUMMISMATCH);
			}

		}
		catch (...) {
			m_pExportStream = nullptr;

			AMCCommon::CUtils::deleteFileFromDisk(m_sPath, false);
			throw;
		}
	}


	CStorageWriter_RandomAccess::CStorageWriter_RandomAccess(const std::string& sUUID, const std::string& sPath)
		: CStorageWriter(), m_sUUID(AMCCommon::CUtils::normalizeUUIDString(sUUID)), m_sPath(sPath)
	{
		m_pExportStream = std::make_shared<AMCCommon::CExportStream_Native>(sPath);
	}

	CStorageWriter_RandomAccess::~CStorageWriter_RandomAccess()
	{
		if (m_pExportStream.get() != nullptr) {
			m_pExportStream = nullptr;
			AMCCommon::CUtils::deleteFileFromDisk(m_sPath, false);
		}
	}

	std::string CStorageWriter_RandomAccess::getUUID()
	{
		return m_sUUID;
	}


	void CStorageWriter_RandomAccess::writeChunkAsync(const uint8_t* pChunkData, const uint64_t nChunkSize, const uint64_t nOffset)
	{

		std::lock_guard<std::mutex> lockGuard(m_WriteMutex);

		if (nChunkSize > 0) {
			m_pExportStream->seekFromEnd(0, true);
			auto nSize = m_pExportStream->getPosition();

			if (nOffset > nSize) {
				m_pExportStream->writeZeros(nOffset - nSize);
			}

			if (nOffset < nSize) {
				m_pExportStream->seekPosition(nOffset, true);
			}


			m_pExportStream->writeBuffer(pChunkData, nChunkSize);
		}

	}

	void CStorageWriter_RandomAccess::finalize(std::string& sCalculatedSHA256, std::string& sCalculatedBlockSHA256)
	{
		std::lock_guard<std::mutex> lockGuard(m_WriteMutex);

		if (m_pExportStream.get() == nullptr)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_NOCURRENTUPLOAD);

		try {

			m_pExportStream->seekFromEnd(0, true);
			auto nSize = m_pExportStream->getPosition();

			// Free ExportStream and close file
			m_pExportStream = nullptr;

			sCalculatedSHA256 = AMCCommon::CUtils::calculateSHA256FromFile(m_sPath);
			sCalculatedBlockSHA256 = AMCCommon::CUtils::calculateBlockwiseSHA256FromFile(m_sPath, 65536);

		}
		catch (...) {
			m_pExportStream = nullptr;

			AMCCommon::CUtils::deleteFileFromDisk(m_sPath, false);
			throw;
		}
	}

	uint64_t CStorageWriter_RandomAccess::getCurrentSize()
	{
		std::lock_guard<std::mutex> lockGuard(m_WriteMutex);

		if (m_pExportStream.get() == nullptr)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_NOCURRENTUPLOAD);

		m_pExportStream->seekFromEnd(0, true);
		auto nSize = m_pExportStream->getPosition();

		return nSize;

	}



	CStorageWriter_ZIPStream::CStorageWriter_ZIPStream(const std::string& sUUID, const std::string& sPath)
		: CStorageWriter(), m_sUUID(AMCCommon::CUtils::normalizeUUIDString(sUUID)), 
		  m_sPath(sPath), 
		  m_nZIPSize (0),
		  m_nEntryIDCounter (1),
		  m_nCurrentEntryID (0),
		  m_nCurrentEntryDataSize (0)

	{
		m_pExportStream = std::make_shared<AMCCommon::CExportStream_Native>(sPath);
		m_pPortableZIPWriter = std::make_shared<AMCCommon::CPortableZIPWriter>(m_pExportStream, true);
	}

	CStorageWriter_ZIPStream::~CStorageWriter_ZIPStream()
	{
		m_pPortableZIPWriter = nullptr;

		if (m_pExportStream.get() != nullptr) {
			m_pExportStream = nullptr;
			AMCCommon::CUtils::deleteFileFromDisk(m_sPath, false);
		}
	}

	std::string CStorageWriter_ZIPStream::getUUID()
	{
		return m_sUUID;
	}


	void CStorageWriter_ZIPStream::writeChunkAsync(const uint8_t* pChunkData, const uint64_t nChunkSize, const uint64_t nOffset)
	{
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_ZIPSTREAMSDONOTSUPPORTASYNCCHUNKWRITE);
	}


	uint32_t CStorageWriter_ZIPStream::startNewEntry(const std::string& sFileName, uint64_t nAbsoluteTimeStamp)
	{
		finishCurrentEntry();

		std::lock_guard<std::mutex> lockGuard(m_WriteMutex);

		if (m_pExportStream.get() == nullptr)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_ZIPWRITINGALREADYFINISHED);
		if (m_pPortableZIPWriter.get () == nullptr)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_ZIPWRITINGALREADYFINISHED);

		if (m_nEntryIDCounter > STORAGE_ZIPSTREAM_MAXENTRIES)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_ZIPSTREAMEXCEEDSMAXIMUMNUMBEROFENTRIES);

		m_pExportStream->seekFromEnd (0, true);
		m_nZIPSize = m_pExportStream->getPosition();

		m_nCurrentEntryID = m_nEntryIDCounter;
		m_nEntryIDCounter++;

		// Absolute Time stamp is microseconds since 1970, Unix Time Stamp is seconds since 1970.
		uint64_t nUnixTimeStamp = (nAbsoluteTimeStamp / 1000000ULL);
		std::string sNameInZIP = AMCCommon::CUtils::removeLeadingPathDelimiter(sFileName);
		
		m_pCurrentEntryExportStream = m_pPortableZIPWriter->createEntry(sNameInZIP, nUnixTimeStamp);
		m_nCurrentEntryDataSize = 0;

		return m_nCurrentEntryID;
	}

	void CStorageWriter_ZIPStream::finishCurrentEntry()
	{
		std::lock_guard<std::mutex> lockGuard(m_WriteMutex);

		if (m_pExportStream.get() == nullptr)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_ZIPWRITINGALREADYFINISHED);
		if (m_pPortableZIPWriter.get() == nullptr)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_ZIPWRITINGALREADYFINISHED);

		if (m_nCurrentEntryID != 0) {
			m_nEntryDataSizes.insert(std::make_pair (m_nCurrentEntryID, m_nCurrentEntryDataSize));
		}

		m_pCurrentEntryExportStream = nullptr;
		m_pPortableZIPWriter->closeEntry();

		m_pExportStream->seekFromEnd(0, true);
		m_nZIPSize = m_pExportStream->getPosition();

		m_nCurrentEntryID = 0;
	}

	uint32_t CStorageWriter_ZIPStream::getOpenEntryID()
	{
		std::lock_guard<std::mutex> lockGuard(m_WriteMutex);
		return m_nCurrentEntryID;
	}

	void CStorageWriter_ZIPStream::writeToCurrentEntry(uint32_t nEntryID, const uint8_t* pChunkData, const uint64_t nChunkSize)
	{
		std::lock_guard<std::mutex> lockGuard(m_WriteMutex);

		if (m_pExportStream.get() == nullptr)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_ZIPWRITINGALREADYFINISHED);

		if ((m_nCurrentEntryID != nEntryID) || (nEntryID == 0))
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_ATTEMPTEDTOWRITETOFINISHEDZIPSTREAMENTRY);

		if (m_pCurrentEntryExportStream == nullptr)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_ATTEMPTEDTOWRITETOFINISHEDZIPSTREAMENTRY);

		if (nChunkSize > 0) {
			if (pChunkData == nullptr)
				throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

			m_pCurrentEntryExportStream->writeBuffer(pChunkData, nChunkSize);
			m_nCurrentEntryDataSize += nChunkSize;
		}
	}

	uint64_t CStorageWriter_ZIPStream::getEntrySize(uint32_t nEntryID)
	{
		std::lock_guard<std::mutex> lockGuard(m_WriteMutex);

		if (nEntryID == 0)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDZIPSTREAMENTRYID);

		if (m_nCurrentEntryID == nEntryID) {
			return m_nCurrentEntryDataSize;
		}
		else {
			auto iIter = m_nEntryDataSizes.find(nEntryID);
			if (iIter == m_nEntryDataSizes.end ())
				throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_ZIPSTREAMENTRYIDNOTFOUND);

			return iIter->second;
		}

	}

	bool CStorageWriter_ZIPStream::isFinalized()
	{
		std::lock_guard<std::mutex> lockGuard(m_WriteMutex);
		return (m_pExportStream == nullptr);
	}

	uint64_t CStorageWriter_ZIPStream::getZIPStreamSize()
	{
		std::lock_guard<std::mutex> lockGuard(m_WriteMutex);
		return m_nZIPSize;
	}
	
	void CStorageWriter_ZIPStream::finalize(std::string& sCalculatedSHA256, std::string& sCalculatedBlockSHA256)
	{
		finishCurrentEntry();

		std::lock_guard<std::mutex> lockGuard(m_WriteMutex);

		if (m_pExportStream.get() == nullptr)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_ZIPWRITINGALREADYFINISHED);

		try {

			m_pPortableZIPWriter = nullptr;

			m_pExportStream->seekFromEnd(0, true);
			m_nZIPSize = m_pExportStream->getPosition();

			// Free ExportStream and close file
			m_pExportStream = nullptr;

			sCalculatedSHA256 = AMCCommon::CUtils::calculateSHA256FromFile(m_sPath);
			sCalculatedBlockSHA256 = AMCCommon::CUtils::calculateBlockwiseSHA256FromFile(m_sPath, 65536);

		}
		catch (...) {
			m_pExportStream = nullptr;

			AMCCommon::CUtils::deleteFileFromDisk(m_sPath, false);
			throw;
		}
	}


}


