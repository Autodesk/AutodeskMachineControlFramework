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


Abstract: This is a stub class definition of COIEDevice

*/

#include "libmcdriver_scanlaboie_oiedevice.hpp"
#include "libmcdriver_scanlaboie_interfaceexception.hpp"

// Include custom headers here.


using namespace LibMCDriver_ScanLabOIE::Impl;

/*************************************************************************************************************************
 Class definition of COIEDevice 
**************************************************************************************************************************/

void COIEDevice::SetHostName(const std::string & sHostName)
{
	throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_NOTIMPLEMENTED);
}

std::string COIEDevice::GetHostName()
{
	throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_NOTIMPLEMENTED);
}

void COIEDevice::SetPort(const LibMCDriver_ScanLabOIE_uint32 nPort)
{
	throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_NOTIMPLEMENTED);
}

LibMCDriver_ScanLabOIE_uint32 COIEDevice::GetPort()
{
	throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_NOTIMPLEMENTED);
}

bool COIEDevice::IsConnected()
{
	throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_NOTIMPLEMENTED);
}

LibMCDriver_ScanLabOIE_uint32 COIEDevice::GetDeviceID()
{
	throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_NOTIMPLEMENTED);
}

void COIEDevice::Connect(const std::string & sUserName, const std::string & sPassword)
{
	throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_NOTIMPLEMENTED);
}

void COIEDevice::Disconnect()
{
	throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_NOTIMPLEMENTED);
}

LibMCDriver_ScanLabOIE_uint32 COIEDevice::GetAppCount()
{
	throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_NOTIMPLEMENTED);
}

std::string COIEDevice::GetAppName(const LibMCDriver_ScanLabOIE_uint32 nIndex)
{
	throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_NOTIMPLEMENTED);
}

void COIEDevice::GetAppVersion(const LibMCDriver_ScanLabOIE_uint32 nIndex, LibMCDriver_ScanLabOIE_uint32 & nMajor, LibMCDriver_ScanLabOIE_uint32 & nMinor, LibMCDriver_ScanLabOIE_uint32 & nPatch)
{
	throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_NOTIMPLEMENTED);
}

void COIEDevice::GetAppInfo(const LibMCDriver_ScanLabOIE_uint32 nIndex, std::string & sName, LibMCDriver_ScanLabOIE_uint32 & nMajor, LibMCDriver_ScanLabOIE_uint32 & nMinor, LibMCDriver_ScanLabOIE_uint32 & nPatch)
{
	throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_NOTIMPLEMENTED);
}

void COIEDevice::StartAppByName(const std::string & sName, const std::string & sDeviceConfig)
{
	throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_NOTIMPLEMENTED);
}

void COIEDevice::StartAppByIndex(const LibMCDriver_ScanLabOIE_uint32 nIndex, const std::string & sDeviceConfig)
{
	throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_NOTIMPLEMENTED);
}

void COIEDevice::StartAppByMajorVersion(const std::string & sName, const LibMCDriver_ScanLabOIE_uint32 nMajorVersion, const std::string & sDeviceConfig)
{
	throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_NOTIMPLEMENTED);
}

void COIEDevice::StartAppByMinorVersion(const std::string & sName, const LibMCDriver_ScanLabOIE_uint32 nMajorVersion, const LibMCDriver_ScanLabOIE_uint32 nMinorVersion, const std::string & sDeviceConfig)
{
	throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_NOTIMPLEMENTED);
}

void COIEDevice::StopApp()
{
	throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_NOTIMPLEMENTED);
}

bool COIEDevice::AppIsRunning()
{
	throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_NOTIMPLEMENTED);
}

void COIEDevice::GetRunningApp(std::string & sName, LibMCDriver_ScanLabOIE_uint32 & nMajor, LibMCDriver_ScanLabOIE_uint32 & nMinor, LibMCDriver_ScanLabOIE_uint32 & nPatch)
{
	throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_NOTIMPLEMENTED);
}

void COIEDevice::InstallApp(const LibMCDriver_ScanLabOIE_uint64 nAppPackageBufferSize, const LibMCDriver_ScanLabOIE_uint8 * pAppPackageBuffer)
{
	throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_NOTIMPLEMENTED);
}

void COIEDevice::UninstallAppByName(const std::string & sName)
{
	throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_NOTIMPLEMENTED);
}

void COIEDevice::UninstallAppByIndex(const LibMCDriver_ScanLabOIE_uint32 nIndex)
{
	throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_NOTIMPLEMENTED);
}

void COIEDevice::UninstallAppByMajorVersion(const std::string & sName, const LibMCDriver_ScanLabOIE_uint32 nMajorVersion)
{
	throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_NOTIMPLEMENTED);
}

void COIEDevice::UninstallAppByMinorVersion(const std::string & sName, const LibMCDriver_ScanLabOIE_uint32 nMajorVersion, const LibMCDriver_ScanLabOIE_uint32 nMinorVersion)
{
	throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_NOTIMPLEMENTED);
}

