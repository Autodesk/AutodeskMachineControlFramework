/*++

Copyright (C) 2020 Autodesk Inc.

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


#include "libmc_mccontext.hpp"
#include "libmc_interfaceexception.hpp"
#include "libmc_apirequesthandler.hpp"
#include "pugixml.hpp"

#include "amc_statemachineinstance.hpp"
#include "amc_logger.hpp"
#include "amc_parameterhandler.hpp"
#include "amc_statemachinedata.hpp"
#include "amc_logger_multi.hpp"
#include "amc_logger_callback.hpp"
#include "amc_logger_database.hpp"
#include "amc_servicehandler.hpp"
#include "amc_ui_handler.hpp"
#include "amc_resourcepackage.hpp"

#include "amc_api_factory.hpp"
#include "amc_api_sessionhandler.hpp"

#include "common_importstream_native.hpp"
#include "libmc_exceptiontypes.hpp"


// Include custom headers here.
#include <iostream>

#define MACHINEDEFINITION_XMLSCHEMA "http://schemas.autodesk.com/amc/machinedefinitions/2020/02"
#define MACHINEDEFINITIONTEST_XMLSCHEMA "http://schemas.autodesk.com/amc/testdefinitions/2020/02"

using namespace LibMC::Impl;
using namespace AMC;

/*************************************************************************************************************************
 Class definition of CMCContext 
**************************************************************************************************************************/

CMCContext::CMCContext(LibMCData::PDataModel pDataModel)
    : m_bIsTestingEnvironment (false)
{
    LibMCAssertNotNull(pDataModel.get());

    m_pStateJournal = std::make_shared<CStateJournal> (std::make_shared<CStateJournalStream> ());

    m_pEnvironmentWrapper = LibMCEnv::CWrapper::loadLibraryFromSymbolLookupMethod((void*) LibMCEnv::Impl::LibMCEnv_GetProcAddress);

    // Create Log Multiplexer to StdOut and Database
    auto pMultiLogger = std::make_shared<AMC::CLogger_Multi>();
    pMultiLogger->addLogger(std::make_shared<AMC::CLogger_Database> (pDataModel->CreateNewLogSession ()));
    if (pDataModel->HasLogCallback())
        pMultiLogger->addLogger(std::make_shared<AMC::CLogger_Callback>(pDataModel));

    // Create system state
    m_pSystemState = std::make_shared <CSystemState> (pMultiLogger, pDataModel, m_pEnvironmentWrapper, "./testoutput");

    // Create API Handlers for data model requests
    m_pAPI = std::make_shared<AMC::CAPI>();
    CAPIFactory factory (m_pAPI, m_pSystemState, m_InstanceList);

    // Create Client Dist Handler
    m_pClientDistHandler = std::make_shared <CAPIHandler_Root>(m_pSystemState->getClientHash ());
    m_pAPI->registerHandler (m_pClientDistHandler);

    std::string sTempPath = pDataModel->GetBaseTempDirectory();

    if (sTempPath.empty()) {
        // Set Temporary Path (as default value)
#ifdef _WIN32
        std::vector<wchar_t> TempPathBuffer;
        TempPathBuffer.resize(MAX_PATH + 1);
        auto nSize = GetTempPathW(MAX_PATH, TempPathBuffer.data());
        if (nSize == 0)
            throw ELibMCNoContextException(LIBMC_ERROR_COULDNOTGETTEMPPATHFROMWINDOWS);

        TempPathBuffer[MAX_PATH] = 0;
        sTempPath = AMCCommon::CUtils::UTF16toUTF8(TempPathBuffer.data());
        m_pSystemState->driverHandler()->setTempBasePath(sTempPath);

#else
        sTempPath = "/tmp";
#endif
    }

    m_pSystemState->driverHandler()->setTempBasePath(sTempPath);
}

CMCContext::~CMCContext()
{
    m_Instances.clear();
    m_InstanceList.clear();
    m_Plugins.clear();
}


