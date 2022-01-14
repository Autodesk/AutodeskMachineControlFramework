/*++

Copyright (C) 2020 Autodesk Inc.

All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/


#ifndef __LIBMCDRIVER_OPCUA_SDK
#define __LIBMCDRIVER_OPCUA_SDK

#include "libmcdriver_opcua_types.hpp"

#include <memory>
#include <string>

#ifdef _WIN32

#define OPCUA_CALLINGCONVENTION __stdcall

#else

#define OPCUA_CALLINGCONVENTION 

#endif

typedef void opcUA_Client;
typedef void opcUA_EndpointDescription;
typedef void opcUA_DataType;

typedef uint8_t opcUA_Byte;
typedef uint32_t opcUA_UInt32;

typedef opcUA_Client* opcUA_ClientP;
typedef uint32_t opcUA_StatusCode;

typedef enum {
	UA_LOGLEVEL_TRACE = 0,
	UA_LOGLEVEL_DEBUG,
	UA_LOGLEVEL_INFO,
	UA_LOGLEVEL_WARNING,
	UA_LOGLEVEL_ERROR,
	UA_LOGLEVEL_FATAL
} opcUA_LogLevel;

typedef enum {
	UA_LOGCATEGORY_NETWORK = 0,
	UA_LOGCATEGORY_SECURECHANNEL,
	UA_LOGCATEGORY_SESSION,
	UA_LOGCATEGORY_SERVER,
	UA_LOGCATEGORY_CLIENT,
	UA_LOGCATEGORY_USERLAND,
	UA_LOGCATEGORY_SECURITYPOLICY,
	UA_LOGCATEGORY_EVENTLOOP
} opcUA_LogCategory;


typedef struct {
	void (*log)(void* logContext, opcUA_LogLevel level, opcUA_LogCategory category, const char* msg, va_list args);
	void* context;
	void (*clear)(void* context);
} opcUA_Logger;

#define CLIENT_CONFIG_BUFFERSIZE 16384

typedef struct {
	void* clientContext;
	opcUA_Logger logger;
	opcUA_UInt32 timeout;
	opcUA_Byte ReservedBuffer[CLIENT_CONFIG_BUFFERSIZE];

} opcUA_ClientConfig;

typedef struct {
	size_t length;
	opcUA_Byte* data;
} opcUA_String;

namespace LibMCDriver_OPCUA {
	namespace Impl {

               
		typedef opcUA_ClientP (OPCUA_CALLINGCONVENTION *POpen62541Ptr_UA_Client_newWithConfig) (const opcUA_ClientConfig* config);
        typedef void(OPCUA_CALLINGCONVENTION *POpen62541Ptr_UA_Client_delete) (opcUA_ClientP client);
        typedef opcUA_StatusCode(OPCUA_CALLINGCONVENTION *POpen62541Ptr_UA_Client_getEndpoints) (opcUA_ClientP client, const char* serverUrl, size_t* endpointDescriptionsSize, opcUA_EndpointDescription** endpointDescriptions);
		typedef opcUA_StatusCode(OPCUA_CALLINGCONVENTION* POpen62541Ptr_UA_Client_connect) (opcUA_ClientP client, const char* endpointUrl);
		typedef void(OPCUA_CALLINGCONVENTION* POpen62541Ptr_UA_clear) (void *ptr, const opcUA_DataType* type);
		typedef opcUA_String(OPCUA_CALLINGCONVENTION* POpen62541Ptr_UA_String_fromChars) (const char* src);
		typedef opcUA_StatusCode(OPCUA_CALLINGCONVENTION* POpen62541Ptr_UA_ClientConfig_setDefault) (opcUA_ClientConfig* config);
		typedef opcUA_StatusCode(OPCUA_CALLINGCONVENTION* POpen62541Ptr_UA_ClientConfig_setDefaultEncryption) (opcUA_ClientConfig* config, opcUA_String localCertificate, opcUA_String privateKey, const opcUA_String* trustList, size_t trustListSize, const opcUA_String* revocationList, size_t revocationListSize);
		

		class COpen62541SDK {
		private:
			void* m_LibraryHandle;
			void resetFunctionPtrs ();
		public:

            POpen62541Ptr_UA_Client_newWithConfig UA_Client_newWithConfig = nullptr;
            POpen62541Ptr_UA_Client_delete UA_Client_delete = nullptr;
			POpen62541Ptr_UA_Client_getEndpoints UA_Client_getEndpoints = nullptr;
			POpen62541Ptr_UA_Client_connect UA_Client_connect = nullptr;
			POpen62541Ptr_UA_clear UA_clear = nullptr;
			POpen62541Ptr_UA_String_fromChars UA_String_fromChars  = nullptr;
			POpen62541Ptr_UA_ClientConfig_setDefault UA_ClientConfig_setDefault = nullptr;
			//POpen62541Ptr_UA_ClientConfig_setDefaultEncryption UA_ClientConfig_setDefaultEncryption = nullptr;

			COpen62541SDK(const std::string & sDLLNameUTF8);
			~COpen62541SDK();

			void checkError(opcUA_StatusCode statusCode);

		};


		typedef std::shared_ptr<COpen62541SDK> POpen62541SDK;

	} 
} 

#endif // __LIBMCDRIVER_OPCUA_SDK
