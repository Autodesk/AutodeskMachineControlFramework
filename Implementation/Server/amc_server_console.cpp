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

#include "amc_server.hpp"
#include "common_utils.hpp"

using namespace AMC;

#ifdef _MSC_VER
int wmain(int argc, wchar_t* argv[])
#else
int main(int argc, char* argv[])
#endif
{

	try
	{

		// Parse arguments
		std::vector<std::string> argumentList;
#ifdef _MSC_VER
		// Convert wide strings to UTF8 on Windows
		for (int argIdx = 1; argIdx < argc; argIdx++) {
			std::wstring wArg(argv[argIdx]);
			argumentList.push_back(AMCCommon::CUtils::UTF16toUTF8(wArg));
		}
#else
		for (int argIdx = 1; argIdx < argc; argIdx++)
			argumentList.push_back(std::string(argv[argIdx]));
#endif

		std::string sConfigurationFileName = "amc_server.xml";
		std::vector<std::pair<std::string, std::string>> parameterOverrides;

		// helper to parse "name=value"
		auto parseNameValue = [](const std::string& token) -> std::pair<std::string, std::string> {
			const auto pos = token.find('=');
			if (pos == std::string::npos)
				throw std::runtime_error("invalid --set argument, expected name=value: \"" + token + "\"");

			std::string name = token.substr(0, pos);
			std::string value = token.substr(pos + 1);

			if (name.empty())
				throw std::runtime_error("invalid --set argument, empty name before '='");

			return { name, value };
		};

		for (size_t argIdx = 0; argIdx < argumentList.size(); argIdx++) {

			bool bHandled = false;

			std::string sArgument = argumentList[argIdx];
			if (sArgument == "--config") {
				argIdx++;
				if (argIdx >= argumentList.size())
					throw std::runtime_error("missing config file name in argument");

				sConfigurationFileName = argumentList[argIdx];
				bHandled = true;
			}

			if (sArgument == "--set") {
				argIdx++;
				if (argIdx >= argumentList.size())
					throw std::runtime_error("missing parameter argument");

				auto nv = parseNameValue(argumentList[argIdx]);
				parameterOverrides.emplace_back(std::move(nv));
				bHandled = true;
			}

			if (!bHandled) {
				std::cout << "Warning! Unknown argument passed: " << sArgument << std::endl;
			}

		}


		auto pServer = std::make_shared <CServer> (std::make_shared <CServerStdIO>());

		for (auto iParameterOverride : parameterOverrides) {
			pServer->addConfigurationParameterOverride(iParameterOverride.first, iParameterOverride.second);
		}

		pServer->executeBlocking(sConfigurationFileName);


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