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


Abstract: This is a stub class definition of CSpinnakerDevice

*/

#include "libmcdriver_spinnaker_spinnakerdevice.hpp"
#include "libmcdriver_spinnaker_interfaceexception.hpp"

// Include custom headers here.


using namespace LibMCDriver_Spinnaker::Impl;

/*************************************************************************************************************************
 Class definition of CSpinnakerDevice 
**************************************************************************************************************************/

CSpinnakerDevice::CSpinnakerDevice(PSpinnakerDeviceInstance pInstance)
	: m_pInstance(pInstance)
{
	if (pInstance.get() == nullptr)
		throw ELibMCDriver_SpinnakerInterfaceException(LIBMCDRIVER_SPINNAKER_ERROR_INVALIDPARAM);
}

CSpinnakerDevice::~CSpinnakerDevice()
{

}


void CSpinnakerDevice::Close()
{
	m_pInstance->close();
}

void CSpinnakerDevice::DisconnectAndClose()
{
	m_pInstance->close();
}

bool CSpinnakerDevice::IsOpen()
{
	return m_pInstance->isOpen();
}

std::string CSpinnakerDevice::GetIdentifier()
{
	return m_pInstance->getIdentifier();
}

bool CSpinnakerDevice::FeatureIsAvailable(const std::string & sFeatureName)
{
	return m_pInstance->featureIsAvailable(sFeatureName);
}

bool CSpinnakerDevice::FeatureIsImplemented(const std::string & sFeatureName)
{
	return m_pInstance->featureIsImplemented(sFeatureName);
}

bool CSpinnakerDevice::FeatureIsReadable(const std::string & sFeatureName)
{
	return m_pInstance->featureIsReadable(sFeatureName);
}

bool CSpinnakerDevice::FeatureIsWriteable(const std::string & sFeatureName)
{
	return m_pInstance->featureIsWriteable(sFeatureName);
}

void CSpinnakerDevice::SetFloatFeature(const std::string & sFeatureName, const LibMCDriver_Spinnaker_double dFloatValue)
{
	m_pInstance->setFloatFeature(sFeatureName, dFloatValue);
}

LibMCDriver_Spinnaker_double CSpinnakerDevice::GetFloatFeature(const std::string & sFeatureName)
{
	return m_pInstance->getFloatFeature(sFeatureName);
}

void CSpinnakerDevice::GetFloatFeatureRange(const std::string & sFeatureName, LibMCDriver_Spinnaker_double & dMinFloatValue, LibMCDriver_Spinnaker_double & dMaxFloatValue)
{
	m_pInstance->getFloatFeatureRange(sFeatureName, dMinFloatValue, dMaxFloatValue);
}

void CSpinnakerDevice::SetIntegerFeature(const std::string & sFeatureName, const LibMCDriver_Spinnaker_int64 nIntegerValue)
{
	m_pInstance->setIntegerFeature(sFeatureName, nIntegerValue);
}

LibMCDriver_Spinnaker_int64 CSpinnakerDevice::GetIntegerFeature(const std::string & sFeatureName)
{
	return m_pInstance->getIntegerFeature(sFeatureName);
}

void CSpinnakerDevice::GetIntegerFeatureRange(const std::string & sFeatureName, LibMCDriver_Spinnaker_int64 & nMinIntegerValue, LibMCDriver_Spinnaker_int64 & nMaxIntegerValue)
{
	m_pInstance->getIntegerFeatureRange(sFeatureName, nMinIntegerValue, nMaxIntegerValue);
}

void CSpinnakerDevice::SetBoolFeature(const std::string & sFeatureName, const bool bBoolValue)
{
	m_pInstance->setBoolFeature(sFeatureName, bBoolValue);
}

bool CSpinnakerDevice::GetBoolFeature(const std::string & sFeatureName)
{
	return m_pInstance->getBoolFeature(sFeatureName);
}

void CSpinnakerDevice::SetAutoExposure(const bool bDoAutoExpose)
{
	m_pInstance->setAutoExposure(bDoAutoExpose);
}

bool CSpinnakerDevice::GetAutoExposure()
{
	return m_pInstance->getAutoExposure();
}

void CSpinnakerDevice::SetExposureTime(const LibMCDriver_Spinnaker_double dExposureTime)
{
	m_pInstance->setExposureTime(dExposureTime);
}

LibMCDriver_Spinnaker_double CSpinnakerDevice::GetExposureTime()
{
	return m_pInstance->getExposureTime();
}

void CSpinnakerDevice::GrabSingleGreyscaleImage(const LibMCDriver_Spinnaker_uint32 nChannelID, const LibMCDriver_Spinnaker_uint32 nTimeoutInMS, LibMCEnv::PImageData pImageInstance)
{
	m_pInstance->grabSingleGreyscaleImage(nChannelID, nTimeoutInMS, pImageInstance);
}

