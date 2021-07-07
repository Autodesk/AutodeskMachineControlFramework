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


Abstract: This is the class declaration of CDriver_BuR

*/


#ifndef __LIBMCDRIVER_BUR_DRIVER_BUR
#define __LIBMCDRIVER_BUR_DRIVER_BUR

#include "libmcdriver_bur_interfaces.hpp"

// Parent classes
#include "libmcdriver_bur_driver.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include <mutex>

namespace brynet {
    namespace net {
        class TcpService;
        class TcpConnection;
        class AsyncConnector;
    }
}

namespace LibMCDriver_BuR {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CDriver_BuR 
**************************************************************************************************************************/

class CDriver_BuR : public virtual IDriver_BuR, public virtual CDriver {
private:

protected:
    LibMCEnv::PDriverEnvironment m_pDriverEnvironment;
    std::string m_sName;

    uint32_t m_nWorkerThreadCount;
    uint32_t m_nMaxReceiveBufferSize;

    std::shared_ptr<brynet::net::TcpService> m_pTcpService;
    std::shared_ptr <brynet::net::AsyncConnector> m_pAsyncConnector;

    std::mutex m_ConnectionMutex;

    void enterCallback(const std::shared_ptr <brynet::net::TcpConnection> session, const std::string & sIPAddress, const uint32_t nPort);
    void failedCallback();

    std::weak_ptr<brynet::net::TcpConnection> m_pCurrentConnection;

public:

    CDriver_BuR (const std::string & sName, LibMCEnv::PDriverEnvironment pDriverEnvironment);

    void Configure(const std::string& sConfigurationString) override;

    std::string GetName() override;

    std::string GetType() override;

    void GetVersion(LibMCDriver_BuR_uint32& nMajor, LibMCDriver_BuR_uint32& nMinor, LibMCDriver_BuR_uint32& nMicro, std::string& sBuild) override;

    void GetHeaderInformation(std::string& sNameSpace, std::string& sBaseName) override;

    void QueryParameters() override;

    void Connect(const std::string& sIPAddress, const LibMCDriver_BuR_uint32 nPort, const LibMCDriver_BuR_uint32 nTimeout) override;

    void Disconnect() override;

};

} // namespace Impl
} // namespace LibMCDriver_BuR

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_BUR_DRIVER_BUR
