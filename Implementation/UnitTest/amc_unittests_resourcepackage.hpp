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

#ifndef __AMCTEST_UNITTEST_RESOURCEPACKAGE
#define __AMCTEST_UNITTEST_RESOURCEPACKAGE


#include "amc_unittests.hpp"

#include "amc_resourcepackage.hpp"
#include "common_exportstream_native.hpp"
#include "common_importstream_native.hpp"
#include "common_portablezipwriter.hpp"
#include "common_utils.hpp"

#include <sstream>
#include <vector>
#include <cctype>
#include <cstring>

namespace AMCUnitTest {

	class CUnitTestGroup_ResourcePackage : public CUnitTestGroup {
	public:

		std::string getTestGroupName() override {
			return "ResourcePackage";
		}

		void registerTests() override {
			registerTest("CreateSingleEntryPackage", "Create package and read single entry metadata", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_ResourcePackage::testCreateSingleEntryPackage, this));
			registerTest("FindEntryByNameCaseInsensitive", "Find entry by name regardless of case", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_ResourcePackage::testFindEntryByNameCaseInsensitive, this));
			registerTest("FindEntryByUUIDRoundTrip", "Find entry by UUID after normalization", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_ResourcePackage::testFindEntryByUUIDRoundTrip, this));
			registerTest("GetEntryInvalidIndexThrows", "Invalid entry index throws", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_ResourcePackage::testGetEntryInvalidIndexThrows, this));
			registerTest("FindEntryByNameMissingReturnsNull", "Missing entry name returns null when allowed", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_ResourcePackage::testFindEntryByNameMissingReturnsNull, this));
			registerTest("FindEntryByNameMissingThrows", "Missing entry name throws when required", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_ResourcePackage::testFindEntryByNameMissingThrows, this));
			registerTest("FindEntryByUUIDMissingReturnsNull", "Missing entry UUID returns null when allowed", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_ResourcePackage::testFindEntryByUUIDMissingReturnsNull, this));
			registerTest("FindEntryByUUIDMissingThrows", "Missing entry UUID throws when required", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_ResourcePackage::testFindEntryByUUIDMissingThrows, this));
			registerTest("ReadEntryBinaryDataMatches", "Read entry returns exact binary data", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_ResourcePackage::testReadEntryBinaryDataMatches, this));
			registerTest("ReadEntryUTF8StringValid", "Read UTF8 string entry succeeds", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_ResourcePackage::testReadEntryUTF8StringValid, this));
			registerTest("ReadEntryUTF8StringInvalidThrows", "Read UTF8 string entry rejects invalid data", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_ResourcePackage::testReadEntryUTF8StringInvalidThrows, this));
			registerTest("ReadEntryExExactSize", "Read entry with exact buffer size", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_ResourcePackage::testReadEntryExExactSize, this));
			registerTest("ReadEntryExBufferTooSmallThrows", "Read entry throws on too-small buffer", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_ResourcePackage::testReadEntryExBufferTooSmallThrows, this));
			registerTest("ReadEntryMissingNameThrows", "Reading missing entry throws", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_ResourcePackage::testReadEntryMissingNameThrows, this));
			registerTest("MissingPackageXMLThrows", "Package without package.xml throws", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_ResourcePackage::testMissingPackageXMLThrows, this));
			registerTest("EmptyPackageXMLThrows", "Empty package.xml throws", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_ResourcePackage::testEmptyPackageXMLThrows, this));
			registerTest("InvalidXMLThrows", "Malformed package.xml throws", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_ResourcePackage::testInvalidXMLThrows, this));
			registerTest("MissingPackageRootThrows", "Missing package root node throws", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_ResourcePackage::testMissingPackageRootThrows, this));
			registerTest("MissingXMLNamespaceThrows", "Missing XML namespace throws", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_ResourcePackage::testMissingXMLNamespaceThrows, this));
			registerTest("WrongXMLNamespaceThrows", "Wrong XML namespace throws", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_ResourcePackage::testWrongXMLNamespaceThrows, this));
			registerTest("MissingEntryNameThrows", "Missing entry name attribute throws", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_ResourcePackage::testMissingEntryNameThrows, this));
			registerTest("MissingEntryFilenameThrows", "Missing entry filename attribute throws", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_ResourcePackage::testMissingEntryFilenameThrows, this));
			registerTest("MissingEntrySizeThrows", "Missing entry size attribute throws", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_ResourcePackage::testMissingEntrySizeThrows, this));
			registerTest("MissingEntryContentTypeThrows", "Missing entry content type attribute throws", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_ResourcePackage::testMissingEntryContentTypeThrows, this));
			registerTest("InvalidEntryExtensionThrows", "Invalid entry extension throws", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_ResourcePackage::testInvalidEntryExtensionThrows, this));
			registerTest("SizeMismatchThrows", "Size mismatch between index and ZIP throws", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_ResourcePackage::testSizeMismatchThrows, this));
			registerTest("SHA256ValidEntrySucceeds", "Valid SHA256 entry reads successfully", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_ResourcePackage::testSHA256ValidEntrySucceeds, this));
			registerTest("SHA256MismatchThrows", "SHA256 mismatch on read throws", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_ResourcePackage::testSHA256MismatchThrows, this));
			registerTest("SHA256AbsentStillWorks", "Missing SHA256 attribute is backwards compatible", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_ResourcePackage::testSHA256AbsentStillWorks, this));
		}

		void initializeTests() override {
		}

	private:

		const std::string m_sSchemaNamespace = "http://schemas.autodesk.com/amc/resourcepackage/2025/01";

		struct CPackageEntrySpec {
			std::string m_sName;
			std::string m_sFileName;
			std::string m_sExtension;
			std::string m_sContentType;
			std::string m_sData;
			std::string m_sSHA256;
		};

		struct CScopedTempDir {
			std::string m_sPath;
			CScopedTempDir()
			{
				std::string sRootPath = "temp";
				if (!AMCCommon::CUtils::fileOrPathExistsOnDisk(sRootPath))
					AMCCommon::CUtils::createDirectoryOnDisk(sRootPath);
				m_sPath = sRootPath + "/amc_unittest_resourcepackage_" + AMCCommon::CUtils::createUUID();
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

		std::string buildPackageXML(const std::string& sSchemaNamespace, const std::vector<CPackageEntrySpec>& entries)
		{
			std::stringstream ss;
			ss << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
			ss << "<package xmlns=\"" << sSchemaNamespace << "\">\n";
			for (const auto& entry : entries) {
				ss << "\t<entry name=\"" << entry.m_sName << "\" filename=\"" << entry.m_sFileName << "\" size=\"" << entry.m_sData.size() << "\" contenttype=\"" << entry.m_sContentType << "\"";
				if (!entry.m_sExtension.empty())
					ss << " extension=\"" << entry.m_sExtension << "\"";
				if (!entry.m_sSHA256.empty())
					ss << " sha256=\"" << entry.m_sSHA256 << "\"";
				ss << " />\n";
			}
			ss << "</package>\n";
			return ss.str();
		}

		std::string buildPackageXMLWithSize(const std::string& sSchemaNamespace, const std::vector<CPackageEntrySpec>& entries, uint64_t nOverrideSize)
		{
			std::stringstream ss;
			ss << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
			ss << "<package xmlns=\"" << sSchemaNamespace << "\">\n";
			for (const auto& entry : entries) {
				ss << "\t<entry name=\"" << entry.m_sName << "\" filename=\"" << entry.m_sFileName << "\" size=\"" << nOverrideSize << "\" contenttype=\"" << entry.m_sContentType << "\"";
				if (!entry.m_sExtension.empty())
					ss << " extension=\"" << entry.m_sExtension << "\"";
				if (!entry.m_sSHA256.empty())
					ss << " sha256=\"" << entry.m_sSHA256 << "\"";
				ss << " />\n";
			}
			ss << "</package>\n";
			return ss.str();
		}

		void writeZIPFile(const std::string& sZipPath, const std::string& sPackageXML, const std::vector<CPackageEntrySpec>& entries, bool bIncludePackageXML)
		{
			auto pExportStream = std::make_shared<AMCCommon::CExportStream_Native>(sZipPath);
			AMCCommon::CPortableZIPWriter zipWriter(pExportStream, true);

			if (bIncludePackageXML) {
				auto pXMLStream = zipWriter.createEntry("package.xml", 0);
				if (!sPackageXML.empty())
					pXMLStream->writeBuffer(sPackageXML.data(), sPackageXML.size());
				zipWriter.closeEntry();
			}

			for (const auto& entry : entries) {
				auto pEntryStream = zipWriter.createEntry(entry.m_sFileName, 0);
				if (!entry.m_sData.empty())
					pEntryStream->writeBuffer(entry.m_sData.data(), entry.m_sData.size());
				zipWriter.closeEntry();
			}

			zipWriter.writeDirectory();
			pExportStream->flushStream();
		}

		AMC::PResourcePackage loadPackageFromZip(const std::string& sZipPath)
		{
			AMCCommon::CImportStream_Native importStream(sZipPath);
			return AMC::CResourcePackage::makeFromStream(&importStream, "UnitTestPackage", m_sSchemaNamespace);
		}

		CPackageEntrySpec makeEntry(const std::string& sName, const std::string& sFileName, const std::string& sExtension, const std::string& sContentType, const std::string& sData)
		{
			CPackageEntrySpec entry;
			entry.m_sName = sName;
			entry.m_sFileName = sFileName;
			entry.m_sExtension = sExtension;
			entry.m_sContentType = sContentType;
			entry.m_sData = sData;
			return entry;
		}

		void testCreateSingleEntryPackage()
		{
			CScopedTempDir tempDir;
			std::string zipPath = joinPath(tempDir.m_sPath, "single_entry.zip");

			auto entry = makeEntry("ReadMe", "readme.txt", "txt", "text/plain", "Hello Resource");
			std::vector<CPackageEntrySpec> entries = { entry };
			std::string packageXML = buildPackageXML(m_sSchemaNamespace, entries);

			writeZIPFile(zipPath, packageXML, entries, true);

			auto pPackage = loadPackageFromZip(zipPath);
			assertTrue(pPackage->getEntryCount() == 1);

			auto pEntry = pPackage->getEntry(0);
			assertTrue(pEntry->getName() == "readme");
			assertTrue(pEntry->getFileName() == "readme.txt");
			assertTrue(pEntry->getExtension() == "txt");
			assertTrue(pEntry->getContentType() == "text/plain");
			assertTrue(pEntry->getSize() == entry.m_sData.size());
		}

		void testFindEntryByNameCaseInsensitive()
		{
			CScopedTempDir tempDir;
			std::string zipPath = joinPath(tempDir.m_sPath, "case_insensitive.zip");

			auto entry = makeEntry("ReadMe", "readme.txt", "txt", "text/plain", "Data");
			std::vector<CPackageEntrySpec> entries = { entry };
			std::string packageXML = buildPackageXML(m_sSchemaNamespace, entries);

			writeZIPFile(zipPath, packageXML, entries, true);
			auto pPackage = loadPackageFromZip(zipPath);

			auto pEntry = pPackage->findEntryByName("README", true);
			assertAssigned(pEntry.get(), "Expected entry to be found by name");
		}

		void testFindEntryByUUIDRoundTrip()
		{
			CScopedTempDir tempDir;
			std::string zipPath = joinPath(tempDir.m_sPath, "uuid_roundtrip.zip");

			auto entry = makeEntry("Data", "data.bin", "bin", "application/octet-stream", "payload");
			std::vector<CPackageEntrySpec> entries = { entry };
			std::string packageXML = buildPackageXML(m_sSchemaNamespace, entries);

			writeZIPFile(zipPath, packageXML, entries, true);
			auto pPackage = loadPackageFromZip(zipPath);

			auto pEntry = pPackage->getEntry(0);
			std::string uuid = pEntry->getUUID();
			for (char& ch : uuid) {
				ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
			}

			auto pFound = pPackage->findEntryByUUID(uuid, true);
			assertAssigned(pFound.get(), "Expected entry to be found by UUID");
			assertTrue(pFound->getUUID() == pEntry->getUUID());
		}

		void testGetEntryInvalidIndexThrows()
		{
			CScopedTempDir tempDir;
			std::string zipPath = joinPath(tempDir.m_sPath, "invalid_index.zip");

			auto entry = makeEntry("Item", "item.bin", "bin", "application/octet-stream", "data");
			std::vector<CPackageEntrySpec> entries = { entry };
			std::string packageXML = buildPackageXML(m_sSchemaNamespace, entries);

			writeZIPFile(zipPath, packageXML, entries, true);
			auto pPackage = loadPackageFromZip(zipPath);

			bool thrown = false;
			try {
				pPackage->getEntry(5);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected getEntry to throw on invalid index");
		}

		void testFindEntryByNameMissingReturnsNull()
		{
			CScopedTempDir tempDir;
			std::string zipPath = joinPath(tempDir.m_sPath, "missing_name_null.zip");

			auto entry = makeEntry("Item", "item.bin", "bin", "application/octet-stream", "data");
			std::vector<CPackageEntrySpec> entries = { entry };
			std::string packageXML = buildPackageXML(m_sSchemaNamespace, entries);

			writeZIPFile(zipPath, packageXML, entries, true);
			auto pPackage = loadPackageFromZip(zipPath);

			auto pMissing = pPackage->findEntryByName("unknown", false);
			assertNull(pMissing.get(), "Expected missing entry to return null");
		}

		void testFindEntryByNameMissingThrows()
		{
			CScopedTempDir tempDir;
			std::string zipPath = joinPath(tempDir.m_sPath, "missing_name_throw.zip");

			auto entry = makeEntry("Item", "item.bin", "bin", "application/octet-stream", "data");
			std::vector<CPackageEntrySpec> entries = { entry };
			std::string packageXML = buildPackageXML(m_sSchemaNamespace, entries);

			writeZIPFile(zipPath, packageXML, entries, true);
			auto pPackage = loadPackageFromZip(zipPath);

			bool thrown = false;
			try {
				pPackage->findEntryByName("unknown", true);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected findEntryByName to throw on missing entry");
		}

		void testFindEntryByUUIDMissingReturnsNull()
		{
			CScopedTempDir tempDir;
			std::string zipPath = joinPath(tempDir.m_sPath, "missing_uuid_null.zip");

			auto entry = makeEntry("Item", "item.bin", "bin", "application/octet-stream", "data");
			std::vector<CPackageEntrySpec> entries = { entry };
			std::string packageXML = buildPackageXML(m_sSchemaNamespace, entries);

			writeZIPFile(zipPath, packageXML, entries, true);
			auto pPackage = loadPackageFromZip(zipPath);

			auto pMissing = pPackage->findEntryByUUID(AMCCommon::CUtils::createUUID(), false);
			assertNull(pMissing.get(), "Expected missing UUID to return null");
		}

		void testFindEntryByUUIDMissingThrows()
		{
			CScopedTempDir tempDir;
			std::string zipPath = joinPath(tempDir.m_sPath, "missing_uuid_throw.zip");

			auto entry = makeEntry("Item", "item.bin", "bin", "application/octet-stream", "data");
			std::vector<CPackageEntrySpec> entries = { entry };
			std::string packageXML = buildPackageXML(m_sSchemaNamespace, entries);

			writeZIPFile(zipPath, packageXML, entries, true);
			auto pPackage = loadPackageFromZip(zipPath);

			bool thrown = false;
			try {
				pPackage->findEntryByUUID(AMCCommon::CUtils::createUUID(), true);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected findEntryByUUID to throw on missing entry");
		}

		void testReadEntryBinaryDataMatches()
		{
			CScopedTempDir tempDir;
			std::string zipPath = joinPath(tempDir.m_sPath, "read_binary.zip");

			std::string binaryData = std::string("ABC") + std::string("\0\1\2", 3);
			auto entry = makeEntry("Binary", "binary.bin", "bin", "application/octet-stream", binaryData);
			std::vector<CPackageEntrySpec> entries = { entry };
			std::string packageXML = buildPackageXML(m_sSchemaNamespace, entries);

			writeZIPFile(zipPath, packageXML, entries, true);
			auto pPackage = loadPackageFromZip(zipPath);

			std::vector<uint8_t> buffer;
			pPackage->readEntry("binary", buffer);
			assertTrue(buffer.size() == binaryData.size());
			assertTrue(std::memcmp(buffer.data(), binaryData.data(), buffer.size()) == 0);
		}

		void testReadEntryUTF8StringValid()
		{
			CScopedTempDir tempDir;
			std::string zipPath = joinPath(tempDir.m_sPath, "read_utf8.zip");

			auto entry = makeEntry("Text", "text.txt", "txt", "text/plain", "Hello UTF8");
			std::vector<CPackageEntrySpec> entries = { entry };
			std::string packageXML = buildPackageXML(m_sSchemaNamespace, entries);

			writeZIPFile(zipPath, packageXML, entries, true);
			auto pPackage = loadPackageFromZip(zipPath);

			std::string value = pPackage->readEntryUTF8String("text");
			assertTrue(value == "Hello UTF8");
		}

		void testReadEntryUTF8StringInvalidThrows()
		{
			CScopedTempDir tempDir;
			std::string zipPath = joinPath(tempDir.m_sPath, "read_utf8_invalid.zip");

			std::string invalidData;
			invalidData.push_back(static_cast<char>(0xC3));
			invalidData.push_back(static_cast<char>(0x28));

			auto entry = makeEntry("Text", "text.txt", "txt", "text/plain", invalidData);
			std::vector<CPackageEntrySpec> entries = { entry };
			std::string packageXML = buildPackageXML(m_sSchemaNamespace, entries);

			writeZIPFile(zipPath, packageXML, entries, true);
			auto pPackage = loadPackageFromZip(zipPath);

			bool thrown = false;
			try {
				pPackage->readEntryUTF8String("text");
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected readEntryUTF8String to throw on invalid UTF8");
		}

		void testReadEntryExExactSize()
		{
			CScopedTempDir tempDir;
			std::string zipPath = joinPath(tempDir.m_sPath, "read_exact.zip");

			auto entry = makeEntry("Data", "data.bin", "bin", "application/octet-stream", "ExactSize");
			std::vector<CPackageEntrySpec> entries = { entry };
			std::string packageXML = buildPackageXML(m_sSchemaNamespace, entries);

			writeZIPFile(zipPath, packageXML, entries, true);
			auto pPackage = loadPackageFromZip(zipPath);

			std::vector<uint8_t> buffer(entry.m_sData.size());
			pPackage->readEntryEx("data", buffer.data(), buffer.size());
			assertTrue(std::memcmp(buffer.data(), entry.m_sData.data(), buffer.size()) == 0);
		}

		void testReadEntryExBufferTooSmallThrows()
		{
			CScopedTempDir tempDir;
			std::string zipPath = joinPath(tempDir.m_sPath, "read_small_buffer.zip");

			auto entry = makeEntry("Data", "data.bin", "bin", "application/octet-stream", "ExactSize");
			std::vector<CPackageEntrySpec> entries = { entry };
			std::string packageXML = buildPackageXML(m_sSchemaNamespace, entries);

			writeZIPFile(zipPath, packageXML, entries, true);
			auto pPackage = loadPackageFromZip(zipPath);

			std::vector<uint8_t> buffer(entry.m_sData.size() - 1);
			bool thrown = false;
			try {
				pPackage->readEntryEx("data", buffer.data(), buffer.size());
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected readEntryEx to throw on small buffer");
		}

		void testReadEntryMissingNameThrows()
		{
			CScopedTempDir tempDir;
			std::string zipPath = joinPath(tempDir.m_sPath, "read_missing_entry.zip");

			auto entry = makeEntry("Data", "data.bin", "bin", "application/octet-stream", "Data");
			std::vector<CPackageEntrySpec> entries = { entry };
			std::string packageXML = buildPackageXML(m_sSchemaNamespace, entries);

			writeZIPFile(zipPath, packageXML, entries, true);
			auto pPackage = loadPackageFromZip(zipPath);

			std::vector<uint8_t> buffer;
			bool thrown = false;
			try {
				pPackage->readEntry("missing", buffer);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected readEntry to throw on missing entry");
		}

		void testMissingPackageXMLThrows()
		{
			CScopedTempDir tempDir;
			std::string zipPath = joinPath(tempDir.m_sPath, "missing_package_xml.zip");

			std::vector<CPackageEntrySpec> entries;
			auto entry = makeEntry("Data", "data.bin", "bin", "application/octet-stream", "Data");
			entries.push_back(entry);

			writeZIPFile(zipPath, "", entries, false);

			bool thrown = false;
			try {
				loadPackageFromZip(zipPath);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected missing package.xml to throw");
		}

		void testEmptyPackageXMLThrows()
		{
			CScopedTempDir tempDir;
			std::string zipPath = joinPath(tempDir.m_sPath, "empty_package_xml.zip");

			std::vector<CPackageEntrySpec> entries;
			auto entry = makeEntry("Data", "data.bin", "bin", "application/octet-stream", "Data");
			entries.push_back(entry);

			writeZIPFile(zipPath, "", entries, true);

			bool thrown = false;
			try {
				loadPackageFromZip(zipPath);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected empty package.xml to throw");
		}

		void testInvalidXMLThrows()
		{
			CScopedTempDir tempDir;
			std::string zipPath = joinPath(tempDir.m_sPath, "invalid_xml.zip");

			auto entry = makeEntry("Data", "data.bin", "bin", "application/octet-stream", "Data");
			std::vector<CPackageEntrySpec> entries = { entry };
			std::string packageXML = "<package";

			writeZIPFile(zipPath, packageXML, entries, true);

			bool thrown = false;
			try {
				loadPackageFromZip(zipPath);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected invalid XML to throw");
		}

		void testMissingPackageRootThrows()
		{
			CScopedTempDir tempDir;
			std::string zipPath = joinPath(tempDir.m_sPath, "missing_root.zip");

			auto entry = makeEntry("Data", "data.bin", "bin", "application/octet-stream", "Data");
			std::vector<CPackageEntrySpec> entries = { entry };
			std::string packageXML = "<?xml version=\"1.0\"?><root xmlns=\"" + m_sSchemaNamespace + "\"></root>";

			writeZIPFile(zipPath, packageXML, entries, true);

			bool thrown = false;
			try {
				loadPackageFromZip(zipPath);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected missing package root to throw");
		}

		void testMissingXMLNamespaceThrows()
		{
			CScopedTempDir tempDir;
			std::string zipPath = joinPath(tempDir.m_sPath, "missing_xmlns.zip");

			auto entry = makeEntry("Data", "data.bin", "bin", "application/octet-stream", "Data");
			std::vector<CPackageEntrySpec> entries = { entry };
			std::string packageXML = "<?xml version=\"1.0\"?><package></package>";

			writeZIPFile(zipPath, packageXML, entries, true);

			bool thrown = false;
			try {
				loadPackageFromZip(zipPath);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected missing xmlns to throw");
		}

		void testWrongXMLNamespaceThrows()
		{
			CScopedTempDir tempDir;
			std::string zipPath = joinPath(tempDir.m_sPath, "wrong_xmlns.zip");

			auto entry = makeEntry("Data", "data.bin", "bin", "application/octet-stream", "Data");
			std::vector<CPackageEntrySpec> entries = { entry };
			std::string packageXML = "<?xml version=\"1.0\"?><package xmlns=\"http://invalid\"></package>";

			writeZIPFile(zipPath, packageXML, entries, true);

			bool thrown = false;
			try {
				loadPackageFromZip(zipPath);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected wrong xmlns to throw");
		}

		void testMissingEntryNameThrows()
		{
			CScopedTempDir tempDir;
			std::string zipPath = joinPath(tempDir.m_sPath, "missing_entry_name.zip");

			std::string packageXML = "<?xml version=\"1.0\"?><package xmlns=\"" + m_sSchemaNamespace + "\"><entry filename=\"data.bin\" size=\"1\" contenttype=\"application/octet-stream\" /></package>";
			auto entry = makeEntry("Data", "data.bin", "bin", "application/octet-stream", "D");
			std::vector<CPackageEntrySpec> entries = { entry };

			writeZIPFile(zipPath, packageXML, entries, true);

			bool thrown = false;
			try {
				loadPackageFromZip(zipPath);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected missing entry name to throw");
		}

		void testMissingEntryFilenameThrows()
		{
			CScopedTempDir tempDir;
			std::string zipPath = joinPath(tempDir.m_sPath, "missing_entry_filename.zip");

			std::string packageXML = "<?xml version=\"1.0\"?><package xmlns=\"" + m_sSchemaNamespace + "\"><entry name=\"data\" size=\"1\" contenttype=\"application/octet-stream\" /></package>";
			auto entry = makeEntry("Data", "data.bin", "bin", "application/octet-stream", "D");
			std::vector<CPackageEntrySpec> entries = { entry };

			writeZIPFile(zipPath, packageXML, entries, true);

			bool thrown = false;
			try {
				loadPackageFromZip(zipPath);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected missing entry filename to throw");
		}

		void testMissingEntrySizeThrows()
		{
			CScopedTempDir tempDir;
			std::string zipPath = joinPath(tempDir.m_sPath, "missing_entry_size.zip");

			std::string packageXML = "<?xml version=\"1.0\"?><package xmlns=\"" + m_sSchemaNamespace + "\"><entry name=\"data\" filename=\"data.bin\" contenttype=\"application/octet-stream\" /></package>";
			auto entry = makeEntry("Data", "data.bin", "bin", "application/octet-stream", "D");
			std::vector<CPackageEntrySpec> entries = { entry };

			writeZIPFile(zipPath, packageXML, entries, true);

			bool thrown = false;
			try {
				loadPackageFromZip(zipPath);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected missing entry size to throw");
		}

		void testMissingEntryContentTypeThrows()
		{
			CScopedTempDir tempDir;
			std::string zipPath = joinPath(tempDir.m_sPath, "missing_entry_contenttype.zip");

			std::string packageXML = "<?xml version=\"1.0\"?><package xmlns=\"" + m_sSchemaNamespace + "\"><entry name=\"data\" filename=\"data.bin\" size=\"1\" /></package>";
			auto entry = makeEntry("Data", "data.bin", "bin", "application/octet-stream", "D");
			std::vector<CPackageEntrySpec> entries = { entry };

			writeZIPFile(zipPath, packageXML, entries, true);

			bool thrown = false;
			try {
				loadPackageFromZip(zipPath);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected missing entry content type to throw");
		}

		void testInvalidEntryExtensionThrows()
		{
			CScopedTempDir tempDir;
			std::string zipPath = joinPath(tempDir.m_sPath, "invalid_entry_extension.zip");

			auto entry = makeEntry("Data", "data.bin", "_bad", "application/octet-stream", "D");
			std::vector<CPackageEntrySpec> entries = { entry };
			std::string packageXML = buildPackageXML(m_sSchemaNamespace, entries);

			writeZIPFile(zipPath, packageXML, entries, true);

			bool thrown = false;
			try {
				loadPackageFromZip(zipPath);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected invalid entry extension to throw");
		}

		void testSizeMismatchThrows()
		{
			CScopedTempDir tempDir;
			std::string zipPath = joinPath(tempDir.m_sPath, "size_mismatch.zip");

			auto entry = makeEntry("Data", "data.bin", "bin", "application/octet-stream", "Hello World");
			std::vector<CPackageEntrySpec> entries = { entry };
			std::string packageXML = buildPackageXMLWithSize(m_sSchemaNamespace, entries, 999);

			writeZIPFile(zipPath, packageXML, entries, true);

			bool thrown = false;
			try {
				loadPackageFromZip(zipPath);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected size mismatch to throw during loading");
		}

		void testSHA256ValidEntrySucceeds()
		{
			CScopedTempDir tempDir;
			std::string zipPath = joinPath(tempDir.m_sPath, "sha256_valid.zip");

			std::string sData = "Hello SHA256";
			std::string sSHA256 = AMCCommon::CUtils::calculateSHA256FromData(
				reinterpret_cast<const uint8_t*>(sData.data()), sData.size());

			auto entry = makeEntry("Data", "data.bin", "bin", "application/octet-stream", sData);
			entry.m_sSHA256 = sSHA256;
			std::vector<CPackageEntrySpec> entries = { entry };
			std::string packageXML = buildPackageXML(m_sSchemaNamespace, entries);

			writeZIPFile(zipPath, packageXML, entries, true);
			auto pPackage = loadPackageFromZip(zipPath);

			std::vector<uint8_t> buffer;
			pPackage->readEntry("data", buffer);
			assertTrue(buffer.size() == sData.size());
			assertTrue(std::memcmp(buffer.data(), sData.data(), buffer.size()) == 0);
		}

		void testSHA256MismatchThrows()
		{
			CScopedTempDir tempDir;
			std::string zipPath = joinPath(tempDir.m_sPath, "sha256_mismatch.zip");

			std::string sData = "Hello SHA256";
			std::string sBadSHA256 = "a000000000000000000000000000000000000000000000000000000000000000";

			auto entry = makeEntry("Data", "data.bin", "bin", "application/octet-stream", sData);
			entry.m_sSHA256 = sBadSHA256;
			std::vector<CPackageEntrySpec> entries = { entry };
			std::string packageXML = buildPackageXML(m_sSchemaNamespace, entries);

			writeZIPFile(zipPath, packageXML, entries, true);
			auto pPackage = loadPackageFromZip(zipPath);

			std::vector<uint8_t> buffer;
			bool thrown = false;
			try {
				pPackage->readEntry("data", buffer);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected SHA256 mismatch to throw on readEntry");
		}

		void testSHA256AbsentStillWorks()
		{
			CScopedTempDir tempDir;
			std::string zipPath = joinPath(tempDir.m_sPath, "sha256_absent.zip");

			auto entry = makeEntry("Data", "data.bin", "bin", "application/octet-stream", "BackwardsCompat");
			std::vector<CPackageEntrySpec> entries = { entry };
			std::string packageXML = buildPackageXML(m_sSchemaNamespace, entries);

			writeZIPFile(zipPath, packageXML, entries, true);
			auto pPackage = loadPackageFromZip(zipPath);

			std::vector<uint8_t> buffer;
			pPackage->readEntry("data", buffer);
			std::string sResult(buffer.begin(), buffer.end());
			assertTrue(sResult == "BackwardsCompat");
		}
	};

}

#endif //__AMCTEST_UNITTEST_RESOURCEPACKAGE
