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


Abstract: This is a stub class definition of CConnection

*/

#include "liboie_connection.hpp"
#include "liboie_interfaceexception.hpp"

// Include custom headers here.


using namespace LibOIE::Impl;

/*************************************************************************************************************************
 Class definition of CConnection 
**************************************************************************************************************************/

void CConnection::Disconnect()
{
	throw ELibOIEInterfaceException(LIBOIE_ERROR_NOTIMPLEMENTED);
}

void CConnection::SetRequestTimeout(const LibOIE_uint32 nTimeout)
{
	throw ELibOIEInterfaceException(LIBOIE_ERROR_NOTIMPLEMENTED);
}

void CConnection::SetMaxErrorCount(const LibOIE_uint32 nErrorCount)
{
	throw ELibOIEInterfaceException(LIBOIE_ERROR_NOTIMPLEMENTED);
}

void CConnection::SetMaxMemoryCount(const LibOIE_uint32 nMaxMemoryCount)
{
	throw ELibOIEInterfaceException(LIBOIE_ERROR_NOTIMPLEMENTED);
}

void CConnection::SetBufferFile(const std::string & sFileName, const LibOIE_uint64 nMaxMemoryCount)
{
	throw ELibOIEInterfaceException(LIBOIE_ERROR_NOTIMPLEMENTED);
}

void CConnection::UnsetBufferFile()
{
	throw ELibOIEInterfaceException(LIBOIE_ERROR_NOTIMPLEMENTED);
}

void CConnection::SetCheckAliveInterval(const LibOIE_uint32 nCheckAliveInterval)
{
	throw ELibOIEInterfaceException(LIBOIE_ERROR_NOTIMPLEMENTED);
}

void CConnection::RetrieveFirmwareVersion(const std::string & sFirmwareName, std::string & sFirmwareVersion, std::string & sFilename, std::string & sFiledate, std::string & sSHA256)
{
	throw ELibOIEInterfaceException(LIBOIE_ERROR_NOTIMPLEMENTED);
}

void CConnection::UpdateFirmware(const std::string & sFirmwareName, const std::string & sFirmwareVersion, const std::string & sFileName, const std::string & sFileDate, const std::string & sSHA256, const LibOIE_uint64 nBinaryDataBufferSize, const LibOIE_uint8 * pBinaryDataBuffer)
{
	throw ELibOIEInterfaceException(LIBOIE_ERROR_NOTIMPLEMENTED);
}

bool CConnection::StartFirmware(const std::string & sFileName)
{
	throw ELibOIEInterfaceException(LIBOIE_ERROR_NOTIMPLEMENTED);
}

bool CConnection::StopFirmware(const std::string & sFileName)
{
	throw ELibOIEInterfaceException(LIBOIE_ERROR_NOTIMPLEMENTED);
}

void CConnection::SetConnectionErrorCallback(const LibOIE::ConnectionErrorCallback pCallback)
{
	throw ELibOIEInterfaceException(LIBOIE_ERROR_NOTIMPLEMENTED);
}

void CConnection::ClearErrors()
{
	throw ELibOIEInterfaceException(LIBOIE_ERROR_NOTIMPLEMENTED);
}

LibOIE_uint32 CConnection::GetErrorCount()
{
	throw ELibOIEInterfaceException(LIBOIE_ERROR_NOTIMPLEMENTED);
}

void CConnection::GetError(const LibOIE_uint32 nErrorIndex, std::string & sErrorMessage, std::string & sTimestamp)
{
	throw ELibOIEInterfaceException(LIBOIE_ERROR_NOTIMPLEMENTED);
}

IRemoteFunctionCall * CConnection::CreateRemoteFunctionCall(const std::string & sFunctionName)
{
	throw ELibOIEInterfaceException(LIBOIE_ERROR_NOTIMPLEMENTED);
}

IPyroDataPackageIterator * CConnection::GetBufferedPyroPackages()
{
	throw ELibOIEInterfaceException(LIBOIE_ERROR_NOTIMPLEMENTED);
}

IOctDataPackageIterator * CConnection::GetBufferedOctPackages()
{
	throw ELibOIEInterfaceException(LIBOIE_ERROR_NOTIMPLEMENTED);
}

