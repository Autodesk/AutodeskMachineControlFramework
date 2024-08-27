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
	: m_LibraryHandle (nullptr), m_bEnableJournal (false)
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


	this->ptrConnect = (PrlConnect)_loadRaylaseAddress(hLibrary, "rlConnect");
	this->ptrIsConnected = (PrlIsConnected)_loadRaylaseAddress(hLibrary, "rlIsConnected");
	this->ptrDisconnect = (PrlDisconnect)_loadRaylaseAddress(hLibrary, "rlDisconnect");
	this->ptrSystemResetToDefaults = (PrlDisconnect)_loadRaylaseAddress(hLibrary, "rlSystemResetToDefaults");
	this->ptrLaserLaserOn = (PrlLaserLaserOn)_loadRaylaseAddress(hLibrary, "rlLaserLaserOn");
	this->ptrLaserLaserOff = (PrlLaserLaserOff)_loadRaylaseAddress(hLibrary, "rlLaserLaserOff");
	this->ptrLaserIsPilotEnabled = (PrlLaserIsPilotEnabled)_loadRaylaseAddress(hLibrary, "rlLaserIsPilotEnabled");
	this->ptrLaserEnablePilot = (PrlLaserEnablePilot)_loadRaylaseAddress(hLibrary, "rlLaserEnablePilot");
	this->ptrLaserIsLaserArmed = (PrlLaserIsLaserArmed)_loadRaylaseAddress(hLibrary, "rlLaserIsLaserArmed");
	this->ptrLaserArmLaser = (PrlLaserArmLaser)_loadRaylaseAddress(hLibrary, "rlLaserArmLaser");
	this->ptrLaserReadLaserStatus = (PrlLaserReadLaserStatus)_loadRaylaseAddress(hLibrary, "rlLaserReadLaserStatus");

	this->ptrListAllocate = (PrlListAllocate)_loadRaylaseAddress(hLibrary, "rlListAllocate");
	this->ptrListReleaseHandle = (PrlListReleaseHandle)_loadRaylaseAddress(hLibrary, "rlListReleaseHandle");
	this->ptrListAppendLaserOn = (PrlListAppendLaserOn)_loadRaylaseAddress(hLibrary, "rlListAppendLaserOn");
	this->ptrListAppendLaserOff = (PrlListAppendLaserOff)_loadRaylaseAddress(hLibrary, "rlListAppendLaserOff");
	this->ptrListAppendPower = (PrlListAppendPower)_loadRaylaseAddress(hLibrary, "rlListAppendPower");
	this->ptrListAppendJumpSpeed = (PrlListAppendJumpSpeed)_loadRaylaseAddress(hLibrary, "rlListAppendJumpSpeed");
	this->ptrListAppendMarkSpeed = (PrlListAppendMarkSpeed)_loadRaylaseAddress(hLibrary, "rlListAppendMarkSpeed");
	this->ptrListAppendJumpDelay = (PrlListAppendJumpDelay)_loadRaylaseAddress(hLibrary, "rlListAppendJumpDelay");
	this->ptrListAppendMinJumpDelay = (PrlListAppendMinJumpDelay)_loadRaylaseAddress(hLibrary, "rlListAppendMinJumpDelay");
	this->ptrListAppendMarkDelay = (PrlListAppendMarkDelay)_loadRaylaseAddress(hLibrary, "rlListAppendMarkDelay");
	this->ptrListAppendPolyDelay = (PrlListAppendPolyDelay)_loadRaylaseAddress(hLibrary, "rlListAppendPolyDelay");
	this->ptrListAppendLaserOnDelay = (PrlListAppendLaserOnDelay)_loadRaylaseAddress(hLibrary, "rlListAppendLaserOnDelay");
	this->ptrListAppendLaserOffDelay = (PrlListAppendLaserOffDelay)_loadRaylaseAddress(hLibrary, "rlListAppendLaserOffDelay");
	this->ptrListAppendJumpAbs2D = (PrlListAppendJumpAbs2D)_loadRaylaseAddress(hLibrary, "rlListAppendJumpAbs2D");
	this->ptrListAppendMarkAbs2D = (PrlListAppendMarkAbs2D)_loadRaylaseAddress(hLibrary, "rlListAppendMarkAbs2D");
	this->ptrListSet = (PrlListSet)_loadRaylaseAddress(hLibrary, "rlListSet");
	this->ptrListExecute = (PrlListExecute)_loadRaylaseAddress(hLibrary, "rlListExecute");
	this->ptrListWaitForListIdle = (PrlListWaitForListIdle)_loadRaylaseAddress(hLibrary, "rlListWaitForListIdle");
	this->ptrListWaitForListDone = (PrlListWaitForListDone)_loadRaylaseAddress(hLibrary, "rlListWaitForListDone");
	this->ptrListDelete = (PrlListDelete)_loadRaylaseAddress(hLibrary, "rlListDelete");
	this->ptrGetLastError = (PrlGetLastError)_loadRaylaseAddress(hLibrary, "rlGetLastError");
	this->ptrGetLastErrorLen = (PrlGetLastErrorLen)_loadRaylaseAddress(hLibrary, "rlGetLastErrorLen");

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
		if ((ptrGetLastErrorLen != nullptr) && (ptrGetLastError != nullptr)) {
			int32_t nLength = ptrGetLastErrorLen();
			if (nLength > 0) {
				std::vector<char> messageBuffer;
				messageBuffer.resize((size_t)nLength + 2);

				ptrGetLastError(messageBuffer.data(), nLength + 1);
				messageBuffer.at(nLength) = 0;

				sErrorMessage += std::string(messageBuffer.data());
			}
			else {
				sErrorMessage += "<unknown error>";
			}

		}
		else {
			sErrorMessage += "<unknown error>";
		}

		if (!sDebugMessage.empty ())
			sErrorMessage += " - " + sDebugMessage;
		sErrorMessage += ")";

		throw std::runtime_error(sErrorMessage);
	}
}

