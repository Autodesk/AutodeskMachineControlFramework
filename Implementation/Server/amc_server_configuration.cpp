/*++

Copyright (C) 2022 Autodesk Inc.

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

#include "amc_server_configuration.hpp"
#include "libmc_dynamic.hpp"
#include "common_utils.hpp"
#include "common_importstream_native.hpp"

#include <iostream>
#include <pugixml.hpp>

using namespace AMC;

#define XMLNS_SERVERCONFIG "http://schemas.autodesk.com/amc/2020/06"
#define XMLNS_PACKAGECONFIG "http://schemas.autodesk.com/amcpackage/2020/06"

CServerLibrary::CServerLibrary(const std::string& sLibraryPath, const std::string& sResourcePath)
	: m_sLibraryPath (sLibraryPath), m_sResourcePath (sResourcePath)
{

}

CServerLibrary::~CServerLibrary()
{

}

std::string CServerLibrary::getLibraryPath()
{
	return m_sLibraryPath;
}

std::string CServerLibrary::getResourcePath()
{
	return m_sResourcePath;
}

CServerConfiguration::CServerConfiguration(const std::string& configurationXMLString)
	: m_nPort(0), m_DataBaseType(LibMCData::eDataBaseType::Unknown)
{
	pugi::xml_document xmlDoc;
	pugi::xml_parse_result result = xmlDoc.load_string(configurationXMLString.c_str());
	if (!result)
		throw LibMC::ELibMCException(LIBMC_ERROR_COULDNOTPARSESERVERCONFIGURATION, "Could not parse server configuration");


	auto amcNode = xmlDoc.child("amc");
	if (amcNode.empty())
		throw LibMC::ELibMCException(LIBMC_ERROR_MISSINGSERVERCONFIGURATIONNODE, "Missing server configuration node");

	auto xmlnsAttrib = amcNode.attribute("xmlns");
	if (xmlnsAttrib.empty())
		throw LibMC::ELibMCException(LIBMC_ERROR_MISSINGXMLSCHEMA, "Missing XML Schema");

	std::string xmlns(xmlnsAttrib.as_string());
	if (xmlns != XMLNS_SERVERCONFIG)
		throw LibMC::ELibMCException(LIBMC_ERROR_INVALIDXMLSCHEMA, "Invalid XML Schema: " + xmlns);


	auto serverNode = amcNode.child("server");
	if (serverNode.empty ())
		throw LibMC::ELibMCException(LIBMC_ERROR_MISSINGSERVERNODE, "Missing server node");

	auto hostnameAttrib = serverNode.attribute("hostname");
	m_sHostName = hostnameAttrib.as_string();
	if (m_sHostName.empty())
		throw LibMC::ELibMCException(LIBMC_ERROR_MISSINGHOSTNAME, "Missing host name");


	auto portAttrib = serverNode.attribute("port");
	if (portAttrib.empty())
		throw LibMC::ELibMCException(LIBMC_ERROR_MISSINGPORT, "Missing port");
	m_nPort = portAttrib.as_uint(0);
	if ((m_nPort < 0x0400) || (m_nPort > 0xBFFF)) 
		throw LibMC::ELibMCException(LIBMC_ERROR_INVALIDPORT, "Invalid port: " + std::string (portAttrib.as_string ()));



	auto dataNode = amcNode.child("data");
	if (dataNode.empty())
		throw LibMC::ELibMCException(LIBMC_ERROR_MISSINGDATANODE, "Missing data node");

	auto directoryAttrib = dataNode.attribute("directory");
	std::string sDataDirectory = directoryAttrib.as_string();
	if (sDataDirectory.empty ())
		throw LibMC::ELibMCException(LIBMC_ERROR_MISSINGDATADIRECTORY, "Missing data directory");
	m_sDataDirectory = AMCCommon::CUtils::getFullPathName (sDataDirectory, true);

	if (!AMCCommon::CUtils::fileOrPathExistsOnDisk(m_sDataDirectory)) {
		throw LibMC::ELibMCException(LIBMC_ERROR_DATADIRECTORYDOESNOTEXISTS, "Data directory does not exist: " + m_sDataDirectory);
	}

	if (!AMCCommon::CUtils::pathIsDirectory (m_sDataDirectory))
		throw LibMC::ELibMCException(LIBMC_ERROR_INVALIDDATADIRECTORY, "Invalid data directory: " + m_sDataDirectory);

	auto dataBaseTypeAttrib = dataNode.attribute("database");
	std::string sDataBaseType = dataBaseTypeAttrib.as_string();
	if (sDataBaseType == "sqlite") {

		auto sqLiteDBAttrib = dataNode.attribute("sqlitedb");
		std::string sSQLiteDB = sqLiteDBAttrib.as_string();
		if (sSQLiteDB.empty ())
			throw LibMC::ELibMCException(LIBMC_ERROR_INVALIDSQLITEDBPATH, "Missing SQLite DB Path");

		m_DataBaseType = LibMCData::eDataBaseType::SqLite;
		m_sConnectionString = AMCCommon::CUtils::getFullPathName (AMCCommon::CUtils::includeTrailingPathDelimiter(m_sDataDirectory) + sSQLiteDB, false);

	}
	else {

		throw LibMC::ELibMCException(LIBMC_ERROR_INVALIDDATABASETYPE, "Invalid database type: " + sDataBaseType);
	}

	auto defaultPackageNode = amcNode.child("defaultpackage");
	if (defaultPackageNode.empty ())
		throw LibMC::ELibMCException(LIBMC_ERROR_DEFAULTPACKAGEMISSING, "Default package missing");

	auto defaultPackageNameAttrib = defaultPackageNode.attribute("name");
	std::string sDefaultPackageName = defaultPackageNameAttrib.as_string ();
	if (sDefaultPackageName.empty())
		throw LibMC::ELibMCException(LIBMC_ERROR_DEFAULTPACKAGENAMEMISSING, "Default package name missing");

	loadPackageXML(AMCCommon::CUtils::getFullPathName (sDefaultPackageName, true));
}


CServerConfiguration::~CServerConfiguration()
{

}

std::string CServerConfiguration::getHostName()
{
	return m_sHostName;
}

uint32_t CServerConfiguration::getPort()
{
	return m_nPort;
}

std::string CServerConfiguration::getDataDirectory()
{
	return m_sDataDirectory;
}

LibMCData::eDataBaseType CServerConfiguration::getDataBaseType()
{
	return m_DataBaseType;
}

std::string CServerConfiguration::getConnectionString()
{
	return m_sConnectionString;
}

std::string CServerConfiguration::getPackageName()
{
	return m_sPackageName;
}

std::string CServerConfiguration::getPackageCoreClient()
{
	return m_sPackageCoreClient;
}

std::string CServerConfiguration::getPackageConfig()
{
	return m_sPackageConfig;
}

std::string CServerConfiguration::getLibraryPath(const std::string& sLibraryName)
{
	auto iIter = m_Libraries.find(sLibraryName);
	if (iIter == m_Libraries.end())
		throw LibMC::ELibMCException(LIBMC_ERROR_COULDNOTFINDLIBRARYENTRY, "Could not find library entry: " + sLibraryName);

	return iIter->second->getLibraryPath();
}
	
std::string CServerConfiguration::getResourcePath(const std::string& sLibraryName)
{
	auto iIter = m_Libraries.find(sLibraryName);
	if (iIter == m_Libraries.end())
		throw LibMC::ELibMCException(LIBMC_ERROR_COULDNOTFINDLIBRARYENTRY, "Could not find library entry: " + sLibraryName);

	return iIter->second->getResourcePath();
}

std::set<std::string> CServerConfiguration::getLibraryNames()
{
	std::set<std::string> libraryNames;
	for (auto iIter : m_Libraries) {
		libraryNames.insert(iIter.first);
	}

	return libraryNames;
}


void CServerConfiguration::loadPackageXML(const std::string sPackageFileName)
{
	AMCCommon::CImportStream_Native importStream(sPackageFileName);
	std::string packageXMLString = importStream.readAsString();

	pugi::xml_document xmlDoc;
	pugi::xml_parse_result result = xmlDoc.load_string(packageXMLString.c_str());
	if (!result)
		throw LibMC::ELibMCException(LIBMC_ERROR_COULDNOTPARSEPACKAGEXML, "Could not parse package xml");


	auto packageNode = xmlDoc.child("amcpackage");
	if (packageNode.empty())
		throw LibMC::ELibMCException(LIBMC_ERROR_MISSINGPACKAGENODE, "Missing package node");

	auto xmlnsAttrib = packageNode.attribute("xmlns");
	if (xmlnsAttrib.empty())
		throw LibMC::ELibMCException(LIBMC_ERROR_MISSINGXMLSCHEMA, "Missing XML Schema: " + sPackageFileName);

	std::string xmlns(xmlnsAttrib.as_string());
	if (xmlns != XMLNS_PACKAGECONFIG)
		throw LibMC::ELibMCException(LIBMC_ERROR_INVALIDXMLSCHEMA, "Invalid XML Schema: " + xmlns);

	auto buildNode = packageNode.child("build");
	if (buildNode.empty())
		throw LibMC::ELibMCException(LIBMC_ERROR_MISSINGBUILDNODE, "Missing build node");

	std::string sName = buildNode.attribute("name").as_string ();
	if (sName.empty ())
		throw LibMC::ELibMCException(LIBMC_ERROR_MISSINGBUILDNAME, "Missing build name");

	std::string sConfigurationName = buildNode.attribute("configuration").as_string();
	if (sConfigurationName.empty())
		throw LibMC::ELibMCException(LIBMC_ERROR_MISSINGCONFIGURATIONNAME, "Missing configuration name");

	std::string sCoreClient = buildNode.attribute("coreclient").as_string();
	if (sCoreClient.empty())
		throw LibMC::ELibMCException(LIBMC_ERROR_MISSINGCORECLIENT, "Missing core client");

	m_sPackageName = sName;
	m_sPackageConfig = AMCCommon::CUtils::getFullPathName(sConfigurationName, true);
	m_sPackageCoreClient = AMCCommon::CUtils::getFullPathName(sCoreClient, true);

	auto libraries = buildNode.children("library");
	for (auto libraryNode : libraries) {

		std::string libraryName = libraryNode.attribute("name").as_string ();
		if (libraryName.empty ())
			throw LibMC::ELibMCException(LIBMC_ERROR_MISSINGLIBRARYNAME, "Missing library name");

		std::string libraryImport = libraryNode.attribute("import").as_string();
		if (libraryImport.empty())
			throw LibMC::ELibMCException(LIBMC_ERROR_MISSINGLIBRARYIMPORTNAME, "Missing library import name: " + libraryName);
		std::string libraryImportPath = AMCCommon::CUtils::getFullPathName(libraryImport, true);

		std::string libraryResources = libraryNode.attribute("resources").as_string();
		std::string libraryResourcePath;
		if (!libraryResources.empty())
			libraryResourcePath = AMCCommon::CUtils::getFullPathName(libraryResources, true);

		if (m_Libraries.find (libraryName) != m_Libraries.end ())
			throw LibMC::ELibMCException(LIBMC_ERROR_DUPLICATELIBRARYNAME, "Duplicate library name: " + libraryName);

		m_Libraries.insert(std::make_pair (libraryName, std::make_shared<CServerLibrary>(libraryImportPath, libraryResourcePath)));
	}

}
