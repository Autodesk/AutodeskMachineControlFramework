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

#include "common_importstream_native.hpp"
#include "common_exportstream_native.hpp"
#include "common_portablezipwriter.hpp"

static void writeUsage()
{
	std::cout << "Please start with createClientSource <inputpath> <outputfile>" << std::endl;
}

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

static void addSourceFolder(AMCCommon::CPortableZIPWriter& zipWriter, const std::filesystem::path& inputPath, const std::string& sPrefix)
{
	std::filesystem::path folderPath = inputPath / "src" / sPrefix;
	if (!std::filesystem::exists(folderPath) || !std::filesystem::is_directory(folderPath))
		throw std::runtime_error("source folder does not exist: " + folderPath.u8string());

	for (auto const& entry : std::filesystem::recursive_directory_iterator(folderPath)) {
		if (!entry.is_regular_file())
			continue;

		std::string sFileName = entry.path().filename().u8string();
		if (sFileName.empty())
			continue;

		std::string sZipPath = sPrefix + "/" + sFileName;
		std::string sSourcePath = entry.path().u8string();

		std::cout << "adding " << sZipPath << "..." << std::endl;
		addFileToZip(zipWriter, sSourcePath, sZipPath);
	}
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

		AMCCommon::PExportStream pExportStream = std::make_shared<AMCCommon::CExportStream_Native>(sOutputFileName);
		AMCCommon::CPortableZIPWriter zipWriter(pExportStream, true);

		addSourceFolder(zipWriter, inputPath, "common");
		addSourceFolder(zipWriter, inputPath, "modules");
		addSourceFolder(zipWriter, inputPath, "dialogs");

		addFileToZip(zipWriter, (inputPath / "package.json").u8string(), "packages/package.json");
		addFileToZip(zipWriter, (inputPath / "package-lock.json").u8string(), "packages/package-lock.json");

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
