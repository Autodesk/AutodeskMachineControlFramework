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

#include "amc_test.hpp"
#include <iostream>
#include <pugixml.hpp>

using namespace AMCTest;

#define GITHASH_MIN_LENGTH 6
#define GITHASH_MAX_LENGTH 8

#define XMLNS_TESTDEFINITION "http://schemas.autodesk.com/amc/testdefinitions/2020/02"

CTest::CTest(PTestIO pTestIO, const std::string& sPackageConfigurationXML, const std::string& sGitHash)
	: m_pTestIO (pTestIO), m_sGitHash (sGitHash), m_sPackageConfigurationXML (sPackageConfigurationXML)
{
	if (pTestIO.get() == nullptr)
		throw LibMC::ELibMCException(LIBMC_ERROR_INVALIDPARAM, "invalid parameter");

	if (sGitHash.empty())
		throw LibMC::ELibMCException(LIBMC_ERROR_INVALIDGITHASH, "igithash is empty!");

	if ((sGitHash.length() < GITHASH_MIN_LENGTH) || (sGitHash.length() > GITHASH_MAX_LENGTH)) 
		throw LibMC::ELibMCException(LIBMC_ERROR_INVALIDGITHASH, "invalid githash length!");

	for (auto ch : sGitHash) {
		bool bIsHexChar = ((ch >= '0') && (ch <= '9')) || ((ch >= 'a') && (ch <= 'f')) || ((ch >= 'A') && (ch <= 'F'));
		if (!bIsHexChar)
			throw LibMC::ELibMCException(LIBMC_ERROR_INVALIDGITHASH, "invalid git hash: " + sGitHash + "!");
	}



	pugi::xml_document xmlDoc;
	pugi::xml_parse_result result = xmlDoc.load_string(sPackageConfigurationXML.c_str());
	if (!result)
		throw LibMC::ELibMCException(LIBMC_ERROR_COULDNOTPARSETESTDEFINITION, "Could not parse test definition");


	auto amcNode = xmlDoc.child("testdefinition");
	if (amcNode.empty())
		throw LibMC::ELibMCException(LIBMC_ERROR_MISSINGTESTDEFINITIONNODE, "Missing test definition node");

	auto xmlnsAttrib = amcNode.attribute("xmlns");
	if (xmlnsAttrib.empty())
		throw LibMC::ELibMCException(LIBMC_ERROR_MISSINGXMLSCHEMA, "Missing XML Schema");

	std::string xmlns(xmlnsAttrib.as_string());
	if (xmlns != XMLNS_TESTDEFINITION)
		throw LibMC::ELibMCException(LIBMC_ERROR_INVALIDXMLSCHEMA, "Invalid XML Schema: " + xmlns);


	auto librariesNode = amcNode.child("libraries");
	if (librariesNode.empty ())
		throw LibMC::ELibMCException(LIBMC_ERROR_MISSINGLIBRARIESNODE, "Missing libraries node");

	auto libraryNodes = librariesNode.children("library");
	for (auto libraryNode : libraryNodes) {
		auto nameAttrib = libraryNode.attribute("name");
		std::string sName = nameAttrib.as_string();
		if (sName.empty ())
			throw LibMC::ELibMCException(LIBMC_ERROR_MISSINGLIBRARYNAME, "Missing library name");

		auto dllFileNameAttrib = libraryNode.attribute("dll");
		std::string sDLLFileName = dllFileNameAttrib.as_string();
		if (sDLLFileName.empty())
			throw LibMC::ELibMCException(LIBMC_ERROR_MISSINGLIBRARYDLLFILENAME, "Missing library dll file name");

		auto resourcesFileNameAttrib = libraryNode.attribute("resources");
		std::string sResourceFileName = resourcesFileNameAttrib.as_string();

		m_TestLibraries.push_back(std::make_shared<CTestLibrary> (sName, sDLLFileName, sResourceFileName));
	}

	auto testNodes = amcNode.children("test");
	for (auto testNode : testNodes) {
		auto descriptionAttrib = testNode.attribute("description");
		std::string sDescription = descriptionAttrib.as_string();
		if (sDescription.empty ())
			throw LibMC::ELibMCException(LIBMC_ERROR_MISSINGTESTDESCRIPTION, "Missing test description");

		auto pTestDefinition = std::make_shared<CTestDefinition>(sDescription);

		auto instanceNodes = testNode.children("instance");
		for (auto instanceNode : instanceNodes) {
			auto instanceNameAttrib = instanceNode.attribute("name");
			std::string sInstanceName = instanceNameAttrib.as_string();
			if (sInstanceName.empty ())
				throw LibMC::ELibMCException(LIBMC_ERROR_MISSINGTESTINSTANCENAME, "Missing test instance name");

			pTestDefinition->addInstance(sInstanceName);	
		}

		if (!pTestDefinition->hasInstances ())
			throw LibMC::ELibMCException(LIBMC_ERROR_TESTDEFINITIONHASNOINSTANCES, "Test definition has no instances");


		m_TestDefinitions.push_back(pTestDefinition);
	}

}