void CMCContext::ParseConfiguration(const std::string & sXMLString)
{

    try {

        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_string(sXMLString.c_str());
        if (!result)
            throw ELibMCNoContextException(LIBMC_ERROR_COULDNOTPARSECONFIGURATION);

        pugi::xml_node mainNode;

        auto machinedefinitionNode = doc.child("machinedefinition");
        auto testdefinitionNode = doc.child("testdefinition");

        if (!machinedefinitionNode.empty()) {
            mainNode = machinedefinitionNode;
            m_bIsTestingEnvironment = false;
        }

        if (!testdefinitionNode.empty()) {
            if (!mainNode.empty ())
                throw ELibMCNoContextException(LIBMC_ERROR_AMBIGIOUSMAINNODE);
            mainNode = testdefinitionNode;
            m_bIsTestingEnvironment = true;
        } 


        if (mainNode.empty()) {
            throw ELibMCNoContextException(LIBMC_ERROR_MISSINGMAINNODE);
        }

        auto xmlnsAttrib = mainNode.attribute("xmlns");
        if (xmlnsAttrib.empty())
            throw ELibMCNoContextException(LIBMC_ERROR_MISSINGXMLSCHEMA);

        std::string xmlns(xmlnsAttrib.as_string());
        if (m_bIsTestingEnvironment) {
            if (xmlns != MACHINEDEFINITIONTEST_XMLSCHEMA)
                throw ELibMCCustomException(LIBMC_ERROR_INVALIDXMLSCHEMA, xmlns);
        }
        else {
            if (xmlns != MACHINEDEFINITION_XMLSCHEMA)
                throw ELibMCCustomException(LIBMC_ERROR_INVALIDXMLSCHEMA, xmlns);

        }

        auto servicesNode = mainNode.child("services");
        if (!servicesNode.empty()) {

            auto threadCountAttrib = servicesNode.attribute("threadcount");
            if (threadCountAttrib.empty()) 
                throw ELibMCNoContextException(LIBMC_ERROR_MISSINGTHREADCOUNT);
            auto nMaxThreadCount = threadCountAttrib.as_uint(SERVICETHREADCOUNT_DEFAULT);
            if ((nMaxThreadCount < SERVICETHREADCOUNT_MIN) || (nMaxThreadCount > SERVICETHREADCOUNT_MAX))
                throw ELibMCCustomException(LIBMC_ERROR_INVALIDTHREADCOUNT, threadCountAttrib.as_string());
            m_pSystemState->serviceHandler()->setMaxThreadCount((uint32_t)nMaxThreadCount);
        }
        else {
            m_pSystemState->serviceHandler()->setMaxThreadCount(SERVICETHREADCOUNT_DEFAULT);            
        }

        auto sCoreResourcePath = m_pSystemState->getLibraryResourcePath("core");
        m_pSystemState->logger()->logMessage("Loading core resources from " + sCoreResourcePath + "...", LOG_SUBSYSTEM_SYSTEM, AMC::eLogLevel::Message);
        auto pResourcePackageStream = std::make_shared<AMCCommon::CImportStream_Native>(sCoreResourcePath);
        m_pCoreResourcePackage = CResourcePackage::makeFromStream(pResourcePackageStream, sCoreResourcePath, AMCPACKAGE_SCHEMANAMESPACE);


        m_pSystemState->logger()->logMessage("Loading drivers...", LOG_SUBSYSTEM_SYSTEM, AMC::eLogLevel::Message);
        auto driversNodes = mainNode.children("driver");
        for (pugi::xml_node driversNode : driversNodes)
        {
            addDriver(driversNode);
        }


        m_pSystemState->logger()->logMessage("Initializing state machines...", LOG_SUBSYSTEM_SYSTEM, AMC::eLogLevel::Message);
        auto statemachinesNodes = mainNode.children("statemachine");
        for (pugi::xml_node instanceNode : statemachinesNodes)
        {
            addMachineInstance(instanceNode);
        }


        m_pSystemState->logger()->logMessage("Starting Journal recording...", LOG_SUBSYSTEM_SYSTEM, AMC::eLogLevel::Message);
        // Start journal recording
        m_pStateJournal->startRecording();

        // Load persistent parameters
        for (auto pStateMachineInstance : m_InstanceList)
            pStateMachineInstance->getParameterHandler()->loadPersistentParameters(m_pSystemState->getPersistencyHandler ());

        // Load User Interface
        auto userInterfaceNode = mainNode.child("userinterface");
        if (userInterfaceNode.empty()) {

            if (!m_bIsTestingEnvironment)
                throw ELibMCNoContextException(LIBMC_ERROR_NOUSERINTERFACEDEFINITION);

            m_pSystemState->logger()->logMessage("Using default testing UI Handler...", LOG_SUBSYSTEM_SYSTEM, AMC::eLogLevel::Message);
            m_pSystemState->uiHandler()->setCoreResourcePackage(m_pCoreResourcePackage);
        }
        else {

            // Load user interface
            auto uiLibraryAttrib = userInterfaceNode.attribute("library");
            if (uiLibraryAttrib.empty())
                throw ELibMCNoContextException(LIBMC_ERROR_NOUSERINTERFACEPLUGIN);

            auto sUILibraryPath = m_pSystemState->getLibraryPath(uiLibraryAttrib.as_string());

            m_pSystemState->logger()->logMessage("Loading UI Handler...", LOG_SUBSYSTEM_SYSTEM, AMC::eLogLevel::Message);
            m_pSystemState->uiHandler()->setCoreResourcePackage(m_pCoreResourcePackage);
            m_pSystemState->uiHandler()->loadFromXML(userInterfaceNode, sUILibraryPath, m_pSystemState->getBuildJobHandlerInstance());
        }

    }
    catch (std::exception& E) {
        m_pSystemState->logger()->logMessage(std::string ("initialization error: ") + E.what(), LOG_SUBSYSTEM_SYSTEM, AMC::eLogLevel::CriticalError);
        throw E;
    }

}


