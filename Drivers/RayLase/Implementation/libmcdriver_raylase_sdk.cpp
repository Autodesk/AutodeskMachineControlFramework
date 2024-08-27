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

#include "libmcdriver_raylase_sdk.hpp"
#include "libmcdriver_raylase_interfaceexception.hpp"

#ifdef _WIN32
#include <windows.h>
#else // _WIN32
#include <dlfcn.h>
#endif // _WIN32

#include <vector>

using namespace LibMCDriver_Raylase::Impl;

#define RAYLASE_MAXDLLNAMELENGTH 1024 * 1024

#ifdef _WIN32
void* _loadRaylaseAddress (HMODULE hLibrary, const std::string & sSymbolName) {

	void * pFuncPtr = (void*) GetProcAddress(hLibrary, sSymbolName.c_str ());
	if (pFuncPtr == nullptr)
		throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_COULDNOTFINDLIBRARYEXPORT);

	return pFuncPtr;
}
#else
void* _loadRaylaseAddress(void * hLibrary, const char* pSymbolName) {
	void* pFuncPtr = (void*) dlsym(hLibrary, pSymbolName);
	dlerror();
	if (pFuncPtr == nullptr)
		throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_COULDNOTFINDLIBRARYEXPORT);

	return pFuncPtr;
}
#endif




CRaylaseSDK::CRaylaseSDK(const std::string& sDLLNameUTF8)
	: m_LibraryHandle (nullptr)
{

	resetFunctionPtrs();

#ifdef _WIN32

	if (sDLLNameUTF8.length() > RAYLASE_MAXDLLNAMELENGTH)
		throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDPARAM);

	// Convert filename to UTF16-string
	int nLength = (int)sDLLNameUTF8.length ();
	int nBufferSize = nLength * 2 + 2;
	std::vector<wchar_t> wsLibraryFileName(nBufferSize);
	int nResult = MultiByteToWideChar(CP_UTF8, 0, sDLLNameUTF8.c_str(), nLength, &wsLibraryFileName[0], nBufferSize);
	if (nResult == 0)
		throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_COULDNOTLOADLIBRARY);

	HMODULE hLibrary = LoadLibraryW(wsLibraryFileName.data());
	if (hLibrary == 0)
		throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_COULDNOTLOADLIBRARY);
#else // _WIN32
	void* hLibrary = dlopen(sDLLNameUTF8.c_str(), RTLD_LAZY);
	if (hLibrary == 0) {
		dlerror();
		throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_COULDNOTLOADLIBRARY);
	}
#endif // _WIN32


	this->rlConnect = (PrlConnect)_loadRaylaseAddress(hLibrary, "rlConnect");
	this->rlIsConnected = (PrlIsConnected)_loadRaylaseAddress(hLibrary, "rlIsConnected");
	this->rlDisconnect = (PrlDisconnect)_loadRaylaseAddress(hLibrary, "rlDisconnect");
	this->rlSystemResetToDefaults = (PrlDisconnect)_loadRaylaseAddress(hLibrary, "rlSystemResetToDefaults");
	this->rlLaserLaserOn = (PrlLaserLaserOn)_loadRaylaseAddress(hLibrary, "rlLaserLaserOn");
	this->rlLaserLaserOff = (PrlLaserLaserOff)_loadRaylaseAddress(hLibrary, "rlLaserLaserOff");
	this->rlLaserIsPilotEnabled = (PrlLaserIsPilotEnabled)_loadRaylaseAddress(hLibrary, "rlLaserIsPilotEnabled");
	this->rlLaserEnablePilot = (PrlLaserEnablePilot)_loadRaylaseAddress(hLibrary, "rlLaserEnablePilot");
	this->rlLaserIsLaserArmed = (PrlLaserIsLaserArmed)_loadRaylaseAddress(hLibrary, "rlLaserIsLaserArmed");
	this->rlLaserArmLaser = (PrlLaserArmLaser)_loadRaylaseAddress(hLibrary, "rlLaserArmLaser");
	this->rlLaserReadLaserStatus = (PrlLaserReadLaserStatus)_loadRaylaseAddress(hLibrary, "rlLaserReadLaserStatus");

	this->rlListAllocate = (PrlListAllocate)_loadRaylaseAddress(hLibrary, "rlListAllocate");
	this->rlListReleaseHandle = (PrlListReleaseHandle)_loadRaylaseAddress(hLibrary, "rlListReleaseHandle");
	this->rlListAppendLaserOn = (PrlListAppendLaserOn)_loadRaylaseAddress(hLibrary, "rlListAppendLaserOn");
	this->rlListAppendLaserOff = (PrlListAppendLaserOff)_loadRaylaseAddress(hLibrary, "rlListAppendLaserOff");
	this->rlListAppendPower = (PrlListAppendPower)_loadRaylaseAddress(hLibrary, "rlListAppendPower");
	this->rlListAppendJumpSpeed = (PrlListAppendJumpSpeed)_loadRaylaseAddress(hLibrary, "rlListAppendJumpSpeed");
	this->rlListAppendMarkSpeed = (PrlListAppendMarkSpeed)_loadRaylaseAddress(hLibrary, "rlListAppendMarkSpeed");
	this->rlListAppendJumpDelay = (PrlListAppendJumpDelay)_loadRaylaseAddress(hLibrary, "rlListAppendJumpDelay");
	this->rlListAppendMinJumpDelay = (PrlListAppendMinJumpDelay)_loadRaylaseAddress(hLibrary, "rlListAppendMinJumpDelay");
	this->rlListAppendMarkDelay = (PrlListAppendMarkDelay)_loadRaylaseAddress(hLibrary, "rlListAppendMarkDelay");
	this->rlListAppendPolyDelay = (PrlListAppendPolyDelay)_loadRaylaseAddress(hLibrary, "rlListAppendPolyDelay");
	this->rlListAppendLaserOnDelay = (PrlListAppendLaserOnDelay)_loadRaylaseAddress(hLibrary, "rlListAppendLaserOnDelay");
	this->rlListAppendLaserOffDelay = (PrlListAppendLaserOffDelay)_loadRaylaseAddress(hLibrary, "rlListAppendLaserOffDelay");
	this->rlListAppendJumpAbs2D = (PrlListAppendJumpAbs2D)_loadRaylaseAddress(hLibrary, "rlListAppendJumpAbs2D");
	this->rlListAppendMarkAbs2D = (PrlListAppendMarkAbs2D)_loadRaylaseAddress(hLibrary, "rlListAppendMarkAbs2D");
	this->rlListSet = (PrlListSet)_loadRaylaseAddress(hLibrary, "rlListSet");
	this->rlListExecute = (PrlListExecute)_loadRaylaseAddress(hLibrary, "rlListExecute");
	this->rlListWaitForListIdle = (PrlListWaitForListIdle)_loadRaylaseAddress(hLibrary, "rlListWaitForListIdle");
	this->rlListWaitForListDone = (PrlListWaitForListDone)_loadRaylaseAddress(hLibrary, "rlListWaitForListDone");
	this->rlListDelete = (PrlListDelete)_loadRaylaseAddress(hLibrary, "rlListDelete");
	this->rlGetLastError = (PrlGetLastError)_loadRaylaseAddress(hLibrary, "rlGetLastError");
	this->rlGetLastErrorLen = (PrlGetLastErrorLen)_loadRaylaseAddress(hLibrary, "rlGetLastErrorLen");

	m_LibraryHandle = (void*) hLibrary;
}


