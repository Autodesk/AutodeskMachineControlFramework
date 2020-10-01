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
#include "amc_parameterinstances.hpp"
#include "amc_logger_multi.hpp"
#include "amc_logger_stdout.hpp"
#include "amc_logger_database.hpp"
#include "amc_servicehandler.hpp"
#include "amc_ui_handler.hpp"
#include "amc_resourcepackage.hpp"

#include "amc_api_factory.hpp"
#include "amc_api_sessionhandler.hpp"

#include "common_importstream_native.hpp"


// Include custom headers here.
#include <iostream>

#define MACHINEDEFINITION_XMLSCHEMA "http://schemas.autodesk.com/amc/machinedefinitions/2020/02"

using namespace LibMC::Impl;
using namespace AMC;

/*************************************************************************************************************************
 Class definition of CMCContext 
**************************************************************************************************************************/

CMCContext::CMCContext(LibMCData::PDataModel pDataModel)
{
    if (pDataModel.get() == nullptr)
        throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

    m_pStateJournal = std::make_shared<CStateJournal> (std::make_shared<CStateJournalStream> ());

    m_pEnvironmentWrapper = LibMCEnv::CWrapper::loadLibraryFromSymbolLookupMethod((void*) LibMCEnv::Impl::LibMCEnv_GetProcAddress);

    // Create Log Multiplexer to StdOut and Database
    auto pMultiLogger = std::make_shared<AMC::CLogger_Multi>();
    pMultiLogger->addLogger(std::make_shared<AMC::CLogger_StdOut>());
    pMultiLogger->addLogger(std::make_shared<AMC::CLogger_Database> (pDataModel->CreateNewLogSession ()));

    // Create system state
    m_pSystemState = std::make_shared <CSystemState> (pMultiLogger, pDataModel, m_pEnvironmentWrapper);

    // Create API Handlers for data model requests
    m_pAPI = std::make_shared<AMC::CAPI>();
    CAPIFactory factory (m_pAPI, m_pSystemState, m_InstanceList);

    // Create Client Dist Handler
    m_pClientDistHandler = std::make_shared <CAPIHandler_Root>();
    m_pAPI->registerHandler (m_pClientDistHandler);
}



void CMCContext::ParseConfiguration(const std::string & sXMLString)
{

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string (sXMLString.c_str ());
    if (!result)
        throw ELibMCInterfaceException (LIBMC_ERROR_COULDNOTPARSECONFIGURATION);

     
    auto machinedefinitionNode = doc.child("machinedefinition");
    if (machinedefinitionNode.empty ())
        throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGMACHINEDEFINITION);

    auto xmlnsAttrib = machinedefinitionNode.attribute("xmlns");
    if (xmlnsAttrib.empty ())
        throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGXMLSCHEMA);

    std::string xmlns(xmlnsAttrib.as_string ());
    if (xmlns != MACHINEDEFINITION_XMLSCHEMA)
        throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDXMLSCHEMA);

    auto servicesNode = machinedefinitionNode.child("services");
    if (servicesNode.empty ())
        throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGSERVICESNODE);

    auto threadCountAttrib = servicesNode.attribute("threadcount");
    if (threadCountAttrib.empty())
        throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGTHREADCOUNT);
    auto nMaxThreadCount = threadCountAttrib.as_uint();
    if ((nMaxThreadCount < SERVICETHREADCOUNT_MIN) || (nMaxThreadCount > SERVICETHREADCOUNT_MAX))
        throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDTHREADCOUNT);
    m_pSystemState->serviceHandler()->setMaxThreadCount((uint32_t) nMaxThreadCount);



    m_pSystemState->logger()->logMessage("Loading drivers...", LOG_SUBSYSTEM_SYSTEM, AMC::eLogLevel::Message);
    auto driversNodes = machinedefinitionNode.children("driver");
    for (pugi::xml_node driversNode : driversNodes)
    {
        addDriver(driversNode);        
    }


    m_pSystemState->logger()->logMessage("Initializing state machines...", LOG_SUBSYSTEM_SYSTEM, AMC::eLogLevel::Message);
    auto statemachinesNodes = machinedefinitionNode.children("statemachine");
    for (pugi::xml_node instanceNode : statemachinesNodes)
    {
        addMachineInstance(instanceNode);
    }

    // Load User Interface
    auto userInterfaceNode = machinedefinitionNode.child("userinterface");
    if (userInterfaceNode.empty())
        throw ELibMCInterfaceException(LIBMC_ERROR_NOUSERINTERFACEDEFINITION);
    m_pSystemState->uiHandler()->loadFromXML(userInterfaceNode);


    m_pStateJournal->startRecording();

}