void CMCContext::RegisterLibraryPath(const std::string& sLibraryName, const std::string& sLibraryPath, const std::string& sLibraryResource)
{
    m_pSystemState->logger()->logMessage("mapping " + sLibraryName + " to " + sLibraryPath + "...", LOG_SUBSYSTEM_SYSTEM, AMC::eLogLevel::Message);
    m_pSystemState->addLibraryPath(sLibraryName, sLibraryPath, sLibraryResource);    
}

void CMCContext::SetTempBasePath(const std::string& sTempBasePath)
{
    if (sTempBasePath.empty())
        throw ELibMCNoContextException(LIBMC_ERROR_TEMPBASEPATHEMPTY);

    m_pSystemState->driverHandler()->setTempBasePath(sTempBasePath);

}


void CMCContext::LoadClientPackage(const std::string& sResourcePath)
{
    auto pStream = std::make_shared<AMCCommon::CImportStream_Native>(sResourcePath);
    auto pPackage = CResourcePackage::makeFromStream(pStream, sResourcePath, AMCPACKAGE_SCHEMANAMESPACE);

    m_pClientDistHandler->LoadClientPackage (pPackage);
}

struct xml_sstream_writer : pugi::xml_writer
{
    std::stringstream resultStream;

    virtual void write(const void* data, size_t size)
    {
        resultStream << std::string (static_cast<const char*>(data), size);
    }
};

void CMCContext::addDriver(const pugi::xml_node& xmlNode)
{
    auto nameAttrib = xmlNode.attribute("name");
    if (nameAttrib.empty())
        throw ELibMCNoContextException(LIBMC_ERROR_MISSINGDRIVERNAME);
    std::string sName(nameAttrib.as_string());


    auto typeAttrib = xmlNode.attribute("type");
    if (typeAttrib.empty())
        throw ELibMCCustomException(LIBMC_ERROR_MISSINGDRIVERTYPE, sName);
    std::string sType(typeAttrib.as_string());

    auto libraryAttrib = xmlNode.attribute("library");
    if (libraryAttrib.empty())
        throw ELibMCCustomException(LIBMC_ERROR_MISSINGDRIVERLIBRARY, sName);
    std::string sLibraryName(libraryAttrib.as_string());
    
    m_pSystemState->logger()->logMessage("Initializing " + sName + " (" + sType + "@" + sLibraryName + ")", LOG_SUBSYSTEM_SYSTEM, AMC::eLogLevel::Message);

    std::string sConfigurationData = "";

    std::string sConfigSchema = xmlNode.attribute("configurationschema").as_string ();
    std::string sConfigResource = xmlNode.attribute("configurationresource").as_string ();
    if ((!sConfigResource.empty()) && (!sConfigSchema.empty()))
        throw ELibMCCustomException(LIBMC_ERROR_AMBIGUOUSDRIVERCONFIGURATION, sName);

    // If configuration schema is given, then the driver configuration is inlined in the config XML.
    if (!sConfigSchema.empty()) {
        pugi::xml_document config_document;
        xml_sstream_writer config_writer;

        // Copy driver node content into driver configuration XML.
        auto driverNode = config_document.append_child("driverconfiguration");
        driverNode.append_attribute("xmlns").set_value(sConfigSchema.c_str());
        for (auto subNode : xmlNode.children()) {
            driverNode.prepend_copy(subNode);
        }
       
        config_document.save(config_writer);

        sConfigurationData = config_writer.resultStream.str();

        std::cout << sConfigurationData << std::endl;
    }

    if (!sConfigResource.empty()) {
        sConfigurationData = m_pCoreResourcePackage->readEntryUTF8String(sConfigResource);
    }

    try {
        m_pSystemState->driverHandler()->registerDriver(sName, sType, m_pSystemState->getLibraryPath(sLibraryName), m_pSystemState->getLibraryResourcePath(sLibraryName), sConfigurationData, m_pCoreResourcePackage);
    } 
    catch (std::exception & E) {
        m_pSystemState->logger()->logMessage(std::string ("Driver error: ") + E.what(), LOG_SUBSYSTEM_SYSTEM, AMC::eLogLevel::FatalError);
    }

}

