/*++

Copyright (C) 2025 Autodesk Inc.

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

#ifndef __AMCTEST_UNITTEST_STREAMS
#define __AMCTEST_UNITTEST_STREAMS


#include "amc_unittests.hpp"
#include "common_exportstream_native.hpp"
#include "common_importstream_native.hpp"
#include "common_portablezipwriter.hpp"
#include "common_utils.hpp"


namespace AMCUnitTest {

	class CUnitTestGroup_Streams : public CUnitTestGroup {
	public:

		std::string getTestGroupName() override {
			return "Streams";
		}

		void registerTests() override {
			registerTest("NativeRoundTrip", "Export/import stream roundtrip including seek operations", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_Streams::testNativeRoundTrip, this));
			registerTest("ZIPRoundTrip", "ZIP export stream writes expected ZIP structures", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_Streams::testZIPRoundTrip, this));
		}

		void initializeTests() override {
		}

	private:

		struct CScopedTempDir {
			std::string m_sPath;
			CScopedTempDir()
			{
				std::string sRootPath = "temp";
				if (!AMCCommon::CUtils::fileOrPathExistsOnDisk(sRootPath))
					AMCCommon::CUtils::createDirectoryOnDisk(sRootPath);
				m_sPath = sRootPath + "/amc_unittest_streams_" + AMCCommon::CUtils::createUUID();
				AMCCommon::CUtils::createDirectoryOnDisk(m_sPath);
			}
			~CScopedTempDir()
			{
				try {
					AMCCommon::CUtils::deleteDirectoryFromDisk(m_sPath, false);
				}
				catch (...) {
				}
			}
		};

		std::string joinPath(const std::string& sBase, const std::string& sEntry)
		{
			std::string baseWithDelimiter = AMCCommon::CUtils::includeTrailingPathDelimiter(sBase);
			return baseWithDelimiter + sEntry;
		}

		bool bufferContainsSequence(const std::vector<uint8_t>& buffer, const std::vector<uint8_t>& sequence)
		{
			if (sequence.empty() || (buffer.size() < sequence.size()))
				return false;

			for (size_t offset = 0; offset <= buffer.size() - sequence.size(); offset++) {
				bool matches = true;
				for (size_t index = 0; index < sequence.size(); index++) {
					if (buffer[offset + index] != sequence[index]) {
						matches = false;
						break;
					}
				}
				if (matches)
					return true;
			}

			return false;
		}

		void testNativeRoundTrip()
		{
			CScopedTempDir tempDir;
			std::string filePath = joinPath(tempDir.m_sPath, "stream.bin");

			{
				AMCCommon::CExportStream_Native exportStream(filePath);
				const char* initialData = "abcdef";
				exportStream.writeBuffer(initialData, 6);
				assertTrue(exportStream.getPosition() == 6);

				exportStream.seekPosition(2, true);
				exportStream.writeBuffer("ZZ", 2);
				assertTrue(exportStream.getPosition() == 4);

				exportStream.seekForward(1, true);
				exportStream.writeBuffer("X", 1);

				exportStream.seekFromEnd(0, true);
				exportStream.writeZeros(2);
				exportStream.flushStream();
			}

			AMCCommon::CImportStream_Native importStream(filePath);
			uint64_t size = importStream.retrieveSize();
			assertTrue(size == 8);

			std::vector<uint8_t> buffer;
			buffer.resize(size);
			importStream.seekPosition(0, true);
			importStream.readBuffer(buffer.data(), size, true);
			assertTrue(buffer.at(0) == 'a');
			assertTrue(buffer.at(1) == 'b');
			assertTrue(buffer.at(2) == 'Z');
			assertTrue(buffer.at(3) == 'Z');
			assertTrue(buffer.at(4) == 'e');
			assertTrue(buffer.at(5) == 'X');
			assertTrue(buffer.at(6) == 0);
			assertTrue(buffer.at(7) == 0);

			importStream.seekPosition(2, true);
			uint8_t zz[2] = { 0, 0 };
			importStream.readBuffer(zz, 2, true);
			assertTrue((zz[0] == 'Z') && (zz[1] == 'Z'));

			importStream.seekPosition(0, true);
			importStream.seekForward(1, true);
			uint8_t bChar = 0;
			importStream.readBuffer(&bChar, 1, true);
			assertTrue(bChar == 'b');

			importStream.seekFromEnd(2, true);
			uint8_t tail[2] = { 1, 1 };
			importStream.readBuffer(tail, 2, true);
			assertTrue((tail[0] == 0) && (tail[1] == 0));
		}

		void testZIPRoundTrip()
		{
			CScopedTempDir tempDir;
			std::string filePath = joinPath(tempDir.m_sPath, "stream.zip");

			const std::string entryName = "test.txt";
			const std::string entryData = "hello zip";

			{
				auto pExportStream = std::make_shared<AMCCommon::CExportStream_Native>(filePath);
				AMCCommon::CPortableZIPWriter zipWriter(pExportStream, true);
				auto pEntryStream = zipWriter.createEntry(entryName, 0);

				bool thrown = false;
				try {
					pEntryStream->seekPosition(0, true);
				}
				catch (...) {
					thrown = true;
				}
				assertTrue(thrown, "Expected ZIP export stream to reject seeking");

				pEntryStream->writeBuffer(entryData.data(), entryData.size());
				zipWriter.closeEntry();
				zipWriter.writeDirectory();
			}

			AMCCommon::CImportStream_Native importStream(filePath);
			std::vector<uint8_t> buffer;
			importStream.readIntoMemory(buffer);
			assertTrue(buffer.size() > 0);

			const std::vector<uint8_t> localHeader = { 'P', 'K', 0x03, 0x04 };
			const std::vector<uint8_t> centralHeader = { 'P', 'K', 0x01, 0x02 };
			const std::vector<uint8_t> endHeader = { 'P', 'K', 0x05, 0x06 };
			const std::vector<uint8_t> nameBytes(entryName.begin(), entryName.end());

			assertTrue(bufferContainsSequence(buffer, localHeader));
			assertTrue(bufferContainsSequence(buffer, centralHeader));
			assertTrue(bufferContainsSequence(buffer, endHeader));
			assertTrue(bufferContainsSequence(buffer, nameBytes));
		}

	};

}

#endif // __AMCTEST_UNITTEST_STREAMS
