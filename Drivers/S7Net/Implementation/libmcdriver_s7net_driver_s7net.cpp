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


Abstract: This is a stub class definition of CDriver_S7Net

*/

#include "libmcdriver_s7net_driver_s7net.hpp"
#include "libmcdriver_s7net_interfaceexception.hpp"
#include "libs7com_abi.hpp"
#include "libs7com_interfaces.hpp"

// Include custom headers here.


using namespace LibMCDriver_S7Net::Impl;

/*************************************************************************************************************************
 Class definition of CDriver_S7Net 
**************************************************************************************************************************/

CDriver_S7Net::CDriver_S7Net(const std::string& sName, const std::string& sType, LibMCEnv::PDriverEnvironment pDriverEnvironment)
    : CDriver (sName, sType), m_pDriverEnvironment (pDriverEnvironment)
{
    if (pDriverEnvironment.get() == nullptr)
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_INVALIDPARAM);

    std::string sLibS7LibraryPath;
    std::string sLibS7LibraryComHostPath;
    std::string sProtocolConfiguration;

    m_pPLCWrapper = LibS7Net::CWrapper::loadLibrary (sLibS7LibraryPath);
    m_pPLC = m_pPLCWrapper->CreatePLC(sLibS7LibraryComHostPath);

    m_pCommunicationWrapper = LibS7Com::CWrapper::loadLibraryFromSymbolLookupMethod(&LibS7Com::Impl::LibS7Com_GetProcAddress);
    m_pCommunicationWrapper->InjectComponent("LibS7Net", m_pPLCWrapper->GetSymbolLookupMethod());
    m_pCommunication = m_pCommunicationWrapper->CreatePLCCommunication();
    m_pCommunication->SetProtocolConfiguration(sProtocolConfiguration);
}

CDriver_S7Net::~CDriver_S7Net()
{
    m_pCommunication = nullptr;
    m_pCommunicationWrapper = nullptr;
    m_pPLC = nullptr;
    m_pPLCWrapper = nullptr;
}

void CDriver_S7Net::Connect()
{
    m_pCommunication->StartCommunication(m_pPLC);
}

void CDriver_S7Net::Disconnect()
{	
    m_pCommunication->StopCommunication();
}

void CDriver_S7Net::QueryParameters()
{
}