AMC::PStateMachineInstance CMCContext::addMachineInstance(const pugi::xml_node& xmlNode)
{
    auto nameAttrib = xmlNode.attribute("name");
    if (nameAttrib.empty())
        throw ELibMCNoContextException(LIBMC_ERROR_MISSINGINSTANCENAME);
    std::string sName (nameAttrib.as_string());

    auto descriptionAttrib = xmlNode.attribute("description");
    if (descriptionAttrib.empty())
        throw ELibMCCustomException(LIBMC_ERROR_MISSINGINSTANCEDESCRIPTION, sName);
    std::string sDescription(descriptionAttrib.as_string());

    auto initstateAttrib = xmlNode.attribute("initstate");
    if (initstateAttrib.empty())
        throw ELibMCCustomException(LIBMC_ERROR_MISSINGINITSTATE, sName);
    std::string sInitState(initstateAttrib.as_string());
    if (sInitState.length() == 0)
        throw ELibMCCustomException(LIBMC_ERROR_EMPTYINITSTATE, sName);

    auto failedstateAttrib = xmlNode.attribute("failedstate");
    if (failedstateAttrib.empty())
        throw ELibMCCustomException(LIBMC_ERROR_MISSINGFAILEDSTATE, sName);
    std::string sFailedState(failedstateAttrib.as_string());
    if (sFailedState.length() == 0)
        throw ELibMCCustomException(LIBMC_ERROR_EMPTYFAILEDSTATE, sName);

    auto successstateAttrib = xmlNode.attribute("successstate");
    std::string sSuccessState = successstateAttrib.as_string ();

    auto libraryAttrib = xmlNode.attribute("library");
    if (libraryAttrib.empty())
        throw ELibMCCustomException(LIBMC_ERROR_MISSINGPLUGINNAME, sName);
    std::string slibraryName (libraryAttrib.as_string());
    if (slibraryName.length() == 0)
        throw ELibMCCustomException(LIBMC_ERROR_EMPTYPLUGINNAME, sName);

    auto pInstance = findMachineInstance(sName, false);
    if (pInstance.get() != nullptr)
        throw ELibMCCustomException(LIBMC_ERROR_DUPLICATESTATENAME, sName);

    if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sName))
        throw ELibMCCustomException(LIBMC_ERROR_INVALIDSTATEMACHINENAME, sName);

    
    m_pSystemState->logger()->logMessage("Creating state machine \"" + sName + "\"", LOG_SUBSYSTEM_SYSTEM, AMC::eLogLevel::Message);
    pInstance = std::make_shared<CStateMachineInstance> (sName, sDescription, m_pEnvironmentWrapper, m_pSystemState, m_pStateJournal);

    auto signalNodes = xmlNode.children("signaldefinition");
    for (pugi::xml_node signalNode : signalNodes) {
        auto signalNameAttrib = signalNode.attribute("name");
        if (signalNameAttrib.empty())
            throw ELibMCCustomException(LIBMC_ERROR_MISSINGSIGNALNAME, "statemachine " + sName);

        std::list<CStateSignalParameter> SignalParameters;
        std::list<CStateSignalParameter> SignalResults;

        readSignalParameters(signalNameAttrib.as_string(), signalNode, SignalParameters, SignalResults);

        m_pSystemState->stateSignalHandler()->addSignalDefinition(sName, signalNameAttrib.as_string(), SignalParameters, SignalResults);

    }


    auto pParameterHandler = pInstance->getParameterHandler();

    // Load all value parameters
    auto parameterGroupNodes = xmlNode.children("parametergroup");
    for (pugi::xml_node parameterGroupNode : parameterGroupNodes) {
        auto groupNameAttrib = parameterGroupNode.attribute("name");
        if (groupNameAttrib.empty())
            throw ELibMCCustomException(LIBMC_ERROR_MISSINGPARAMETERGROUPNAME, "statemachine " + sName);
        auto groupDescriptionAttrib = parameterGroupNode.attribute("description");
        if (groupDescriptionAttrib.empty())
            throw ELibMCCustomException(LIBMC_ERROR_MISSINGPARAMETERGROUPDESCRIPTION, groupNameAttrib.as_string ());

        auto pGroup = pParameterHandler->addGroup(groupNameAttrib.as_string(), groupDescriptionAttrib.as_string());
        pGroup->setJournal(m_pStateJournal, sName);
        loadParameterGroup(parameterGroupNode, pGroup);
    }

    // Load all driver parameters
    auto driverParameterGroupNodes = xmlNode.children("driverparametergroup");
    for (pugi::xml_node driverParameterGroupNode : driverParameterGroupNodes) {
        auto groupNameAttrib = driverParameterGroupNode.attribute("name");
        if (groupNameAttrib.empty())
            throw ELibMCCustomException(LIBMC_ERROR_MISSINGPARAMETERGROUPNAME, "statemachine " + sName);
        auto groupDescriptionAttrib = driverParameterGroupNode.attribute("description");
        if (groupDescriptionAttrib.empty())
            throw ELibMCCustomException(LIBMC_ERROR_MISSINGPARAMETERGROUPDESCRIPTION, groupNameAttrib.as_string ());

        auto pGroup = pParameterHandler->addGroup(groupNameAttrib.as_string(), groupDescriptionAttrib.as_string());
        pGroup->setJournal(m_pStateJournal, sName);
        loadDriverParameterGroup(driverParameterGroupNode, pGroup);
    }


    // Load all derived parameters
    for (pugi::xml_node parameterGroupNode : parameterGroupNodes) {
        auto groupNameAttrib = parameterGroupNode.attribute("name");
        if (groupNameAttrib.empty())
            throw ELibMCCustomException(LIBMC_ERROR_MISSINGPARAMETERGROUPNAME, "statemachine " + sName);

        auto pGroup = pParameterHandler->findGroup(groupNameAttrib.as_string(), true);
        loadParameterGroupDerives(parameterGroupNode, pGroup, sName);
    }


    auto statesNodes = xmlNode.children("state");
    for (pugi::xml_node stateNode : statesNodes)
    {

        auto stateNameAttrib = stateNode.attribute("name");
        if (stateNameAttrib.empty())
            throw ELibMCCustomException(LIBMC_ERROR_MISSINGSTATENAME, "statemachine " + sName);

        uint32_t nRepeatDelay;

        auto repeatDelayAttrib = stateNode.attribute("repeatdelay");
        if (!repeatDelayAttrib.empty()) {
            nRepeatDelay = repeatDelayAttrib.as_int();
        }
        else {
            if (m_bIsTestingEnvironment) {
                nRepeatDelay = AMC_DEFAULTREPEATDELAY_FOR_TESTING_MS;
            }
            else
            {
                throw ELibMCCustomException(LIBMC_ERROR_MISSINGREPEATDELAY, stateNameAttrib.as_string());
            }
            
        }

        auto pState = pInstance->addState(stateNameAttrib.as_string(), nRepeatDelay);
        

    }


    // Load Out States
    for (pugi::xml_node stateNode : statesNodes)
    {
        auto stateNameAttrib = stateNode.attribute("name");
        if (stateNameAttrib.empty())
            throw ELibMCCustomException(LIBMC_ERROR_MISSINGSTATENAME, "statemachine " + sName);

        auto pState = pInstance->findState(stateNameAttrib.as_string(), true);

        auto outstateNodes = stateNode.children("outstate");
        for (pugi::xml_node outstateNode : outstateNodes) {
            auto targetAttrib = outstateNode.attribute("target");
            if (targetAttrib.empty())
                throw ELibMCCustomException(LIBMC_ERROR_MISSINGOUTSTATETARGET, pState->getName ());

            auto pTargetState = pInstance->findState(targetAttrib.as_string (), false);
            if (pTargetState.get() == nullptr)
                throw ELibMCCustomException(LIBMC_ERROR_INVALIDOUTSTATETARGET, targetAttrib.as_string());

            pState->addOutState(pTargetState);
        }


    }


    pInstance->setInitState(sInitState);
    pInstance->setFailedState(sFailedState);
    if (!sSuccessState.empty()) {
        pInstance->setSuccessState(sSuccessState);
    }

    // load Plugin DLLs
    auto pPlugin = loadPlugin (m_pSystemState->getLibraryPath (slibraryName));
    LibMCPlugin::PStateFactory pStateFactory;
    try {
        pStateFactory = pPlugin->CreateStateFactory(sName);
    }
    catch (std::exception & E) {
        throw new ELibMCInterfaceException (LIBMC_ERROR_COULDNOTCREATESTATEFACTORY, E.what());
    }

    pInstance->setStateFactory(pStateFactory);
        
    if (m_InstanceList.size() >= MAXSTATEMACHINEINSTANCECOUNT)
        throw ELibMCNoContextException(LIBMC_ERROR_TOOMANYMACHINEINSTANCES);

    m_Instances.insert(std::make_pair(sName, pInstance));
    m_InstanceList.push_back(pInstance);

    return pInstance;
}


