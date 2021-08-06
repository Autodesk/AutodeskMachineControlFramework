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


Abstract: This is a stub class definition of CDriver_MQTT

*/

#include "libmcdriver_mqtt_driver_mqtt.hpp"
#include "libmcdriver_mqtt_interfaceexception.hpp"

// Include custom headers here.
#include "pugixml.hpp"

using namespace LibMCDriver_MQTT::Impl;

/*************************************************************************************************************************
 Class definition of CDriver_MQTT 
**************************************************************************************************************************/

/*************************************************************************************************************************
 Class definition of CDriver
**************************************************************************************************************************/

CDriver_MQTT::CDriver_MQTT(const std::string& sName, const std::string& sType, LibMCEnv::PDriverEnvironment pDriverEnvironment)
    : m_pDriverEnvironment (pDriverEnvironment), m_sName (sName), m_sType (sType)
{
    if (pDriverEnvironment.get() == nullptr)
        throw ELibMCDriver_MQTTInterfaceException(LIBMCDRIVER_MQTT_ERROR_INVALIDPARAM);

    m_pWorkingDirectory = m_pDriverEnvironment->CreateWorkingDirectory();
}

CDriver_MQTT::~CDriver_MQTT()
{
    m_pMQTTContext = nullptr;
    m_pLibMQTTWrapper = nullptr;
    m_pLibMQTTDLL = nullptr;
    m_pLibCryptoDLL = nullptr;
    m_pOpenSSLDLL = nullptr;
    m_pWorkingDirectory = nullptr;
}


