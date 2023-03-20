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

// Include custom headers here.
#define RTC6_MINLASERCOUNT 2
#define RTC6_MAXLASERCOUNT 64

using namespace LibMCDriver_ScanLab::Impl;

/*************************************************************************************************************************
 Class definition of CDriver_ScanLab_RTC6xN
**************************************************************************************************************************/

CDriver_ScanLab_RTC6xN::CDriver_ScanLab_RTC6xN(const std::string& sName, const std::string& sType, uint32_t nScannerCount, LibMCEnv::PDriverEnvironment pDriverEnvironment)
	: CDriver_ScanLab(pDriverEnvironment), m_sName(sName), m_sType(sType), m_fMaxLaserPowerInWatts(0.0f), m_SimulationMode(false), m_nScannerCount(nScannerCount)
{
	if ((nScannerCount < RTC6_MINLASERCOUNT) || (nScannerCount > RTC6_MAXLASERCOUNT))
		throw ELibMCDriver_ScanLabInterfaceException (LIBMCDRIVER_SCANLAB_ERROR_INVALIDSCANNERCOUNT);


}

CDriver_ScanLab_RTC6xN::~CDriver_ScanLab_RTC6xN()
{

}

void CDriver_ScanLab_RTC6xN::Configure(const std::string& sConfigurationString)
{
	for (uint32_t nIndex = 1; nIndex <= m_nScannerCount; nIndex++) {

		std::string sPrefix = "scanner" + std::to_string(nIndex) + "_";

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
	auto iIter = m_pScannerContexts.find(nScannerIndex);
	return (iIter != m_pScannerContexts.end());

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
		m_pDriverEnvironment->SetIntegerParameter(sPrefix + "rtc_version", 1);
		m_pDriverEnvironment->SetIntegerParameter(sPrefix + "card_type", 1);
		m_pDriverEnvironment->SetIntegerParameter(sPrefix + "dll_version", 1);
		m_pDriverEnvironment->SetIntegerParameter(sPrefix + "hex_version", 1);
		m_pDriverEnvironment->SetIntegerParameter(sPrefix + "bios_version", 1);
		m_pDriverEnvironment->SetIntegerParameter(sPrefix + "serialnumber", (uint32_t) (123450ULL + nScannerIndex));

	}
	else {

		if (m_pRTCSelector.get() == nullptr)
			m_pRTCSelector = act_managed_ptr<IRTCSelector>(CreateRTCSelector());

		m_pScannerContexts.erase(nScannerIndex);

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
		m_pDriverEnvironment->SetIntegerParameter(sPrefix + "rtc_version", nRTCVersion);
		m_pDriverEnvironment->SetIntegerParameter(sPrefix + "card_type", nRTCType);
		m_pDriverEnvironment->SetIntegerParameter(sPrefix + "dll_version", nDLLVersion);
		m_pDriverEnvironment->SetIntegerParameter(sPrefix + "hex_version", nHEXVersion);
		m_pDriverEnvironment->SetIntegerParameter(sPrefix + "bios_version", nBIOSVersion);
		m_pDriverEnvironment->SetIntegerParameter(sPrefix + "serialnumber", pRTCContext->GetSerialNumber());

		m_pScannerContexts.insert(std::make_pair (nScannerIndex, pRTCContext));
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
	auto pRTCContext = getRTCContext(nScannerIndex);

	return pRTCContext->GetIPAddress();
}

std::string CDriver_ScanLab_RTC6xN::GetNetmask(const LibMCDriver_ScanLab_uint32 nScannerIndex)
{
	auto pRTCContext = getRTCContext(nScannerIndex);

	return pRTCContext->GetNetmask();
}

LibMCDriver_ScanLab_uint32 CDriver_ScanLab_RTC6xN::GetSerialNumber(const LibMCDriver_ScanLab_uint32 nScannerIndex)
{
	auto pRTCContext = getRTCContext(nScannerIndex);

	return pRTCContext->GetSerialNumber();
}

LibMCDriver_ScanLab_uint32 CDriver_ScanLab_RTC6xN::GetLaserIndex(const LibMCDriver_ScanLab_uint32 nScannerIndex)
{
	throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_NOTIMPLEMENTED);
}

IRTCContext * CDriver_ScanLab_RTC6xN::GetContext(const LibMCDriver_ScanLab_uint32 nScannerIndex)
{
	throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_NOTIMPLEMENTED);
}

void CDriver_ScanLab_RTC6xN::LoadFirmware(const LibMCDriver_ScanLab_uint32 nScannerIndex, const std::string & sFirmwareResource, const std::string & sFPGAResource, const std::string & sAuxiliaryResource)
{
	throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_NOTIMPLEMENTED);
}

void CDriver_ScanLab_RTC6xN::LoadCustomFirmware(const LibMCDriver_ScanLab_uint32 nScannerIndex, const LibMCDriver_ScanLab_uint64 nFirmwareDataBufferSize, const LibMCDriver_ScanLab_uint8 * pFirmwareDataBuffer, const LibMCDriver_ScanLab_uint64 nFPGADataBufferSize, const LibMCDriver_ScanLab_uint8 * pFPGADataBuffer, const LibMCDriver_ScanLab_uint64 nAuxiliaryDataBufferSize, const LibMCDriver_ScanLab_uint8 * pAuxiliaryDataBuffer)
{
	throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_NOTIMPLEMENTED);
}