void CMCContext::readSignalParameters(const std::string& sSignalName, const pugi::xml_node& xmlNode, std::list<AMC::CStateSignalParameter>& Parameters, std::list<AMC::CStateSignalParameter>& Results)
{
    auto signalParameters = xmlNode.children("parameter");
    for (pugi::xml_node signalParameter : signalParameters) {
        auto parameterNameAttrib = signalParameter.attribute("name");
        if (parameterNameAttrib.empty())
            throw ELibMCCustomException(LIBMC_ERROR_MISSINGPARAMETERNAME, sSignalName);

        auto parameterTypeAttrib = signalParameter.attribute("type");
        if (parameterTypeAttrib.empty())
            throw ELibMCCustomException(LIBMC_ERROR_MISSINGPARAMETERTYPE, sSignalName + "/" + parameterNameAttrib.as_string ());

        Parameters.push_back(CStateSignalParameter(parameterNameAttrib.as_string(), parameterTypeAttrib.as_string(), true));
    }

    auto optionalParameters = xmlNode.children("optionalparameter");
    for (pugi::xml_node signalParameter : optionalParameters) {
        auto parameterNameAttrib = signalParameter.attribute("name");
        if (parameterNameAttrib.empty())
            throw ELibMCCustomException(LIBMC_ERROR_MISSINGPARAMETERNAME, sSignalName);

        auto parameterTypeAttrib = signalParameter.attribute("type");
        if (parameterTypeAttrib.empty())
            throw ELibMCCustomException(LIBMC_ERROR_MISSINGPARAMETERTYPE, sSignalName + "/" + parameterNameAttrib.as_string());

        Parameters.push_back(CStateSignalParameter(parameterNameAttrib.as_string(), parameterTypeAttrib.as_string(), false));
    }

    auto signalResults = xmlNode.children("result");
    for (pugi::xml_node signalResult : signalResults) {
        auto parameterNameAttrib = signalResult.attribute("name");
        if (parameterNameAttrib.empty())
            throw ELibMCCustomException(LIBMC_ERROR_MISSINGPARAMETERNAME, sSignalName);

        auto parameterTypeAttrib = signalResult.attribute("type");
        if (parameterTypeAttrib.empty())
            throw ELibMCCustomException(LIBMC_ERROR_MISSINGPARAMETERTYPE, sSignalName + "/" + parameterNameAttrib.as_string());

        Results.push_back(CStateSignalParameter(parameterNameAttrib.as_string(), parameterTypeAttrib.as_string(), true));
    }

    auto optionalResults = xmlNode.children("optionalresult");
    for (pugi::xml_node signalResult : optionalResults) {
        auto parameterNameAttrib = signalResult.attribute("name");
        if (parameterNameAttrib.empty())
            throw ELibMCCustomException(LIBMC_ERROR_MISSINGPARAMETERNAME, sSignalName);

        auto parameterTypeAttrib = signalResult.attribute("type");
        if (parameterTypeAttrib.empty())
            throw ELibMCCustomException(LIBMC_ERROR_MISSINGPARAMETERTYPE, sSignalName + "/" + parameterNameAttrib.as_string());

        Results.push_back(CStateSignalParameter(parameterNameAttrib.as_string(), parameterTypeAttrib.as_string(), false));
    }

}