void CMCContext::RegisterLibraryPath(const std::string& sLibraryName, const std::string& sLibraryPath, const std::string& sLibraryResource)
{
    m_pSystemState->logger()->logMessage("mapping " + sLibraryName + " to " + sLibraryPath + "...", LOG_SUBSYSTEM_SYSTEM, AMC::eLogLevel::Message);
    m_pSystemState->addLibraryPath(sLibraryName, sLibraryPath, sLibraryResource);
    
}


void CMCContext::LoadClientPackage(const std::string& sResourcePath)
{
    auto pStream = std::make_shared<AMCCommon::CImportStream_Native>(sResourcePath);
    auto pPackage = CResourcePackage::makeFromStream(pStream);

    m_pClientDistHandler->LoadClientPackage (pPackage);
}


void CMCContext::addDriver(const pugi::xml_node& xmlNode)
{
    auto nameAttrib = xmlNode.attribute("name");
    if (nameAttrib.empty())
        throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGDRIVERNAME);
    std::string sName(nameAttrib.as_string());


    auto typeAttrib = xmlNode.attribute("type");
    if (typeAttrib.empty())
        throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGDRIVERTYPE);
    std::string sType(typeAttrib.as_string());

    auto libraryAttrib = xmlNode.attribute("library");
    if (libraryAttrib.empty())
        throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGDRIVERLIBRARY);
    std::string sLibraryName(libraryAttrib.as_string());
    
    m_pSystemState->logger()->logMessage("Initializing " + sName + " (" + sType + "@" + sLibraryName + ")", LOG_SUBSYSTEM_SYSTEM, AMC::eLogLevel::Message);

    try {
        m_pSystemState->driverHandler()->registerDriver(sName, sType, m_pSystemState->getLibraryPath(sLibraryName));
    } 
    catch (std::exception & E) {
        m_pSystemState->logger()->logMessage(std::string ("Driver error: ") + E.what(), LOG_SUBSYSTEM_SYSTEM, AMC::eLogLevel::FatalError);
    }

}

