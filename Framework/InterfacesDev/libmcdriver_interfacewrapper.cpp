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

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL AUTODESK INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


This file has been generated by the Automatic Component Toolkit (ACT) version 1.8.0-develop.

Abstract: This is an autogenerated C++ implementation file in order to allow easy
development of MC Driver Interface. The functions in this file need to be implemented. It needs to be generated only once.

Interface version: 2.0.0

*/

#include "libmcdriver_abi.hpp"
#include "libmcdriver_interfaces.hpp"
#include "libmcdriver_interfaceexception.hpp"

#include <map>

using namespace LibMCDriver::Impl;

LibMCDriverResult handleLibMCDriverException(IBase * pIBaseClass, ELibMCDriverInterfaceException & Exception)
{
	LibMCDriverResult errorCode = Exception.getErrorCode();

	if (pIBaseClass != nullptr)
		pIBaseClass->RegisterErrorMessage(Exception.what());

	return errorCode;
}

LibMCDriverResult handleStdException(IBase * pIBaseClass, std::exception & Exception)
{
	LibMCDriverResult errorCode = LIBMCDRIVER_ERROR_GENERICEXCEPTION;

	if (pIBaseClass != nullptr)
		pIBaseClass->RegisterErrorMessage(Exception.what());

	return errorCode;
}

LibMCDriverResult handleUnhandledException(IBase * pIBaseClass)
{
	LibMCDriverResult errorCode = LIBMCDRIVER_ERROR_GENERICEXCEPTION;

	if (pIBaseClass != nullptr)
		pIBaseClass->RegisterErrorMessage("Unhandled Exception");

	return errorCode;
}



/*************************************************************************************************************************
 Class implementation for Base
**************************************************************************************************************************/

/*************************************************************************************************************************
 Class implementation for Driver
**************************************************************************************************************************/
LibMCDriverResult libmcdriver_driver_configure(LibMCDriver_Driver pDriver, const char * pConfigurationString)
{
	IBase* pIBaseClass = (IBase *)pDriver;

	try {
		if (pConfigurationString == nullptr)
			throw ELibMCDriverInterfaceException (LIBMCDRIVER_ERROR_INVALIDPARAM);
		std::string sConfigurationString(pConfigurationString);
		IDriver* pIDriver = dynamic_cast<IDriver*>(pIBaseClass);
		if (!pIDriver)
			throw ELibMCDriverInterfaceException(LIBMCDRIVER_ERROR_INVALIDCAST);
		
		pIDriver->Configure(sConfigurationString);

		return LIBMCDRIVER_SUCCESS;
	}
	catch (ELibMCDriverInterfaceException & Exception) {
		return handleLibMCDriverException(pIBaseClass, Exception);
	}
	catch (std::exception & StdException) {
		return handleStdException(pIBaseClass, StdException);
	}
	catch (...) {
		return handleUnhandledException(pIBaseClass);
	}
}

