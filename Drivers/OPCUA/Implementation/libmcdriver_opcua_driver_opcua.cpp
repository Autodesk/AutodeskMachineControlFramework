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


Abstract: This is a stub class definition of CDriver_OPCUA

*/

// Include custom headers here.
#include "libmcdriver_opcua_driver_opcua.hpp"
#include "libmcdriver_opcua_interfaceexception.hpp"
    
#define __STRINGIZE(x) #x
#define __STRINGIZE_VALUE_OF(x) __STRINGIZE(x)


using namespace LibMCDriver_OPCUA::Impl;

class COPCUA_DLLDirectoryCache {
private:
#ifdef _WIN32
    std::wstring m_sCachedDLLDirectoryW;
#endif // _WIN32

public:
    COPCUA_DLLDirectoryCache();
    virtual ~COPCUA_DLLDirectoryCache();

};

typedef std::shared_ptr<COPCUA_DLLDirectoryCache> POPCUA_DLLDirectoryCache;


COPCUA_DLLDirectoryCache::COPCUA_DLLDirectoryCache()
{
#ifdef _WIN32
    std::vector<wchar_t> buffer;
    buffer.resize(MAX_PATH + 1);
    GetDllDirectoryW(MAX_PATH, buffer.data());

    buffer.at(MAX_PATH) = 0;
    m_sCachedDLLDirectoryW = std::wstring(buffer.data());
#endif // _WIN32
}

COPCUA_DLLDirectoryCache::~COPCUA_DLLDirectoryCache()
{
#ifdef _WIN32
    if (!m_sCachedDLLDirectoryW.empty()) {
        SetDllDirectoryW(m_sCachedDLLDirectoryW.c_str());
    }
#endif // _WIN32
}


/*************************************************************************************************************************
 Class definition of CDriver_OPCUA 
**************************************************************************************************************************/

CDriver_OPCUA::CDriver_OPCUA(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment)
	: m_sName (sName), m_bSimulationMode (false), m_pDriverEnvironment (pDriverEnvironment), m_nMajorVersion (0), m_nMinorVersion (0), m_nPatchVersion (0)
{
	if (pDriverEnvironment.get() == nullptr)
		throw ELibMCDriver_OPCUAInterfaceException(LIBMCDRIVER_OPCUA_ERROR_INVALIDPARAM);
}


CDriver_OPCUA::~CDriver_OPCUA()
{
    m_pClient = nullptr;
    m_pLibOpen62541DLL = nullptr;
    m_pOpen62541DLL = nullptr;
    m_pLibSSLDLL = nullptr;
    m_pLibCryptoDLL = nullptr;
    m_pWorkingDirectory = nullptr;
}

void CDriver_OPCUA::Configure(const std::string& sConfigurationString)
{

    m_pWorkingDirectory = m_pDriverEnvironment->CreateWorkingDirectory();
    m_pOpen62541DLL = m_pWorkingDirectory->StoreDriverData("open62541.dll", "open62541-x64");
    m_pLibCryptoDLL = m_pWorkingDirectory->StoreDriverData("libcrypto-3-x64.dll", "libcrypto-3-x64");
    m_pLibSSLDLL = m_pWorkingDirectory->StoreDriverData("libssl-3-x64.dll", "libssl-3-x64");
    m_pLibOpen62541DLL = m_pWorkingDirectory->StoreDriverData("libopen62541.dll", "libopen62541-x64");

    auto pDLLDirectoryCache = std::make_shared<COPCUA_DLLDirectoryCache>();

#ifdef _WIN32
    std::string sDLLDirectoryUTF8 = m_pWorkingDirectory->GetAbsoluteFilePath();

    int nPathLength = (int)sDLLDirectoryUTF8.length();
    int nPathBufferSize = nPathLength * 2 + 2;
    std::vector<wchar_t> wsDLLPath(nPathBufferSize);
    int nPathResult = MultiByteToWideChar(CP_UTF8, 0, sDLLDirectoryUTF8.c_str(), nPathLength, &wsDLLPath[0], nPathBufferSize);
    if (nPathResult == 0)
        throw ELibMCDriver_OPCUAInterfaceException(LIBMCDRIVER_OPCUA_ERROR_COULDNOTLOADLIBRARY);
    SetDllDirectoryW(wsDLLPath.data ());
#endif



    m_pLibraryWrapper = LibOpen62541::CWrapper::loadLibrary(m_pLibOpen62541DLL->GetAbsoluteFileName ());
    m_pClient = m_pLibraryWrapper->CreateClient();
}

