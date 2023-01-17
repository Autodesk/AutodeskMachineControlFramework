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

#ifdef _WIN32
#define CPPHTTPLIB_OPENSSL_SUPPORT
#endif

#include "Libraries/cpp-httplib/httplib.h"

#include "amc_server.hpp"
#include <iostream>

using namespace AMC;

#define __STRINGIZE(x) #x
#define __STRINGIZE_VALUE_OF(x) __STRINGIZE(x)

#define PEMMAXLENGTH (1024 * 1024)

#ifdef _WIN32
class CX509Certificate {
private:
	BIO * m_pIOObject;
	X509* m_pX509Object;
public:
	CX509Certificate()
		: m_pIOObject (nullptr), m_pX509Object (nullptr)
	{

	}

	~CX509Certificate()
	{
		clear();
	}

	void clear()
	{
		if (m_pX509Object != nullptr) {
			X509_free(m_pX509Object);
			m_pX509Object = nullptr;
		}

		if (m_pIOObject != nullptr) {
			BIO_free(m_pIOObject);
			m_pIOObject = nullptr;
		}

	}

	void setPEM(const std::string& sPEMString)
	{
		if (sPEMString.length() > PEMMAXLENGTH)
			throw std::runtime_error("invalid PEM string");

		clear();

		m_pIOObject = BIO_new(BIO_s_mem());
		BIO_write(m_pIOObject, sPEMString.c_str (), (int32_t) sPEMString.length ());
		BIO_seek(m_pIOObject, 0);

		m_pX509Object = PEM_read_bio_X509(m_pIOObject, nullptr, 0, 0);
	}

	X509* getCertificate()
	{
		if (m_pX509Object == nullptr)
			throw std::runtime_error("invalid X509 Certificate");

		return m_pX509Object;
	}
	

};



class CServerPrivateKey {
private:
	BIO* m_pIOObject;
	EVP_PKEY* m_pPKeyObject;
public:
	CServerPrivateKey()
		: m_pIOObject(nullptr), m_pPKeyObject(nullptr)
	{

	}

	~CServerPrivateKey()
	{
		clear();
	}

	void clear()
	{
		if (m_pPKeyObject != nullptr) {
			EVP_PKEY_free(m_pPKeyObject);
			m_pPKeyObject = nullptr;
		}

		if (m_pIOObject != nullptr) {
			BIO_free(m_pIOObject);
			m_pIOObject = nullptr;
		}

	}

	void setPEM(const std::string& sPEMString)
	{
		if (sPEMString.length() > PEMMAXLENGTH)
			throw std::runtime_error("invalid PEM string");

		clear();

		m_pIOObject = BIO_new(BIO_s_mem());
		BIO_write(m_pIOObject, sPEMString.c_str(), (int32_t)sPEMString.length());
		BIO_seek(m_pIOObject, 0);

		m_pPKeyObject = PEM_read_bio_PrivateKey(m_pIOObject, nullptr, 0, 0);

	}

	EVP_PKEY* getPrivateKey()
	{
		if (m_pPKeyObject == nullptr)
			throw std::runtime_error("invalid Private Key");

		return m_pPKeyObject;
	}


};

#endif // _WIN32

CServer::CServer(PServerIO pServerIO)
	: m_pServerIO (pServerIO), m_pListeningServerInstance (nullptr)
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
	m_pContext = nullptr;
	m_pWrapper = nullptr;
	m_pDataModel = nullptr;
	m_pDataWrapper = nullptr;

}

void onLogMessage(const char* pLogMessage, const char* pSubSystem, LibMCData::eLogLevel eLogLevel, const char* pTimeStamp, LibMCData_pvoid pUserData)
{
	if ((pLogMessage != nullptr) && (pSubSystem != nullptr) && (pTimeStamp != nullptr) && (pUserData != nullptr)) {
		std::string sLogMessage(pLogMessage);
		std::string sSubSystem(pSubSystem);
		std::string sTimeStamp(pTimeStamp);
		CServer* pServer = (CServer*)pUserData;

		pServer->log(sTimeStamp + " | " + sLogMessage);
	}
}

