/*++

Copyright (C) 2024 Autodesk Inc.

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


#include <string>
#include <iostream>
#include <memory>
#include <exception>
#include <vector>
#include <algorithm>
#include <map>
#include <filesystem>
#include <cctype>

#include "PugiXML/pugixml.hpp"
#include "common_importstream_native.hpp"
#include "common_exportstream_native.hpp"
#include "common_portablezipwriter.hpp"

static void writeUsage()
{
	std::cout << "Please start with createClientDist <inputpath> <outputfile>" << std::endl;
}

static std::string toLowerString(const std::string& sValue)
{
	std::string sResult = sValue;
	std::transform(sResult.begin(), sResult.end(), sResult.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
	return sResult;
}

static std::string getContentTypeFromExtension(const std::string& sExtension)
{
	if (sExtension.empty())
		return "";

	std::string sExtLower = toLowerString(sExtension);
	static const std::map<std::string, std::string> sExtensionMap = {
		{ ".ico", "image/x-icon" },
		{ ".woff", "font/woff" },
		{ ".woff2", "font/woff2" },
		{ ".ttf", "font/ttf" },
		{ ".png", "image/png" },
		{ ".jpg", "image/jpeg" },
		{ ".jpeg", "image/jpeg" },
		{ ".gif", "image/gif" },
		{ ".bmp", "image/bmp" },
		{ ".svg", "image/svg+xml" },
		{ ".html", "text/html" },
		{ ".htm", "text/html" },
		{ ".css", "text/css" },
		{ ".js", "application/javascript" },
		{ ".json", "application/json" },
		{ ".xml", "text/xml" },
		{ ".txt", "text/plain" },
		{ ".md", "text/markdown" }
	};

	auto iIter = sExtensionMap.find(sExtLower);
	if (iIter != sExtensionMap.end())
		return iIter->second;

	return "";
}

struct SDistEntry
{
	std::string m_sURL;
	std::string m_sFileName;
	uint64_t m_nSize;
	std::string m_sContentType;
	std::string m_sSourcePath;
};

class CStringXMLWriter : public pugi::xml_writer
{
private:
	std::string m_sBuffer;
public:
	void write(const void* data, size_t size) override
	{
		if (size == 0)
			return;
		const char* pChar = reinterpret_cast<const char*>(data);
		m_sBuffer.append(pChar, pChar + size);
	}

	const std::string& getBuffer() const
	{
		return m_sBuffer;
	}
};

static void addFileToZip(AMCCommon::CPortableZIPWriter& zipWriter, const std::string& sSourcePath, const std::string& sZipPath)
{
	AMCCommon::CImportStream_Native importStream(sSourcePath);
	auto pEntryStream = zipWriter.createEntry(sZipPath, 0);

	const uint64_t nBufferSize = 1024 * 64;
	std::vector<uint8_t> buffer;
	buffer.resize(nBufferSize);

	uint64_t nTotalSize = importStream.retrieveSize();
	uint64_t nRemaining = nTotalSize;

	while (nRemaining > 0) {
		uint64_t nChunk = std::min<uint64_t>(nRemaining, nBufferSize);
		uint64_t nReadBytes = importStream.readBuffer(buffer.data(), nChunk, true);
		if (nReadBytes != nChunk)
			throw std::runtime_error("could not read full data from " + sSourcePath);

		pEntryStream->writeBuffer(buffer.data(), nReadBytes);
		nRemaining -= nReadBytes;
	}

	zipWriter.closeEntry();
}

int main(int argc, char* argv[])
{
	try
	{
		if (argc < 3) {
			writeUsage();
			return 1;
		}

		std::filesystem::path inputPath = std::filesystem::path(argv[1]);
		std::string sOutputFileName = argv[2];

		if (!std::filesystem::exists(inputPath) || !std::filesystem::is_directory(inputPath))
			throw std::runtime_error("input directory does not exist: " + inputPath.u8string());

		std::vector<SDistEntry> entries;

		for (auto const& entry : std::filesystem::recursive_directory_iterator(inputPath)) {
			if (!entry.is_regular_file())
				continue;

			std::string sFileName = entry.path().filename().u8string();
			std::string sExtension = entry.path().extension().u8string();
			if (toLowerString(sExtension) == ".map")
				continue;

			std::string sRelativePath = std::filesystem::relative(entry.path(), inputPath).generic_u8string();
			std::string sURL = sRelativePath;
			if (sURL == "index.html")
				sURL = "";

			SDistEntry distEntry;
			distEntry.m_sURL = sURL;
			distEntry.m_sFileName = sFileName;
			distEntry.m_nSize = entry.file_size();
			distEntry.m_sContentType = getContentTypeFromExtension(sExtension);
			distEntry.m_sSourcePath = entry.path().u8string();

			std::cout << "Adding " << distEntry.m_sSourcePath << " (" << distEntry.m_nSize << " bytes) as " << distEntry.m_sURL << std::endl;

			entries.push_back(distEntry);
		}

		pugi::xml_document packageDoc;
		auto rootNode = packageDoc.append_child("package");
		rootNode.append_attribute("xmlns").set_value("http://schemas.autodesk.com/amc/resourcepackage/2020/07");

		for (auto& distEntry : entries) {
			auto entryNode = rootNode.append_child("entry");
			entryNode.append_attribute("name").set_value(distEntry.m_sURL.c_str());
			entryNode.append_attribute("filename").set_value(distEntry.m_sFileName.c_str());
			entryNode.append_attribute("size").set_value(std::to_string(distEntry.m_nSize).c_str());
			entryNode.append_attribute("contenttype").set_value(distEntry.m_sContentType.c_str());
		}

		CStringXMLWriter xmlWriter;
		packageDoc.save(xmlWriter, "  ", pugi::format_raw | pugi::format_no_declaration, pugi::encoding_utf8);
		std::string packageXMLString = xmlWriter.getBuffer();

		AMCCommon::PExportStream pExportStream = std::make_shared<AMCCommon::CExportStream_Native>(sOutputFileName);
		AMCCommon::CPortableZIPWriter zipWriter(pExportStream, true);

		for (auto& distEntry : entries) {
			addFileToZip(zipWriter, distEntry.m_sSourcePath, distEntry.m_sFileName);
		}

		{
			auto pEntryStream = zipWriter.createEntry("package.xml", 0);
			pEntryStream->writeBuffer(packageXMLString.c_str(), packageXMLString.length());
			zipWriter.closeEntry();
		}

		zipWriter.writeDirectory();
		return 0;
	}
	catch (std::exception& E)
	{
		std::cout << "Fatal error: " << E.what() << std::endl;
		return -1;
	}
	catch (...)
	{
		std::cout << "Unhandled fatal exception!" << std::endl;
		return -1;
	}
}
