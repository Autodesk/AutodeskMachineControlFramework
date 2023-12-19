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


#ifndef __LIBMCDRIVER_A3200_SDK
#define __LIBMCDRIVER_A3200_SDK

#include <memory>
#include <string>

#ifdef _WIN32

#define A3200_CALLINGCONVENTION __stdcall

#else

#define A3200_CALLINGCONVENTION 

#endif


namespace LibMCDriver_A3200 {
	namespace Impl {

		typedef void* A3200Handle;
		typedef uint32_t A3200Bool;

#pragma	pack(push, 1)
		typedef struct _A3200ErrorData
		{
			int32_t m_nCode;
			int32_t m_nLocation; 
		} A3200ErrorData;


		typedef struct _A3200Version
		{
			uint32_t m_nMajor;
			uint32_t m_nMinor;
			uint32_t m_nPatch;
			uint32_t m_nBuild;
			uint32_t m_nReserved[4];
		} A3200Version;

#pragma	pack(pop)

		
		typedef A3200Bool(A3200_CALLINGCONVENTION* PA3200InformationGetLibraryVersion) (A3200Version * pVersion);

		typedef A3200Bool(A3200_CALLINGCONVENTION* PA3200Connect) (A3200Handle* ppHandle);
		typedef A3200Bool(A3200_CALLINGCONVENTION* PA3200Disconnect) (A3200Handle pHandle);
		typedef A3200Bool(A3200_CALLINGCONVENTION* PA3200Reset) (A3200Handle pHandle);
		typedef A3200Bool(A3200_CALLINGCONVENTION* PA3200IsInitialized) (A3200Bool pIsInitialized);
		typedef A3200ErrorData(A3200_CALLINGCONVENTION* PA3200GetLastError) ();
		typedef A3200Bool(A3200_CALLINGCONVENTION* PA3200GetLastErrorString) (char * pszBuffer, uint32_t nBufferSize);
		typedef A3200Bool(A3200_CALLINGCONVENTION* PA3200ProgramRun) (A3200Handle pHandle, uint32_t nTaskID, const char * pszFilePath);
		typedef A3200Bool(A3200_CALLINGCONVENTION* PA3200ProgramStopAndWait) (A3200Handle pHandle, uint32_t nTaskID, uint32_t nTimeOutInMS);

		typedef A3200Bool(A3200_CALLINGCONVENTION* PA3200CommandExecute) (A3200Handle pHandle, uint32_t nTaskID, const char* pszCommand, double * pdReturnValue);
		typedef A3200Bool(A3200_CALLINGCONVENTION* PA3200VariableGetValueStringByName) (A3200Handle pHandle, uint32_t nTaskID, const char* pszVariableName, char * pszBuffer, uint32_t nBufferSize);
		typedef A3200Bool(A3200_CALLINGCONVENTION* PA3200VariableSetValueStringByName) (A3200Handle pHandle, uint32_t nTaskID, const char* pszVariableName, const char* pszValue);
		typedef A3200Bool(A3200_CALLINGCONVENTION* PA3200VariableGetValueByName) (A3200Handle pHandle, uint32_t nTaskID, const char* pszVariableName, double * pdValue);
		typedef A3200Bool(A3200_CALLINGCONVENTION* PA3200VariableSetValueByName) (A3200Handle pHandle, uint32_t nTaskID, const char* pszVariableName, double dValue);

		class CA3200SDK_DLLDirectoryCache {
		private:
#ifdef _WIN32
			std::wstring m_sCachedDLLDirectoryW;
#endif // _WIN32

		public:
			CA3200SDK_DLLDirectoryCache();
			virtual ~CA3200SDK_DLLDirectoryCache();

		};

		typedef std::shared_ptr<CA3200SDK_DLLDirectoryCache> PA3200SDK_DLLDirectoryCache;

		class CA3200SDK {
		private:
			void* m_LibraryHandle;		
			std::wstring m_sDLLDirectoryW;

			void resetFunctionPtrs ();

			PA3200SDK_DLLDirectoryCache CA3200SDK::cacheDllDirectory();

		public:
			PA3200Connect A3200Connect = nullptr;
			PA3200Disconnect A3200Disconnect = nullptr;
			PA3200Reset A3200Reset = nullptr;
			PA3200IsInitialized A3200IsInitialized = nullptr;
			PA3200GetLastError A3200GetLastError = nullptr;
			PA3200GetLastErrorString A3200GetLastErrorString = nullptr;
			PA3200ProgramRun A3200ProgramRun = nullptr;
			PA3200ProgramStopAndWait A3200ProgramStopAndWait = nullptr;
			PA3200CommandExecute A3200CommandExecute = nullptr;
			PA3200VariableGetValueStringByName A3200VariableGetValueStringByName = nullptr;
			PA3200VariableSetValueStringByName A3200VariableSetValueStringByName = nullptr;
			PA3200VariableGetValueByName A3200VariableGetValueByName = nullptr;
			PA3200VariableSetValueByName A3200VariableSetValueByName = nullptr;
			PA3200InformationGetLibraryVersion A3200InformationGetLibraryVersion = nullptr;

			CA3200SDK(const std::string & sDLLNameUTF8, const std::string & sDLLDirectory);
			~CA3200SDK();

			void checkError(A3200Bool bSuccess);

		};


		typedef std::shared_ptr<CA3200SDK> PA3200SDK;



	} 
} 


#endif // __LIBMCDRIVER_A3200_SDK
