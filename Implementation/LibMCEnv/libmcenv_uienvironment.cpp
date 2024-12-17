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

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL AUTODESK INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


Abstract: This is a stub class definition of CUIEnvironment

*/

#include "libmcenv_uienvironment.hpp"
#include "libmcenv_interfaceexception.hpp"
#include "libmcenv_xmldocument.hpp"
#include "libmcenv_discretefielddata2d.hpp"
#include "libmcenv_usermanagementhandler.hpp"
#include "libmcenv_journalhandler_current.hpp"
#include "libmcenv_dataseries.hpp"
#include "libmcenv_datetime.hpp"
#include "libmcenv_scenehandler.hpp"
#include "libmcenv_meshobject.hpp"
#include "libmcenv_modeldatacomponentinstance.hpp"
#include "libmcenv_persistentmeshobject.hpp"
#include "libmcenv_alert.hpp"
#include "libmcenv_alertiterator.hpp"
#include "libmcenv_cryptocontext.hpp"
#include "libmcenv_tempstreamwriter.hpp"
#include "libmcenv_zipstreamwriter.hpp"
#include "libmcenv_imageloader.hpp"

#include "amc_systemstate.hpp"
#include "amc_accesscontrol.hpp"
#include "amc_meshhandler.hpp"
#include "amc_dataserieshandler.hpp"
#include "amc_alerthandler.hpp"
#include "libmcenv_signaltrigger.hpp"
#include "libmcenv_imagedata.hpp"
#include "libmcenv_testenvironment.hpp"
#include "libmcenv_build.hpp"
#include "libmcenv_buildexecution.hpp"
#include "libmcenv_streamreader.hpp"
#include "libmcenv_datatable.hpp"

#include "amc_toolpathhandler.hpp"
#include "amc_logger.hpp"
#include "amc_statemachinedata.hpp"
#include "amc_ui_handler.hpp"
#include "libmcdata_dynamic.hpp"

// Include custom headers here.
#include "common_utils.hpp"

#include <cmath>

#define DOWNLOADFILENAME_MAXLENGTH 256

using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CUIEnvironment 
**************************************************************************************************************************/

uint32_t colorRGBtoInteger(const LibMCEnv::sColorRGB Color)
{
    int32_t nColorRed = (int32_t)round(Color.m_Red * 255.0);
    if (nColorRed < 0)
        nColorRed = 0;
    if (nColorRed > 255)
        nColorRed = 255;

    int32_t nColorGreen = (int32_t)round(Color.m_Green * 255.0);
    if (nColorGreen < 0)
        nColorGreen = 0;
    if (nColorGreen > 255)
        nColorGreen = 255;

    int32_t nColorBlue = (int32_t)round(Color.m_Blue * 255.0);
    if (nColorBlue < 0)
        nColorBlue = 0;
    if (nColorBlue > 255)
        nColorBlue = 255;

    return (uint32_t)nColorRed + (uint32_t)nColorGreen * 256 + (uint32_t)nColorBlue * 65536;
}


CUIEnvironment::CUIEnvironment(AMC::CUIHandler* pUIHandler, const std::string& sSenderUUID, const std::string& sSenderName, AMC::PAPIAuth pAPIAuth, const std::string& sTestEnvironmentPath)
    : 
      m_pUIHandler (pUIHandler),
      m_sLogSubSystem ("ui"),
      m_sSenderName (sSenderName),
      m_pAPIAuth (pAPIAuth)
{

    if (pAPIAuth.get () == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
    if (pUIHandler == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

    m_pUISystemState = pUIHandler->getUISystemState();
    m_pLogger = m_pUISystemState->getLogger();

    if (!sSenderUUID.empty()) {
        m_sSenderUUID = (AMCCommon::CUtils::normalizeUUIDString(sSenderUUID));
    }
    else {
        m_sSenderUUID = AMCCommon::CUtils::createEmptyUUID();
    }

}

CUIEnvironment::~CUIEnvironment()
{

}

void CUIEnvironment::ActivateModalDialog(const std::string& sDialogName)
{
    m_ClientActions.push_back(std::make_shared<AMC::CUIClientAction_ActivateModalDialog> (sDialogName));
}

void CUIEnvironment::CloseModalDialog()
{
    m_ClientActions.push_back(std::make_shared<AMC::CUIClientAction_CloseModalDialog>());
}

void CUIEnvironment::ActivatePage(const std::string& sPageName)
{
    m_ClientActions.push_back(std::make_shared<AMC::CUIClientAction_ActivatePage>(sPageName));
}

void CUIEnvironment::StartStreamDownload(const std::string& sUUID, const std::string& sFilename)
{
    std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);
    auto pDataModel = m_pUISystemState->getDataModel ();
    auto pStorage = pDataModel->CreateStorage ();

    if (!pStorage->StreamIsReady(sNormalizedUUID))
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_DOWNLOADSTREAMDOESNOTEXIST, "Download stream does not exist: " + sNormalizedUUID);

    if (sFilename.empty ())
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_EMPTYDOWNLOADSTREAMFILENAME);

    if (sFilename.size () > DOWNLOADFILENAME_MAXLENGTH)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDDOWNLOADSTREAMFILENAME);

    if (!m_pAPIAuth->userIsAuthorized ())
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_USERISNOTAUTHORIZED);

    std::string sUserUUID = m_pAPIAuth->getUserUUID();

    auto pGlobalChrono = m_pUISystemState->getGlobalChronoInstance();

    std::string sTicketUUID = m_pAPIAuth->createStreamDownloadTicket (sNormalizedUUID, sFilename);
    pStorage->CreateDownloadTicket (sTicketUUID, sNormalizedUUID, sFilename, m_pAPIAuth->getSessionUUID (), sUserUUID, pGlobalChrono->getUTCTimeStampInMicrosecondsSince1970 ());

    m_ClientActions.push_back(std::make_shared<AMC::CUIClientAction_StreamDownload>(sTicketUUID, sFilename));
}



