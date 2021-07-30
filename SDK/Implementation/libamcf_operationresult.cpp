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


Abstract: This is a stub class definition of COperationResult

*/

#include "libamcf_operationresult.hpp"
#include "libamcf_interfaceexception.hpp"

// Include custom headers here.


using namespace LibAMCF::Impl;

COperationResult::COperationResult(std::future<PAsyncResult> future)
    : m_Future(std::move(future))
{

}

bool COperationResult::WaitFor(const LibAMCF_uint32 nTimeOut)
{
    if (m_CachedResult.get() != nullptr)
        return true;

    if (nTimeOut > 0) {
        m_Future.wait_for(std::chrono::milliseconds(nTimeOut));            
    }
    else {
        m_Future.wait ();
    }

    return m_Future.valid();

}

bool COperationResult::InProgress()
{
    return !m_Future.valid();
}

bool COperationResult::Success()
{
    if (m_CachedResult.get() == nullptr) {
        m_Future.wait();
        m_CachedResult = m_Future.get();
    }

    return dynamic_cast<CAsyncSuccess*> (m_CachedResult.get()) != nullptr;
}

std::string COperationResult::GetErrorMessage()
{
    if (m_CachedResult.get() == nullptr) 
        throw ELibAMCFInterfaceException(LIBAMCF_ERROR_OPERATIONRESULTNOTREADY);

    
    auto pError = dynamic_cast<CAsyncError*> (m_CachedResult.get());

    if (pError != nullptr)
        return pError->getMessage();

    return "";


}