void CDriver_ScanLab_RTC6xN::SetCorrectionFile(const LibMCDriver_ScanLab_uint32 nScannerIndex, const LibMCDriver_ScanLab_uint64 nCorrectionFileBufferSize, const LibMCDriver_ScanLab_uint8 * pCorrectionFileBuffer, const LibMCDriver_ScanLab_uint32 nTableNumber, const LibMCDriver_ScanLab_uint32 nDimension, const LibMCDriver_ScanLab_uint32 nTableNumberHeadA, const LibMCDriver_ScanLab_uint32 nTableNumberHeadB)
{
	throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_NOTIMPLEMENTED);
}

void CDriver_ScanLab_RTC6xN::ConfigureLaserMode(const LibMCDriver_ScanLab_uint32 nScannerIndex, const LibMCDriver_ScanLab::eLaserMode eLaserMode, const LibMCDriver_ScanLab::eLaserPort eLaserPort, const LibMCDriver_ScanLab_double dMaxLaserPower, const bool bFinishLaserPulseAfterOn, const bool bPhaseShiftOfLaserSignal, const bool bLaserOnSignalLowActive, const bool bLaserHalfSignalsLowActive, const bool bSetDigitalInOneHighActive, const bool bOutputSynchronizationActive)
{
	throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_NOTIMPLEMENTED);
}

void CDriver_ScanLab_RTC6xN::ConfigureDelays(const LibMCDriver_ScanLab_uint32 nScannerIndex, const LibMCDriver_ScanLab_double dLaserOnDelay, const LibMCDriver_ScanLab_double dLaserOffDelay, const LibMCDriver_ScanLab_double dMarkDelay, const LibMCDriver_ScanLab_double dJumpDelay, const LibMCDriver_ScanLab_double dPolygonDelay)
{
	throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_NOTIMPLEMENTED);
}

void CDriver_ScanLab_RTC6xN::DrawLayer(const std::string & sStreamUUID, const LibMCDriver_ScanLab_uint32 nLayerIndex, const bool bFailIfNonAssignedDataExists)
{
	throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_NOTIMPLEMENTED);
}

void CDriver_ScanLab_RTC6xN::SetCommunicationTimeouts(const LibMCDriver_ScanLab_uint32 nScannerIndex, const LibMCDriver_ScanLab_double dInitialTimeout, const LibMCDriver_ScanLab_double dMaxTimeout, const LibMCDriver_ScanLab_double dMultiplier)
{
	throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_NOTIMPLEMENTED);
}

void CDriver_ScanLab_RTC6xN::GetCommunicationTimeouts(const LibMCDriver_ScanLab_uint32 nScannerIndex, LibMCDriver_ScanLab_double & dInitialTimeout, LibMCDriver_ScanLab_double & dMaxTimeout, LibMCDriver_ScanLab_double & dMultiplier)
{
	throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_NOTIMPLEMENTED);
}


void CDriver_ScanLab_RTC6xN::updateCardStatus(LibMCEnv::PDriverStatusUpdateSession pDriverUpdateInstance)
{
	if (pDriverUpdateInstance.get() == nullptr)
		return;

	if (!m_SimulationMode) {

		for (uint32_t nIndex = 1; nIndex <= m_nScannerCount; nIndex++) {

			if (ScannerIsInitialized(nIndex)) {

				std::string sPrefix = "scanner" + std::to_string(nIndex) + "_";
				auto pRTCContext = getRTCContext(nIndex);

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

				pRTCContext->GetStateValues(bLaserIsOn, nPositionX, nPositionY, nPositionZ, nCorrectedPositionX, nCorrectedPositionY, nCorrectedPositionZ, nFocusShift, nMarkSpeed);
				pDriverUpdateInstance->SetBoolParameter(sPrefix + "laser_on", bLaserIsOn);
				pDriverUpdateInstance->SetIntegerParameter(sPrefix + "position_x", nPositionX);
				pDriverUpdateInstance->SetIntegerParameter(sPrefix + "position_y", nPositionY);
				pDriverUpdateInstance->SetIntegerParameter(sPrefix + "position_z", nPositionZ);
				pDriverUpdateInstance->SetIntegerParameter(sPrefix + "position_x_corrected", nCorrectedPositionX);
				pDriverUpdateInstance->SetIntegerParameter(sPrefix + "position_y_corrected", nCorrectedPositionY);
				pDriverUpdateInstance->SetIntegerParameter(sPrefix + "position_z_corrected", nCorrectedPositionZ);
				pDriverUpdateInstance->SetIntegerParameter(sPrefix + "focus_shift", nFocusShift);
				pDriverUpdateInstance->SetIntegerParameter(sPrefix + "mark_speed", nMarkSpeed);
			}

		}

	}

}

act_managed_ptr<IRTCContext> CDriver_ScanLab_RTC6xN::getRTCContext(uint32_t nScannerIndex)
{
	auto iIter = m_pScannerContexts.find(nScannerIndex);
	if (iIter == m_pScannerContexts.end ())
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDSCANNERINDEX, "invalid scanner index: " + std::to_string (nScannerIndex));

	return iIter->second;
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
