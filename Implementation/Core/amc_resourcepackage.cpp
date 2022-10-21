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


#include "amc_resourcepackage.hpp"

#include "common_utils.hpp"
#include "Libraries/libzip/zip.h"
#include "Libraries/pugixml/pugixml.hpp"
#include "libmc_exceptiontypes.hpp"
#include <map>

#define ROOT_ZIP_READCHUNKSIZE 65536
#define ROOT_PACKAGEFILENAME "package.xml"

namespace AMC {


	class CResourcePackage_ZIPFilePtr {
	private:
		zip_file_t* m_pFile;
	public:

		CResourcePackage_ZIPFilePtr(zip_file_t* pFile)
			: m_pFile(pFile)
		{
			LibMCAssertNotNull(m_pFile);

		}

		~CResourcePackage_ZIPFilePtr()
		{
			if (m_pFile != nullptr) {
				zip_fclose(m_pFile);
			}
			m_pFile = nullptr;
		}

	};


	class CResourcePackageZIP {

	private:
		zip_error_t m_ZIPError;
		zip_t* m_ZIParchive;
		zip_source_t* m_ZIPsource;
		std::map <std::string, uint64_t> m_ZIPEntries;
		std::string m_sZIPDebugName;
		
	public:

		CResourcePackageZIP(const uint64_t nZIPStreamBufferSize, const uint8_t* pZIPStreamBuffer, const std::string & sZIPDebugName)
			: m_ZIParchive(nullptr), m_ZIPsource(nullptr), m_sZIPDebugName (sZIPDebugName)

		{
			LibMCAssertNotNull(pZIPStreamBuffer);

			m_ZIPError.str = nullptr;
			m_ZIPError.sys_err = 0;
			m_ZIPError.zip_err = 0;

			zip_error_init(&m_ZIPError);

			m_ZIPsource = zip_source_buffer_create(pZIPStreamBuffer, (size_t)nZIPStreamBufferSize, 0, &m_ZIPError);

			if (m_ZIPsource == nullptr)
				throw ELibMCCustomException(LIBMC_ERROR_COULDNOTREADZIPFILE, sZIPDebugName);

			m_ZIParchive = zip_open_from_source(m_ZIPsource, ZIP_RDONLY | ZIP_CHECKCONS, &m_ZIPError);
			if (m_ZIParchive == nullptr)
				throw ELibMCCustomException(LIBMC_ERROR_COULDNOTOPENZIPFILE, sZIPDebugName);

			int64_t nEntryCount = zip_get_num_entries(m_ZIParchive, ZIP_FL_UNCHANGED);
			if (nEntryCount < 0)
				throw ELibMCCustomException(LIBMC_ERROR_COULDNOTGETZIPENTRIES, sZIPDebugName);

			for (int64_t nIndex = 0; nIndex < nEntryCount; nIndex++) {
				const char* pszName = zip_get_name(m_ZIParchive, (uint64_t)nIndex, ZIP_FL_ENC_GUESS);
				m_ZIPEntries.insert(std::make_pair(pszName, nIndex));
			}

		}


		~CResourcePackageZIP()
		{
			if (m_ZIParchive != nullptr)
				zip_close(m_ZIParchive);

			if (m_ZIPsource != nullptr)
				zip_source_close(m_ZIPsource);

			zip_error_fini(&m_ZIPError);

			m_ZIPsource = nullptr;
			m_ZIParchive = nullptr;

		}


		bool hasFile(const std::string& sName)
		{
			auto iIter = m_ZIPEntries.find(sName);
			return (iIter != m_ZIPEntries.end());
		}