AMC::PStateMachineInstance CMCContext::addMachineInstance(const pugi::xml_node& xmlNode)
{
    auto nameAttrib = xmlNode.attribute("name");
    if (nameAttrib.empty())
        throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGINSTANCENAME);
    std::string sName (nameAttrib.as_string());

    auto descriptionAttrib = xmlNode.attribute("description");
    if (descriptionAttrib.empty())
        throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGINSTANCEDESCRIPTION);
    std::string sDescription(descriptionAttrib.as_string());

    auto initstateAttrib = xmlNode.attribute("initstate");
    if (initstateAttrib.empty())
        throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGINITSTATE);
    std::string sInitState(initstateAttrib.as_string());
    if (sInitState.length() == 0)
        throw ELibMCInterfaceException(LIBMC_ERROR_EMPTYINITSTATE);

    auto failedstateAttrib = xmlNode.attribute("failedstate");
    if (failedstateAttrib.empty())
        throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGFAILEDSTATE);
    std::string sFailedState(failedstateAttrib.as_string());
    if (sFailedState.length() == 0)
        throw ELibMCInterfaceException(LIBMC_ERROR_EMPTYFAILEDSTATE);

    auto libraryAttrib = xmlNode.attribute("library");
    if (libraryAttrib.empty())
        throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGPLUGINNAME);
    std::string slibraryName (libraryAttrib.as_string());
    if (slibraryName.length() == 0)
        throw ELibMCInterfaceException(LIBMC_ERROR_EMPTYPLUGINNAME);


    auto pInstance = findMachineInstance(sName, false);
    if (pInstance.get() != nullptr)
        throw ELibMCInterfaceException(LIBMC_ERROR_DUPLICATESTATENAME);
    
    m_pSystemState->logger()->logMessage("Creating state machine \"" + sName + "\"", LOG_SUBSYSTEM_SYSTEM, AMC::eLogLevel::Message);
    pInstance = std::make_shared<CStateMachineInstance> (sName, sDescription, m_pEnvironmentWrapper, m_pSystemState, m_pStateJournal);

    auto signalNodes = xmlNode.children("signaldefinition");
    for (pugi::xml_node signalNode : signalNodes) {
        auto signalNameAttrib = signalNode.attribute("name");
        if (signalNameAttrib.empty())
            throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGSIGNALNAME);

        std::list<CStateSignalParameter> SignalParameters;
        std::list<CStateSignalParameter> SignalResults;

        readSignalParameters(signalNode, SignalParameters, SignalResults);

        m_pSystemState->stateSignalHandler()->addSignalDefinition(sName, signalNameAttrib.as_string(), SignalParameters, SignalResults);

    }


    auto pParameterHandler = pInstance->getParameterHandler();

    // Load all value parameters
    auto parameterGroupNodes = xmlNode.children("parametergroup");
    for (pugi::xml_node parameterGroupNode : parameterGroupNodes) {
        auto groupNameAttrib = parameterGroupNode.attribute("name");
        if (groupNameAttrib.empty())
            throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGPARAMETERGROUPNAME);
        auto groupDescriptionAttrib = parameterGroupNode.attribute("description");
        if (groupDescriptionAttrib.empty())
            throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGPARAMETERGROUPDESCRIPTION);

        auto pGroup = pParameterHandler->addGroup(groupNameAttrib.as_string(), groupDescriptionAttrib.as_string());
        pGroup->setJournal(m_pStateJournal, sName);
        loadParameterGroup(parameterGroupNode, pGroup);
    }

    // Load all driver parameters
    auto driverParameterGroupNodes = xmlNode.children("driverparametergroup");
    for (pugi::xml_node driverParameterGroupNode : driverParameterGroupNodes) {
        auto groupNameAttrib = driverParameterGroupNode.attribute("name");
        if (groupNameAttrib.empty())
            throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGPARAMETERGROUPNAME);
        auto groupDescriptionAttrib = driverParameterGroupNode.attribute("description");
        if (groupDescriptionAttrib.empty())
            throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGPARAMETERGROUPDESCRIPTION);

        auto pGroup = pParameterHandler->addGroup(groupNameAttrib.as_string(), groupDescriptionAttrib.as_string());
        pGroup->setJournal(m_pStateJournal, sName);
        loadDriverParameterGroup(driverParameterGroupNode, pGroup);
    }


    // Load all derived parameters
    for (pugi::xml_node parameterGroupNode : parameterGroupNodes) {
        auto groupNameAttrib = parameterGroupNode.attribute("name");
        if (groupNameAttrib.empty())
            throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGPARAMETERGROUPNAME);

        auto pGroup = pParameterHandler->findGroup(groupNameAttrib.as_string(), true);
        loadParameterGroupDerives(parameterGroupNode, pGroup, sName);
    }


    auto statesNodes = xmlNode.children("state");
    for (pugi::xml_node stateNode : statesNodes)
    {

        auto stateNameAttrib = stateNode.attribute("name");
        if (stateNameAttrib.empty())
            throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGSTATENAME);

        auto repeatDelayAttrib = stateNode.attribute("repeatdelay");
        if (repeatDelayAttrib.empty())
            throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGREPEATDELAY);

        auto pState = pInstance->addState(stateNameAttrib.as_string(), repeatDelayAttrib.as_int());
        

    }


    // Load Out States
    for (pugi::xml_node stateNode : statesNodes)
    {
        auto stateNameAttrib = stateNode.attribute("name");
        if (stateNameAttrib.empty())
            throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGSTATENAME);

        auto pState = pInstance->findState(stateNameAttrib.as_string(), true);

        auto outstateNodes = stateNode.children("outstate");
        for (pugi::xml_node outstateNode : outstateNodes) {
            auto targetAttrib = outstateNode.attribute("target");
            if (targetAttrib.empty())
                throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGOUTSTATETARGET);

            auto pTargetState = pInstance->findState(targetAttrib.as_string (), false);
            if (pTargetState.get() == nullptr)
                throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDOUTSTATETARGET);

            pState->addOutState(pTargetState);
        }


    }


    pInstance->setInitState(sInitState);
    pInstance->setFailedState(sFailedState);

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
        throw ELibMCInterfaceException(LIBMC_ERROR_TOOMANYMACHINEINSTANCES);

    m_Instances.insert(std::make_pair(sName, pInstance));
    m_InstanceList.push_back(pInstance);

    return pInstance;
}