std::string CUIEnvironment::RetrieveEventSender()
{
    return m_sSenderName;
}

std::string CUIEnvironment::RetrieveEventSenderPage()
{
    std::vector<std::string> sPathNames;
    AMCCommon::CUtils::splitString(m_sSenderName, ".", sPathNames);

    if (sPathNames.empty())
        return "";

    return sPathNames.at(0);
}


std::string CUIEnvironment::RetrieveEventSenderUUID()
{
    return m_sSenderUUID;
}


ISignalTrigger * CUIEnvironment::PrepareSignal(const std::string & sMachineInstance, const std::string & sSignalName)
{
    if (!m_pUISystemState->getSignalHandler()->hasSignalDefinition(sMachineInstance, sSignalName))
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNOTFINDSIGNALDEFINITON);

    return new CSignalTrigger(m_pUISystemState->getSignalHandler(), sMachineInstance, sSignalName, m_pUISystemState->getGlobalChronoInstance ());
}

std::string CUIEnvironment::GetMachineState(const std::string & sMachineInstance)
{
    return m_pUISystemState->getStateMachineData()->getInstanceStateName (sMachineInstance);
}

void CUIEnvironment::LogMessage(const std::string & sLogString)
{	
    m_pLogger->logMessage(sLogString, m_sLogSubSystem, AMC::eLogLevel::Message);
}

void CUIEnvironment::LogWarning(const std::string & sLogString)
{	
    m_pLogger->logMessage(sLogString, m_sLogSubSystem, AMC::eLogLevel::Warning);
}

void CUIEnvironment::LogInfo(const std::string & sLogString)
{	
    m_pLogger->logMessage(sLogString, m_sLogSubSystem, AMC::eLogLevel::Info);
}



std::string CUIEnvironment::GetMachineParameter(const std::string& sMachineInstance, const std::string& sParameterGroup, const std::string& sParameterName)
{
    auto pParameterHandler = m_pUISystemState->getStateMachineData ()->getParameterHandler(sMachineInstance);
    auto pGroup = pParameterHandler->findGroup(sParameterGroup, true);
    return pGroup->getParameterValueByName(sParameterName);
}

std::string CUIEnvironment::GetMachineParameterAsUUID(const std::string& sMachineInstance, const std::string& sParameterGroup, const std::string& sParameterName)
{
    auto pParameterHandler = m_pUISystemState->getStateMachineData()->getParameterHandler(sMachineInstance);
    auto pGroup = pParameterHandler->findGroup(sParameterGroup, true);
    return pGroup->getParameterValueByName(sParameterName);
}

LibMCEnv_double CUIEnvironment::GetMachineParameterAsDouble(const std::string& sMachineInstance, const std::string& sParameterGroup, const std::string& sParameterName)
{
    auto pParameterHandler = m_pUISystemState->getStateMachineData()->getParameterHandler(sMachineInstance);
    auto pGroup = pParameterHandler->findGroup(sParameterGroup, true);
    return pGroup->getDoubleParameterValueByName(sParameterName);
}

LibMCEnv_int64 CUIEnvironment::GetMachineParameterAsInteger(const std::string& sMachineInstance, const std::string& sParameterGroup, const std::string& sParameterName)
{
    auto pParameterHandler = m_pUISystemState->getStateMachineData()->getParameterHandler(sMachineInstance);
    auto pGroup = pParameterHandler->findGroup(sParameterGroup, true);
    return pGroup->getIntParameterValueByName(sParameterName);
}

bool CUIEnvironment::GetMachineParameterAsBool(const std::string& sMachineInstance, const std::string& sParameterGroup, const std::string& sParameterName)
{
    auto pParameterHandler = m_pUISystemState->getStateMachineData()->getParameterHandler(sMachineInstance);
    auto pGroup = pParameterHandler->findGroup(sParameterGroup, true);
    return pGroup->getBoolParameterValueByName(sParameterName);
}


std::string CUIEnvironment::GetUIProperty(const std::string& sElementPath, const std::string& sPropertyName) 
{   
    auto pClientVariableHandler = m_pAPIAuth->getClientVariableHandler();
    if (pClientVariableHandler.get() == nullptr)
        throw ELibMCEnvInterfaceException (LIBMCENV_ERROR_COULDNNOTACCESSCLIENTVARIABLES);

    auto pGroup = pClientVariableHandler->findGroup(sElementPath, true);
    return pGroup->getParameterValueByName(sPropertyName);
}

std::string CUIEnvironment::GetUIPropertyAsUUID(const std::string& sElementPath, const std::string& sPropertyName)
{
    auto pClientVariableHandler = m_pAPIAuth->getClientVariableHandler();
    if (pClientVariableHandler.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNNOTACCESSCLIENTVARIABLES);

    auto pGroup = pClientVariableHandler->findGroup(sElementPath, true);
    return pGroup->getParameterValueByName(sPropertyName);

}

