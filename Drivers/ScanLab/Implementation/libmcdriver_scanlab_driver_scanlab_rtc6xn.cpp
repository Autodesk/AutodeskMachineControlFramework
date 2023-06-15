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


Abstract: This is a stub class definition of CDriver_ScanLab_RTC6xN

*/

#include "libmcdriver_scanlab_driver_scanlab_rtc6xn.hpp"
#include "libmcdriver_scanlab_interfaceexception.hpp"

#include <cmath>

// Include custom headers here.
#define RTC6_MINLASERCOUNT 2
#define RTC6_MAXLASERCOUNT 64

using namespace LibMCDriver_ScanLab::Impl;

/*************************************************************************************************************************
 Class definition of CDriver_ScanLab_RTC6xN
**************************************************************************************************************************/

CDriver_ScanLab_RTC6xN::CDriver_ScanLab_RTC6xN(const std::string& sName, const std::string& sType, uint32_t nScannerCount, LibMCEnv::PDriverEnvironment pDriverEnvironment)
	: CDriver_ScanLab(pDriverEnvironment), m_sName(sName), m_sType(sType), m_fMaxLaserPowerInWatts(0.0f), m_SimulationMode(false), m_nScannerCount(nScannerCount),
		m_OIERecordingMode (LibMCDriver_ScanLab::eOIERecordingMode::OIERecordingDisabled)
{
	if ((nScannerCount < RTC6_MINLASERCOUNT) || (nScannerCount > RTC6_MAXLASERCOUNT))
		throw ELibMCDriver_ScanLabInterfaceException (LIBMCDRIVER_SCANLAB_ERROR_INVALIDSCANNERCOUNT);


}

CDriver_ScanLab_RTC6xN::~CDriver_ScanLab_RTC6xN()
{

}

void CDriver_ScanLab_RTC6xN::Configure(const std::string& sConfigurationString)
{

	if (!sConfigurationString.empty()) {
		auto pXMLDocument = m_pDriverEnvironment->ParseXMLString(sConfigurationString);

		std::string sXMLNs = pXMLDocument->GetDefaultNamespace();
		if (sXMLNs != SCANLAB_CONFIGURATIONSCHEMA)
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDCONFIGURATIONSCHEMA);

		auto pConfigurationNode = pXMLDocument->GetRootNode();

		auto pPresetsNode = pConfigurationNode->FindChild("", "presets", false);

		auto pPresetNodes = pPresetsNode->GetChildrenByName("", "preset");
		size_t nPresetCount = pPresetNodes->GetNodeCount();

		for (size_t nIndex = 0; nIndex < nPresetCount; nIndex++) {

			auto pPresetNode = pPresetNodes->GetNode(nIndex);

			auto pConfigurationPreset = std::make_shared<CDriver_ScanLab_RTC6ConfigurationPreset>(pPresetNode);

			std::string sName = pConfigurationPreset->getName();
			auto iIter = m_ConfigurationPresets.find(sName);

			if (iIter != m_ConfigurationPresets.end())
				throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_DUPLICATECONFIGURATIONPRESETNAME, "duplicate configuration preset name: " + sName);

			m_ConfigurationPresets.insert(std::make_pair(sName, pConfigurationPreset));

		}

	}

	for (uint32_t nIndex = 1; nIndex <= m_nScannerCount; nIndex++) {

		std::string sPrefix = "scanner" + std::to_string(nIndex) + "_";

		m_pDriverEnvironment->RegisterIntegerParameter(sPrefix + "laserindex", "Laser Index", 0);
		m_pDriverEnvironment->RegisterIntegerParameter(sPrefix + "serialnumber", "Serial Number", 0);
		m_pDriverEnvironment->RegisterBoolParameter(sPrefix + "position_x_ok", "Scan Position X is ok", false);
		m_pDriverEnvironment->RegisterBoolParameter(sPrefix + "position_y_ok", "Scan Position Y is ok", false);
		m_pDriverEnvironment->RegisterBoolParameter(sPrefix + "temperature_ok", "Scan Head Temperature is ok", false);
		m_pDriverEnvironment->RegisterBoolParameter(sPrefix + "power_ok", "Scan Head Power is ok", false);
		m_pDriverEnvironment->RegisterBoolParameter(sPrefix + "laser_on", "Laser is On", false);
		m_pDriverEnvironment->RegisterIntegerParameter(sPrefix + "position_x", "Laser Position X", 0);
		m_pDriverEnvironment->RegisterIntegerParameter(sPrefix + "position_y", "Laser Position Y", 0);
		m_pDriverEnvironment->RegisterIntegerParameter(sPrefix + "position_z", "Laser Position Z", 0);
		m_pDriverEnvironment->RegisterIntegerParameter(sPrefix + "position_x_corrected", "corrected Laser Position X", 0);
		m_pDriverEnvironment->RegisterIntegerParameter(sPrefix + "position_y_corrected", "corrected Laser Position Y", 0);
		m_pDriverEnvironment->RegisterIntegerParameter(sPrefix + "position_z_corrected", "corrected Laser Position Z", 0);
		m_pDriverEnvironment->RegisterIntegerParameter(sPrefix + "focus_shift", "current Focus Shift", 0);
		m_pDriverEnvironment->RegisterIntegerParameter(sPrefix + "mark_speed", "current Mark Speed", 0);
		m_pDriverEnvironment->RegisterIntegerParameter(sPrefix + "list_position", "current List Position", 0);
		m_pDriverEnvironment->RegisterIntegerParameter(sPrefix + "encoder_position_x", "Encoder Position X", 0);
		m_pDriverEnvironment->RegisterIntegerParameter(sPrefix + "encoder_position_y", "Encoder Position Y", 0);
		m_pDriverEnvironment->RegisterBoolParameter(sPrefix + "card_busy", "Card is busy", false);
		m_pDriverEnvironment->RegisterIntegerParameter(sPrefix + "rtc_version", "Scanlab RTC Version", 0);
		m_pDriverEnvironment->RegisterIntegerParameter(sPrefix + "card_type", "Scanlab RTC Type", 0);
		m_pDriverEnvironment->RegisterIntegerParameter(sPrefix + "dll_version", "Scanlab DLL Version", 0);
		m_pDriverEnvironment->RegisterIntegerParameter(sPrefix + "hex_version", "Scanlab HEX Version", 0);
		m_pDriverEnvironment->RegisterIntegerParameter(sPrefix + "bios_version", "Scanlab BIOS Version", 0);
	}
}

