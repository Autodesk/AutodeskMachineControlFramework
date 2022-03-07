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

#include "Libraries/cpp-httplib/httplib.h"

#include "amc_server.hpp"
#include <iostream>

using namespace AMC;

#define __STRINGIZE(x) #x
#define __STRINGIZE_VALUE_OF(x) __STRINGIZE(x)



CServer::CServer(PServerIO pServerIO)
	: m_pServerIO (pServerIO)
{
	if (pServerIO.get() == nullptr)
		throw LibMC::ELibMCException(LIBMC_ERROR_INVALIDPARAM, "invalid parameter");

	m_sVersionString = std::to_string (LIBMC_VERSION_MAJOR) + "." + std::to_string(LIBMC_VERSION_MINOR) + "." + std::to_string(LIBMC_VERSION_MICRO);
	m_sGitHash = __STRINGIZE_VALUE_OF(__GITHASH);

	log("----------------------------------------------------------------------------------");
	log("Autodesk Machine Control Framework v" + m_sVersionString + " (" + m_sGitHash + ")");
	log("----------------------------------------------------------------------------------");


}

CServer::~CServer()
{

}

void CServer::executeBlocking(const std::string& sConfigurationFileName)
{
	uint32_t nMajorDataVersion = 0;
	uint32_t nMinorDataVersion = 0;
	uint32_t nMicroDataVersion = 0;

	uint32_t nMajorFrameworkVersion = 0;
	uint32_t nMinorFrameworkVersion = 0;
	uint32_t nMicroFrameworkVersion = 0;

	log("Loading server configuration...");

	std::string sConfigurationXML = m_pServerIO->readConfigurationXMLString(sConfigurationFileName);
	m_pServerConfiguration = std::make_shared<CServerConfiguration>(sConfigurationXML);

	log("Loading data model...");

	m_pDataWrapper = LibMCData::CWrapper::loadLibrary(m_pServerConfiguration->getLibraryPath ("datamodel"));
	m_pDataWrapper->GetVersion(nMajorDataVersion, nMinorDataVersion, nMicroDataVersion);
	log("Found data model interface " + std::to_string (nMajorDataVersion) + "." + std::to_string (nMinorDataVersion) + "." + std::to_string (nMicroDataVersion));

	m_pDataModel = m_pDataWrapper->CreateDataModelInstance();

	log("Initialising Database...");
	m_pDataModel->InitialiseDatabase(m_pServerConfiguration->getDataDirectory(), m_pServerConfiguration->getDataBaseType(), m_pServerConfiguration->getConnectionString());

	log("Loading framework...");
	m_pWrapper = LibMC::CWrapper::loadLibrary(m_pServerConfiguration->getLibraryPath ("core"));
	m_pWrapper->GetVersion(nMajorFrameworkVersion, nMinorFrameworkVersion, nMicroFrameworkVersion);
	log("Found data model interface " + std::to_string(nMajorFrameworkVersion) + "." + std::to_string(nMinorFrameworkVersion) + "." + std::to_string(nMicroFrameworkVersion));

	m_pWrapper->InjectComponent("LibMCData", m_pDataWrapper->GetSymbolLookupMethod());

	log("Initializing framework...");
	auto pContext = m_pWrapper->CreateMCContext(m_pDataModel);

	//Register Library Path
	auto libraryList = m_pServerConfiguration->getLibraryNames();
	for (auto sLibraryName : libraryList) {
		pContext->RegisterLibraryPath(sLibraryName, m_pServerConfiguration->getLibraryPath(sLibraryName), m_pServerConfiguration->getResourcePath(sLibraryName));
	}

	pContext->Log ("Loading " + m_pServerConfiguration->getPackageName () + " (" + m_pServerConfiguration->getPackageConfig () + ")", LibMC::eLogSubSystem::System, LibMC::eLogLevel::Message);
	std::string sPackageConfigurationXML = m_pServerIO->readConfigurationXMLString(m_pServerConfiguration->getPackageConfig());

	pContext->Log("Parsing package configuration", LibMC::eLogSubSystem::System, LibMC::eLogLevel::Message);
	pContext->ParseConfiguration(sPackageConfigurationXML);

	pContext->Log("Loading " + m_pServerConfiguration->getPackageCoreClient() + "...", LibMC::eLogSubSystem::System, LibMC::eLogLevel::Message);
	pContext->LoadClientPackage(m_pServerConfiguration->getPackageCoreClient ());

	pContext->StartAllThreads();


	std::string sHostName = m_pServerConfiguration->getHostName();
	uint32_t nPort = m_pServerConfiguration->getPort();



	try {

		httplib::Server svr;


		auto requestHandler = [this, pContext](const httplib::Request& req, httplib::Response& res) {
			try {

				std::string sAuthorization = std::string(req.get_header_value("authorization"));
				std::string sURL = std::string(req.path);
				std::string sMethod = req.method;

				std::vector <uint8_t> Buffer;
				std::vector <uint8_t> ResultBuffer;
				std::string sContentType;
				uint32_t nHttpCode;

				auto pHandler = pContext->CreateAPIRequestHandler(sURL, sMethod, sAuthorization);

				uint32_t nFieldCount = 0;
				if (pHandler->ExpectsFormData(nFieldCount)) {
					for (uint32_t nIndex = 0; nIndex < nFieldCount; nIndex++) {
						std::string sName;
						bool bIsFile = false;
						bool bIsMandatory = false;
						pHandler->GetFormDataDetails(nIndex, sName, bIsFile, bIsMandatory);

						if (bIsFile) {
							auto pFormData = req.get_file_value(sName.c_str());
							std::vector <uint8_t> FormData(pFormData.content.begin(), pFormData.content.end());
							pHandler->SetFormDataField(sName, FormData);
						}
						else {
							auto pFormData = req.get_file_value(sName.c_str());
							pHandler->SetFormStringField(sName, pFormData.content);
						}
					}
				}

				if (pHandler->ExpectsRawBody()) {
					auto body = req.body;
					Buffer.reserve(body.length());
					for (auto c : body)
						Buffer.push_back((uint8_t)c);
				}


				pHandler->Handle(Buffer, sContentType, nHttpCode);

				pHandler->GetResultData(ResultBuffer);

				if (!ResultBuffer.empty()) {
					std::string sResult(reinterpret_cast<char*>(ResultBuffer.data()), ResultBuffer.size());

					res.set_content(sResult, sContentType.c_str());

				}
				else {
					res.set_content("", sContentType.c_str());
				}

				res.status = nHttpCode;

			}
			catch (std::exception & E) {
				this->log("Internal server error: " + std::string (E.what()));
				res.status = 500;
				res.set_content("Internal Server Error", "text/plain");
			}
			catch (...) {
				this->log("Internal server error (Unknown)");

				res.status = 500;
				res.set_content("Internal Server Error", "text/plain");
			}
		};
		
		

		svr.Get("(.*?)", requestHandler);
		svr.Post("(.*?)", requestHandler);
		svr.Put("(.*?)", requestHandler);

		svr.listen(sHostName.c_str(), nPort);

		this->log("Failed to listen on " + sHostName + ":" + std::to_string(nPort));
	}
	catch (std::exception& E) {
		this->log("Fatal error while listening on " + sHostName + ":" + std::to_string(nPort));
		this->log(E.what());
	}
}


void CServer::log(const std::string& sMessage)
{
	m_pServerIO->logMessageString(sMessage);
}