void CRaylaseSDK::logJournal(const std::string& sLine)
{
	// TODO
}

void CRaylaseSDK::resetFunctionPtrs()
{

	ptrConnect = nullptr;
	ptrIsConnected = nullptr;
	ptrDisconnect = nullptr;
	ptrSystemResetToDefaults = nullptr;
	ptrLaserLaserOn = nullptr;
	ptrLaserLaserOff = nullptr;
	ptrLaserIsPilotEnabled = nullptr;
	ptrLaserEnablePilot = nullptr;
	ptrLaserIsLaserArmed = nullptr;
	ptrLaserArmLaser = nullptr;
	ptrLaserReadLaserStatus = nullptr;
	ptrListAllocate = nullptr;
	ptrListReleaseHandle = nullptr;
	ptrListAppendLaserOn = nullptr;
	ptrListAppendLaserOff = nullptr;
	ptrListAppendPower = nullptr;
	ptrListAppendJumpSpeed = nullptr;
	ptrListAppendMarkSpeed = nullptr;
	ptrListAppendJumpDelay = nullptr;
	ptrListAppendMinJumpDelay = nullptr;
	ptrListAppendMarkDelay = nullptr;
	ptrListAppendPolyDelay = nullptr;
	ptrListAppendLaserOnDelay = nullptr;
	ptrListAppendLaserOffDelay = nullptr;
	ptrListAppendJumpAbs2D = nullptr;
	ptrListAppendMarkAbs2D = nullptr;
	ptrListSet = nullptr;
	ptrListExecute = nullptr;
	ptrListWaitForListIdle = nullptr;
	ptrListWaitForListDone = nullptr;
	ptrListDelete = nullptr;
	ptrGetLastError = nullptr;
	ptrGetLastErrorLen = nullptr;

}

rlHandle CRaylaseSDK::rlConnect(const char* pszAddress, int32_t nPort)
{
	if (pszAddress == nullptr)
		throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDPARAM);

	if (m_bEnableJournal) {
		logJournal("rlConnect (" + std::string (pszAddress) + ", " + std::to_string (nPort) + ");");
	}

	return ptrConnect(pszAddress, nPort);
	
}

rlResult CRaylaseSDK::rlIsConnected(rlHandle handle, bool* pbIsConnected)
{
	if (m_bEnableJournal) {
		logJournal("rlIsConnected (" + std::to_string(handle) + ", &bIsConnected);");
	}

	return ptrIsConnected(handle, pbIsConnected);
	

}

rlResult CRaylaseSDK::rlDisconnect(rlHandle handle)
{
	if (m_bEnableJournal) {
		logJournal("rlDisconnect (" + std::to_string(handle) + "); ");
	}

	return ptrDisconnect(handle);
	
}

