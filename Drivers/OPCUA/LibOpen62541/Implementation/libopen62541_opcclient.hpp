/*++

Copyright (C) 2024 Autodesk Inc.

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


Abstract: This is the class declaration of COPCClient

*/


#ifndef __LIBOPEN62541_OPCCLIENT
#define __LIBOPEN62541_OPCCLIENT

#include "libopen62541_interfaces.hpp"

// Parent classes
#include "libopen62541_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include <open62541/client_config_default.h>
#include <open62541/client_highlevel.h>
#include <open62541/plugin/log_stdout.h>
#include <open62541/plugin/securitypolicy.h>

namespace LibOpen62541 {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of COPCClient 
**************************************************************************************************************************/

class COPCClient : public virtual IOPCClient, public virtual CBase {
private:

	UA_Client* m_Client;

	std::string m_sCertificate;
	std::string m_sPrivateKey;
	LibOpen62541::eUASecurityMode m_SecurityMode;

public:

	COPCClient();

	virtual ~COPCClient();

	void EnableEncryption(const std::string& sLocalCertificate, const std::string& sPrivateKey, const LibOpen62541::eUASecurityMode eSecurityMode) override;

	void DisableEncryption() override;

	void ConnectUserName(const std::string & sEndPointURL, const std::string & sUsername, const std::string & sPassword, const std::string & sApplicationURL) override;

	void Disconnect() override;

	bool IsConnected() override;

	LibOpen62541_int64 ReadInteger(const LibOpen62541_uint32 nNameSpace, const std::string & sNodeName, const LibOpen62541::eUAIntegerType eNodeType) override;

	LibOpen62541_double ReadDouble(const LibOpen62541_uint32 nNameSpace, const std::string & sNodeName, const LibOpen62541::eUADoubleType eNodeType) override;

	std::string ReadString(const LibOpen62541_uint32 nNameSpace, const std::string & sNodeName) override;

	void WriteInteger(const LibOpen62541_uint32 nNameSpace, const std::string & sNodeName, const LibOpen62541::eUAIntegerType eNodeType, const LibOpen62541_int64 nValue) override;

	void WriteDouble(const LibOpen62541_uint32 nNameSpace, const std::string & sNodeName, const LibOpen62541::eUADoubleType eNodeType, const LibOpen62541_double dValue) override;

	void WriteString(const LibOpen62541_uint32 nNameSpace, const std::string & sNodeName, const std::string & sValue) override;

};

} // namespace Impl
} // namespace LibOpen62541

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBOPEN62541_OPCCLIENT
