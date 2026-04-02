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
#include <set>
#include <exception>
#include <vector>
#include <algorithm>
#include <cstdint>

#include "PugiXML/pugixml.hpp"
#include "common_utils.hpp"
#include "common_importstream_native.hpp"
#include "common_exportstream_native.hpp"
#include "common_portablezipwriter.hpp"

static void writeUsage()
{
	std::cout << "Usage: create_deployment_zip --package <packagefile> --output <zipfile>" << std::endl;
	std::cout << "Options are:" << std::endl;
	std::cout << "   --package <packagefile>: Path to Package XML to use. (mandatory)" << std::endl;
	std::cout << "   --output <zipfile>: Deployment ZIP file to write. (mandatory)" << std::endl;
}

static std::string getDirectoryFromPath(const std::string& sPathName)
{
	auto posSlash = sPathName.find_last_of("/\\");
	if (posSlash == std::string::npos)
		return "";
	return sPathName.substr(0, posSlash);
}

static std::string getFileNameFromPath(const std::string& sPathName)
{
	auto posSlash = sPathName.find_last_of("/\\");
	if (posSlash == std::string::npos)
		return sPathName;
	return sPathName.substr(posSlash + 1);
}

struct SZipEntry
{
	std::string m_sZipPath;
	std::string m_sSourcePath;
	bool m_bHasSource;
	uint32_t m_nExternalAttributes;
};

static uint32_t makeExternalAttributesForFile(uint32_t nUnixPermissions)
{
	uint32_t nMode = 0x8000 | (nUnixPermissions & 0x0FFF);
	return (nMode << 16);
}

static uint32_t makeExternalAttributesForDirectory(uint32_t nUnixPermissions)
{
	uint32_t nMode = 0x4000 | (nUnixPermissions & 0x0FFF);
	return (nMode << 16);
}

static bool isValidZipPath(const std::string& sZipPath)
{
	if (sZipPath.empty())
		return false;
	if ((sZipPath[0] == '/') || (sZipPath[0] == '\\'))
		return false;
	if (sZipPath.find("\\") != std::string::npos)
		return false;

	size_t startPos = 0;
	while (startPos <= sZipPath.size()) {
		size_t endPos = sZipPath.find('/', startPos);
		if (endPos == std::string::npos)
			endPos = sZipPath.size();

		size_t segmentLength = endPos - startPos;
		if (segmentLength == 0) {
			if (endPos == sZipPath.size())
				return true;
			return false;
		}

		std::string segment = sZipPath.substr(startPos, segmentLength);
		if (!AMCCommon::CUtils::stringIsValidFileName(segment))
			return false;
		if ((segment == ".") || (segment == ".."))
			return false;

		startPos = endPos + 1;
	}

	return true;
}

static void addEntry(std::vector<SZipEntry>& entries, std::set<std::string>& entryNames, const std::string& sZipPath, const std::string& sSourcePath, const std::string& sDisplayName, uint32_t nExternalAttributes)
{
	if (sZipPath.empty())
		throw std::runtime_error("missing " + sDisplayName + " name");

	if (!isValidZipPath(sZipPath))
		throw std::runtime_error("invalid " + sDisplayName + " name: " + sZipPath);

	if (!AMCCommon::CUtils::fileOrPathExistsOnDisk(sSourcePath))
		throw std::runtime_error("missing " + sDisplayName + " file: " + sSourcePath);

	if (entryNames.find(sZipPath) != entryNames.end())
		return;

	SZipEntry entry;
	entry.m_sZipPath = sZipPath;
	entry.m_sSourcePath = sSourcePath;
	entry.m_bHasSource = true;
	entry.m_nExternalAttributes = nExternalAttributes;
	entries.push_back(entry);
	entryNames.insert(sZipPath);
}

static void addEmptyEntry(std::vector<SZipEntry>& entries, std::set<std::string>& entryNames, const std::string& sZipPath, uint32_t nExternalAttributes)
{
	if (sZipPath.empty())
		throw std::runtime_error("missing entry name");

	if (!isValidZipPath(sZipPath))
		throw std::runtime_error("invalid entry name: " + sZipPath);

	if (entryNames.find(sZipPath) != entryNames.end())
		return;

	SZipEntry entry;
	entry.m_sZipPath = sZipPath;
	entry.m_sSourcePath = "";
	entry.m_bHasSource = false;
	entry.m_nExternalAttributes = nExternalAttributes;
	entries.push_back(entry);
	entryNames.insert(sZipPath);
}