void CMCContext::loadDriverParameterGroup(const pugi::xml_node& xmlNode, AMC::PParameterGroup pGroup)
{
    LibMCAssertNotNull (pGroup.get ())

    auto driverNameAttrib = xmlNode.attribute("driver");
    if (driverNameAttrib.empty())
        throw ELibMCCustomException(LIBMC_ERROR_MISSINGDRIVERNAME, pGroup->getName ());

    auto driverGroup = m_pSystemState->driverHandler()->getDriverParameterGroup(driverNameAttrib.as_string ());
    pGroup->addDerivativesFromGroup(driverGroup);

}


void CMCContext::loadParameterGroup(const pugi::xml_node& xmlNode, AMC::PParameterGroup pGroup)
{
    LibMCAssertNotNull(pGroup.get());

    auto parameterNodes = xmlNode.children("parameter");
    for (pugi::xml_node parameterNode : parameterNodes) {
        auto nameAttrib = parameterNode.attribute("name");
        if (nameAttrib.empty())
            throw ELibMCCustomException(LIBMC_ERROR_MISSINGPARAMETERNAME, pGroup->getName ());
        auto descriptionAttrib = parameterNode.attribute("description");
        if (descriptionAttrib.empty())
            throw ELibMCCustomException(LIBMC_ERROR_MISSINGPARAMETERDESCRIPTION, pGroup->getName () + "/" + nameAttrib.as_string ());
        auto defaultValueAttrib = parameterNode.attribute("default");
        if (defaultValueAttrib.empty())
            throw ELibMCCustomException(LIBMC_ERROR_MISSINGPARAMETERDEFAULTVALUE, pGroup->getName() + "/" + nameAttrib.as_string());
        auto typeAttrib = parameterNode.attribute("type");
        if (typeAttrib.empty())
            throw ELibMCCustomException(LIBMC_ERROR_MISSINGPARAMETERTYPE, pGroup->getName() + "/" + nameAttrib.as_string());
        auto unitsAttrib = parameterNode.attribute("units");
        auto persistentAttrib = parameterNode.attribute("persistent");

        std::string sName = nameAttrib.as_string();
        std::string sPersistentUUID = persistentAttrib.as_string();
       
        pGroup->addNewTypedParameter(sName, typeAttrib.as_string(), descriptionAttrib.as_string(), defaultValueAttrib.as_string(), unitsAttrib.as_string());
        if (!sPersistentUUID.empty())
            pGroup->setParameterPersistentUUID(sName, sPersistentUUID);
    }

}

