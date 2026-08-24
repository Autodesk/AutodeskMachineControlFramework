/*++

Copyright (C) 2025 Autodesk Inc.

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


Abstract: This is the class declaration of CSpinnakerDevice

*/


#ifndef __LIBMCDRIVER_SPINNAKER_SPINNAKERDEVICE
#define __LIBMCDRIVER_SPINNAKER_SPINNAKERDEVICE

#include "libmcdriver_spinnaker_interfaces.hpp"

// Parent classes
#include "libmcdriver_spinnaker_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "libmcdriver_spinnaker_spinnakerdeviceinstance.hpp"

namespace LibMCDriver_Spinnaker {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CSpinnakerDevice 
**************************************************************************************************************************/

class CSpinnakerDevice : public virtual ISpinnakerDevice, public virtual CBase {
private:

	PSpinnakerDeviceInstance m_pInstance;

public:

	CSpinnakerDevice(PSpinnakerDeviceInstance pInstance);

	virtual ~CSpinnakerDevice();

	void Close() override;

	void DisconnectAndClose() override;

	bool IsOpen() override;

	std::string GetIdentifier() override;

	bool FeatureIsAvailable(const std::string & sFeatureName) override;

	bool FeatureIsImplemented(const std::string & sFeatureName) override;

	bool FeatureIsReadable(const std::string & sFeatureName) override;

	bool FeatureIsWriteable(const std::string & sFeatureName) override;

	void SetFloatFeature(const std::string & sFeatureName, const LibMCDriver_Spinnaker_double dFloatValue) override;

	LibMCDriver_Spinnaker_double GetFloatFeature(const std::string & sFeatureName) override;

	void GetFloatFeatureRange(const std::string & sFeatureName, LibMCDriver_Spinnaker_double & dMinFloatValue, LibMCDriver_Spinnaker_double & dMaxFloatValue) override;

	void SetIntegerFeature(const std::string & sFeatureName, const LibMCDriver_Spinnaker_int64 nIntegerValue) override;

	LibMCDriver_Spinnaker_int64 GetIntegerFeature(const std::string & sFeatureName) override;

	void GetIntegerFeatureRange(const std::string & sFeatureName, LibMCDriver_Spinnaker_int64 & nMinIntegerValue, LibMCDriver_Spinnaker_int64 & nMaxIntegerValue) override;

	void SetBoolFeature(const std::string & sFeatureName, const bool bBoolValue) override;

	bool GetBoolFeature(const std::string & sFeatureName) override;

	void SetAutoExposure(const bool bDoAutoExpose) override;

	bool GetAutoExposure() override;

	void SetExposureTime(const LibMCDriver_Spinnaker_double dExposureTime) override;

	LibMCDriver_Spinnaker_double GetExposureTime() override;

	void GrabSingleGreyscaleImage(const LibMCDriver_Spinnaker_uint32 nChannelID, const LibMCDriver_Spinnaker_uint32 nTimeoutInMS, LibMCEnv::PImageData pImageInstance) override;

};

} // namespace Impl
} // namespace LibMCDriver_Spinnaker

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_SPINNAKER_SPINNAKERDEVICE