std::string CDriver_ScanLab_RTC6xN::GetName()
{
	return m_sName;
}

std::string CDriver_ScanLab_RTC6xN::GetType()
{
	return m_sType;
}

void CDriver_ScanLab_RTC6xN::QueryParameters()
{
	QueryParametersEx(m_pDriverEnvironment->CreateStatusUpdateSession());
}

void CDriver_ScanLab_RTC6xN::QueryParametersEx(LibMCEnv::PDriverStatusUpdateSession pDriverUpdateInstance)
{
	updateCardStatus(pDriverUpdateInstance);
}



void CDriver_ScanLab_RTC6xN::SetToSimulationMode()
{
	m_SimulationMode = true;
}

bool CDriver_ScanLab_RTC6xN::IsSimulationMode()
{
	return m_SimulationMode;
}

bool CDriver_ScanLab_RTC6xN::IsInitialized()
{		
	for (uint32_t nScannerIndex = 1; nScannerIndex <= m_nScannerCount; nScannerIndex++) {
		if (!ScannerIsInitialized (nScannerIndex))
			return false;
	}

	return true;
		
}

bool CDriver_ScanLab_RTC6xN::ScannerIsInitialized(const LibMCDriver_ScanLab_uint32 nScannerIndex)
{
	auto iIter = m_ScannerContexts.find(nScannerIndex);
	return (iIter != m_ScannerContexts.end());
}


LibMCDriver_ScanLab_uint32 CDriver_ScanLab_RTC6xN::GetScannerCount()
{
	return m_nScannerCount;
}

void CDriver_ScanLab_RTC6xN::InitialiseScanner(const LibMCDriver_ScanLab_uint32 nScannerIndex, const std::string & sIP, const std::string & sNetmask, const LibMCDriver_ScanLab_uint32 nTimeout, const LibMCDriver_ScanLab_uint32 nSerialNumber, const LibMCDriver_ScanLab_uint32 nLaserIndex)
{

	if ((nScannerIndex < 1) || (nScannerIndex > m_nScannerCount))
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDSCANNERINDEX, "invalid scanner index: " + std::to_string (nScannerIndex));

	std::string sPrefix = "scanner" + std::to_string(nScannerIndex) + "_";

	if (m_SimulationMode) {
		m_pDriverEnvironment->SetIntegerParameter(sPrefix + "laserindex", nLaserIndex);
		m_pDriverEnvironment->SetIntegerParameter(sPrefix + "rtc_version", 1);
		m_pDriverEnvironment->SetIntegerParameter(sPrefix + "card_type", 1);
		m_pDriverEnvironment->SetIntegerParameter(sPrefix + "dll_version", 1);
		m_pDriverEnvironment->SetIntegerParameter(sPrefix + "hex_version", 1);
		m_pDriverEnvironment->SetIntegerParameter(sPrefix + "bios_version", 1);
		m_pDriverEnvironment->SetIntegerParameter(sPrefix + "serialnumber", (uint32_t) (123450ULL + nScannerIndex));

	}
	else {

		auto iLaserIndexIter = m_LaserIndexMapping.find(nLaserIndex);
		if (iLaserIndexIter != m_LaserIndexMapping.end())
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_DUPLICATELASERINDEX, "duplicate laser index: " + std::to_string(nLaserIndex));


		if (m_pRTCSelector.get() == nullptr)
			m_pRTCSelector = act_managed_ptr<IRTCSelector>(CreateRTCSelector());

		m_ScannerContexts.erase(nScannerIndex);

		act_managed_ptr<IRTCContext> pRTCContext;
		if (sIP.empty()) {
			pRTCContext = act_managed_ptr<IRTCContext>(m_pRTCSelector->AcquireCardBySerial(nSerialNumber));
		}
		else {
			m_pRTCSelector->SearchCards(sIP, sNetmask, nTimeout);
			auto pContext = m_pRTCSelector->AcquireEthernetCardBySerial(nSerialNumber);
			pRTCContext = act_managed_ptr<IRTCContext>(pContext);

			auto pContextInstance = dynamic_cast<CRTCContext*> (pContext);
			if (pContextInstance != nullptr)
				pContextInstance->setIPAddress(sIP, sNetmask);
		}

		uint32_t nRTCVersion = 0;
		uint32_t nRTCType = 0;
		uint32_t nDLLVersion = 0;
		uint32_t nHEXVersion = 0;
		uint32_t nBIOSVersion = 0;
		pRTCContext->GetRTCVersion(nRTCVersion, nRTCType, nDLLVersion, nHEXVersion, nBIOSVersion);
		m_pDriverEnvironment->SetIntegerParameter(sPrefix + "laserindex", nLaserIndex);
		m_pDriverEnvironment->SetIntegerParameter(sPrefix + "rtc_version", nRTCVersion);
		m_pDriverEnvironment->SetIntegerParameter(sPrefix + "card_type", nRTCType);
		m_pDriverEnvironment->SetIntegerParameter(sPrefix + "dll_version", nDLLVersion);
		m_pDriverEnvironment->SetIntegerParameter(sPrefix + "hex_version", nHEXVersion);
		m_pDriverEnvironment->SetIntegerParameter(sPrefix + "bios_version", nBIOSVersion);
		m_pDriverEnvironment->SetIntegerParameter(sPrefix + "serialnumber", pRTCContext->GetSerialNumber());

		m_ScannerContexts.insert(std::make_pair (nScannerIndex, pRTCContext));
		m_LaserIndexMapping.insert(std::make_pair (nLaserIndex, pRTCContext));
	}

}

