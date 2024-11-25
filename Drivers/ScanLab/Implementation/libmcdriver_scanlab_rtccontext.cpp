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

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL AUTODESK INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "libmcdriver_scanlab_rtccontext.hpp"
#include "libmcdriver_scanlab_interfaceexception.hpp"
#include "libmcdriver_scanlab_uartconnection.hpp"
#include "libmcdriver_scanlab_rtcrecording.hpp"

// Include custom headers here.
#include <math.h>
#include <iostream>
#include <thread>
#include <fstream>
#include <algorithm>

#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <string>
#include <array>

using namespace LibMCDriver_ScanLab::Impl;

#define RTCCONTEXT_LASERPOWERCALIBRATIONUNITS 0.005


#define RTCCONTEXT_MIN_LINESUBDIVISIONTHRESHOLD 0.001
#define RTCCONTEXT_MAX_LINESUBDIVISIONTHRESHOLD 1000000.0

CRTCContextOwnerData::CRTCContextOwnerData()
	: m_nAttributeFilterValue (0), m_OIERecordingMode (LibMCDriver_ScanLab::eOIERecordingMode::OIERecordingDisabled), m_dMaxLaserPowerInWatts (100.0)
{

}

CRTCContextOwnerData::~CRTCContextOwnerData()
{

}

void CRTCContextOwnerData::getAttributeFilters(std::string& sAttributeFilterNameSpace, std::string& sAttributeFilterName, int64_t& nAttributeFilterValue)
{
	sAttributeFilterNameSpace = m_sAttributeFilterNameSpace;
	sAttributeFilterName = m_sAttributeFilterName;
	nAttributeFilterValue = m_nAttributeFilterValue;
}

void CRTCContextOwnerData::setAttributeFilters(const std::string& sAttributeFilterNameSpace, const std::string& sAttributeFilterName, const int64_t nAttributeFilterValue)
{
	m_sAttributeFilterNameSpace = sAttributeFilterNameSpace;
	m_sAttributeFilterName = sAttributeFilterName;
	m_nAttributeFilterValue = nAttributeFilterValue;
}

void CRTCContextOwnerData::setMaxLaserPower(double dMaxLaserPowerInWatts)
{
	m_dMaxLaserPowerInWatts = dMaxLaserPowerInWatts;
}

double CRTCContextOwnerData::getMaxLaserPower()
{
	return m_dMaxLaserPowerInWatts;
}

void CRTCContextOwnerData::setOIERecordingMode(LibMCDriver_ScanLab::eOIERecordingMode oieRecordingMode)
{
	m_OIERecordingMode = oieRecordingMode;
}

LibMCDriver_ScanLab::eOIERecordingMode CRTCContextOwnerData::getOIERecordingMode()
{
	return m_OIERecordingMode;
}

PScanLabSDK CRTCContextOwnerData::getScanLabSDK()
{
	if (m_pScanlabSDK.get() == nullptr)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_SCANLABSDKNOTLOADED);

	return m_pScanlabSDK;
}

void CRTCContextOwnerData::setScanLabSDK(PScanLabSDK pScanLabSDK)
{
	m_pScanlabSDK = pScanLabSDK;
}


/*************************************************************************************************************************
 Class definition of CRTCContext 
**************************************************************************************************************************/

CRTCContext::CRTCContext(PRTCContextOwnerData pOwnerData, uint32_t nCardNo, bool bIsNetwork, LibMCEnv::PDriverEnvironment pDriverEnvironment)
	: m_pOwnerData(pOwnerData),
	m_CardNo (nCardNo), 
	m_dCorrectionFactor(10000.0), 
	m_dZCorrectionFactor(10000.0),
	m_nLaserIndex (0),
	m_LaserPort(eLaserPort::Port12BitAnalog1), 
	m_pDriverEnvironment (pDriverEnvironment),
	m_OIEOperationMode (LibMCDriver_ScanLab::eOIEOperationMode::OIENotInitialized),
	m_bIsNetwork (bIsNetwork),
	m_nCurrentFreeVariable0 (0),
	m_dLaserOriginX (0.0),
	m_dLaserOriginY (0.0),
	m_dLaserFieldMinX (0.0),
	m_dLaserFieldMinY (0.0),
	m_dLaserFieldMaxX (0.0),
	m_dLaserFieldMaxY (0.0),
	m_nCurrentScanPositionX (0),
	m_nCurrentScanPositionY (0),
	m_bHasLaserField (false),
	m_b2DMarkOnTheFlyEnabled (false),
	m_dScaleXInBitsPerEncoderStep (1.0),
	m_dScaleYInBitsPerEncoderStep (1.0),
	m_bEnableOIEPIDControl (false),
	m_dLaserPowerCalibrationUnits (RTCCONTEXT_LASERPOWERCALIBRATIONUNITS),
	m_pModulationCallback (nullptr),
	m_pModulationCallbackUserData (nullptr),
	m_bEnableLineSubdivision (false),
	m_bMeasurementTagging (false),
	m_dLineSubdivisionThreshold (RTCCONTEXT_MAX_LINESUBDIVISIONTHRESHOLD)

{
	if (pOwnerData.get() == nullptr)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);
	m_pScanLabSDK = pOwnerData->getScanLabSDK();
	if (m_pScanLabSDK.get() == nullptr)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);

	if (pDriverEnvironment.get () == nullptr)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);
	if (nCardNo == 0)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);

	m_pScanLabSDK->n_reset_error(m_CardNo, 0xffffffff);

	m_CurrentMeasurementTagInfo.m_nCurrentPartID = 0;
	m_CurrentMeasurementTagInfo.m_nCurrentProfileID = 0;
	m_CurrentMeasurementTagInfo.m_nCurrentSegmentID = 0;
	m_CurrentMeasurementTagInfo.m_nCurrentVectorID = 0;

}

CRTCContext::~CRTCContext()
{
	if (m_pScanLabSDK.get() != nullptr) {
		m_pScanLabSDK->release_rtc(m_CardNo);
		m_CardNo = 0;
	}
}

void CRTCContext::loadFirmwareEx(PScanLabSDK pSDK, uint32_t nCardNo, const LibMCDriver_ScanLab_uint64 nFirmwareDataBufferSize, const LibMCDriver_ScanLab_uint8* pFirmwareDataBuffer, const LibMCDriver_ScanLab_uint64 nFPGADataBufferSize, const LibMCDriver_ScanLab_uint8* pFPGADataBuffer, const LibMCDriver_ScanLab_uint64 nAuxiliaryDataBufferSize, const LibMCDriver_ScanLab_uint8* pAuxiliaryDataBuffer, bool bIsNetwork, LibMCEnv::PDriverEnvironment pDriverEnvironment)
{
	if (pSDK.get() == nullptr)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);
	if (pDriverEnvironment.get() == nullptr)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);

	auto pWorkingDirectory = pDriverEnvironment->CreateWorkingDirectory();

	std::string sFirmwareName;
	if (bIsNetwork) {
		sFirmwareName = "RTC6ETH.out";
	}
	else {
		sFirmwareName = "RTC6OUT.out";
	}

	auto pFirmwareFile = pWorkingDirectory->StoreCustomData(sFirmwareName, LibMCEnv::CInputVector<uint8_t>(pFirmwareDataBuffer, nFirmwareDataBufferSize));
	auto pFPGAFile = pWorkingDirectory->StoreCustomData("RTC6RBF.rbf", LibMCEnv::CInputVector<uint8_t>(pFPGADataBuffer, nFPGADataBufferSize));
	auto pAuxiliaryFile = pWorkingDirectory->StoreCustomData("RTC6DAT.dat", LibMCEnv::CInputVector<uint8_t>(pAuxiliaryDataBuffer, nAuxiliaryDataBufferSize));

	// TODO: Convert to ANSI
	uint32_t nErrorCode = pSDK->n_load_program_file(nCardNo, pWorkingDirectory->GetAbsoluteFilePath().c_str());

	pFirmwareFile = nullptr;
	pFPGAFile = nullptr;
	pAuxiliaryFile = nullptr;

	pWorkingDirectory->CleanUp();

	if (nErrorCode != 0)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_COULDNOTLOADPROGRAMFILE, "could not load program file: #" + std::to_string(nErrorCode));


}

void CRTCContext::setCommunicationTimeoutsEx(PScanLabSDK pSDK, uint32_t nCardNo, const LibMCDriver_ScanLab_double dInitialTimeout, const LibMCDriver_ScanLab_double dMaxTimeout, const LibMCDriver_ScanLab_double dMultiplier)
{
	if (pSDK.get() == nullptr)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);

	double dOldInitialTimeout = 0.0;
	double dOldMaxTimeout = 0.0;
	double dOldMultiplier = 0.0;
	uint32_t nOldMode = 0;

	// Turn on high performance mode...
	pSDK->n_eth_set_high_performance_mode(nCardNo, 1);

	// Set Timeouts, but keep old mode
	pSDK->n_eth_get_com_timeouts_auto(nCardNo, &dOldInitialTimeout, &dOldMaxTimeout, &dOldMultiplier, &nOldMode);
	pSDK->n_eth_set_com_timeouts_auto(nCardNo, dInitialTimeout, dMaxTimeout, dMultiplier, nOldMode);
}


std::string CRTCContext::GetIPAddress()
{
	return m_sIPAddress;
}

std::string CRTCContext::GetNetmask()
{
	return m_sNetmask;
}

void CRTCContext::setIPAddress(const std::string& sIPAddress, const std::string& sNetmask)
{
	m_sIPAddress = sIPAddress;
	m_sNetmask = sNetmask;
}

// setLaserIndex should not be exposed via API
void CRTCContext::setLaserIndex(const uint32_t nLaserIndex)
{
	m_nLaserIndex = nLaserIndex;
}


void CRTCContext::LoadFirmware(const LibMCDriver_ScanLab_uint64 nFirmwareDataBufferSize, const LibMCDriver_ScanLab_uint8* pFirmwareDataBuffer, const LibMCDriver_ScanLab_uint64 nFPGADataBufferSize, const LibMCDriver_ScanLab_uint8* pFPGADataBuffer, const LibMCDriver_ScanLab_uint64 nAuxiliaryDataBufferSize, const LibMCDriver_ScanLab_uint8* pAuxiliaryDataBuffer)
{
	loadFirmwareEx(m_pScanLabSDK, m_CardNo, nFirmwareDataBufferSize, pFirmwareDataBuffer, nFPGADataBufferSize, pFPGADataBuffer, nAuxiliaryDataBufferSize, pAuxiliaryDataBuffer, m_bIsNetwork, m_pDriverEnvironment);
}

void CRTCContext::LoadCorrectionFile(const LibMCDriver_ScanLab_uint64 nCorrectionFileBufferSize, const LibMCDriver_ScanLab_uint8* pCorrectionFileBuffer, const LibMCDriver_ScanLab_uint32 nTableNumber, const LibMCDriver_ScanLab_uint32 nDimension)
{

	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);

	auto pWorkingDirectory = m_pDriverEnvironment->CreateWorkingDirectory();

	auto pCorrectionFile = pWorkingDirectory->StoreCustomData("cor.ct5", LibMCEnv::CInputVector<uint8_t>(pCorrectionFileBuffer, nCorrectionFileBufferSize));

	// TODO: Convert to ANSI
	uint32_t nErrorCode = m_pScanLabSDK->n_load_correction_file(m_CardNo, pCorrectionFile->GetAbsoluteFileName ().c_str(), nTableNumber, nDimension);

	pCorrectionFile = nullptr;
	pWorkingDirectory->CleanUp();

	if (nErrorCode != 0)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_COULDNOTLOADCORRECTIONFILE, "Could not load correction file #" + std::to_string (nErrorCode));

	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);

}


void CRTCContext::SelectCorrectionTable(const LibMCDriver_ScanLab_uint32 nTableNumberHeadA, const LibMCDriver_ScanLab_uint32 nTableNumberHeadB)
{
	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);

	m_pScanLabSDK->n_select_cor_table(m_CardNo, nTableNumberHeadA, nTableNumberHeadB);
	m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));

	double dCorrectionFactor = m_pScanLabSDK->n_get_table_para(m_CardNo, nTableNumberHeadA, 1);
	m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));
	if (dCorrectionFactor > 0.0) {
		// For RTC6 both correction factors are equal, for RTC 5, there is a factor of 16
		m_dCorrectionFactor = dCorrectionFactor;
		m_dZCorrectionFactor = dCorrectionFactor;
	}

	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);

}

void CRTCContext::ConfigureLists(const LibMCDriver_ScanLab_uint32 nSizeListA, const LibMCDriver_ScanLab_uint32 nSizeListB)
{
	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);

	m_pScanLabSDK->n_config_list(m_CardNo, nSizeListA, nSizeListB);
	m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));

	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);
}

void CRTCContext::SetLaserMode(const LibMCDriver_ScanLab::eLaserMode eLaserMode, const LibMCDriver_ScanLab::eLaserPort eLaserPort)
{
	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);

	m_pScanLabSDK->n_set_laser_mode(m_CardNo, (uint32_t)eLaserMode);
	m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));

	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);

	m_LaserPort = eLaserPort;
}

void CRTCContext::DisableAutoLaserControl()
{
	m_pScanLabSDK->n_set_auto_laser_control(m_CardNo, 0, 0, 0, 0, 0);
	m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));

}

void CRTCContext::SetLaserControlParameters(const bool DisableLaser, const bool bFinishLaserPulseAfterOn, const bool bPhaseShiftOfLaserSignal, const bool bLaserOnSignalLowActive, const bool bLaserHalfSignalsLowActive, const bool bSetDigitalInOneHighActive, const bool bOutputSynchronizationActive)
{
	uint32_t bitmode = 0;
	if (bFinishLaserPulseAfterOn)
		bitmode |= (1UL << 0);
	if (bPhaseShiftOfLaserSignal)
		bitmode |= (1UL << 1);
	if (DisableLaser)
		bitmode |= (1UL << 2);
	if (bLaserOnSignalLowActive)
		bitmode |= (1UL << 3);
	if (bLaserHalfSignalsLowActive)
		bitmode |= (1UL << 4);
	if (bSetDigitalInOneHighActive)
		bitmode |= (1UL << 5);
	if (bOutputSynchronizationActive)
		bitmode |= (1UL << 6);

	m_pScanLabSDK->n_set_laser_control(m_CardNo, bitmode);
	m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));
}

