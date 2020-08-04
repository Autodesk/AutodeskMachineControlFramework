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


#include "amc_api_handler_root.hpp"

#include "libmc_interfaceexception.hpp"

#include <vector>
#include <memory>
#include <string>
#include <iostream>

#include "Libraries/libzip/zip.h"
#include "pugixml.hpp"

#define CLIENTDISTRIBUTION_XMLSCHEMA "http://schemas.autodesk.com/amc/clientdistribution/2020/07"

using namespace AMC;


#define ROOT_ZIP_READCHUNKSIZE 65536
#define ROOT_DISTFILENAME "dist.xml"

class CAPIHandler_Root_ZIPFilePtr {
private:
	zip_file_t* m_pFile;
public:

	CAPIHandler_Root_ZIPFilePtr(zip_file_t* pFile)
		: m_pFile(pFile)
	{
		if (m_pFile == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	}

	~CAPIHandler_Root_ZIPFilePtr()
	{
		if (m_pFile != nullptr) {
			zip_fclose(m_pFile);
		}
		m_pFile = nullptr;
	}

};


class CAPIHandler_Root_ZIP {

private:
	zip_error_t m_ZIPError;
	zip_t* m_ZIParchive;
	zip_source_t* m_ZIPsource;
	std::map <std::string, uint64_t> m_ZIPEntries;

public:

	CAPIHandler_Root_ZIP(const uint64_t nZIPStreamBufferSize, const uint8_t* pZIPStreamBuffer)
		: m_ZIParchive(nullptr), m_ZIPsource(nullptr)

	{

		if (pZIPStreamBuffer == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

		m_ZIPError.str = nullptr;
		m_ZIPError.sys_err = 0;
		m_ZIPError.zip_err = 0;

		zip_error_init(&m_ZIPError);

		m_ZIPsource = zip_source_buffer_create(pZIPStreamBuffer, (size_t)nZIPStreamBufferSize, 0, &m_ZIPError);

		if (m_ZIPsource == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_COULDNOTREADZIPFILE);

		m_ZIParchive = zip_open_from_source(m_ZIPsource, ZIP_RDONLY | ZIP_CHECKCONS, &m_ZIPError);
		if (m_ZIParchive == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_COULDNOTOPENZIPFILE);

		int64_t nEntryCount = zip_get_num_entries(m_ZIParchive, ZIP_FL_UNCHANGED);
		if (nEntryCount < 0)
			throw ELibMCInterfaceException(LIBMC_ERROR_COULDNOTGETZIPENTRIES);

		for (int64_t nIndex = 0; nIndex < nEntryCount; nIndex++) {
			const char* pszName = zip_get_name(m_ZIParchive, (uint64_t)nIndex, ZIP_FL_ENC_GUESS);
			m_ZIPEntries.insert(std::make_pair(pszName, nIndex));
		}

	}


	~CAPIHandler_Root_ZIP()
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


	void unzipFile(const std::string& sName, std::vector<uint8_t>& Buffer)
	{

		auto iIter = m_ZIPEntries.find(sName);
		if (iIter == m_ZIPEntries.end())
			throw ELibMCInterfaceException(LIBMC_ERROR_ZIPENTRYNOTFOUND);

		zip_stat_t Stat;
		int32_t nResult = zip_stat_index(m_ZIParchive, iIter->second, ZIP_FL_UNCHANGED, &Stat);
		if (nResult != 0)
			throw ELibMCInterfaceException(LIBMC_ERROR_COULDNOTSTATZIPENTRY);

		uint64_t nSize = Stat.size;

		zip_file_t* pFile = zip_fopen_index(m_ZIParchive, iIter->second, ZIP_FL_UNCHANGED);
		if (pFile == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_COULDNOTOPENZIPENTRY);

		CAPIHandler_Root_ZIPFilePtr pZIPFilePtr(pFile);

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
					throw ELibMCInterfaceException(LIBMC_ERROR_COULDNOTREADZIPSTREAM);
				cbBytesRead += readSize;

				if (readSize != (uint64_t)cbBytesToRead)
					break;

				pData += readSize;
			}


			if ((uint64_t)cbBytesRead != nSize) {
				throw ELibMCInterfaceException(LIBMC_ERROR_COULDNOTREADFULLZIPDATA);
			}

		}
	}


};


CAPIHandler_Root::CAPIHandler_Root()
{
	
}

CAPIHandler_Root::~CAPIHandler_Root()
{
	
}
				
std::string CAPIHandler_Root::getBaseURI () 
{
	return "";
}
		
PAPIResponse CAPIHandler_Root::handleGetRequest(const std::string& sURI)
{
	auto iIterator = m_FilesToServe.find(sURI);
	if (iIterator != m_FilesToServe.end())
		return iIterator->second;

	return nullptr;
}

PAPIResponse CAPIHandler_Root::handlePostRequest(const std::string& sURI, const uint8_t* pBodyData, const size_t nBodyDataSize)
{
	return nullptr;
}


void CAPIHandler_Root::LoadClientPackage(const uint64_t nZIPStreamBufferSize, const uint8_t* pZIPStreamBuffer)
{
	CAPIHandler_Root_ZIP ZipFile(nZIPStreamBufferSize, pZIPStreamBuffer);

	if (!ZipFile.hasFile(ROOT_DISTFILENAME))
		throw ELibMCInterfaceException(LIBMC_ERROR_COULDNOTFINDDISTRIBUTIONINDEX);

	std::vector<uint8_t> DistBuffer;
	ZipFile.unzipFile(ROOT_DISTFILENAME, DistBuffer);

	if (DistBuffer.size () == 0)
		throw ELibMCInterfaceException(LIBMC_ERROR_EMPTYDISTRIBUTIONINDEX);

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_buffer(DistBuffer.data(), DistBuffer.size());
	if (!result)
		throw ELibMCInterfaceException(LIBMC_ERROR_COULDNOTPARSEDISTRIBUTIONINDEX);


	auto rootNode = doc.child("serve");
	if (rootNode.empty())
		throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGDISTRIBUTIONINDEX);

	auto xmlnsAttrib = rootNode.attribute("xmlns");
	if (xmlnsAttrib.empty())
		throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGXMLSCHEMA);

	std::string xmlns(xmlnsAttrib.as_string());
	if (xmlns != CLIENTDISTRIBUTION_XMLSCHEMA)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDXMLSCHEMA);

	auto entryNodes = rootNode.children("entry");
	for (pugi::xml_node entryNode : entryNodes) {
		auto urlAttrib = entryNode.attribute("url");
		if (urlAttrib.empty())
			throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGDISTRIBUTIONURL);

		auto fileNameAttrib = entryNode.attribute("filename");
		if (fileNameAttrib.empty())
			throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGDISTRIBUTIONFILENAME);

		auto sizeAttrib = entryNode.attribute("size");
		if (sizeAttrib.empty())
			throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGDISTRIBUTIONSIZE);

		auto contenttypeAttrib = entryNode.attribute("contenttype");
		if (contenttypeAttrib.empty())
			throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGDISTRIBUTIONCONTENTTYPE);

		std::string sURL = urlAttrib.as_string();
		std::string sFileName = fileNameAttrib.as_string();
		std::string sContentType = contenttypeAttrib.as_string();
		uint32_t nSize = sizeAttrib.as_uint();

		auto apiResponse = std::make_shared<CAPIFixedBufferResponse>(sContentType);
		ZipFile.unzipFile(sFileName, apiResponse->getBuffer());
		m_FilesToServe.insert(std::make_pair(sURL, apiResponse));

	}


}
		