void CServer::executeBlocking(const std::string& sConfigurationFileName)
{
	m_pContext = nullptr;
	m_pWrapper = nullptr;
	m_pDataModel = nullptr;
	m_pDataWrapper = nullptr;
	m_pListeningServerInstance = nullptr;

	try {
		uint32_t nMajorDataVersion = 0;
		uint32_t nMinorDataVersion = 0;
		uint32_t nMicroDataVersion = 0;

		uint32_t nMajorFrameworkVersion = 0;
		uint32_t nMinorFrameworkVersion = 0;
		uint32_t nMicroFrameworkVersion = 0;

		log("Loading server configuration...");

		std::string sConfigurationXML = m_pServerIO->readConfigurationString(sConfigurationFileName);
		m_pServerConfiguration = std::make_shared<CServerConfiguration>(sConfigurationXML, m_pServerIO);

		log("Loading data model...");

		m_pDataWrapper = LibMCData::CWrapper::loadLibrary(m_pServerConfiguration->getLibraryPath("datamodel"));
		m_pDataWrapper->GetVersion(nMajorDataVersion, nMinorDataVersion, nMicroDataVersion);
		log("Found data model interface " + std::to_string(nMajorDataVersion) + "." + std::to_string(nMinorDataVersion) + "." + std::to_string(nMicroDataVersion));

		m_pDataModel = m_pDataWrapper->CreateDataModelInstance();

		m_pDataModel->SetBaseTempDirectory(m_pServerConfiguration->getBaseTempDirectory ());

		log("Initialising Database...");
		m_pDataModel->InitialiseDatabase(m_pServerConfiguration->getDataDirectory(), m_pServerConfiguration->getDataBaseType(), m_pServerConfiguration->getConnectionString());

		log("Loading framework...");
		m_pWrapper = LibMC::CWrapper::loadLibrary(m_pServerConfiguration->getLibraryPath("core"));
		m_pWrapper->GetVersion(nMajorFrameworkVersion, nMinorFrameworkVersion, nMicroFrameworkVersion);
		log("Found framework interface " + std::to_string(nMajorFrameworkVersion) + "." + std::to_string(nMinorFrameworkVersion) + "." + std::to_string(nMicroFrameworkVersion));

		m_pWrapper->InjectComponent("LibMCData", m_pDataWrapper->GetSymbolLookupMethod());

		m_pDataModel->SetLogCallback (onLogMessage, this);

		log("Initializing framework...");
		m_pContext = m_pWrapper->CreateMCContext(m_pDataModel);

		//Register Library Path
		auto libraryList = m_pServerConfiguration->getLibraryNames();
		for (auto sLibraryName : libraryList) {
			m_pContext->RegisterLibraryPath(sLibraryName, m_pServerConfiguration->getLibraryPath(sLibraryName), m_pServerConfiguration->getResourcePath(sLibraryName));
		}

		m_pContext->Log("Loading " + m_pServerConfiguration->getPackageName() + " (" + m_pServerConfiguration->getPackageConfig() + ")", LibMC::eLogSubSystem::System, LibMC::eLogLevel::Message);
		std::string sPackageConfigurationXML = m_pServerIO->readConfigurationString(m_pServerConfiguration->getPackageConfig());

		m_pContext->Log("Parsing package configuration", LibMC::eLogSubSystem::System, LibMC::eLogLevel::Message);
		m_pContext->ParseConfiguration(sPackageConfigurationXML);

		m_pContext->Log("Loading " + m_pServerConfiguration->getPackageCoreClient() + "...", LibMC::eLogSubSystem::System, LibMC::eLogLevel::Message);
		m_pContext->LoadClientPackage(m_pServerConfiguration->getPackageCoreClient());


		std::string sHostName = m_pServerConfiguration->getHostName();
		uint32_t nPort = m_pServerConfiguration->getPort();

		m_pContext->StartAllThreads();

		try {

			auto requestHandler = [this](const httplib::Request& req, httplib::Response& res) {
				try {

					std::string sAuthorization = std::string(req.get_header_value("authorization"));
					std::string sURL = std::string(req.path);
					std::string sMethod = req.method;

					std::vector <uint8_t> Buffer;
					std::vector <uint8_t> ResultBuffer;
					std::string sContentType;
					uint32_t nHttpCode;

					auto pHandler = m_pContext->CreateAPIRequestHandler(sURL, sMethod, sAuthorization);

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
				catch (std::exception& E) {
					this->log("Internal server error: " + std::string(E.what()));
					res.status = 500;
					res.set_content("Internal Server Error", "text/plain");
				}
				catch (...) {
					this->log("Internal server error (Unknown)");

					res.status = 500;
					res.set_content("Internal Server Error", "text/plain");
				}
			};


			if (m_pServerConfiguration->useSSL()) {

#ifdef _WIN32

				CX509Certificate serverCertificate;
				CServerPrivateKey privateKey;

				try {
					serverCertificate.setPEM(m_pServerConfiguration->getServerCertificatePEM());
					privateKey.setPEM(m_pServerConfiguration->getServerPrivateKeyPEM());

					httplib::SSLServer sslsvr(serverCertificate.getCertificate(), privateKey.getPrivateKey());

					sslsvr.Get("(.*?)", requestHandler);
					sslsvr.Post("(.*?)", requestHandler);
					sslsvr.Put("(.*?)", requestHandler);

					m_pListeningServerInstance = &sslsvr;
					sslsvr.listen(sHostName.c_str(), nPort);
					m_pListeningServerInstance = nullptr;

					m_pContext->TerminateAllThreads();

				}
				catch (std::exception & E) {
					m_pListeningServerInstance = nullptr;
					this->log("Fatal error: " + std::string(E.what()));

					m_pContext->TerminateAllThreads();
					throw;
				}
#else
				throw std::runtime_error("SSL support is not implemented on this platform!");

#endif // _WIN32
			}
			else {

				httplib::Server svr;
				svr.Get("(.*?)", requestHandler);
				svr.Post("(.*?)", requestHandler);
				svr.Put("(.*?)", requestHandler);
				m_pListeningServerInstance = &svr;
				svr.listen(sHostName.c_str(), nPort);
				m_pListeningServerInstance = nullptr;

				m_pContext->TerminateAllThreads();
			}

			

			this->log("Failed to listen on " + sHostName + ":" + std::to_string(nPort));

		}
		catch (std::exception& E) {

			m_pListeningServerInstance = nullptr;
			this->log("Fatal error while listening on " + sHostName + ":" + std::to_string(nPort));
			this->log(E.what());
		}

	}
	catch (std::exception& E) {
		this->log("Fatal initialization error: " + std::string (E.what ()));
	}
}


void CServer::log(const std::string& sMessage)
{
	m_pServerIO->logMessageString(sMessage);
}

PServerIO CServer::getServerIO()
{
	return m_pServerIO;
}

void CServer::stopListening()
{
	if (m_pListeningServerInstance != nullptr) {
		httplib::Server* pServerInstance = (httplib::Server*)m_pListeningServerInstance;
		pServerInstance->stop();
	}
}

