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

#ifndef __AMCTEST_UNITTEST_LIBMCDATA_MACHINECONFIG
#define __AMCTEST_UNITTEST_LIBMCDATA_MACHINECONFIG

#include "amc_unittests.hpp"
#include "amc_unittests_libmcdata.hpp"
#include "common_utils.hpp"
#include "libmcdata_dynamic.hpp"

#include <cstdint>
#include <string>

namespace AMCUnitTest {

	class CUnitTestGroup_LibMCData_MachineConfig : public CUnitTestGroup {
	public:
		std::string getTestGroupName() override {
			return "LibMCData_MachineConfig";
		}

		void registerTests() override {
			// MachineConfigurationType tests (15 tests)
			registerTest("CreateType", "Create configuration type", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_MachineConfig::testCreateType, this));
			registerTest("FindBySchema", "Find configuration type by schema", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_MachineConfig::testFindBySchema, this));
			registerTest("FindByUUID", "Find configuration type by UUID", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_MachineConfig::testFindByUUID, this));
			registerTest("ListRegistered", "List registered configuration types", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_MachineConfig::testListRegistered, this));
			registerTest("GetTypeName", "Get configuration type name", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_MachineConfig::testGetTypeName, this));
			registerTest("GetSchemaType", "Get schema type", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_MachineConfig::testGetSchemaType, this));
			registerTest("GetTypeTimestamp", "Get type timestamp", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_MachineConfig::testGetTypeTimestamp, this));
			registerTest("RegisterNewXSD", "Register new XSD version", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_MachineConfig::testRegisterNewXSD, this));
			registerTest("GetLatestXSD", "Get latest XSD version", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_MachineConfig::testGetLatestXSD, this));
			registerTest("ListXSDVersions", "List XSD versions", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_MachineConfig::testListXSDVersions, this));
			registerTest("FindXSDByUUID", "Find XSD by UUID", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_MachineConfig::testFindXSDByUUID, this));
			registerTest("FindXSDByVersion", "Find XSD by numeric version", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_MachineConfig::testFindXSDByVersion, this));
			registerTest("CreateDefaultConfig", "Create default configuration", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_MachineConfig::testCreateDefaultConfig, this));
			registerTest("GetActiveVersion", "Get active configuration version", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_MachineConfig::testGetActiveVersion, this));
			registerTest("SetActiveVersion", "Set active configuration version", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_MachineConfig::testSetActiveVersion, this));
			
			// MachineConfigurationVersion tests (8 tests)
			registerTest("VersionGetUUID", "Get version UUID", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_MachineConfig::testVersionGetUUID, this));
			registerTest("VersionGetXSDUUID", "Get version XSD UUID", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_MachineConfig::testVersionGetXSDUUID, this));
			registerTest("VersionGetXMLContent", "Get version XML content", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_MachineConfig::testVersionGetXMLContent, this));
			registerTest("VersionGetTimestamp", "Get version timestamp", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_MachineConfig::testVersionGetTimestamp, this));
			registerTest("ListAllVersions", "List all configuration versions", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_MachineConfig::testListAllVersions, this));
			registerTest("ListVersionsForXSD", "List versions for specific XSD", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_MachineConfig::testListVersionsForXSD, this));
			registerTest("FindVersionByUUID", "Find version by UUID", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_MachineConfig::testFindVersionByUUID, this));
			registerTest("GetLatestVersion", "Get latest configuration version", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_LibMCData_MachineConfig::testGetLatestVersion, this));
		}

		void initializeTests() override {
		}

	private:

		struct SMachineConfigFixture {
			LibMCData::PWrapper m_pDataWrapper;
			LibMCData::PDataModel m_pDataModel;
			std::string m_sBasePath;
		};

		SMachineConfigFixture createFixture(const std::string& sSuffix)
		{
			SMachineConfigFixture fixture;

			std::string sRootPath = "temp";
			if (!AMCCommon::CUtils::fileOrPathExistsOnDisk(sRootPath))
				AMCCommon::CUtils::createDirectoryOnDisk(sRootPath);

			fixture.m_sBasePath = sRootPath + "/libmcdata_machineconfig_" + sSuffix + "_" + AMCCommon::CUtils::createUUID();
			if (!AMCCommon::CUtils::fileOrPathExistsOnDisk(fixture.m_sBasePath))
				AMCCommon::CUtils::createDirectoryOnDisk(fixture.m_sBasePath);

			std::string sDatabaseFile = fixture.m_sBasePath + "/machineconfig.db";

			fixture.m_pDataWrapper = AMCUnitTest::loadLibMCDataInProcess();
			fixture.m_pDataModel = fixture.m_pDataWrapper->CreateDataModelInstance();
			fixture.m_pDataModel->InitialiseDatabase(fixture.m_sBasePath, LibMCData::eDataBaseType::SqLite, sDatabaseFile);

			return fixture;
		}

		std::string getSimpleXSD()
		{
			return R"(<?xml version="1.0" encoding="UTF-8"?>
<xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema">
  <xs:element name="configuration">
    <xs:complexType>
      <xs:sequence>
        <xs:element name="name" type="xs:string"/>
        <xs:element name="value" type="xs:integer"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>
</xs:schema>)";
		}

		std::string getSimpleXML()
		{
			return R"(<?xml version="1.0" encoding="UTF-8"?>
<configuration>
  <name>TestConfiguration</name>
  <value>42</value>
</configuration>)";
		}

		std::string getCurrentTimestampUTC()
		{
			return "2025-01-15T12:00:00Z";
		}

		// ============= MachineConfigurationType Tests =============

		void testCreateType()
		{
			auto fixture = createFixture("create_type");
			
			std::string sSchemaType = "http://test.schema/config/v1";
			std::string sName = "Test Configuration Type";
			
			auto pConfigType = fixture.m_pDataModel->CreateConfigurationType(sSchemaType, sName);
			
			assertAssigned(pConfigType.get(), "Configuration type should be created");
			assertFalse(pConfigType->GetUUID().empty(), "Type UUID should not be empty");
			assertTrue(pConfigType->GetName() == sName, "Type name should match");
			assertTrue(pConfigType->GetSchemaType() == sSchemaType, "Schema type should match");
		}

		void testFindBySchema()
		{
			auto fixture = createFixture("find_schema");
			
			std::string sSchemaType = "http://test.schema/findbyschema/v1";
			std::string sName = "Find By Schema Type";
			
			// Create the type first
			auto pCreated = fixture.m_pDataModel->CreateConfigurationType(sSchemaType, sName);
			std::string sCreatedUUID = pCreated->GetUUID();
			
			// Find by schema
			auto pFound = fixture.m_pDataModel->FindConfigurationTypeBySchema(sSchemaType);
			
			assertAssigned(pFound.get(), "Should find configuration type by schema");
			assertTrue(pFound->GetUUID() == sCreatedUUID, "Found type UUID should match created UUID");
			assertTrue(pFound->GetSchemaType() == sSchemaType, "Schema type should match");
		}

		void testFindByUUID()
		{
			auto fixture = createFixture("find_uuid");
			
			std::string sSchemaType = "http://test.schema/findbyuuid/v1";
			std::string sName = "Find By UUID Type";
			
			// Create the type first
			auto pCreated = fixture.m_pDataModel->CreateConfigurationType(sSchemaType, sName);
			std::string sCreatedUUID = pCreated->GetUUID();
			
			// Find by UUID
			auto pFound = fixture.m_pDataModel->FindConfigurationTypeByUUID(sCreatedUUID);
			
			assertAssigned(pFound.get(), "Should find configuration type by UUID");
			assertTrue(pFound->GetUUID() == sCreatedUUID, "Found type UUID should match");
			assertTrue(pFound->GetSchemaType() == sSchemaType, "Schema type should match");
		}

		void testListRegistered()
		{
			auto fixture = createFixture("list_registered");
			
			// Create multiple types
			fixture.m_pDataModel->CreateConfigurationType("http://test.schema/list1/v1", "Type 1");
			fixture.m_pDataModel->CreateConfigurationType("http://test.schema/list2/v1", "Type 2");
			fixture.m_pDataModel->CreateConfigurationType("http://test.schema/list3/v1", "Type 3");
			
			auto pIterator = fixture.m_pDataModel->ListRegisteredConfigurationTypes();
			
			assertAssigned(pIterator.get(), "Iterator should be returned");
			assertTrue(pIterator->Count() >= 3, "Should have at least 3 registered types");
		}

		void testGetTypeName()
		{
			auto fixture = createFixture("type_name");
			
			std::string sSchemaType = "http://test.schema/typename/v1";
			std::string sName = "My Configuration Type Name";
			
			auto pConfigType = fixture.m_pDataModel->CreateConfigurationType(sSchemaType, sName);
			
			assertTrue(pConfigType->GetName() == sName, "Type name should match");
		}

		void testGetSchemaType()
		{
			auto fixture = createFixture("schema_type");
			
			std::string sSchemaType = "http://test.schema/schematype/v1";
			std::string sName = "Schema Type Test";
			
			auto pConfigType = fixture.m_pDataModel->CreateConfigurationType(sSchemaType, sName);
			
			assertTrue(pConfigType->GetSchemaType() == sSchemaType, "Schema type should match");
		}

		void testGetTypeTimestamp()
		{
			auto fixture = createFixture("type_timestamp");
			
			auto pConfigType = fixture.m_pDataModel->CreateConfigurationType("http://test.schema/timestamp/v1", "Timestamp Test");
			
			std::string sTimestamp = pConfigType->GetTimestamp();
			assertFalse(sTimestamp.empty(), "Timestamp should not be empty");
		}

		void testRegisterNewXSD()
		{
			auto fixture = createFixture("register_xsd");
			
			auto pConfigType = fixture.m_pDataModel->CreateConfigurationType("http://test.schema/registerxsd/v1", "XSD Test");
			
			std::string sXSDContent = getSimpleXSD();
			uint32_t nXSDVersion = 1;
			
			auto pXSD = pConfigType->RegisterNewXSD(sXSDContent, nXSDVersion);
			
			assertAssigned(pXSD.get(), "XSD should be registered");
			assertFalse(pXSD->GetUUID().empty(), "XSD UUID should not be empty");
		}

		void testGetLatestXSD()
		{
			auto fixture = createFixture("latest_xsd");
			
			auto pConfigType = fixture.m_pDataModel->CreateConfigurationType("http://test.schema/latestxsd/v1", "Latest XSD Test");
			
			// Register multiple XSD versions
			pConfigType->RegisterNewXSD(getSimpleXSD(), 1);
			pConfigType->RegisterNewXSD(getSimpleXSD(), 2);
			auto pXSD3 = pConfigType->RegisterNewXSD(getSimpleXSD(), 3);
			
			auto pLatest = pConfigType->GetLatestXSD();
			
			assertAssigned(pLatest.get(), "Latest XSD should be returned");
			assertTrue(pLatest->GetUUID() == pXSD3->GetUUID(), "Latest XSD should be version 3");
		}

		void testListXSDVersions()
		{
			auto fixture = createFixture("list_xsd");
			
			auto pConfigType = fixture.m_pDataModel->CreateConfigurationType("http://test.schema/listxsd/v1", "List XSD Test");
			
			pConfigType->RegisterNewXSD(getSimpleXSD(), 1);
			pConfigType->RegisterNewXSD(getSimpleXSD(), 2);
			pConfigType->RegisterNewXSD(getSimpleXSD(), 3);
			
			auto pIterator = pConfigType->ListXSDVersions();
			
			assertAssigned(pIterator.get(), "XSD iterator should be returned");
			assertTrue(pIterator->Count() == 3, "Should have 3 XSD versions");
		}

		void testFindXSDByUUID()
		{
			auto fixture = createFixture("find_xsd_uuid");
			
			auto pConfigType = fixture.m_pDataModel->CreateConfigurationType("http://test.schema/findxsduuid/v1", "Find XSD UUID Test");
			
			auto pXSD = pConfigType->RegisterNewXSD(getSimpleXSD(), 1);
			std::string sXSDUUID = pXSD->GetUUID();
			
			auto pFound = pConfigType->FindXSDByUUID(sXSDUUID);
			
			assertAssigned(pFound.get(), "XSD should be found by UUID");
			assertTrue(pFound->GetUUID() == sXSDUUID, "Found XSD UUID should match");
		}

		void testFindXSDByVersion()
		{
			auto fixture = createFixture("find_xsd_version");
			
			auto pConfigType = fixture.m_pDataModel->CreateConfigurationType("http://test.schema/findxsdver/v1", "Find XSD Version Test");
			
			pConfigType->RegisterNewXSD(getSimpleXSD(), 1);
			auto pXSD2 = pConfigType->RegisterNewXSD(getSimpleXSD(), 2);
			pConfigType->RegisterNewXSD(getSimpleXSD(), 3);
			
			auto pFound = pConfigType->FindXSDByNumericVersion(2);
			
			assertAssigned(pFound.get(), "XSD should be found by version");
			assertTrue(pFound->GetUUID() == pXSD2->GetUUID(), "Found XSD should be version 2");
		}

		void testCreateDefaultConfig()
		{
			auto fixture = createFixture("create_default");
			
			auto pConfigType = fixture.m_pDataModel->CreateConfigurationType("http://test.schema/createdefault/v1", "Create Default Test");
			
			auto pXSD = pConfigType->RegisterNewXSD(getSimpleXSD(), 1);
			std::string sXSDUUID = pXSD->GetUUID();
			
			auto pVersion = pConfigType->CreateDefaultConfiguration(sXSDUUID, getSimpleXML(), getCurrentTimestampUTC());
			
			assertAssigned(pVersion.get(), "Configuration version should be created");
			assertFalse(pVersion->GetVersionUUID().empty(), "Version UUID should not be empty");
		}

		void testGetActiveVersion()
		{
			auto fixture = createFixture("get_active");
			
			auto pConfigType = fixture.m_pDataModel->CreateConfigurationType("http://test.schema/getactive/v1", "Get Active Test");
			
			auto pXSD = pConfigType->RegisterNewXSD(getSimpleXSD(), 1);
			auto pVersion = pConfigType->CreateDefaultConfiguration(pXSD->GetUUID(), getSimpleXML(), getCurrentTimestampUTC());
			
			// Set as active
			pConfigType->SetActiveConfigurationVersion(pVersion->GetVersionUUID());
			
			auto pActive = pConfigType->GetActiveConfigurationVersion();
			
			assertAssigned(pActive.get(), "Active version should be returned");
			assertTrue(pActive->GetVersionUUID() == pVersion->GetVersionUUID(), "Active version should match set version");
		}

		void testSetActiveVersion()
		{
			auto fixture = createFixture("set_active");
			
			auto pConfigType = fixture.m_pDataModel->CreateConfigurationType("http://test.schema/setactive/v1", "Set Active Test");
			
			auto pXSD = pConfigType->RegisterNewXSD(getSimpleXSD(), 1);
			auto pVersion1 = pConfigType->CreateDefaultConfiguration(pXSD->GetUUID(), getSimpleXML(), getCurrentTimestampUTC());
			auto pVersion2 = pConfigType->CreateDefaultConfiguration(pXSD->GetUUID(), getSimpleXML(), getCurrentTimestampUTC());
			
			// Set first as active
			pConfigType->SetActiveConfigurationVersion(pVersion1->GetVersionUUID());
			auto pActive1 = pConfigType->GetActiveConfigurationVersion();
			assertTrue(pActive1->GetVersionUUID() == pVersion1->GetVersionUUID(), "Active should be version 1");
			
			// Change to second
			pConfigType->SetActiveConfigurationVersion(pVersion2->GetVersionUUID());
			auto pActive2 = pConfigType->GetActiveConfigurationVersion();
			assertTrue(pActive2->GetVersionUUID() == pVersion2->GetVersionUUID(), "Active should be version 2");
		}

		// ============= MachineConfigurationVersion Tests =============

		void testVersionGetUUID()
		{
			auto fixture = createFixture("ver_uuid");
			
			auto pConfigType = fixture.m_pDataModel->CreateConfigurationType("http://test.schema/veruuid/v1", "Version UUID Test");
			auto pXSD = pConfigType->RegisterNewXSD(getSimpleXSD(), 1);
			auto pVersion = pConfigType->CreateDefaultConfiguration(pXSD->GetUUID(), getSimpleXML(), getCurrentTimestampUTC());
			
			std::string sUUID = pVersion->GetVersionUUID();
			
			assertFalse(sUUID.empty(), "Version UUID should not be empty");
			assertTrue(sUUID.length() == 36, "Version UUID should be 36 characters");
		}

		void testVersionGetXSDUUID()
		{
			auto fixture = createFixture("ver_xsd_uuid");
			
			auto pConfigType = fixture.m_pDataModel->CreateConfigurationType("http://test.schema/verxsduuid/v1", "Version XSD UUID Test");
			auto pXSD = pConfigType->RegisterNewXSD(getSimpleXSD(), 1);
			std::string sXSDUUID = pXSD->GetUUID();
			
			auto pVersion = pConfigType->CreateDefaultConfiguration(sXSDUUID, getSimpleXML(), getCurrentTimestampUTC());
			
			std::string sVersionXSDUUID = pVersion->GetXSDUUID();
			assertTrue(sVersionXSDUUID == sXSDUUID, "Version XSD UUID should match");
		}

		void testVersionGetXMLContent()
		{
			auto fixture = createFixture("ver_xml");
			
			auto pConfigType = fixture.m_pDataModel->CreateConfigurationType("http://test.schema/verxml/v1", "Version XML Test");
			auto pXSD = pConfigType->RegisterNewXSD(getSimpleXSD(), 1);
			
			std::string sXMLContent = getSimpleXML();
			auto pVersion = pConfigType->CreateDefaultConfiguration(pXSD->GetUUID(), sXMLContent, getCurrentTimestampUTC());
			
			std::string sRetrievedXML = pVersion->GetConfigurationXMLString();
			
			assertFalse(sRetrievedXML.empty(), "XML content should not be empty");
			assertTrue(sRetrievedXML == sXMLContent, "XML content should match");
		}

		void testVersionGetTimestamp()
		{
			auto fixture = createFixture("ver_timestamp");
			
			auto pConfigType = fixture.m_pDataModel->CreateConfigurationType("http://test.schema/vertimestamp/v1", "Version Timestamp Test");
			auto pXSD = pConfigType->RegisterNewXSD(getSimpleXSD(), 1);
			
			std::string sTimestamp = getCurrentTimestampUTC();
			auto pVersion = pConfigType->CreateDefaultConfiguration(pXSD->GetUUID(), getSimpleXML(), sTimestamp);
			
			std::string sRetrievedTimestamp = pVersion->GetTimestamp();
			
			assertFalse(sRetrievedTimestamp.empty(), "Timestamp should not be empty");
			assertTrue(sRetrievedTimestamp == sTimestamp, "Timestamp should match");
		}

		void testListAllVersions()
		{
			auto fixture = createFixture("list_all_ver");
			
			auto pConfigType = fixture.m_pDataModel->CreateConfigurationType("http://test.schema/listallver/v1", "List All Versions Test");
			auto pXSD = pConfigType->RegisterNewXSD(getSimpleXSD(), 1);
			
			// Create multiple versions
			pConfigType->CreateDefaultConfiguration(pXSD->GetUUID(), getSimpleXML(), "2025-01-01T12:00:00Z");
			pConfigType->CreateDefaultConfiguration(pXSD->GetUUID(), getSimpleXML(), "2025-01-02T12:00:00Z");
			pConfigType->CreateDefaultConfiguration(pXSD->GetUUID(), getSimpleXML(), "2025-01-03T12:00:00Z");
			
			auto pIterator = pConfigType->ListAllConfigurationVersions();
			
			assertAssigned(pIterator.get(), "Version iterator should be returned");
			assertTrue(pIterator->Count() == 3, "Should have 3 versions");
		}

		void testListVersionsForXSD()
		{
			auto fixture = createFixture("list_ver_xsd");
			
			auto pConfigType = fixture.m_pDataModel->CreateConfigurationType("http://test.schema/listverxsd/v1", "List Versions For XSD Test");
			auto pXSD1 = pConfigType->RegisterNewXSD(getSimpleXSD(), 1);
			auto pXSD2 = pConfigType->RegisterNewXSD(getSimpleXSD(), 2);
			
			// Create versions for XSD1
			pConfigType->CreateDefaultConfiguration(pXSD1->GetUUID(), getSimpleXML(), "2025-01-01T12:00:00Z");
			pConfigType->CreateDefaultConfiguration(pXSD1->GetUUID(), getSimpleXML(), "2025-01-02T12:00:00Z");
			
			// Create versions for XSD2
			pConfigType->CreateDefaultConfiguration(pXSD2->GetUUID(), getSimpleXML(), "2025-01-03T12:00:00Z");
			
			auto pIterator1 = pConfigType->ListConfigurationVersionsForXSD(pXSD1->GetUUID());
			auto pIterator2 = pConfigType->ListConfigurationVersionsForXSD(pXSD2->GetUUID());
			
			assertTrue(pIterator1->Count() == 2, "XSD1 should have 2 versions");
			assertTrue(pIterator2->Count() == 1, "XSD2 should have 1 version");
		}

		void testFindVersionByUUID()
		{
			auto fixture = createFixture("find_ver_uuid");
			
			auto pConfigType = fixture.m_pDataModel->CreateConfigurationType("http://test.schema/findveruuid/v1", "Find Version UUID Test");
			auto pXSD = pConfigType->RegisterNewXSD(getSimpleXSD(), 1);
			auto pVersion = pConfigType->CreateDefaultConfiguration(pXSD->GetUUID(), getSimpleXML(), getCurrentTimestampUTC());
			
			std::string sVersionUUID = pVersion->GetVersionUUID();
			
			auto pFound = pConfigType->FindConfigurationVersionByUUID(sVersionUUID);
			
			assertAssigned(pFound.get(), "Version should be found by UUID");
			assertTrue(pFound->GetVersionUUID() == sVersionUUID, "Found version UUID should match");
		}

		void testGetLatestVersion()
		{
			auto fixture = createFixture("latest_ver");
			
			auto pConfigType = fixture.m_pDataModel->CreateConfigurationType("http://test.schema/latestver/v1", "Latest Version Test");
			auto pXSD = pConfigType->RegisterNewXSD(getSimpleXSD(), 1);
			
			// Create a single version and verify we can get it as "latest"
			auto pVersion = pConfigType->CreateDefaultConfiguration(pXSD->GetUUID(), getSimpleXML(), getCurrentTimestampUTC());
			
			auto pLatest = pConfigType->GetLatestConfigurationVersion();
			
			assertAssigned(pLatest.get(), "Latest version should be returned");
			assertFalse(pLatest->GetVersionUUID().empty(), "Latest version should have UUID");
			
			// Check we can list all versions
			auto pIterator = pConfigType->ListAllConfigurationVersions();
			assertTrue(pIterator->Count() >= 1, "Should have at least 1 version");
		}
	};

}

#endif // __AMCTEST_UNITTEST_LIBMCDATA_MACHINECONFIG
