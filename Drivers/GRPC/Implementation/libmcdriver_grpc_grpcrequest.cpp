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


Abstract: This is a stub class definition of CGRPCRequest

*/

#include "libmcdriver_grpc_grpcrequest.hpp"
#include "libmcdriver_grpc_interfaceexception.hpp"

// Include custom headers here.


using namespace LibMCDriver_GRPC::Impl;

/*************************************************************************************************************************
 Class definition of CGRPCRequest 
**************************************************************************************************************************/
CGRPCRequest::CGRPCRequest(LibGRPCWrapper::PWrapper pWrapper, LibGRPCWrapper::PRequest pRequest)
    : CGRPCMessage(pWrapper, pRequest)
{

}

CGRPCRequest::~CGRPCRequest()
{

}

LibGRPCWrapper::CRequest* CGRPCRequest::getRequest()
{
    auto pRequest = dynamic_cast<LibGRPCWrapper::CRequest*> (getMessage().get());
    if (pRequest == nullptr)
        throw ELibMCDriver_GRPCInterfaceException(LIBMCDRIVER_GRPC_ERROR_COULDNOTCASTTOGRPCREQUEST);

    return pRequest;

}

std::string CGRPCRequest::GetRequestType()
{
	throw ELibMCDriver_GRPCInterfaceException(LIBMCDRIVER_GRPC_ERROR_NOTIMPLEMENTED);
}

std::string CGRPCRequest::GetExpectedResponseType()
{
	throw ELibMCDriver_GRPCInterfaceException(LIBMCDRIVER_GRPC_ERROR_NOTIMPLEMENTED);
}

IGRPCResponse * CGRPCRequest::SendBlocking(const std::string & sServiceMethod, const LibMCDriver_GRPC_uint32 nTimeOutInMS)
{
	throw ELibMCDriver_GRPCInterfaceException(LIBMCDRIVER_GRPC_ERROR_NOTIMPLEMENTED);
}

