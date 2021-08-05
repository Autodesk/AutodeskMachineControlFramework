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


#ifndef __LIBAMCF_CONNECTIONSTATE
#define __LIBAMCF_CONNECTIONSTATE

#include <memory>
#include <string>
#include <mutex>

#include "libamcf_asyncrequest.hpp"

class CConnectionState
{
private:

    std::string m_sBaseURL;

    std::mutex m_Mutex;
    uint32_t m_nTimeOut;
    uint32_t m_nRetryCount;
    std::string m_sAuthTokenInternal;

    PAsyncRequestHandler m_pRequestHandler;

public:

    CConnectionState(const std::string & sBaseURL);
    ~CConnectionState();

    std::string getBaseURL();
    PAsyncRequestHandler getRequestHandler();

    uint32_t getTimeOut();
    void setTimeOut(uint32_t nValue);

    uint32_t getRetryCount();
    void setRetryCount(uint32_t nValue);

    void setTimeOutAndRetryCount(uint32_t nTimeOut, uint32_t nRetryCount);

    std::string getAuthToken();
    void setAuthToken(const std::string & sAuthToken);

    
};


typedef std::shared_ptr<CConnectionState> PConnectionState;



#endif // __LIBAMCF_CONNECTIONSTATE