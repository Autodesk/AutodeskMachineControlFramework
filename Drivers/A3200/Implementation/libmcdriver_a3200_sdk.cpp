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

#include "libmcdriver_a3200_sdk.hpp"
#include "libmcdriver_a3200_interfaceexception.hpp"

#ifdef _WIN32
#include <windows.h>
#else // _WIN32
#include <dlfcn.h>
#endif // _WIN32

#include <vector>

using namespace LibMCDriver_A3200::Impl;

#define A3200_MAXDLLNAMELENGTH 1024 * 1024

#ifdef _WIN32
void* _loadA3200Address (HMODULE hLibrary, const std::string & sSymbolName, uint32_t nParameterByteCount) {

	std::string sAnnotatedSymbolName;
	if (sizeof(uint32_t) == sizeof(intptr_t)) { // On 32 Bit Windows, DLL names are annotated
		sAnnotatedSymbolName = "_" + sSymbolName + "@" + std::to_string(nParameterByteCount);
	}
	else {
		sAnnotatedSymbolName = sSymbolName;
	}

	void * pFuncPtr = (void*) GetProcAddress(hLibrary, sAnnotatedSymbolName.c_str ());
	if (pFuncPtr == nullptr)
		throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_COULDNOTFINDLIBRARYEXPORT);

	return pFuncPtr;
}
#else
void* _loadA3200Address(void * hLibrary, const char* pSymbolName, uint32_t nParameterByteCount) {
	void* pFuncPtr = (void*) dlsym(hLibrary, pSymbolName);
	dlerror();
	if (pFuncPtr == nullptr)
		throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_COULDNOTFINDLIBRARYEXPORT);

	return pFuncPtr;
}
#endif




CA3200SDK::CA3200SDK(const std::string& sDLLNameUTF8, const std::string& sDLLDirectory)
	: m_LibraryHandle (nullptr)
{

	resetFunctionPtrs();

#ifdef _WIN32

	if (sDLLNameUTF8.length() > A3200_MAXDLLNAMELENGTH)
		throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_INVALIDPARAM);

	// Convert filename to UTF16-string
	int nLength = (int)sDLLNameUTF8.length ();
	int nBufferSize = nLength * 2 + 2;
	std::vector<wchar_t> wsLibraryFileName(nBufferSize);
	int nResult = MultiByteToWideChar(CP_UTF8, 0, sDLLNameUTF8.c_str(), nLength, &wsLibraryFileName[0], nBufferSize);
	if (nResult == 0)
		throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_COULDNOTLOADLIBRARY);

	int nPathLength = (int)sDLLDirectory.length();
	int nPathBufferSize = nPathLength * 2 + 2;
	std::vector<wchar_t> wsDLLPath(nPathBufferSize);
	int nPathResult = MultiByteToWideChar(CP_UTF8, 0, sDLLDirectory.c_str(), nPathLength, &wsDLLPath[0], nPathBufferSize);
	if (nPathResult == 0)
		throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_COULDNOTLOADLIBRARY);

	m_sDLLDirectoryW = std::wstring(wsDLLPath.data());

	auto pDirectoryCache = cacheDllDirectory();

	HMODULE hLibrary = LoadLibraryW(wsLibraryFileName.data());
	if (hLibrary == 0)
		throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_COULDNOTLOADLIBRARY);
#else // _WIN32
	void* hLibrary = dlopen(sDLLNameUTF8.c_str(), RTLD_LAZY);
	if (hLibrary == 0) {
		dlerror();
		throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_COULDNOTLOADLIBRARY);
	}
