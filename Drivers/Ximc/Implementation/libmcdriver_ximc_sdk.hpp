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


#ifndef __LIBMCDRIVER_XIMC_SDK
#define __LIBMCDRIVER_XIMC_SDK

#include "libmcdriver_ximc_types.hpp"

#include <memory>
#include <string>

#ifdef _WIN32

#define XIMC_CALLINGCONVENTION __stdcall

#else

#define XIMC_CALLINGCONVENTION 

#endif


namespace LibMCDriver_Ximc {
	namespace Impl {


		typedef void (XIMC_CALLINGCONVENTION *PXimcPtr_ximc_version) (char * version);

		class CXimcSDK {
		private:
			bool m_bIsInitialized;

			void* m_LibraryHandle;
			void resetFunctionPtrs ();
		public:

			PXimcPtr_ximc_version ximc_version = nullptr;

			CXimcSDK(const std::string & sDLLNameUTF8);
			~CXimcSDK();

		};


		typedef std::shared_ptr<CXimcSDK> PXimcSDK;

	} 
} 

#endif // __LIBMCDRIVER_XIMC_SDK