void CDriver_ScanLab_RTC6xN::InitialiseScannerFromConfiguration(const LibMCDriver_ScanLab_uint32 nScannerIndex, const std::string & sPresetName)
{
	PDriver_ScanLab_RTC6ConfigurationPreset pPreset = findPresetByName(sPresetName, true);

	InitialiseScanner(nScannerIndex, pPreset->getIP(), pPreset->getNetmask(), pPreset->getTimeout(), pPreset->getSerialNumber(), pPreset->getLaserIndex ());

	SetCommunicationTimeouts(nScannerIndex, pPreset->getInitialTimeout(), pPreset->getMaxTimeout(), pPreset->getMultiplier());

	LoadFirmware(nScannerIndex, pPreset->getFirmwareResourceName(), pPreset->getFPGAResourceName(), pPreset->getAuxiliaryResourceName());

	std::vector<uint8_t> CorrectionBuffer;
	m_pDriverEnvironment->RetrieveMachineResourceData(pPreset->getCorrectionResourceName(), CorrectionBuffer);

	if (CorrectionBuffer.size() == 0)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDRTCCORRECTIONDATA);

	SetCorrectionFile(nScannerIndex, CorrectionBuffer.size(), CorrectionBuffer.data(), pPreset->getCorrectionTableNumber(), pPreset->getCorrectionDimension(), pPreset->getCorrectionTableNumberHeadA(), pPreset->getCorrectionTableNumberHeadB());

	ConfigureDelays(nScannerIndex, pPreset->getLaserOnDelay(), pPreset->getLaserOffDelay(), pPreset->getMarkDelay(), pPreset->getJumpDelay(), pPreset->getPolygonDelay());

	ConfigureLaserMode(nScannerIndex, pPreset->getLaserMode(), pPreset->getLaserPort(), pPreset->getMaxLaserPower(), pPreset->getFinishLaserPulseAfterOn(), pPreset->getPhaseShiftOfLaserSignal(), pPreset->getLaserOnSignalLowActive(), pPreset->getLaserHalfSignalsLowActive(), pPreset->getSetDigitalInOneHighActive(), pPreset->getOutputSynchronizationActive());
}

std::string CDriver_ScanLab_RTC6xN::GetIPAddress(const LibMCDriver_ScanLab_uint32 nScannerIndex)
{
	auto pRTCContext = getRTCContextForScannerIndex(nScannerIndex, true);

	return pRTCContext->GetIPAddress();
}

std::string CDriver_ScanLab_RTC6xN::GetNetmask(const LibMCDriver_ScanLab_uint32 nScannerIndex)
{
	auto pRTCContext = getRTCContextForScannerIndex(nScannerIndex, true);

	return pRTCContext->GetNetmask();
}

LibMCDriver_ScanLab_uint32 CDriver_ScanLab_RTC6xN::GetSerialNumber(const LibMCDriver_ScanLab_uint32 nScannerIndex)
{
	auto pRTCContext = getRTCContextForScannerIndex(nScannerIndex, true);

	return pRTCContext->GetSerialNumber();
}

LibMCDriver_ScanLab_uint32 CDriver_ScanLab_RTC6xN::GetLaserIndex(const LibMCDriver_ScanLab_uint32 nScannerIndex)
{
	auto pRTCContext = getRTCContextForScannerIndex(nScannerIndex, true);

	return pRTCContext->GetLaserIndex();
}

IRTCSelector* CDriver_ScanLab_RTC6xN::GetSelector()
{
	// Attention, Caller MUST reduce Reference count with the result
	if (m_pRTCSelector.get() == nullptr)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CARDNOTINITIALIZED);

	m_pRTCSelector->IncRefCount();

	return m_pRTCSelector.get();
}