LibMCEnv_double CUIEnvironment::GetUIPropertyAsDouble(const std::string& sElementPath, const std::string& sPropertyName)
{
    auto pClientVariableHandler = m_pAPIAuth->getClientVariableHandler();
    if (pClientVariableHandler.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNNOTACCESSCLIENTVARIABLES);

    auto pGroup = pClientVariableHandler->findGroup(sElementPath, true);
    return pGroup->getDoubleParameterValueByName(sPropertyName);
}

LibMCEnv_int64 CUIEnvironment::GetUIPropertyAsInteger(const std::string& sElementPath, const std::string& sPropertyName)
{
    auto pClientVariableHandler = m_pAPIAuth->getClientVariableHandler();
    if (pClientVariableHandler.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNNOTACCESSCLIENTVARIABLES);

    auto pGroup = pClientVariableHandler->findGroup(sElementPath, true);
    return pGroup->getIntParameterValueByName(sPropertyName);
}

bool CUIEnvironment::GetUIPropertyAsBool(const std::string& sElementPath, const std::string& sPropertyName)
{
    auto pClientVariableHandler = m_pAPIAuth->getClientVariableHandler();
    if (pClientVariableHandler.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNNOTACCESSCLIENTVARIABLES);

    auto pGroup = pClientVariableHandler->findGroup(sElementPath, true);
    return pGroup->getBoolParameterValueByName(sPropertyName);
}

void CUIEnvironment::SetUIProperty(const std::string& sElementPath, const std::string& sPropertyName, const std::string& sValue)
{
    auto pClientVariableHandler = m_pAPIAuth->getClientVariableHandler();
    if (pClientVariableHandler.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNNOTACCESSCLIENTVARIABLES);

    auto pGroup = pClientVariableHandler->findGroup(sElementPath, true);
    pGroup->setParameterValueByName(sPropertyName, sValue);
}

void CUIEnvironment::SetUIPropertyAsUUID(const std::string& sElementPath, const std::string& sPropertyName, const std::string& sValue)
{
    auto pClientVariableHandler = m_pAPIAuth->getClientVariableHandler();
    if (pClientVariableHandler.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNNOTACCESSCLIENTVARIABLES);

    auto pGroup = pClientVariableHandler->findGroup(sElementPath, true);
    if (!sValue.empty())
        pGroup->setParameterValueByName(sPropertyName, AMCCommon::CUtils::normalizeUUIDString (sValue));
    else
        pGroup->setParameterValueByName(sPropertyName, AMCCommon::CUtils::createEmptyUUID());
}

void CUIEnvironment::SetUIPropertyAsDouble(const std::string& sElementPath, const std::string& sPropertyName, const LibMCEnv_double dValue) 
{
    auto pClientVariableHandler = m_pAPIAuth->getClientVariableHandler();
    if (pClientVariableHandler.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNNOTACCESSCLIENTVARIABLES);

    auto pGroup = pClientVariableHandler->findGroup(sElementPath, true);
    pGroup->setDoubleParameterValueByName(sPropertyName, dValue);

}

void CUIEnvironment::SetUIPropertyAsInteger(const std::string& sElementPath, const std::string& sPropertyName, const LibMCEnv_int64 nValue) 
{
    auto pClientVariableHandler = m_pAPIAuth->getClientVariableHandler();
    if (pClientVariableHandler.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNNOTACCESSCLIENTVARIABLES);

    auto pGroup = pClientVariableHandler->findGroup(sElementPath, true);
    pGroup->setIntParameterValueByName(sPropertyName, nValue);
}

void CUIEnvironment::SetUIPropertyAsBool(const std::string& sElementPath, const std::string& sPropertyName, const bool bValue)
{
    auto pClientVariableHandler = m_pAPIAuth->getClientVariableHandler();
    if (pClientVariableHandler.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNNOTACCESSCLIENTVARIABLES);

    auto pGroup = pClientVariableHandler->findGroup(sElementPath, true);
    pGroup->setBoolParameterValueByName(sPropertyName, bValue);
}



IImageData* CUIEnvironment::CreateEmptyImage(const LibMCEnv_uint32 nPixelSizeX, const LibMCEnv_uint32 nPixelSizeY, const LibMCEnv_double dDPIValueX, const LibMCEnv_double dDPIValueY, const LibMCEnv::eImagePixelFormat ePixelFormat)
{
    return CImageData::createEmpty(nPixelSizeX, nPixelSizeY, dDPIValueX, dDPIValueY, ePixelFormat);
}

IImageLoader* CUIEnvironment::CreateImageLoader()
{
    return new CImageLoader();
}


LibMCEnv_uint64 CUIEnvironment::GetGlobalTimerInMilliseconds()
{
    return GetGlobalTimerInMicroseconds () / 1000ULL;
}

LibMCEnv_uint64 CUIEnvironment::GetGlobalTimerInMicroseconds()
{
    auto pGlobalChrono = m_pUISystemState->getGlobalChronoInstance();
    uint64_t nStartTime = pGlobalChrono->getStartTimeStampInMicrosecondsSince1970();
    uint64_t nCurrentTime = pGlobalChrono->getUTCTimeStampInMicrosecondsSince1970();
    
    if (nCurrentTime < nStartTime)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_GLOBALTIMERNOTCONTINUOUS);

    return nCurrentTime - nStartTime;
}