rlResult CRaylaseSDK::rlSystemResetToDefaults(rlHandle handle)
{
	if (m_bEnableJournal) {
		logJournal("rlSystemResetToDefaults (" + std::to_string(handle) + "); ");
	}

	return ptrSystemResetToDefaults(handle);
	
}

rlResult CRaylaseSDK::rlLaserLaserOn(rlHandle handle)
{
	if (m_bEnableJournal) {
		logJournal("rlLaserLaserOn (" + std::to_string(handle) + "); ");
	}

	return ptrLaserLaserOn(handle);

}

rlResult CRaylaseSDK::rlLaserLaserOff(rlHandle handle)
{
	if (m_bEnableJournal) {
		logJournal("rlLaserLaserOff (" + std::to_string(handle) + "); ");
	}

	return ptrLaserLaserOff(handle);

}

rlResult CRaylaseSDK::rlLaserIsPilotEnabled(rlHandle handle, bool& enabled)
{
	if (m_bEnableJournal) {
		logJournal("rlLaserIsPilotEnabled (" + std::to_string(handle) + ", bEnabled); ");
	}

	return ptrLaserIsPilotEnabled(handle, enabled);
	
}

rlResult CRaylaseSDK::rlLaserEnablePilot(rlHandle handle, bool enable)
{
	if (m_bEnableJournal) {
		if (enable) 
			logJournal("rlLaserEnablePilot (" + std::to_string(handle) + ", true); ");
		else
			logJournal("rlLaserEnablePilot (" + std::to_string(handle) + ", false); ");
	}

	return ptrLaserEnablePilot(handle, enable);

}

rlResult CRaylaseSDK::rlLaserIsLaserArmed(rlHandle handle, bool& armed)
{
	if (m_bEnableJournal) {
		logJournal("rlLaserEnablePilot (" + std::to_string(handle) + ", bArmed); ");
	}


	return ptrLaserEnablePilot(handle, armed);

}

rlResult CRaylaseSDK::rlLaserArmLaser(rlHandle handle, bool arm)
{
	if (m_bEnableJournal) {
		if (arm)
			logJournal("rlLaserArmLaser (" + std::to_string(handle) + ", true); ");
		else
			logJournal("rlLaserArmLaser (" + std::to_string(handle) + ", false); ");
	}

	return ptrLaserArmLaser(handle, arm);

}

rlResult CRaylaseSDK::rlLaserReadLaserStatus(rlHandle handle, uint32_t& laserStatus)
{
	if (m_bEnableJournal) {
		logJournal("rlLaserReadLaserStatus (" + std::to_string(handle) + ", bLaserStatus); ");
	}

	return ptrLaserArmLaser(handle, laserStatus);
}


rlListHandle CRaylaseSDK::rlListAllocate(rlHandle handle)
{
	if (m_bEnableJournal) {
		logJournal("rlListAllocate (" + std::to_string(handle) + "); ");
	}

	return ptrListAllocate(handle);

}


rlResult CRaylaseSDK::rlListReleaseHandle(rlListHandle handle)
{
	if (m_bEnableJournal) {
		logJournal("rlListReleaseHandle (" + std::to_string(handle) + "); ");
	}

	return ptrListReleaseHandle(handle);
}

rlResult CRaylaseSDK::rlListAppendLaserOn(rlListHandle handle)
{
	if (m_bEnableJournal) {
		logJournal("rlListAppendLaserOn (" + std::to_string(handle) + "); ");
	}

	return ptrListAppendLaserOn(handle);
}

rlResult CRaylaseSDK::rlListAppendLaserOff(rlListHandle handle)
{
	if (m_bEnableJournal) {
		logJournal("rlListAppendLaserOn (" + std::to_string(handle) + "); ");
	}

	return ptrListAppendLaserOff(handle);
}

rlResult CRaylaseSDK::rlListAppendPower(rlListHandle handle, uint16_t nPower)
{
	if (m_bEnableJournal) {
		logJournal("rlListAppendPower (" + std::to_string(handle) + ", " + std::to_string (nPower) + "); ");
	}

	return ptrListAppendPower(handle, nPower);
}

