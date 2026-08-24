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
#include "amc_telemetry.hpp"
#include "amc_dataserieshandler.hpp"
#include "amc_toolpathhandler.hpp"
#include "amc_ui_handler.hpp"
#include "amc_ui_systemstate.hpp"
#include "amc_statemachinedata.hpp"
#include "amc_accesscontrol.hpp"
#include "amc_stringresourcehandler.hpp"
#include "amc_languagehandler.hpp"
#include "amc_meshhandler.hpp"
#include "amc_streamregistry.hpp"

#include "libmcdata_dynamic.hpp"
#include "libmc_types.hpp"
#include "libmcdata_types.hpp"

#include "common_chrono.hpp"

#include <exception>

#define __STRINGIZE(x) #x
#define __STRINGIZE_VALUE_OF(x) __STRINGIZE(x)

static std::string formatVersionString(const uint32_t nMajor, const uint32_t nMinor, const uint32_t nMicro)
{
	return std::to_string(nMajor) + "." + std::to_string(nMinor) + "." + std::to_string(nMicro);
}

static std::string formatVersionWithHash(const uint32_t nMajor, const uint32_t nMinor, const uint32_t nMicro, const std::string& sBuild)
{
	std::string sVersion = formatVersionString(nMajor, nMinor, nMicro);
	if (sBuild.empty())
		return sVersion;
	return sVersion + "." + sBuild;
}

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

		auto pTelemetryWriter = std::make_shared<CTelemetryWriter> (m_pDataModel->CreateTelemetrySession (), m_pGlobalChrono);
		m_pTelemetryHandler = std::make_shared<CTelemetryHandler> (pTelemetryWriter);

		m_pMeshHandler = std::make_shared<CMeshHandler>();
		m_pToolpathHandler = std::make_shared<CToolpathHandler>(m_pDataModel);
		m_pStreamRegistry = std::make_shared<CStreamRegistry>();
		m_pDriverHandler = std::make_shared<CDriverHandler>(pEnvWrapper, m_pToolpathHandler, m_pMeshHandler, m_pLogger, m_pDataModel, m_pGlobalChrono, m_pStateJournal, m_pTelemetryHandler, m_pStreamRegistry);
		m_pSignalHandler = std::make_shared<CStateSignalHandler>(m_pTelemetryHandler);
		m_pStateMachineData = std::make_shared<CStateMachineData>();
		m_pLanguageHandler = std::make_shared<CLanguageHandler>();
		m_pDataSeriesHandler = std::make_shared<CDataSeriesHandler>();
		m_pAlertHandler = std::make_shared<CAlertHandler>();

		auto pUISystemState = std::make_shared<CUISystemState>(m_pStateMachineData, m_pToolpathHandler, m_pSignalHandler, m_pLogger, m_pStateJournal, getTestEnvironmentPath(), m_pAccessControl, m_pLanguageHandler, m_pMeshHandler, m_pDataSeriesHandler, m_pGlobalChrono, m_pAlertHandler, m_pDataModel, m_pStreamRegistry);
		m_pUIHandler = std::make_shared<CUIHandler>(pEnvWrapper, pUISystemState);

		m_pSystemParameterHandler = std::make_shared<CParameterHandler>("System", m_pGlobalChrono);
		auto pSystemInformationGroup = std::make_shared<CParameterGroup>("information", "Information", m_pGlobalChrono);
		pSystemInformationGroup->addNewStringParameter("githash", "Git Hash", getGitHash ());
		pSystemInformationGroup->addNewStringParameter("clienthash", "Client Hash", getClientHash());
		pSystemInformationGroup->addNewStringParameter("libmc_version", "LibMC Core Version", formatVersionWithHash(LIBMC_VERSION_MAJOR, LIBMC_VERSION_MINOR, LIBMC_VERSION_MICRO, getGitHash()));
		pSystemInformationGroup->addNewStringParameter("libmcdata_version", "LibMCData Version", formatVersionWithHash(LIBMCDATA_VERSION_MAJOR, LIBMCDATA_VERSION_MINOR, LIBMCDATA_VERSION_MICRO, getGitHash()));
		pSystemInformationGroup->addNewStringParameter("compile_time", "Compile time", std::string (__DATE__) + " " + std::string (__TIME__));
		pSystemInformationGroup->addNewStringParameter("installation_uuid", "Installation", getInstallationUUID ());
		m_pSystemParameterHandler->addGroup(pSystemInformationGroup);

		m_pSystemMemoryGroup = std::make_shared<CParameterGroup>("memory", "Memory usage", m_pGlobalChrono);
		m_pSystemParameterHandler->addGroup(m_pSystemMemoryGroup);


		registerMemoryUsageProvider("mesh_entities_bytes", "Mesh entity memory usage (bytes)", [this]() {
			if (m_pMeshHandler.get() == nullptr)
				return (uint64_t)0;
			return m_pMeshHandler->getMeshEntityMemoryUsageInBytes();
		});
		registerMemoryUsageProvider("mesh_scenes_bytes", "Mesh scene memory usage (bytes)", [this]() {
			if (m_pMeshHandler.get() == nullptr)
				return (uint64_t)0;
			return m_pMeshHandler->getSceneMemoryUsageInBytes();
		});
		registerMemoryUsageProvider("data_series_bytes", "Data series memory usage (bytes)", [this]() {
			if (m_pDataSeriesHandler.get() == nullptr)
				return (uint64_t)0;
			return m_pDataSeriesHandler->getMemoryUsageInBytes();
		});
		registerMemoryUsageProvider("known_total_bytes", "Total known memory usage (bytes)", [this]() {
			return (uint64_t)0;
		});

		m_pStateMachineData->registerParameterHandler("system", m_pSystemParameterHandler, m_pGlobalChrono);
		updateMemoryUsageParameters();

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

	CTelemetryHandler* CSystemState::telemetryHandler()
	{
		return m_pTelemetryHandler.get();
	}

	PTelemetryHandler CSystemState::getTelemetryHandlerInstance()
	{
		return m_pTelemetryHandler;
	}

	PStreamRegistry CSystemState::getStreamRegistryInstance()
	{
		return m_pStreamRegistry;
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

	void CSystemState::registerMemoryUsageProvider(const std::string& sParameterName, const std::string& sDescription, std::function<uint64_t()> callback)
	{
		if (callback == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

		std::lock_guard<std::mutex> lockGuard(m_MemoryUsageMutex);
		if (m_MemoryUsageProviders.find(sParameterName) != m_MemoryUsageProviders.end())
			throw ELibMCCustomException(LIBMC_ERROR_DUPLICATEPARAMETERNAME, sParameterName);

		sMemoryUsageProvider provider;
		provider.m_sParameterName = sParameterName;
		provider.m_sDescription = sDescription;
		provider.m_Callback = callback;
		m_MemoryUsageProviders.insert(std::make_pair(sParameterName, provider));

		if (m_pSystemMemoryGroup.get() != nullptr)
			m_pSystemMemoryGroup->addNewIntParameter(sParameterName, sDescription, 0);
	}

	void CSystemState::updateMemoryUsageParameters()
	{
		std::map<std::string, sMemoryUsageProvider> providers;
		PParameterGroup pMemoryGroup;
		{
			std::lock_guard<std::mutex> lockGuard(m_MemoryUsageMutex);
			providers = m_MemoryUsageProviders;
			pMemoryGroup = m_pSystemMemoryGroup;
		}

		if (pMemoryGroup.get() == nullptr)
			return;

		uint64_t nKnownTotal = 0;
		for (auto& providerPair : providers) {
			auto& provider = providerPair.second;
			if (provider.m_sParameterName == "known_total_bytes")
				continue;

			uint64_t nUsage = 0;
			try {
				if (provider.m_Callback)
					nUsage = provider.m_Callback();
			}
			catch (std::exception &) {
				nUsage = 0;
			}

			nKnownTotal += nUsage;
			pMemoryGroup->setIntParameterValueByName(provider.m_sParameterName, (int64_t)nUsage);
		}

		if (providers.find("known_total_bytes") != providers.end())
			pMemoryGroup->setIntParameterValueByName("known_total_bytes", (int64_t)nKnownTotal);
	}

	void CSystemState::addDriverVersionInfo(const std::string& sDriverName)
	{
		if (m_pDriverHandler.get() == nullptr)
			return;
		if (m_pSystemParameterHandler.get() == nullptr)
			return;
		if (!m_pSystemParameterHandler->hasGroup("information"))
			return;

		std::string sDriverType;
		HSymbolLookupHandle pSymbolLookup = nullptr;
		m_pDriverHandler->GetDriverInformation(sDriverName, sDriverType, pSymbolLookup);
		(void)pSymbolLookup;

		uint32_t nMajor = 0;
		uint32_t nMinor = 0;
		uint32_t nMicro = 0;
		std::string sBuild;
		m_pDriverHandler->GetDriverVersionInfo(sDriverName, nMajor, nMinor, nMicro, sBuild);

		std::string sGroupDescription = "Driver " + sDriverName;
		if (!sDriverType.empty())
			sGroupDescription += " (" + sDriverType + ")";

		PParameterGroup pInformationGroup = m_pSystemParameterHandler->findGroup("information", true);
		std::string sParameterName = "driver_" + sDriverName;
		std::string sVersion = formatVersionWithHash(nMajor, nMinor, nMicro, sBuild);
		if (pInformationGroup->hasParameter(sParameterName)) {
			pInformationGroup->setParameterValueByName(sParameterName, sVersion);
		}
		else {
			pInformationGroup->addNewStringParameter(sParameterName, sGroupDescription, sVersion);
		}
	}

}
