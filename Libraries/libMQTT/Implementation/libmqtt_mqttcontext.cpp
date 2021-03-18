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


Abstract: This is a stub class definition of CMQTTContext

*/

#include "libmqtt_mqttcontext.hpp"
#include "libmqtt_interfaceexception.hpp"
// Include custom headers here.
/* Standard includes. */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "transport_interface.h"
#include "core_mqtt_serializer.h"
/* Include Demo Config as the first non-system header. */

using namespace LibMQTT::Impl;

/* Each compilation unit must define the NetworkContext struct. */
struct NetworkContext
{
    OpensslParams_t* pParams;
};


typedef struct PublishPackets
{
    /**
     * @brief Packet identifier of the publish packet.
     */
    uint16_t packetId;

    /**
     * @brief Publish info of the publish packet.
     */
    MQTTPublishInfo_t pubInfo;
} PublishPackets_t;

/**
 * @brief The network buffer must remain valid for the lifetime of the MQTT context.
 */
static uint8_t buffer[NETWORK_BUFFER_SIZE];
/**
* @brief Array to keep the outgoing publish messages.
* These stored outgoing publish messages are kept until a successful ack
* is received.
*/
static PublishPackets_t outgoingPublishPacket = { 0 };


/*************************************************************************************************************************
 Class definition of CMQTTContext 
**************************************************************************************************************************/

static uint32_t generateRandomNumber()
{
    return(rand());
}

static int disconnectMqttSession(MQTTContext_t* pMqttContext)
{
    MQTTStatus_t mqttStatus = MQTTSuccess;
    int returnStatus = EXIT_SUCCESS;

    assert(pMqttContext != NULL);

    /* Send DISCONNECT. */
    mqttStatus = MQTT_Disconnect(pMqttContext);

    if (mqttStatus != MQTTSuccess)
    {
        //LogError(("Sending MQTT DISCONNECT failed with status=%s.",
           // MQTT_Status_strerror(mqttStatus)));
        returnStatus = EXIT_FAILURE;
    }

    return returnStatus;
}

int CMQTTContext::publishToTopic(MQTTContext_t* pMqttContext)
{
    int returnStatus = EXIT_SUCCESS;
    MQTTStatus_t mqttStatus = MQTTSuccess;
    MQTTPublishInfo_t publishInfo;

    assert(pMqttContext != NULL);

    if (returnStatus == EXIT_FAILURE)
    {   
        //Unable to find a free spot for outgoing PUBLISH message
        return returnStatus;
    }
    else
    {
        /* This example publishes to only one topic and uses QOS1. */
        publishInfo.qos = MQTTQoS0;
        publishInfo.pTopicName = m_mqttTopic.c_str();
        publishInfo.topicNameLength = (uint16_t)m_mqttTopic.length();
        publishInfo.pPayload = m_mqttMessage.c_str();
        publishInfo.payloadLength = (uint16_t)m_mqttMessage.length();

        /* Send PUBLISH packet. Packet Id is not used for a QoS0 publish.
            * Hence 0 is passed as packet id. */
        mqttStatus = MQTT_Publish(pMqttContext,
            &publishInfo,
            0U);

        if (mqttStatus != MQTTSuccess)
        {
            //Failed to send PUBLISH packet to broker with error = %s.",
            //MQTT_Status_strerror(mqttStatus)));
            returnStatus = EXIT_FAILURE;
        }
    }

    return returnStatus;
}