void CUIEnvironment::LogOut()
{
    m_ClientActions.push_back(std::make_shared<AMC::CUIClientAction_Logout>());
}

void CUIEnvironment::ShowHint(const std::string& sHint, const LibMCEnv_uint32 nTimeoutInMS)
{
    m_ClientActions.push_back(std::make_shared<AMC::CUIClientAction_ShowHint>(sHint, nTimeoutInMS, -1, -1));
}


void CUIEnvironment::ShowHintColored(const std::string& sHint, const LibMCEnv_uint32 nTimeoutInMS, const LibMCEnv::sColorRGB Color, const LibMCEnv::sColorRGB FontColor)
{

    int32_t nColorValue = (int32_t)colorRGBtoInteger (Color);
    int32_t nFontColorValue = (int32_t)colorRGBtoInteger(FontColor);

    m_ClientActions.push_back(std::make_shared<AMC::CUIClientAction_ShowHint>(sHint, nTimeoutInMS, nColorValue, nFontColorValue));
}

void CUIEnvironment::HideHint()
{
    m_ClientActions.push_back(std::make_shared<AMC::CUIClientAction_HideHint>());
}

std::string CUIEnvironment::ShowMessageDlg(const std::string& sCaption, const std::string& sTitle, const LibMCEnv::eMessageDialogType eDialogType, const std::string& sYesEvent, const std::string& sNoEvent, const std::string& sCancelEvent)
{
    std::string sDialogType;
    std::string sDialogUUID = AMCCommon::CUtils::createUUID();

    switch (eDialogType) {
    case LibMCEnv::eMessageDialogType::DialogOK:
        sDialogType = "ok"; break;
    case LibMCEnv::eMessageDialogType::DialogOKCancel:
        sDialogType = "okcancel"; break;
    case LibMCEnv::eMessageDialogType::DialogYesNo:
        sDialogType = "yesno"; break;
    case LibMCEnv::eMessageDialogType::DialogYesNoCancel:
        sDialogType = "yesnocancel"; break;        
    }

    if (!sYesEvent.empty())
        m_pUIHandler->ensureUIEventExists(sYesEvent);
    if (!sNoEvent.empty())
        m_pUIHandler->ensureUIEventExists(sNoEvent);
    if (!sCancelEvent.empty())
        m_pUIHandler->ensureUIEventExists(sCancelEvent);

    m_ClientActions.push_back(std::make_shared<AMC::CUIClientAction_ShowMessageDlg> (sDialogUUID, sDialogType, sCaption, sTitle, sYesEvent, sNoEvent, sCancelEvent));

    return sDialogUUID;
}


std::vector<AMC::PUIClientAction>& CUIEnvironment::getClientActions()
{
    return m_ClientActions;
}

std::map<std::string, std::string>& CUIEnvironment::getExternalEventReturnValues()
{
    return m_ExternalEventReturnValues;
}



ITestEnvironment* CUIEnvironment::GetTestEnvironment()
{
    return new CTestEnvironment(m_pUISystemState->getTestOutputPath ());
}

LibMCEnv::Impl::IXMLDocument* CUIEnvironment::CreateXMLDocument(const std::string& sRootNodeName, const std::string& sDefaultNamespace)
{
    auto pDocument = std::make_shared<AMC::CXMLDocumentInstance> ();

    pDocument->createEmptyDocument(sRootNodeName, sDefaultNamespace);

    return new CXMLDocument(pDocument);
}

LibMCEnv::Impl::IXMLDocument* CUIEnvironment::ParseXMLString(const std::string& sXMLString)
{
    auto pDocument = std::make_shared<AMC::CXMLDocumentInstance>();

    try {
        pDocument->parseXMLString(sXMLString);
    } 
    catch (std::exception& E) {
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNOTPARSEXMLSTRING, "could not parse XML string: " + std::string (E.what()));
    }

    return new CXMLDocument(pDocument);

}

LibMCEnv::Impl::IXMLDocument* CUIEnvironment::ParseXMLData(const LibMCEnv_uint64 nXMLDataBufferSize, const LibMCEnv_uint8* pXMLDataBuffer)
{
    auto pDocument = std::make_shared<AMC::CXMLDocumentInstance>();

    try {
        pDocument->parseXMLData(nXMLDataBufferSize, pXMLDataBuffer);
    }
    catch (std::exception& E) {
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNOTPARSEXMLDATA, "could not parse XML data: " + std::string(E.what()));
    }

    return new CXMLDocument(pDocument);

}


IDataTable* CUIEnvironment::CreateDataTable()
{
    return new CDataTable();
}


bool CUIEnvironment::HasBuildJob(const std::string& sBuildUUID)
{
    std::string sNormalizedBuildUUID = AMCCommon::CUtils::normalizeUUIDString(sBuildUUID);

    try {
        auto pDataModel = m_pUISystemState->getDataModel();
        auto pBuildJobHandler = pDataModel->CreateBuildJobHandler();
        auto pBuildJob = pBuildJobHandler->RetrieveJob(sNormalizedBuildUUID);

        return (pBuildJob.get () != nullptr);
    }
    catch (std::exception) {
        return false;
    }
}

