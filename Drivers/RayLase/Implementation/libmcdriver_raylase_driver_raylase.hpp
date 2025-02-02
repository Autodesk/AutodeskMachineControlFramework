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


Abstract: This is the class declaration of CDriver_Raylase

*/


#ifndef __LIBMCDRIVER_RAYLASE_DRIVER_RAYLASE
#define __LIBMCDRIVER_RAYLASE_DRIVER_RAYLASE

#include "libmcdriver_raylase_interfaces.hpp"
#include "libmcdriver_raylase_sdk.hpp"
#include "libmcdriver_raylase_raylasecardimpl.hpp"
#include "libmcdriver_raylase_raylasecard.hpp"

// Parent classes
#include "libmcdriver_raylase_driver.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include <map>

namespace LibMCDriver_Raylase {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CDriver_Raylase 
**************************************************************************************************************************/

class CDriver_Raylase : public virtual IDriver_Raylase, public virtual CDriver {
private:

protected:
    std::string m_sName;
    PRaylaseSDK m_pRayLaseSDK;

    bool m_bSimulationMode;

    std::string m_sSDKResourceNameOverride;

    LibMCEnv::PDriverEnvironment m_pDriverEnvironment;

    LibMCEnv::PWorkingDirectory m_pSDKWorkingDirectory;
    LibMCEnv::PWorkingFile m_pSDKClientLib;

    std::map<std::string, PRaylaseCardImpl> m_CardInstances;


public:
    CDriver_Raylase(const std::string & sName, LibMCEnv::PDriverEnvironment pDriverEnvironment);
    
	virtual ~CDriver_Raylase();

    void Configure(const std::string& sConfigurationString) override;

    std::string GetName() override;

    std::string GetType() override;

    void GetVersion(LibMCDriver_Raylase_uint32& nMajor, LibMCDriver_Raylase_uint32& nMinor, LibMCDriver_Raylase_uint32& nMicro, std::string& sBuild) override;

    void QueryParameters() override;

    void QueryParametersEx(LibMCEnv::PDriverStatusUpdateSession pDriverUpdateInstance) override;

    void SetCustomSDKResource(const std::string& sResourceName) override;

	void LoadSDK() override;

    IRaylaseCard* ConnectByIP(const std::string& sCardName, const std::string& sCardIP, const LibMCDriver_Raylase_uint32 nPort, const LibMCDriver_Raylase_double dMaxLaserPowerInWatts) override;

    IRaylaseCard* GetConnectedCard(const std::string& sCardName) override;

    bool CardExists(const std::string& sCardName) override;

    void DisconnectCard(const std::string& sCardName) override;

    void SetToSimulationMode() override;

    bool IsSimulationMode() override;

    void DrawLayerMultiLaser(const std::string& sStreamUUID, const LibMCDriver_Raylase_uint32 nLayerIndex, const bool bFailIfNonAssignedDataExists, const LibMCDriver_Raylase_uint32 nScanningTimeoutInMS);


};

} // namespace Impl
} // namespace LibMCDriver_Raylase

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_RAYLASE_DRIVER_RAYLASE
