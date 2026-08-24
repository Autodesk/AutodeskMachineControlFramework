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
#include <filesystem>
#include <cstdint>

#include "common_utils.hpp"
#include "common_importstream_native.hpp"
#include "common_exportstream_native.hpp"
#include "common_portablezipwriter.hpp"

static void writeUsage()
{
	std::cout << "Please start with createDevPackage <devpackagepath> <outputpath> <githash> <systemprefix>" << std::endl;
}

static std::string ensureTrailingPath(const std::string& sPathName)
{
	return AMCCommon::CUtils::includeTrailingPathDelimiter(sPathName);
}

static uint32_t makeExternalAttributesForFile(uint32_t nUnixPermissions)
{
	uint32_t nMode = 0x8000 | (nUnixPermissions & 0x0FFF);
	return (nMode << 16);
}

static uint32_t getExternalAttributesForFile(const std::filesystem::path& filePath)
{
#ifdef _WIN32
	(void)filePath;
	return 0;
#else
	auto perms = std::filesystem::status(filePath).permissions();
	uint32_t nPerms = 0;
	if ((perms & std::filesystem::perms::owner_read) != std::filesystem::perms::none) nPerms |= 0400;
	if ((perms & std::filesystem::perms::owner_write) != std::filesystem::perms::none) nPerms |= 0200;
	if ((perms & std::filesystem::perms::owner_exec) != std::filesystem::perms::none) nPerms |= 0100;
	if ((perms & std::filesystem::perms::group_read) != std::filesystem::perms::none) nPerms |= 0040;
	if ((perms & std::filesystem::perms::group_write) != std::filesystem::perms::none) nPerms |= 0020;
	if ((perms & std::filesystem::perms::group_exec) != std::filesystem::perms::none) nPerms |= 0010;
	if ((perms & std::filesystem::perms::others_read) != std::filesystem::perms::none) nPerms |= 0004;
	if ((perms & std::filesystem::perms::others_write) != std::filesystem::perms::none) nPerms |= 0002;
	if ((perms & std::filesystem::perms::others_exec) != std::filesystem::perms::none) nPerms |= 0001;
	return makeExternalAttributesForFile(nPerms);
#endif
}

static void addFileToZip(AMCCommon::CPortableZIPWriter& zipWriter, const std::string& sSourcePath, const std::string& sZipPath, uint32_t nExternalAttributes)
{
	std::cout << "Adding " << sSourcePath << std::endl;

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

static void addDirectory(AMCCommon::CPortableZIPWriter& zipWriter, const std::string& sBasePath, const std::string& sRelativePath)
{
	std::string sDiskPath = sBasePath + sRelativePath;

	if (!std::filesystem::exists(sDiskPath) || !std::filesystem::is_directory(sDiskPath))
		throw std::runtime_error("directory does not exist: " + sDiskPath);

	for (auto const& entry : std::filesystem::recursive_directory_iterator(sDiskPath)) {
		if (!entry.is_regular_file())
			continue;

		std::string sFileName = entry.path().filename().u8string();
		if (sFileName.empty())
			continue;

		std::string sZipPath = sRelativePath + sFileName;
		std::string sSourcePath = entry.path().u8string();

		uint32_t nExternalAttributes = getExternalAttributesForFile(entry.path());
		addFileToZip(zipWriter, sSourcePath, sZipPath, nExternalAttributes);
	}
}


int main(int argc, char* argv[])
{

	try
	{
		if (argc < 5) {
			writeUsage();
			return 1;
		}

		std::string sDevPackageDir = ensureTrailingPath(argv[1]);
		std::string sOutputPath = ensureTrailingPath(argv[2]);
		std::string sGitHash = argv[3];
		std::string sSystemPrefix = argv[4];

		std::string sZipName = "amcf_" + sSystemPrefix + "_" + sGitHash + ".zip";
		std::string sZipFileName = sOutputPath + sZipName;

		AMCCommon::PExportStream pExportStream = std::make_shared<AMCCommon::CExportStream_Native>(sZipFileName);
		AMCCommon::CPortableZIPWriter zipWriter(pExportStream, true);

		addDirectory(zipWriter, sDevPackageDir, "Dist/");
		addDirectory(zipWriter, sDevPackageDir, "HeadersDev/CppDynamic/");
		addDirectory(zipWriter, sDevPackageDir, "HeadersDriver/CppDynamic/");
		addDirectory(zipWriter, sDevPackageDir, "InterfacesDev/");
		addDirectory(zipWriter, sDevPackageDir, "PluginCpp/");
		addDirectory(zipWriter, sDevPackageDir, "ClientSource/");

		if (sSystemPrefix == "win64") {
			addFileToZip(zipWriter, sDevPackageDir + "buildresources.exe", "buildresources.exe", 0);
			addFileToZip(zipWriter, sDevPackageDir + "create_package_xml.exe", "create_package_xml.exe", 0);
			addFileToZip(zipWriter, sDevPackageDir + "create_deployment_zip.exe", "create_deployment_zip.exe", 0);
		}
		else if (sSystemPrefix == "linux64") {
			addFileToZip(zipWriter, sDevPackageDir + "buildresources.linux64", "buildresources.linux64", getExternalAttributesForFile(sDevPackageDir + "buildresources.linux64"));
			addFileToZip(zipWriter, sDevPackageDir + "create_package_xml", "create_package_xml", getExternalAttributesForFile(sDevPackageDir + "create_package_xml"));
			addFileToZip(zipWriter, sDevPackageDir + "create_deployment_zip", "create_deployment_zip", getExternalAttributesForFile(sDevPackageDir + "create_deployment_zip"));
		}
		else if (sSystemPrefix == "rpi") {
			addFileToZip(zipWriter, sDevPackageDir + "buildresources.rpi", "buildresources.rpi", getExternalAttributesForFile(sDevPackageDir + "buildresources.rpi"));
			addFileToZip(zipWriter, sDevPackageDir + "create_package_xml", "create_package_xml", getExternalAttributesForFile(sDevPackageDir + "create_package_xml"));
			addFileToZip(zipWriter, sDevPackageDir + "create_deployment_zip", "create_deployment_zip", getExternalAttributesForFile(sDevPackageDir + "create_deployment_zip"));
		}
		else {
			throw std::runtime_error("Invalid system prefix: " + sSystemPrefix);
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

	return 0;
}
