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


Abstract: This is a stub class definition of CSpinnakerDeviceInstance

*/

#include "libmcdriver_spinnaker_spinnakerdeviceinstance.hpp"
#include "libmcdriver_spinnaker_interfaceexception.hpp"

#include <cstring>
#include <vector>

// Include custom headers here.
#define SPINNAKER_FEATURENAME_AUTOEXPOSURE "ExposureAuto"
#define SPINNAKER_FEATURENAME_EXPOSURETIME "ExposureTime"

using namespace LibMCDriver_Spinnaker::Impl;

static libSpinnakerNodeHandle getFeatureNode(PLibSpinnakerSDK pSpinnakerSDK, libSpinnakerCameraHandle pCameraHandle, const std::string& sFeatureName)
{
	if (pSpinnakerSDK.get() == nullptr)
		throw ELibMCDriver_SpinnakerInterfaceException(LIBMCDRIVER_SPINNAKER_ERROR_INVALIDPARAM);

	libSpinnakerNodeMapHandle hNodeMap = nullptr;
	pSpinnakerSDK->checkError(pSpinnakerSDK->SpinCameraGetNodeMap(pCameraHandle, &hNodeMap));
	if (hNodeMap == nullptr)
		throw ELibMCDriver_SpinnakerInterfaceException(LIBMCDRIVER_SPINNAKER_ERROR_DRIVERERROR, "node map unavailable");

	libSpinnakerNodeHandle hNode = nullptr;
	pSpinnakerSDK->checkError(pSpinnakerSDK->SpinNodeMapGetNode(hNodeMap, sFeatureName.c_str(), &hNode));
	return hNode;
}

static int64_t getEnumEntryValue(PLibSpinnakerSDK pSpinnakerSDK, libSpinnakerNodeHandle hEnumNode, const std::string& sEntryName)
{
	libSpinnakerEnumEntryHandle hEntry = nullptr;
	pSpinnakerSDK->checkError(pSpinnakerSDK->SpinEnumerationGetEntryByName(hEnumNode, sEntryName.c_str(), &hEntry));
	if (hEntry == nullptr)
		throw ELibMCDriver_SpinnakerInterfaceException(LIBMCDRIVER_SPINNAKER_ERROR_DRIVERERROR, "enum entry not found: " + sEntryName);

	int64_t nValue = 0;
	pSpinnakerSDK->checkError(pSpinnakerSDK->SpinEnumerationEntryGetIntValue(hEntry, &nValue));
	return nValue;
}

static std::string getEnumEntrySymbolic(PLibSpinnakerSDK pSpinnakerSDK, libSpinnakerEnumEntryHandle hEntry)
{
	if (pSpinnakerSDK->SpinEnumerationEntryGetSymbolic == nullptr)
		return std::string();

	size_t nLength = 0;
	pSpinnakerSDK->checkError(pSpinnakerSDK->SpinEnumerationEntryGetSymbolic(hEntry, nullptr, &nLength));
	if (nLength == 0)
		return std::string();

	std::vector<char> buffer;
	buffer.resize(nLength + 1);
	pSpinnakerSDK->checkError(pSpinnakerSDK->SpinEnumerationEntryGetSymbolic(hEntry, buffer.data(), &nLength));
	buffer.at(nLength) = 0;
	return std::string(buffer.data());
}

/*************************************************************************************************************************
 Class definition of CSpinnakerDeviceInstance
**************************************************************************************************************************/

CSpinnakerDeviceInstance::CSpinnakerDeviceInstance(const std::string& sIdentifier, PLibSpinnakerSDK pSpinnakerSDK, libSpinnakerCameraHandle pCameraHandle)
	: m_sIdentifier(sIdentifier), m_pSpinnakerSDK(pSpinnakerSDK), m_pCameraHandle(pCameraHandle), m_bInitialized(false)
{
	if (pSpinnakerSDK.get() == nullptr)
		throw ELibMCDriver_SpinnakerInterfaceException(LIBMCDRIVER_SPINNAKER_ERROR_INVALIDPARAM);
	if (pCameraHandle == nullptr)
		throw ELibMCDriver_SpinnakerInterfaceException(LIBMCDRIVER_SPINNAKER_ERROR_INVALIDPARAM);

	m_pSpinnakerSDK->checkError(m_pSpinnakerSDK->SpinCameraInit(m_pCameraHandle));
	m_bInitialized = true;
}