void CMCContext::loadParameterGroupDerives(const pugi::xml_node& xmlNode, AMC::PParameterGroup pGroup, const std::string& sStateMachineInstance)
{
    LibMCAssertNotNull(pGroup.get ());

    auto parameterNodes = xmlNode.children("derivedparameter");
    for (pugi::xml_node parameterNode : parameterNodes) {
        auto nameAttrib = parameterNode.attribute("name");
        if (nameAttrib.empty())
            throw ELibMCCustomException(LIBMC_ERROR_MISSINGPARAMETERNAME, pGroup->getName ());

        AMC::PParameterHandler pParameterHandler;

        auto sourceStateMachineAttrib = parameterNode.attribute("statemachine");
        if (sourceStateMachineAttrib.empty()) {
            pParameterHandler = m_pSystemState->stateMachineData()->getParameterHandler(sStateMachineInstance);
        }
        else {
            pParameterHandler = m_pSystemState->stateMachineData()->getParameterHandler(sourceStateMachineAttrib.as_string());
        }


        std::string sSourceGroupName;
        auto groupAttrib = parameterNode.attribute("group");
        if (!groupAttrib.empty()) {
            sSourceGroupName = groupAttrib.as_string();
        }
        else {
            sSourceGroupName = pGroup->getName();
        }
   

        AMC::PParameterGroup pSourceGroup = pParameterHandler->findGroup (sSourceGroupName, true);

        std::string sSourceParameterName;
        auto sourceParameterAttrib = parameterNode.attribute("parameter");
        if (!sourceParameterAttrib.empty()) {
            sSourceParameterName = sourceParameterAttrib.as_string();
        }
        else {
            sSourceParameterName = nameAttrib.as_string();
        }

        pGroup->addNewDerivedParameter (nameAttrib.as_string(), pSourceGroup, sSourceParameterName);
    }

}



AMC::PStateMachineInstance CMCContext::findMachineInstance(std::string sName, bool bFailIfNotExisting)
{
    auto iter = m_Instances.find(sName);
    if (iter == m_Instances.end()) {
        if (bFailIfNotExisting)
            throw ELibMCCustomException(LIBMC_ERROR_MACHINEINSTANCENOTFOUND, sName);

        return nullptr;
    }

    return iter->second;
}


LibMCPlugin::PWrapper CMCContext::loadPlugin(std::string sPluginName)
{

    m_pSystemState->logger()->logMessage("loading plugin: " + sPluginName, LOG_SUBSYSTEM_SYSTEM, AMC::eLogLevel::Message);

    auto iPluginIter = m_Plugins.find(sPluginName);

    if (iPluginIter == m_Plugins.end()) {
        
        LibMCPlugin::PWrapper pPlugin;

        try {
            pPlugin = LibMCPlugin::CWrapper::loadLibrary(sPluginName);
            pPlugin->InjectComponent("LibMCEnv", (void*) LibMCEnv::Impl::LibMCEnv_GetProcAddress);

        } catch (std::exception& E) {
            throw ELibMCInterfaceException (LIBMC_ERROR_COULDNOTLOADPLUGIN, E.what ());
        }

        m_Plugins.insert(std::make_pair (sPluginName, pPlugin));

        return pPlugin;

    }
    else {
        return iPluginIter->second ;
    }

}


void CMCContext::StartAllThreads()
{
    m_pSystemState->logger()->logMessage("starting threads...", LOG_SUBSYSTEM_SYSTEM, AMC::eLogLevel::Message);
    for (auto instance : m_InstanceList)
        instance->startThread();
}