LibMCDriverResult libmcdriver_driver_getname(LibMCDriver_Driver pDriver, const LibMCDriver_uint32 nNameBufferSize, LibMCDriver_uint32* pNameNeededChars, char * pNameBuffer)
{
	IBase* pIBaseClass = (IBase *)pDriver;

	try {
		if ( (!pNameBuffer) && !(pNameNeededChars) )
			throw ELibMCDriverInterfaceException (LIBMCDRIVER_ERROR_INVALIDPARAM);
		std::string sName("");
		IDriver* pIDriver = dynamic_cast<IDriver*>(pIBaseClass);
		if (!pIDriver)
			throw ELibMCDriverInterfaceException(LIBMCDRIVER_ERROR_INVALIDCAST);
		
		bool isCacheCall = (pNameBuffer == nullptr);
		if (isCacheCall) {
			sName = pIDriver->GetName();

			pIDriver->_setCache (new ParameterCache_1<std::string> (sName));
		}
		else {
			auto cache = dynamic_cast<ParameterCache_1<std::string>*> (pIDriver->_getCache ());
			if (cache == nullptr)
				throw ELibMCDriverInterfaceException(LIBMCDRIVER_ERROR_INVALIDCAST);
			cache->retrieveData (sName);
			pIDriver->_setCache (nullptr);
		}
		
		if (pNameNeededChars)
			*pNameNeededChars = (LibMCDriver_uint32) (sName.size()+1);
		if (pNameBuffer) {
			if (sName.size() >= nNameBufferSize)
				throw ELibMCDriverInterfaceException (LIBMCDRIVER_ERROR_BUFFERTOOSMALL);
			for (size_t iName = 0; iName < sName.size(); iName++)
				pNameBuffer[iName] = sName[iName];
			pNameBuffer[sName.size()] = 0;
		}
		return LIBMCDRIVER_SUCCESS;
	}
	catch (ELibMCDriverInterfaceException & Exception) {
		return handleLibMCDriverException(pIBaseClass, Exception);
	}
	catch (std::exception & StdException) {
		return handleStdException(pIBaseClass, StdException);
	}
	catch (...) {
		return handleUnhandledException(pIBaseClass);
	}
}

LibMCDriverResult libmcdriver_driver_gettype(LibMCDriver_Driver pDriver, const LibMCDriver_uint32 nTypeBufferSize, LibMCDriver_uint32* pTypeNeededChars, char * pTypeBuffer)
{
	IBase* pIBaseClass = (IBase *)pDriver;

	try {
		if ( (!pTypeBuffer) && !(pTypeNeededChars) )
			throw ELibMCDriverInterfaceException (LIBMCDRIVER_ERROR_INVALIDPARAM);
		std::string sType("");
		IDriver* pIDriver = dynamic_cast<IDriver*>(pIBaseClass);
		if (!pIDriver)
			throw ELibMCDriverInterfaceException(LIBMCDRIVER_ERROR_INVALIDCAST);
		
		bool isCacheCall = (pTypeBuffer == nullptr);
		if (isCacheCall) {
			sType = pIDriver->GetType();

			pIDriver->_setCache (new ParameterCache_1<std::string> (sType));
		}
		else {
			auto cache = dynamic_cast<ParameterCache_1<std::string>*> (pIDriver->_getCache ());
			if (cache == nullptr)
				throw ELibMCDriverInterfaceException(LIBMCDRIVER_ERROR_INVALIDCAST);
			cache->retrieveData (sType);
			pIDriver->_setCache (nullptr);
		}
		
		if (pTypeNeededChars)
			*pTypeNeededChars = (LibMCDriver_uint32) (sType.size()+1);
		if (pTypeBuffer) {
			if (sType.size() >= nTypeBufferSize)
				throw ELibMCDriverInterfaceException (LIBMCDRIVER_ERROR_BUFFERTOOSMALL);
			for (size_t iType = 0; iType < sType.size(); iType++)
				pTypeBuffer[iType] = sType[iType];
			pTypeBuffer[sType.size()] = 0;
		}
		return LIBMCDRIVER_SUCCESS;
	}
	catch (ELibMCDriverInterfaceException & Exception) {
		return handleLibMCDriverException(pIBaseClass, Exception);
	}
	catch (std::exception & StdException) {
		return handleStdException(pIBaseClass, StdException);
	}
	catch (...) {
		return handleUnhandledException(pIBaseClass);
	}
}