void CRTCContext::SetLaserPulsesInBits(const LibMCDriver_ScanLab_uint32 nHalfPeriod, const LibMCDriver_ScanLab_uint32 nPulseLength)
{
	m_pScanLabSDK->n_set_laser_pulses_ctrl(m_CardNo, nHalfPeriod, nPulseLength);
	m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));
}

void CRTCContext::SetLaserPulsesInMicroSeconds(const LibMCDriver_ScanLab_double dHalfPeriod, const LibMCDriver_ScanLab_double dPulseLength)
{
	double HalfPeriodBits = round(dHalfPeriod * 64.0);
	double PulseLengthBits = round(dPulseLength * 64.0);

	if ((HalfPeriodBits < 1) || (HalfPeriodBits >= (double)(1UL << 31)))
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);
	if ((PulseLengthBits < 1) || (PulseLengthBits >= (double)(1UL << 31)))
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);
	if (PulseLengthBits > 2 * HalfPeriodBits)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);

	SetLaserPulsesInBits((uint32_t)HalfPeriodBits, (uint32_t)PulseLengthBits);
}

void CRTCContext::SetStandbyInBits(const LibMCDriver_ScanLab_uint32 nHalfPeriod, const LibMCDriver_ScanLab_uint32 nPulseLength)
{
	m_pScanLabSDK->n_set_standby(m_CardNo, nHalfPeriod, nPulseLength);
	m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));
}

void CRTCContext::SetStandbyInMicroSeconds(const LibMCDriver_ScanLab_double dHalfPeriod, const LibMCDriver_ScanLab_double dPulseLength)
{
	double HalfPeriodBits = round(dHalfPeriod * 64.0);
	double PulseLengthBits = round(dPulseLength * 64.0);

	if ((HalfPeriodBits < 1) || (HalfPeriodBits >= (double)(1UL << 31)))
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);
	if ((PulseLengthBits < 1) || (PulseLengthBits >= (double)(1UL << 31)))
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);
	if (PulseLengthBits > 2 * HalfPeriodBits)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);

	SetStandbyInBits((uint32_t)HalfPeriodBits, (uint32_t)PulseLengthBits);
}

LibMCDriver_ScanLab_uint32 CRTCContext::GetSerialNumber()
{
	uint32_t serialNo = m_pScanLabSDK->n_get_serial_number(m_CardNo);
	return serialNo;
}

LibMCDriver_ScanLab_uint32 CRTCContext::GetLaserIndex()
{
	return m_nLaserIndex;
}

void CRTCContext::SetStartList(const LibMCDriver_ScanLab_uint32 nListIndex, const LibMCDriver_ScanLab_uint32 nPosition)
{
	m_pScanLabSDK->n_set_start_list_pos(m_CardNo, nListIndex, nPosition);
	m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));

	m_CurrentMeasurementTagInfo.m_nCurrentPartID = 0;
	m_CurrentMeasurementTagInfo.m_nCurrentProfileID = 0;
	m_CurrentMeasurementTagInfo.m_nCurrentSegmentID = 0;
	m_CurrentMeasurementTagInfo.m_nCurrentVectorID = 0;
}

void CRTCContext::SetEndOfList()
{
	m_pScanLabSDK->n_set_end_of_list(m_CardNo);
	m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));
}

void CRTCContext::ExecuteList(const LibMCDriver_ScanLab_uint32 nListIndex, const LibMCDriver_ScanLab_uint32 nPosition)
{
	m_pScanLabSDK->n_execute_list_pos(m_CardNo, nListIndex, nPosition);
	m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));
}

void CRTCContext::SetAutoChangePos(const LibMCDriver_ScanLab_uint32 nPosition)
{
	m_pScanLabSDK->n_auto_change_pos(m_CardNo, nPosition);
	m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));
}

void CRTCContext::SetDelays(const LibMCDriver_ScanLab_uint32 nMarkDelay, const LibMCDriver_ScanLab_uint32 nJumpDelay, const LibMCDriver_ScanLab_uint32 nPolygonDelay)
{
	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);
	if ((nMarkDelay % 10) != 0)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_DELAYSMUSTBEMULTIPLEOF10);
	if ((nJumpDelay % 10) != 0)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_DELAYSMUSTBEMULTIPLEOF10);
	if ((nPolygonDelay % 10) != 0)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_DELAYSMUSTBEMULTIPLEOF10);

	uint32_t MarkInBits = nMarkDelay / 10;
	uint32_t JumpInBits = nJumpDelay / 10;
	uint32_t PolygonInBits = nPolygonDelay / 10;

	m_pScanLabSDK->n_set_scanner_delays(m_CardNo, JumpInBits, MarkInBits, PolygonInBits);
	m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));

	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);

}

void CRTCContext::SetLaserDelaysInMicroseconds(const LibMCDriver_ScanLab_double dLaserOnDelay, const LibMCDriver_ScanLab_double dLaserOffDelay)
{
	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);

	double LaserOnDelayBits = round(dLaserOnDelay * 64.0);
	double LaserOffDelayBits = round(dLaserOffDelay * 64.0);

	if ((LaserOnDelayBits <= - (double)(1LL << 31)) || (LaserOnDelayBits >= (double)(1LL << 31)))
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDLASERDELAY);
	if ((LaserOffDelayBits <= - (double)(1LL << 31)) || (LaserOffDelayBits >= (double)(1LL << 31)))
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDLASERDELAY);

	SetLaserDelaysInBits((int32_t)LaserOnDelayBits, (int32_t)LaserOffDelayBits);

	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);

}

void CRTCContext::SetLaserDelaysInBits(const LibMCDriver_ScanLab_int32 nLaserOnDelay, const LibMCDriver_ScanLab_int32 nLaserOffDelay)
{
	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);

	m_pScanLabSDK->n_set_laser_delays(m_CardNo, nLaserOnDelay, nLaserOffDelay);
	m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));

	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);

}


void CRTCContext::writeJumpSpeed(float jumpSpeedinMMPerSecond)
{
	double dBitsPerMM = m_dCorrectionFactor;

	double dJumpSpeedInMMPerMilliSecond = (double)jumpSpeedinMMPerSecond / 1000.0;
	double dJumpSpeedInBits = dJumpSpeedInMMPerMilliSecond * dBitsPerMM;
	m_pScanLabSDK->n_set_jump_speed(m_CardNo, dJumpSpeedInBits);
	m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));
}

void CRTCContext::writeMarkSpeed(float markSpeedinMMPerSecond)
{
	double dBitsPerMM = m_dCorrectionFactor;

	double dMarkSpeedInMMPerMilliSecond = (double)markSpeedinMMPerSecond / 1000.0;
	double dMarkSpeedInBits = dMarkSpeedInMMPerMilliSecond * dBitsPerMM;
	m_pScanLabSDK->n_set_mark_speed(m_CardNo, dMarkSpeedInBits);
	m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));
}

void CRTCContext::writePower(double dPowerInPercent, bool bOIEPIDControlFlag)
{

	double dAdjustedPowerInPercent = dPowerInPercent;
	if (m_LaserPowerCalibrationList.size() == 1) {
		auto& calibration = m_LaserPowerCalibrationList.at (0);
		dAdjustedPowerInPercent = adjustLaserPowerCalibration(dPowerInPercent, calibration.m_PowerOffsetInPercent, calibration.m_PowerOutputScaling);
	}

	if (m_LaserPowerCalibrationList.size() >= 2) {
		size_t nMinIndex = 0;
		size_t nMaxIndex = m_LaserPowerCalibrationList.size() - 1;
		auto& minCalibration = m_LaserPowerCalibrationList.at(nMinIndex);
		auto& maxCalibration = m_LaserPowerCalibrationList.at(nMaxIndex);

		if (dPowerInPercent < minCalibration.m_PowerOffsetInPercent) {
			dAdjustedPowerInPercent = adjustLaserPowerCalibration(dPowerInPercent, minCalibration.m_PowerOffsetInPercent, minCalibration.m_PowerOutputScaling);
		} 
		else if (dPowerInPercent > maxCalibration.m_PowerOffsetInPercent) {
			dAdjustedPowerInPercent = adjustLaserPowerCalibration(dPowerInPercent, maxCalibration.m_PowerOffsetInPercent, maxCalibration.m_PowerOutputScaling);
		}
		else {
			// Binary search of calibration values
			while ((nMinIndex + 1) < nMaxIndex) {
				size_t nMidIndex = (nMinIndex + nMaxIndex) / 2;
				auto& calibration = m_LaserPowerCalibrationList.at(nMidIndex);

				if (dPowerInPercent < calibration.m_PowerOffsetInPercent) {
					nMaxIndex = nMidIndex;
					maxCalibration = calibration;
				}
				else {
					nMinIndex = nMidIndex;
					minCalibration = calibration;
				}
			}

			if ((nMinIndex + 1) != nMaxIndex)
				throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_POWERCALIBRATIONLOOKUPFAILED);
			
			double dAdjustedMinPowerInPercent = adjustLaserPowerCalibration(dPowerInPercent, minCalibration.m_PowerOffsetInPercent, minCalibration.m_PowerOutputScaling);
			double dAdjustedMaxPowerInPercent = adjustLaserPowerCalibration(dPowerInPercent, maxCalibration.m_PowerOffsetInPercent, maxCalibration.m_PowerOutputScaling);
			double dDelta = (maxCalibration.m_PowerSetPointInPercent - minCalibration.m_PowerSetPointInPercent);
			// dDelta should be larger than any arbitrary fraction of units.
			if (dDelta < m_dLaserPowerCalibrationUnits * 0.1)
				throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_POWERCALIBRATIONLOOKUPFAILED);

			double dFactor = (dPowerInPercent - minCalibration.m_PowerSetPointInPercent) / dDelta;
			if (dFactor < 0.0)
				throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_POWERCALIBRATIONLOOKUPFAILED);
			if (dFactor > 1.0)
				throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_POWERCALIBRATIONLOOKUPFAILED);

			// Linear interpolation between factors
			dAdjustedPowerInPercent = (1.0 - dFactor) * dAdjustedMinPowerInPercent + dFactor * dAdjustedMaxPowerInPercent;

		}



		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_NOTIMPLEMENTED, "multiple power calibration values not implemented");
	}


	double dClippedPowerFactor = dPowerInPercent / 100.0f;
	if (dClippedPowerFactor > 1.0f)
		dClippedPowerFactor = 1.0f;
	if (dClippedPowerFactor < 0.0f)
		dClippedPowerFactor = 0.0f;

	int32_t digitalPowerValue = 0;


	if (bOIEPIDControlFlag) {

		switch (m_LaserPort) {
		case eLaserPort::Port16bitDigital:
			digitalPowerValue = (int32_t)round(dClippedPowerFactor * 65535.0);
			//nPortIndex = 6;  See set_auto_laser_control in SDK documentation
			m_pScanLabSDK->n_set_multi_mcbsp_in_list(m_CardNo, 6, digitalPowerValue, 1);
			break;
		case eLaserPort::Port8bitDigital:
			digitalPowerValue = (int32_t)round(dClippedPowerFactor * 255.0);
			//nPortIndex = 3;  See set_auto_laser_control in SDK documentation
			m_pScanLabSDK->n_set_multi_mcbsp_in_list(m_CardNo, 3, digitalPowerValue, 1);
			break;
		case eLaserPort::Port12BitAnalog1:
			digitalPowerValue = (int32_t)round(dClippedPowerFactor * 4095.0);
			//nPortIndex = 1; // See set_auto_laser_control in SDK documentation
			m_pScanLabSDK->n_set_multi_mcbsp_in_list(m_CardNo, 1, digitalPowerValue, 1);
			break;
		case eLaserPort::Port12BitAnalog2:
			digitalPowerValue = (int32_t)round(dClippedPowerFactor * 4095.0);
			//nPortIndex = 2; // See set_auto_laser_control in SDK documentation
			m_pScanLabSDK->n_set_multi_mcbsp_in_list(m_CardNo, 2, digitalPowerValue, 1);
			break;
		case eLaserPort::Port12BitAnalog1andAnalog2:
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_MULTIPLELASERPORTSNOTCOMPATIBLEWITHPID);
		}

		m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

		m_pScanLabSDK->n_set_fly_2d(m_CardNo, 1.0, 1.0);
		m_pScanLabSDK->n_long_delay(m_CardNo, 10);
		m_pScanLabSDK->n_fly_return(m_CardNo, 0, 0);

		m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);


	}
	else {

		switch (m_LaserPort) {
		case eLaserPort::Port16bitDigital:
			digitalPowerValue = (int32_t)round(dClippedPowerFactor * 65535.0);
			//nPortIndex = 3;  See set_laser_power in SDK documentation
			m_pScanLabSDK->n_set_laser_power(m_CardNo, 3, digitalPowerValue);

			break;
		case eLaserPort::Port8bitDigital:
			digitalPowerValue = (int32_t)round(dClippedPowerFactor * 255.0);
			//nPortIndex = 2; See set_laser_power in SDK documentation
			m_pScanLabSDK->n_set_laser_power(m_CardNo, 2, digitalPowerValue);
			break;
		case eLaserPort::Port12BitAnalog1:
			digitalPowerValue = (int32_t)round(dClippedPowerFactor * 4095.0);
			//nPortIndex = 0;  See set_laser_power in SDK documentation
			m_pScanLabSDK->n_set_laser_power(m_CardNo, 0, digitalPowerValue);
			break;
		case eLaserPort::Port12BitAnalog2:
			digitalPowerValue = (int32_t)round(dClippedPowerFactor * 4095.0);
			//nPortIndex = 1; // See set_laser_power in SDK documentation
			m_pScanLabSDK->n_set_laser_power(m_CardNo, 1, digitalPowerValue);
			break;
		case eLaserPort::Port12BitAnalog1andAnalog2:
			digitalPowerValue = (int32_t)round(dClippedPowerFactor * 4095.0);
			m_pScanLabSDK->n_set_laser_power(m_CardNo, 0, digitalPowerValue);
			m_pScanLabSDK->n_set_laser_power(m_CardNo, 1, digitalPowerValue);
			break;
		}

		m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

		/*

RTC5 Compatibility mode:

switch (m_LaserPort) {
case eLaserPort::Port16bitDigital:
	digitalPowerValue = (int)round(fClippedPowerFactor * 65535.0);
	// m_pScanLabSDK->n_write_io_port_list(m_CardNo, digitalPowerValue);
	break;
case eLaserPort::Port8bitDigital:
	digitalPowerValue = (int)round(fClippedPowerFactor * 255.0);
	m_pScanLabSDK->n_write_8bit_port_list(m_CardNo, digitalPowerValue);
	break;
case eLaserPort::Port12BitAnalog1:
	digitalPowerValue = (int)round(fClippedPowerFactor * 4095.0);
	m_pScanLabSDK->n_write_da_1_list(m_CardNo, digitalPowerValue);
	break;
case eLaserPort::Port12BitAnalog2:
	digitalPowerValue = (int)round(fClippedPowerFactor * 4095.0);
	m_pScanLabSDK->n_write_da_2_list(m_CardNo, digitalPowerValue);
	break;

} */

	}
}