rlResult CRaylaseSDK::rlListAppendJumpSpeed(rlListHandle handle, double jumpSpeed)
{
	if (m_bEnableJournal) {
		logJournal("rlListAppendJumpSpeed (" + std::to_string(handle) + ", " + std::to_string (jumpSpeed) + "); ");
	}

	return ptrListAppendJumpSpeed(handle, jumpSpeed);
}

rlResult CRaylaseSDK::rlListAppendMarkSpeed(rlListHandle handle, double markSpeed)
{
	if (m_bEnableJournal) {
		logJournal("rlListAppendMarkSpeed (" + std::to_string(handle) + ", " + std::to_string(markSpeed) + "); ");
	}

	return ptrListAppendMarkSpeed(handle, markSpeed);
}

rlResult CRaylaseSDK::rlListAppendJumpDelay(rlListHandle handle, double jumpDelay)
{
	if (m_bEnableJournal) {
		logJournal("rlListAppendJumpDelay (" + std::to_string(handle) + ", " + std::to_string(jumpDelay) + "); ");
	}

	return ptrListAppendJumpDelay(handle, jumpDelay);
}

rlResult CRaylaseSDK::rlListAppendMinJumpDelay(rlListHandle handle, double minJumpDelay)
{
	if (m_bEnableJournal) {
		logJournal("rlListAppendMinJumpDelay (" + std::to_string(handle) + ", " + std::to_string(minJumpDelay) + "); ");
	}

	return ptrListAppendMinJumpDelay(handle, minJumpDelay);
}

rlResult CRaylaseSDK::rlListAppendMarkDelay(rlListHandle handle, double markDelay)
{
	if (m_bEnableJournal) {
		logJournal("rlListAppendMarkDelay (" + std::to_string(handle) + ", " + std::to_string(markDelay) + "); ");
	}

	return ptrListAppendMarkDelay(handle, markDelay);
}

rlResult CRaylaseSDK::rlListAppendPolyDelay(rlListHandle handle, double polyDelay)
{
	if (m_bEnableJournal) {
		logJournal("rlListAppendPolyDelay (" + std::to_string(handle) + ", " + std::to_string(polyDelay) + "); ");
	}

	return ptrListAppendPolyDelay(handle, polyDelay);

}

rlResult CRaylaseSDK::rlListAppendLaserOnDelay(rlListHandle handle, double laserOnDelay)
{
	if (m_bEnableJournal) {
		logJournal("rlListAppendLaserOnDelay (" + std::to_string(handle) + ", " + std::to_string(laserOnDelay) + "); ");
	}

	return ptrListAppendLaserOnDelay(handle, laserOnDelay);

}

rlResult CRaylaseSDK::rlListAppendLaserOffDelay(rlListHandle handle, double laserOffDelay)
{
	if (m_bEnableJournal) {
		logJournal("rlListAppendLaserOffDelay (" + std::to_string(handle) + ", " + std::to_string(laserOffDelay) + "); ");
	}

	return ptrListAppendLaserOffDelay(handle, laserOffDelay);
}

rlResult CRaylaseSDK::rlListAppendJumpAbs2D(rlListHandle handle, double x, double y)
{
	if (m_bEnableJournal) {
		logJournal("rlListAppendJumpAbs2D (" + std::to_string(handle) + ", " + std::to_string(x) + ", " + std::to_string(y) + "); ");
	}

	return ptrListAppendJumpAbs2D(handle, x, y);
}

rlResult CRaylaseSDK::rlListAppendMarkAbs2D(rlListHandle handle, double x, double y)
{
	if (m_bEnableJournal) {
		logJournal("rlListAppendMarkAbs2D (" + std::to_string(handle) + ", " + std::to_string(x) + ", " + std::to_string(y) + "); ");
	}

	return ptrListAppendMarkAbs2D(handle, x, y);

}

rlResult CRaylaseSDK::rlListSet(rlHandle handle, int32_t listID, rlListHandle listHandle, bool append, int32_t executionLimit)
{
	if (m_bEnableJournal) {
		logJournal("rlListSet (" + std::to_string(handle) + ", " + std::to_string(listID) + ", " + std::to_string(listHandle) + ", " + std::to_string((int)append) + ", " + std::to_string(executionLimit) + "); ");
	}

	return ptrListSet(handle, listID, listHandle, append, executionLimit);

}