		void unzipFileEx(const std::string& sName, uint8_t* pBuffer, const uint64_t nBufferSize)
		{
			auto iIter = m_ZIPEntries.find(sName);
			if (iIter == m_ZIPEntries.end())
				throw ELibMCCustomException(LIBMC_ERROR_ZIPENTRYNOTFOUND, m_sZIPDebugName + "|" + sName);

			zip_stat_t Stat;
			int32_t nResult = zip_stat_index(m_ZIParchive, iIter->second, ZIP_FL_UNCHANGED, &Stat);
			if (nResult != 0)
				throw ELibMCCustomException(LIBMC_ERROR_COULDNOTSTATZIPENTRY, m_sZIPDebugName + "|" + sName);

			uint64_t nSize = Stat.size;

			zip_file_t* pFile = zip_fopen_index(m_ZIParchive, iIter->second, ZIP_FL_UNCHANGED);
			if (pFile == nullptr)
				throw ELibMCCustomException(LIBMC_ERROR_COULDNOTOPENZIPENTRY, m_sZIPDebugName + "|" + sName);

			CResourcePackage_ZIPFilePtr pZIPFilePtr(pFile);
			
			if (nSize > 0) {

				LibMCAssertNotNull(pBuffer);
				if (nBufferSize < nSize)
					throw ELibMCCustomException(LIBMC_ERROR_BUFFERTOOSMALL, m_sZIPDebugName + "|" + sName);

				uint64_t cbBytesLeft = nSize;
				uint64_t cbBytesRead = 0;

				uint8_t* pData = pBuffer;
				while (cbBytesLeft > 0) {
					uint32_t cbBytesToRead;
					if (cbBytesLeft > ROOT_ZIP_READCHUNKSIZE)
						cbBytesToRead = ROOT_ZIP_READCHUNKSIZE;
					else
						cbBytesToRead = (uint32_t)cbBytesLeft;
					cbBytesLeft -= cbBytesToRead;

					uint64_t readSize = zip_fread(pFile, pData, cbBytesToRead);
					if (readSize < 0)
						throw ELibMCCustomException(LIBMC_ERROR_COULDNOTREADZIPSTREAM, m_sZIPDebugName + "|" + sName);
					cbBytesRead += readSize;

					if (readSize != (uint64_t)cbBytesToRead)
						break;

					pData += readSize;
				}


				if ((uint64_t)cbBytesRead != nSize) {
					throw ELibMCCustomException(LIBMC_ERROR_COULDNOTREADFULLZIPDATA, m_sZIPDebugName + "|" + sName);
				}

			}

		}


		void unzipFile(const std::string& sName, std::vector<uint8_t>& Buffer)
		{

			auto iIter = m_ZIPEntries.find(sName);
			if (iIter == m_ZIPEntries.end())
				throw ELibMCCustomException(LIBMC_ERROR_ZIPENTRYNOTFOUND, m_sZIPDebugName + "|" + sName);

			zip_stat_t Stat;
			int32_t nResult = zip_stat_index(m_ZIParchive, iIter->second, ZIP_FL_UNCHANGED, &Stat);
			if (nResult != 0)
				throw ELibMCCustomException(LIBMC_ERROR_COULDNOTSTATZIPENTRY, m_sZIPDebugName + "|" + sName);

			uint64_t nSize = Stat.size;

			zip_file_t* pFile = zip_fopen_index(m_ZIParchive, iIter->second, ZIP_FL_UNCHANGED);
			if (pFile == nullptr)
				throw ELibMCCustomException(LIBMC_ERROR_COULDNOTOPENZIPENTRY, m_sZIPDebugName + "|" + sName);

			CResourcePackage_ZIPFilePtr pZIPFilePtr(pFile);

			Buffer.resize(nSize);
			if (nSize > 0) {

				uint64_t cbBytesLeft = nSize;
				uint64_t cbBytesRead = 0;

				uint8_t* pData = Buffer.data();
				while (cbBytesLeft > 0) {
					uint32_t cbBytesToRead;
					if (cbBytesLeft > ROOT_ZIP_READCHUNKSIZE)
						cbBytesToRead = ROOT_ZIP_READCHUNKSIZE;
					else
						cbBytesToRead = (uint32_t)cbBytesLeft;
					cbBytesLeft -= cbBytesToRead;

					uint64_t readSize = zip_fread(pFile, pData, cbBytesToRead);
					if (readSize < 0)
						throw ELibMCCustomException(LIBMC_ERROR_COULDNOTREADZIPSTREAM, m_sZIPDebugName + "|" + sName);
					cbBytesRead += readSize;

					if (readSize != (uint64_t)cbBytesToRead)
						break;

					pData += readSize;
				}


				if ((uint64_t)cbBytesRead != nSize) {
					throw ELibMCCustomException(LIBMC_ERROR_COULDNOTREADFULLZIPDATA, m_sZIPDebugName + "|" + sName);
				}

			}
		}


	};
	


