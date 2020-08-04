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

#include "libmcdriver_marlin_abi.hpp"
#include "libmcdriver_marlin_interfaces.hpp"
#include "libmcdriver_marlin_interfaceexception.hpp"

#include "libmcdriver_marlin_driver_marlin.hpp"

using namespace LibMCDriver_Marlin;
using namespace LibMCDriver_Marlin::Impl;

// Injected Components
LibMCDriverEnv::PWrapper CWrapper::sPLibMCDriverEnvWrapper;

void CWrapper::GetVersion(LibMCDriver_Marlin_uint32 & nMajor, LibMCDriver_Marlin_uint32 & nMinor, LibMCDriver_Marlin_uint32 & nMicro)
{
	nMajor = LIBMCDRIVER_MARLIN_VERSION_MAJOR;
	nMinor = LIBMCDRIVER_MARLIN_VERSION_MINOR;
	nMicro = LIBMCDRIVER_MARLIN_VERSION_MICRO;
}

bool CWrapper::GetLastError(IBase* pInstance, std::string & sErrorMessage)
{
	if (pInstance) {
		return pInstance->GetLastErrorMessage (sErrorMessage);
	} else {
		return false;
	}
}

void CWrapper::ReleaseInstance(IBase* pInstance)
{
	IBase::ReleaseBaseClassInterface(pInstance);
}

void CWrapper::AcquireInstance(IBase* pInstance)
{
	IBase::AcquireBaseClassInterface(pInstance);
}



IDriver * CWrapper::CreateDriver(const std::string& sName, const std::string& sType, LibMCDriverEnv::PDriverEnvironment pDriverEnvironment)
{
	if (sType == "marlin-2.0") 
		return new CDriver_Marlin(sName, sType);

	throw ELibMCDriver_MarlinInterfaceException(LIBMCDRIVER_MARLIN_ERROR_DRIVERERROR);
	
}