LibMCDriverResult libmcdriver_driver_getversion(LibMCDriver_Driver pDriver, LibMCDriver_uint32 * pMajor, LibMCDriver_uint32 * pMinor, LibMCDriver_uint32 * pMicro, const LibMCDriver_uint32 nBuildBufferSize, LibMCDriver_uint32* pBuildNeededChars, char * pBuildBuffer)
{
	IBase* pIBaseClass = (IBase *)pDriver;

	try {
		if (!pMajor)
			throw ELibMCDriverInterfaceException (LIBMCDRIVER_ERROR_INVALIDPARAM);
		if (!pMinor)
			throw ELibMCDriverInterfaceException (LIBMCDRIVER_ERROR_INVALIDPARAM);
		if (!pMicro)
			throw ELibMCDriverInterfaceException (LIBMCDRIVER_ERROR_INVALIDPARAM);
		if ( (!pBuildBuffer) && !(pBuildNeededChars) )
			throw ELibMCDriverInterfaceException (LIBMCDRIVER_ERROR_INVALIDPARAM);
		std::string sBuild("");
		IDriver* pIDriver = dynamic_cast<IDriver*>(pIBaseClass);
		if (!pIDriver)
			throw ELibMCDriverInterfaceException(LIBMCDRIVER_ERROR_INVALIDCAST);
		
		bool isCacheCall = (pBuildBuffer == nullptr);
		if (isCacheCall) {
			pIDriver->GetVersion(*pMajor, *pMinor, *pMicro, sBuild);

			pIDriver->_setCache (new ParameterCache_4<LibMCDriver_uint32, LibMCDriver_uint32, LibMCDriver_uint32, std::string> (*pMajor, *pMinor, *pMicro, sBuild));
		}
		else {
			auto cache = dynamic_cast<ParameterCache_4<LibMCDriver_uint32, LibMCDriver_uint32, LibMCDriver_uint32, std::string>*> (pIDriver->_getCache ());
			if (cache == nullptr)
				throw ELibMCDriverInterfaceException(LIBMCDRIVER_ERROR_INVALIDCAST);
			cache->retrieveData (*pMajor, *pMinor, *pMicro, sBuild);
			pIDriver->_setCache (nullptr);
		}
		
		if (pBuildNeededChars)
			*pBuildNeededChars = (LibMCDriver_uint32) (sBuild.size()+1);
		if (pBuildBuffer) {
			if (sBuild.size() >= nBuildBufferSize)
				throw ELibMCDriverInterfaceException (LIBMCDRIVER_ERROR_BUFFERTOOSMALL);
			for (size_t iBuild = 0; iBuild < sBuild.size(); iBuild++)
				pBuildBuffer[iBuild] = sBuild[iBuild];
			pBuildBuffer[sBuild.size()] = 0;
		}
		return LIBMCDRIVER_SUCCESS;
	}
	catch (ELibMCDriverInterfaceException & Exception) {
		return handleLibMCDriverException(pIBaseClass, Exception);
	}
	catch (std::exception & StdException) {
		return handleStdException(pIBaseClass, StdException);
	}
	catch (...) {
		return handleUnhandledException(pIBaseClass);
	}
}

LibMCDriverResult libmcdriver_driver_queryparametersex(LibMCDriver_Driver pDriver, LibMCEnv_DriverStatusUpdateSession pDriverUpdateInstance)
{
	IBase* pIBaseClass = (IBase *)pDriver;

	try {
		LibMCEnv::PDriverStatusUpdateSession pIDriverUpdateInstance = std::make_shared<LibMCEnv::CDriverStatusUpdateSession>(CWrapper::sPLibMCEnvWrapper.get(), pDriverUpdateInstance);
		CWrapper::sPLibMCEnvWrapper->AcquireInstance(pIDriverUpdateInstance.get());
		if (!pIDriverUpdateInstance)
			throw ELibMCDriverInterfaceException (LIBMCDRIVER_ERROR_INVALIDCAST);
		
		IDriver* pIDriver = dynamic_cast<IDriver*>(pIBaseClass);
		if (!pIDriver)
			throw ELibMCDriverInterfaceException(LIBMCDRIVER_ERROR_INVALIDCAST);
		
		pIDriver->QueryParametersEx(pIDriverUpdateInstance);

		return LIBMCDRIVER_SUCCESS;
	}
	catch (ELibMCDriverInterfaceException & Exception) {
		return handleLibMCDriverException(pIBaseClass, Exception);
	}
	catch (std::exception & StdException) {
		return handleStdException(pIBaseClass, StdException);
	}
	catch (...) {
		return handleUnhandledException(pIBaseClass);
	}
}



