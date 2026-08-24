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


#ifndef __LIBMCDRIVER_SPINNAKER_SPINNAKERDEVICEINSTANCE
#define __LIBMCDRIVER_SPINNAKER_SPINNAKERDEVICEINSTANCE

#include "libmcdriver_spinnaker_interfaces.hpp"
#include "libmcdriver_spinnaker_sdk.hpp"


namespace LibMCDriver_Spinnaker {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CSpinnakerDeviceInstance
**************************************************************************************************************************/

class CSpinnakerDeviceInstance {
private:
	std::string m_sIdentifier;
	PLibSpinnakerSDK m_pSpinnakerSDK;
	libSpinnakerCameraHandle m_pCameraHandle;
	bool m_bInitialized;

public:

	CSpinnakerDeviceInstance(const std::string& sIdentifier, PLibSpinnakerSDK pSpinnakerSDK, libSpinnakerCameraHandle pCameraHandle);

	virtual ~CSpinnakerDeviceInstance();

	void close();

	bool isOpen();

	std::string getIdentifier();

	bool featureIsAvailable(const std::string & sFeatureName);

	bool featureIsImplemented(const std::string & sFeatureName);

	bool featureIsReadable(const std::string & sFeatureName);

	bool featureIsWriteable(const std::string & sFeatureName);

	void setFloatFeature(const std::string & sFeatureName, const double dFloatValue);

	double getFloatFeature(const std::string & sFeatureName);

	void getFloatFeatureRange(const std::string& sFeatureName, double& dMinFloatValue, double& dMaxFloatValue);

	void setIntegerFeature(const std::string & sFeatureName, const int64_t nIntegerValue);

	LibMCDriver_Spinnaker_int64 getIntegerFeature(const std::string & sFeatureName);

	void getIntegerFeatureRange(const std::string & sFeatureName, int64_t& nMinIntegerValue, int64_t& nMaxIntegerValue);

	void setBoolFeature(const std::string & sFeatureName, const bool bBoolValue);

	bool getBoolFeature(const std::string & sFeatureName);

	void setAutoExposure(const bool bDoAutoExpose);

	bool getAutoExposure();

	void setExposureTime(const double dExposureTime);

	double getExposureTime();

	void grabSingleGreyscaleImage(const LibMCDriver_Spinnaker_uint32 nChannelID, const LibMCDriver_Spinnaker_uint32 nTimeoutInMS, LibMCEnv::PImageData pImageInstance);

};

typedef std::shared_ptr<CSpinnakerDeviceInstance> PSpinnakerDeviceInstance;

} // namespace Impl
} // namespace LibMCDriver_Spinnaker


#endif // __LIBMCDRIVER_SPINNAKER_SPINNAKERDEVICEINSTANCE
