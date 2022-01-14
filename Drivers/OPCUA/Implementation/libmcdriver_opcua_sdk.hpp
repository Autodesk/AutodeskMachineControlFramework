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
typedef void opcUA_ClientConfig;
typedef void opcUA_EndpointDescription;

typedef opcUA_Client* opcUA_ClientP;
typedef uint32_t opcUA_StatusCode;
typedef uint32_t opcUA_LogLevel;
typedef uint32_t opcUA_LogCategory;



namespace LibMCDriver_OPCUA {
	namespace Impl {

               
		typedef opcUA_ClientP (OPCUA_CALLINGCONVENTION *POpen62541Ptr_UA_Client_newWithConfig) (const opcUA_ClientConfig* config);
        typedef void(OPCUA_CALLINGCONVENTION *POpen62541Ptr_UA_Client_delete) (opcUA_ClientP client);
        typedef opcUA_StatusCode(OPCUA_CALLINGCONVENTION *POpen62541Ptr_UA_Client_getEndpoints) (opcUA_ClientP client, const char* serverUrl, size_t* endpointDescriptionsSize, opcUA_EndpointDescription** endpointDescriptions);

		class COpen62541SDK {
		private:
			bool m_bIsInitialized;

			void* m_LibraryHandle;
			void resetFunctionPtrs ();
		public:

            POpen62541Ptr_UA_Client_newWithConfig UA_Client_newWithConfig = nullptr;
            POpen62541Ptr_UA_Client_delete UA_Client_delete = nullptr;

			COpen62541SDK(const std::string & sDLLNameUTF8);
			~COpen62541SDK();

			void checkError(uint32_t nUAError);

		};


		typedef std::shared_ptr<COpen62541SDK> POpen62541SDK;

	} 
} 

#endif // __LIBMCDRIVER_OPCUA_SDK