CRaylaseSDK::~CRaylaseSDK()
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


void CRaylaseSDK::checkError(int32_t statusCode, const std::string& sDebugMessage)
{
		if (statusCode != 0) {
		std::string sErrorMessage = "Raylase Error " + std::to_string(statusCode) + " (";
		if ((rlGetLastErrorLen != nullptr) && (rlGetLastError != nullptr)) {
			int32_t nLength = rlGetLastErrorLen();
			if (nLength > 0) {
				std::vector<char> messageBuffer;
				messageBuffer.resize(nLength + 2);

				rlGetLastError(messageBuffer.data(), nLength + 1);
				messageBuffer.at(nLength) = 0;

				sErrorMessage += std::string(messageBuffer.data());
			}
			else {
				sErrorMessage += "<unknown error>";
			}

		}

		if (!sDebugMessage.empty ())
			sErrorMessage += " - " + sDebugMessage;
		sErrorMessage += ")";

		throw std::runtime_error(sErrorMessage);
	}
}

void CRaylaseSDK::resetFunctionPtrs()
{

	rlConnect = nullptr;
	rlIsConnected = nullptr;
	rlDisconnect = nullptr;
	rlSystemResetToDefaults = nullptr;
	rlLaserLaserOn = nullptr;
	rlLaserLaserOff = nullptr;
	rlLaserIsPilotEnabled = nullptr;
	rlLaserEnablePilot = nullptr;
	rlLaserIsLaserArmed = nullptr;
	rlLaserArmLaser = nullptr;
	rlLaserReadLaserStatus = nullptr;
	rlListAllocate = nullptr;
	rlListReleaseHandle = nullptr;
	rlListAppendLaserOn = nullptr;
	rlListAppendLaserOff = nullptr;
	rlListAppendPower = nullptr;
	rlListAppendJumpSpeed = nullptr;
	rlListAppendMarkSpeed = nullptr;
	rlListAppendJumpDelay = nullptr;
	rlListAppendMinJumpDelay = nullptr;
	rlListAppendMarkDelay = nullptr;
	rlListAppendPolyDelay = nullptr;
	rlListAppendLaserOnDelay = nullptr;
	rlListAppendLaserOffDelay = nullptr;
	rlListAppendJumpAbs2D = nullptr;
	rlListAppendMarkAbs2D = nullptr;
	rlListSet = nullptr;
	rlListExecute = nullptr;
	rlListWaitForListIdle = nullptr;
	rlListWaitForListDone = nullptr;
	rlListDelete = nullptr;
	rlGetLastError = nullptr;
	rlGetLastErrorLen = nullptr;

}

