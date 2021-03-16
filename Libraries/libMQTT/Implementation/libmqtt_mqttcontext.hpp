/*++

Copyright (C) 2021 Autodesk Inc.

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


Abstract: This is the class declaration of CMQTTContext

*/


#ifndef __LIBMQTT_MQTTCONTEXT
#define __LIBMQTT_MQTTCONTEXT

#include "libmqtt_interfaces.hpp"

// Parent classes
#include "libmqtt_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
/* MQTT API headers. */
extern "C"{
#include "core_mqtt.h"
}

extern "C" {
#include "core_mqtt_state.h"
}

/* OpenSSL sockets transport implementation. */
extern "C" {
#include "openssl_posix.h"
}

/*Include backoff algorithm header for retry logic.*/
extern "C" {
#include "backoff_algorithm.h"
}

/* Clock for timer. */
extern "C" {
#include "clock.h"
}

#include <windows.h>
#include "logging_stack.h"

namespace LibMQTT {
namespace Impl {

	/**
	 * @brief Maximum number of outgoing publishes maintained in the application
	 * until an ack is received from the broker.
	 */
#define MAX_OUTGOING_PUBLISHES              ( 5U )

      /**
       * @brief Size of the network buffer for MQTT packets.
       */
#define NETWORK_BUFFER_SIZE       ( 1024U )


/*************************************************************************************************************************
 Class declaration of CMQTTContext 
**************************************************************************************************************************/

class CMQTTContext : public virtual IMQTTContext, public virtual CBase {
private:
    std::string m_endpoint;
    uint32_t m_port;
    std::string m_clientIdentifier;
    std::string m_mqttTopic;
    std::string m_mqttMessage;
    std::string m_rootCertificate;
    std::string m_clientCertificate;
    std::string m_clientPrivateKey;
    bool m_pClientSessionPresent;
    bool m_mqttSessionEstablished;

    std::unique_ptr<MQTTContext_t> m_mqttContext;
    std::unique_ptr < NetworkContext_t> m_networkContext;
    std::unique_ptr<OpensslParams_t> m_opensslParams;

    int connectToServerWithBackoffRetries(NetworkContext_t* pNetworkContext);
    int establishMqttSession(MQTTContext_t* pMqttContext, bool createCleanSession, bool* pSessionPresent);
    int publishToTopic(MQTTContext_t* pMqttContext);



protected:



public:

	/**
	* Put additional public members here. They will not be visible in the external API.
	*/


	/**
	* Public member functions to implement.
	*/

    CMQTTContext();

	void SetIotEndpoint(const std::string & sEndPoint, const LibMQTT_uint32 nPort) override;

	void SetRootCertificate(const std::string & sCertificateString) override;

	void SetClientInformation(const std::string & sClientIdentifier, const std::string & sClientCertificateString, const std::string & sCertificatePrivateKeyString) override;

	void SetMqttTopic(const std::string & sMqttTopic) override;

	void SetMqttMessage(const std::string & sMqttMessage) override;

	void Connect() override;

	void Disconnect() override;

#undef SendMessage
	void SendMessage(const std::string & sMessageJSON) override;

};

} // namespace Impl
} // namespace LibMQTT

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMQTT_MQTTCONTEXT
