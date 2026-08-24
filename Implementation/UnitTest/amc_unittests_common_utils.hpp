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

#ifndef __AMCTEST_UNITTEST_COMMONUTILS
#define __AMCTEST_UNITTEST_COMMONUTILS


#include "amc_unittests.hpp"
#include "common_utils.hpp"

#include <fstream>
#include <cstdlib>

namespace AMCUnitTest {

	class CUnitTestGroup_CommonUtils : public CUnitTestGroup {
	public:

		std::string getTestGroupName() override {
			return "CommonUtils";
		}

		void registerTests() override {
			registerTest("UUIDFunctions", "UUID creation, normalization, and validation", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_CommonUtils::testUUIDFunctions, this));
			registerTest("UTF8Conversions", "UTF8/UTF16 conversions and UTF8 validation", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_CommonUtils::testUTF8Conversions, this));
			registerTest("StringConversions", "String trimming, splitting, and numeric parsing", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_CommonUtils::testStringConversions, this));
			registerTest("SHA256Functions", "SHA256 helpers for data, strings, and files", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_CommonUtils::testSHA256Functions, this));
			registerTest("Base64Functions", "Base64 encoding/decoding and RFC5987 encoding", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_CommonUtils::testBase64Functions, this));
			registerTest("FilePathFunctions", "Path handling and file/directory helpers", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_CommonUtils::testFilePathFunctions, this));
			registerTest("TempAndDirectoryFunctions", "Temporary paths, directory content, and OS helpers", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_CommonUtils::testTempAndDirectoryFunctions, this));
			registerTest("AlphanumericValidation", "Alphanumeric name and path validation", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_CommonUtils::testAlphanumericValidation, this));
			registerTest("FileNameValidation", "Filename validation for invalid characters and dot-dot", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_CommonUtils::testFileNameValidation, this));
		}

		void initializeTests() override {
		}

	private:

		struct CScopedTempDir {
			std::string m_sPath;
			CScopedTempDir()
			{
				m_sPath = std::string("amc_unittest_utils_") + AMCCommon::CUtils::createUUID();
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

		struct CScopedTempFile {
			std::string m_sPath;
			explicit CScopedTempFile(const std::string& sPath)
				: m_sPath(sPath)
			{
			}
			~CScopedTempFile()
			{
				try {
					if (AMCCommon::CUtils::fileOrPathExistsOnDisk(m_sPath)) {
						AMCCommon::CUtils::deleteFileFromDisk(m_sPath, false);
					}
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

		void writeFile(const std::string& sPath, const std::string& sContent)
		{
			std::ofstream stream(sPath, std::ios::binary);
			stream.write(sContent.data(), sContent.size());
		}

		void testUUIDFunctions()
		{
			std::string uuid = AMCCommon::CUtils::createUUID();
			assertTrue(AMCCommon::CUtils::stringIsUUIDString(uuid));
			assertTrue(AMCCommon::CUtils::stringIsNonEmptyUUIDString(uuid));
			assertTrue(AMCCommon::CUtils::normalizeUUIDString(uuid) == uuid);
			assertTrue(AMCCommon::CUtils::stringIsUUIDString("  " + uuid + "  "));

			std::string uuidV4 = AMCCommon::CUtils::createUUIDV4();
			assertTrue(AMCCommon::CUtils::stringIsUUIDString(uuidV4));
			assertTrue(AMCCommon::CUtils::stringIsNonEmptyUUIDString(uuidV4));
			assertTrue(AMCCommon::CUtils::normalizeUUIDString(uuidV4) == uuidV4);
			assertTrue(uuidV4.at(14) == '4');
			assertTrue((uuidV4.at(19) == '8') || (uuidV4.at(19) == '9') || (uuidV4.at(19) == 'a') || (uuidV4.at(19) == 'b'));

			std::string emptyUUID = AMCCommon::CUtils::createEmptyUUID();
			assertTrue(AMCCommon::CUtils::stringIsUUIDString(emptyUUID));
			assertFalse(AMCCommon::CUtils::stringIsNonEmptyUUIDString(emptyUUID));

			std::string rawUUID = " {A1B2C3D4-1111-2222-3333-444455556666} ";
			std::string normalizedUUID = AMCCommon::CUtils::normalizeUUIDString(rawUUID);
			assertTrue(normalizedUUID == "a1b2c3d4-1111-2222-3333-444455556666");

			assertFalse(AMCCommon::CUtils::stringIsUUIDString("not-a-uuid"));
			assertFalse(AMCCommon::CUtils::stringIsUUIDString("a1b2c3d4-1111-2222-3333-44445555666"));
			assertFalse(AMCCommon::CUtils::stringIsUUIDString("a1b2c3d4-1111-2222-3333-4444555566667"));
			assertFalse(AMCCommon::CUtils::stringIsUUIDString("a1b2c3d4-1111-2222-3333444455556666"));
			assertFalse(AMCCommon::CUtils::stringIsUUIDString("a1b2c3d4-1111-2222-3333-44445555666x"));
			assertFalse(AMCCommon::CUtils::stringIsUUIDString("a1b2c3d4-1111-2222-3333_444455556666"));
			assertFalse(AMCCommon::CUtils::stringIsUUIDString("A1B2C3D4-1111-2222-3333-44445555666G"));
			assertFalse(AMCCommon::CUtils::stringIsUUIDString("a1b2c3d4111122223333444455556666"));

			bool thrown = false;
			try {
				AMCCommon::CUtils::normalizeUUIDString("1234");
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected normalizeUUIDString to throw on invalid input");

			thrown = false;
			try {
				AMCCommon::CUtils::normalizeUUIDString("a1b2c3d4111122223333444455556666ff");
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected normalizeUUIDString to throw on oversized input");
		}

		void testUTF8Conversions()
		{
			std::string ascii = "Hello";
			std::wstring asciiUTF16 = AMCCommon::CUtils::UTF8toUTF16(ascii);
			assertTrue(AMCCommon::CUtils::UTF16toUTF8(asciiUTF16) == ascii);
			assertTrue(AMCCommon::CUtils::UTF8StringIsValid(ascii));

			std::string utf8Text = std::string("H") + std::string("\xC3\xB6");
			std::wstring utf16Text = AMCCommon::CUtils::UTF8toUTF16(utf8Text);
			assertTrue(AMCCommon::CUtils::UTF16toUTF8(utf16Text) == utf8Text);
			assertTrue(AMCCommon::CUtils::UTF8StringIsValid(utf8Text));

			std::string bomText = std::string("\xEF\xBB\xBF") + "abc";
			assertTrue(AMCCommon::CUtils::UTF8StringIsValid(bomText));

			std::string invalidUTF8 = std::string("\xC3\x28");
			assertFalse(AMCCommon::CUtils::UTF8StringIsValid(invalidUTF8));

			bool thrown = false;
			try {
				AMCCommon::CUtils::UTF8toUTF16(invalidUTF8);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected UTF8toUTF16 to throw on invalid UTF8");

			thrown = false;
			std::wstring invalidUTF16;
			invalidUTF16.push_back(static_cast<wchar_t>(0xD800));
			invalidUTF16.push_back(0);
			try {
				AMCCommon::CUtils::UTF16toUTF8(invalidUTF16);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected UTF16toUTF8 to throw on invalid UTF16");
		}

		void testStringConversions()
		{
			assertTrue(AMCCommon::CUtils::trimString(" \tvalue \n") == "value");
			assertTrue(AMCCommon::CUtils::toLowerString("AbC") == "abc");

			std::vector<std::string> parts;
			AMCCommon::CUtils::splitString("a,,b,c", ",", parts);
			assertTrue(parts.size() == 3);
			assertTrue(parts.at(0) == "a");
			assertTrue(parts.at(1) == "b");
			assertTrue(parts.at(2) == "c");

			bool thrown = false;
			try {
				std::vector<std::string> emptySplit;
				AMCCommon::CUtils::splitString("a,b", "", emptySplit);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected splitString to throw on empty delimiter");

			assertTrue(AMCCommon::CUtils::stringToInteger(" 42 ") == 42);
			assertTrue(AMCCommon::CUtils::stringToIntegerWithAccuracy("2.0", 0.1) == 2);
			assertTrue(AMCCommon::CUtils::stringToDouble(" 1.5 ") == 1.5);
			assertTrue(AMCCommon::CUtils::stringToBool("true"));
			assertFalse(AMCCommon::CUtils::stringToBool("false"));
			assertFalse(AMCCommon::CUtils::stringToBool("0"));
			assertTrue(AMCCommon::CUtils::stringToBool("2"));

			thrown = false;
			try {
				AMCCommon::CUtils::stringToInteger("12x");
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected stringToInteger to throw on invalid input");

			thrown = false;
			try {
				AMCCommon::CUtils::stringToIntegerWithAccuracy("2.2", 0.1);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected stringToIntegerWithAccuracy to throw when outside accuracy");

			thrown = false;
			try {
				AMCCommon::CUtils::stringToIntegerWithAccuracy("2", 0.0);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected stringToIntegerWithAccuracy to throw on invalid accuracy");

			thrown = false;
			try {
				AMCCommon::CUtils::stringToDouble("1.2.3");
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected stringToDouble to throw on invalid input");

			thrown = false;
			try {
				AMCCommon::CUtils::stringToBool("truthy");
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected stringToBool to throw on invalid input");
		}

		void testSHA256Functions()
		{
			const std::string expectedHash = "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad";
			assertTrue(AMCCommon::CUtils::calculateSHA256FromString("abc") == expectedHash);

			const uint8_t data[3] = { 'a', 'b', 'c' };
			assertTrue(AMCCommon::CUtils::calculateSHA256FromData(data, 3) == expectedHash);

			bool thrown = false;
			try {
				AMCCommon::CUtils::calculateSHA256FromData(nullptr, 0);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected calculateSHA256FromData to throw on empty input");

			CScopedTempDir tempDir;
			std::string filePath = joinPath(tempDir.m_sPath, "hash.txt");
			CScopedTempFile tempFile(filePath);
			writeFile(filePath, "abc");

			assertTrue(AMCCommon::CUtils::calculateSHA256FromFile(filePath) == expectedHash);

			std::string blockHash1 = AMCCommon::CUtils::calculateSHA256FromData(data, 2);
			std::string blockHash2 = AMCCommon::CUtils::calculateSHA256FromData(data + 2, 1);
			std::string expectedBlockwise = AMCCommon::CUtils::calculateSHA256FromString(blockHash1 + blockHash2);
			assertTrue(AMCCommon::CUtils::calculateBlockwiseSHA256FromFile(filePath, 2) == expectedBlockwise);

			std::string randomHash = AMCCommon::CUtils::calculateRandomSHA256String(1);
			assertTrue(randomHash.length() == 64);
			assertTrue(AMCCommon::CUtils::normalizeSHA256String(randomHash) == randomHash);

			std::string rawSHA = " BA78 16BF8F01CFEA414140DE5DAE2223B00361A396177A9CB410FF61F20015AD ";
			std::string normalizedSHA = AMCCommon::CUtils::normalizeSHA256String(rawSHA);
			assertTrue(normalizedSHA == expectedHash);

			thrown = false;
			try {
				AMCCommon::CUtils::normalizeSHA256String("abc");
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected normalizeSHA256String to throw on invalid length");

			thrown = false;
			try {
				AMCCommon::CUtils::normalizeSHA256String(expectedHash + "0");
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected normalizeSHA256String to throw on oversized input");

			thrown = false;
			try {
				AMCCommon::CUtils::calculateRandomSHA256String(0);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected calculateRandomSHA256String to throw on invalid iterations");

			thrown = false;
			try {
				AMCCommon::CUtils::calculateRandomSHA256String(1025);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected calculateRandomSHA256String to throw on too many iterations");
		}

		void testBase64Functions()
		{
			std::string input = "hello world";
			std::string encoded = AMCCommon::CUtils::encodeBase64(input, AMCCommon::eBase64Type::RFC4648);

			std::vector<uint8_t> decoded;
			AMCCommon::CUtils::decodeBase64(encoded, AMCCommon::eBase64Type::RFC4648, decoded);
			assertTrue(std::string(decoded.begin(), decoded.end()) == input);
			assertTrue(AMCCommon::CUtils::decodeBase64ToASCIIString(encoded, AMCCommon::eBase64Type::RFC4648) == input);

			std::string urlEncoded = AMCCommon::CUtils::encodeBase64(input, AMCCommon::eBase64Type::URL);
			decoded.clear();
			AMCCommon::CUtils::decodeBase64(urlEncoded, AMCCommon::eBase64Type::URL, decoded);
			assertTrue(std::string(decoded.begin(), decoded.end()) == input);

			bool thrown = false;
			try {
				AMCCommon::CUtils::encodeBase64(input, AMCCommon::eBase64Type::Unknown);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected encodeBase64 to throw on invalid type");

			thrown = false;
			try {
				std::vector<uint8_t> buffer;
				AMCCommon::CUtils::decodeBase64(encoded, AMCCommon::eBase64Type::Unknown, buffer);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected decodeBase64 to throw on invalid type");

			thrown = false;
			try {
				AMCCommon::CUtils::decodeBase64ToASCIIString(encoded, AMCCommon::eBase64Type::Unknown);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected decodeBase64ToASCIIString to throw on invalid type");

			std::string rawBinary;
			rawBinary.push_back(static_cast<char>(1));
			std::string encodedBinary = AMCCommon::CUtils::encodeBase64(rawBinary, AMCCommon::eBase64Type::RFC4648);
			thrown = false;
			try {
				AMCCommon::CUtils::decodeBase64ToASCIIString(encodedBinary, AMCCommon::eBase64Type::RFC4648);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected decodeBase64ToASCIIString to reject non-ASCII data");

			std::string rfc5987 = AMCCommon::CUtils::encodeRFC5987("file name.txt");
			assertTrue(rfc5987 == "UTF-8''file%20name.txt");
		}

		void testFilePathFunctions()
		{
			CScopedTempDir tempDir;
			std::string filePath = joinPath(tempDir.m_sPath, "file.txt");
			CScopedTempFile tempFile(filePath);

			assertFalse(AMCCommon::CUtils::fileOrPathExistsOnDisk(filePath));
			writeFile(filePath, "data");
			assertTrue(AMCCommon::CUtils::fileOrPathExistsOnDisk(filePath));
			assertTrue(AMCCommon::CUtils::pathIsDirectory(tempDir.m_sPath));
			assertFalse(AMCCommon::CUtils::pathIsDirectory(filePath));

			std::string newDir = joinPath(tempDir.m_sPath, "dir");
			AMCCommon::CUtils::createDirectoryOnDisk(newDir);
			assertTrue(AMCCommon::CUtils::pathIsDirectory(newDir));
			AMCCommon::CUtils::deleteDirectoryFromDisk(newDir, true);
			assertFalse(AMCCommon::CUtils::fileOrPathExistsOnDisk(newDir));

			AMCCommon::CUtils::deleteFileFromDisk(filePath, true);
			assertFalse(AMCCommon::CUtils::fileOrPathExistsOnDisk(filePath));

			std::string absolutePath = AMCCommon::CUtils::getFullPathName(tempDir.m_sPath, true);
			assertTrue(!absolutePath.empty());

			std::string nonExistingPath = joinPath(tempDir.m_sPath, "tempfile.tmp");
			std::string fullPath = AMCCommon::CUtils::getFullPathName(nonExistingPath, false);
			assertTrue(!fullPath.empty());
			assertFalse(AMCCommon::CUtils::fileOrPathExistsOnDisk(nonExistingPath));
			assertFalse(AMCCommon::CUtils::pathIsDirectory(nonExistingPath));

			char delimiter = AMCCommon::CUtils::getPathDelimiter();
#ifdef _WIN32
			assertTrue(delimiter == '\\');
#else
			assertTrue(delimiter == '/');
#endif
			std::string withDelimiter = AMCCommon::CUtils::includeTrailingPathDelimiter("path");
			assertTrue(withDelimiter.back() == '/' || withDelimiter.back() == '\\');
			assertTrue(AMCCommon::CUtils::includeTrailingPathDelimiter(withDelimiter) == withDelimiter);
			std::string emptyWithDelimiter = AMCCommon::CUtils::includeTrailingPathDelimiter("");
			assertTrue(emptyWithDelimiter.size() == 1);
			assertTrue(emptyWithDelimiter.at(0) == delimiter);
			assertTrue(AMCCommon::CUtils::removeLeadingPathDelimiter("///path") == "path");
		}

		void testTempAndDirectoryFunctions()
		{
			CScopedTempDir tempDir;
			std::string basePath = tempDir.m_sPath;
			std::string tmpFile = AMCCommon::CUtils::findTemporaryFileName(basePath, "tmp_", ".dat", 10);
			assertFalse(AMCCommon::CUtils::fileOrPathExistsOnDisk(tmpFile));

			std::string fileName = "content.txt";
			std::string dirName = "contentdir";
			std::string filePath = joinPath(basePath, fileName);
			std::string dirPath = joinPath(basePath, dirName);
			CScopedTempFile tempFile(filePath);
			writeFile(filePath, "content");
			AMCCommon::CUtils::createDirectoryOnDisk(dirPath);

			auto files = AMCCommon::CUtils::findContentOfDirectory(basePath, true, false);
			auto dirs = AMCCommon::CUtils::findContentOfDirectory(basePath, false, true);
			assertTrue(files.find(fileName) != files.end());
			assertTrue(dirs.find(dirName) != dirs.end());

			AMCCommon::CUtils::deleteDirectoryFromDisk(dirPath, true);

			std::string tmpFolder = AMCCommon::CUtils::getTempFolder();
#if defined(_WIN32)
			assertTrue(!tmpFolder.empty());
			assertTrue(AMCCommon::CUtils::fileOrPathExistsOnDisk(tmpFolder));
#else
			const char* tmpEnv = std::getenv("TMPDIR");
			if ((tmpEnv != nullptr) && (std::string(tmpEnv).length() > 0)) {
				assertTrue(!tmpFolder.empty());
				assertTrue(AMCCommon::CUtils::fileOrPathExistsOnDisk(tmpFolder));
			}
			else {
				assertTrue(tmpFolder.empty());
			}
#endif

#ifdef _WIN32
			std::string homeDir = AMCCommon::CUtils::getCurrentUserHomeDirectory();
			assertTrue(!homeDir.empty());
#else
			bool thrown = false;
			try {
				AMCCommon::CUtils::getCurrentUserHomeDirectory();
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected getCurrentUserHomeDirectory to throw on non-Windows");
#endif
		}

		void testAlphanumericValidation()
		{
			assertTrue(AMCCommon::CUtils::stringIsValidAlphanumericNameString("abc_123"));
			assertTrue(AMCCommon::CUtils::stringIsValidAlphanumericNameString("9start"));
			assertFalse(AMCCommon::CUtils::stringIsValidAlphanumericNameString("_hidden"));
			assertFalse(AMCCommon::CUtils::stringIsValidAlphanumericNameString("bad-char"));

			assertTrue(AMCCommon::CUtils::stringIsValidAlphanumericPathString("abc.def_123"));
			assertFalse(AMCCommon::CUtils::stringIsValidAlphanumericPathString("abc..def"));
			assertFalse(AMCCommon::CUtils::stringIsValidAlphanumericPathString(".abc"));
			assertFalse(AMCCommon::CUtils::stringIsValidAlphanumericPathString("abc."));
		}

		void testFileNameValidation()
		{
			assertFalse(AMCCommon::CUtils::stringIsValidFileName(""));
			assertFalse(AMCCommon::CUtils::stringIsValidFileName("file:name.txt"));
			assertFalse(AMCCommon::CUtils::stringIsValidFileName("file>name.txt"));
			assertFalse(AMCCommon::CUtils::stringIsValidFileName("file<name.txt"));
			assertFalse(AMCCommon::CUtils::stringIsValidFileName("file\"name.txt"));
			assertFalse(AMCCommon::CUtils::stringIsValidFileName("file/name.txt"));
			assertFalse(AMCCommon::CUtils::stringIsValidFileName("file\\name.txt"));
			assertFalse(AMCCommon::CUtils::stringIsValidFileName("file|name.txt"));
			assertFalse(AMCCommon::CUtils::stringIsValidFileName("file?name.txt"));
			assertFalse(AMCCommon::CUtils::stringIsValidFileName("file*name.txt"));
			assertFalse(AMCCommon::CUtils::stringIsValidFileName("file..name.txt"));

			assertTrue(AMCCommon::CUtils::stringIsValidFileName("file.name.txt"));
			assertTrue(AMCCommon::CUtils::stringIsValidFileName(".hidden"));
			assertTrue(AMCCommon::CUtils::stringIsValidFileName("file_name-123.txt"));
		}
	};

}

#endif // __AMCTEST_UNITTEST_COMMONUTILS
