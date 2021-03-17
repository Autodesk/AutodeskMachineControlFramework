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
    m_pLibMQTTDLL = m_pWorkingDirectory->StoreDriverData("libmqtt.dll", "libmqtt");
    m_pLibCryptoDLL = m_pWorkingDirectory->StoreDriverData("libcrypto-1_1-x64.dll", "libcrypto-1_1-x64");
    m_pOpenSSLDLL = m_pWorkingDirectory->StoreDriverData("libssl-1_1-x64.dll", "libssl-1_1-x64");

    m_pLibMQTTWrapper = LibMQTT::CWrapper::loadLibrary(m_pLibMQTTDLL->GetAbsoluteFileName ());
    m_pMQTTContext = m_pLibMQTTWrapper->CreateContext("");

    std::string sIotEndPoint = "";
    uint32_t nIotPort = 8883;
    std::string sClientName = "client";
    std::string sTopicName = "topic";
    
    auto pRootCertificate = m_pWorkingDirectory->StoreCustomString("x590.pem", "");
    auto pClientCertificate = m_pWorkingDirectory->StoreCustomString("client.pem.crt", "");
    auto pPrivateKey = m_pWorkingDirectory->StoreCustomString("private.pem.key", "");

    m_pMQTTContext->SetIotEndpoint(sIotEndPoint, nIotPort);
    m_pMQTTContext->SetRootCertificate(pRootCertificate->GetAbsoluteFileName ());
    m_pMQTTContext->SetClientInformation(sClientName, pClientCertificate->GetAbsoluteFileName(), pPrivateKey->GetAbsoluteFileName());
    m_pMQTTContext->SetMqttTopic(sTopicName);

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
        throw ELibMCDriver_MQTTInterfaceException(LIBMCDRIVER_MQTT_ERROR_NOTINITIZALIZED);
}


void CDriver_MQTT::Connect()
{
    if (m_pMQTTContext.get() == nullptr)
        throw ELibMCDriver_MQTTInterfaceException(LIBMCDRIVER_MQTT_ERROR_NOTINITIZALIZED);

    m_pMQTTContext->Connect();
}

void CDriver_MQTT::Disconnect()
{
    if (m_pMQTTContext.get() == nullptr)
        throw ELibMCDriver_MQTTInterfaceException(LIBMCDRIVER_MQTT_ERROR_NOTINITIZALIZED);

    m_pMQTTContext->Disconnect();
}

void CDriver_MQTT::SendMQTTMessage(const std::string & sMessageJSON)
{
    if (m_pMQTTContext.get() == nullptr)
        throw ELibMCDriver_MQTTInterfaceException(LIBMCDRIVER_MQTT_ERROR_NOTINITIZALIZED);

    m_pMQTTContext->SendMQTTMessage(sMessageJSON);
}