int CMQTTContext::establishMqttSession(MQTTContext_t* pMqttContext,
    bool createCleanSession,
    bool* pSessionPresent)
{
    int returnStatus = EXIT_SUCCESS;
    MQTTStatus_t mqttStatus;
    MQTTConnectInfo_t connectInfo = { 0 };

    //The MQTT metrics string expected by AWS IoT.
    const unsigned int MQTT_KEEP_ALIVE_INTERVAL_SECONDS = 60U;
    //Timeout for receiving CONNACK packet in milli seconds.
    const unsigned int CONNACK_RECV_TIMEOUT_MS = 1000U;

    assert(pMqttContext != NULL);
    assert(pSessionPresent != NULL);

    /* Establish MQTT session by sending a CONNECT packet. */

    /* If #createCleanSession is true, start with a clean session
     * i.e. direct the MQTT broker to discard any previous session data.
     * If #createCleanSession is false, directs the broker to attempt to
     * reestablish a session which was already present. */
    connectInfo.cleanSession = createCleanSession;


    /* The client identifier is used to uniquely identify this MQTT client to
 * the MQTT broker. In a production device the identifier can be something
 * unique, such as a device serial number. */
    connectInfo.pClientIdentifier = m_clientIdentifier.c_str();
    connectInfo.clientIdentifierLength = (uint16_t)(sizeof(m_clientIdentifier.c_str()) - 1);

    /* The maximum time interval in seconds which is allowed to elapse
     * between two Control Packets.
     * It is the responsibility of the Client to ensure that the interval between
     * Control Packets being sent does not exceed the this Keep Alive value. In the
     * absence of sending any other Control Packets, the Client MUST send a
     * PINGREQ Packet. */
    connectInfo.keepAliveSeconds = MQTT_KEEP_ALIVE_INTERVAL_SECONDS;

    /* Send MQTT CONNECT packet to broker. */
    mqttStatus = MQTT_Connect(pMqttContext, &connectInfo, NULL, CONNACK_RECV_TIMEOUT_MS, pSessionPresent);

    if (mqttStatus != MQTTSuccess)
    {
        returnStatus = EXIT_FAILURE;
        //LogError(("Connection with MQTT broker failed with status %s.", MQTT_Status_strerror(mqttStatus)));
    }
    //else
    //{
    //    //LogInfo(("MQTT connection successfully established with broker.\n\n"));
    //}

    return returnStatus;
}


int CMQTTContext::connectToServerWithBackoffRetries(NetworkContext_t* pNetworkContext)
{
    //Transport timeout in milliseconds for transport send and receive.
    const uint32_t TRANSPORT_SEND_RECV_TIMEOUT_MS = 500;
    //The maximum number of retries for connecting to server.
    const uint32_t CONNECTION_RETRY_MAX_ATTEMPTS = 5U;
    //The maximum back-off delay (in milliseconds) for retrying connection to server.
    const uint16_t CONNECTION_RETRY_MAX_BACKOFF_DELAY_MS = 5000U;
    //The base back-off delay (in milliseconds) to use for connection retry attempts.
    const uint16_t CONNECTION_RETRY_BACKOFF_BASE_MS = 500U;
    int returnStatus = EXIT_SUCCESS;
    BackoffAlgorithmStatus_t backoffAlgStatus = BackoffAlgorithmSuccess;
    OpensslStatus_t opensslStatus = OPENSSL_SUCCESS;
    BackoffAlgorithmContext_t reconnectParams;
    ServerInfo_t serverInfo;
    OpensslCredentials_t opensslCredentials;
    uint16_t nextRetryBackOff;

    /* Initialize information to connect to the MQTT broker. */
    serverInfo.pHostName = m_endpoint.c_str();
    serverInfo.hostNameLength = m_endpoint.length();
    serverInfo.port = m_port;

    /* Initialize credentials for establishing TLS session. */
    memset(&opensslCredentials, 0, sizeof(OpensslCredentials_t));
    opensslCredentials.pRootCaPath = m_rootCertificate.c_str();

    /* If #CLIENT_USERNAME is defined, username/password is used for authenticating
     * the client. */
    opensslCredentials.pClientCertPath = m_clientCertificate.c_str();
    opensslCredentials.pPrivateKeyPath = m_clientPrivateKey.c_str();

    /* AWS IoT requires devices to send the Server Name Indication (SNI)
     * extension to the Transport Layer Security (TLS) protocol and provide
     * the complete endpoint address in the host_name field. Details about
     * SNI for AWS IoT can be found in the link below.
     * https://docs.aws.amazon.com/iot/latest/developerguide/transport-security.html */
    opensslCredentials.sniHostName = m_endpoint.c_str();

    /* Initialize reconnect attempts and interval */
    BackoffAlgorithm_InitializeParams(&reconnectParams,
        CONNECTION_RETRY_BACKOFF_BASE_MS,
        CONNECTION_RETRY_MAX_BACKOFF_DELAY_MS,
        CONNECTION_RETRY_MAX_ATTEMPTS);

    /* Attempt to connect to MQTT broker. If connection fails, retry after
     * a timeout. Timeout value will exponentially increase until maximum
     * attempts are reached.
     */
    do
    {
        /* Establish a TLS session with the MQTT broker. This example connects
         * to the MQTT broker as specified in AWS_IOT_ENDPOINT and AWS_MQTT_PORT
         * at the demo config header. */
        opensslStatus = Openssl_Connect(pNetworkContext,
            &serverInfo,
            &opensslCredentials,
            TRANSPORT_SEND_RECV_TIMEOUT_MS,
            TRANSPORT_SEND_RECV_TIMEOUT_MS);

        if (opensslStatus != OPENSSL_SUCCESS)
        {
            /* Generate a random number and get back-off value (in milliseconds) for the next connection retry. */
            backoffAlgStatus = BackoffAlgorithm_GetNextBackoff(&reconnectParams, generateRandomNumber(), &nextRetryBackOff);

            if (backoffAlgStatus == BackoffAlgorithmRetriesExhausted)
            {
                //LogError(("Connection to the broker failed, all attempts exhausted."));
                returnStatus = EXIT_FAILURE;
            }
            else if (backoffAlgStatus == BackoffAlgorithmSuccess)
            {
                //LogWarn(("Connection to the broker failed. Retrying connection "
                   // "after %hu ms backoff.",
                   // (unsigned short)nextRetryBackOff));
                Clock_SleepMs(nextRetryBackOff);
            }
        }
    } while ((opensslStatus != OPENSSL_SUCCESS) && (backoffAlgStatus == BackoffAlgorithmSuccess));

    return returnStatus;
}