IRTCContext * CDriver_ScanLab_RTC6xN::GetContext(const LibMCDriver_ScanLab_uint32 nScannerIndex)
{
	// Attention, Caller MUST reduce Reference count with the result
	auto pRTCContext = getRTCContextForScannerIndex(nScannerIndex, true);
	pRTCContext->IncRefCount();
	return pRTCContext.get();
}

void CDriver_ScanLab_RTC6xN::LoadFirmware(const LibMCDriver_ScanLab_uint32 nScannerIndex, const std::string & sFirmwareResource, const std::string & sFPGAResource, const std::string & sAuxiliaryResource)
{
	if (!m_SimulationMode) {

		auto pRTCContext = getRTCContextForScannerIndex(nScannerIndex, true);

		std::vector<uint8_t> FirmwareData;
		if (m_pDriverEnvironment->MachineHasResourceData(sFirmwareResource)) {
			m_pDriverEnvironment->RetrieveMachineResourceData(sFirmwareResource, FirmwareData);
		}
		else {
			m_pDriverEnvironment->RetrieveDriverResourceData(sFirmwareResource, FirmwareData);
		}

		std::vector<uint8_t> FPGAData;
		if (m_pDriverEnvironment->MachineHasResourceData(sFPGAResource)) {
			m_pDriverEnvironment->RetrieveMachineResourceData(sFPGAResource, FPGAData);
		}
		else {
			m_pDriverEnvironment->RetrieveDriverResourceData(sFPGAResource, FPGAData);
		}

		std::vector<uint8_t> AuxiliaryData;
		if (m_pDriverEnvironment->MachineHasResourceData(sAuxiliaryResource)) {
			m_pDriverEnvironment->RetrieveMachineResourceData(sAuxiliaryResource, AuxiliaryData);
		}
		else {
			m_pDriverEnvironment->RetrieveDriverResourceData(sAuxiliaryResource, AuxiliaryData);
		}

		pRTCContext->LoadFirmware(FirmwareData.size(), FirmwareData.data(), FPGAData.size(), FPGAData.data(), AuxiliaryData.size(), AuxiliaryData.data());

	}
}

void CDriver_ScanLab_RTC6xN::LoadCustomFirmware(const LibMCDriver_ScanLab_uint32 nScannerIndex, const LibMCDriver_ScanLab_uint64 nFirmwareDataBufferSize, const LibMCDriver_ScanLab_uint8 * pFirmwareDataBuffer, const LibMCDriver_ScanLab_uint64 nFPGADataBufferSize, const LibMCDriver_ScanLab_uint8 * pFPGADataBuffer, const LibMCDriver_ScanLab_uint64 nAuxiliaryDataBufferSize, const LibMCDriver_ScanLab_uint8 * pAuxiliaryDataBuffer)
{
	if (!m_SimulationMode) {
		auto pRTCContext = getRTCContextForScannerIndex(nScannerIndex, true);

		pRTCContext->LoadFirmware(nFirmwareDataBufferSize, pFirmwareDataBuffer, nFPGADataBufferSize, pFPGADataBuffer, nAuxiliaryDataBufferSize, pAuxiliaryDataBuffer);
	}
}

void CDriver_ScanLab_RTC6xN::SetCorrectionFile(const LibMCDriver_ScanLab_uint32 nScannerIndex, const LibMCDriver_ScanLab_uint64 nCorrectionFileBufferSize, const LibMCDriver_ScanLab_uint8 * pCorrectionFileBuffer, const LibMCDriver_ScanLab_uint32 nTableNumber, const LibMCDriver_ScanLab_uint32 nDimension, const LibMCDriver_ScanLab_uint32 nTableNumberHeadA, const LibMCDriver_ScanLab_uint32 nTableNumberHeadB)
{
	if (!m_SimulationMode) {
		auto pRTCContext = getRTCContextForScannerIndex(nScannerIndex, true);

		pRTCContext->LoadCorrectionFile(nCorrectionFileBufferSize, pCorrectionFileBuffer, nTableNumber, nDimension);
		pRTCContext->SelectCorrectionTable(nTableNumberHeadA, nTableNumberHeadB);

	}
}

void CDriver_ScanLab_RTC6xN::ConfigureLaserMode(const LibMCDriver_ScanLab_uint32 nScannerIndex, const LibMCDriver_ScanLab::eLaserMode eLaserMode, const LibMCDriver_ScanLab::eLaserPort eLaserPort, const LibMCDriver_ScanLab_double dMaxLaserPower, const bool bFinishLaserPulseAfterOn, const bool bPhaseShiftOfLaserSignal, const bool bLaserOnSignalLowActive, const bool bLaserHalfSignalsLowActive, const bool bSetDigitalInOneHighActive, const bool bOutputSynchronizationActive)
{
	if (!m_SimulationMode) {
		auto pRTCContext = getRTCContextForScannerIndex(nScannerIndex, true);

		if (((float)dMaxLaserPower < RTC6_MIN_MAXLASERPOWER) || ((float)dMaxLaserPower > RTC6_MAX_MAXLASERPOWER))
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDMAXLASERPOWER);

		m_fMaxLaserPowerInWatts = (float)dMaxLaserPower;

		pRTCContext->ConfigureLists(1 << 22, 1 << 22);
		pRTCContext->SetLaserMode(eLaserMode, eLaserPort);
		pRTCContext->DisableAutoLaserControl();
		pRTCContext->SetLaserControlParameters(false, bFinishLaserPulseAfterOn, bPhaseShiftOfLaserSignal, bLaserOnSignalLowActive, bLaserHalfSignalsLowActive, bSetDigitalInOneHighActive, bOutputSynchronizationActive);
		pRTCContext->SetLaserPulsesInMicroSeconds(5, 5);
		pRTCContext->SetStandbyInMicroSeconds(1, 1);

	}
}