void CRTCContext::writeSpeeds(const LibMCDriver_ScanLab_single fMarkSpeed, const LibMCDriver_ScanLab_single fJumpSpeed, const LibMCDriver_ScanLab_single fPower, bool bOIEPIDControlFlag)
{

	writeMarkSpeed(fMarkSpeed);
	writeJumpSpeed(fJumpSpeed);
	writePower(fPower, bOIEPIDControlFlag);



}

void CRTCContext::jumpAbsoluteEx(double dTargetXInMM, double dTargetYInMM)
{
	double dTargetXInUnits = round((dTargetXInMM - m_dLaserOriginX) * m_dCorrectionFactor);
	double dTargetYInUnits = round((dTargetYInMM - m_dLaserOriginY) * m_dCorrectionFactor);

	int nTargetX = (int32_t)dTargetXInUnits;
	int nTargetY = (int32_t)dTargetYInUnits;

	// Avoid Null Jumps!
	if ((nTargetX != m_nCurrentScanPositionX) || (nTargetY != m_nCurrentScanPositionY)) {
		m_pScanLabSDK->n_jump_abs(m_CardNo, nTargetX, nTargetY);
		m_nCurrentScanPositionX = nTargetX;
		m_nCurrentScanPositionY = nTargetY;
	}

	// Do not check error because that creates timing issues..
	// m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));

}

void CRTCContext::markAbsoluteEx(double dStartXInMM, double dStartYInMM, double dTargetXInMM, double dTargetYInMM, double dLaserPowerInPercent, bool bOIEControlFlag)
{
	int32_t nSubdivisionCount = 1;
	double dDeltaX = dTargetXInMM - dStartXInMM;
	double dDeltaY = dTargetYInMM - dStartYInMM;

	int32_t nModulationType = 0; // TODO

	if (m_bEnableLineSubdivision) {
		double dLen = sqrt(dDeltaX * dDeltaX + dDeltaY * dDeltaY);
		if (dLen > RTCCONTEXT_MIN_LINESUBDIVISIONTHRESHOLD) {
			nSubdivisionCount = (int32_t) ceil(dLen / m_dLineSubdivisionThreshold);
			dDeltaX /= nSubdivisionCount;
			dDeltaY /= nSubdivisionCount;
		}
	}

	double dOldX = dStartXInMM;
	double dOldY = dStartYInMM;

	for (int32_t nSubdivisionIndex = 1; nSubdivisionIndex <= nSubdivisionCount; nSubdivisionIndex++) {

		double dMarkToX = dStartXInMM + nSubdivisionIndex * dDeltaX;
		double dMarkToY = dStartYInMM + nSubdivisionIndex * dDeltaY;

		double dTargetXInUnits = round((dMarkToX - m_dLaserOriginX) * m_dCorrectionFactor);
		double dTargetYInUnits = round((dMarkToY - m_dLaserOriginY) * m_dCorrectionFactor);

		if (m_pModulationCallback != nullptr) {
			double dNewPowerInPercent = dLaserPowerInPercent;
			m_pModulationCallback(dOldX, dOldY, dMarkToX, dMarkToY, dLaserPowerInPercent, nModulationType, m_pModulationCallbackUserData, &dNewPowerInPercent);
			writePower(dNewPowerInPercent, bOIEControlFlag);
		}

		int32_t nTargetX = (int32_t)dTargetXInUnits;
		int32_t nTargetY = (int32_t)dTargetYInUnits;
		m_pScanLabSDK->n_mark_abs(m_CardNo, nTargetX, nTargetY);
		m_nCurrentScanPositionX = nTargetX;
		m_nCurrentScanPositionY = nTargetY;

		//m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));
		
		dOldX = dMarkToX;
		dOldY = dMarkToY;
	}


}

void CRTCContext::DrawPolyline(const LibMCDriver_ScanLab_uint64 nPointsBufferSize, const LibMCDriver_ScanLab::sPoint2D* pPointsBuffer, const LibMCDriver_ScanLab_single fMarkSpeed, const LibMCDriver_ScanLab_single fJumpSpeed, const LibMCDriver_ScanLab_single fPower, const LibMCDriver_ScanLab_single fZValue)
{
	DrawPolylineOIE(nPointsBufferSize, pPointsBuffer, fMarkSpeed, fJumpSpeed, fPower, fZValue, 0);
}

void CRTCContext::DrawPolylineOIE(const LibMCDriver_ScanLab_uint64 nPointsBufferSize, const LibMCDriver_ScanLab::sPoint2D* pPointsBuffer, const LibMCDriver_ScanLab_single fMarkSpeed, const LibMCDriver_ScanLab_single fJumpSpeed, const LibMCDriver_ScanLab_single fPower, const LibMCDriver_ScanLab_single fZValue, const uint32_t nOIEPIDControlIndex)
{

	if (!pPointsBuffer)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);

	if (nPointsBufferSize == 0)
		return;

	bool bOIEControlFlag = (nOIEPIDControlIndex != 0);
	writeSpeeds(fMarkSpeed, fJumpSpeed, fPower, bOIEControlFlag);

	// Z Plane
	double defocusZ = round(fZValue * m_dZCorrectionFactor);
	int intDefocusZ = (int)defocusZ;
	m_pScanLabSDK->n_set_defocus_list (m_CardNo, intDefocusZ);

	const sPoint2D* pPoint = pPointsBuffer;
	jumpAbsoluteEx(pPoint->m_X, pPoint->m_Y);
	auto pPrevPoint = pPoint;

	if (bOIEControlFlag)
		SetOIEPIDMode(nOIEPIDControlIndex);

	for (uint64_t index = 1; index < nPointsBufferSize; index++) {
		pPoint++;

		sendOIEMeasurementTag((uint32_t)index);

		markAbsoluteEx(pPrevPoint->m_X, pPrevPoint->m_Y, pPoint->m_X, pPoint->m_Y, fPower, bOIEControlFlag);
		pPrevPoint = pPoint;

	}

}

void CRTCContext::DrawHatchesOIE(const LibMCDriver_ScanLab_uint64 nHatchesBufferSize, const LibMCDriver_ScanLab::sHatch2D* pHatchesBuffer, const LibMCDriver_ScanLab_single fMarkSpeed, const LibMCDriver_ScanLab_single fJumpSpeed, const LibMCDriver_ScanLab_single fPower, const LibMCDriver_ScanLab_single fZValue, const uint32_t nOIEPIDControlIndex)
{
	if (!pHatchesBuffer)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);

	if (nHatchesBufferSize == 0)
		return;

	bool bOIEControlFlag = (nOIEPIDControlIndex != 0);
	writeSpeeds(fMarkSpeed, fJumpSpeed, fPower, bOIEControlFlag);

	// Z Plane
	double defocusZ = round(fZValue * m_dZCorrectionFactor);
	int intDefocusZ = (int)defocusZ;
	m_pScanLabSDK->n_set_defocus_list(m_CardNo, intDefocusZ);

	if (bOIEControlFlag)
		SetOIEPIDMode(nOIEPIDControlIndex);

	const sHatch2D* pHatch = pHatchesBuffer;

	for (uint64_t index = 0; index < nHatchesBufferSize; index++) {
		jumpAbsoluteEx(pHatch->m_X1, pHatch->m_Y1);

		sendOIEMeasurementTag((uint32_t)index);

		markAbsoluteEx(pHatch->m_X1, pHatch->m_Y1, pHatch->m_X2, pHatch->m_Y2, fPower, bOIEControlFlag);

		pHatch++;
	}

}

void CRTCContext::DrawHatches(const LibMCDriver_ScanLab_uint64 nHatchesBufferSize, const LibMCDriver_ScanLab::sHatch2D* pHatchesBuffer, const LibMCDriver_ScanLab_single fMarkSpeed, const LibMCDriver_ScanLab_single fJumpSpeed, const LibMCDriver_ScanLab_single fPower, const LibMCDriver_ScanLab_single fZValue)
{
	DrawHatchesOIE(nHatchesBufferSize, pHatchesBuffer, fMarkSpeed, fJumpSpeed, fPower, fZValue, 0);
}

void CRTCContext::AddJumpMovement(const LibMCDriver_ScanLab_double dTargetX, const LibMCDriver_ScanLab_double dTargetY)
{
	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);
	jumpAbsoluteEx(dTargetX, dTargetY);
}

void CRTCContext::AddMarkMovement(const LibMCDriver_ScanLab_double dTargetX, const LibMCDriver_ScanLab_double dTargetY)
{
	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);
	double dX = round((dTargetX - m_dLaserOriginX) * m_dCorrectionFactor);
	double dY = round((dTargetY - m_dLaserOriginY) * m_dCorrectionFactor);

	int32_t intX = (int32_t)dX;
	int32_t intY = (int32_t)dY;

	m_pScanLabSDK->n_mark_abs(m_CardNo, intX, intY);
	m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));

	m_nCurrentScanPositionX = intX;
	m_nCurrentScanPositionY = intY;
}

void CRTCContext::AddFreeVariable(const LibMCDriver_ScanLab_uint32 nVariableNo, const LibMCDriver_ScanLab_uint32 nValue)
{
	if (nVariableNo > 7)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDFREEVARIABLEINDEX);

	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);
	m_pScanLabSDK->n_set_free_variable_list(m_CardNo, nVariableNo, nValue);
	m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));

}

void CRTCContext::StopExecution()
{
	m_pScanLabSDK->n_stop_execution(m_CardNo);
	m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));

}

bool CRTCContext::LaserPowerCalibrationIsEnabled()
{
	return m_LaserPowerCalibrationList.size() > 0;
}

bool CRTCContext::LaserPowerCalibrationIsLinear()
{
	return m_LaserPowerCalibrationList.size() == 1;
}

void CRTCContext::ClearLaserPowerCalibration()
{
	m_LaserPowerCalibrationList.clear();
}

void CRTCContext::GetLaserPowerCalibration(LibMCDriver_ScanLab_uint64 nCalibrationPointsBufferSize, LibMCDriver_ScanLab_uint64* pCalibrationPointsNeededCount, LibMCDriver_ScanLab::sLaserCalibrationPoint* pCalibrationPointsBuffer)
{
	if (pCalibrationPointsNeededCount != nullptr)
		*pCalibrationPointsNeededCount = m_LaserPowerCalibrationList.size();

	if (pCalibrationPointsBuffer != nullptr) {
		if (nCalibrationPointsBufferSize < m_LaserPowerCalibrationList.size())
			throw ELibMCDriver_ScanLabInterfaceException (LIBMCDRIVER_SCANLAB_ERROR_BUFFERTOOSMALL);

		auto pTarget = pCalibrationPointsBuffer;
		for (auto iIter = m_LaserPowerCalibrationList.begin(); iIter != m_LaserPowerCalibrationList.end(); iIter++) {
			*pTarget = *iIter;
			pTarget++;
		}
	}
}

void CRTCContext::SetLinearLaserPowerCalibration(const LibMCDriver_ScanLab_double dPowerOffsetInPercent, const LibMCDriver_ScanLab_double dPowerOutputScaling) 
{
	if (dPowerOutputScaling < 0.0)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPOWERCALIBRATIONOUTPUTSCALING);

	m_LaserPowerCalibrationList.clear();
	sLaserCalibrationPoint calibration;
	calibration.m_PowerSetPointInPercent = 0.0;
	calibration.m_PowerOffsetInPercent = dPowerOffsetInPercent;
	calibration.m_PowerOutputScaling = dPowerOutputScaling;
	m_LaserPowerCalibrationList.push_back(calibration);

}

bool calibrationPointCompare (LibMCDriver_ScanLab::sLaserCalibrationPoint point1, LibMCDriver_ScanLab::sLaserCalibrationPoint point2) {
	return (point1.m_PowerSetPointInPercent < point2.m_PowerSetPointInPercent); 
}