CSpinnakerDeviceInstance::~CSpinnakerDeviceInstance()
{
	if (m_pSpinnakerSDK.get() != nullptr) {
		if (m_pCameraHandle != nullptr) {
			if (m_bInitialized) {
				m_pSpinnakerSDK->SpinCameraDeInit(m_pCameraHandle);
				m_bInitialized = false;
			}
			m_pSpinnakerSDK->SpinCameraRelease(m_pCameraHandle);
		}
	}
	m_pCameraHandle = nullptr;
	m_pSpinnakerSDK = nullptr;
}

void CSpinnakerDeviceInstance::close()
{
	if (m_pSpinnakerSDK.get() != nullptr) {
		if (m_pCameraHandle != nullptr && m_bInitialized) {
			m_pSpinnakerSDK->checkError(m_pSpinnakerSDK->SpinCameraDeInit(m_pCameraHandle));
			m_bInitialized = false;
		}
	}
}

bool CSpinnakerDeviceInstance::isOpen()
{
	return m_bInitialized;
}

std::string CSpinnakerDeviceInstance::getIdentifier()
{
	return m_sIdentifier;
}

bool CSpinnakerDeviceInstance::featureIsAvailable(const std::string& sFeatureName)
{
	libSpinnakerNodeHandle hNode = getFeatureNode(m_pSpinnakerSDK, m_pCameraHandle, sFeatureName);
	if (hNode == nullptr)
		return false;

	spinnakerBool bAvailable = 0;
	m_pSpinnakerSDK->checkError(m_pSpinnakerSDK->SpinNodeIsAvailable(hNode, &bAvailable));
	return bAvailable != 0;
}

bool CSpinnakerDeviceInstance::featureIsImplemented(const std::string& sFeatureName)
{
	return featureIsAvailable(sFeatureName);
}

bool CSpinnakerDeviceInstance::featureIsReadable(const std::string& sFeatureName)
{
	libSpinnakerNodeHandle hNode = getFeatureNode(m_pSpinnakerSDK, m_pCameraHandle, sFeatureName);
	if (hNode == nullptr)
		return false;

	spinnakerBool bReadable = 0;
	m_pSpinnakerSDK->checkError(m_pSpinnakerSDK->SpinNodeIsReadable(hNode, &bReadable));
	return bReadable != 0;
}

bool CSpinnakerDeviceInstance::featureIsWriteable(const std::string& sFeatureName)
{
	libSpinnakerNodeHandle hNode = getFeatureNode(m_pSpinnakerSDK, m_pCameraHandle, sFeatureName);
	if (hNode == nullptr)
		return false;

	spinnakerBool bWritable = 0;
	m_pSpinnakerSDK->checkError(m_pSpinnakerSDK->SpinNodeIsWritable(hNode, &bWritable));
	return bWritable != 0;
}

void CSpinnakerDeviceInstance::setFloatFeature(const std::string& sFeatureName, const double dFloatValue)
{
	libSpinnakerNodeHandle hNode = getFeatureNode(m_pSpinnakerSDK, m_pCameraHandle, sFeatureName);
	m_pSpinnakerSDK->checkError(m_pSpinnakerSDK->SpinFloatSetValue(hNode, dFloatValue));
}

double CSpinnakerDeviceInstance::getFloatFeature(const std::string& sFeatureName)
{
	double dValue = 0.0;
	libSpinnakerNodeHandle hNode = getFeatureNode(m_pSpinnakerSDK, m_pCameraHandle, sFeatureName);
	m_pSpinnakerSDK->checkError(m_pSpinnakerSDK->SpinFloatGetValue(hNode, &dValue));
	return dValue;
}

void CSpinnakerDeviceInstance::getFloatFeatureRange(const std::string& sFeatureName, double& dMinFloatValue, double& dMaxFloatValue)
{
	dMinFloatValue = 0.0;
	dMaxFloatValue = 0.0;
	libSpinnakerNodeHandle hNode = getFeatureNode(m_pSpinnakerSDK, m_pCameraHandle, sFeatureName);
	m_pSpinnakerSDK->checkError(m_pSpinnakerSDK->SpinFloatGetMin(hNode, &dMinFloatValue));
	m_pSpinnakerSDK->checkError(m_pSpinnakerSDK->SpinFloatGetMax(hNode, &dMaxFloatValue));
}

void CSpinnakerDeviceInstance::setIntegerFeature(const std::string& sFeatureName, const int64_t nIntegerValue)
{
	libSpinnakerNodeHandle hNode = getFeatureNode(m_pSpinnakerSDK, m_pCameraHandle, sFeatureName);
	m_pSpinnakerSDK->checkError(m_pSpinnakerSDK->SpinIntegerSetValue(hNode, nIntegerValue));
}