IBuild* CUIEnvironment::GetBuildJob(const std::string& sBuildUUID)
{
    std::string sNormalizedBuildUUID = AMCCommon::CUtils::normalizeUUIDString(sBuildUUID);

    auto pDataModel = m_pUISystemState->getDataModel();
    auto pBuildJobHandler = pDataModel->CreateBuildJobHandler();
    auto pBuildJob = pBuildJobHandler->RetrieveJob(sNormalizedBuildUUID);
    return new CBuild(pDataModel, pBuildJob->GetUUID (), m_pUISystemState->getToolpathHandler(), m_pUISystemState->getMeshHandler (), m_pUISystemState->getGlobalChronoInstance ());
}

bool CUIEnvironment::HasBuildExecution(const std::string& sExecutionUUID)
{
    std::string sNormalizedExecutionUUID = AMCCommon::CUtils::normalizeUUIDString(sExecutionUUID);

    try {
        auto pDataModel = m_pUISystemState->getDataModel();
        auto pBuildJobHandler = pDataModel->CreateBuildJobHandler();
        auto pExecution = pBuildJobHandler->RetrieveJobExecution(sNormalizedExecutionUUID);

        return (pExecution.get () != nullptr);
    }
    catch (std::exception) {
        return false;
    }

}

IBuildExecution* CUIEnvironment::GetBuildExecution(const std::string& sExecutionUUID)
{
    std::string sNormalizedExecutionUUID = AMCCommon::CUtils::normalizeUUIDString(sExecutionUUID);
    auto pDataModel = m_pUISystemState->getDataModel();
    auto pBuildJobHandler = pDataModel->CreateBuildJobHandler();
    auto pBuildExecution = pBuildJobHandler->RetrieveJobExecution(sNormalizedExecutionUUID);
    return new CBuildExecution (pBuildExecution, pDataModel, m_pUISystemState->getToolpathHandler(), m_pUISystemState->getMeshHandler(), m_pUISystemState->getGlobalChronoInstance());

}


IDiscreteFieldData2D* CUIEnvironment::CreateDiscreteField2D(const LibMCEnv_uint32 nPixelSizeX, const LibMCEnv_uint32 nPixelSizeY, const LibMCEnv_double dDPIValueX, const LibMCEnv_double dDPIValueY, const LibMCEnv_double dOriginX, const LibMCEnv_double dOriginY, const LibMCEnv_double dDefaultValue)
{
    AMC::PDiscreteFieldData2DInstance pInstance = std::make_shared<AMC::CDiscreteFieldData2DInstance>(nPixelSizeX, nPixelSizeY, dDPIValueX, dDPIValueY, dOriginX, dOriginY, dDefaultValue, true);
    return new CDiscreteFieldData2D(pInstance);
}