void CMCContext::readSignalParameters(const pugi::xml_node& xmlNode, std::list<AMC::CStateSignalParameter>& Parameters, std::list<AMC::CStateSignalParameter>& Results)
{
    auto signalParameters = xmlNode.children("parameter");
    for (pugi::xml_node signalParameter : signalParameters) {
        auto parameterNameAttrib = signalParameter.attribute("name");
        if (parameterNameAttrib.empty())
            throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGPARAMETERNAME);

        auto parameterTypeAttrib = signalParameter.attribute("type");
        if (parameterTypeAttrib.empty())
            throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGPARAMETERTYPE);

        Parameters.push_back(CStateSignalParameter(parameterNameAttrib.as_string(), parameterTypeAttrib.as_string(), true));
    }

    auto optionalParameters = xmlNode.children("optionalparameter");
    for (pugi::xml_node signalParameter : optionalParameters) {
        auto parameterNameAttrib = signalParameter.attribute("name");
        if (parameterNameAttrib.empty())
            throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGPARAMETERNAME);

        auto parameterTypeAttrib = signalParameter.attribute("type");
        if (parameterTypeAttrib.empty())
            throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGPARAMETERTYPE);

        Parameters.push_back(CStateSignalParameter(parameterNameAttrib.as_string(), parameterTypeAttrib.as_string(), false));
    }

    auto signalResults = xmlNode.children("result");
    for (pugi::xml_node signalResult : signalResults) {
        auto parameterNameAttrib = signalResult.attribute("name");
        if (parameterNameAttrib.empty())
            throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGPARAMETERNAME);

        auto parameterTypeAttrib = signalResult.attribute("type");
        if (parameterTypeAttrib.empty())
            throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGPARAMETERTYPE);

        Results.push_back(CStateSignalParameter(parameterNameAttrib.as_string(), parameterTypeAttrib.as_string(), true));
    }

    auto optionalResults = xmlNode.children("optionalresult");
    for (pugi::xml_node signalResult : optionalResults) {
        auto parameterNameAttrib = signalResult.attribute("name");
        if (parameterNameAttrib.empty())
            throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGPARAMETERNAME);

        auto parameterTypeAttrib = signalResult.attribute("type");
        if (parameterTypeAttrib.empty())
            throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGPARAMETERTYPE);

        Results.push_back(CStateSignalParameter(parameterNameAttrib.as_string(), parameterTypeAttrib.as_string(), false));
    }

}


void CMCContext::loadDriverParameterGroup(const pugi::xml_node& xmlNode, AMC::PParameterGroup pGroup)
{
    if (pGroup.get() == nullptr)
        throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

    auto driverNameAttrib = xmlNode.attribute("driver");
    if (driverNameAttrib.empty())
        throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGDRIVERNAME);

    auto driverGroup = m_pSystemState->driverHandler()->getDriverParameterGroup(driverNameAttrib.as_string ());
    driverGroup->copyToGroup(pGroup.get());

}


