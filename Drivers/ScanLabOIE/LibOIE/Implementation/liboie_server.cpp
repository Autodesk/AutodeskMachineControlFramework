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


Abstract: This is a stub class definition of CServer

*/

#include "liboie_server.hpp"
#include "liboie_interfaceexception.hpp"

// Include custom headers here.


using namespace LibOIE::Impl;

/*************************************************************************************************************************
 Class definition of CServer 
**************************************************************************************************************************/

void CServer::Start(const std::string & sIPAddress, const LibOIE_uint32 nPort, const bool bIPv6)
{
	throw ELibOIEInterfaceException(LIBOIE_ERROR_NOTIMPLEMENTED);
}

void CServer::Stop()
{
	throw ELibOIEInterfaceException(LIBOIE_ERROR_NOTIMPLEMENTED);
}

void CServer::CloseAllConnections()
{
	throw ELibOIEInterfaceException(LIBOIE_ERROR_NOTIMPLEMENTED);
}

void CServer::SetThreadCount(const LibOIE_uint32 nThreadCount)
{
	throw ELibOIEInterfaceException(LIBOIE_ERROR_NOTIMPLEMENTED);
}

void CServer::SetBufferSize(const LibOIE_uint32 nRCVBufferSize)
{
	throw ELibOIEInterfaceException(LIBOIE_ERROR_NOTIMPLEMENTED);
}

LibOIE_uint32 CServer::AcceptDevice(const std::string & sDeviceName, const std::string & sApplicationName, const std::string & sVersionName)
{
	throw ELibOIEInterfaceException(LIBOIE_ERROR_NOTIMPLEMENTED);
}

void CServer::UnAcceptDevice(const LibOIE_uint32 nRuleID)
{
	throw ELibOIEInterfaceException(LIBOIE_ERROR_NOTIMPLEMENTED);
}

void CServer::ClearAcceptedDevices()
{
	throw ELibOIEInterfaceException(LIBOIE_ERROR_NOTIMPLEMENTED);
}

IConnectionIterator * CServer::ListConnections()
{
	throw ELibOIEInterfaceException(LIBOIE_ERROR_NOTIMPLEMENTED);
}

void CServer::SetConnectionAcceptedCallback(const LibOIE::ConnectionAcceptedCallback pCallback)
{
	throw ELibOIEInterfaceException(LIBOIE_ERROR_NOTIMPLEMENTED);
}

void CServer::SetConnectionRejectedCallback(const LibOIE::ConnectionRejectedCallback pCallback)
{
	throw ELibOIEInterfaceException(LIBOIE_ERROR_NOTIMPLEMENTED);
}