void CRTCContext::SetPiecewiseLinearLaserPowerCalibration(const LibMCDriver_ScanLab_uint64 nCalibrationPointsBufferSize, const LibMCDriver_ScanLab::sLaserCalibrationPoint* pCalibrationPointsBuffer)
{
	m_LaserPowerCalibrationList.clear();
	if (nCalibrationPointsBufferSize > 0) {
		if (pCalibrationPointsBuffer == nullptr)
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);

		for (size_t nIndex = 0; nIndex < nCalibrationPointsBufferSize; nIndex++) {
			auto point = pCalibrationPointsBuffer[nIndex];
			if (point.m_PowerSetPointInPercent < 0.0)
				throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPOWERCALIBRATIONSETPOINT);
			if (point.m_PowerOutputScaling < 0.0)
				throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPOWERCALIBRATIONOUTPUTSCALING);

			m_LaserPowerCalibrationList.push_back(point);
		}

		std::sort(m_LaserPowerCalibrationList.begin(), m_LaserPowerCalibrationList.end(), calibrationPointCompare);

		// Make sure List is strictly ascending in Power!
		int64_t nOldDiscretePower = -1;
		for (auto & point : m_LaserPowerCalibrationList) {
			int64_t nDiscreteLaserPower = (int64_t) round (point.m_PowerSetPointInPercent / m_dLaserPowerCalibrationUnits);
			if (nDiscreteLaserPower <= nOldDiscretePower)
				throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_DUPLICATELASERPOWERCALIBRATIONSETPOINT);
			nOldDiscretePower = nDiscreteLaserPower;
		}

	}

}

void CRTCContext::EnableSpatialLaserPowerModulation(const LibMCDriver_ScanLab::SpatialPowerModulationCallback pModulationCallback, const LibMCDriver_ScanLab_pvoid pUserData)
{
	if (pModulationCallback == nullptr)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDMODULATIONCALLBACK);

	m_pModulationCallback = pModulationCallback;
	m_pModulationCallbackUserData = pUserData;
}

void CRTCContext::DisablePowerModulation()
{
	m_pModulationCallback = nullptr;
	m_pModulationCallbackUserData = nullptr;
}

void CRTCContext::EnableLineSubdivision(const LibMCDriver_ScanLab_double dLengthThreshold)
{
	if (dLengthThreshold < RTCCONTEXT_MIN_LINESUBDIVISIONTHRESHOLD)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDSUBDIVISIONTHRESHOLD);
	if (dLengthThreshold > RTCCONTEXT_MAX_LINESUBDIVISIONTHRESHOLD)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDSUBDIVISIONTHRESHOLD);

	m_dLineSubdivisionThreshold = dLengthThreshold;
	m_bEnableLineSubdivision = true;
}

void CRTCContext::DisableLineSubdivision()
{
	m_dLineSubdivisionThreshold = RTCCONTEXT_MAX_LINESUBDIVISIONTHRESHOLD;
	m_bEnableLineSubdivision = false;

}


double CRTCContext::adjustLaserPowerCalibration(double dLaserPowerInPercent, double dPowerOffsetInPercent, double dPowerOutputScaling)
{
	return ((dLaserPowerInPercent + dPowerOffsetInPercent) * dPowerOutputScaling);
}


void CRTCContext::AddSetPower(const LibMCDriver_ScanLab_single fPowerInPercent)
{
	writePower(fPowerInPercent, false);
}

void CRTCContext::AddSetAnalogOut(const LibMCDriver_ScanLab::eLaserPort eLaserPort, const LibMCDriver_ScanLab_single fOutputValue)
{

	float fClippedValue = fOutputValue;
	if (fClippedValue < 0.0f)
		fClippedValue = 0.0f;
	if (fClippedValue > 1.0f)
		fClippedValue = 1.0f;

	int32_t nDigitalValue = (int32_t)round(fClippedValue * 4095.0);
	if (nDigitalValue < 0)
		nDigitalValue = 0;
	if (nDigitalValue > 4095)
		nDigitalValue = 4095;

	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);

	switch (eLaserPort) {		
		case eLaserPort::Port12BitAnalog1:
			m_pScanLabSDK->n_write_da_1_list(m_CardNo, nDigitalValue);
			m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);
			break;

		case eLaserPort::Port12BitAnalog2:
			m_pScanLabSDK->n_write_da_2_list(m_CardNo, nDigitalValue);
			m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);
			break;

		case eLaserPort::Port12BitAnalog1andAnalog2:
			m_pScanLabSDK->n_write_da_1_list(m_CardNo, nDigitalValue);
			m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

			m_pScanLabSDK->n_write_da_2_list(m_CardNo, nDigitalValue);
			m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);
			break;

		default:
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_PORTNUMBERISNOTANALOG);
	}

}

void CRTCContext::AddSetDigitalOut(const LibMCDriver_ScanLab::eLaserPort eLaserPort, const LibMCDriver_ScanLab_single fOutputValue)
{
	float fClippedValue = fOutputValue;
	if (fClippedValue < 0.0f)
		fClippedValue = 0.0f;
	if (fClippedValue > 1.0f)
		fClippedValue = 1.0f;

	int32_t nDigitalValue;
	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);

	switch (eLaserPort) {
	case eLaserPort::Port16bitDigital:
		nDigitalValue = (int32_t)round(fClippedValue * 65535.0);
		if (nDigitalValue < 0)
			nDigitalValue = 0;
		if (nDigitalValue > 65535)
			nDigitalValue = 65535;

		m_pScanLabSDK->n_write_io_port_list(m_CardNo, nDigitalValue);
		m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);
		break;

	case eLaserPort::Port8bitDigital:
		nDigitalValue = (int32_t)round(fClippedValue * 255.0);
		if (nDigitalValue < 0)
			nDigitalValue = 0;
		if (nDigitalValue > 255)
			nDigitalValue = 255;

		m_pScanLabSDK->n_write_8bit_port_list(m_CardNo, nDigitalValue);
		m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);
		break;

	default:
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_PORTNUMBERISNOTDIGITAL);

	}

}


void CRTCContext::AddSetPowerForPIDControl(const LibMCDriver_ScanLab_single fPowerInPercent)
{
	writePower(fPowerInPercent, true);
}


void CRTCContext::AddSetJumpSpeed(const LibMCDriver_ScanLab_single fJumpSpeedInMMPerSecond)
{
	writeJumpSpeed(fJumpSpeedInMMPerSecond);
}

void CRTCContext::AddSetMarkSpeed(const LibMCDriver_ScanLab_single fMarkSpeedInMMPerSecond)
{
	writeMarkSpeed(fMarkSpeedInMMPerSecond);
}

void CRTCContext::AddTimedMarkMovement(const LibMCDriver_ScanLab_double dTargetX, const LibMCDriver_ScanLab_double dTargetY, const LibMCDriver_ScanLab_double dDurationInMicroseconds)
{
	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);
	double dX = round((dTargetX - m_dLaserOriginX) * m_dCorrectionFactor);
	double dY = round((dTargetY - m_dLaserOriginY) * m_dCorrectionFactor);

	int32_t intX = (int32_t)dX;
	int32_t intY = (int32_t)dY;

	m_pScanLabSDK->n_timed_mark_abs(m_CardNo, intX, intY, dDurationInMicroseconds);
	m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));

	m_nCurrentScanPositionX = intX;
	m_nCurrentScanPositionY = intY;
}


LibMCDriver_ScanLab_uint32 CRTCContext::GetCurrentFreeVariable(const LibMCDriver_ScanLab_uint32 nVariableNo)
{
	if (nVariableNo > 7)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDFREEVARIABLEINDEX);

	return m_pScanLabSDK->n_get_free_variable(m_CardNo, nVariableNo);

}

LibMCDriver_ScanLab_uint32 CRTCContext::GetTimeStamp()
{
	// See set_trigger in Scanlab documentation. 52 is time stamp counter.
	return m_pScanLabSDK->n_get_value(m_CardNo, 52);
}

void CRTCContext::AddCustomDelay(const LibMCDriver_ScanLab_uint32 nDelay)
{
	if ((nDelay % 10) != 0)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_DELAYSMUSTBEMULTIPLEOF10);

	uint32_t DelayInBits = nDelay / 10;

	m_pScanLabSDK->n_long_delay(m_CardNo, DelayInBits);
	m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));
}

LibMCDriver_ScanLab_double CRTCContext::GetCorrectionFactor()
{
	return m_dCorrectionFactor;
}

void CRTCContext::GetStatus(bool & bBusy, LibMCDriver_ScanLab_uint32 & nPosition)
{
	uint32_t Status = 0;
	uint32_t Pos = 0;
	m_pScanLabSDK->n_get_status(m_CardNo, &Status, &Pos);

	//std::cout << "Free Variable: " << m_pScanLabSDK->n_get_value (m_CardNo, 40) << std::endl;

	m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));

	bBusy = ((Status & ((1UL << 0) | (1UL << 7) | (1UL << 15))) != 0);
	nPosition = Pos;
}

void CRTCContext::GetHeadStatus(const LibMCDriver_ScanLab_uint32 nHeadNo, bool& bPositionXisOK, bool& bPositionYisOK, bool& bTemperatureisOK, bool& bPowerisOK)
{
	uint32_t Status = m_pScanLabSDK->n_get_head_status(m_CardNo, nHeadNo);
	bPositionXisOK = (Status & (1UL << 3)) != 0;
	bPositionYisOK = (Status & (1UL << 4)) != 0;
	bTemperatureisOK = (Status & (1UL << 6)) != 0;
	bPowerisOK = (Status & (1UL << 7)) != 0;
}


LibMCDriver_ScanLab_uint32 CRTCContext::GetInputPointer()
{
	uint32_t Pos = m_pScanLabSDK->n_get_input_pointer(m_CardNo);
	m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));
	return Pos;
}

void CRTCContext::GetRTCVersion(LibMCDriver_ScanLab_uint32& nRTCVersion, LibMCDriver_ScanLab_uint32& nRTCType, LibMCDriver_ScanLab_uint32& nDLLVersion, LibMCDriver_ScanLab_uint32& nHEXVersion, LibMCDriver_ScanLab_uint32& nBIOSVersion) 
{

	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);
	nDLLVersion = m_pScanLabSDK->get_dll_version();
	nRTCVersion = m_pScanLabSDK->n_get_rtc_version(m_CardNo);
	nRTCType = m_pScanLabSDK->n_get_card_type(m_CardNo);
	nBIOSVersion = m_pScanLabSDK->n_get_bios_version(m_CardNo);
	nHEXVersion = m_pScanLabSDK->n_get_hex_version(m_CardNo);

	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);

}

void CRTCContext::GetStateValues(bool& bLaserIsOn, LibMCDriver_ScanLab_int32& nPositionX, LibMCDriver_ScanLab_int32& nPositionY, LibMCDriver_ScanLab_int32& nPositionZ, LibMCDriver_ScanLab_int32& nCorrectedPositionX, LibMCDriver_ScanLab_int32& nCorrectedPositionY, LibMCDriver_ScanLab_int32& nCorrectedPositionZ, LibMCDriver_ScanLab_int32& nFocusShift, LibMCDriver_ScanLab_int32& nMarkSpeed)
{
	bLaserIsOn = m_pScanLabSDK->n_get_value(m_CardNo, 0); // LASERON
	nPositionX = m_pScanLabSDK->n_get_value(m_CardNo, 7); // SampleX
	nPositionY = m_pScanLabSDK->n_get_value(m_CardNo, 8); // SampleY
	nPositionZ = m_pScanLabSDK->n_get_value(m_CardNo, 9); // SampleZ
	nCorrectedPositionX = m_pScanLabSDK->n_get_value(m_CardNo, 10); // SampleXCorr
	nCorrectedPositionY = m_pScanLabSDK->n_get_value(m_CardNo, 11); // SampleYCorr
	nCorrectedPositionZ = m_pScanLabSDK->n_get_value(m_CardNo, 12); // SampleZCorr
	nFocusShift = m_pScanLabSDK->n_get_value(m_CardNo, 32); // Focus shift
	nMarkSpeed = m_pScanLabSDK->n_get_value(m_CardNo, 45); // Mark Speed
}

void CRTCContext::SetCommunicationTimeouts(const LibMCDriver_ScanLab_double dInitialTimeout, const LibMCDriver_ScanLab_double dMaxTimeout, const LibMCDriver_ScanLab_double dMultiplier)
{
	setCommunicationTimeoutsEx(m_pScanLabSDK, m_CardNo, dInitialTimeout, dMaxTimeout, dMultiplier);
}

void CRTCContext::GetCommunicationTimeouts(LibMCDriver_ScanLab_double& dInitialTimeout, LibMCDriver_ScanLab_double& dMaxTimeout, LibMCDriver_ScanLab_double& dMultiplier)
{
	double dRetrievedInitialTimeout = 0.0;
	double dRetrievedMaxTimeout = 0.0;
	double dRetrievedMultiplier = 0.0;
	uint32_t nRetrievedMode = 0;
	m_pScanLabSDK->n_eth_get_com_timeouts_auto(m_CardNo, &dRetrievedInitialTimeout, &dRetrievedMaxTimeout, &dRetrievedMultiplier, &nRetrievedMode);

	dInitialTimeout = dRetrievedInitialTimeout;
	dMaxTimeout = dRetrievedMaxTimeout;
	dMultiplier = dRetrievedMultiplier;
}