static void eventCallback(MQTTContext_t* pMqttContext,
    MQTTPacketInfo_t* pPacketInfo,
    MQTTDeserializedInfo_t* pDeserializedInfo)
{
    uint16_t packetIdentifier;

    assert(pMqttContext != NULL);
    assert(pPacketInfo != NULL);
    assert(pDeserializedInfo != NULL);

    /* Suppress unused parameter warning when asserts are disabled in build. */
    (void)pMqttContext;

    packetIdentifier = pDeserializedInfo->packetIdentifier;

    /* Handle other packets. */
    switch (pPacketInfo->type)
    {
    case MQTT_PACKET_TYPE_PINGRESP:

        /* Nothing to be done from application as library handles
            * PINGRESP. */
       //LogWarn(("PINGRESP should not be handled by the application "
        //    "callback when using MQTT_ProcessLoop.\n\n"));
        break;

        /* Any other packet type is invalid. */
    // // default:
        // LogError(("Unknown packet type received:(%02x).\n\n",
           // pPacketInfo->type));
    }

}


static int initializeMqtt(MQTTContext_t* pMqttContext,
    NetworkContext_t* pNetworkContext)
{
    int returnStatus = EXIT_SUCCESS;
    MQTTStatus_t mqttStatus;
    MQTTFixedBuffer_t networkBuffer;
    TransportInterface_t transport;

    assert(pMqttContext != NULL);
    assert(pNetworkContext != NULL);

    /* Fill in TransportInterface send and receive function pointers.
     * For this demo, TCP sockets are used to send and receive data
     * from network. Network context is SSL context for OpenSSL.*/
    transport.pNetworkContext = pNetworkContext;
    transport.send = Openssl_Send;
    transport.recv = Openssl_Recv;

    /* Fill the values for network buffer. */
    networkBuffer.pBuffer = buffer;
    networkBuffer.size = NETWORK_BUFFER_SIZE;

    /* Initialize MQTT library. */
    mqttStatus = MQTT_Init(pMqttContext,
        &transport,
        Clock_GetTimeMs,
        eventCallback,
        &networkBuffer);

    if (mqttStatus != MQTTSuccess)
    {
        returnStatus = EXIT_FAILURE;
    }

    return returnStatus;
}


CMQTTContext::CMQTTContext()
    : m_mqttContext(new MQTTContext_t),
    m_networkContext(new NetworkContext_t),
    m_opensslParams(new OpensslParams_t),
    m_pClientSessionPresent(false),
    m_mqttSessionEstablished(false)
{
    *m_networkContext = { 0 };
    *m_mqttContext = { 0 };
    *m_opensslParams = { 0 };
}


void CMQTTContext::SetIotEndpoint(const std::string & sEndPoint, const LibMQTT_uint32 nPort)
{
    m_endpoint = sEndPoint;
    m_port = nPort;
}

void CMQTTContext::SetRootCertificate(const std::string & sCertificateString)
{
    m_rootCertificate = sCertificateString;
}

void CMQTTContext::SetClientInformation(const std::string & sClientIdentifier, const std::string & sClientCertificateString, const std::string & sCertificatePrivateKeyString)
{
    m_clientIdentifier = sClientIdentifier;
    m_clientCertificate = sClientCertificateString;
    m_clientPrivateKey = sCertificatePrivateKeyString;
}

