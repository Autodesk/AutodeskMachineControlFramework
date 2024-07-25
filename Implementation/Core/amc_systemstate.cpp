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


#include "amc_systemstate.hpp"
#include "libmc_exceptiontypes.hpp"

#include "amc_logger.hpp"
#include "amc_parameterhandler.hpp"
#include "amc_statesignalhandler.hpp"
#include "amc_driverhandler.hpp"
#include "amc_alerthandler.hpp"
#include "amc_dataserieshandler.hpp"
#include "amc_toolpathhandler.hpp"
#include "amc_ui_handler.hpp"
#include "amc_ui_systemstate.hpp"
#include "amc_statemachinedata.hpp"
#include "amc_accesscontrol.hpp"
#include "amc_stringresourcehandler.hpp"
#include "amc_languagehandler.hpp"
#include "amc_meshhandler.hpp"

#include "libmcdata_dynamic.hpp"

#include "common_chrono.hpp"

#define __STRINGIZE(x) #x
#define __STRINGIZE_VALUE_OF(x) __STRINGIZE(x)


namespace AMC {

	CSystemState::CSystemState(AMC::PLogger pLogger, LibMCData::PDataModel pDataModel, LibMCEnv::PWrapper pEnvWrapper, AMC::PStateJournal pStateJournal, const std::string& sTestEnvironmentPath, AMCCommon::PChrono pGlobalChrono)
		: m_sTestEnvironmentPath (sTestEnvironmentPath), m_pStateJournal (pStateJournal)
	{
		LibMCAssertNotNull(pLogger.get());
		LibMCAssertNotNull(pDataModel.get());
		LibMCAssertNotNull(pEnvWrapper.get());
		LibMCAssertNotNull(pStateJournal.get());
		LibMCAssertNotNull(pGlobalChrono.get());
		
		m_pGlobalChrono = pGlobalChrono;

		m_pLogger = pLogger;
		m_pDataModel = pDataModel;

		// Retrieve Installation UUID and Secret
		m_pDataModel->GetInstallationInformation(m_sInstallationUUID, m_sInstallationSecret);

		m_pAccessControl = std::make_shared<CAccessControl> ();
		m_pStringResourceHandler = std::make_shared<CStringResourceHandler> ();

		m_pMeshHandler = std::make_shared<CMeshHandler>();
		m_pToolpathHandler = std::make_shared<CToolpathHandler>(m_pDataModel);
		m_pDriverHandler = std::make_shared<CDriverHandler>(pEnvWrapper, m_pToolpathHandler, m_pMeshHandler, m_pLogger, m_pDataModel, m_pGlobalChrono, m_pStateJournal);
		m_pSignalHandler = std::make_shared<CStateSignalHandler>();
		m_pStateMachineData = std::make_shared<CStateMachineData>();
		m_pLanguageHandler = std::make_shared<CLanguageHandler>();
		m_pDataSeriesHandler = std::make_shared<CDataSeriesHandler>();
		m_pAlertHandler = std::make_shared<CAlertHandler>();

		auto pUISystemState = std::make_shared<CUISystemState>(m_pStateMachineData, m_pToolpathHandler, m_pSignalHandler, m_pLogger, m_pStateJournal, getTestEnvironmentPath(), m_pAccessControl, m_pLanguageHandler, m_pMeshHandler, m_pDataSeriesHandler, m_pGlobalChrono, m_pAlertHandler, m_pDataModel);
		m_pUIHandler = std::make_shared<CUIHandler>(pEnvWrapper, pUISystemState);

		auto pSystemParameterHandler = std::make_shared<CParameterHandler>("System", m_pGlobalChrono);
		auto pSystemInformationGroup = std::make_shared<CParameterGroup>("information", "Information", m_pGlobalChrono);
		pSystemInformationGroup->addNewStringParameter("githash", "Git Hash", getGitHash ());
		pSystemInformationGroup->addNewStringParameter("clienthash", "Client Hash", getClientHash());
		pSystemInformationGroup->addNewStringParameter("compile_time", "Compile time", std::string (__DATE__) + " " + std::string (__TIME__));
		pSystemInformationGroup->addNewStringParameter("installation_uuid", "Installation", getInstallationUUID ());
		pSystemParameterHandler->addGroup(pSystemInformationGroup);

		m_pStateMachineData->registerParameterHandler("system", pSystemParameterHandler, m_pGlobalChrono);

	}

	CSystemState::~CSystemState()
	{
		m_pDriverHandler = nullptr;
		m_pUIHandler = nullptr;
		m_pStateMachineData = nullptr;
		m_pToolpathHandler = nullptr;
		m_pSignalHandler = nullptr;
		m_pAlertHandler = nullptr;				
		m_pLogger = nullptr;
		m_pDataModel = nullptr;

	}