LibMCDriver_Spinnaker_int64 CSpinnakerDeviceInstance::getIntegerFeature(const std::string& sFeatureName)
{
	int64_t nValue = 0;
	libSpinnakerNodeHandle hNode = getFeatureNode(m_pSpinnakerSDK, m_pCameraHandle, sFeatureName);
	m_pSpinnakerSDK->checkError(m_pSpinnakerSDK->SpinIntegerGetValue(hNode, &nValue));
	return nValue;
}

void CSpinnakerDeviceInstance::getIntegerFeatureRange(const std::string& sFeatureName, int64_t& nMinIntegerValue, int64_t& nMaxIntegerValue)
{
	nMinIntegerValue = 0;
	nMaxIntegerValue = 0;
	libSpinnakerNodeHandle hNode = getFeatureNode(m_pSpinnakerSDK, m_pCameraHandle, sFeatureName);
	m_pSpinnakerSDK->checkError(m_pSpinnakerSDK->SpinIntegerGetMin(hNode, &nMinIntegerValue));
	m_pSpinnakerSDK->checkError(m_pSpinnakerSDK->SpinIntegerGetMax(hNode, &nMaxIntegerValue));
}

void CSpinnakerDeviceInstance::setBoolFeature(const std::string& sFeatureName, const bool bBoolValue)
{
	spinnakerBool bSpinBool = bBoolValue ? 1 : 0;
	libSpinnakerNodeHandle hNode = getFeatureNode(m_pSpinnakerSDK, m_pCameraHandle, sFeatureName);
	m_pSpinnakerSDK->checkError(m_pSpinnakerSDK->SpinBooleanSetValue(hNode, bSpinBool));
}

bool CSpinnakerDeviceInstance::getBoolFeature(const std::string& sFeatureName)
{
	spinnakerBool bSpinBool = 0;
	libSpinnakerNodeHandle hNode = getFeatureNode(m_pSpinnakerSDK, m_pCameraHandle, sFeatureName);
	m_pSpinnakerSDK->checkError(m_pSpinnakerSDK->SpinBooleanGetValue(hNode, &bSpinBool));
	return bSpinBool != 0;
}

void CSpinnakerDeviceInstance::setAutoExposure(const bool bDoAutoExpose)
{
	try {
		libSpinnakerNodeHandle hNode = getFeatureNode(m_pSpinnakerSDK, m_pCameraHandle, SPINNAKER_FEATURENAME_AUTOEXPOSURE);
		const std::string sEntry = bDoAutoExpose ? "Continuous" : "Off";
		int64_t nValue = getEnumEntryValue(m_pSpinnakerSDK, hNode, sEntry);
		m_pSpinnakerSDK->checkError(m_pSpinnakerSDK->SpinEnumerationSetIntValue(hNode, nValue));
		return;
	}
	catch (...) {
	}

	setBoolFeature(SPINNAKER_FEATURENAME_AUTOEXPOSURE, bDoAutoExpose);
}

bool CSpinnakerDeviceInstance::getAutoExposure()
{
	try {
		libSpinnakerNodeHandle hNode = getFeatureNode(m_pSpinnakerSDK, m_pCameraHandle, SPINNAKER_FEATURENAME_AUTOEXPOSURE);
		libSpinnakerEnumEntryHandle hEntry = nullptr;
		m_pSpinnakerSDK->checkError(m_pSpinnakerSDK->SpinEnumerationGetCurrentEntry(hNode, &hEntry));
		if (hEntry != nullptr) {
			std::string sSymbolic = getEnumEntrySymbolic(m_pSpinnakerSDK, hEntry);
			if (!sSymbolic.empty())
				return sSymbolic != "Off";

			int64_t nValue = 0;
			m_pSpinnakerSDK->checkError(m_pSpinnakerSDK->SpinEnumerationEntryGetIntValue(hEntry, &nValue));
			int64_t nOffValue = getEnumEntryValue(m_pSpinnakerSDK, hNode, "Off");
			return nValue != nOffValue;
		}
	}
	catch (...) {
	}

	return getBoolFeature(SPINNAKER_FEATURENAME_AUTOEXPOSURE);
}

void CSpinnakerDeviceInstance::setExposureTime(const double dExposureTime)
{
	setFloatFeature(SPINNAKER_FEATURENAME_EXPOSURETIME, dExposureTime);
}

double CSpinnakerDeviceInstance::getExposureTime()
{
	return getFloatFeature(SPINNAKER_FEATURENAME_EXPOSURETIME);
}