IDiscreteFieldData2D* CUIEnvironment::CreateDiscreteField2DFromImage(IImageData* pImageDataInstance, const LibMCEnv_double dBlackValue, const LibMCEnv_double dWhiteValue, const LibMCEnv_double dOriginX, const LibMCEnv_double dOriginY)
{
    if (pImageDataInstance == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

    auto pImageDataImpl = dynamic_cast<CImageData*> (pImageDataInstance);
    if (pImageDataImpl == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDCAST);

    uint32_t nPixelSizeX, nPixelSizeY;
    pImageDataImpl->GetSizeInPixels(nPixelSizeX, nPixelSizeY);

    double dDPIValueX, dDPIValueY;
    pImageDataImpl->GetDPI(dDPIValueX, dDPIValueY);

    AMC::PDiscreteFieldData2DInstance pFieldInstance = std::make_shared<AMC::CDiscreteFieldData2DInstance>(nPixelSizeX, nPixelSizeY, dDPIValueX, dDPIValueY, dOriginX, dOriginY, 0.0, false);

    auto pixelFormat = pImageDataImpl->GetPixelFormat();
    auto& rawPixelData = pImageDataImpl->getPixelData();
    pFieldInstance->loadFromRawPixelData (rawPixelData, pixelFormat, dBlackValue, dWhiteValue);
    
    return new CDiscreteFieldData2D(pFieldInstance);

}


bool CUIEnvironment::CheckPermission(const std::string& sPermissionIdentifier)
{
    auto pUserInformation = m_pAPIAuth->getUserInformation();
    return m_pUISystemState->getAccessControl()->checkPermissionInRole(pUserInformation->getRoleIdentifier(), sPermissionIdentifier);
}

std::string CUIEnvironment::GetCurrentUserLogin()
{
    auto pUserInformation = m_pAPIAuth->getUserInformation();
    return pUserInformation->getLogin();
}

std::string CUIEnvironment::GetCurrentUserDescription()
{
    auto pUserInformation = m_pAPIAuth->getUserInformation();
    return pUserInformation->getDescription();
}

std::string CUIEnvironment::GetCurrentUserRole()
{
    auto pUserInformation = m_pAPIAuth->getUserInformation();
    return pUserInformation->getRoleIdentifier();

}

std::string CUIEnvironment::GetCurrentUserLanguage()
{
    auto pUserInformation = m_pAPIAuth->getUserInformation();
    return pUserInformation->getLanguageIdentifier();

}

std::string CUIEnvironment::GetCurrentUserUUID()
{
    auto pUserInformation = m_pAPIAuth->getUserInformation();
    return pUserInformation->getUUID();

}

IUserManagementHandler* CUIEnvironment::CreateUserManagement()
{
    return new CUserManagementHandler(m_pUISystemState->getDataModel(), m_pUISystemState->getAccessControl(), m_pUISystemState->getLanguageHandler());
}

IJournalHandler* CUIEnvironment::GetCurrentJournal()
{
    return new CJournalHandler_Current(m_pUISystemState->getStateJournal());
}

ISceneHandler* CUIEnvironment::CreateSceneHandler() 
{
	auto pToolpathHandler = m_pUISystemState->getToolpathHandler();
	if (pToolpathHandler == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INTERNALERROR);

	auto pLib3MFWrapper = pToolpathHandler->getLib3MFWrapper();
	if (pLib3MFWrapper == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INTERNALERROR);

	auto pMeshHandler = m_pUISystemState->getMeshHandler();
	if (pMeshHandler == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INTERNALERROR);

	auto pCoreResourcePackage = m_pUIHandler->getCoreResourcePackage();
	if (pCoreResourcePackage.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INTERNALERROR);

	return new CSceneHandler(pMeshHandler, pLib3MFWrapper, pCoreResourcePackage);
}

/*IMeshObject* CUIEnvironment::RegisterMeshFrom3MFResource(const std::string& sResourceName, const std::string& sMeshUUID)
{

    auto pResourcePackage = m_pUIHandler->getCoreResourcePackage();
    if (pResourcePackage.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INTERNALERROR);

    auto pResourceEntry = pResourcePackage->findEntryByName(sResourceName, true);

    auto pMeshHandler = m_pUISystemState->getMeshHandler();
    auto pLib3MFWrapper = m_pUISystemState->getToolpathHandler()->getLib3MFWrapper();

    auto pMeshEntity = pMeshHandler->register3MFResource(pLib3MFWrapper.get(), pResourcePackage.get(), sResourceName);

    return new CMeshObject(pMeshHandler, pMeshEntity->getUUID());
}*/

IDataSeries* CUIEnvironment::CreateDataSeries(const std::string& sName, const bool bBoundToLogin)
{    
    auto pDataSeriesHandler = m_pUISystemState->getDataSeriesHandler();
    auto pDataSeries = pDataSeriesHandler->createDataSeries(sName);

    return new CDataSeries(pDataSeries);

}

bool CUIEnvironment::HasDataSeries(const std::string& sDataSeriesUUID)
{
    auto pDataSeriesHandler = m_pUISystemState->getDataSeriesHandler();
    return pDataSeriesHandler->hasDataSeries(sDataSeriesUUID);
}

IDataSeries* CUIEnvironment::FindDataSeries(const std::string& sDataSeriesUUID)
{
    auto pDataSeries = m_pUISystemState->getDataSeriesHandler()->findDataSeries(sDataSeriesUUID, true);

    return new CDataSeries(pDataSeries);


}

void CUIEnvironment::ReleaseDataSeries(const std::string& sDataSeriesUUID)
{
    auto pDataSeriesHandler = m_pUISystemState->getDataSeriesHandler();
    pDataSeriesHandler->unloadDataSeries(sDataSeriesUUID);

}

IAlert* CUIEnvironment::CreateAlert(const std::string& sIdentifier, const std::string& sReadableContextInformation, const bool bAutomaticLogEntry)
{
    if (sIdentifier.empty())
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_EMPTYALERTIDENTIFIER);

    if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sIdentifier))
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDALERTIDENTIFIER, "invalid alert identifier: " + sIdentifier);

    auto sNewUUID = AMCCommon::CUtils::createUUID();

    auto pGlobalChrono = m_pUISystemState->getGlobalChronoInstance();
    auto sTimeStamp = pGlobalChrono->getUTCTimeInISO8601 ();

    auto pDefinition = m_pUISystemState->getAlertHandler()->findDefinition(sIdentifier, true);
    auto alertDescription = pDefinition->getDescription();
    auto alertLevel = pDefinition->getAlertLevel();

    auto pDataModel = m_pUISystemState->getDataModel();
    auto pAlertSession = pDataModel->CreateAlertSession();
    auto pAlertData = pAlertSession->AddAlert(sNewUUID, pDefinition->getIdentifier(), alertLevel, alertDescription.getCustomValue(), alertDescription.getStringIdentifier(), sReadableContextInformation, pDefinition->needsAcknowledgement(), sTimeStamp);

    if (bAutomaticLogEntry) {

        std::string sLogString = "Created alert " + sNewUUID + ": " + pDefinition->getIdentifier() + " / " + alertDescription.getCustomValue();
        AMC::eLogLevel logLevel;
        switch (alertLevel) {
        case LibMCData::eAlertLevel::FatalError:
            logLevel = AMC::eLogLevel::FatalError;
            break;
        case LibMCData::eAlertLevel::CriticalError:
            logLevel = AMC::eLogLevel::CriticalError;
            break;
        case LibMCData::eAlertLevel::Message:
            logLevel = AMC::eLogLevel::Message;
            break;
        case LibMCData::eAlertLevel::Warning:
            logLevel = AMC::eLogLevel::Warning;
            break;
        default:
            logLevel = AMC::eLogLevel::Unknown;
            break;

        }

        m_pLogger->logMessage(sLogString, m_sLogSubSystem, logLevel);
    }


    auto pUserInformation = m_pAPIAuth->getUserInformation();
    return new CAlert(pDataModel, pAlertData->GetUUID (), pUserInformation->getUUID(), m_pLogger, m_sLogSubSystem, m_pUISystemState->getGlobalChronoInstance ());
}