void CRTCContext::InitializeForOIE(const LibMCDriver_ScanLab_uint64 nSignalChannelsBufferSize, const LibMCDriver_ScanLab_uint32* pSignalChannelsBuffer, const LibMCDriver_ScanLab::eOIEOperationMode eOperationMode)
{

	switch (eOperationMode) {
		case LibMCDriver_ScanLab::eOIEOperationMode::OIEVersion2:
			m_OIEOperationMode = LibMCDriver_ScanLab::eOIEOperationMode::OIEVersion2;
			break;

		case LibMCDriver_ScanLab::eOIEOperationMode::OIEVersion3Compatibility:
			m_OIEOperationMode = LibMCDriver_ScanLab::eOIEOperationMode::OIEVersion3Compatibility;
			break;

		case LibMCDriver_ScanLab::eOIEOperationMode::OIEVersion3:
			m_OIEOperationMode = LibMCDriver_ScanLab::eOIEOperationMode::OIEVersion3;
			break;

		default:
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_UNSUPPORTEDOIEOPERATIONMODE);
	}

	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);

	// Configure Scanhead backchannels
	m_pScanLabSDK->n_control_command(m_CardNo, 1, 1, 0x0501);  //00 XY status word, 01 actual position, 02 set position, 03 position error
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

	m_pScanLabSDK->n_control_command(m_CardNo, 1, 2, 0x0501);  //00 XY status word, 01 actual position, 02 set position, 03 position error
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

	if ((nSignalChannelsBufferSize == 0) || (pSignalChannelsBuffer == nullptr))
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDOIESIGNALBUFFERARRAY);

	m_MCBSPSignalChannels.resize(nSignalChannelsBufferSize);
	for (size_t nIndex = 0; nIndex < nSignalChannelsBufferSize; nIndex++)
		m_MCBSPSignalChannels.at(nIndex) = pSignalChannelsBuffer[nIndex];
		
	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);

	// Frequency should be 6MHz by specification
	uint32_t nMCBSPSignalFrequency = 6000000;
	m_pScanLabSDK->n_set_mcbsp_freq(m_CardNo, nMCBSPSignalFrequency);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

	// Magic number parameters, MUST be 1, 1
	m_pScanLabSDK->n_mcbsp_init(m_CardNo, 1, 1);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

	switch (eOperationMode) {
	case LibMCDriver_ScanLab::eOIEOperationMode::OIEVersion2:
	case LibMCDriver_ScanLab::eOIEOperationMode::OIEVersion3Compatibility:
	{
		// Set signal channels
		uint32_t nTransferLaserOnFlag = (1UL << 31);
		m_pScanLabSDK->n_set_mcbsp_out_ptr(m_CardNo, (uint32_t)m_MCBSPSignalChannels.size() | nTransferLaserOnFlag, m_MCBSPSignalChannels.data());
		m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

		break;
	}

	case LibMCDriver_ScanLab::eOIEOperationMode::OIEVersion3:
	{
		// Set signal channels
		if (m_MCBSPSignalChannels.size() < 2)
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDOIECHANNELSIZE);

		m_pScanLabSDK->n_set_mcbsp_out_oie_ctrl(m_CardNo, m_MCBSPSignalChannels.at(0), m_MCBSPSignalChannels.at(1));
		m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

		// See documentation. 8192 is the packet stream size,
		// The 1 means datastreaming is on
		m_pScanLabSDK->n_eth_config_waveform_streaming_ctrl(m_CardNo, 8192, 1);
		m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

		break;
	}

	default:
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_UNSUPPORTEDOIEOPERATIONMODE);
	}


	// No PID control for now
	m_pScanLabSDK->n_set_multi_mcbsp_in(m_CardNo, 0, 0, 0);


	// Workaround: Ensure that MCBSP Mark On The Fly is disabled!
	SetStartList(1, 0);
	m_pScanLabSDK->n_set_fly_2d(m_CardNo, 1.0, 1.0);
	m_pScanLabSDK->n_long_delay(m_CardNo, 10);
	m_pScanLabSDK->n_fly_return(m_CardNo, 0, 0);
	SetEndOfList();
	ExecuteList(1, 0);

	bool Busy = true;
	uint32_t Pos = 0;
	uint32_t nMaxRetries = 256;

	for (uint32_t nRetries = 0; nRetries < nMaxRetries; nRetries++) {
		GetStatus(Busy, Pos);
		if (!Busy)
			break;

		m_pDriverEnvironment->Sleep(10);
	}

	if (Busy)
		throw std::runtime_error("could not properly intialise MCBSP connection to OIE");

	// Check Error
	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);

}


uint32_t CRTCContext::getCurrentFreeVariable0()
{
	return m_nCurrentFreeVariable0;
}


void CRTCContext::sendFreeVariable0(uint32_t nValue)
{
	if (m_OIEOperationMode == LibMCDriver_ScanLab::eOIEOperationMode::OIEVersion3) {
		m_pScanLabSDK->n_list_nop(m_CardNo);
	}
	else {
		m_pScanLabSDK->n_long_delay(m_CardNo, (uint32_t)m_MCBSPSignalChannels.size());
	}
	
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

	m_pScanLabSDK->n_set_free_variable_list(m_CardNo, 0, nValue);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

	m_pScanLabSDK->n_list_nop(m_CardNo);

	m_nCurrentFreeVariable0 = nValue;
}

void CRTCContext::sendOIEMeasurementTag(uint32_t nCurrentVectorID)
{
	if (m_bMeasurementTagging) {
		m_CurrentMeasurementTagInfo.m_nCurrentVectorID = nCurrentVectorID;
		m_MeasurementTags.push_back(m_CurrentMeasurementTagInfo);

		uint32_t nMeasurementTag = (uint32_t)m_MeasurementTags.size() & ((1UL << 22) - 1);

		m_pScanLabSDK->n_set_free_variable_list(m_CardNo, 1, nMeasurementTag);
		m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

	}

}

void CRTCContext::SetLaserPinOut(const bool bLaserOut1, const bool bLaserOut2)
{
	uint32_t nPins = 0;
	if (bLaserOut1)
		nPins |= 1;
	if (bLaserOut2)
		nPins |= 2;

	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);
	m_pScanLabSDK->n_set_laser_pin_out(m_CardNo, nPins);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

}

void CRTCContext::GetLaserPinIn(bool& bLaserOut1, bool& bLaserOut2)
{
	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);
	uint32_t nPins = m_pScanLabSDK->n_get_laser_pin_in(m_CardNo);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

	bLaserOut1 = (nPins & 1UL) != 0;
	bLaserOut2 = (nPins & 2UL) != 0;
}

void CRTCContext::AddLaserPinOutToList(const bool bLaserOut1, const bool bLaserOut2)
{
	uint32_t nPins = 0;
	if (bLaserOut1)
		nPins |= 1;
	if (bLaserOut2)
		nPins |= 2;

	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);
	m_pScanLabSDK->n_set_laser_pin_out_list(m_CardNo, nPins);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

}


void CRTCContext::AddWriteDigitalIOList(const LibMCDriver_ScanLab_uint32 nDigitalOutput)
{
	if (nDigitalOutput > 65535)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDDIGITALOUTPUTVALUE);

	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);
	m_pScanLabSDK->n_write_io_port_list(m_CardNo, nDigitalOutput);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

}

void CRTCContext::AddWriteMaskedDigitalIOList(const LibMCDriver_ScanLab_uint32 nDigitalOutput, const LibMCDriver_ScanLab_uint32 nOutputMask)
{
	if (nDigitalOutput > 65535)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDDIGITALOUTPUTVALUE);
	if (nOutputMask > 65535)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDDIGITALOUTPUTMASK);

	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);
	m_pScanLabSDK->n_write_io_port_mask_list(m_CardNo, nDigitalOutput, nOutputMask);
	
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

}


void CRTCContext::callSetTriggerOIE(uint32_t nPeriod)
{


	if (m_OIEOperationMode == LibMCDriver_ScanLab::eOIEOperationMode::OIEVersion3) {

		std::array<uint32_t, 8> channelArray;
		for (uint32_t nIndex = 0; nIndex < 8; nIndex++) {
			if (nIndex < m_MCBSPSignalChannels.size())
				channelArray.at(nIndex) = m_MCBSPSignalChannels.at(nIndex);
			else
				channelArray.at(nIndex) = 0;
		}

		m_pScanLabSDK->n_set_trigger8(m_CardNo, nPeriod, channelArray[0], channelArray[1], channelArray[2], channelArray[3], channelArray[4], channelArray[5], channelArray[6], channelArray[7]);
		m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);
	}
	else {

		// Compatibility with Version less than 3 final.
		m_pScanLabSDK->n_set_trigger4(m_CardNo, nPeriod, 20, 21, 1, 2);
		m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);
	}
}


void CRTCContext::EnableOIE()
{
	if (m_OIEOperationMode == LibMCDriver_ScanLab::eOIEOperationMode::OIENotInitialized)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_OIEHASNOTBEENINITIALIZED);

	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);

	sendFreeVariable0(1);

	m_pScanLabSDK->n_set_free_variable_list(m_CardNo, 1, 0);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

	callSetTriggerOIE(1);


}

void CRTCContext::DisableOIE()
{
	if (m_OIEOperationMode == LibMCDriver_ScanLab::eOIEOperationMode::OIENotInitialized)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_OIEHASNOTBEENINITIALIZED);

	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);

	callSetTriggerOIE(0);

	sendFreeVariable0(0);

	m_pScanLabSDK->n_set_free_variable_list(m_CardNo, 1, 0);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

}

INLightAFXProfileSelector* CRTCContext::CreateNLightAFXBeamProfileSelector()
{
	if (m_pNLightAFXSelectorInstance.get () == nullptr)
		m_pNLightAFXSelectorInstance = std::make_shared <CNLightAFXProfileSelectorInstance> (m_pScanLabSDK, m_CardNo);

	return new CNLightAFXProfileSelector(m_pNLightAFXSelectorInstance);
}

IGPIOSequence* CRTCContext::AddGPIOSequence(const std::string& sIdentifier)
{
	throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_NOTIMPLEMENTED);
}

IGPIOSequence* CRTCContext::FindGPIOSequence(const std::string& sIdentifier, const bool bMustExist)
{
	throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_NOTIMPLEMENTED);
}

void CRTCContext::DeleteGPIOSequence(const std::string& sIdentifier)
{

}



void CRTCContext::StartOIEMeasurement()
{
	StartOIEMeasurementEx(false);
}

void CRTCContext::StartOIEMeasurementEx(bool bTriggerOnFlag)
{
	if (m_OIEOperationMode == LibMCDriver_ScanLab::eOIEOperationMode::OIENotInitialized)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_OIEHASNOTBEENINITIALIZED);

	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);

	

	uint32_t nFreeVariable = 0;

	switch (m_OIEOperationMode) {
	case LibMCDriver_ScanLab::eOIEOperationMode::OIEVersion2:
		// Bit 8 triggers OIE Measurement Start
		sendFreeVariable0(1UL | (1UL << 8));
		break;

	case LibMCDriver_ScanLab::eOIEOperationMode::OIEVersion3Compatibility:
	case LibMCDriver_ScanLab::eOIEOperationMode::OIEVersion3:
		// Bit 4 triggers OIE Measurement Start
		nFreeVariable = 1UL | (1UL << 4);

		// Bit 5 sets LaserON Trigger flag
		if (bTriggerOnFlag)
			nFreeVariable |= (1UL << 5);

		sendFreeVariable0(nFreeVariable);
		break;
	}
	
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);


}

void CRTCContext::StopOIEMeasurement()
{
	if (m_OIEOperationMode == LibMCDriver_ScanLab::eOIEOperationMode::OIENotInitialized)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_OIEHASNOTBEENINITIALIZED);

	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);

	sendFreeVariable0(1);

}

void CRTCContext::SetOIEPIDMode(const LibMCDriver_ScanLab_uint32 nOIEPIDIndex)
{
	if (m_OIEOperationMode == LibMCDriver_ScanLab::eOIEOperationMode::OIENotInitialized)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_OIEHASNOTBEENINITIALIZED);

	if (nOIEPIDIndex > 63)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_OIEPIDVARIABLEOUTOFBOUNDS);

	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);

	// That bitmask is 0xFFFFF03F (see documentation?)
	uint32_t nCurrentVariable0 = getCurrentFreeVariable0() & (0xffffffff ^ (63UL << 6));

	sendFreeVariable0 (nCurrentVariable0 | (nOIEPIDIndex << 6));



}

void CRTCContext::ClearOIEMeasurementTags()
{
	m_MeasurementTags.clear();
}

LibMCDriver_ScanLab_uint32 CRTCContext::GetOIEMaxMeasurementTag()
{
	return (uint32_t)m_MeasurementTags.size();
}


void CRTCContext::EnableOIEMeasurementTagging()
{
	m_bMeasurementTagging = true;
}

void CRTCContext::DisableOIEMeasurementTagging()
{
	m_bMeasurementTagging = false;
}

void CRTCContext::MapOIEMeasurementTag(const LibMCDriver_ScanLab_uint32 nMeasurementTag, LibMCDriver_ScanLab_uint32& nPartID, LibMCDriver_ScanLab_uint32& nProfileID, LibMCDriver_ScanLab_uint32& nSegmentID, LibMCDriver_ScanLab_uint32& nVectorID)
{
	if ((nMeasurementTag >= 1) && (nMeasurementTag <= m_MeasurementTags.size())) {
		auto & record = m_MeasurementTags.at (nMeasurementTag - 1);
		nPartID = record.m_nCurrentPartID;
		nSegmentID = record.m_nCurrentSegmentID;
		nProfileID = record.m_nCurrentProfileID;
		nVectorID = record.m_nCurrentVectorID;

	} else
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDOIEMEASUREMENTTAG, "Invalid OIE Measurement Tag" + std::to_string (nMeasurementTag));
}


void CRTCContext::DisableSkyWriting()
{
	m_pScanLabSDK->n_set_sky_writing_mode_list (m_CardNo, 0);
}

void CRTCContext::EnableSkyWritingMode1(const LibMCDriver_ScanLab_double dTimelag, const LibMCDriver_ScanLab_int64 nLaserOnShift, const LibMCDriver_ScanLab_int64 nNPrev, const LibMCDriver_ScanLab_int64 nNPost)
{
	if (dTimelag < 0.0)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDSKYWRITINGTIMELAG);
	if ((nLaserOnShift < (int64_t)INT32_MIN) || (nLaserOnShift > (int64_t)INT32_MAX))
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDSKYWRITINGLASERONSHIFT);
	if ((nNPrev < 0) || (nNPrev > (int64_t) UINT32_MAX))
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDSKYWRITINGNPREV);
	if ((nNPost < 0) || (nNPost > (int64_t)UINT32_MAX))
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDSKYWRITINGNPOST);

	m_pScanLabSDK->n_set_sky_writing_para_list(m_CardNo, dTimelag, (int32_t)nLaserOnShift, (uint32_t)nNPrev, (uint32_t)nNPost);
	m_pScanLabSDK->n_set_sky_writing_mode_list(m_CardNo, 1);
}

