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


Abstract: This is a stub class definition of CDriver_BuR

*/
// Include custom headers here.
#include <iostream>
#include <brynet/base/AppStatus.hpp>
#include <brynet/net/AsyncConnector.hpp>
#include <brynet/net/TcpService.hpp>
#include <brynet/net/wrapper/ConnectionBuilder.hpp>

#include "libmcdriver_bur_driver_bur.hpp"
#include "libmcdriver_bur_interfaceexception.hpp"


using namespace LibMCDriver_BuR::Impl;

/*************************************************************************************************************************
 Class definition of CDriver_BuR 
**************************************************************************************************************************/

CDriver_BuR::CDriver_BuR(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment)
    : m_sName(sName), m_pDriverEnvironment (pDriverEnvironment), m_nWorkerThreadCount (1), m_nMaxReceiveBufferSize (1024*1024)
{  
    if (pDriverEnvironment.get() == nullptr)
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_INVALIDPARAM);
}



void CDriver_BuR::Configure(const std::string& sConfigurationString)
{

    m_pTcpService = brynet::net::TcpService::Create();
    m_pTcpService->startWorkerThread(m_nWorkerThreadCount);

    m_pAsyncConnector = brynet::net::AsyncConnector::Create();
    m_pAsyncConnector->startWorkerThread();

}

std::string CDriver_BuR::GetName()
{
    return m_sName;
}

std::string CDriver_BuR::GetType()
{
    return "bur-1.0";
}

void CDriver_BuR::GetVersion(LibMCDriver_BuR_uint32& nMajor, LibMCDriver_BuR_uint32& nMinor, LibMCDriver_BuR_uint32& nMicro, std::string& sBuild)
{
    nMajor = 1;
    nMinor = 0;
    nMicro = 0;
}

void CDriver_BuR::GetHeaderInformation(std::string& sNameSpace, std::string& sBaseName)
{
    sNameSpace = "LibMCDriver_BuR";
    sBaseName = "libmcdriver_bur";
}

void CDriver_BuR::QueryParameters()
{
    brynet::base::app_kbhit();

    if (!m_pCurrentConnection.expired()) {
        auto pConnection = m_pCurrentConnection.lock();
        //pConnection->send ();
    }

}

void CDriver_BuR::enterCallback(const std::shared_ptr <brynet::net::TcpConnection> session, const std::string& sIPAddress, const uint32_t nPort)
{

    std::lock_guard<std::mutex> lockGuard(m_ConnectionMutex);

    if (!m_pCurrentConnection.expired()) {        
        auto pConnection = m_pCurrentConnection.lock();
        pConnection->postDisConnect();        
    }


    session->setDataCallback([session](brynet::base::BasePacketReader& reader) 
    {


    });

    m_pCurrentConnection = session->weak_from_this();
    std::cout << "Connected!" << std::endl;
    

}

void CDriver_BuR::failedCallback()
{
    if (!m_pCurrentConnection.expired()) {
        std::lock_guard<std::mutex> lockGuard(m_ConnectionMutex);
        auto pConnection = m_pCurrentConnection.lock();
        pConnection->postDisConnect();
    }
    m_pCurrentConnection.reset();

}


void CDriver_BuR::Connect(const std::string& sIPAddress, const LibMCDriver_BuR_uint32 nPort, const LibMCDriver_BuR_uint32 nTimeout)
{
    brynet::net::wrapper::ConnectionBuilder connectionBuilder;

    connectionBuilder.WithService(m_pTcpService)
        .WithConnector(m_pAsyncConnector)
        .WithMaxRecvBufferSize(m_nMaxReceiveBufferSize)
        .AddEnterCallback([this, sIPAddress, nPort](const brynet::net::TcpConnection::Ptr& session) { enterCallback(session, sIPAddress, nPort); });

    connectionBuilder
        .WithAddr(sIPAddress, nPort)
        .WithTimeout(std::chrono::milliseconds(nTimeout))
        .WithFailedCallback([this]() { failedCallback(); })
        .AddSocketProcessCallback([](brynet::net::TcpSocket& socket) {
            socket.setNodelay();
        })
        .asyncConnect();

}

void CDriver_BuR::Disconnect()
{
    if (!m_pCurrentConnection.expired()) {
        std::lock_guard<std::mutex> lockGuard(m_ConnectionMutex);
        auto pConnection = m_pCurrentConnection.lock();
        pConnection->postDisConnect();
    }
    m_pCurrentConnection.reset();
}