IAlert* CUIEnvironment::FindAlert(const std::string& sUUID)
{
    std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);

    auto pDataModel = m_pUISystemState->getDataModel();
    auto pAlertSession = pDataModel->CreateAlertSession();

    if (!pAlertSession->HasAlert(sNormalizedUUID))
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_ALERTNOTFOUND, "alert not found: " + sNormalizedUUID + " (in UIEnvironment::FindAlert)");

    auto pAlertData = pAlertSession->GetAlertByUUID(sNormalizedUUID);
    auto pUserInformation = m_pAPIAuth->getUserInformation();
    return new CAlert(pDataModel, pAlertData->GetUUID(), pUserInformation->getUUID (), m_pLogger, m_sLogSubSystem, m_pUISystemState->getGlobalChronoInstance ());

}

bool CUIEnvironment::AlertExists(const std::string& sUUID)
{
    std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);

    auto pDataModel = m_pUISystemState->getDataModel();
    auto pAlertSession = pDataModel->CreateAlertSession();

    return pAlertSession->HasAlert(sNormalizedUUID);

}


IAlertIterator* CUIEnvironment::RetrieveAlerts(const bool bOnlyActive)
{
    std::unique_ptr<LibMCEnv::Impl::CAlertIterator> returnIterator(new CAlertIterator());

    auto pDataModel = m_pUISystemState->getDataModel();
    auto pAlertSession = pDataModel->CreateAlertSession();

    auto pAlertIterator = pAlertSession->RetrieveAlerts(bOnlyActive);
    while (pAlertIterator->MoveNext()) {
        auto pAlertData = pAlertIterator->GetCurrentAlert();
        auto pUserInformation = m_pAPIAuth->getUserInformation();
        returnIterator->AddAlert(std::make_shared<CAlert>(pDataModel, pAlertData->GetUUID (), pUserInformation->getUUID (), m_pLogger, m_sLogSubSystem, m_pUISystemState->getGlobalChronoInstance ()));
    }

    return returnIterator.release();

}

IAlertIterator* CUIEnvironment::RetrieveAlertsByType(const std::string& sIdentifier, const bool bOnlyActive)
{
    std::unique_ptr<LibMCEnv::Impl::CAlertIterator> returnIterator(new CAlertIterator());

    auto pDataModel = m_pUISystemState->getDataModel();
    auto pAlertSession = pDataModel->CreateAlertSession();

    auto pAlertIterator = pAlertSession->RetrieveAlertsByType(sIdentifier, bOnlyActive);
    while (pAlertIterator->MoveNext()) {
        auto pAlertData = pAlertIterator->GetCurrentAlert();
        auto pUserInformation = m_pAPIAuth->getUserInformation();
        returnIterator->AddAlert(std::make_shared<CAlert>(pDataModel, pAlertData->GetUUID(), pUserInformation->getUUID(), m_pLogger, m_sLogSubSystem, m_pUISystemState->getGlobalChronoInstance ()));
    }

    return returnIterator.release();

}

bool CUIEnvironment::HasAlertOfType(const std::string& sIdentifier, const bool bOnlyActive)
{
    auto pDataModel = m_pUISystemState->getDataModel();
    auto pAlertSession = pDataModel->CreateAlertSession();

    auto pAlertIterator = pAlertSession->RetrieveAlertsByType(sIdentifier, bOnlyActive);
    return (pAlertIterator->MoveNext());

}

ICryptoContext* CUIEnvironment::CreateCryptoContext()
{
    return new CCryptoContext ();
}


ITempStreamWriter* CUIEnvironment::CreateTemporaryStream(const std::string& sName, const std::string& sMIMEType)
{
    if (sName.empty()) 
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_EMPTYJOURNALSTREAMNAME);
    if (sMIMEType.empty())
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_EMPTYJOURNALSTREAMMIMETYPE);

    auto pUserInformation = m_pAPIAuth->getUserInformation();
    std::string sUserUUID = pUserInformation->getUUID();
    
    return new CTempStreamWriter(m_pUISystemState->getDataModel(), sName, sMIMEType, sUserUUID, m_pUISystemState->getGlobalChronoInstance ());
}

IZIPStreamWriter* CUIEnvironment::CreateZIPStream(const std::string& sName)
{
    if (sName.empty())
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_EMPTYJOURNALSTREAMNAME);

    auto pChrono = m_pUISystemState->getGlobalChronoInstance();

    auto pUserInformation = m_pAPIAuth->getUserInformation();
    std::string sUserUUID = pUserInformation->getUUID();

    std::string sStreamUUID = AMCCommon::CUtils::createUUID();

    auto pDataModel = m_pUISystemState->getDataModel();
    auto pStorage = pDataModel->CreateStorage();
    auto pZIPWriter = pStorage->CreateZIPStream(sStreamUUID, sName, sUserUUID, pChrono->getUTCTimeStampInMicrosecondsSince1970());

    return new CZIPStreamWriter(pDataModel, pZIPWriter, sStreamUUID, sName, pChrono);
}


