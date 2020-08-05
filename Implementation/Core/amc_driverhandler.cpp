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

#include "libmcdriverenv_driverenvironment.hpp"
#include "libmc_interfaceexception.hpp"

#include <vector>
#include <memory>
#include <string>
#include <iostream>


using namespace AMC;

CDriverHandler::CDriverHandler(LibMCDriverEnv::PWrapper pEnvironmentWrapper)
	: m_pEnvironmentWrapper (pEnvironmentWrapper)
{
	if (pEnvironmentWrapper.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
}


CDriverHandler::~CDriverHandler()
{

}

template <class C> std::shared_ptr<C> mapInternalDriverEnvInstance(std::shared_ptr<LibMCDriverEnv::Impl::IBase> pImplInstance, LibMCDriverEnv::PWrapper pWrapper)
{
	if (pWrapper.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	auto pExternalInstance = std::make_shared <C>(pWrapper.get(), (LibMCDriverEnv::Impl::IBase*) (pImplInstance.get()));
	pImplInstance->IncRefCount();
	return pExternalInstance;
}


void CDriverHandler::registerDriver(const std::string& sName, const std::string& sType, const std::string& sLibrary)
{
	std::lock_guard<std::mutex> lockGuard(m_Mutex);

	if (findDriver(sName, false) != nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_DRIVERALREADYREGISTERED);

	auto pInternalEnvironment = std::make_shared<LibMCDriverEnv::Impl::CDriverEnvironment>();
	auto pExternalEnvironment = mapInternalDriverEnvInstance<LibMCDriverEnv::CDriverEnvironment>(pInternalEnvironment, m_pEnvironmentWrapper);

	PDriver pDriver = std::make_shared <CDriver>(sName, sType, sLibrary, pExternalEnvironment);
	m_DriverList.push_back(pDriver);
	m_DriverMap.insert(std::make_pair(sName, pDriver));	
}

CDriver* CDriverHandler::findDriver(const std::string& sName, bool bFailIfNotExisting)
{
	auto iIterator = m_DriverMap.find(sName);

	if (iIterator != m_DriverMap.end())
		return iIterator->second.get();

	if (bFailIfNotExisting)
		throw ELibMCInterfaceException(LIBMC_ERROR_DRIVERNOTFOUND);

	return nullptr;
}


HDriverHandle CDriverHandler::acquireDriver(const std::string& sName, const std::string& sInstanceName)
{
	std::lock_guard<std::mutex> lockGuard(m_Mutex);

	auto pDriver = findDriver(sName, true);
	return pDriver->acquireDriverHandle(sInstanceName);

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