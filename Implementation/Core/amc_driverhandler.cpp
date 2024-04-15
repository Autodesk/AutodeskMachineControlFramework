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


#define _AMC_DRIVER_HEADERPROTECTION
#include "amc_driver.hpp"

#include "amc_driverhandler.hpp"
#include "amc_toolpathhandler.hpp"

#include "libmcenv_driverenvironment.hpp"
#include "libmc_exceptiontypes.hpp"
#include "amc_logger.hpp"

#include <vector>
#include <memory>

#include <iostream>


using namespace AMC;


template <class C> std::shared_ptr<C> mapInternalDriverEnvInstance(std::shared_ptr<LibMCEnv::Impl::IBase> pImplInstance, LibMCEnv::PWrapper pWrapper)
{
	LibMCAssertNotNull(pWrapper.get());

	auto pExternalInstance = std::make_shared <C>(pWrapper.get(), (LibMCEnv::Impl::IBase*)(pImplInstance.get()));
	pImplInstance->IncRefCount();
	return pExternalInstance;
}

CDriverHandler::CDriverHandler(LibMCEnv::PWrapper pEnvironmentWrapper, PToolpathHandler pToolpathHandler, PLogger pLogger, LibMCData::PDataModel pDataModel, AMCCommon::PChrono pGlobalChrono, std::string sSystemUserID, PStateJournal pStateJournal)
	: m_pEnvironmentWrapper (pEnvironmentWrapper), 
	m_pToolpathHandler (pToolpathHandler), 
	m_pLogger (pLogger),
	m_pDataModel (pDataModel),
	m_pGlobalChrono (pGlobalChrono),
	m_pStateJournal(pStateJournal)
{
	LibMCAssertNotNull(pEnvironmentWrapper.get());
	LibMCAssertNotNull(pToolpathHandler.get());
	LibMCAssertNotNull(pLogger.get());
	LibMCAssertNotNull(pDataModel.get());
	LibMCAssertNotNull(pGlobalChrono.get());
	LibMCAssertNotNull(pStateJournal.get());

}


CDriverHandler::~CDriverHandler()
{

}


void CDriverHandler::registerDriver(const std::string& sName, const std::string& sType, const std::string& sLibraryName, const std::string& sLibraryPath, const std::string& sResourcePath, const std::string& sDriverConfigurationData, AMC::PResourcePackage pMachineResourcePackage)
{
	std::lock_guard<std::mutex> lockGuard(m_Mutex);

	if (pMachineResourcePackage.get() == nullptr)
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDPARAM, "no machine package");

	if (findDriver(sName, false) != nullptr)
		throw ELibMCCustomException(LIBMC_ERROR_DRIVERALREADYREGISTERED, sName);

	if (m_sTempBasePath.empty ())
		throw ELibMCInterfaceException(LIBMC_ERROR_TEMPBASEPATHEMPTY);

	PResourcePackage pDriverResourcePackage;
	if (!sResourcePath.empty()) {
		auto pStream = std::make_shared <AMCCommon::CImportStream_Native> (sResourcePath);
		pDriverResourcePackage = CResourcePackage::makeFromStream(pStream, sResourcePath, AMCPACKAGE_SCHEMANAMESPACE);
	}
	else {
		pDriverResourcePackage = CResourcePackage::makeEmpty(sResourcePath);
	}

	auto pParameterGroup = std::make_shared<CParameterGroup>(m_pGlobalChrono);
	pParameterGroup->setJournal(m_pStateJournal, sName);

	auto pInternalEnvironment = std::make_shared<LibMCEnv::Impl::CDriverEnvironment>(pParameterGroup, pDriverResourcePackage, pMachineResourcePackage, m_pToolpathHandler, m_sTempBasePath, m_pLogger, m_pDataModel, m_pGlobalChrono, m_sSystemUserID, sName);

	pInternalEnvironment->setIsInitializing(true);

	LibMCDriver::PWrapper pLibraryWrapper;
	auto iDLLIter = m_DriverWrapperMap.find(sLibraryName);
	if (iDLLIter != m_DriverWrapperMap.end()) {
		pLibraryWrapper = iDLLIter->second;
	}
	else {
		pLibraryWrapper = LibMCDriver::CWrapper::loadLibrary(sLibraryPath);
		pLibraryWrapper->InjectComponent("LibMCEnv", m_pEnvironmentWrapper->GetSymbolLookupMethod());

		m_DriverWrapperMap.insert(std::make_pair (sLibraryName, pLibraryWrapper));
	}
	

	PDriver pDriver = std::make_shared <CDriver>(sName, sType, pLibraryWrapper, pDriverResourcePackage, pParameterGroup, m_pEnvironmentWrapper, pInternalEnvironment);
	m_DriverList.push_back(pDriver);
	m_DriverMap.insert(std::make_pair(sName, pDriver));	

	pDriver->configureDriver (sDriverConfigurationData);

	pInternalEnvironment->setIsInitializing(false);
}

CDriver* CDriverHandler::findDriver(const std::string& sName, bool bFailIfNotExisting)
{
	auto iIterator = m_DriverMap.find(sName);

	if (iIterator != m_DriverMap.end())
		return iIterator->second.get();

	if (bFailIfNotExisting)
		throw ELibMCCustomException(LIBMC_ERROR_DRIVERNOTFOUND, sName);

	return nullptr;
}


HDriverHandle CDriverHandler::acquireDriver(const std::string& sName, const std::string& sInstanceName)
{
	std::lock_guard<std::mutex> lockGuard(m_Mutex);

	auto pDriver = findDriver(sName, true);
	return pDriver->acquireDriverHandle(sInstanceName);

}

PParameterGroup CDriverHandler::getDriverParameterGroup(const std::string& sName)
{
	std::lock_guard<std::mutex> lockGuard(m_Mutex);

	auto pDriver = findDriver(sName, true);
	return pDriver->getParameterGroup();

}


void CDriverHandler::releaseDriverLocks(const std::string& sInstanceName)
{
	std::lock_guard<std::mutex> lockGuard(m_Mutex);
	for (auto pDriver : m_DriverList) {
		pDriver->releaseDriverHandle(sInstanceName);
	}

}

void CDriverHandler::GetDriverInformation(const std::string& sName, std::string& sType, HSymbolLookupHandle& pSymbolLookup)
{
	std::lock_guard<std::mutex> lockGuard(m_Mutex);
	auto pDriver = findDriver(sName, true);

	sType = pDriver->getType();
	pSymbolLookup = pDriver->getSymbolLookup();
}


void CDriverHandler::setTempBasePath(const std::string& sTempBasePath)
{
	m_sTempBasePath = sTempBasePath;
}