#endif // _WIN32

	this->A3200Connect = (PA3200Connect)_loadA3200Address(hLibrary, "A3200Connect", 0);
	this->A3200Disconnect = (PA3200Disconnect)_loadA3200Address(hLibrary, "A3200Disconnect", 0);
	this->A3200Reset = (PA3200Reset)_loadA3200Address(hLibrary, "A3200Reset", 0);
	this->A3200IsInitialized = (PA3200IsInitialized)_loadA3200Address(hLibrary, "A3200IsInitialized", 0);
	this->A3200GetLastError = (PA3200GetLastError)_loadA3200Address(hLibrary, "A3200GetLastError", 0);
	this->A3200GetLastErrorString = (PA3200GetLastErrorString)_loadA3200Address(hLibrary, "A3200GetLastErrorString", 0);
	this->A3200ProgramRun = (PA3200ProgramRun)_loadA3200Address(hLibrary, "A3200ProgramRun", 0);
	this->A3200ProgramStopAndWait = (PA3200ProgramStopAndWait)_loadA3200Address(hLibrary, "A3200ProgramStopAndWait", 0);
	this->A3200CommandExecute = (PA3200CommandExecute)_loadA3200Address(hLibrary, "A3200CommandExecute", 0);
	this->A3200VariableGetValueStringByName = (PA3200VariableGetValueStringByName)_loadA3200Address(hLibrary, "A3200VariableGetValueStringByName", 0);
	this->A3200VariableSetValueStringByName = (PA3200VariableSetValueStringByName)_loadA3200Address(hLibrary, "A3200VariableSetValueStringByName", 0);
	this->A3200VariableGetValueByName = (PA3200VariableGetValueByName)_loadA3200Address(hLibrary, "A3200VariableGetValueByName", 0);
	this->A3200VariableSetValueByName = (PA3200VariableSetValueByName)_loadA3200Address(hLibrary, "A3200VariableSetValueByName", 0);
	this->A3200InformationGetLibraryVersion = (PA3200InformationGetLibraryVersion)_loadA3200Address(hLibrary, "A3200InformationGetLibraryVersion", 0);

	m_LibraryHandle = (void*) hLibrary;
}


CA3200SDK::~CA3200SDK()
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


void CA3200SDK::checkError(A3200Bool bSuccess)
{
	if (!bSuccess) {
		if (A3200GetLastError != nullptr) {
			A3200ErrorData errorData = A3200GetLastError();
			std::string sMessage = "A3200 Error #" + std::to_string(errorData.m_nCode) + " at location " + std::to_string(errorData.m_nLocation);
			
			if (A3200GetLastErrorString != nullptr) {
				std::vector<char> buffer;
				buffer.resize(65536);
				if (A3200GetLastErrorString(buffer.data(), 65535)) {
					buffer.at(65535) = 0;
					sMessage += ": " + std::string(buffer.data());
				}
			}

			throw ELibMCDriver_A3200InterfaceException (errorData.m_nCode, sMessage);
		}
		else {
			throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_GENERICEXCEPTION, "Unknown A3200 Error");
		}
	}
}

void CA3200SDK::resetFunctionPtrs()
{
	A3200Connect = nullptr;
	A3200Disconnect = nullptr;
	A3200Reset = nullptr;
	A3200IsInitialized = nullptr;
	A3200GetLastError = nullptr;
	A3200GetLastErrorString = nullptr;
	A3200ProgramRun = nullptr;
	A3200ProgramStopAndWait = nullptr;
	A3200CommandExecute = nullptr;
	A3200VariableGetValueStringByName = nullptr;
	A3200VariableSetValueStringByName = nullptr;
	A3200VariableGetValueByName = nullptr;
	A3200VariableSetValueByName = nullptr;
	A3200InformationGetLibraryVersion = nullptr;

}

PA3200SDK_DLLDirectoryCache CA3200SDK::cacheDllDirectory()
{
	auto pCache = std::make_shared<CA3200SDK_DLLDirectoryCache>();

#ifdef _WIN32
	SetDllDirectoryW(m_sDLLDirectoryW.c_str());
	SetCurrentDirectoryW(m_sDLLDirectoryW.c_str());
#endif // _WIN32

	return pCache;

}


CA3200SDK_DLLDirectoryCache::CA3200SDK_DLLDirectoryCache()
{
#ifdef _WIN32
	std::vector<wchar_t> buffer;
	buffer.resize(MAX_PATH + 1);
	GetDllDirectoryW(MAX_PATH, buffer.data());

	buffer.at(MAX_PATH) = 0;
	m_sCachedDLLDirectoryW = std::wstring(buffer.data());
#endif // _WIN32
}

CA3200SDK_DLLDirectoryCache::~CA3200SDK_DLLDirectoryCache()
{
#ifdef _WIN32
	if (!m_sCachedDLLDirectoryW.empty()) {
		SetDllDirectoryW(m_sCachedDLLDirectoryW.c_str());
	}
#endif // _WIN32
}