void CSpinnakerDeviceInstance::grabSingleGreyscaleImage(const LibMCDriver_Spinnaker_uint32 nChannelID, const LibMCDriver_Spinnaker_uint32 nTimeoutInMS, LibMCEnv::PImageData pImageInstance)
{
	if (pImageInstance.get() == nullptr)
		throw ELibMCDriver_SpinnakerInterfaceException(LIBMCDRIVER_SPINNAKER_ERROR_INVALIDPARAM);
	if (nChannelID != 0)
		throw ELibMCDriver_SpinnakerInterfaceException(LIBMCDRIVER_SPINNAKER_ERROR_INVALIDPARAM);
	if (!m_bInitialized)
		throw ELibMCDriver_SpinnakerInterfaceException(LIBMCDRIVER_SPINNAKER_ERROR_DRIVERNOTCONNECTED);

	libSpinnakerImageHandle hImage = nullptr;
	bool bAcquisitionStarted = false;

	try {
		m_pSpinnakerSDK->checkError(m_pSpinnakerSDK->SpinCameraBeginAcquisition(m_pCameraHandle));
		bAcquisitionStarted = true;

		if (m_pSpinnakerSDK->SpinCameraGetNextImageEx != nullptr) {
			m_pSpinnakerSDK->checkError(m_pSpinnakerSDK->SpinCameraGetNextImageEx(m_pCameraHandle, nTimeoutInMS, &hImage));
		}
		else {
			m_pSpinnakerSDK->checkError(m_pSpinnakerSDK->SpinCameraGetNextImage(m_pCameraHandle, &hImage));
		}

		if (hImage == nullptr)
			throw ELibMCDriver_SpinnakerInterfaceException(LIBMCDRIVER_SPINNAKER_ERROR_IMAGENOTREADY);

		spinnakerBool bIncomplete = 0;
		m_pSpinnakerSDK->checkError(m_pSpinnakerSDK->SpinImageIsIncomplete(hImage, &bIncomplete));
		if (bIncomplete != 0)
			throw ELibMCDriver_SpinnakerInterfaceException(LIBMCDRIVER_SPINNAKER_ERROR_IMAGENOTREADY);

		size_t nWidth = 0;
		size_t nHeight = 0;
		size_t nDataSize = 0;
		void* pData = nullptr;
		m_pSpinnakerSDK->checkError(m_pSpinnakerSDK->SpinImageGetWidth(hImage, &nWidth));
		m_pSpinnakerSDK->checkError(m_pSpinnakerSDK->SpinImageGetHeight(hImage, &nHeight));
		m_pSpinnakerSDK->checkError(m_pSpinnakerSDK->SpinImageGetDataSize(hImage, &nDataSize));
		m_pSpinnakerSDK->checkError(m_pSpinnakerSDK->SpinImageGetData(hImage, &pData));

		if ((nWidth == 0) || (nHeight == 0))
			throw ELibMCDriver_SpinnakerInterfaceException(LIBMCDRIVER_SPINNAKER_ERROR_INVALIDGRABIMAGESIZE);

		uint32_t nImageSizeX = 0;
		uint32_t nImageSizeY = 0;
		pImageInstance->GetSizeInPixels(nImageSizeX, nImageSizeY);
		if ((nImageSizeX != (uint32_t)nWidth) || (nImageSizeY != (uint32_t)nHeight))
			throw ELibMCDriver_SpinnakerInterfaceException(LIBMCDRIVER_SPINNAKER_ERROR_IMAGEDATASIZEMISMATCH,
				"Image data size does not match grabbed image size " +
				std::to_string(nImageSizeX) + "x" + std::to_string(nImageSizeY) + " != " +
				std::to_string(nWidth) + "x" + std::to_string(nHeight));

		if (nDataSize != (nWidth * nHeight))
			throw ELibMCDriver_SpinnakerInterfaceException(LIBMCDRIVER_SPINNAKER_ERROR_IMAGEDATASIZEMISMATCH,
				"Image payload size does not match image dimensions");

		std::vector<uint8_t> payloadBuffer;
		payloadBuffer.resize(nDataSize);
		std::memcpy(payloadBuffer.data(), pData, nDataSize);

		pImageInstance->SetPixels(0, 0, nImageSizeX, nImageSizeY, LibMCEnv::eImagePixelFormat::GreyScale8bit, payloadBuffer);

		m_pSpinnakerSDK->checkError(m_pSpinnakerSDK->SpinImageRelease(hImage));
		hImage = nullptr;
		m_pSpinnakerSDK->checkError(m_pSpinnakerSDK->SpinCameraEndAcquisition(m_pCameraHandle));
		bAcquisitionStarted = false;
	}
	catch (...) {
		if (hImage != nullptr) {
			m_pSpinnakerSDK->SpinImageRelease(hImage);
			hImage = nullptr;
		}
		if (bAcquisitionStarted) {
			m_pSpinnakerSDK->SpinCameraEndAcquisition(m_pCameraHandle);
		}
		throw;
	}
}
