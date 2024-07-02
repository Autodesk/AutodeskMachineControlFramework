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
#include <sstream>
#include <vector>

#include "PugiXML/pugixml.hpp"
#include "common_utils.hpp"
#include "common_importstream_native.hpp"
#include "common_exportstream_native.hpp"

void writeUsage()
{
	std::cout << "Usage: create_package_xml --option1 <option1> --option2 <option2>" << std::endl;
	std::cout << "Options are:" << std::endl;
	std::cout << "   --config <configfile>: Path to ACMF Configuration XML to use. (mandatory)" << std::endl;
	std::cout << "   --devpackage <githash>: Short git hash of developer package to use. (mandatory)" << std::endl;
	std::cout << "   --output: Output package xml file to write. (default: GITHASH_package.xml)" << std::endl;
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
		std::cout << "Autodesk Machine Control Framework - Package builder" << std::endl;
		std::cout << "---------------------------------------------------------------------" << std::endl;

		// Parse arguments
		std::vector<std::string> argumentList;
#ifdef _MSC_VER
		// Convert wide strings to UTF8 on Windows
		for (int argIdx = 1; argIdx < argc; argIdx++) {
			std::wstring wArg(argv[argIdx]);
			argumentList.push_back (AMCCommon::CUtils::UTF16toUTF8 (wArg));
		}
#else
		for (int argIdx = 1; argIdx < argc; argIdx++)
			argumentList.push_back(std::string (argv[argIdx]));
#endif

		std::string sDevPackagePrefix = "";
		std::string sConfigFileName = "";
		std::string sOutputFileName = "";
		std::string sServerOutputFileName = "amc_server.xml";
		for (size_t argIdx = 0; argIdx < argumentList.size(); argIdx++) {

			bool bHandled = false;

			std::string sArgument = argumentList[argIdx];
			if (sArgument == "--config") {
				argIdx++;
				if (argIdx >= argumentList.size())
					throw std::runtime_error("missing config file name in argument");

				sConfigFileName = argumentList[argIdx];
				bHandled = true;
			} 
			
			if (sArgument == "--devpackage") {
				argIdx++;
				if (argIdx >= argumentList.size()) 
					throw std::runtime_error("missing dev package prefix in argument");

				sDevPackagePrefix = argumentList[argIdx];
				bHandled = true;
			}

			if (sArgument == "--output") {
				argIdx++;
				if (argIdx >= argumentList.size())
					throw std::runtime_error("missing output file name in argument");

				sOutputFileName = argumentList[argIdx];
				bHandled = true;
			} 

			if (sArgument == "--serveroutput") {
				argIdx++;
				if (argIdx >= argumentList.size())
					throw std::runtime_error("missing server output file name in argument");

				sServerOutputFileName = argumentList[argIdx];
				bHandled = true;
			} 

			if (!bHandled) {
				std::cout << "Warning! Unknown argument passed: " << sArgument << std::endl;
			}

		}

		if (sConfigFileName.empty()) {
			writeUsage();
			throw std::runtime_error("No config file given...");
		}
		if (sDevPackagePrefix.empty()) {
			writeUsage();
			throw std::runtime_error("No dev package prefix given...");
		}

		if (sOutputFileName.empty()) {
			sOutputFileName = sDevPackagePrefix + "_package.xml";
		}

		std::cout << "Importing " << sConfigFileName << "..." << std::endl;
		std::string sConfigAsString;
		{
			AMCCommon::CImportStream_Native configStream(sConfigFileName);
			sConfigAsString = configStream.readAsString();
			if (sConfigAsString.empty ())
				throw std::runtime_error("configuration file is empty!");
		}

		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_string(sConfigAsString.c_str());
		if (!result)
			throw std::runtime_error("could not parse configuration file!");

		std::set<std::string> drivers;

		auto mainNode = doc.child("machinedefinition");
		auto driversNodes = mainNode.children("driver");
		for (pugi::xml_node driversNode : driversNodes)
		{
			auto nameAttrib = driversNode.attribute("name");
			if (nameAttrib.empty())
				throw std::runtime_error("missing driver name!");
			std::string sName(nameAttrib.as_string());


			auto typeAttrib = driversNode.attribute("type");
			if (nameAttrib.empty())
				throw std::runtime_error("missing driver type!");
			std::string sType(typeAttrib.as_string());

			auto libraryAttrib = driversNode.attribute("library");
			if (libraryAttrib.empty())
				throw std::runtime_error("missing driver library!");
			std::string sLibraryName(libraryAttrib.as_string());

			std::cout << "   - using driver " << sName << " with library " << sLibraryName << std::endl;
			drivers.insert (sLibraryName);

			if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString (sName))
				throw std::runtime_error("invalid driver name: " + sName);
			if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sLibraryName))
				throw std::runtime_error("invalid driver library: " + sLibraryName);

		}

		std::set<std::string> plugins;

		auto statemachinesNodes = mainNode.children("statemachine");
		for (pugi::xml_node instanceNode : statemachinesNodes)
		{
			auto nameAttrib = instanceNode.attribute("name");
			if (nameAttrib.empty())
				throw std::runtime_error("missing statemachine name!");
			std::string sName(nameAttrib.as_string());

			auto libraryAttrib = instanceNode.attribute("library");
			if (libraryAttrib.empty())
				throw std::runtime_error("missing statemachine library!");
			std::string sLibraryName(libraryAttrib.as_string());
			if (sLibraryName.length() == 0)
				throw std::runtime_error("empty statemachine library!");

			std::cout << "   - using plugin " << sName << " with library " << sLibraryName << std::endl;
			plugins.insert (sLibraryName);

			if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sName))
				throw std::runtime_error("invalid plugin name: " + sName);
			if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sLibraryName))
				throw std::runtime_error("invalid plugin library: " + sLibraryName);

		}

		auto userInterfaceNode = mainNode.child("userinterface");
		if (userInterfaceNode.empty()) 
			throw std::runtime_error("missing user interface node");

		// Load user interface
		auto uiLibraryAttrib = userInterfaceNode.attribute("library");
		if (uiLibraryAttrib.empty())
			throw std::runtime_error("missing user interface library");
		std::string sUILibrary(uiLibraryAttrib.as_string ());

		if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sUILibrary))
			throw std::runtime_error("invalid user interface library: " + sUILibrary);

		plugins.insert (sUILibrary);

		std::cout << std::endl;
		std::cout << "Creating Package XML for project..." << std::endl;

