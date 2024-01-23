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

namespace AMCData {


	CStorageWriter::CStorageWriter(const std::string& sUUID, const std::string& sPath, uint64_t nSize)	
		: m_nSize (nSize), m_sUUID (AMCCommon::CUtils::normalizeUUIDString (sUUID)), m_sPath (sPath)
	{
		m_pExportStream = std::make_shared<AMCCommon::CExportStream_Native>(sPath);
	}

	CStorageWriter::~CStorageWriter()
	{
		if (m_pExportStream.get() != nullptr) {
			m_pExportStream = nullptr;
			AMCCommon::CUtils::deleteFileFromDisk (m_sPath, false);
		}
	}

	std::string CStorageWriter::getUUID()
	{
		return m_sUUID;
	}


	void CStorageWriter::writeChunkAsync(const uint8_t* pChunkData, const uint64_t nChunkSize, const uint64_t nOffset)
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

	void CStorageWriter::finalize(const std::string& sNeededSHA256, const std::string& sNeededBlockSHA256, std::string & sCalculatedSHA256, std::string & sCalculatedBlockSHA256)
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


}