void CRTCContext::EnableSkyWritingMode2(const LibMCDriver_ScanLab_double dTimelag, const LibMCDriver_ScanLab_int64 nLaserOnShift, const LibMCDriver_ScanLab_int64 nNPrev, const LibMCDriver_ScanLab_int64 nNPost)
{
	if (dTimelag < 0.0)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDSKYWRITINGTIMELAG);
	if ((nLaserOnShift < (int64_t)INT32_MIN) || (nLaserOnShift > (int64_t)INT32_MAX))
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDSKYWRITINGLASERONSHIFT);
	if ((nNPrev < 0) || (nNPrev > (int64_t) UINT32_MAX))
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDSKYWRITINGNPREV);
	if ((nNPost < 0) || (nNPost > (int64_t)UINT32_MAX))
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDSKYWRITINGNPOST);

	m_pScanLabSDK->n_set_sky_writing_para_list(m_CardNo, dTimelag, (int32_t)nLaserOnShift, (uint32_t)nNPrev, (uint32_t)nNPost);
	m_pScanLabSDK->n_set_sky_writing_mode_list(m_CardNo, 2);
}

void CRTCContext::EnableSkyWritingMode3(const LibMCDriver_ScanLab_double dTimelag, const LibMCDriver_ScanLab_int64 nLaserOnShift, const LibMCDriver_ScanLab_int64 nNPrev, const LibMCDriver_ScanLab_int64 nNPost, const LibMCDriver_ScanLab_double dLimit)
{

	//std::cout << "Enabling Skywriting mode 3: timelag " << dTimelag << " laseronshift " << nLaserOnShift << " nprev " << nNPrev << " npost " << nNPost << " limit " << dLimit << std::endl;
	if (dTimelag < 0.0)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDSKYWRITINGTIMELAG);
	if ((nLaserOnShift < (int64_t)INT32_MIN) || (nLaserOnShift > (int64_t)INT32_MAX))
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDSKYWRITINGLASERONSHIFT);
	if ((nNPrev < 0) || (nNPrev > (int64_t) UINT32_MAX))
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDSKYWRITINGNPREV);
	if ((nNPost < 0) || (nNPost > (int64_t)UINT32_MAX))
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDSKYWRITINGNPOST);

	m_pScanLabSDK->n_set_sky_writing_para_list(m_CardNo, dTimelag, (int32_t) nLaserOnShift, (uint32_t) nNPrev, (uint32_t)nNPost);
	m_pScanLabSDK->n_set_sky_writing_limit_list(m_CardNo, dLimit);
	m_pScanLabSDK->n_set_sky_writing_mode_list(m_CardNo, 3);
}

void CRTCContext::EnableSkyWritingMode4(const LibMCDriver_ScanLab_double dTimelag, const LibMCDriver_ScanLab_int64 nLaserOnShift, const LibMCDriver_ScanLab_int64 nNPrev, const LibMCDriver_ScanLab_int64 nNPost, const LibMCDriver_ScanLab_double dLimit)
{

	//std::cout << "Enabling Skywriting mode 4: timelag " << dTimelag << " laseronshift " << nLaserOnShift << " nprev " << nNPrev << " npost " << nNPost << " limit " << dLimit << std::endl;
	if (dTimelag < 0.0)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDSKYWRITINGTIMELAG);
	if ((nLaserOnShift < (int64_t)INT32_MIN) || (nLaserOnShift > (int64_t)INT32_MAX))
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDSKYWRITINGLASERONSHIFT);
	if ((nNPrev < 0) || (nNPrev > (int64_t) UINT32_MAX))
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDSKYWRITINGNPREV);
	if ((nNPost < 0) || (nNPost > (int64_t)UINT32_MAX))
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDSKYWRITINGNPOST);

	m_pScanLabSDK->n_set_sky_writing_para_list(m_CardNo, dTimelag, (int32_t)nLaserOnShift, (uint32_t)nNPrev, (uint32_t)nNPost);
	m_pScanLabSDK->n_set_sky_writing_limit_list(m_CardNo, dLimit);
	m_pScanLabSDK->n_set_sky_writing_mode_list(m_CardNo, 4);
}


void CRTCContext::SetTransformationAngle(const LibMCDriver_ScanLab_double dAngleInDegrees)
{
	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);
	m_pScanLabSDK->n_set_angle(m_CardNo, 1, dAngleInDegrees, 0);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

}

void CRTCContext::SetTransformationScale(const LibMCDriver_ScanLab_double dScaleFactor)
{
	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);
	m_pScanLabSDK->n_set_scale(m_CardNo, 1, dScaleFactor, 0);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);
}

void CRTCContext::SetTransformationOffset(const LibMCDriver_ScanLab_int32 nOffsetX, const LibMCDriver_ScanLab_int32 nOffsetY)
{
	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);
	m_pScanLabSDK->n_set_offset(m_CardNo, 1, nOffsetX, nOffsetY, 0);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);
}

void CRTCContext::SetTransformationMatrix(const LibMCDriver_ScanLab_double dM11, const LibMCDriver_ScanLab_double dM12, const LibMCDriver_ScanLab_double dM21, const LibMCDriver_ScanLab_double dM22)
{
	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);
	m_pScanLabSDK->n_set_matrix(m_CardNo, 1, dM11, dM12, dM21, dM22, 0);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);
}

IRTCRecording* CRTCContext::PrepareRecording(const bool bKeepInMemory)
{
	auto pCryptoContext = m_pDriverEnvironment->CreateCryptoContext();
	std::string sUUID = pCryptoContext->CreateUUID();
	auto pInstance = std::make_shared<CRTCRecordingInstance>(sUUID, m_pScanLabSDK, m_CardNo, m_dCorrectionFactor, RTC_CHUNKSIZE_DEFAULT);

	if (bKeepInMemory)
		m_Recordings.insert(std::make_pair (pInstance->getUUID(), pInstance));

	return new CRTCRecording(pInstance);
}

bool CRTCContext::HasRecording(const std::string& sUUID)
{
	auto iIter = m_Recordings.find(sUUID);
	return (iIter != m_Recordings.end());
}

IRTCRecording* CRTCContext::FindRecording(const std::string& sUUID)
{
	auto pCryptoContext = m_pDriverEnvironment->CreateCryptoContext();
	std::string sNormalizedUUID = pCryptoContext->NormalizeUUIDString(sUUID);

	auto iIter = m_Recordings.find(sNormalizedUUID);
	if (iIter != m_Recordings.end())
	{
		return new CRTCRecording(iIter->second);
	}
	else
	{
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_COULDNOTFINDRECORDING, "Could not find recording: " + sNormalizedUUID);
	}

}

/*void CRTCContext::PrepareRecording()
{
	bool bCheckIfScanHeadIsConnected = false;

	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);

	m_HeadTransform.resize(528520 * 4);
	
	m_pScanLabSDK->checkError(m_pScanLabSDK->n_upload_transform(m_CardNo, 1, m_HeadTransform.data()));
	 
	double CalibrationFactorXY = m_dCorrectionFactor; 

	uint32_t nHeadNoXY = 1;
	uint32_t nHeadNoZ = 2;
	uint32_t nAxisX = 1;
	uint32_t nAxisY = 2;
	uint32_t nAxisZ = 1;

	// check whether scan head is connected
	if (bCheckIfScanHeadIsConnected) {

		// TODO: Check Head Status bits
		uint32_t HeadStatus1 = m_pScanLabSDK->n_get_head_status(m_CardNo, nHeadNoXY);
		if (!HeadStatus1)
		{
			throw std::runtime_error("head status 1 invalid");
		}

		uint32_t HeadStatus2 = m_pScanLabSDK->n_get_head_status(m_CardNo, nHeadNoZ);
		if (!HeadStatus2)
		{
			throw std::runtime_error("head status 2 invalid");
		} 
	}

	uint32_t ControlCommand = 0x0501; // activates actual position recording
	//activate porition recording for each axis induvidually, x-axis = 1, y-axis = 2
	m_pScanLabSDK->n_control_command(m_CardNo, nHeadNoXY, nAxisX, ControlCommand);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

	m_pScanLabSDK->n_control_command(m_CardNo, nHeadNoXY, nAxisY, ControlCommand);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

	m_pScanLabSDK->n_control_command(m_CardNo, nHeadNoZ, nAxisZ, ControlCommand);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

	m_pScanLabSDK->n_control_command(m_CardNo, nHeadNoZ, 2, ControlCommand);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

}

void CRTCContext::EnableRecording ()
{
	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);
	//m_pScanLabSDK->n_set_trigger4(m_CardNo, 1, 1, 2, 4, 32); // record signals: 1=StatusAX (actual x position of first scanhead), 2=StatusAY (actual y position of first scanhead), 0=LaserOn
	m_pScanLabSDK->n_set_trigger4(m_CardNo, 1, 7, 8, 52, 0); // 
	//m_pScanLabSDK->n_set_trigger4(m_CardNo, 1, 10, 11, 12, 0); // 
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

}

void CRTCContext::DisableRecording()
{
	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);
	m_pScanLabSDK->n_long_delay(m_CardNo, 1200); // add long delay ~ TODO: tracking error or preview time
	m_pScanLabSDK->n_set_trigger4(m_CardNo, 0, 1, 2, 4, 32); // deactivates signal recording
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

}

constexpr uint32_t MAXMESPOSITION = (1 << 23) - 1;

uint32_t CRTCContext::saveRecordedDataBlock(std::ofstream& MyFile, uint32_t DataStart, uint32_t DataEnd, double CalibrationFactorXY)
{
	uint32_t Error = 0;
	uint32_t nDataLength = DataEnd - DataStart;

	std::cout << "Saving RTC Data Block FROM " << DataStart << " TO " << DataEnd << std::endl;

	if (nDataLength > 0) {

		std::vector<std::vector<int32_t>> ChannelData;
		std::vector<std::vector<int32_t>> ChannelDataTransformed;
		ChannelData.resize(4);

		for (int64_t Channel = 1; Channel < 5; Channel++)
		{
			ChannelData[Channel - 1].resize(nDataLength);
			m_pScanLabSDK->n_get_waveform_offset(m_CardNo, (uint32_t)Channel, DataStart, nDataLength, ChannelData[Channel - 1].data ());
			m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);
		}


		for (uint32_t nDataIndex = 0; nDataIndex < nDataLength; nDataIndex++) {
			// Apply BackTransformation XY
			m_pScanLabSDK->checkError(m_pScanLabSDK->transform(&ChannelData[0].at(nDataIndex), &ChannelData[1].at(nDataIndex), m_HeadTransform.data(), 0));

			// Apply BackTransformation Z
			int32_t nDummy = 0;
			m_pScanLabSDK->checkError(m_pScanLabSDK->transform(&ChannelData[2].at(nDataIndex), &nDummy, m_HeadTransform.data(), 1));

			MyFile << ChannelData[0][nDataIndex] << ", " << ChannelData[1][nDataIndex] << ", " << ChannelData[2][nDataIndex] << ", " << ChannelData[3][nDataIndex] << std::endl;
		}

	}
	return Error;

}

std::string return_current_time_and_date()
{
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);

	std::stringstream ss;
	ss << std::put_time(std::localtime(&in_time_t), "%Y_%m_%d_%H_%M_%S_");
	return ss.str();
}

void CRTCContext::ExecuteListWithRecording(const LibMCDriver_ScanLab_uint32 nListIndex, const LibMCDriver_ScanLab_uint32 nPosition)
{
	//std::cout << "Executing list position" << std::endl;

	m_pScanLabSDK->n_execute_list_pos(m_CardNo, nListIndex, nPosition);
	m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));

	uint32_t Busy, Position, MesBusy, MesPosition;
	uint32_t LastPosition = 0;
	uint32_t Increment = 100000;

	std::string sFileName = "c:/temp/recording_out_" + return_current_time_and_date() + ".csv";
	std::cout << "Creating recording file" << std::endl;

	std::ofstream MyFile;
	MyFile.open(sFileName, std::ios::out);
	if (!MyFile.is_open())
		throw std::runtime_error("could not create file");

	double CalibrationFactorXY = m_dCorrectionFactor;

	std::cout << "Wait for measurement to start" << std::endl;
	do // Wait for measurement to start
	{
		m_pScanLabSDK->n_measurement_status(m_CardNo, &MesBusy, &MesPosition);
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	} while ((MesBusy == 0) && (MesPosition == (uint32_t)-1));

	do   //blockwise data polling
	{

		m_pScanLabSDK->n_get_status(m_CardNo, &Busy, &Position);
		m_pScanLabSDK->n_measurement_status(m_CardNo, &MesBusy, &MesPosition);
		std::cout << "RTC Status busy: " << Busy << " Position: " << Position << " Measure Busy: " << MesBusy << " Measure Position: " << MesPosition << std::endl;

		if (MesPosition > LastPosition + Increment)
		{
			saveRecordedDataBlock(MyFile, LastPosition, LastPosition + Increment, CalibrationFactorXY);
			LastPosition += Increment;
		}
		else if (MesPosition < LastPosition)
		{
			saveRecordedDataBlock(MyFile, LastPosition, MAXMESPOSITION, CalibrationFactorXY);
			LastPosition = 0;
		}
		else {
			if (!MesBusy) {
				saveRecordedDataBlock(MyFile, LastPosition, MesPosition, CalibrationFactorXY);
				LastPosition = MesPosition;
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		m_pScanLabSDK->n_measurement_status(m_CardNo, &MesBusy, &MesPosition);

	} while (MesBusy || (MesPosition != LastPosition));           // Wait for the job to be finished executing

}  */

