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

#include "libmcdriver_scanlab_driver_scanlab_rtc5.hpp"
#include "libmcdriver_scanlab_interfaceexception.hpp"

// Include custom headers here.


using namespace LibMCDriver_ScanLab::Impl;

/*************************************************************************************************************************
 Class definition of CDriver_ScanLab_RTC5 
**************************************************************************************************************************/

CDriver_ScanLab_RTC5::CDriver_ScanLab_RTC5(const std::string& sName, const std::string& sType)
	: m_sName (sName), m_sType (sType)
{

}


IRTCSelector * CDriver_ScanLab_RTC5::CreateRTCSelector()
{
	throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_NOTIMPLEMENTED);
}

std::string CDriver_ScanLab_RTC5::GetName()
{
	return m_sName;
}

void CDriver_ScanLab_RTC5::GetVersion(LibMCDriver_ScanLab_uint32& nMajor, LibMCDriver_ScanLab_uint32& nMinor, LibMCDriver_ScanLab_uint32& nMicro, std::string& sBuild)
{
	nMajor = 1;
	nMinor = 0;
	nMicro = 0;
	sBuild = "";
}

std::string CDriver_ScanLab_RTC5::GetType()
{
	return m_sType;
}

void CDriver_ScanLab_RTC5::GetHeaderInformation(std::string& sNameSpace, std::string& sBaseName)
{
	sNameSpace = "LibMCDriver_ScanLab";
	sBaseName = "libmcdriver_scanlab";
}