void CDriver_ScanLab_RTC6xN::ConfigureDelays(const LibMCDriver_ScanLab_uint32 nScannerIndex, const LibMCDriver_ScanLab_double dLaserOnDelay, const LibMCDriver_ScanLab_double dLaserOffDelay, const LibMCDriver_ScanLab_double dMarkDelay, const LibMCDriver_ScanLab_double dJumpDelay, const LibMCDriver_ScanLab_double dPolygonDelay)
{

	if (!m_SimulationMode) {

		auto pRTCContext = getRTCContextForScannerIndex(nScannerIndex, true);

		if ((dLaserOnDelay < RTC6_MIN_LASER_DELAY) || (dLaserOnDelay > RTC6_MAX_LASER_DELAY))
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDLASERDELAY);
		if ((dLaserOffDelay < RTC6_MIN_LASER_DELAY) || (dLaserOffDelay > RTC6_MAX_LASER_DELAY))
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDLASERDELAY);
		if ((dMarkDelay < RTC6_MIN_MARK_DELAY) || (dMarkDelay > RTC6_MAX_MARK_DELAY))
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDSCANNERDELAY);
		if ((dJumpDelay < RTC6_MIN_JUMP_DELAY) || (dJumpDelay > RTC6_MAX_JUMP_DELAY))
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDSCANNERDELAY);
		if ((dPolygonDelay < RTC6_MIN_POLYGON_DELAY) || (dPolygonDelay > RTC6_MAX_POLYGON_DELAY))
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDSCANNERDELAY);


		int nIntMarkDelay = ((int)round(dMarkDelay / 10.0)) * 10;
		int nIntJumpDelay = ((int)round(dJumpDelay / 10.0)) * 10;
		int nIntPolygonDelay = ((int)round(dPolygonDelay / 10.0)) * 10;

		if (nIntMarkDelay < 0)
			nIntMarkDelay = 0;
		if (nIntJumpDelay < 0)
			nIntJumpDelay = 0;
		if (nIntPolygonDelay < 0)
			nIntPolygonDelay = 0;

		pRTCContext->SetStartList(1, 0);
		pRTCContext->SetLaserDelaysInMicroseconds(dLaserOnDelay, dLaserOffDelay);
		pRTCContext->SetDelays(nIntMarkDelay, nIntJumpDelay, nIntPolygonDelay);
		pRTCContext->SetEndOfList();
		pRTCContext->ExecuteList(1, 0);

	}
}

void CDriver_ScanLab_RTC6xN::SetOIERecordingMode(const LibMCDriver_ScanLab::eOIERecordingMode eRecordingMode)
{
	m_OIERecordingMode = eRecordingMode;
}

LibMCDriver_ScanLab::eOIERecordingMode CDriver_ScanLab_RTC6xN::GetOIERecordingMode()
{
	return m_OIERecordingMode;
}


