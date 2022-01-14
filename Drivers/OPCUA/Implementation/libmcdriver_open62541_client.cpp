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

using namespace LibMCDriver_OPCUA::Impl;

#define LIBOPEN62541_LOGBUFFERSIZE 1024

typedef unsigned int UA_UInt32;
typedef unsigned char UA_Byte;
typedef void UA_Client;
typedef UA_Client* UA_ClientP;
typedef uint32_t UA_StatusCode;

typedef enum {
	UA_LOGLEVEL_TRACE = 0,
	UA_LOGLEVEL_DEBUG,
	UA_LOGLEVEL_INFO,
	UA_LOGLEVEL_WARNING,
	UA_LOGLEVEL_ERROR,
	UA_LOGLEVEL_FATAL
} UA_LogLevel;

typedef enum {
	UA_LOGCATEGORY_NETWORK = 0,
	UA_LOGCATEGORY_SECURECHANNEL,
	UA_LOGCATEGORY_SESSION,
	UA_LOGCATEGORY_SERVER,
	UA_LOGCATEGORY_CLIENT,
	UA_LOGCATEGORY_USERLAND,
	UA_LOGCATEGORY_SECURITYPOLICY,
	UA_LOGCATEGORY_EVENTLOOP
} UA_LogCategory;

typedef struct {
	void (*log)(void* logContext, UA_LogLevel level, UA_LogCategory category, const char* msg, va_list args);
	void* context;
	void (*clear)(void* context);
} UA_Logger;

#define CLIENT_CONFIG_BUFFERSIZE 16384

typedef struct {
	void* clientContext;
	UA_Logger logger;
	UA_UInt32 timeout;
	UA_Byte ReservedBuffer[CLIENT_CONFIG_BUFFERSIZE];

} UA_ClientConfig;

typedef struct {
	size_t length;
	UA_Byte* data; /* The content (not null-terminated) */
} UA_String;

void onClientLog(void* logContext, UA_LogLevel level, UA_LogCategory category, const char* msg, va_list args)
{
	if (!logContext)
		return;
	COpen62541Client* pClient = (COpen62541Client*)logContext;

	size_t bufferSize = LIBOPEN62541_LOGBUFFERSIZE;
	std::vector<char> Buffer;
	Buffer.resize(bufferSize + 1);

	int formatResult = sprintf_s(Buffer.data(), bufferSize, msg, args);
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
	

	UA_ClientConfig config;
	config.logger.context = this;
	config.logger.log = onClientLog;

	m_Client = m_pSDK->UA_Client_newWithConfig(&config);
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
