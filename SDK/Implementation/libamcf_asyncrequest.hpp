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


Abstract: This is the class declaration of COperationResult

*/


#ifndef __LIBAMCF_ASYNCREQUEST
#define __LIBAMCF_ASYNCREQUEST

#include <future>
#include <functional>
#include <memory>
#include <string>
#include <mutex>
#include <map>

class CAsyncResult {
public:
    virtual ~CAsyncResult()
    {

    }
};

class CAsyncSuccess : public CAsyncResult {
public:
};


class CAsyncError : public CAsyncResult {
protected:
    std::string m_sMessage;
public:
    CAsyncError(const std::string& sMessage)
        : m_sMessage (sMessage)
    {

    }

    std::string getMessage() const {
        return m_sMessage;
    }

};


typedef std::shared_ptr<CAsyncResult> PAsyncResult;
typedef std::function<void(CAsyncResult*)> AsyncResultCallback;
typedef uint64_t AsyncRequestID;

class CAsyncRequestHandler;

class CAsyncRequest
{
protected:   

    AsyncRequestID m_nID;

    std::promise<PAsyncResult> m_Promise;

    AsyncResultCallback m_pCallback;

public:

    CAsyncRequest();

    std::future<PAsyncResult> getFuture();

    virtual PAsyncResult onExecute() = 0;

    void setResult(PAsyncResult pResult);

    void setCallback(AsyncResultCallback pCallback);

    AsyncRequestID getID() const;


    friend class CAsyncRequestHandler;
};


typedef std::shared_ptr<CAsyncRequest> PAsyncRequest;


class CAsyncRequestHandler
{
private:
    AsyncRequestID m_nIDCounter;
    std::map <AsyncRequestID, PAsyncRequest> m_CurrentRequests;
    std::mutex m_Mutex;


public:

    CAsyncRequestHandler();
    AsyncRequestID executeRequest(PAsyncRequest pAsyncRequest, AsyncResultCallback pCallback);

    void releaseCallback(AsyncRequestID requestID);

};


typedef std::shared_ptr<CAsyncRequestHandler> PAsyncRequestHandler;



#endif // __LIBAMCF_ASYNCREQUEST