std::string CDriver_OPCUA::GetName()
{
	return m_sName;
}

std::string CDriver_OPCUA::GetType()
{
	return "opcua-1.0";
}

void CDriver_OPCUA::GetVersion(LibMCDriver_OPCUA_uint32& nMajor, LibMCDriver_OPCUA_uint32& nMinor, LibMCDriver_OPCUA_uint32& nMicro, std::string& sBuild)
{
	nMajor = LIBMCDRIVER_OPCUA_VERSION_MAJOR;
	nMinor = LIBMCDRIVER_OPCUA_VERSION_MINOR;
	nMicro = LIBMCDRIVER_OPCUA_VERSION_MICRO;
	sBuild = __STRINGIZE_VALUE_OF(__GITHASH);
}

void CDriver_OPCUA::QueryParameters()
{
    QueryParametersEx(m_pDriverEnvironment->CreateStatusUpdateSession());
}

void CDriver_OPCUA::QueryParametersEx(LibMCEnv::PDriverStatusUpdateSession pDriverUpdateInstance) 
{
    if (pDriverUpdateInstance.get() == nullptr)
        return;
}


void CDriver_OPCUA::SetToSimulationMode()
{
	m_bSimulationMode = true;
}

bool CDriver_OPCUA::IsSimulationMode()
{
	return m_bSimulationMode;
}

void CDriver_OPCUA::DisableEncryption() 
{
    m_pClient->DisableEncryption ();
}

void CDriver_OPCUA::EnableEncryption(const std::string& sLocalCertificate, const std::string& sPrivateKey, const LibMCDriver_OPCUA::eUASecurityMode eSecurityMode)
{
    LibOpen62541::eUASecurityMode eLibSecurityMode;

    switch (eSecurityMode) {
        case LibMCDriver_OPCUA::eUASecurityMode::Sign:
            eLibSecurityMode = LibOpen62541::eUASecurityMode::Sign;
            break;
        case LibMCDriver_OPCUA::eUASecurityMode::SignAndEncrypt:
            eLibSecurityMode = LibOpen62541::eUASecurityMode::SignAndEncrypt;
            break;
        default:
            eLibSecurityMode = LibOpen62541::eUASecurityMode::None;
    }

    m_pClient->EnableEncryption(sLocalCertificate, sPrivateKey, eLibSecurityMode);
}

bool CDriver_OPCUA::IsConnected()
{
    return m_pClient->IsConnected();
}

void CDriver_OPCUA::ConnectWithUserName(const std::string& sEndPointURL, const std::string& sUsername, const std::string& sPassword, const std::string& sApplicationURL)
{
    m_pClient->ConnectUserName (sEndPointURL, sUsername, sPassword, sApplicationURL);
}

void CDriver_OPCUA::Disconnect()
{
    m_pClient->Disconnect();
}

LibMCDriver_OPCUA_int64 CDriver_OPCUA::ReadInteger(const LibMCDriver_OPCUA_uint32 nNameSpace, const std::string& sNodeName, const LibMCDriver_OPCUA::eUAIntegerType eNodeType)
{
    LibOpen62541::eUAIntegerType eLibNodeType;
    switch (eNodeType) {
        case LibMCDriver_OPCUA::eUAIntegerType::UAUInt8:
            eLibNodeType = LibOpen62541::eUAIntegerType::UAUInt8;
            break;
        case LibMCDriver_OPCUA::eUAIntegerType::UAUInt16:
            eLibNodeType = LibOpen62541::eUAIntegerType::UAUInt16;
            break;
        case LibMCDriver_OPCUA::eUAIntegerType::UAUInt32:
            eLibNodeType = LibOpen62541::eUAIntegerType::UAUInt32;
            break;
        case LibMCDriver_OPCUA::eUAIntegerType::UAUInt64:
            eLibNodeType = LibOpen62541::eUAIntegerType::UAUInt64;
            break;
        case LibMCDriver_OPCUA::eUAIntegerType::UAInt8:
            eLibNodeType = LibOpen62541::eUAIntegerType::UAInt8;
            break;
        case LibMCDriver_OPCUA::eUAIntegerType::UAInt16:
            eLibNodeType = LibOpen62541::eUAIntegerType::UAInt16;
            break;
        case LibMCDriver_OPCUA::eUAIntegerType::UAInt32:
            eLibNodeType = LibOpen62541::eUAIntegerType::UAInt32;
            break;
        case LibMCDriver_OPCUA::eUAIntegerType::UAInt64:
            eLibNodeType = LibOpen62541::eUAIntegerType::UAInt64;
            break;
        default:
            eLibNodeType = LibOpen62541::eUAIntegerType::Unknown;
    }

    return m_pClient->ReadInteger (nNameSpace, sNodeName, eLibNodeType);
}