#ifdef _WIN32
		std::string sExtension = "dll";
#else
		std::string sExtension = "so";
#endif
		std::string sPackageName = "Build " + sDevPackagePrefix;
		std::string sConfigName = sDevPackagePrefix + "_config.xml";
		std::string sClientName = sDevPackagePrefix + "_core.client";
		std::string sCoreName = sDevPackagePrefix + "_core_libmc." + sExtension;
		std::string sCoreResourcesName = sDevPackagePrefix + "_core.data";
		std::string sCoreDataName = sDevPackagePrefix + "_core_libmcdata." + sExtension;
		std::string sLib3MFName = sDevPackagePrefix + "_core_lib3mf." + sExtension;

		std::stringstream packageXMLStream;

		packageXMLStream << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";
		packageXMLStream << "<amcpackage xmlns=\"http://schemas.autodesk.com/amcpackage/2020/06\">\n";
		packageXMLStream << "  <build name=\"" << sPackageName << "\" configuration=\"" << sConfigName << "\" coreclient=\"" << sClientName << "\">\n";

		packageXMLStream << "    <library name=\"core\" import=\"" << sCoreName << "\" resources=\"" << sCoreResourcesName << "\" />\n";
		packageXMLStream << "    <library name=\"datamodel\" import=\"" << sCoreDataName << "\" />\n";
		packageXMLStream << "    <library name=\"lib3mf\" import=\"" << sLib3MFName << "\" />\n";

		for (auto sLibraryName : drivers) {
			packageXMLStream << "    <library name=\"" << sLibraryName << "\" import=\"" << sDevPackagePrefix << "_" << sLibraryName << "." << sExtension << "\" resources=\"" << sDevPackagePrefix << "_" << sLibraryName << ".data\" />\n";
		}

		for (auto sLibraryName : plugins) {
			packageXMLStream << "    <library name=\"" << sLibraryName << "\" import=\"" << sDevPackagePrefix << "_" << sLibraryName << "." << sExtension << "\"  />\n";
		}

		//fmt.Fprintf(pkgfile, "    <library name=\"plugin_main\" import=\"%s_plugin_main.%s\" />\n", gitHash, dllExtension);
		//fmt.Fprintf(pkgfile, "    <library name=\"driver_rasterizer\" import=\"%s_driver_rasterizer.%s\" resources=\"%s_driver_rasterizer.data\"  />\n", gitHash, dllExtension, gitHash);

		packageXMLStream << "  </build>\n";
		packageXMLStream << "</amcpackage>\n";

		std::string packageXMLString = packageXMLStream.str();

		std::cout << std::endl;
		std::cout << packageXMLString;
		std::cout << std::endl;
		
		std::string sPackageSHA = AMCCommon::CUtils::calculateSHA256FromString(packageXMLString);
		
		std::stringstream serverXMLStream;
		serverXMLStream << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";
		serverXMLStream << "<amc xmlns=\"http://schemas.autodesk.com/amc/2020/06\">\n";
		serverXMLStream << "  <server hostname=\"0.0.0.0\" port=\"8869\" />\n";
		serverXMLStream << "  <data directory=\"data/\" database=\"sqlite\" sqlitedb=\"storage.db\" />\n";
		serverXMLStream << "  <defaultpackage name=\""<< sDevPackagePrefix << "_package.xml\" githash=\"" << sDevPackagePrefix <<"\" sha256=\"" << sPackageSHA << "\" />\n";
		serverXMLStream << "</amc>\n";

		std::string serverXMLString = serverXMLStream.str();

		{
			std::cout << "Writing to " << sOutputFileName << ".." << std::endl;

			AMCCommon::CExportStream_Native exportStream(sOutputFileName);
			exportStream.writeBuffer((void*)packageXMLString.c_str(), packageXMLString.length());
		}
		
		{
			std::cout << "Writing to " << sServerOutputFileName << ".." << std::endl;

			AMCCommon::CExportStream_Native exportStream(sServerOutputFileName);
			exportStream.writeBuffer((void*)serverXMLString.c_str(), serverXMLString.length());
		}


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