void CMCContext::TerminateAllThreads()
{
    m_pSystemState->logger()->logMessage("terminating threads...", LOG_SUBSYSTEM_SYSTEM, AMC::eLogLevel::Message);
    for (auto instance : m_InstanceList)
        instance->terminateThread();

}


void CMCContext::StartInstanceThread(const std::string& sInstanceName)
{
    if (sInstanceName.empty())
        throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDSTATEMACHINENAME);

    m_pSystemState->logger()->logMessage("starting thread " + sInstanceName + "...", LOG_SUBSYSTEM_SYSTEM, AMC::eLogLevel::Message);

    auto pInstance = findMachineInstance(sInstanceName, true);
    pInstance->startThread();

}

void CMCContext::TerminateInstanceThread(const std::string& sInstanceName) 
{
    if (sInstanceName.empty())
        throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDSTATEMACHINENAME);

    m_pSystemState->logger()->logMessage("starting thread " + sInstanceName + "...", LOG_SUBSYSTEM_SYSTEM, AMC::eLogLevel::Message);

    auto pInstance = findMachineInstance(sInstanceName, true);
    pInstance->terminateThread();
}

std::string CMCContext::GetInstanceThreadState(const std::string& sInstanceName)
{
    if (sInstanceName.empty())
        throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDSTATEMACHINENAME);

    auto pInstance = findMachineInstance(sInstanceName, true);
    return pInstance->getCurrentStateName ();

}

bool CMCContext::InstanceStateIsSuccessful(const std::string& sInstanceName)
{
    if (sInstanceName.empty())
        throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDSTATEMACHINENAME);

    auto pInstance = findMachineInstance(sInstanceName, true);
    return pInstance->currentStateIsSuccessState();
}

bool CMCContext::InstanceStateHasFailed(const std::string& sInstanceName)
{
    if (sInstanceName.empty())
        throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDSTATEMACHINENAME);

    auto pInstance = findMachineInstance(sInstanceName, true);
    return pInstance->currentStateIsFailureState();
}


void CMCContext::Log(const std::string& sMessage, const LibMC::eLogSubSystem eSubsystem, const LibMC::eLogLevel eLogLevel)
{
    std::string sSubSystem;

    switch (eSubsystem) {
    case LibMC::eLogSubSystem::Network: sSubSystem = LOG_SUBSYSTEM_NETWORK; break;
    case LibMC::eLogSubSystem::System: sSubSystem = LOG_SUBSYSTEM_SYSTEM; break;
    case LibMC::eLogSubSystem::Testing: sSubSystem = LOG_SUBSYSTEM_TESTING; break;
    default:
        sSubSystem = LOG_SUBSYSTEM_UNKNOWN;
    }

    m_pSystemState->logger()->logMessage (sMessage, sSubSystem, (AMC::eLogLevel) eLogLevel);
}

IAPIRequestHandler* CMCContext::CreateAPIRequestHandler(const std::string& sURI, const std::string& sRequestMethod, const std::string& sAuthorization)
{

    auto pSessionHandler = m_pAPI->getSessionHandler();
    auto requestType = m_pAPI->getRequestTypeFromString(sRequestMethod);
  
    PAPIAuth pAuth;
    if (sAuthorization.empty()) {

        bool bNeedsToBeAuthorized = true;
        bool bCreateNewSession = false;
        m_pAPI->checkAuthorizationMode(sURI, requestType, bNeedsToBeAuthorized, bCreateNewSession);

        if (bNeedsToBeAuthorized)
            throw ELibMCNoContextException(LIBMC_ERROR_INVALIDAUTHORIZATION);

        if (bCreateNewSession) {
            pAuth = pSessionHandler->createNewAuthenticationSession();
            LibMCAssertNotNull(pAuth);
            m_pSystemState->uiHandler()->populateClientVariables(pAuth->getClientVariableHandler().get());
        }
        else
            pAuth = pSessionHandler->createEmptyAuthenticationSession();

    }
    else {

        if (sAuthorization.length () < 7)
            throw ELibMCNoContextException(LIBMC_ERROR_INVALIDAUTHORIZATION);
        if (sAuthorization.substr (0, 7) != "Bearer ")
            throw ELibMCNoContextException(LIBMC_ERROR_INVALIDAUTHORIZATION);

        auto sAuthJSONString = AMCCommon::CUtils::decodeBase64ToASCIIString(sAuthorization.substr (7), AMCCommon::eBase64Type::URL);


        pAuth = pSessionHandler->createAuthentication(sAuthJSONString);
    }

     

    return new CAPIRequestHandler(m_pAPI, sURI, requestType, pAuth, m_pSystemState->getLoggerInstance ());

}

