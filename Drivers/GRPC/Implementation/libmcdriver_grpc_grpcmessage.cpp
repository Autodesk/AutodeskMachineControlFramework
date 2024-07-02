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


Abstract: This is a stub class definition of CGRPCMessage

*/

#include "libmcdriver_grpc_grpcmessage.hpp"
#include "libmcdriver_grpc_interfaceexception.hpp"

// Include custom headers here.


using namespace LibMCDriver_GRPC::Impl;

/*************************************************************************************************************************
 Class definition of CGRPCMessage 
**************************************************************************************************************************/

CGRPCMessage::CGRPCMessage(LibGRPCWrapper::PWrapper pWrapper, LibGRPCWrapper::PMessage pMessage)
	: m_pWrapper (pWrapper), m_pMessage (pMessage)
{
	if (pWrapper.get () == nullptr)
		throw ELibMCDriver_GRPCInterfaceException(LIBMCDRIVER_GRPC_ERROR_INVALIDPARAM);
	if (pMessage.get() == nullptr)
		throw ELibMCDriver_GRPCInterfaceException(LIBMCDRIVER_GRPC_ERROR_INVALIDPARAM);
}

CGRPCMessage::~CGRPCMessage()
{
	m_pMessage = nullptr;
	m_pWrapper = nullptr;
}


bool CGRPCMessage::HasField(const std::string & sFieldName)
{
	return m_pMessage->HasField(sFieldName);
}

bool CGRPCMessage::HasMessageField(const std::string& sFieldName)
{
	return m_pMessage->HasMessageField(sFieldName);
}

IGRPCMessage* CGRPCMessage::GetMessageField(const std::string& sFieldName)
{
	auto pSubMessage = m_pMessage->GetMessageField(sFieldName);
	return new CGRPCMessage(m_pWrapper, pSubMessage);
}


bool CGRPCMessage::HasStringField(const std::string & sFieldName)
{
	return m_pMessage->HasStringField(sFieldName);
}

void CGRPCMessage::SetStringField(const std::string & sFieldName, const std::string & sValue)
{
	m_pMessage->SetStringField(sFieldName, sValue);
}

std::string CGRPCMessage::GetStringField(const std::string & sFieldName)
{
	return m_pMessage->GetStringField(sFieldName);
}

void CGRPCMessage::SetInt32Field(const std::string & sFieldName, const LibMCDriver_GRPC_int32 nValue)
{
	m_pMessage->SetInt32Field(sFieldName, nValue);
}

LibMCDriver_GRPC_int32 CGRPCMessage::GetInt32Field(const std::string & sFieldName)
{
	return m_pMessage->GetInt32Field(sFieldName);
}

void CGRPCMessage::SetUInt32Field(const std::string & sFieldName, const LibMCDriver_GRPC_uint32 nValue)
{
	m_pMessage->SetUInt32Field(sFieldName, nValue);
}

LibMCDriver_GRPC_uint32 CGRPCMessage::GetUInt32Field(const std::string & sFieldName)
{
	return m_pMessage->GetUInt32Field(sFieldName);
}

void CGRPCMessage::SetInt64Field(const std::string & sFieldName, const LibMCDriver_GRPC_int64 nValue)
{
	m_pMessage->SetInt64Field(sFieldName, nValue);
}

LibMCDriver_GRPC_int64 CGRPCMessage::GetInt64Field(const std::string & sFieldName)
{
	return m_pMessage->GetInt64Field(sFieldName);
}

void CGRPCMessage::SetUInt64Field(const std::string & sFieldName, const LibMCDriver_GRPC_uint64 nValue)
{
	m_pMessage->SetUInt64Field(sFieldName, nValue);
}

LibMCDriver_GRPC_uint64 CGRPCMessage::GetUInt64Field(const std::string & sFieldName)
{
	return m_pMessage->GetUInt64Field(sFieldName);
}

void CGRPCMessage::SetBoolField(const std::string & sFieldName, const bool bValue)
{
	m_pMessage->SetBoolField(sFieldName, bValue);
}

bool CGRPCMessage::GetBoolField(const std::string & sFieldName)
{
	return m_pMessage->GetBoolField(sFieldName);
}

void CGRPCMessage::SetFloatField(const std::string & sFieldName, const LibMCDriver_GRPC_single fValue)
{
	m_pMessage->SetFloatField(sFieldName, fValue);
}

LibMCDriver_GRPC_single CGRPCMessage::GetFloatField(const std::string & sFieldName)
{
	return m_pMessage->GetFloatField(sFieldName);
}

void CGRPCMessage::SetDoubleField(const std::string & sFieldName, const LibMCDriver_GRPC_double dValue)
{
	m_pMessage->SetDoubleField(sFieldName, dValue);
}

LibMCDriver_GRPC_double CGRPCMessage::GetDoubleField(const std::string & sFieldName)
{
	return m_pMessage->GetDoubleField(sFieldName);
}

LibGRPCWrapper::PMessage CGRPCMessage::getMessage()
{
	return m_pMessage;
}

LibGRPCWrapper::PWrapper CGRPCMessage::getWrapper()
{
	return m_pWrapper;
}
