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

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
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

#include "libmcdriver_open62541_client.hpp"
#include "libmcdriver_opcua_interfaceexception.hpp"

#include <vector>
#include <cstring>

using namespace LibMCDriver_OPCUA::Impl;

#define LIBOPEN62541_LOGBUFFERSIZE 1024




void onClientLog(void* logContext, opcUA_LogLevel level, opcUA_LogCategory category, const char* msg, va_list args)
{
	if (!logContext)
		return;
	COpen62541Client* pClient = (COpen62541Client*)logContext;

	size_t bufferSize = LIBOPEN62541_LOGBUFFERSIZE;
	std::vector<char> Buffer;
	Buffer.resize(bufferSize + 1);

	int formatResult = snprintf(Buffer.data(), bufferSize, msg, args);
	if (formatResult < 0)
		return;

	Buffer[bufferSize] = 0;

	pClient->onLog(Buffer.data(), level, category);

}


COpen62541Client::COpen62541Client(POpen62541SDK pSDK, COpen62541ClientLogger* pLogger)
	: m_pSDK (pSDK), m_pLogger (pLogger)
{
	if (pSDK.get() == nullptr)
		throw ELibMCDriver_OPCUAInterfaceException (LIBMCDRIVER_OPCUA_ERROR_INVALIDPARAM);
	

	std::unique_ptr<opcUA_ClientConfig> pConfig (new opcUA_ClientConfig);
	memset(pConfig.get(), 0, sizeof(opcUA_ClientConfig));
	pConfig->logger.context = this;
	pConfig->logger.log = onClientLog;

	m_pSDK->UA_ClientConfig_setDefault(pConfig.get());

	m_Client = m_pSDK->UA_Client_newWithConfig(pConfig.get());
	if (m_Client == nullptr)
		throw std::runtime_error("could not create UA client");

}


COpen62541Client::~COpen62541Client()
{
	if (m_Client && m_pSDK)
		m_pSDK->UA_Client_delete(m_Client);
	m_Client = nullptr;
}


void COpen62541Client::onLog(const std::string& sMessage, opcUA_LogLevel level, opcUA_LogCategory category)
{
	if (m_pLogger)
		m_pLogger->onLog(sMessage, level, category);
}


void COpen62541Client::connect(const std::string& sURL)
{
	opcUA_StatusCode status = m_pSDK->UA_Client_connect(m_Client, sURL.c_str ());
}