/*************************************************************************************************************************
 Function table lookup implementation
**************************************************************************************************************************/

LibMCDriverResult LibMCDriver::Impl::LibMCDriver_GetProcAddress (const char * pProcName, void ** ppProcAddress)
{
	if (pProcName == nullptr)
		return LIBMCDRIVER_ERROR_INVALIDPARAM;
	if (ppProcAddress == nullptr)
		return LIBMCDRIVER_ERROR_INVALIDPARAM;
	*ppProcAddress = nullptr;
	std::string sProcName (pProcName);
	
	if (sProcName == "libmcdriver_driver_configure") 
		*ppProcAddress = (void*) &libmcdriver_driver_configure;
	if (sProcName == "libmcdriver_driver_getname") 
		*ppProcAddress = (void*) &libmcdriver_driver_getname;
	if (sProcName == "libmcdriver_driver_gettype") 
		*ppProcAddress = (void*) &libmcdriver_driver_gettype;
	if (sProcName == "libmcdriver_driver_getversion") 
		*ppProcAddress = (void*) &libmcdriver_driver_getversion;
	if (sProcName == "libmcdriver_driver_queryparametersex") 
		*ppProcAddress = (void*) &libmcdriver_driver_queryparametersex;
	if (sProcName == "libmcdriver_getversion") 
		*ppProcAddress = (void*) &libmcdriver_getversion;
	if (sProcName == "libmcdriver_getlasterror") 
		*ppProcAddress = (void*) &libmcdriver_getlasterror;
	if (sProcName == "libmcdriver_releaseinstance") 
		*ppProcAddress = (void*) &libmcdriver_releaseinstance;
	if (sProcName == "libmcdriver_acquireinstance") 
		*ppProcAddress = (void*) &libmcdriver_acquireinstance;
	if (sProcName == "libmcdriver_injectcomponent") 
		*ppProcAddress = (void*) &libmcdriver_injectcomponent;
	if (sProcName == "libmcdriver_getsymbollookupmethod") 
		*ppProcAddress = (void*) &libmcdriver_getsymbollookupmethod;
	if (sProcName == "libmcdriver_createdriver") 
		*ppProcAddress = (void*) &libmcdriver_createdriver;
	
	if (*ppProcAddress == nullptr) 
		return LIBMCDRIVER_ERROR_COULDNOTFINDLIBRARYEXPORT;
	return LIBMCDRIVER_SUCCESS;
}

/*************************************************************************************************************************
 Global functions implementation
**************************************************************************************************************************/
LibMCDriverResult libmcdriver_getversion(LibMCDriver_uint32 * pMajor, LibMCDriver_uint32 * pMinor, LibMCDriver_uint32 * pMicro)
{
	IBase* pIBaseClass = nullptr;

	try {
		if (!pMajor)
			throw ELibMCDriverInterfaceException (LIBMCDRIVER_ERROR_INVALIDPARAM);
		if (!pMinor)
			throw ELibMCDriverInterfaceException (LIBMCDRIVER_ERROR_INVALIDPARAM);
		if (!pMicro)
			throw ELibMCDriverInterfaceException (LIBMCDRIVER_ERROR_INVALIDPARAM);
		CWrapper::GetVersion(*pMajor, *pMinor, *pMicro);

		return LIBMCDRIVER_SUCCESS;
	}
	catch (ELibMCDriverInterfaceException & Exception) {
		return handleLibMCDriverException(pIBaseClass, Exception);
	}
	catch (std::exception & StdException) {
		return handleStdException(pIBaseClass, StdException);
	}
	catch (...) {
		return handleUnhandledException(pIBaseClass);
	}
}

