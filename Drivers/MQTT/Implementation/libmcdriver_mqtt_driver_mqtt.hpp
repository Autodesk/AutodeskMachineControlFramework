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


Abstract: This is the class declaration of CDriver_MQTT

*/


#ifndef __LIBMCDRIVER_MQTT_DRIVER_MQTT
#define __LIBMCDRIVER_MQTT_DRIVER_MQTT

#include "libmcdriver_mqtt_interfaces.hpp"

// Parent classes
#include "libmcdriver_mqtt_driver.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "libmqtt_dynamic.hpp"

namespace LibMCDriver_MQTT {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CDriver_MQTT 
**************************************************************************************************************************/

class CDriver_MQTT : public virtual IDriver_MQTT, public virtual CDriver {
private:

	LibMCEnv::PDriverEnvironment m_pDriverEnvironment;
	LibMCEnv::PWorkingDirectory m_pWorkingDirectory;

	LibMCEnv::PWorkingFile m_pLibMQTTDLL;
	LibMCEnv::PWorkingFile m_pOpenSSLDLL;
	LibMCEnv::PWorkingFile m_pLibCryptoDLL;

	LibMQTT::PWrapper m_pLibMQTTWrapper;
	LibMQTT::PMQTTContext m_pMQTTContext;

	std::string m_sName;
	std::string m_sType;

protected:


public:

	CDriver_MQTT(const std::string & sName, const std::string & sType, LibMCEnv::PDriverEnvironment pDriverEnvironment);
	~CDriver_MQTT();

	void Configure(const std::string& sConfigurationString) override;

	std::string GetName() override;

	std::string GetType() override;


	void QueryParameters() override;


	void Connect() override;

	void Disconnect() override;

	void SendMQTTMessage(const std::string & sMessageJSON) override;

};

} // namespace Impl
} // namespace LibMCDriver_MQTT

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_MQTT_DRIVER_MQTT