void CDriver_MQTT::Configure(const std::string& sConfigurationString)
{
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(sConfigurationString.c_str());
    if (!result)
        throw ELibMCDriver_MQTTInterfaceException(LIBMCDRIVER_MQTT_ERROR_COULDNOTPARSEMQTTCONFIG);

    pugi::xml_node mqttconfigNode = doc.child("mqttconfig");
    if (mqttconfigNode.empty())
        throw ELibMCDriver_MQTTInterfaceException(LIBMCDRIVER_MQTT_ERROR_INVALIDMQTTCONFIG);

    auto iotEndpointNode = mqttconfigNode.child("iot_endpoint");
    if (iotEndpointNode.empty())
        throw ELibMCDriver_MQTTInterfaceException(LIBMCDRIVER_MQTT_ERROR_IOTENDPOINTMISSING);

    auto iotEndpointURLAttrib = iotEndpointNode.attribute("url");
    auto iotEndpointPortAttrib = iotEndpointNode.attribute("port");
    if (iotEndpointURLAttrib.empty ())
        throw ELibMCDriver_MQTTInterfaceException(LIBMCDRIVER_MQTT_ERROR_IOTENDPOINTURLMISSING);
    if (iotEndpointPortAttrib.empty())
        throw ELibMCDriver_MQTTInterfaceException(LIBMCDRIVER_MQTT_ERROR_IOTENDPOINTPORTMISSING);

    std::string sIotEndPoint = iotEndpointURLAttrib.as_string ();
    uint32_t nIotPort = iotEndpointPortAttrib.as_uint (0);
    if (sIotEndPoint.empty ())
        throw ELibMCDriver_MQTTInterfaceException(LIBMCDRIVER_MQTT_ERROR_INVALIDIOTENDPOINT);
    if (nIotPort == 0)
        throw ELibMCDriver_MQTTInterfaceException(LIBMCDRIVER_MQTT_ERROR_INVALIDIOTENDPOINT);

    auto iotEndpointCertificate = iotEndpointNode.child("certificate");
    if (iotEndpointCertificate.empty())
        throw ELibMCDriver_MQTTInterfaceException(LIBMCDRIVER_MQTT_ERROR_IOTENDPOINTCERTIFICATEMISSING);
    auto iotEndpointCertificateText = iotEndpointCertificate.text();
    std::string sRootCertificate = iotEndpointCertificateText.as_string();

    auto clientNode = mqttconfigNode.child("client");
    if (clientNode.empty())
        throw ELibMCDriver_MQTTInterfaceException(LIBMCDRIVER_MQTT_ERROR_CLIENTMISSING);

    auto clientNameAttrib = clientNode.attribute("name");
    auto clientTopicAttrib = clientNode.attribute("topic");
    if (clientNameAttrib.empty())
        throw ELibMCDriver_MQTTInterfaceException(LIBMCDRIVER_MQTT_ERROR_CLIENTNAMEMISSING);
    if (clientTopicAttrib.empty())
        throw ELibMCDriver_MQTTInterfaceException(LIBMCDRIVER_MQTT_ERROR_CLIENTTOPICMISSING);

    std::string sClientName = clientNameAttrib.as_string();
    std::string sClientTopic = clientTopicAttrib.as_string();
    if (sClientName.empty())
        throw ELibMCDriver_MQTTInterfaceException(LIBMCDRIVER_MQTT_ERROR_INVALIDCLIENTNAME);
    if (sClientTopic.empty())
        throw ELibMCDriver_MQTTInterfaceException(LIBMCDRIVER_MQTT_ERROR_INVALIDCLIENTTOPIC);

    auto clientCertificate = clientNode.child("certificate");
    if (clientCertificate.empty())
        throw ELibMCDriver_MQTTInterfaceException(LIBMCDRIVER_MQTT_ERROR_CLIENTCERTIFICATEMISSING);
    auto clientCertificateText = clientCertificate.text();
    std::string sClientCertificate = clientCertificateText.as_string ();

    auto clientPrivateKey = clientNode.child("privatekey");
    if (clientPrivateKey.empty())
        throw ELibMCDriver_MQTTInterfaceException(LIBMCDRIVER_MQTT_ERROR_CLIENTPRIVATEKEYMISSING);
    auto clientPrivateKeyText = clientPrivateKey.text();
    std::string sClientPrivateKey = clientPrivateKeyText.as_string ();

    m_pLibMQTTDLL = m_pWorkingDirectory->StoreDriverData("libmqtt.dll", "libmqtt");
    m_pLibCryptoDLL = m_pWorkingDirectory->StoreDriverData("libcrypto-1_1-x64.dll", "libcrypto-1_1-x64");
    m_pOpenSSLDLL = m_pWorkingDirectory->StoreDriverData("libssl-1_1-x64.dll", "libssl-1_1-x64");

    SetDllDirectory(m_pWorkingDirectory->GetAbsoluteFilePath ().c_str());

    m_pLibMQTTWrapper = LibMQTT::CWrapper::loadLibrary(m_pLibMQTTDLL->GetAbsoluteFileName ());
    m_pMQTTContext = m_pLibMQTTWrapper->CreateContext("");

    
    auto pRootCertificate = m_pWorkingDirectory->StoreCustomString("x590.pem", sRootCertificate);
    auto pClientCertificate = m_pWorkingDirectory->StoreCustomString("client.pem.crt", sClientCertificate);
    auto pPrivateKey = m_pWorkingDirectory->StoreCustomString("private.pem.key", sClientPrivateKey);

    m_pMQTTContext->SetIotEndpoint(sIotEndPoint, nIotPort);
    m_pMQTTContext->SetRootCertificate(pRootCertificate->GetAbsoluteFileName ());
    m_pMQTTContext->SetClientInformation(sClientName, pClientCertificate->GetAbsoluteFileName(), pPrivateKey->GetAbsoluteFileName());
    m_pMQTTContext->SetMqttTopic(sClientTopic);

}

std::string CDriver_MQTT::GetName()
{
    return m_sName;
}

std::string CDriver_MQTT::GetType()
{
    return m_sType;
}


void CDriver_MQTT::QueryParameters()
{
    if (m_pMQTTContext.get() == nullptr)
        throw ELibMCDriver_MQTTInterfaceException(LIBMCDRIVER_MQTT_ERROR_NOTINITIALIZED);
}


void CDriver_MQTT::Connect()
{
    if (m_pMQTTContext.get() == nullptr)
        throw ELibMCDriver_MQTTInterfaceException(LIBMCDRIVER_MQTT_ERROR_NOTINITIALIZED);

    m_pMQTTContext->Connect();
}

void CDriver_MQTT::Disconnect()
{
    if (m_pMQTTContext.get() == nullptr)
        throw ELibMCDriver_MQTTInterfaceException(LIBMCDRIVER_MQTT_ERROR_NOTINITIALIZED);

    m_pMQTTContext->Disconnect();
}

void CDriver_MQTT::SendMQTTMessage(const std::string & sMessageJSON)
{
    if (m_pMQTTContext.get() == nullptr)
        throw ELibMCDriver_MQTTInterfaceException(LIBMCDRIVER_MQTT_ERROR_NOTINITIALIZED);

    m_pMQTTContext->SendMQTTMessage(sMessageJSON);
}