static void addEntryFromPackage(std::vector<SZipEntry>& entries, std::set<std::string>& entryNames, const std::string& sBaseDir, const std::string& sFileName, const std::string& sDisplayName, uint32_t nExternalAttributes)
{
	if (sFileName.empty())
		return;

	std::string sSourcePath = sBaseDir + sFileName;
	addEntry(entries, entryNames, sFileName, sSourcePath, sDisplayName, nExternalAttributes);
}

static void addRequiredEntryFromPackage(std::vector<SZipEntry>& entries, std::set<std::string>& entryNames, const std::string& sBaseDir, const std::string& sFileName, const std::string& sDisplayName)
{
	if (sFileName.empty())
		throw std::runtime_error("missing " + sDisplayName + " name");

	std::string sSourcePath = sBaseDir + sFileName;
	addEntry(entries, entryNames, sFileName, sSourcePath, sDisplayName, 0);
}

static void addOptionalEntryFromPackage(std::vector<SZipEntry>& entries, std::set<std::string>& entryNames, const std::string& sBaseDir, const std::string& sFileName)
{
	if (sFileName.empty())
		return;

	std::string sSourcePath = sBaseDir + sFileName;
	if (!AMCCommon::CUtils::fileOrPathExistsOnDisk(sSourcePath))
		return;

	addEntry(entries, entryNames, sFileName, sSourcePath, sFileName, 0);
}