void CRTCContext::EnableTimelagCompensation(const LibMCDriver_ScanLab_uint32 nTimeLagXYInMicroseconds, const LibMCDriver_ScanLab_uint32 nTimeLagZInMicroseconds)
{
	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);

	if (nTimeLagXYInMicroseconds % 10 != 0)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_TIMELAGMUSTBEAMULTIPLEOF10);
	if (nTimeLagZInMicroseconds % 10 != 0)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_TIMELAGMUSTBEAMULTIPLEOF10);

	uint32_t nTimeLagXYInTicks = nTimeLagXYInMicroseconds / 10;
	uint32_t nTimeLagZInTicks = nTimeLagZInMicroseconds / 10;
	
	m_pScanLabSDK->n_set_timelag_compensation(m_CardNo, 1, nTimeLagXYInTicks, nTimeLagZInTicks);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

}

void CRTCContext::DisableTimelagCompensation()
{
	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);

	m_pScanLabSDK->n_set_timelag_compensation(m_CardNo, 1, 0, 0);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

}


void CRTCContext::SetLaserOrigin(const LibMCDriver_ScanLab_double dOriginX, const LibMCDriver_ScanLab_double dOriginY)
{
	m_dLaserOriginX = dOriginX;
	m_dLaserOriginY = dOriginY;
}

void CRTCContext::GetLaserOrigin(LibMCDriver_ScanLab_double& dOriginX, LibMCDriver_ScanLab_double& dOriginY)
{
	dOriginX = m_dLaserOriginX;
	dOriginY = m_dLaserOriginY;

}

void CRTCContext::SetLaserField(const LibMCDriver_ScanLab_double dMinX, const LibMCDriver_ScanLab_double dMinY, const LibMCDriver_ScanLab_double dMaxX, const LibMCDriver_ScanLab_double dMaxY)
{	
	//std::cout << "Setting laser field: " << dMinX << "/" << dMinY << " - " << dMaxX << "/" << dMaxY << std::endl;
	if ((dMinX >= dMaxX) || (dMinY >= dMaxY))
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDLASERFIELDCOORDINATES);

	updateLaserField(dMinX, dMaxX, dMinY, dMaxY);

	m_bHasLaserField = true;


}

bool CRTCContext::GetLaserField(LibMCDriver_ScanLab_double& dMinX, LibMCDriver_ScanLab_double& dMinY, LibMCDriver_ScanLab_double& dMaxX, LibMCDriver_ScanLab_double& dMaxY)
{
	dMinX = m_dLaserFieldMinX;
	dMaxX = m_dLaserFieldMaxX;
	dMinY = m_dLaserFieldMinY;
	dMaxY = m_dLaserFieldMaxY;
	return m_bHasLaserField;
}



void CRTCContext::ResetLaserField()
{
	m_bHasLaserField = false;
	m_dLaserFieldMinX = 0.0;
	m_dLaserFieldMaxX = 0.0;
	m_dLaserFieldMinY = 0.0;
	m_dLaserFieldMaxY = 0.0;

	clearLaserField();
}

void CRTCContext::EnableRangeChecking()
{
	if (!m_bHasLaserField)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_NOLASERFIELDSET);

	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);

	m_pScanLabSDK->n_range_checking(m_CardNo, 1, 2, 4);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

}

void CRTCContext::DisableRangeChecking()
{
	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);

	m_pScanLabSDK->n_range_checking(m_CardNo, 0, 0, 0);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

}

void CRTCContext::updateLaserField(double dMinXInMM, double dMaxXInMM, double dMinYInMM, double dMaxYInMM)
{
	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);

	int64_t nMinX = (int64_t)(round(dMinXInMM * m_dCorrectionFactor));
	int64_t nMaxX = (int64_t)(round(dMaxXInMM * m_dCorrectionFactor));
	int64_t nMinY = (int64_t)(round(dMinYInMM * m_dCorrectionFactor));
	int64_t nMaxY = (int64_t)(round(dMaxYInMM * m_dCorrectionFactor));

	/*std::cout << "updating laser field" << std::endl;
	std::cout << " minx: " << nMinX << std::endl;
	std::cout << " maxx: " << nMaxX << std::endl;
	std::cout << " miny: " << nMinY << std::endl;
	std::cout << " maxy: " << nMaxY << std::endl;*/


	if (nMinX < SCANLAB_LASERFIELD_MINIMUMUNITS)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDLASERFIELDCOORDINATES);
	if (nMinY < SCANLAB_LASERFIELD_MINIMUMUNITS)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDLASERFIELDCOORDINATES);
	if (nMaxX > SCANLAB_LASERFIELD_MAXIMUMUNITS)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDLASERFIELDCOORDINATES);
	if (nMaxY > SCANLAB_LASERFIELD_MAXIMUMUNITS)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDLASERFIELDCOORDINATES);
	if (nMaxX <= nMinX)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDLASERFIELDCOORDINATES);
	if (nMaxY <= nMinY)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDLASERFIELDCOORDINATES);

	m_pScanLabSDK->n_set_fly_limits(m_CardNo, (int32_t)nMinX, (int32_t)nMaxX, (int32_t)nMinY, (int32_t)nMaxY);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

	m_dLaserFieldMinX = dMinXInMM;
	m_dLaserFieldMaxX = dMaxXInMM;
	m_dLaserFieldMinY = dMinYInMM;
	m_dLaserFieldMaxY = dMaxYInMM;

}

void CRTCContext::clearLaserField()
{
	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);
	m_pScanLabSDK->n_set_fly_limits(m_CardNo, SCANLAB_LASERFIELD_MINIMUMUNITS, SCANLAB_LASERFIELD_MAXIMUMUNITS, SCANLAB_LASERFIELD_MINIMUMUNITS, SCANLAB_LASERFIELD_MAXIMUMUNITS);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);
}




void CRTCContext::EnableMarkOnTheFly2D(const LibMCDriver_ScanLab_double dScaleXInMMperEncoderStep, const LibMCDriver_ScanLab_double dScaleYInMMperEncoderStep)
{
	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);
	double dBitsPerMM = m_dCorrectionFactor;

	double dScaleXInBitsPerEncoderStep = dScaleXInMMperEncoderStep * dBitsPerMM;
	double dScaleYInBitsPerEncoderStep = dScaleYInMMperEncoderStep * dBitsPerMM;

	double dAbsScaleX = abs(dScaleXInBitsPerEncoderStep);
	double dAbsScaleY = abs(dScaleYInBitsPerEncoderStep);
	if ((dAbsScaleX < (1.0 / 256.0)) || (dAbsScaleX > 16000.0))
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDENCODERSCALINGINX);
	if ((dAbsScaleY < (1.0 / 256.0)) || (dAbsScaleY > 16000.0))
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDENCODERSCALINGINY);

	//std::cout << "n_set_fly_2d: card no:" << m_CardNo << " " << dScaleXInBitsPerEncoderStep << " / " << dScaleYInBitsPerEncoderStep << std::endl;

	// Reset Laser position to be sure we are in scope.
	m_pScanLabSDK->n_set_fly_2d(m_CardNo, 0.0, 0.0);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

	m_pScanLabSDK->n_jump_abs(m_CardNo, 0, 0);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);
	m_nCurrentScanPositionX = 0;
	m_nCurrentScanPositionY = 0;

	m_pScanLabSDK->n_init_fly_2d(m_CardNo, 0, 0, 0);
	m_pScanLabSDK->n_set_fly_2d(m_CardNo, dScaleXInBitsPerEncoderStep, dScaleYInBitsPerEncoderStep);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

	m_dScaleXInBitsPerEncoderStep = dScaleXInBitsPerEncoderStep;
	m_dScaleYInBitsPerEncoderStep = dScaleYInBitsPerEncoderStep;

	m_b2DMarkOnTheFlyEnabled = true;

}

void CRTCContext::DisableMarkOnTheFly2D()
{
	
	m_b2DMarkOnTheFlyEnabled = false;

	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);

	m_pScanLabSDK->n_fly_return(m_CardNo, 0, 0);
	m_nCurrentScanPositionX = 0;
	m_nCurrentScanPositionY = 0;
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);
}

void CRTCContext::EnableOIEPIDControl()
{
	m_bEnableOIEPIDControl = true;
}

void CRTCContext::DisableOIEPIDControl()
{
	m_bEnableOIEPIDControl = false;
}


bool CRTCContext::MarkOnTheFly2DIsEnabled()
{
	return m_b2DMarkOnTheFlyEnabled;
}

void CRTCContext::AddLayerToList(LibMCEnv::PToolpathLayer pLayer, bool bFailIfNonAssignedDataExists)
{

	if (pLayer.get() == nullptr)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);

	int64_t nAttributeFilterValue = 0;
	std::string sAttributeFilterNameSpace;
	std::string sAttributeFilterName;
	m_pOwnerData->getAttributeFilters(sAttributeFilterNameSpace, sAttributeFilterName, nAttributeFilterValue);
	uint32_t nAttributeFilterID = 0;
	if ((!sAttributeFilterNameSpace.empty()) && (!sAttributeFilterName.empty())) {
		nAttributeFilterID = pLayer->FindCustomSegmentAttributeID(sAttributeFilterNameSpace, sAttributeFilterName);
	}

	LibMCDriver_ScanLab::eOIERecordingMode oieRecordingMode = m_pOwnerData->getOIERecordingMode ();
	double dMaxLaserPowerInWatts = m_pOwnerData->getMaxLaserPower();

	addLayerToListEx(pLayer, oieRecordingMode, nAttributeFilterID, nAttributeFilterValue, (float)dMaxLaserPowerInWatts, bFailIfNonAssignedDataExists);
}


void CRTCContext::addLayerToListEx(LibMCEnv::PToolpathLayer pLayer, eOIERecordingMode oieRecordingMode, uint32_t nAttributeFilterID, int64_t nAttributeFilterValue, float fMaxLaserPowerInWatts, bool bFailIfNonAssignedDataExists)
{
	if (pLayer.get() == nullptr)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);

	double dUnits = pLayer->GetUnits();

	switch (oieRecordingMode) {
	case eOIERecordingMode::OIEEnableAndContinuousMeasurement:
	case eOIERecordingMode::OIEEnableAndLaserActiveMeasurement:
		EnableOIE();
		break;
	}

	switch (oieRecordingMode) {
	case eOIERecordingMode::OIEContinuousMeasurement:
	case eOIERecordingMode::OIEEnableAndContinuousMeasurement:
		StartOIEMeasurementEx (false);
		break;
	case eOIERecordingMode::OIELaserActiveMeasurement:
	case eOIERecordingMode::OIEEnableAndLaserActiveMeasurement:
		StartOIEMeasurementEx(true);
		break;
	}

	if (m_bEnableOIEPIDControl) {
		SetOIEPIDMode(0);
	}

	uint32_t nSegmentCount = pLayer->GetSegmentCount();
	for (uint32_t nSegmentIndex = 0; nSegmentIndex < nSegmentCount; nSegmentIndex++) {

		m_CurrentMeasurementTagInfo.m_nCurrentSegmentID = (uint32_t) (nSegmentIndex + 1);
		m_CurrentMeasurementTagInfo.m_nCurrentProfileID = (uint32_t) pLayer->GetSegmentProfileIntegerValueDef(nSegmentIndex, "http://schemas.scanlab.com/oie/2023/08", "measurementid", 0);
		m_CurrentMeasurementTagInfo.m_nCurrentPartID = (uint32_t)pLayer->GetSegmentLocalPartID(nSegmentIndex);

		LibMCEnv::eToolpathSegmentType eSegmentType;
		uint32_t nPointCount;
		pLayer->GetSegmentInfo(nSegmentIndex, eSegmentType, nPointCount);
		
		bool bDrawSegment = true;
		if (nAttributeFilterID != 0) {
			int64_t segmentAttributeValue = pLayer->GetSegmentIntegerAttribute(nSegmentIndex, nAttributeFilterID);
			bDrawSegment = (segmentAttributeValue == nAttributeFilterValue);
		}

		if (bDrawSegment && (nPointCount >= 2)) {

			// Update nLight AFX Mode if necessary
			if (m_pNLightAFXSelectorInstance.get() != nullptr) {
				if (m_pNLightAFXSelectorInstance->isEnabled()) {
					int64_t nLightAFXMode = pLayer->GetSegmentProfileIntegerValueDef(nSegmentIndex, "http://schemas.nlight.com/afx/2024/09", "afxmode", 0);
					if (nLightAFXMode < 0) 
						throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDNLIGHTAFXMODE, "Invalid nLightAFXMode: " + std::to_string(nLightAFXMode));
					if (nLightAFXMode > (int64_t) m_pNLightAFXSelectorInstance->getMaxAFXMode ())
						throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDNLIGHTAFXMODE, "Invalid nLightAFXMode: " + std::to_string(nLightAFXMode));

					m_pNLightAFXSelectorInstance->selectAFXModeIfNecessary((uint32_t)nLightAFXMode);

				}
			}


			float fJumpSpeedInMMPerSecond = (float)pLayer->GetSegmentProfileTypedValue(nSegmentIndex, LibMCEnv::eToolpathProfileValueType::JumpSpeed);
			float fMarkSpeedInMMPerSecond = (float)pLayer->GetSegmentProfileTypedValue(nSegmentIndex, LibMCEnv::eToolpathProfileValueType::Speed);
			float fPowerInWatts = (float)pLayer->GetSegmentProfileTypedValue(nSegmentIndex, LibMCEnv::eToolpathProfileValueType::LaserPower);
			float fPowerInPercent = (fPowerInWatts * 100.f) / fMaxLaserPowerInWatts;
			float fLaserFocus = (float)pLayer->GetSegmentProfileTypedValue(nSegmentIndex, LibMCEnv::eToolpathProfileValueType::LaserFocus);

			uint32_t nOIEPIDControlIndex = 0;
			if (m_bEnableOIEPIDControl) {
				nOIEPIDControlIndex = (uint32_t) pLayer->GetSegmentProfileIntegerValueDef(nSegmentIndex, "http://schemas.scanlab.com/oie/2023/08", "pidindex", 0);
			}

			int64_t nLaserIndexToDraw = pLayer->GetSegmentProfileIntegerValueDef(nSegmentIndex, "", "laserindex", 0);
			int64_t nCurrentLaserIndex = GetLaserIndex();

			if (nLaserIndexToDraw == 0) {
				if (bFailIfNonAssignedDataExists)
					throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_LASERINDEXHASNOASSIGNEDSCANNER, "Laser index has no assigned scanner: " + std::to_string(nLaserIndexToDraw));
			}

			if (nLaserIndexToDraw == nCurrentLaserIndex) {

				int64_t nSkywritingMode = pLayer->GetSegmentProfileIntegerValueDef(nSegmentIndex, "http://schemas.scanlab.com/skywriting/2023/01", "mode", 0);

				if (nSkywritingMode != 0) {
					double dSkywritingTimeLag = pLayer->GetSegmentProfileDoubleValue(nSegmentIndex, "http://schemas.scanlab.com/skywriting/2023/01", "timelag");
					int64_t nSkywritingLaserOnShift = pLayer->GetSegmentProfileIntegerValue(nSegmentIndex, "http://schemas.scanlab.com/skywriting/2023/01", "laseronshift");
					int64_t nSkywritingPrev = pLayer->GetSegmentProfileIntegerValue(nSegmentIndex, "http://schemas.scanlab.com/skywriting/2023/01", "nprev");
					int64_t nSkywritingPost = pLayer->GetSegmentProfileIntegerValue(nSegmentIndex, "http://schemas.scanlab.com/skywriting/2023/01", "npost");

					double dSkywritingLimit = 0.0;
					if ((nSkywritingMode == 3) || (nSkywritingMode == 4)) {
						dSkywritingLimit = pLayer->GetSegmentProfileDoubleValue(nSegmentIndex, "http://schemas.scanlab.com/skywriting/2023/01", "limit");
					}


					switch (nSkywritingMode) {
					case 1:
						EnableSkyWritingMode1(dSkywritingTimeLag, nSkywritingLaserOnShift, nSkywritingPrev, nSkywritingPost);
						break;
					case 2:
						EnableSkyWritingMode2(dSkywritingTimeLag, nSkywritingLaserOnShift, nSkywritingPrev, nSkywritingPost);
						break;
					case 3:
						EnableSkyWritingMode3(dSkywritingTimeLag, nSkywritingLaserOnShift, nSkywritingPrev, nSkywritingPost, dSkywritingLimit);
						break;
					case 4:
						EnableSkyWritingMode4(dSkywritingTimeLag, nSkywritingLaserOnShift, nSkywritingPrev, nSkywritingPost, dSkywritingLimit);
						break;
					default:
						DisableSkyWriting();
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

					DrawPolylineOIE(nPointCount, ContourPoints.data(), fMarkSpeedInMMPerSecond, fJumpSpeedInMMPerSecond, fPowerInPercent, fLaserFocus, nOIEPIDControlIndex);

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

					DrawHatchesOIE(Hatches.size(), Hatches.data(), fMarkSpeedInMMPerSecond, fJumpSpeedInMMPerSecond, fPowerInPercent, fLaserFocus, nOIEPIDControlIndex);

					break;
				}

				}
			}

		}

	}

	if (m_bEnableOIEPIDControl) {
		SetOIEPIDMode(0);
	}

	if ((oieRecordingMode != eOIERecordingMode::OIERecordingDisabled))
		StopOIEMeasurement();

	// Disable AFX Mode
	if (m_pNLightAFXSelectorInstance.get() != nullptr) {
		if (m_pNLightAFXSelectorInstance->isEnabled())
			m_pNLightAFXSelectorInstance->selectAFXModeIfNecessary (0);
	}

	switch (oieRecordingMode) {
	case eOIERecordingMode::OIEEnableAndContinuousMeasurement:
	case eOIERecordingMode::OIEEnableAndLaserActiveMeasurement:
		DisableOIE();
		break;
	}

}

