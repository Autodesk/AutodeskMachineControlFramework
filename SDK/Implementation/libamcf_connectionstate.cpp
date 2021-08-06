/*++

Copyright (C) 2021 Autodesk Inc.

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


*/


#include "libamcf_connectionstate.hpp"
#include "libamcf_interfaceexception.hpp"

#define AMCF_MINRETRYCOUNT 1
#define AMCF_MAXRETRYCOUNT 1024
#define AMCF_DEFAULTRETRYCOUNT 3

#define AMCF_MINTIMEOUT 1
#define AMCF_MAXTIMEOUT 65536
#define AMCF_DEFAULTTIMEOUT 1000

CConnectionState::CConnectionState(const std::string& sBaseURL)
    : m_sBaseURL(sBaseURL), m_nTimeOut(AMCF_DEFAULTTIMEOUT), m_nRetryCount(AMCF_DEFAULTRETRYCOUNT)
{
    if (m_sBaseURL.empty())
        throw ELibAMCFInterfaceException(LIBAMCF_ERROR_INVALIDBASEURL);

    if (m_sBaseURL.back() != '/')
        m_sBaseURL = m_sBaseURL + "/";

    m_pRequestHandler = std::make_shared<CAsyncRequestHandler>();

}

CConnectionState::~CConnectionState()
{
}


std::string CConnectionState::getBaseURL()
{
    return m_sBaseURL;
}

PAsyncRequestHandler CConnectionState::getRequestHandler()
{
    return m_pRequestHandler;
}

uint32_t CConnectionState::getTimeOut()
{
    std::lock_guard<std::mutex> lockGuard(m_Mutex);
    return m_nTimeOut;
}

void CConnectionState::setTimeOut(uint32_t nValue)
{
    setTimeOutAndRetryCount(nValue, m_nRetryCount);


}

uint32_t CConnectionState::getRetryCount()
{
    std::lock_guard<std::mutex> lockGuard(m_Mutex);
    return m_nRetryCount;
}

void CConnectionState::setRetryCount(uint32_t nValue)
{
    setTimeOutAndRetryCount(m_nTimeOut, nValue);    
}

void CConnectionState::setTimeOutAndRetryCount(uint32_t nTimeOut, uint32_t nRetryCount)
{
    if (nTimeOut < AMCF_MINTIMEOUT)
        throw ELibAMCFInterfaceException(LIBAMCF_ERROR_INVALIDTIMEOUT);
    if (nTimeOut > AMCF_MAXTIMEOUT)
        throw ELibAMCFInterfaceException(LIBAMCF_ERROR_INVALIDTIMEOUT);
    if (nRetryCount < AMCF_MINRETRYCOUNT)
        throw ELibAMCFInterfaceException(LIBAMCF_ERROR_INVALIDRETRYCOUNT);
    if (nRetryCount > AMCF_MAXRETRYCOUNT)
        throw ELibAMCFInterfaceException(LIBAMCF_ERROR_INVALIDRETRYCOUNT);

    std::lock_guard<std::mutex> lockGuard(m_Mutex);
    m_nTimeOut = nTimeOut;
    m_nRetryCount = nRetryCount;
}


std::string CConnectionState::getAuthToken()
{
    std::lock_guard<std::mutex> lockGuard(m_Mutex);
    return m_sAuthTokenInternal;
}

void CConnectionState::setAuthToken(const std::string& sAuthToken)
{
    std::lock_guard<std::mutex> lockGuard(m_Mutex);
    m_sAuthTokenInternal = sAuthToken;
}

