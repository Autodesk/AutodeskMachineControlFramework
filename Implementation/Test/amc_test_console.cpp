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


#include <string>
#include <iostream>
#include <memory>
#include <vector>

#include "amc_test.hpp"
#include "common_utils.hpp"

#define __STRINGIZE(x) #x
#define __STRINGIZE_VALUE_OF(x) __STRINGIZE(x)

using namespace AMCTest;

int main(int argc, char* argv[])
{

	try
	{

		std::string sGitHash = __STRINGIZE_VALUE_OF(__GITHASH);
		std::vector<std::string> testDefinitions;

		std::vector<std::string> commandArguments;
		for (int idx = 1; idx < argc; idx++)
			commandArguments.push_back(argv[idx]);

		for (size_t nIndex = 0; nIndex < commandArguments.size(); nIndex++) {
			std::string sArgument = commandArguments[nIndex];


			if (sArgument == "--githash") {
				nIndex++;
				if (nIndex >= commandArguments.size())
					throw std::runtime_error("missing --githash value");

				sGitHash = commandArguments[nIndex];				
			}

			if (sArgument == "--testdefinition") {
				nIndex++;
				if (nIndex >= commandArguments.size())
					throw std::runtime_error("missing --testdefinition path");

				testDefinitions.push_back (commandArguments[nIndex]);
			}

		}

		std::string sVersionString = std::to_string(LIBMC_VERSION_MAJOR) + "." + std::to_string(LIBMC_VERSION_MINOR) + "." + std::to_string(LIBMC_VERSION_MICRO);

		auto pTestIO = std::make_shared <CTestStdIO>();
		pTestIO->logMessageString("----------------------------------------------------------------------------------");
		pTestIO->logMessageString("Autodesk Machine Control Testing Framework v" + sVersionString);
		pTestIO->logMessageString("----------------------------------------------------------------------------------");
		pTestIO->logMessageString("Using core git hash " + sGitHash + "...");

		if (testDefinitions.empty ())
			throw std::runtime_error("no testdefinitions given");

		for (auto testDefinitionFile : testDefinitions) {
			auto sFullPathName = AMCCommon::CUtils::getFullPathName(testDefinitionFile, true);
			pTestIO->logMessageString("loading tests in " + sFullPathName + "...");

			std::string sTestDefinitionXML = pTestIO->readConfigurationXMLString(sFullPathName);
			
			auto pTest = std::make_shared <CTest>(pTestIO, sTestDefinitionXML, sGitHash);
			pTest->executeBlocking();
		}


	}
	catch (std::exception& E)
	{
		std::cout << "Fatal error: " << E.what () << std::endl;
		return -1;
	}
	catch (...)
	{
		std::cout << "Unhandled fatal exception!" << std::endl;
		return -1;
	}
	
	return 0;

	
}