LibMCDriverResult libmcdriver_getlasterror(LibMCDriver_Base pInstance, const LibMCDriver_uint32 nErrorMessageBufferSize, LibMCDriver_uint32* pErrorMessageNeededChars, char * pErrorMessageBuffer, bool * pHasError)
{
	IBase* pIBaseClass = nullptr;

	try {
		if ( (!pErrorMessageBuffer) && !(pErrorMessageNeededChars) )
			throw ELibMCDriverInterfaceException (LIBMCDRIVER_ERROR_INVALIDPARAM);
		if (pHasError == nullptr)
			throw ELibMCDriverInterfaceException (LIBMCDRIVER_ERROR_INVALIDPARAM);
		IBase* pIBaseClassInstance = (IBase *)pInstance;
		IBase* pIInstance = dynamic_cast<IBase*>(pIBaseClassInstance);
		if (!pIInstance)
			throw ELibMCDriverInterfaceException (LIBMCDRIVER_ERROR_INVALIDCAST);
		
		std::string sErrorMessage("");
		*pHasError = CWrapper::GetLastError(pIInstance, sErrorMessage);

		if (pErrorMessageNeededChars)
			*pErrorMessageNeededChars = (LibMCDriver_uint32) (sErrorMessage.size()+1);
		if (pErrorMessageBuffer) {
			if (sErrorMessage.size() >= nErrorMessageBufferSize)
				throw ELibMCDriverInterfaceException (LIBMCDRIVER_ERROR_BUFFERTOOSMALL);
			for (size_t iErrorMessage = 0; iErrorMessage < sErrorMessage.size(); iErrorMessage++)
				pErrorMessageBuffer[iErrorMessage] = sErrorMessage[iErrorMessage];
			pErrorMessageBuffer[sErrorMessage.size()] = 0;
		}
		return LIBMCDRIVER_SUCCESS;
	}
	catch (ELibMCDriverInterfaceException & Exception) {
		return handleLibMCDriverException(pIBaseClass, Exception);
	}
	catch (std::exception & StdException) {
		return handleStdException(pIBaseClass, StdException);
	}
	catch (...) {
		return handleUnhandledException(pIBaseClass);
	}
}

LibMCDriverResult libmcdriver_releaseinstance(LibMCDriver_Base pInstance)
{
	IBase* pIBaseClass = nullptr;

	try {
		IBase* pIBaseClassInstance = (IBase *)pInstance;
		IBase* pIInstance = dynamic_cast<IBase*>(pIBaseClassInstance);
		if (!pIInstance)
			throw ELibMCDriverInterfaceException (LIBMCDRIVER_ERROR_INVALIDCAST);
		
		CWrapper::ReleaseInstance(pIInstance);

		return LIBMCDRIVER_SUCCESS;
	}
	catch (ELibMCDriverInterfaceException & Exception) {
		return handleLibMCDriverException(pIBaseClass, Exception);
	}
	catch (std::exception & StdException) {
		return handleStdException(pIBaseClass, StdException);
	}
	catch (...) {
		return handleUnhandledException(pIBaseClass);
	}
}

LibMCDriverResult libmcdriver_acquireinstance(LibMCDriver_Base pInstance)
{
	IBase* pIBaseClass = nullptr;

	try {
		IBase* pIBaseClassInstance = (IBase *)pInstance;
		IBase* pIInstance = dynamic_cast<IBase*>(pIBaseClassInstance);
		if (!pIInstance)
			throw ELibMCDriverInterfaceException (LIBMCDRIVER_ERROR_INVALIDCAST);
		
		CWrapper::AcquireInstance(pIInstance);

		return LIBMCDRIVER_SUCCESS;
	}
	catch (ELibMCDriverInterfaceException & Exception) {
		return handleLibMCDriverException(pIBaseClass, Exception);
	}
	catch (std::exception & StdException) {
		return handleStdException(pIBaseClass, StdException);
	}
	catch (...) {
		return handleUnhandledException(pIBaseClass);
	}
}

