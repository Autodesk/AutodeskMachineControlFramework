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


Abstract: This is a stub class definition of CGRPCResponse

*/

#include "libmcdriver_grpc_grpcresponse.hpp"
#include "libmcdriver_grpc_interfaceexception.hpp"

// Include custom headers here.


using namespace LibMCDriver_GRPC::Impl;

/*************************************************************************************************************************
 Class definition of CGRPCResponse 
**************************************************************************************************************************/

CGRPCResponse::CGRPCResponse(LibGRPCWrapper::PWrapper pWrapper, LibGRPCWrapper::PResponse pResponse)
    : CGRPCMessage (pWrapper, pResponse)
{

}

CGRPCResponse::~CGRPCResponse()
{

}

std::string CGRPCResponse::GetResponseType()
{
    auto pResponse = getResponse();
    return pResponse->GetResponseType();
}

LibGRPCWrapper::CResponse* CGRPCResponse::getResponse()
{
    auto pResponse = dynamic_cast<LibGRPCWrapper::CResponse*> (getMessage().get ());
    if (pResponse == nullptr)
        throw ELibMCDriver_GRPCInterfaceException(LIBMCDRIVER_GRPC_ERROR_COULDNOTCASTTOGRPCRESPONSE);

    return pResponse;

}


