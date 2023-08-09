/*++

Copyright (C) 2023 Autodesk Inc.

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


Abstract: This is a stub class definition of CDriver_ScanLabSMC

*/

#include "libmcdriver_scanlabsmc_driver_scanlabsmc.hpp"
#include "libmcdriver_scanlabsmc_interfaceexception.hpp"
#include "libmcdriver_scanlabsmc_smccontext.hpp"

// Include custom headers here.
#define __STRINGIZE(x) #x
#define __STRINGIZE_VALUE_OF(x) __STRINGIZE(x)


using namespace LibMCDriver_ScanLabSMC::Impl;

/*************************************************************************************************************************
 Class definition of CDriver_ScanLabSMC 
**************************************************************************************************************************/

CDriver_ScanLabSMC::CDriver_ScanLabSMC(const std::string& sName, const std::string& sType, LibMCEnv::PDriverEnvironment pDriverEnvironment)
    : m_sName (sName), m_sType (sType), m_pDriverEnvironment (pDriverEnvironment)
{
    if (pDriverEnvironment.get() == nullptr)
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPARAM);

}

CDriver_ScanLabSMC::~CDriver_ScanLabSMC()
{

}

void CDriver_ScanLabSMC::LoadSDK(const std::string & sSMCResourceName)
{
	
}

ISMCContext * CDriver_ScanLabSMC::CreateContext(const std::string & sConfigurationXML)
{
    return new CSMCContext (sConfigurationXML, nullptr, m_pDriverEnvironment);
}


void CDriver_ScanLabSMC::Configure(const std::string& sConfigurationString)
{
    
}

std::string CDriver_ScanLabSMC::GetName()
{
    return m_sName;
}

std::string CDriver_ScanLabSMC::GetType()
{
    return m_sType;
}

void CDriver_ScanLabSMC::GetVersion(LibMCDriver_ScanLabSMC_uint32& nMajor, LibMCDriver_ScanLabSMC_uint32& nMinor, LibMCDriver_ScanLabSMC_uint32& nMicro, std::string& sBuild)
{
    nMajor = LIBMCDRIVER_SCANLABSMC_VERSION_MAJOR;
    nMinor = LIBMCDRIVER_SCANLABSMC_VERSION_MINOR;
    nMicro = LIBMCDRIVER_SCANLABSMC_VERSION_MICRO;
    sBuild = __STRINGIZE_VALUE_OF(__GITHASH);
}

void CDriver_ScanLabSMC::QueryParameters()
{
    QueryParametersEx(m_pDriverEnvironment->CreateStatusUpdateSession());

}

void CDriver_ScanLabSMC::QueryParametersEx(LibMCEnv::PDriverStatusUpdateSession pDriverUpdateInstance)
{
    
}

