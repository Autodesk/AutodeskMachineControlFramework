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

#include <iostream>
#include <pugixml.hpp>

using namespace AMC;

#define XMLNS_SERVERCONFIG "http://schemas.autodesk.com/amc/2020/06"
#define XMLNS_PACKAGECONFIG "http://schemas.autodesk.com/amc/2020/06"

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