void CDriver_ScanLab_RTC6xN::DrawLayer(const std::string & sStreamUUID, const LibMCDriver_ScanLab_uint32 nLayerIndex, const bool bFailIfNonAssignedDataExists)
{
	if (!m_SimulationMode) {

		if ((m_fMaxLaserPowerInWatts < RTC6_MIN_MAXLASERPOWER) || (m_fMaxLaserPowerInWatts > RTC6_MAX_MAXLASERPOWER))
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDMAXLASERPOWER);

		bool bEnableOIEMeasurementPerHatch = (m_OIERecordingMode == eOIERecordingMode::OIELaserActiveMeasurement) || (m_OIERecordingMode == eOIERecordingMode::OIEEnableAndLaserActiveMeasurement);

		for (uint32_t nScannerIndex = 1; nScannerIndex <= m_nScannerCount; nScannerIndex++) {
			auto pRTCContext = getRTCContextForScannerIndex(nScannerIndex, true);
			pRTCContext->SetStartList(1, 0);

			switch (m_OIERecordingMode) {
			case eOIERecordingMode::OIEEnableAndContinuousMeasurement:
			case eOIERecordingMode::OIEEnableAndLaserActiveMeasurement:
				pRTCContext->EnableOIE();
				break;
			}

			switch (m_OIERecordingMode) {
			case eOIERecordingMode::OIEContinuousMeasurement:
			case eOIERecordingMode::OIEEnableAndContinuousMeasurement:
				pRTCContext->StartOIEMeasurement();
				break;
			}


		}

		auto pToolpathAccessor = m_pDriverEnvironment->CreateToolpathAccessor(sStreamUUID);
		auto pLayer = pToolpathAccessor->LoadLayer(nLayerIndex);

		double dUnits = pToolpathAccessor->GetUnits();

		uint32_t nSegmentCount = pLayer->GetSegmentCount();
		for (uint32_t nSegmentIndex = 0; nSegmentIndex < nSegmentCount; nSegmentIndex++) {

			LibMCEnv::eToolpathSegmentType eSegmentType;
			uint32_t nPointCount;
			pLayer->GetSegmentInfo(nSegmentIndex, eSegmentType, nPointCount);

			if (nPointCount >= 2) {

				float fJumpSpeedInMMPerSecond = (float)pLayer->GetSegmentProfileTypedValue(nSegmentIndex, LibMCEnv::eToolpathProfileValueType::JumpSpeed);
				float fMarkSpeedInMMPerSecond = (float)pLayer->GetSegmentProfileTypedValue(nSegmentIndex, LibMCEnv::eToolpathProfileValueType::Speed);
				float fPowerInWatts = (float)pLayer->GetSegmentProfileTypedValue(nSegmentIndex, LibMCEnv::eToolpathProfileValueType::LaserPower);
				float fPowerInPercent = (fPowerInWatts * 100.f) / m_fMaxLaserPowerInWatts;
				float fLaserFocus = (float)pLayer->GetSegmentProfileTypedValue(nSegmentIndex, LibMCEnv::eToolpathProfileValueType::LaserFocus);

				int64_t nLaserIndex = pLayer->GetSegmentProfileIntegerValueDef(nSegmentIndex, "", "laserindex", 0);

				act_managed_ptr<IRTCContext> pRTCContext;

				pRTCContext = getRTCContextForLaserIndex((uint32_t) nLaserIndex, false);

				if (pRTCContext.get() == nullptr) {
					if (bFailIfNonAssignedDataExists)
						throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_LASERINDEXHASNOASSIGNEDSCANNER, "Laser index has no assigned scanner: " + std::to_string(nLaserIndex));
				}

				if (pRTCContext.get() != nullptr) {

					int64_t nSkywritingMode = pLayer->GetSegmentProfileIntegerValueDef(nSegmentIndex, "http://schemas.scanlab.com/skywriting/2023/01", "mode", 0);

					if (nSkywritingMode != 0) {
						double dSkywritingTimeLag = pLayer->GetSegmentProfileDoubleValue(nSegmentIndex, "http://schemas.scanlab.com/skywriting/2023/01", "timelag");
						int64_t nSkywritingLaserOnShift = pLayer->GetSegmentProfileIntegerValue(nSegmentIndex, "http://schemas.scanlab.com/skywriting/2023/01", "laseronshift");
						int64_t nSkywritingPrev = pLayer->GetSegmentProfileIntegerValue(nSegmentIndex, "http://schemas.scanlab.com/skywriting/2023/01", "nprev");
						int64_t nSkywritingPost = pLayer->GetSegmentProfileIntegerValue(nSegmentIndex, "http://schemas.scanlab.com/skywriting/2023/01", "npost");

						double dSkywritingLimit = 0.0;
						if (nSkywritingMode == 3) {
							dSkywritingLimit = pLayer->GetSegmentProfileDoubleValue(nSegmentIndex, "http://schemas.scanlab.com/skywriting/2023/01", "limit");
						}


						switch (nSkywritingMode) {
						case 1:
							pRTCContext->EnableSkyWritingMode1(dSkywritingTimeLag, nSkywritingLaserOnShift, nSkywritingPrev, nSkywritingPost);
							break;
						case 2:
							pRTCContext->EnableSkyWritingMode2(dSkywritingTimeLag, nSkywritingLaserOnShift, nSkywritingPrev, nSkywritingPost);
							break;
						case 3:
							pRTCContext->EnableSkyWritingMode3(dSkywritingTimeLag, nSkywritingLaserOnShift, nSkywritingPrev, nSkywritingPost, dSkywritingLimit);
							break;
						default:
							pRTCContext->DisableSkyWriting();
						}

					}

					std::vector<LibMCEnv::sPosition2D> Points;
					pLayer->GetSegmentPointData(nSegmentIndex, Points);

					if (nPointCount != Points.size())
						throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPOINTCOUNT);

					switch (eSegmentType) {
					case LibMCEnv::eToolpathSegmentType::Loop:
					case LibMCEnv::eToolpathSegmentType::Polyline:
					{

						std::vector<sPoint2D> ContourPoints;
						ContourPoints.resize(nPointCount);

						for (uint32_t nPointIndex = 0; nPointIndex < nPointCount; nPointIndex++) {
							auto pContourPoint = &ContourPoints.at(nPointIndex);
							pContourPoint->m_X = (float)(Points[nPointIndex].m_Coordinates[0] * dUnits);
							pContourPoint->m_Y = (float)(Points[nPointIndex].m_Coordinates[1] * dUnits);
						}

						pRTCContext->DrawPolylineOIE(nPointCount, ContourPoints.data(), fMarkSpeedInMMPerSecond, fJumpSpeedInMMPerSecond, fPowerInPercent, fLaserFocus, bEnableOIEMeasurementPerHatch);

						break;
					}

					case LibMCEnv::eToolpathSegmentType::Hatch:
					{
						if (nPointCount % 2 == 1)
							throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPOINTCOUNT);

						uint64_t nHatchCount = nPointCount / 2;
						std::vector<sHatch2D> Hatches;
						Hatches.resize(nHatchCount);

						for (uint64_t nHatchIndex = 0; nHatchIndex < nHatchCount; nHatchIndex++) {
							auto pHatch = &Hatches.at(nHatchIndex);
							pHatch->m_X1 = (float)(Points[nHatchIndex * 2].m_Coordinates[0] * dUnits);
							pHatch->m_Y1 = (float)(Points[nHatchIndex * 2].m_Coordinates[1] * dUnits);
							pHatch->m_X2 = (float)(Points[nHatchIndex * 2 + 1].m_Coordinates[0] * dUnits);
							pHatch->m_Y2 = (float)(Points[nHatchIndex * 2 + 1].m_Coordinates[1] * dUnits);
						}

						pRTCContext->DrawHatchesOIE(Hatches.size(), Hatches.data(), fMarkSpeedInMMPerSecond, fJumpSpeedInMMPerSecond, fPowerInPercent, fLaserFocus, bEnableOIEMeasurementPerHatch);

						break;
					}

					}

				}

			}

		}

		for (uint32_t nScannerIndex = 1; nScannerIndex <= m_nScannerCount; nScannerIndex++) {
			auto pRTCContext = getRTCContextForScannerIndex(nScannerIndex, true);

			if ((m_OIERecordingMode != eOIERecordingMode::OIERecordingDisabled))
				pRTCContext->StopOIEMeasurement();

			switch (m_OIERecordingMode) {
			case eOIERecordingMode::OIEEnableAndContinuousMeasurement:
			case eOIERecordingMode::OIEEnableAndLaserActiveMeasurement:
				pRTCContext->DisableOIE();
				break;
			}


			pRTCContext->SetEndOfList();
			pRTCContext->ExecuteList(1, 0);
		}


		// Wait For 
		auto pDriverUpdateInstance = m_pDriverEnvironment->CreateStatusUpdateSession();

		bool AnyScannerIsBusy = true;
		while (AnyScannerIsBusy) {

			AnyScannerIsBusy = false;
			for (uint32_t nScannerIndex = 1; nScannerIndex <= m_nScannerCount; nScannerIndex++) {
				uint32_t Pos = 0;
				auto pRTCContext = getRTCContextForScannerIndex(nScannerIndex, true);

				bool Busy = true;
				pRTCContext->GetStatus(Busy, Pos);

				AnyScannerIsBusy |= Busy;
			}

			pDriverUpdateInstance->Sleep(10);

			updateCardStatus(pDriverUpdateInstance);
		}

	}
}


