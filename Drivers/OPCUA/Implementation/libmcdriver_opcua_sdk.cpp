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

#include "libmcdriver_opcua_sdk.hpp"
#include "libmcdriver_opcua_interfaceexception.hpp"

#ifdef _WIN32
#include <windows.h>
#else // _WIN32
#include <dlfcn.h>
#endif // _WIN32

#include <vector>

using namespace LibMCDriver_OPCUA::Impl;


#define OPCUA_MAXDLLNAMELENGTH 1024 * 1024

#ifdef _WIN32
void* _loadOpen62541Address (HMODULE hLibrary, const char * pSymbolName) {
	void * pFuncPtr = (void*) GetProcAddress(hLibrary, pSymbolName);
	if (pFuncPtr == nullptr)
		throw ELibMCDriver_OPCUAInterfaceException(LIBMCDRIVER_OPCUA_ERROR_COULDNOTFINDLIBRARYEXPORT);

	return pFuncPtr;
}
#else
void* _loadOpen62541Address(void * hLibrary, const char* pSymbolName) {
	void* pFuncPtr = (void*) dlsym(hLibrary, pSymbolName);
	dlerror();
	if (pFuncPtr == nullptr)
		throw ELibMCDriver_OPCUAInterfaceException(LIBMCDRIVER_OPCUA_ERROR_COULDNOTFINDLIBRARYEXPORT);

	return pFuncPtr;
}
#endif


COpen62541SDK::COpen62541SDK(const std::string& sDLLNameUTF8)
	: m_LibraryHandle (nullptr), m_bIsInitialized (false)
{

	resetFunctionPtrs();

#ifdef _WIN32

	if (sDLLNameUTF8.length() > OPCUA_MAXDLLNAMELENGTH)
		throw ELibMCDriver_OPCUAInterfaceException(LIBMCDRIVER_OPCUA_ERROR_INVALIDPARAM);

	// Convert filename to UTF16-string
	int nLength = (int)sDLLNameUTF8.length ();
	int nBufferSize = nLength * 2 + 2;
	std::vector<wchar_t> wsLibraryFileName(nBufferSize);
	int nResult = MultiByteToWideChar(CP_UTF8, 0, sDLLNameUTF8.c_str(), nLength, &wsLibraryFileName[0], nBufferSize);
	if (nResult == 0)
		throw ELibMCDriver_OPCUAInterfaceException(LIBMCDRIVER_OPCUA_ERROR_COULDNOTLOADLIBRARY);

	HMODULE hLibrary = LoadLibraryW(wsLibraryFileName.data());
	if (hLibrary == 0)
		throw ELibMCDriver_OPCUAInterfaceException(LIBMCDRIVER_OPCUA_ERROR_COULDNOTLOADLIBRARY);
#else // _WIN32
	void* hLibrary = dlopen(sDLLNameUTF8.c_str(), RTLD_LAZY);
	if (hLibrary == 0) {
		dlerror();
		throw ELibMCDriver_OPCUAInterfaceException(LIBMCDRIVER_OPCUA_ERROR_COULDNOTLOADLIBRARY);
	}
#endif // _WIN32

	this->UA_Client_newWithConfig = (POpen62541Ptr_UA_Client_newWithConfig)_loadOpen62541Address(hLibrary, "UA_Client_newWithConfig");
	this->UA_Client_delete = (POpen62541Ptr_UA_Client_delete)_loadOpen62541Address(hLibrary, "UA_Client_delete");

	m_LibraryHandle = (void*) hLibrary;
}


COpen62541SDK::~COpen62541SDK()
{

	if (m_LibraryHandle != nullptr) {
#ifdef _WIN32
		HMODULE hModule = (HMODULE)m_LibraryHandle;
		FreeLibrary(hModule);
#else // _WIN32
		dlclose(m_LibraryHandle);
#endif // _WIN32

		m_LibraryHandle = nullptr;
		resetFunctionPtrs();
	}
}


void COpen62541SDK::checkError(uint32_t nUAError) {
	if (nUAError != 0)
		throw std::runtime_error("UA Error: " + std::to_string (nUAError));
}

void COpen62541SDK::resetFunctionPtrs()
{
	UA_Client_newWithConfig = nullptr;
	UA_Client_delete = nullptr;
}