void CMQTTContext::SetMqttTopic(const std::string& sMqttTopic)
{
    m_mqttTopic = sMqttTopic; // "/example/topic"//CLIENT_IDENTIFIER "/example/topic"
}

void CMQTTContext::SetMqttMessage(const std::string& sMqttMessage)
{
    m_mqttMessage = sMqttMessage;
}

void CMQTTContext::Connect()
{
    int returnStatus = EXIT_SUCCESS;
    bool createCleanSession;
    bool brokerSessionPresent;
    m_mqttSessionEstablished = false;

    /* Set the pParams member of the network context with desired transport. */
    m_networkContext->pParams = m_opensslParams.get();

    /* Seed pseudo random number generator (provided by ISO C standard library) for
     * use by retry utils library when retrying failed network operations. */
     /* Get current time to seed pseudo random number generator. */
    uint32_t timeNs = Clock_GetTimeNs();
    /* Seed pseudo random number generator with nanoseconds. */
    srand(timeNs);

    /* Initialize MQTT library. Initialization of the MQTT library needs to be
     * done only once in this demo. */
    returnStatus = initializeMqtt(m_mqttContext.get(), m_networkContext.get());

    if (returnStatus != EXIT_SUCCESS)
        throw std::runtime_error("Could not initialize mqtt library");

    /* Attempt to connect to the MQTT broker. If connection fails, retry after
    * a timeout. Timeout value will be exponentially increased till the maximum
    * attempts are reached or maximum timeout value is reached. The function
    * returns EXIT_FAILURE if the TCP connection cannot be established to
    * broker after configured number of attempts. */
    returnStatus = connectToServerWithBackoffRetries(m_networkContext.get());

    if (returnStatus == EXIT_FAILURE)
        throw std::runtime_error("Failed to connect to MQTT broker");

    /* A clean MQTT session needs to be created, if there is no session saved
    * in this MQTT client. */
    createCleanSession = m_pClientSessionPresent ? false : true;

    /* Sends an MQTT Connect packet using the established TLS session,
    * then waits for connection acknowledgment (CONNACK) packet. */
    returnStatus = CMQTTContext::establishMqttSession(m_mqttContext.get(), createCleanSession, &brokerSessionPresent);

    if (returnStatus == EXIT_SUCCESS)
    {
        /* Keep a flag for indicating if MQTT session is established. This
            * flag will mark that an MQTT DISCONNECT has to be sent at the end
            * of the demo, even if there are intermediate failures. */
        m_mqttSessionEstablished = true;
    }

    if (returnStatus == EXIT_FAILURE)
    { 
        throw std::runtime_error("Failed to connect to MQTT broker");
    }
       
}

void CMQTTContext::Disconnect()
{
    if (m_mqttSessionEstablished == true)
    {
        /* Send an MQTT Disconnect packet over the already connected TCP socket.
        * There is no corresponding response for the disconnect packet. After sending
        * disconnect, client must close the network connection. */
        (void)disconnectMqttSession(m_mqttContext.get());

        /* End TLS session, then close TCP connection. */
        (void)Openssl_Disconnect(m_networkContext.get());
        *m_networkContext = { 0 };
        *m_mqttContext = { 0 };
        m_mqttSessionEstablished = false;
        m_pClientSessionPresent = false;
    }

}

void CMQTTContext::SendMQTTMessage(const std::string & sMessageJSON)
{
    if (m_mqttSessionEstablished == false)
    {
        return throw std::runtime_error("MQTT session was not established. Execute Connect first.");
    }

    const uint16_t DELAY_BETWEEN_PUBLISHES_SECONDS = 1U;
    MQTTStatus_t mqttStatus = MQTTSuccess;

    SetMqttMessage(sMessageJSON);
    int returnStatus = publishToTopic(m_mqttContext.get());

    /* For any error disconnect from the broker. */
    if (mqttStatus != MQTTSuccess)
    {
        if (m_mqttSessionEstablished == true)
        {
            (void)disconnectMqttSession(m_mqttContext.get());
            m_mqttSessionEstablished = false;
            m_pClientSessionPresent = false;
        }

        /* End TLS session, then close TCP connection. */
        (void)Openssl_Disconnect(m_networkContext.get());
        returnStatus = EXIT_FAILURE;
    }

    if (returnStatus != EXIT_SUCCESS)
        throw std::runtime_error("could not publish message");

    /* Leave connection idle for some time. */
    Sleep(DELAY_BETWEEN_PUBLISHES_SECONDS * 1000);

}