void CRTCContext::WaitForEncoderX(const LibMCDriver_ScanLab_double dPositionInMM, const bool bInPositiveHalfPlane)
{
	if (!m_b2DMarkOnTheFlyEnabled)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_MARKONTHEFLYISDISABLED);
	if (m_dScaleXInBitsPerEncoderStep <= 0.0)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_MARKONTHEFLYISDISABLED);

	double dBitsPerMM = m_dCorrectionFactor;
	double dPositionInBits = dPositionInMM * dBitsPerMM;

	double dEncoderSteps = dPositionInBits / m_dScaleXInBitsPerEncoderStep;
	int32_t nEncoderSteps = (int32_t)round(dEncoderSteps);

	if (m_dScaleXInBitsPerEncoderStep > 0) {
		WaitForEncoderXSteps(nEncoderSteps, bInPositiveHalfPlane);
	} 
	else {
		// If scaling is negative, positive half plane changes
		WaitForEncoderXSteps(nEncoderSteps, !bInPositiveHalfPlane);
	}
}

void CRTCContext::WaitForEncoderY(const LibMCDriver_ScanLab_double dPositionInMM, const bool bInPositiveHalfPlane)
{
	if (!m_b2DMarkOnTheFlyEnabled)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_MARKONTHEFLYISDISABLED);
	if (m_dScaleYInBitsPerEncoderStep <= 0.0)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_MARKONTHEFLYISDISABLED);

	double dBitsPerMM = m_dCorrectionFactor;
	double dPositionInBits = dPositionInMM * dBitsPerMM;

	double dEncoderSteps = dPositionInBits / m_dScaleYInBitsPerEncoderStep;

	int32_t nEncoderSteps = (int32_t)round(dEncoderSteps);

	if (m_dScaleYInBitsPerEncoderStep > 0) {
		WaitForEncoderYSteps(nEncoderSteps, bInPositiveHalfPlane);
	}
	else {
		// If scaling is negative, positive half plane changes sign too
		WaitForEncoderYSteps(nEncoderSteps, !bInPositiveHalfPlane);
	}

}

void CRTCContext::WaitForEncoderXSteps(const LibMCDriver_ScanLab_int32 nPositionValue, const bool bInPositiveHalfPlane)
{
	int32_t nMode;
	if (bInPositiveHalfPlane)
		nMode = 1;
	else
		nMode = -1;

	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);
	m_pScanLabSDK->n_wait_for_encoder_mode(m_CardNo, nPositionValue, 0, nMode);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

}

void CRTCContext::WaitForEncoderYSteps(const LibMCDriver_ScanLab_int32 nPositionValue, const bool bInPositiveHalfPlane)
{
	int32_t nMode;
	if (bInPositiveHalfPlane)
		nMode = 1;
	else
		nMode = -1;

	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);
	m_pScanLabSDK->n_wait_for_encoder_mode(m_CardNo, nPositionValue, 1, nMode);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

}


void CRTCContext::Get2DMarkOnTheFlyPosition(LibMCDriver_ScanLab_int32& nPositionX, LibMCDriver_ScanLab_int32& nPositionY)
{
	nPositionX = 0;
	nPositionY = 0;

	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);
	m_pScanLabSDK->n_get_encoder(m_CardNo, &nPositionX, &nPositionY);

	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);
	

}

LibMCDriver_ScanLab_uint32 CRTCContext::CheckOnTheFlyError(const bool bFailIfError)
{
	if (m_b2DMarkOnTheFlyEnabled) {
		uint32_t nMarkingInfo = m_pScanLabSDK->n_get_marking_info(m_CardNo);

		if (bFailIfError) {
			if (nMarkingInfo != 0)
				throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_ONTHEFLYMARKINGERROR, "Scanlab on the fly marking error: " + std::to_string(nMarkingInfo));
		}

		return nMarkingInfo;
	}
	else
		return 0;
}

LibMCDriver_ScanLab_int32 CRTCContext::ReadMultiMCBSP(const LibMCDriver_ScanLab_uint32 nRegisterNo)
{
	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);
	int32_t nRegisterContent = m_pScanLabSDK->n_read_multi_mcbsp(m_CardNo, nRegisterNo);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

	return nRegisterContent;
}

IUARTConnection* CRTCContext::CreateUARTConnection(const LibMCDriver_ScanLab_uint32 nDesiredBaudRate)
{
	return new CUARTConnection(m_pScanLabSDK, nDesiredBaudRate, m_CardNo, m_pDriverEnvironment);
}


void CRTCContext::EnableScanAhead(const LibMCDriver_ScanLab_uint32 nHeadNo, const LibMCDriver_ScanLab_uint32 nTableNo)
{
	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);
	uint32_t nErrorCode = m_pScanLabSDK->n_set_scanahead_params(m_CardNo, 1, nHeadNo, nTableNo, 0, 0, 0.0);
	if (nErrorCode) {

		// See RTC documentation for detailed information about the error codes
		if (nErrorCode == 1)
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_NOSCANAHEADOPTION);
		if (nErrorCode == 3)
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_NOEXCELLISCAN);
		if (nErrorCode == 5)
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_SCANAHEADLISTISACTIVE);
		if (nErrorCode == 6)
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_SCANAHEADPARAMETERERROR);
		if (nErrorCode == 7)
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_SCANAHEADSCALINGERROR);
		if (nErrorCode == 8)
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_SCANAHEADMISSINGRTCRESPONSE);
		if (nErrorCode == 11)
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_SCANAHEADPCIERROR);

		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_SCANAHEADUNKNOWNERROR, "Scanahead unknown error: " + std::to_string(nErrorCode));
	}

	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);


}

void CRTCContext::DisableScanAhead()
{
	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);
	uint32_t nErrorCode = m_pScanLabSDK->n_set_scanahead_params(m_CardNo, 0, 0, 0, 0, 0, 0.0);
	if (nErrorCode) {

		// See RTC documentation for detailed information about the error codes
		if (nErrorCode == 1)
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_NOSCANAHEADOPTION);
		if (nErrorCode == 3)
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_NOEXCELLISCAN);
		if (nErrorCode == 5)
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_SCANAHEADLISTISACTIVE);
		if (nErrorCode == 6)
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_SCANAHEADPARAMETERERROR);
		if (nErrorCode == 7)
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_SCANAHEADSCALINGERROR);
		if (nErrorCode == 8)
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_SCANAHEADMISSINGRTCRESPONSE);
		if (nErrorCode == 11)
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_SCANAHEADPCIERROR);

		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_SCANAHEADUNKNOWNERROR, "Scanahead unknown error: " + std::to_string(nErrorCode));
	}

	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

}

void CRTCContext::ActivateScanAheadAutoDelays()
{
	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);
	m_pScanLabSDK->n_activate_scanahead_autodelays(m_CardNo, 1);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);
}

void CRTCContext::DeactivateScanAheadAutoDelays()
{
	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);
	m_pScanLabSDK->n_activate_scanahead_autodelays(m_CardNo, 0);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);
}

bool CRTCContext::ScanAheadAutoDelaysAreActivated()
{
	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);
	int32_t nAutoDelayMode = m_pScanLabSDK->n_activate_scanahead_autodelays(m_CardNo, -1);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

	return (nAutoDelayMode == 1);

}

void CRTCContext::SetScanAheadLaserShiftsInMicroseconds(const LibMCDriver_ScanLab_double dLaserOnShiftInMicroSeconds, const LibMCDriver_ScanLab_double dLaserOffShiftInMicroSeconds)
{
	SetScanAheadLaserShiftsInUnits((int32_t)round(dLaserOnShiftInMicroSeconds * 64.0), (int32_t)round(dLaserOffShiftInMicroSeconds * 64.0));
}

void CRTCContext::SetScanAheadLaserShiftsInUnits(const LibMCDriver_ScanLab_int32 nLaserOnShift, const LibMCDriver_ScanLab_int32 nLaserOffShift)
{
	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);
	m_pScanLabSDK->n_set_scanahead_laser_shifts (m_CardNo, nLaserOnShift, nLaserOffShift);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

}

void CRTCContext::SetScanAheadLineParameters(const LibMCDriver_ScanLab_uint32 nCornerScale, const LibMCDriver_ScanLab_uint32 nEndScale, const LibMCDriver_ScanLab_uint32 nAccelerationScale)
{
	uint32_t nCappedCornerScale = nCornerScale;
	if (nCappedCornerScale > 100)
		nCappedCornerScale = 100;
	uint32_t nCappedEndScale = nEndScale;
	if (nCappedEndScale > 100)
		nCappedEndScale = 100;
	uint32_t nCappedAccScale = nAccelerationScale;
	if (nCappedAccScale > 100)
		nCappedAccScale = 100;


	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);
	m_pScanLabSDK->n_set_scanahead_line_params(m_CardNo, nCappedCornerScale, nCappedEndScale, nCappedAccScale);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

}

LibMCDriver_ScanLab_int32 CRTCContext::GetRTCChannel(const LibMCDriver_ScanLab::eRTCChannelType eChannelType)
{
	uint32_t nInternalSignalID = (uint32_t)eChannelType;
	if ((nInternalSignalID == 0) || (nInternalSignalID >= (uint32_t)LibMCDriver_ScanLab::eRTCChannelType::ChannelLaserOn))
		throw ELibMCDriver_ScanLabInterfaceException (LIBMCDRIVER_SCANLAB_ERROR_INVALIDCHANNELTYPE);

	return m_pScanLabSDK->n_get_value(m_CardNo, nInternalSignalID);

}

LibMCDriver_ScanLab_int32 CRTCContext::GetRTCInternalValue(const LibMCDriver_ScanLab_uint32 nInternalSignalID)
{
	return m_pScanLabSDK->n_get_value(m_CardNo, nInternalSignalID);
}