	CLogger* CSystemState::logger()
	{
		return m_pLogger.get();
	}


	CStateSignalHandler* CSystemState::stateSignalHandler()
	{
		return m_pSignalHandler.get();
	}

	CDriverHandler* CSystemState::driverHandler()
	{
		return m_pDriverHandler.get();
	}

	CToolpathHandler* CSystemState::toolpathHandler()
	{
		return m_pToolpathHandler.get();
	}

	
	CUIHandler* CSystemState::uiHandler()
	{
		return m_pUIHandler.get();
	}

	CStateMachineData* CSystemState::stateMachineData()
	{
		return m_pStateMachineData.get();
	}

	CAccessControl* CSystemState::accessControl()
	{
		return m_pAccessControl.get();
	}

	CAlertHandler* CSystemState::alertHandler()
	{
		return m_pAlertHandler.get();
	}

	AMC::CStringResourceHandler* CSystemState::stringResourceHandler()
	{
		return m_pStringResourceHandler.get();
	}


	PLogger CSystemState::getLoggerInstance()
	{
		return m_pLogger;
	}

	PStateSignalHandler CSystemState::getStateSignalHandlerInstance()
	{
		return m_pSignalHandler;
	}

	PDriverHandler CSystemState::getDriverHandlerInstance()
	{
		return m_pDriverHandler;
	}

	PToolpathHandler CSystemState::getToolpathHandlerInstance()
	{
		return m_pToolpathHandler;
	}

	PStateJournal CSystemState::getStateJournalInstance()
	{
		return m_pStateJournal;
	}

	PAccessControl CSystemState::getAccessControlInstance()
	{
		return m_pAccessControl;
	}

	PLanguageHandler CSystemState::getLanguageHandlerInstance()
	{
		return m_pLanguageHandler;
	}

	PMeshHandler CSystemState::getMeshHandlerInstance()
	{
		return m_pMeshHandler;
	}

	PDataSeriesHandler CSystemState::getDataSeriesHandlerInstance()
	{
		return m_pDataSeriesHandler;
	}

	PAlertHandler CSystemState::getAlertHandlerInstance()
	{
		return m_pAlertHandler;
	}



	PStateMachineData CSystemState::getStateMachineData()
	{
		return m_pStateMachineData;
	}


	LibMCData::PDataModel CSystemState::getDataModelInstance()
	{
		return m_pDataModel;
	}

	AMCCommon::PChrono CSystemState::getGlobalChronoInstance()
	{
		return m_pGlobalChrono;
	}


	AMCCommon::CChrono* CSystemState::globalChrono()
	{
		return m_pGlobalChrono.get();
	}

	void CSystemState::addLibraryPath(const std::string& sLibraryName, const std::string& sLibraryPath, const std::string& sLibraryResourcePath)
	{
		m_LibraryPathes.insert(std::make_pair(sLibraryName, std::make_pair (sLibraryPath, sLibraryResourcePath)));
		m_pToolpathHandler->setLibraryPath(sLibraryName, sLibraryPath);
	}

	std::string CSystemState::getLibraryPath(const std::string& sLibraryName)
	{
		auto iIter = m_LibraryPathes.find(sLibraryName);
		if (iIter == m_LibraryPathes.end())
			throw ELibMCCustomException(LIBMC_ERROR_LIBRARYPATHNOTFOUND, sLibraryName);

		return iIter->second.first;
	}

	std::string CSystemState::getLibraryResourcePath(const std::string& sLibraryName)
	{
		auto iIter = m_LibraryPathes.find(sLibraryName);
		if (iIter == m_LibraryPathes.end())
			throw ELibMCCustomException(LIBMC_ERROR_LIBRARYPATHNOTFOUND, sLibraryName);

		return iIter->second.second;

	}


	std::string CSystemState::getInstallationUUID()
	{
		return m_sInstallationUUID;
	}

	std::string CSystemState::getInstallationSecret()
	{
		return m_sInstallationSecret;
	}

	std::string CSystemState::getGitHash()
	{
		return __STRINGIZE_VALUE_OF(__GITHASH);
	}

	std::string CSystemState::getClientHash()
	{
		return __STRINGIZE_VALUE_OF(__CLIENTHASH);
	}

	std::string CSystemState::getTestEnvironmentPath()
	{
		return m_sTestEnvironmentPath;
	}

	uint64_t CSystemState::getAbsoluteTimeStamp()
	{
		return m_pGlobalChrono->getUTCTimeStampInMicrosecondsSince1970();
	}

}