	CResourcePackageEntry::CResourcePackageEntry(const std::string& sUUID,  const std::string& sName, const std::string& sFileName, const std::string& sExtension, const std::string& sContentType, uint32_t nSize)
			: m_sName (sName), m_sFileName (sFileName), m_sContentType (sContentType), m_nSize (nSize), m_sUUID (AMCCommon::CUtils::normalizeUUIDString (sUUID)), m_sExtension (sExtension)
	{

		if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(m_sName))
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDPACKAGENAMEENTRY);

		if (!m_sExtension.empty()) {
			if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(m_sExtension))
				throw ELibMCCustomException(LIBMC_ERROR_INVALIDPACKAGEEXTENSIONENTRY);
		}
	}

	std::string CResourcePackageEntry::getName()
	{
		return m_sName;
	}

	std::string CResourcePackageEntry::getFileName()
	{
		return m_sFileName;
	}

	std::string CResourcePackageEntry::getContentType()
	{
		return m_sContentType;
	}

	std::string CResourcePackageEntry::getUUID()
	{
		return m_sUUID;
	}

	uint32_t CResourcePackageEntry::getSize()
	{
		return m_nSize;
	}

	std::string CResourcePackageEntry::getExtension()
	{
		return m_sExtension;
	}



    PResourcePackage CResourcePackage::makeFromStream(AMCCommon::PImportStream pStream, const std::string& sPackageDebugName, const std::string& sSchemaNamespace)
	{
		return makeFromStream(pStream.get(), sPackageDebugName, sSchemaNamespace);
	}

	PResourcePackage CResourcePackage::makeFromStream(AMCCommon::CImportStream* pStream, const std::string& sPackageDebugName, const std::string& sSchemaNamespace)
	{
		LibMCAssertNotNull(pStream);

		return std::make_shared<CResourcePackage>(pStream, sPackageDebugName, sSchemaNamespace);
	}

	PResourcePackage CResourcePackage::makeEmpty(const std::string& sPackageDebugName)
	{
		return std::make_shared<CResourcePackage>(sPackageDebugName);
	}

	CResourcePackage::CResourcePackage(const std::string& sPackageDebugName)
		: m_sPackageDebugName (sPackageDebugName)
	{
		m_pResourcePackageZIP = nullptr;
	}


	CResourcePackage::CResourcePackage(AMCCommon::CImportStream* pStream, const std::string& sPackageDebugName, const std::string& sSchemaNamespace)
		: m_sPackageDebugName (sPackageDebugName)
	{
		LibMCAssertNotNull(pStream);

		pStream->readIntoMemory(m_ZIPBuffer);

		if (m_ZIPBuffer.size () == 0)
			throw ELibMCCustomException(LIBMC_ERROR_COULDNOTPARSERESOURCEINDEX, m_sPackageDebugName);

		m_pResourcePackageZIP = std::make_shared<CResourcePackageZIP>(m_ZIPBuffer.size(), m_ZIPBuffer.data(), sPackageDebugName);

		if (!m_pResourcePackageZIP->hasFile(ROOT_PACKAGEFILENAME))
			throw ELibMCCustomException(LIBMC_ERROR_COULDNOTFINDRESOURCEINDEX, m_sPackageDebugName);

		std::vector<uint8_t> DistBuffer;
		m_pResourcePackageZIP->unzipFile(ROOT_PACKAGEFILENAME, DistBuffer);

		if (DistBuffer.size() == 0)
			throw ELibMCCustomException(LIBMC_ERROR_EMPTYRESOURCEINDEX, m_sPackageDebugName);

		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_buffer(DistBuffer.data(), DistBuffer.size());
		if (!result)
			throw ELibMCCustomException(LIBMC_ERROR_COULDNOTPARSERESOURCEINDEX, m_sPackageDebugName);

		auto rootNode = doc.child("package");
		if (rootNode.empty())
			throw ELibMCCustomException(LIBMC_ERROR_MISSINGRESOURCEINDEX, m_sPackageDebugName);

		auto xmlnsAttrib = rootNode.attribute("xmlns");
		if (xmlnsAttrib.empty())
			throw ELibMCCustomException(LIBMC_ERROR_MISSINGXMLSCHEMA, m_sPackageDebugName);

		std::string xmlns(xmlnsAttrib.as_string());
		if (xmlns != sSchemaNamespace)
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDXMLSCHEMA, m_sPackageDebugName);

		auto entryNodes = rootNode.children("entry");
		for (pugi::xml_node entryNode : entryNodes) {
			auto nameAttrib = entryNode.attribute("name");
			if (nameAttrib.empty())
				throw ELibMCCustomException(LIBMC_ERROR_MISSINGRESOURCENAME, m_sPackageDebugName);

			auto extensionAttrib = entryNode.attribute("extension");

			auto fileNameAttrib = entryNode.attribute("filename");
			if (fileNameAttrib.empty())
				throw ELibMCCustomException(LIBMC_ERROR_MISSINGRESOURCEFILENAME, m_sPackageDebugName);

			auto sizeAttrib = entryNode.attribute("size");
			if (sizeAttrib.empty())
				throw ELibMCCustomException(LIBMC_ERROR_MISSINGRESOURCESIZE, m_sPackageDebugName);

			auto contenttypeAttrib = entryNode.attribute("contenttype");
			if (contenttypeAttrib.empty())
				throw ELibMCCustomException(LIBMC_ERROR_MISSINGRESOURCECONTENTTYPE, m_sPackageDebugName);

			std::string sName = nameAttrib.as_string();
			std::string sFileName = fileNameAttrib.as_string();
			std::string sContentType = contenttypeAttrib.as_string();
			std::string sUUID = AMCCommon::CUtils::createUUID();
			std::string sExtension = extensionAttrib.as_string();
			uint32_t nSize = sizeAttrib.as_uint();

			auto pEntry = std::make_shared <CResourcePackageEntry> (sUUID, sName, sFileName, sContentType, nSize);
			m_Entries.push_back(pEntry);
			m_NameMap.insert(std::make_pair(sName, pEntry));
			m_UUIDMap.insert(std::make_pair (sUUID, pEntry));

		}


	}

	CResourcePackage::~CResourcePackage()
	{

	}


	uint64_t CResourcePackage::getEntryCount()
	{
		return m_Entries.size();
	}


	PResourcePackageEntry CResourcePackage::getEntry(uint64_t nIndex)
	{
		if (nIndex >= m_Entries.size())
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDINDEX, m_sPackageDebugName);

		return m_Entries[nIndex];
	}

	PResourcePackageEntry CResourcePackage::findEntryByUUID(const std::string& sUUID, const bool bHasToExist)
	{
		auto iIter = m_UUIDMap.find(AMCCommon::CUtils::normalizeUUIDString (sUUID));
		if (iIter == m_UUIDMap.end()) {
			if (bHasToExist)
				throw ELibMCCustomException(LIBMC_ERROR_RESOURCEENTRYNOTFOUND, m_sPackageDebugName + "/" + sUUID);

			return nullptr;
		}

		return iIter->second;
	}


	PResourcePackageEntry CResourcePackage::findEntryByName(const std::string& sName, const bool bHasToExist)
	{
		auto iIter = m_NameMap.find(sName);
		if (iIter == m_NameMap.end()) {
			if (bHasToExist)
				throw ELibMCCustomException(LIBMC_ERROR_RESOURCEENTRYNOTFOUND, m_sPackageDebugName + "/" + sName);

			return nullptr;
		}

		return iIter->second;
	}





	void CResourcePackage::readEntry(const std::string& sName, std::vector<uint8_t>& Buffer)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);

		auto iIter = m_NameMap.find(sName);
		if (iIter == m_NameMap.end())
			throw ELibMCCustomException(LIBMC_ERROR_RESOURCEENTRYNOTFOUND, m_sPackageDebugName + "/" + sName);

		auto sFileName = iIter->second->getFileName ();
		m_pResourcePackageZIP->unzipFile(sFileName, Buffer);

	}


	void CResourcePackage::readEntryEx(const std::string& sName, uint8_t* pBuffer, const uint64_t nBufferSize)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);

		auto iIter = m_NameMap.find(sName);
		if (iIter == m_NameMap.end())
			throw ELibMCCustomException(LIBMC_ERROR_RESOURCEENTRYNOTFOUND, m_sPackageDebugName + "/" + sName);

		auto sFileName = iIter->second->getFileName();
		m_pResourcePackageZIP->unzipFileEx(sFileName, pBuffer, nBufferSize);

	}



}