void CDriver_ScanLab_RTC6xN::SetCommunicationTimeouts(const LibMCDriver_ScanLab_uint32 nScannerIndex, const LibMCDriver_ScanLab_double dInitialTimeout, const LibMCDriver_ScanLab_double dMaxTimeout, const LibMCDriver_ScanLab_double dMultiplier)
{
	if (!m_SimulationMode) {
		auto pRTCContext = getRTCContextForScannerIndex(nScannerIndex, true);

		pRTCContext->SetCommunicationTimeouts(dInitialTimeout, dMaxTimeout, dMultiplier);
	}
}

void CDriver_ScanLab_RTC6xN::GetCommunicationTimeouts(const LibMCDriver_ScanLab_uint32 nScannerIndex, LibMCDriver_ScanLab_double & dInitialTimeout, LibMCDriver_ScanLab_double & dMaxTimeout, LibMCDriver_ScanLab_double & dMultiplier)
{
	if (!m_SimulationMode) {
		auto pRTCContext = getRTCContextForScannerIndex(nScannerIndex, true);

		pRTCContext->GetCommunicationTimeouts(dInitialTimeout, dMaxTimeout, dMultiplier);

	}
	else {
		dInitialTimeout = 0.75;
		dMaxTimeout = 20.0;
		dMultiplier = 1.3;
	}
}


