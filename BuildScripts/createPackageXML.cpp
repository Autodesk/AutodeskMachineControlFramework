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

		std::cout << "Importing " << sConfigFileName << "..." << std::endl;
		std::string sConfigAsString;
		{
			AMCCommon::CImportStream_Native configStream(sConfigFileName);
			sConfigAsString = configStream.readAsString();
			if (sConfigAsString.empty ())
				throw std::runtime_error("configuration file is empty!");
		}


		std::cout << "Creating Package XML for project..." << std::endl;


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