CTest::~CTest()
{
	m_pContext = nullptr;
	m_pWrapper = nullptr;
	m_pDataModel = nullptr;
	m_pDataWrapper = nullptr;

}

void CTest::executeBlocking()
{
	m_pContext = nullptr;
	m_pWrapper = nullptr;
	m_pDataModel = nullptr;
	m_pDataWrapper = nullptr;

	try {

		uint32_t nMajorDataVersion = 0;
		uint32_t nMinorDataVersion = 0;
		uint32_t nMicroDataVersion = 0;

		uint32_t nMajorFrameworkVersion = 0;
		uint32_t nMinorFrameworkVersion = 0;
		uint32_t nMicroFrameworkVersion = 0;

		log ("Loading data model...");
		std::string sDataModelLibraryPath = m_sGitHash + "_core_libmcdata.dll";
		std::string sCoreLibraryPath = m_sGitHash + "_core_libmc.dll";
		std::string sCoreLibraryResourcePath = m_sGitHash + "_core.data";
		std::string sTestOutputDirectory = "./";

		m_pDataWrapper = LibMCData::CWrapper::loadLibrary(sDataModelLibraryPath);
		m_pDataWrapper->GetVersion(nMajorDataVersion, nMinorDataVersion, nMicroDataVersion);

		log("Found data model interface " + std::to_string(nMajorDataVersion) + "." + std::to_string(nMinorDataVersion) + "." + std::to_string(nMicroDataVersion));

		m_pDataModel = m_pDataWrapper->CreateDataModelInstance();

		log("Initialising Database...");
		m_pDataModel->InitialiseDatabase(sTestOutputDirectory, LibMCData::eDataBaseType::SqLite, sTestOutputDirectory + "output.db");

		log("Loading framework...");
		m_pWrapper = LibMC::CWrapper::loadLibrary(sCoreLibraryPath);
		m_pWrapper->GetVersion(nMajorFrameworkVersion, nMinorFrameworkVersion, nMicroFrameworkVersion);
		log("Found framework interface " + std::to_string(nMajorFrameworkVersion) + "." + std::to_string(nMinorFrameworkVersion) + "." + std::to_string(nMicroFrameworkVersion));

		m_pWrapper->InjectComponent("LibMCData", m_pDataWrapper->GetSymbolLookupMethod());

		log("Initializing framework...");
		auto pContext = m_pWrapper->CreateMCContext(m_pDataModel);

		pContext->RegisterLibraryPath("core", sCoreLibraryPath, sCoreLibraryResourcePath);
		for (auto pLibrary : m_TestLibraries) {
			pContext->RegisterLibraryPath(pLibrary->getName(), pLibrary->getDLLFileName(m_sGitHash), pLibrary->getResourceFileName(m_sGitHash));
		}

		pContext->Log("Parsing package configuration", LibMC::eLogSubSystem::System, LibMC::eLogLevel::Message);
		pContext->ParseConfiguration(m_sPackageConfigurationXML);

		for (auto pTestDefinition : m_TestDefinitions) {
			auto sDescription = pTestDefinition->getDescription();
			pContext->Log("Running Test " + sDescription + "...", LibMC::eLogSubSystem::Testing, LibMC::eLogLevel::Message);
			auto instanceNames = pTestDefinition->getInstances();

			for (std::string sInstanceName : instanceNames) {
				pContext->StartInstanceThread(sInstanceName);
			}

			bool bAllInstancesAreFinished = false;
			bool bAllInstancesAreSuccessful = false;
			while (!bAllInstancesAreFinished) {
				bAllInstancesAreFinished = true;
				bAllInstancesAreSuccessful = true;

				for (std::string sInstanceName : instanceNames) {
					bool bInstanceIsSuccessful = pContext->InstanceStateIsSuccessful(sInstanceName);
					bool bInstanceHasFailed = pContext->InstanceStateHasFailed(sInstanceName);
					bool bInstanceIsFinished = bInstanceIsSuccessful || bInstanceHasFailed;
					bAllInstancesAreFinished = bAllInstancesAreFinished && bInstanceIsFinished;
					bAllInstancesAreSuccessful = bAllInstancesAreSuccessful && bInstanceIsSuccessful;
				}

				Sleep(100);
			}

			for (std::string sInstanceName : instanceNames) {
				pContext->TerminateInstanceThread(sInstanceName);
			}

		}

	
		pContext->Log("Testing finished", LibMC::eLogSubSystem::System, LibMC::eLogLevel::Message);

	}

	catch (std::exception& E) {
		this->log("Fatal initialization error: " + std::string (E.what ()));
	}
}


void CTest::log(const std::string& sMessage)
{
	m_pTestIO->logMessageString(sMessage);
}