void CDriver_ScanLab_RTC6xN::updateCardStatus(LibMCEnv::PDriverStatusUpdateSession pDriverUpdateInstance)
{
	if (pDriverUpdateInstance.get() == nullptr)
		return;

	if (!m_SimulationMode) {

		for (uint32_t nIndex = 1; nIndex <= m_nScannerCount; nIndex++) {

			if (ScannerIsInitialized(nIndex)) {

				std::string sPrefix = "scanner" + std::to_string(nIndex) + "_";
				auto pRTCContext = getRTCContextForScannerIndex(nIndex, true);

				if (pRTCContext.get() == nullptr)
					throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CARDNOTINITIALIZED);

				bool Busy = true;
				uint32_t ListPosition = 0;
				bool bPositionXisOK, bPositionYisOK, bTemperatureisOK, bPowerisOK;

				pRTCContext->GetStatus(Busy, ListPosition);
				pRTCContext->GetHeadStatus(1, bPositionXisOK, bPositionYisOK, bTemperatureisOK, bPowerisOK);

				pDriverUpdateInstance->SetBoolParameter(sPrefix + "position_x_ok", bPositionXisOK);
				pDriverUpdateInstance->SetBoolParameter(sPrefix + "position_y_ok", bPositionYisOK);
				pDriverUpdateInstance->SetBoolParameter(sPrefix + "temperature_ok", bTemperatureisOK);
				pDriverUpdateInstance->SetBoolParameter(sPrefix + "power_ok", bPowerisOK);

				pDriverUpdateInstance->SetIntegerParameter(sPrefix + "list_position", ListPosition);
				pDriverUpdateInstance->SetBoolParameter(sPrefix + "card_busy", Busy);

				bool bLaserIsOn;
				int32_t nPositionX, nPositionY, nPositionZ;
				int32_t nCorrectedPositionX, nCorrectedPositionY, nCorrectedPositionZ;
				int32_t nFocusShift, nMarkSpeed;
				int32_t nEncoderPositionX, nEncoderPositionY;

				pRTCContext->GetStateValues(bLaserIsOn, nPositionX, nPositionY, nPositionZ, nCorrectedPositionX, nCorrectedPositionY, nCorrectedPositionZ, nFocusShift, nMarkSpeed);
				pRTCContext->Get2DMarkOnTheFlyPosition(nEncoderPositionX, nEncoderPositionY);

				pDriverUpdateInstance->SetBoolParameter(sPrefix + "laser_on", bLaserIsOn);
				pDriverUpdateInstance->SetIntegerParameter(sPrefix + "position_x", nPositionX);
				pDriverUpdateInstance->SetIntegerParameter(sPrefix + "position_y", nPositionY);
				pDriverUpdateInstance->SetIntegerParameter(sPrefix + "position_z", nPositionZ);
				pDriverUpdateInstance->SetIntegerParameter(sPrefix + "position_x_corrected", nCorrectedPositionX);
				pDriverUpdateInstance->SetIntegerParameter(sPrefix + "position_y_corrected", nCorrectedPositionY);
				pDriverUpdateInstance->SetIntegerParameter(sPrefix + "position_z_corrected", nCorrectedPositionZ);
				pDriverUpdateInstance->SetIntegerParameter(sPrefix + "focus_shift", nFocusShift);
				pDriverUpdateInstance->SetIntegerParameter(sPrefix + "mark_speed", nMarkSpeed);
				pDriverUpdateInstance->SetIntegerParameter(sPrefix + "encoder_position_x", nEncoderPositionX);
				pDriverUpdateInstance->SetIntegerParameter(sPrefix + "encoder_position_y", nEncoderPositionY);

			}

		}

	}

}

act_managed_ptr<IRTCContext> CDriver_ScanLab_RTC6xN::getRTCContextForScannerIndex(uint32_t nScannerIndex, bool bFailIfNotExisting)
{
	auto iIter = m_ScannerContexts.find(nScannerIndex);
	if (iIter == m_ScannerContexts.end()) {
		if (bFailIfNotExisting)
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDSCANNERINDEX, "invalid scanner index: " + std::to_string(nScannerIndex));

		return nullptr;
	}

	return iIter->second;
}

act_managed_ptr<IRTCContext> CDriver_ScanLab_RTC6xN::getRTCContextForLaserIndex(uint32_t nLaserIndex, bool bFailIfNotExisting)
{
	auto iIter = m_LaserIndexMapping.find(nLaserIndex);
	if (iIter == m_LaserIndexMapping.end()) {
		if (bFailIfNotExisting)
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_LASERINDEXNOTFOUND, "laser index not found: " + std::to_string(nLaserIndex));

		return nullptr;
	}

	return iIter->second;

}


void CDriver_ScanLab_RTC6xN::EnableTimelagCompensation(const LibMCDriver_ScanLab_uint32 nScannerIndex, const LibMCDriver_ScanLab_uint32 nTimeLagXYInMicroseconds, const LibMCDriver_ScanLab_uint32 nTimeLagZInMicroseconds)
{
	auto iIter = m_ScannerContexts.find(nScannerIndex);
	if (iIter == m_ScannerContexts.end()) 
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDSCANNERINDEX, "invalid scanner index: " + std::to_string(nScannerIndex));

	iIter->second->EnableTimelagCompensation(nTimeLagXYInMicroseconds, nTimeLagZInMicroseconds);

}

void CDriver_ScanLab_RTC6xN::DisableTimelagCompensation(const LibMCDriver_ScanLab_uint32 nScannerIndex)
{
	auto iIter = m_ScannerContexts.find(nScannerIndex);
	if (iIter == m_ScannerContexts.end())
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDSCANNERINDEX, "invalid scanner index: " + std::to_string(nScannerIndex));

	iIter->second->DisableTimelagCompensation();

}


PDriver_ScanLab_RTC6ConfigurationPreset CDriver_ScanLab_RTC6xN::findPresetByName(const std::string& sPresetName, bool bMustExist)
{
	auto iIterator = m_ConfigurationPresets.find(sPresetName);
	if (iIterator != m_ConfigurationPresets.end()) {
		return iIterator->second;
	}
	else {
		if (bMustExist)
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CONFIGURATIONPRESETNOTFOUND, sPresetName);

		return nullptr;
	}
}