rlResult CRaylaseSDK::rlListExecute(rlHandle handle, int32_t listID)
{
	if (m_bEnableJournal) {
		logJournal("rlListExecute (" + std::to_string(handle) + ", " + std::to_string(listID) + "); ");
	}

	return ptrListExecute(handle, listID);
}

rlResult CRaylaseSDK::rlListWaitForListIdle(rlHandle handle, int32_t timeOutInMilliseconds, bool& successful, int32_t& listID)
{
	if (m_bEnableJournal) {
		logJournal("rlListWaitForListIdle (" + std::to_string(handle) + ", " + std::to_string(timeOutInMilliseconds) + ", bSuccessful, nListID); ");
	}

	return ptrListWaitForListIdle(handle, timeOutInMilliseconds, successful, listID);

}

rlResult CRaylaseSDK::rlListWaitForListDone(rlHandle handle, int32_t timeOutInMilliseconds, bool& successful, int32_t& listID)
{
	if (m_bEnableJournal) {
		logJournal("rlListWaitForListDone (" + std::to_string(handle) + ", " + std::to_string(timeOutInMilliseconds) + ", bSuccessful, nListID); ");
	}

	return ptrListWaitForListDone(handle, timeOutInMilliseconds, successful, listID);

}

rlResult CRaylaseSDK::rlListDelete(rlHandle handle, int32_t listID, bool bFromDisk)
{
	if (m_bEnableJournal) {
		logJournal("rlListDelete (" + std::to_string(handle) + ", " + std::to_string(listID) + ", " + std::to_string((int)bFromDisk) + "); ");
	}

	return ptrListDelete(handle, listID, bFromDisk);

}


rlResult CRaylaseSDK::rlEnableCommandLogging(rlHandle handle, const char* pszPath, int32_t truncateArguments)
{
	if (pszPath == nullptr)
		throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDPARAM);

	if (m_bEnableJournal) {
		logJournal("rlEnableCommandLogging (" + std::to_string(handle) + ", " + std::string(pszPath) + ", " + std::to_string (truncateArguments) + "); ");
	}

	return ptrEnableCommandLogging(handle, pszPath, truncateArguments);

}

rlResult CRaylaseSDK::rlDisableCommandLogging(rlHandle handle)
{
	if (m_bEnableJournal) {
		logJournal("rlDisableCommandLogging (" + std::to_string(handle) + "); ");
	}

	return ptrDisableCommandLogging(handle);

}

rlResult CRaylaseSDK::rlSetTimeout(rlHandle handle, uint32_t nTimeOut)
{
	if (m_bEnableJournal) {
		logJournal("rlSetTimeout (" + std::to_string(handle) + ", " + std::to_string(nTimeOut) + "); ");
	}


	return ptrSetTimeout(handle, nTimeOut);
}

rlResult CRaylaseSDK::rlGetTimeout(rlHandle handle, uint32_t* pnTimeOut)
{
	if (m_bEnableJournal) {
		logJournal("rlGetTimeout (" + std::to_string(handle) + ", &nTimeOut); ");
	}

	return ptrGetTimeout(handle, pnTimeOut);

}

rlResult CRaylaseSDK::rlConfigurationBeginUpdate(rlHandle handle)
{
	if (m_bEnableJournal) {
		logJournal("rlConfigurationBeginUpdate (" + std::to_string(handle) + "); ");
	}

	return ptrConfigurationBeginUpdate(handle);

}

rlResult CRaylaseSDK::rlConfigurationEndUpdate(rlHandle handle)
{
	if (m_bEnableJournal) {
		logJournal("rlConfigurationEndUpdate (" + std::to_string(handle) + "); ");
	}

	return ptrConfigurationEndUpdate(handle);
}

rlResult CRaylaseSDK::rlConfigurationAbortUpdate(rlHandle handle)
{
	if (m_bEnableJournal) {
		logJournal("rlConfigurationAbortUpdate (" + std::to_string(handle) + "); ");
	}

	return ptrConfigurationAbortUpdate(handle);
}

rlResult CRaylaseSDK::rlConfigurationIsUpdateActive(rlHandle handle, uint32_t* pbIsActive)
{
	if (m_bEnableJournal) {
		logJournal("rlConfigurationIsUpdateActive (" + std::to_string(handle) + ", &bIsActive); ");
	}

	return ptrConfigurationIsUpdateActive(handle, pbIsActive);
}

