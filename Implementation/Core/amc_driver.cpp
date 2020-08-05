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
#include "libmc_interfaceexception.hpp"


using namespace AMC;

CDriver::CDriver(const std::string& sName, const std::string& sType, const std::string& sLibrary, LibMCDriverEnv::PDriverEnvironment pDriverEnvironment)
	: m_sName (sName), m_sType (sType), m_sLibrary (sLibrary)
{

	m_pDriverWrapper = LibMCDriver::CWrapper::loadLibrary (sLibrary);
	m_pDriverWrapper->InjectComponent("LibMCDriverEnv", pDriverEnvironment->wrapper()->GetSymbolLookupMethod());
	m_pDriverInstance = m_pDriverWrapper->CreateDriver (sName, sType, pDriverEnvironment.get());
}

CDriver::~CDriver()
{
	m_pDriverInstance = nullptr;
	m_pDriverWrapper = nullptr;
}


HDriverHandle CDriver::acquireDriverHandle(const std::string& sInstanceName)
{		
	if (!m_sReservationInstance.empty())
		throw ELibMCInterfaceException(LIBMC_ERROR_DRIVERALREADYRESERVED);

	m_sReservationInstance = sInstanceName;
	m_pDriverWrapper->AcquireInstance(m_pDriverInstance.get());
	return  m_pDriverInstance->handle();	
}

void CDriver::releaseDriverHandle(const std::string& sInstanceName)
{
	if (m_sReservationInstance == sInstanceName)
		m_sReservationInstance = "";
}


std::string CDriver::getName()
{
	return m_sName;
}

std::string CDriver::getType()
{
	return m_sType;
}


HSymbolLookupHandle CDriver::getSymbolLookup()
{
	return m_pDriverWrapper->GetSymbolLookupMethod();
}