void CMCContext::loadParameterGroup(const pugi::xml_node& xmlNode, AMC::PParameterGroup pGroup)
{
    if (pGroup.get() == nullptr)
        throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

    auto parameterNodes = xmlNode.children("parameter");
    for (pugi::xml_node parameterNode : parameterNodes) {
        auto nameAttrib = parameterNode.attribute("name");
        if (nameAttrib.empty())
            throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGPARAMETERNAME);
        auto descriptionAttrib = parameterNode.attribute("description");
        if (descriptionAttrib.empty())
            throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGPARAMETERDESCRIPTION);
        auto defaultValueAttrib = parameterNode.attribute("default");
        if (defaultValueAttrib.empty())
            throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGPARAMETERDEFAULTVALUE);
        auto typeAttrib = parameterNode.attribute("type");
        if (typeAttrib.empty())
            throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGPARAMETERTYPE);
        auto unitsAttrib = parameterNode.attribute("units");

        pGroup->addNewTypedParameter(nameAttrib.as_string(), typeAttrib.as_string(), descriptionAttrib.as_string(), defaultValueAttrib.as_string(), unitsAttrib.as_string());
    }

}

void CMCContext::loadParameterGroupDerives(const pugi::xml_node& xmlNode, AMC::PParameterGroup pGroup, const std::string& sStateMachineInstance)
{
    if (pGroup.get() == nullptr)
        throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

    auto parameterNodes = xmlNode.children("derivedparameter");
    for (pugi::xml_node parameterNode : parameterNodes) {
        auto nameAttrib = parameterNode.attribute("name");
        if (nameAttrib.empty())
            throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGPARAMETERNAME);

        AMC::PParameterHandler pParameterHandler;

        auto sourceStateMachineAttrib = parameterNode.attribute("statemachine");
        if (sourceStateMachineAttrib.empty()) {
            pParameterHandler = m_pSystemState->parameterInstances()->getParameterHandler(sStateMachineInstance);
        }
        else {
            pParameterHandler = m_pSystemState->parameterInstances()->getParameterHandler(sourceStateMachineAttrib.as_string());
        }

        auto groupAttrib = parameterNode.attribute("group");
        if (groupAttrib.empty())
            throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGPARAMETERGROUPNAME);
        AMC::PParameterGroup pSourceGroup = pParameterHandler->findGroup (groupAttrib.as_string(), true);

        auto sourceParameterAttrib = parameterNode.attribute("parameter");
        if (sourceParameterAttrib.empty())
            throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGPARAMETERNAME);

        pGroup->addNewDerivedParameter (nameAttrib.as_string(), pSourceGroup, sourceParameterAttrib.as_string ());
    }

}



AMC::PStateMachineInstance CMCContext::findMachineInstance(std::string sName, bool bFailIfNotExisting)
{
    auto iter = m_Instances.find(sName);
    if (iter == m_Instances.end()) {
        if (bFailIfNotExisting)
            throw ELibMCInterfaceException(LIBMC_ERROR_DUPLICATEMACHINEINSTANCE);

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

void CMCContext::Log(const std::string& sMessage, const LibMC::eLogSubSystem eSubsystem, const LibMC::eLogLevel eLogLevel)
{
    std::string sSubSystem;

    switch (eSubsystem) {
    case LibMC::eLogSubSystem::Network: sSubSystem = LOG_SUBSYSTEM_NETWORK; break;
    case LibMC::eLogSubSystem::System: sSubSystem = LOG_SUBSYSTEM_SYSTEM; break;
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
            throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDAUTHORIZATION);

        if (bCreateNewSession)
            pAuth = pSessionHandler->createNewAuthenticationSession ();
        else
            pAuth = pSessionHandler->createEmptyAuthenticationSession();

    }
    else {

        if (sAuthorization.length () < 7)
            throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDAUTHORIZATION);
        if (sAuthorization.substr (0, 7) != "Bearer ")
            throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDAUTHORIZATION);

        auto sAuthJSONString = AMCCommon::CUtils::decodeBase64ToASCIIString(sAuthorization.substr (7), AMCCommon::eBase64Type::URL);


        pAuth = pSessionHandler->createAuthentication(sAuthJSONString);
    }

     

    return new CAPIRequestHandler(m_pAPI, sURI, requestType, pAuth);

}

