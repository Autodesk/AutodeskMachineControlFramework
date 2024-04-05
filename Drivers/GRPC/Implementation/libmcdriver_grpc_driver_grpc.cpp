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


Abstract: This is a stub class definition of CDriver_GRPC

*/

#include "libmcdriver_grpc_driver_grpc.hpp"
#include "libmcdriver_grpc_interfaceexception.hpp"

// Include custom headers here.
#define __STRINGIZE(x) #x
#define __STRINGIZE_VALUE_OF(x) __STRINGIZE(x)

#include "libmcdriver_grpc_grpcconnection.hpp"

using namespace LibMCDriver_GRPC::Impl;

/*************************************************************************************************************************
 Class definition of CDriver_GRPC 
**************************************************************************************************************************/

CDriver_GRPC::CDriver_GRPC(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment)
	: m_sName(sName),
	m_pDriverEnvironment (pDriverEnvironment),
	m_bSimulationMode (false)
{
	if (pDriverEnvironment.get() == nullptr)
		throw ELibMCDriver_GRPCInterfaceException(LIBMCDRIVER_GRPC_ERROR_INVALIDPARAM);
}

CDriver_GRPC::~CDriver_GRPC()
{

}


void CDriver_GRPC::Configure(const std::string& sConfigurationString)
{
}

std::string CDriver_GRPC::GetName()
{
	return m_sName;
}

std::string CDriver_GRPC::GetType()
{
	return "libgrpc-1.0";
}

void CDriver_GRPC::GetVersion(LibMCDriver_GRPC_uint32& nMajor, LibMCDriver_GRPC_uint32& nMinor, LibMCDriver_GRPC_uint32& nMicro, std::string& sBuild)
{
	nMajor = LIBMCDRIVER_GRPC_VERSION_MAJOR;
	nMinor = LIBMCDRIVER_GRPC_VERSION_MINOR;
	nMicro = LIBMCDRIVER_GRPC_VERSION_MAJOR;
	sBuild = __STRINGIZE_VALUE_OF(__GITHASH);
}

void CDriver_GRPC::QueryParameters()
{

	QueryParametersEx(m_pDriverEnvironment->CreateStatusUpdateSession());

}

void CDriver_GRPC::QueryParametersEx(LibMCEnv::PDriverStatusUpdateSession pDriverUpdateInstance)
{

}



void CDriver_GRPC::SetToSimulationMode()
{
	m_bSimulationMode = true;
}

bool CDriver_GRPC::IsSimulationMode()
{
	return m_bSimulationMode;
}

void CDriver_GRPC::loadGRPCSDK()
{
	if (m_pGRPCWrapper.get() == nullptr) {

		m_pWorkingDirectory = m_pDriverEnvironment->CreateWorkingDirectory();

#ifdef _WIN32
		m_pGRPCDLL_AbseilDLL = m_pWorkingDirectory->StoreDriverData("abseil_dll.dll", "win64_abseil_dll");
		m_pGRPCDLL_Cares = m_pWorkingDirectory->StoreDriverData("cares.dll", "win64_cares");
		m_pGRPCDLL_LibCrypto3 = m_pWorkingDirectory->StoreDriverData("libcrypto-3-x64.dll", "win64_libcrypto-3-x64");
		m_pGRPCDLL_LibGRPCWrapper = m_pWorkingDirectory->StoreDriverData("libgrpcwrapper.dll", "win64_libgrpcwrapper");
		m_pGRPCDLL_LibProtobuf = m_pWorkingDirectory->StoreDriverData("libprotobuf.dll", "win64_libprotobuf");
		m_pGRPCDLL_LibProtobufLite = m_pWorkingDirectory->StoreDriverData("libprotobuf-lite.dll", "win64_libprotobuf-lite");
		m_pGRPCDLL_LibSSL = m_pWorkingDirectory->StoreDriverData("libssl-3-x64.dll", "win64_libssl-3-x64");
		m_pGRPCDLL_Re2 = m_pWorkingDirectory->StoreDriverData("re2.dll", "win64_re2");
		m_pGRPCDLL_ZLib = m_pWorkingDirectory->StoreDriverData("zlib1.dll", "win64_zlib1");

		std::string sDLLDirectoryUTF8 = m_pWorkingDirectory->GetAbsoluteFilePath();
		int nPathLength = (int)sDLLDirectoryUTF8.length();
		int nPathBufferSize = nPathLength * 2 + 2;
		std::vector<wchar_t> wsDLLPath(nPathBufferSize);
		int nPathResult = MultiByteToWideChar(CP_UTF8, 0, sDLLDirectoryUTF8.c_str(), nPathLength, &wsDLLPath[0], nPathBufferSize);
		if (nPathResult == 0)
			throw ELibMCDriver_GRPCInterfaceException(LIBMCDRIVER_GRPC_ERROR_COULDNOTLOADLIBRARY);

		m_sDLLDirectoryW = std::wstring(wsDLLPath.data());

		SetDllDirectoryW(m_sDLLDirectoryW.c_str ());

		m_pGRPCWrapper = LibGRPCWrapper::CWrapper::loadLibrary (m_pGRPCDLL_LibGRPCWrapper->GetAbsoluteFileName ());
#endif

	}

}