IStreamReader* CUIEnvironment::LoadStream(const std::string& sUUID, const bool bMustExist)
{
    auto pDataModel = m_pUISystemState->getDataModel();
    auto pStorage = pDataModel->CreateStorage();

    std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);

    if (pStorage->StreamIsReady(sNormalizedUUID)) {

        auto pStorageStream = pStorage->RetrieveStream(sNormalizedUUID);
        return new CStreamReader (pStorage, pStorageStream);

    }
    else {
        if (bMustExist)
            throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_STORAGESTREAMNOTFOUND, "Storage Stream not found: " + sUUID);
    }

    return nullptr;
}



IDateTime* CUIEnvironment::GetCurrentDateTime()
{
    auto pChrono = m_pUISystemState->getGlobalChronoInstance();
    return new CDateTime(pChrono->getUTCTimeStampInMicrosecondsSince1970());

}

IDateTime* CUIEnvironment::GetCustomDateTime(const LibMCEnv_uint32 nYear, const LibMCEnv_uint32 nMonth, const LibMCEnv_uint32 nDay, const LibMCEnv_uint32 nHour, const LibMCEnv_uint32 nMinute, const LibMCEnv_uint32 nSecond, const LibMCEnv_uint32 nMicrosecond)
{
    return new CDateTime(AMCCommon::CChrono::getMicrosecondsSince1970FromDateTime(nYear, nMonth, nDay, nHour, nMinute, nSecond, nMicrosecond));
}

IDateTime* CUIEnvironment::GetStartDateTime()
{
    auto pGlobalChrono = m_pUISystemState->getGlobalChronoInstance();
    return new CDateTime(pGlobalChrono->getStartTimeStampInMicrosecondsSince1970());
}

void CUIEnvironment::Sleep(const LibMCEnv_uint32 nDelay)
{
    AMCCommon::CChrono chrono;
    chrono.sleepMilliseconds(nDelay);
}

void CUIEnvironment::addExternalEventParameter(const std::string& sKey, const std::string& sValue)
{
    if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sKey))
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDEXTERNALEVENTPARAMETERKEY, sKey);

    if (AMC::CUIHandleEventResponse::externalValueNameIsReserved(sKey))
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_EXTERNALEVENTVALUEKEYISRESERVED, "external return value key is reserved: " + sKey);

    m_ExternalEventParameters.insert(std::make_pair (sKey, sValue));
}

bool CUIEnvironment::HasExternalEventParameter(const std::string& sParameterName)
{
    if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sParameterName))
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDEXTERNALEVENTPARAMETERKEY, sParameterName);

    auto iIter = m_ExternalEventParameters.find(sParameterName);
    return (iIter != m_ExternalEventParameters.end());

}

std::string CUIEnvironment::GetExternalEventParameter(const std::string& sParameterName)
{
    if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sParameterName))
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDEXTERNALEVENTPARAMETERKEY, "invalid external event parameter key: " +  sParameterName);

    auto iIter = m_ExternalEventParameters.find(sParameterName);
    if (iIter == m_ExternalEventParameters.end())
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNOTFINDEXTERNALEVENTPARAMETER, "could not find external event parameter: " + sParameterName);

    return iIter->second;

}

void CUIEnvironment::AddExternalEventResultValue(const std::string& sReturnValueName, const std::string& sReturnValue)
{
    if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sReturnValueName))
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDEXTERNALEVENTRETURNVALUEKEY, "invalid external return value key: " + sReturnValueName);

    if (AMC::CUIHandleEventResponse::externalValueNameIsReserved (sReturnValueName))
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_EXTERNALEVENTRETURNVALUEKEYISRESERVED, "external return value key is reserved: " + sReturnValueName);

    m_ExternalEventReturnValues.insert(std::make_pair (sReturnValueName, sReturnValue));
}

bool CUIEnvironment::HasResourceData(const std::string& sIdentifier)
{

    if (m_pUIHandler == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INTERNALERROR);

    auto pResourcePackage = m_pUIHandler->getCoreResourcePackage();
    if (pResourcePackage.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INTERNALERROR);

    auto pResourceEntry = pResourcePackage->findEntryByName(sIdentifier, false);

    return (pResourceEntry.get() != nullptr);
}


void CUIEnvironment::LoadResourceData(const std::string& sResourceName, LibMCEnv_uint64 nResourceDataBufferSize, LibMCEnv_uint64* pResourceDataNeededCount, LibMCEnv_uint8* pResourceDataBuffer)
{
    
    if (m_pUIHandler == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INTERNALERROR);

    auto pResourcePackage = m_pUIHandler->getCoreResourcePackage();
    if (pResourcePackage.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INTERNALERROR);

    auto pResourceEntry = pResourcePackage->findEntryByName(sResourceName, true);
    auto nResourceSize = pResourceEntry->getSize();

    if (pResourceDataNeededCount != nullptr)
        *pResourceDataNeededCount = nResourceSize;

    if (pResourceDataBuffer != nullptr) {
        if (nResourceDataBufferSize < nResourceSize)
            throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_BUFFERTOOSMALL);

        pResourcePackage->readEntryEx(sResourceName, pResourceDataBuffer, nResourceDataBufferSize);
    }


}

std::string CUIEnvironment::LoadResourceString(const std::string& sResourceName)
{
    
    if (m_pUIHandler == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INTERNALERROR);

    auto pResourcePackage = m_pUIHandler->getCoreResourcePackage();
    if (pResourcePackage.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INTERNALERROR);

    return pResourcePackage->readEntryUTF8String(sResourceName);
}