static void addFileToZip(AMCCommon::CPortableZIPWriter& zipWriter, const std::string& sSourcePath, const std::string& sZipPath, uint32_t nExternalAttributes)
{
	AMCCommon::CImportStream_Native importStream(sSourcePath);
	auto pEntryStream = zipWriter.createEntryWithExternalAttributes(sZipPath, 0, nExternalAttributes);

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

static void addEmptyEntryToZip(AMCCommon::CPortableZIPWriter& zipWriter, const std::string& sZipPath, uint32_t nExternalAttributes)
{
	zipWriter.createEntryWithExternalAttributes(sZipPath, 0, nExternalAttributes);
	zipWriter.closeEntry();
}


#ifdef _MSC_VER
int wmain(int argc, wchar_t* argv[])
#else
int main(int argc, char* argv[])
#endif
{

	try
	{
		std::cout << "---------------------------------------------------------------------" << std::endl;
		std::cout << "Autodesk Machine Control Framework - Deployment ZIP builder" << std::endl;
		std::cout << "---------------------------------------------------------------------" << std::endl;

		// Parse arguments
		std::vector<std::string> argumentList;
#ifdef _MSC_VER
		// Convert wide strings to UTF8 on Windows
		for (int argIdx = 1; argIdx < argc; argIdx++) {
			std::wstring wArg(argv[argIdx]);
			argumentList.push_back(AMCCommon::CUtils::UTF16toUTF8(wArg));
		}
#else
		for (int argIdx = 1; argIdx < argc; argIdx++)
			argumentList.push_back(std::string(argv[argIdx]));
#endif

		std::string sPackageFileName = "";
		std::string sOutputFileName = "";

		for (size_t argIdx = 0; argIdx < argumentList.size(); argIdx++) {

			bool bHandled = false;

			std::string sArgument = argumentList[argIdx];
			if (sArgument == "--package") {
				argIdx++;
				if (argIdx >= argumentList.size())
					throw std::runtime_error("missing package file name in argument");

				sPackageFileName = argumentList[argIdx];
				bHandled = true;
			}

			if (sArgument == "--output") {
				argIdx++;
				if (argIdx >= argumentList.size())
					throw std::runtime_error("missing output file name in argument");

				sOutputFileName = argumentList[argIdx];
				bHandled = true;
			}

			if (!bHandled) {
				std::cout << "Warning! Unknown argument passed: " << sArgument << std::endl;
			}

		}

		if (sPackageFileName.empty() || sOutputFileName.empty()) {
			writeUsage();
			throw std::runtime_error("missing mandatory arguments");
		}

		std::cout << "Importing " << sPackageFileName << "..." << std::endl;
		std::string sPackageAsString;
		{
			AMCCommon::CImportStream_Native packageStream(sPackageFileName);
			sPackageAsString = packageStream.readAsString();
			if (sPackageAsString.empty())
				throw std::runtime_error("package xml file is empty!");
		}

		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_string(sPackageAsString.c_str());
		if (!result)
			throw std::runtime_error("could not parse package xml!");

		auto packageNode = doc.child("amcpackage");
		if (packageNode.empty())
			throw std::runtime_error("missing package node");

		auto xmlnsAttrib = packageNode.attribute("xmlns");
		if (xmlnsAttrib.empty())
			throw std::runtime_error("missing xml schema");
		if (std::string(xmlnsAttrib.as_string()) != "http://schemas.autodesk.com/amcpackage/2020/06")
			throw std::runtime_error("invalid xml schema");

		auto buildNode = packageNode.child("build");
		if (buildNode.empty())
			throw std::runtime_error("missing build node");

		std::string sPackageDir = getDirectoryFromPath(sPackageFileName);
		if (!sPackageDir.empty())
			sPackageDir = AMCCommon::CUtils::includeTrailingPathDelimiter(sPackageDir);

		std::vector<SZipEntry> entries;
		std::set<std::string> entryNames;

		std::string sPackageFileOnly = getFileNameFromPath(sPackageFileName);
		addEntry(entries, entryNames, sPackageFileOnly, sPackageFileName, "package xml", 0);

		bool bAddedServerBinary = false;
#ifndef _WIN32
		addEntry(entries, entryNames, "amc_server", sPackageDir + "amc_server", "server binary", makeExternalAttributesForFile(0755));
		bAddedServerBinary = true;
#else
		if (AMCCommon::CUtils::fileOrPathExistsOnDisk(sPackageDir + "amc_server")) {
			addEntry(entries, entryNames, "amc_server", sPackageDir + "amc_server", "server binary", makeExternalAttributesForFile(0755));
			bAddedServerBinary = true;
		}
#endif

		if (!bAddedServerBinary) {
			addOptionalEntryFromPackage(entries, entryNames, sPackageDir, "amc_server.exe");
			addOptionalEntryFromPackage(entries, entryNames, sPackageDir, "amc_win32.exe");
			bAddedServerBinary = AMCCommon::CUtils::fileOrPathExistsOnDisk(sPackageDir + "amc_server.exe") ||
				AMCCommon::CUtils::fileOrPathExistsOnDisk(sPackageDir + "amc_win32.exe");
		}

		if (!bAddedServerBinary)
			throw std::runtime_error("missing server binary file");

		addRequiredEntryFromPackage(entries, entryNames, sPackageDir, "amc_server.xml", "server configuration");
		addEmptyEntry(entries, entryNames, "data/", makeExternalAttributesForDirectory(0755));
		addEmptyEntry(entries, entryNames, "temp/", makeExternalAttributesForDirectory(0755));

		std::string sConfigurationName = buildNode.attribute("configuration").as_string();
		addRequiredEntryFromPackage(entries, entryNames, sPackageDir, sConfigurationName, "configuration");

		std::string sCoreClient = buildNode.attribute("coreclient").as_string();
		addRequiredEntryFromPackage(entries, entryNames, sPackageDir, sCoreClient, "core client");

		auto coreClientsNode = buildNode.child("coreclients");
		if (!coreClientsNode.empty()) {
			for (auto coreClientNode : coreClientsNode.children("coreclient")) {
				std::string sClientName = coreClientNode.attribute("name").as_string();
				std::string sClientFile = coreClientNode.attribute("file").as_string();
				if (sClientName.empty())
					throw std::runtime_error("missing core client name");
				if (sClientFile.empty())
					throw std::runtime_error("missing core client file for: " + sClientName);
				addRequiredEntryFromPackage(entries, entryNames, sPackageDir, sClientFile, "core client " + sClientName);
			}
		}

		std::string sAPIDocs = buildNode.attribute("apidocs").as_string();
		addEntryFromPackage(entries, entryNames, sPackageDir, sAPIDocs, "api docs", 0);

		auto libraries = buildNode.children("library");
		for (auto libraryNode : libraries) {
			std::string libraryName = libraryNode.attribute("name").as_string();
			if (libraryName.empty())
				throw std::runtime_error("missing library name");

			std::string libraryImport = libraryNode.attribute("import").as_string();
			addRequiredEntryFromPackage(entries, entryNames, sPackageDir, libraryImport, "library import");

			std::string libraryResources = libraryNode.attribute("resources").as_string();
			addEntryFromPackage(entries, entryNames, sPackageDir, libraryResources, "library resources", 0);
		}

		std::cout << "Writing deployment ZIP to " << sOutputFileName << "..." << std::endl;

		AMCCommon::PExportStream pExportStream = std::make_shared<AMCCommon::CExportStream_Native>(sOutputFileName);
		AMCCommon::CPortableZIPWriter zipWriter(pExportStream, true);

		for (auto& entry : entries) {
			std::cout << "   - adding " << entry.m_sZipPath << std::endl;
			if (entry.m_bHasSource) {
				addFileToZip(zipWriter, entry.m_sSourcePath, entry.m_sZipPath, entry.m_nExternalAttributes);
			}
			else {
				addEmptyEntryToZip(zipWriter, entry.m_sZipPath, entry.m_nExternalAttributes);
			}
		}

		zipWriter.writeDirectory();

		std::cout << "done.." << std::endl;

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

	return 0;


}