LibMCDriverResult libmcdriver_injectcomponent(const char * pNameSpace, LibMCDriver_pvoid pSymbolAddressMethod)
{
	IBase* pIBaseClass = nullptr;

	try {
		if (pNameSpace == nullptr)
			throw ELibMCDriverInterfaceException (LIBMCDRIVER_ERROR_INVALIDPARAM);
		std::string sNameSpace(pNameSpace);
		
		bool bNameSpaceFound = false;
		
		if (sNameSpace == "LibMCEnv") {
			if (CWrapper::sPLibMCEnvWrapper.get() != nullptr) {
				throw ELibMCDriverInterfaceException(LIBMCDRIVER_ERROR_COULDNOTLOADLIBRARY);
			}
			CWrapper::sPLibMCEnvWrapper = LibMCEnv::CWrapper::loadLibraryFromSymbolLookupMethod(pSymbolAddressMethod);
			bNameSpaceFound = true;
		}
		
		if (!bNameSpaceFound)
			throw ELibMCDriverInterfaceException(LIBMCDRIVER_ERROR_COULDNOTLOADLIBRARY);
		
		return LIBMCDRIVER_SUCCESS;
	}
	catch (ELibMCDriverInterfaceException & Exception) {
		return handleLibMCDriverException(pIBaseClass, Exception);
	}
	catch (std::exception & StdException) {
		return handleStdException(pIBaseClass, StdException);
	}
	catch (...) {
		return handleUnhandledException(pIBaseClass);
	}
}

LibMCDriverResult libmcdriver_getsymbollookupmethod(LibMCDriver_pvoid * pSymbolLookupMethod)
{
	IBase* pIBaseClass = nullptr;

	try {
		if (pSymbolLookupMethod == nullptr)
			throw ELibMCDriverInterfaceException (LIBMCDRIVER_ERROR_INVALIDPARAM);
		*pSymbolLookupMethod = (void*)&LibMCDriver::Impl::LibMCDriver_GetProcAddress;
		return LIBMCDRIVER_SUCCESS;
	}
	catch (ELibMCDriverInterfaceException & Exception) {
		return handleLibMCDriverException(pIBaseClass, Exception);
	}
	catch (std::exception & StdException) {
		return handleStdException(pIBaseClass, StdException);
	}
	catch (...) {
		return handleUnhandledException(pIBaseClass);
	}
}

LibMCDriverResult libmcdriver_createdriver(const char * pName, const char * pType, LibMCEnv_DriverEnvironment pDriverEnvironment, LibMCDriver_Driver * pInstance)
{
	IBase* pIBaseClass = nullptr;

	try {
		if (pName == nullptr)
			throw ELibMCDriverInterfaceException (LIBMCDRIVER_ERROR_INVALIDPARAM);
		if (pType == nullptr)
			throw ELibMCDriverInterfaceException (LIBMCDRIVER_ERROR_INVALIDPARAM);
		if (pInstance == nullptr)
			throw ELibMCDriverInterfaceException (LIBMCDRIVER_ERROR_INVALIDPARAM);
		std::string sName(pName);
		std::string sType(pType);
		LibMCEnv::PDriverEnvironment pIDriverEnvironment = std::make_shared<LibMCEnv::CDriverEnvironment>(CWrapper::sPLibMCEnvWrapper.get(), pDriverEnvironment);
		CWrapper::sPLibMCEnvWrapper->AcquireInstance(pIDriverEnvironment.get());
		if (!pIDriverEnvironment)
			throw ELibMCDriverInterfaceException (LIBMCDRIVER_ERROR_INVALIDCAST);
		
		IBase* pBaseInstance(nullptr);
		pBaseInstance = CWrapper::CreateDriver(sName, sType, pIDriverEnvironment);

		*pInstance = (IBase*)(pBaseInstance);
		return LIBMCDRIVER_SUCCESS;
	}
	catch (ELibMCDriverInterfaceException & Exception) {
		return handleLibMCDriverException(pIBaseClass, Exception);
	}
	catch (std::exception & StdException) {
		return handleStdException(pIBaseClass, StdException);
	}
	catch (...) {
		return handleUnhandledException(pIBaseClass);
	}
}


