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


Abstract: This is the class declaration of CServer

*/


#ifndef __LIBOIE_SERVER
#define __LIBOIE_SERVER

#include "liboie_interfaces.hpp"
#include "oie_acceptrule.hpp"
#include "oie_connectionhandler.hpp"

#include <map>
#include <mutex>
#include <thread>

// Parent classes
#include "liboie_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
namespace brynet {
    namespace net {
        class TcpService;

        namespace wrapper {
            class ListenerBuilder;
        }

    }
}
#define LIBOIE_THREADCOUNT_MIN 4
#define LIBOIE_THREADCOUNT_MAX 1024
#define LIBOIE_THREADCOUNT_DEFAULT 32

#define LIBOIE_RECEIVEBUFFERSIZE_MIN 4096
#define LIBOIE_RECEIVEBUFFERSIZE_MAX (1024 * 1024 * 1024)
#define LIBOIE_RECEIVEBUFFERSIZE_DEFAULT (1024 * 1024)


namespace LibOIE {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CServer 
**************************************************************************************************************************/

class CServer : public virtual IServer, public virtual CBase {
private:
    uint32_t m_nThreadCount;
    uint32_t m_nReceiveBufferSize;

    std::shared_ptr<brynet::net::TcpService> m_pService;
    std::shared_ptr<brynet::net::wrapper::ListenerBuilder> m_pListener;

    uint32_t m_nAcceptRuleCounter;
    std::map<uint32_t, PAcceptRule> m_AcceptRules;

    uint64_t m_nConnectionCounter;
    std::mutex m_ConnectionMutex;
    std::map<uint64_t, PConnectionHandler> m_CurrentConnections;


protected:

public:

    CServer();

	void Start(const std::string & sIPAddress, const LibOIE_uint32 nPort, const bool bIPv6) override;

	void Stop() override;

	void CloseAllConnections() override;

	void SetThreadCount(const LibOIE_uint32 nThreadCount) override;

	void SetBufferSize(const LibOIE_uint32 nRCVBufferSize) override;

	LibOIE_uint32 AcceptDevice(const std::string & sDeviceName, const std::string & sApplicationName, const std::string & sVersionName) override;

	void UnAcceptDevice(const LibOIE_uint32 nRuleID) override;

	void ClearAcceptedDevices() override;

	IConnectionIterator * ListConnections() override;

    void SetConnectionAcceptedCallback(const LibOIE::ConnectionAcceptedCallback pCallback, const LibOIE_pvoid pUserData) override;
    
    void SetConnectionRejectedCallback(const LibOIE::ConnectionRejectedCallback pCallback, const LibOIE_pvoid pUserData) override;

    PConnectionHandler createConnectionHandler(const std::string& sIPAddress);
    void releaseConnectionHandler(const uint64_t nConnectionID);
    CConnectionHandler* findConnectionHandler(const uint64_t nConnectionID, bool bFailIfNotExisting);


};

} // namespace Impl
} // namespace LibOIE

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBOIE_SERVER