LibMCDriver_OPCUA_double CDriver_OPCUA::ReadDouble(const LibMCDriver_OPCUA_uint32 nNameSpace, const std::string& sNodeName, const LibMCDriver_OPCUA::eUADoubleType eNodeType)
{
    LibOpen62541::eUADoubleType eLibNodeType;
    switch (eNodeType) {
    case LibMCDriver_OPCUA::eUADoubleType::UAFloat32:
        eLibNodeType = LibOpen62541::eUADoubleType::UAFloat32;
        break;
    case LibMCDriver_OPCUA::eUADoubleType::UADouble64:
        eLibNodeType = LibOpen62541::eUADoubleType::UADouble64;
        break;
    default:
        eLibNodeType = LibOpen62541::eUADoubleType::Unknown;
    }

    return m_pClient->ReadDouble (nNameSpace, sNodeName, eLibNodeType);
}

std::string CDriver_OPCUA::ReadString(const LibMCDriver_OPCUA_uint32 nNameSpace, const std::string& sNodeName)
{
    return m_pClient->ReadString(nNameSpace, sNodeName);
}

void CDriver_OPCUA::WriteInteger(const LibMCDriver_OPCUA_uint32 nNameSpace, const std::string& sNodeName, const LibMCDriver_OPCUA::eUAIntegerType eNodeType, const LibMCDriver_OPCUA_int64 nValue)
{
    LibOpen62541::eUAIntegerType eLibNodeType;
    switch (eNodeType) {
    case LibMCDriver_OPCUA::eUAIntegerType::UAUInt8:
        eLibNodeType = LibOpen62541::eUAIntegerType::UAUInt8;
        break;
    case LibMCDriver_OPCUA::eUAIntegerType::UAUInt16:
        eLibNodeType = LibOpen62541::eUAIntegerType::UAUInt16;
        break;
    case LibMCDriver_OPCUA::eUAIntegerType::UAUInt32:
        eLibNodeType = LibOpen62541::eUAIntegerType::UAUInt32;
        break;
    case LibMCDriver_OPCUA::eUAIntegerType::UAUInt64:
        eLibNodeType = LibOpen62541::eUAIntegerType::UAUInt64;
        break;
    case LibMCDriver_OPCUA::eUAIntegerType::UAInt8:
        eLibNodeType = LibOpen62541::eUAIntegerType::UAInt8;
        break;
    case LibMCDriver_OPCUA::eUAIntegerType::UAInt16:
        eLibNodeType = LibOpen62541::eUAIntegerType::UAInt16;
        break;
    case LibMCDriver_OPCUA::eUAIntegerType::UAInt32:
        eLibNodeType = LibOpen62541::eUAIntegerType::UAInt32;
        break;
    case LibMCDriver_OPCUA::eUAIntegerType::UAInt64:
        eLibNodeType = LibOpen62541::eUAIntegerType::UAInt64;
        break;
    default:
        eLibNodeType = LibOpen62541::eUAIntegerType::Unknown;
    }

    m_pClient->WriteInteger(nNameSpace, sNodeName, eLibNodeType, nValue);

}

void CDriver_OPCUA::WriteDouble(const LibMCDriver_OPCUA_uint32 nNameSpace, const std::string& sNodeName, const LibMCDriver_OPCUA::eUADoubleType eNodeType, const LibMCDriver_OPCUA_double dValue)
{
    LibOpen62541::eUADoubleType eLibNodeType;
    switch (eNodeType) {
    case LibMCDriver_OPCUA::eUADoubleType::UAFloat32:
        eLibNodeType = LibOpen62541::eUADoubleType::UAFloat32;
        break;
    case LibMCDriver_OPCUA::eUADoubleType::UADouble64:
        eLibNodeType = LibOpen62541::eUADoubleType::UADouble64;
        break;
    default:
        eLibNodeType = LibOpen62541::eUADoubleType::Unknown;
    }

    m_pClient->WriteDouble(nNameSpace, sNodeName, eLibNodeType, dValue);
}


void CDriver_OPCUA::WriteString(const LibMCDriver_OPCUA_uint32 nNameSpace, const std::string& sNodeName, const std::string& sValue)
{
    m_pClient->WriteString(nNameSpace, sNodeName, sValue);
}


