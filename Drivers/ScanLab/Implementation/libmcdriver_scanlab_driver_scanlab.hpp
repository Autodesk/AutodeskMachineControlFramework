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


Abstract: This is the class declaration of CDriver_ScanLab

*/


#ifndef __LIBMCDRIVER_SCANLAB_DRIVER_SCANLAB
#define __LIBMCDRIVER_SCANLAB_DRIVER_SCANLAB

#include "libmcdriver_scanlab_interfaces.hpp"

// Parent classes
#include "libmcdriver_scanlab_driver.hpp"
#include "libmcdriver_scanlab_rtccontext.hpp"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "libmcdriver_scanlab_sdk.hpp"

#define RTC6_MIN_MAXLASERPOWER 10.0f
#define RTC6_MAX_MAXLASERPOWER 10000.0f

#define RTC6_MIN_LASER_DELAY -1000000.0f
#define RTC6_MAX_LASER_DELAY 1000000.0f

#define RTC6_MIN_MARK_DELAY 0.0
#define RTC6_MAX_MARK_DELAY 1000000.0f
#define RTC6_MIN_JUMP_DELAY 0.0
#define RTC6_MAX_JUMP_DELAY 1000000.0f
#define RTC6_MIN_POLYGON_DELAY 0.0
#define RTC6_MAX_POLYGON_DELAY 1000000.0f

#define SCANLAB_CONFIGURATIONSCHEMA "http://schemas.autodesk.com/amc/scanlab/2023/03"
#define SCANLAB_MAX_VERSIONVALUE 1024

namespace LibMCDriver_ScanLab {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CDriver_ScanLab 
**************************************************************************************************************************/

class CDriver_ScanLab : public virtual IDriver_ScanLab, public virtual CDriver, public virtual IRTCContextOwner {
private:

protected:

    PScanLabSDK m_pScanLabSDK;
    LibMCEnv::PDriverEnvironment m_pDriverEnvironment;
    LibMCEnv::PWorkingDirectory m_pWorkingDirectory;
    LibMCEnv::PWorkingFile m_pSDKLibraryFile;

    uint32_t m_nDLLVersion;

    virtual void updateDLLVersionParameter(uint32_t nDLLVersionParameter) = 0;

public:

    CDriver_ScanLab(LibMCEnv::PDriverEnvironment pDriverEnvironment);

	IRTCSelector * CreateRTCSelector() override;

    void LoadSDK(const std::string& sResourceName) override;

    void LoadCustomSDK(const LibMCDriver_ScanLab_uint64 nScanlabDLLBufferSize, const LibMCDriver_ScanLab_uint8* pScanlabDLLBuffer) override;

    uint32_t getDLLVersion();


};

} // namespace Impl
} // namespace LibMCDriver_ScanLab

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_SCANLAB_DRIVER_SCANLAB
