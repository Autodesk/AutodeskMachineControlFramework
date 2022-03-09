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


#ifndef __LIBMCDRIVER_OPEN62541_CLIENT
#define __LIBMCDRIVER_OPEN62541_CLIENT

#include "libmcdriver_opcua_types.hpp"
#include "libmcdriver_opcua_sdk.hpp"

#include <memory>
#include <string>


namespace LibMCDriver_OPCUA {
	namespace Impl {

		class COpen62541ClientLogger {
		public:
			virtual void onLog(const std::string& sMessage, opcUA_LogLevel level, opcUA_LogCategory category) = 0;
		};


		class COpen62541Client {
		private:
			POpen62541SDK m_pSDK;
			opcUA_Client* m_Client;

			COpen62541ClientLogger * m_pLogger;

		public:
			COpen62541Client (POpen62541SDK pSDK, COpen62541ClientLogger * pLogger);
			~COpen62541Client();

			void onLog(const std::string& sMessage, opcUA_LogLevel level, opcUA_LogCategory category);

			void connect(const std::string & sURL);

		};

		typedef std::shared_ptr<COpen62541Client> POpen62541Client;

	} 
} 

#endif // __LIBMCDRIVER_OPEN62541_CLIENT
