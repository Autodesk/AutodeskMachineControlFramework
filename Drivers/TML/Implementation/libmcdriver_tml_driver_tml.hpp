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


Abstract: This is the class declaration of CDriver_TML

*/


#ifndef __LIBMCDRIVER_TML_DRIVER_TML
#define __LIBMCDRIVER_TML_DRIVER_TML

#include "libmcdriver_tml_interfaces.hpp"

// Parent classes
#include "libmcdriver_tml_driver.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "libmcdriver_tml_sdk.hpp"
#include <vector>

namespace LibMCDriver_TML {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CDriver_TML 
**************************************************************************************************************************/

class CDriver_TML : public virtual IDriver_TML, public virtual CDriver {
private:
    
    std::string m_sName;
    LibMCEnv::PDriverEnvironment m_pDriverEnvironment;

    bool m_bSimulationMode;
     
    PTMLSDK m_pTMLSDK;

    std::vector<uint8_t> m_SDKDLLBuffer;

    LibMCEnv::PWorkingDirectory m_pWorkingDirectory;
    LibMCEnv::PWorkingFile m_pTMLDLLLibrary;

    bool sdkIsLoaded();

    void ensureSDKIsLoaded();

public:

    CDriver_TML(const std::string & sName, LibMCEnv::PDriverEnvironment pDriverEnvironment);

    virtual ~CDriver_TML();

	void SetToSimulationMode() override;

	bool IsSimulationMode() override;

	void SetCustomSDKResource(const std::string & sResourceName) override;

	void LoadSetup(const std::string & sSetupConfig, const std::string & sVariablesConfig) override;

	IChannel * OpenChannel(const std::string & sIdentifier, const std::string & sDeviceName, const LibMCDriver_TML::eChannelType eChannelTypeToUse, const LibMCDriver_TML::eProtocolType eProtocolTypeToUse, const LibMCDriver_TML_uint32 nHostID, const LibMCDriver_TML_uint32 nBaudrate) override;

	IChannel * FindChannel(const std::string & sIdentifier) override;

};

} // namespace Impl
} // namespace LibMCDriver_TML

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_TML_DRIVER_TML
