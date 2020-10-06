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

#include "libmcdriver_abi.hpp"
#include "libmcdriver_scanlab_abi.hpp"

LIBMCDRIVER_DECLSPEC LibMCDriverResult libmcdriver_driver_getname(LibMCDriver_Driver pDriver, const LibMCDriver_uint32 nNameBufferSize, LibMCDriver_uint32* pNameNeededChars, char * pNameBuffer)
{
	return libmcdriver_scanlab_driver_getname (pDriver, nNameBufferSize, pNameNeededChars, pNameBuffer);
}

LIBMCDRIVER_DECLSPEC LibMCDriverResult libmcdriver_driver_gettype(LibMCDriver_Driver pDriver, const LibMCDriver_uint32 nTypeBufferSize, LibMCDriver_uint32* pTypeNeededChars, char * pTypeBuffer)
{
	return libmcdriver_scanlab_driver_gettype(pDriver, nTypeBufferSize, pTypeNeededChars, pTypeBuffer);
}

LIBMCDRIVER_DECLSPEC LibMCDriverResult libmcdriver_driver_getversion(LibMCDriver_Driver pDriver, LibMCDriver_uint32 * pMajor, LibMCDriver_uint32 * pMinor, LibMCDriver_uint32 * pMicro, const LibMCDriver_uint32 nBuildBufferSize, LibMCDriver_uint32* pBuildNeededChars, char * pBuildBuffer)
{
	return libmcdriver_scanlab_driver_getversion(pDriver, pMajor, pMinor, pMicro, nBuildBufferSize, pBuildNeededChars, pBuildBuffer);
}

LIBMCDRIVER_DECLSPEC LibMCDriverResult libmcdriver_driver_getheaderinformation(LibMCDriver_Driver pDriver, const LibMCDriver_uint32 nNameSpaceBufferSize, LibMCDriver_uint32* pNameSpaceNeededChars, char * pNameSpaceBuffer, const LibMCDriver_uint32 nBaseNameBufferSize, LibMCDriver_uint32* pBaseNameNeededChars, char * pBaseNameBuffer)
{
	return libmcdriver_scanlab_driver_getheaderinformation(pDriver, nNameSpaceBufferSize, pNameSpaceNeededChars, pNameSpaceBuffer, nNameSpaceBufferSize, pBaseNameNeededChars, pBaseNameBuffer);
}

LIBMCDRIVER_DECLSPEC LibMCDriverResult libmcdriver_getversion(LibMCDriver_uint32 * pMajor, LibMCDriver_uint32 * pMinor, LibMCDriver_uint32 * pMicro)
{
	return libmcdriver_scanlab_getversion(pMajor, pMinor, pMicro);
}

LIBMCDRIVER_DECLSPEC LibMCDriverResult libmcdriver_getlasterror(LibMCDriver_Base pInstance, const LibMCDriver_uint32 nErrorMessageBufferSize, LibMCDriver_uint32* pErrorMessageNeededChars, char * pErrorMessageBuffer, bool * pHasError)
{
	return libmcdriver_scanlab_getlasterror(pInstance, nErrorMessageBufferSize, pErrorMessageNeededChars, pErrorMessageBuffer, pHasError);
}

LIBMCDRIVER_DECLSPEC LibMCDriverResult libmcdriver_releaseinstance(LibMCDriver_Base pInstance)
{
	return libmcdriver_scanlab_releaseinstance(pInstance);
}

LIBMCDRIVER_DECLSPEC LibMCDriverResult libmcdriver_acquireinstance(LibMCDriver_Base pInstance)
{
	return libmcdriver_scanlab_acquireinstance(pInstance);
}


LIBMCDRIVER_DECLSPEC LibMCDriverResult libmcdriver_injectcomponent(const char * pNameSpace, LibMCDriver_pvoid pSymbolAddressMethod)
{
	return libmcdriver_scanlab_injectcomponent(pNameSpace, pSymbolAddressMethod);
}

LIBMCDRIVER_DECLSPEC LibMCDriverResult libmcdriver_createdriver(const char * pName, const char * pType, LibMCEnv_DriverEnvironment pDriverEnvironment, LibMCDriver_Driver * pInstance)
{
	return libmcdriver_scanlab_createdriver(pName, pType, pDriverEnvironment, pInstance);
}

LIBMCDRIVER_DECLSPEC LibMCDriverResult libmcdriver_getsymbollookupmethod(LibMCDriver_pvoid* pSymbolLookupMethod)
{
	return libmcdriver_scanlab_getsymbollookupmethod(pSymbolLookupMethod);
}