IGRPCConnection* CDriver_GRPC::ConnectUnsecure(const std::string& sIdentifier, const std::string& sNetworkCredentials, const std::string& sProtobufDefinition)
{
	loadGRPCSDK();

	if (sIdentifier.empty ())
		throw ELibMCDriver_GRPCInterfaceException(LIBMCDRIVER_GRPC_ERROR_EMPTYCONNECTIONIDENTIFIER);

	for (auto ch : sIdentifier) {
		if (!(isalnum (ch) || (ch == '_') || (ch == '-') || (ch == '.')))
			throw ELibMCDriver_GRPCInterfaceException(LIBMCDRIVER_GRPC_ERROR_INVALIDCONNECTIONIDENTIFIER, "invalid connection identifier: " + sIdentifier);
	}

	auto iIter = m_Connections.find(sIdentifier);
	if (iIter != m_Connections.end ())
		throw ELibMCDriver_GRPCInterfaceException(LIBMCDRIVER_GRPC_ERROR_DUPLICATECONNECTIONIDENTIFIER, "duplicate connection identifier: " + sIdentifier);

	if (m_pGRPCWrapper.get() == nullptr)
		throw ELibMCDriver_GRPCInterfaceException(LIBMCDRIVER_GRPC_ERROR_COULDNOTLOADGRPCWRAPPER);

	auto pProtocol = m_pGRPCWrapper->CreateProtocol(sProtobufDefinition);

	auto pConnection = pProtocol->ConnectUnsecure(sNetworkCredentials);

	pProtocol = nullptr;

	return new CGRPCConnection(m_pGRPCWrapper, pConnection);


/*	m_pDriverEnvironment->LogMessage ("Connection end point: " + pConnection->GetEndPoint());

	m_pDriverEnvironment->LogMessage("Creating request ");
	auto pRequest = pConnection->CreateStaticRequest("DoorOpenRequest", "Result");

	m_pDriverEnvironment->LogMessage("Setting fields");
	pRequest->SetBoolField("both", true);

	m_pDriverEnvironment->LogMessage("Sending Request");
	auto pResponse = pRequest->SendBlocking("/machine_interface.Machine/OpenDoors", 10000);
	//         = pResponse->GetBoolField("success");

	if (!pResponse->HasField("success")) {
		std::string sResponseField = pResponse->GetStringField("error");
		m_pDriverEnvironment->LogMessage("Response field: " + sResponseField);
	}

	m_pDriverEnvironment->LogMessage("done");
	return nullptr; */

}


IGRPCConnection* CDriver_GRPC::FindConnection(const std::string& sIdentifier, const bool bMustExist)
{
	auto iIter = m_Connections.find(sIdentifier);
	if (iIter != m_Connections.end())
		throw ELibMCDriver_GRPCInterfaceException(LIBMCDRIVER_GRPC_ERROR_COULDNOTFINDCONNECTIONIDENTIFIER, "duplicate connection identifier: " + sIdentifier);

	if (m_pGRPCWrapper.get() == nullptr)
		throw ELibMCDriver_GRPCInterfaceException(LIBMCDRIVER_GRPC_ERROR_GRPCWRAPPERNOTLOADED);
	
	auto pConnection = iIter->second;

	return new CGRPCConnection(m_pGRPCWrapper, pConnection);
}

