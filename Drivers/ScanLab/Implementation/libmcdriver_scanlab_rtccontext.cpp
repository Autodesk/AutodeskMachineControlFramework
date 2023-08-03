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

// Include custom headers here.
#include <math.h>
#include <iostream>
#include <thread>
#include <fstream>

#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <string>

using namespace LibMCDriver_ScanLab::Impl;

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

void CRTCContextOwnerData::getExposureParameters(double& dMaxLaserPowerInWatts, LibMCDriver_ScanLab::eOIERecordingMode& oieRecordingMode)
{
	dMaxLaserPowerInWatts = dMaxLaserPowerInWatts;
	oieRecordingMode = m_OIERecordingMode;
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
	m_bHasLaserField (false),
	m_b2DMarkOnTheFlyEnabled (false),
	m_dScaleXInBitsPerEncoderStep (1.0),
	m_dScaleYInBitsPerEncoderStep (1.0)

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

}

CRTCContext::~CRTCContext()
{
	if (m_pScanLabSDK.get() != nullptr) {
		m_pScanLabSDK->release_rtc(m_CardNo);
		m_CardNo = 0;
	}
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
	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);

	auto pWorkingDirectory = m_pDriverEnvironment->CreateWorkingDirectory();

	std::string sFirmwareName;
	if (m_bIsNetwork) {
		sFirmwareName = "RTC6ETH.out";
	}
	else {
		sFirmwareName = "RTC6OUT.out";
	}

	auto pFirmwareFile = pWorkingDirectory->StoreCustomData (sFirmwareName, LibMCEnv::CInputVector<uint8_t> (pFirmwareDataBuffer, nFirmwareDataBufferSize));
	auto pFPGAFile = pWorkingDirectory->StoreCustomData("RTC6RBF.rbf", LibMCEnv::CInputVector<uint8_t>(pFPGADataBuffer, nFPGADataBufferSize));
	auto pAuxiliaryFile = pWorkingDirectory->StoreCustomData("RTC6DAT.dat", LibMCEnv::CInputVector<uint8_t>(pAuxiliaryDataBuffer, nAuxiliaryDataBufferSize));

	// TODO: Convert to ANSI
	uint32_t nErrorCode = m_pScanLabSDK->n_load_program_file (m_CardNo, pWorkingDirectory->GetAbsoluteFilePath().c_str ());

	pFirmwareFile = nullptr;
	pFPGAFile = nullptr;
	pAuxiliaryFile = nullptr;

	pWorkingDirectory->CleanUp();

	if (nErrorCode != 0)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_COULDNOTLOADPROGRAMFILE, "could not load program file: #" + std::to_string (nErrorCode));

	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);

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
	m_pScanLabSDK->n_set_laser_pulses(m_CardNo, nHalfPeriod, nPulseLength);
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


void CRTCContext::writeSpeeds(const LibMCDriver_ScanLab_single fMarkSpeed, const LibMCDriver_ScanLab_single fJumpSpeed, const LibMCDriver_ScanLab_single fPower, bool bOIEPIDControlFlag)
{

	double dBitsPerMM = m_dCorrectionFactor;

	double dMarkSpeedInMMPerMilliSecond = (double)fMarkSpeed / 1000.0;
	double dMarkSpeedInBits = dMarkSpeedInMMPerMilliSecond * dBitsPerMM;
	m_pScanLabSDK->n_set_mark_speed(m_CardNo, dMarkSpeedInBits);
	m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));

	double dJumpSpeedInMMPerMilliSecond = (double)fJumpSpeed / 1000.0;
	double dJumpSpeedInBits = dJumpSpeedInMMPerMilliSecond * dBitsPerMM;
	m_pScanLabSDK->n_set_jump_speed(m_CardNo, dJumpSpeedInBits);
	m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));

	double fClippedPowerFactor = fPower / 100.0f;
	if (fClippedPowerFactor > 1.0f)
		fClippedPowerFactor = 1.0f;
	if (fClippedPowerFactor < 0.0f)
		fClippedPowerFactor = 0.0f;

	int digitalPowerValue = 0;
	

	if (bOIEPIDControlFlag) {

		switch (m_LaserPort) {
		case eLaserPort::Port16bitDigital:
			digitalPowerValue = (int)round(fClippedPowerFactor * 65535.0);
			//nPortIndex = 6;  See set_auto_laser_control in SDK documentation
			m_pScanLabSDK->n_set_multi_mcbsp_in_list(m_CardNo, 6, digitalPowerValue, 1);
			break;
		case eLaserPort::Port8bitDigital:
			digitalPowerValue = (int)round(fClippedPowerFactor * 255.0);
			//nPortIndex = 3;  See set_auto_laser_control in SDK documentation
			m_pScanLabSDK->n_set_multi_mcbsp_in_list(m_CardNo, 3, digitalPowerValue, 1);
			break;
		case eLaserPort::Port12BitAnalog1:
			digitalPowerValue = (int)round(fClippedPowerFactor * 4095.0);
			//nPortIndex = 1; // See set_auto_laser_control in SDK documentation
			m_pScanLabSDK->n_set_multi_mcbsp_in_list(m_CardNo, 1, digitalPowerValue, 1);
			break;
		case eLaserPort::Port12BitAnalog2:
			digitalPowerValue = (int)round(fClippedPowerFactor * 4095.0);
			//nPortIndex = 2; // See set_auto_laser_control in SDK documentation
			m_pScanLabSDK->n_set_multi_mcbsp_in_list(m_CardNo, 2, digitalPowerValue, 1);
			break;
		case eLaserPort::Port12BitAnalog1andAnalog2:
			digitalPowerValue = (int)round(fClippedPowerFactor * 4095.0);
			m_pScanLabSDK->n_set_multi_mcbsp_in_list(m_CardNo, 1, digitalPowerValue, 1);
			m_pScanLabSDK->n_set_multi_mcbsp_in_list(m_CardNo, 2, digitalPowerValue, 1);
			break;
		}
	
		// See documentation what 1 means.
		m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);


	}
	else {

		switch (m_LaserPort) {
		case eLaserPort::Port16bitDigital:
			digitalPowerValue = (int)round(fClippedPowerFactor * 65535.0);
			//nPortIndex = 3;  See set_laser_power in SDK documentation
			m_pScanLabSDK->n_set_laser_power(m_CardNo, 3, digitalPowerValue);

			break;
		case eLaserPort::Port8bitDigital:
			digitalPowerValue = (int)round(fClippedPowerFactor * 255.0);
			//nPortIndex = 2; See set_laser_power in SDK documentation
			m_pScanLabSDK->n_set_laser_power(m_CardNo, 2, digitalPowerValue);
			break;
		case eLaserPort::Port12BitAnalog1:
			digitalPowerValue = (int)round(fClippedPowerFactor * 4095.0);
			//nPortIndex = 0;  See set_laser_power in SDK documentation
			m_pScanLabSDK->n_set_laser_power(m_CardNo, 0, digitalPowerValue);
			break;
		case eLaserPort::Port12BitAnalog2:
			digitalPowerValue = (int)round(fClippedPowerFactor * 4095.0);
			//nPortIndex = 1; // See set_laser_power in SDK documentation
			m_pScanLabSDK->n_set_laser_power(m_CardNo, 1, digitalPowerValue);
			break;
		case eLaserPort::Port12BitAnalog1andAnalog2:
			digitalPowerValue = (int)round(fClippedPowerFactor * 4095.0);
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

	writeSpeeds(fMarkSpeed, fJumpSpeed, fPower, nOIEPIDControlIndex != 0);

	// Z Plane
	double defocusZ = round(fZValue * m_dZCorrectionFactor);
	int intDefocusZ = (int)defocusZ;
	m_pScanLabSDK->n_set_defocus_list (m_CardNo, intDefocusZ);

	const sPoint2D* pPoint = pPointsBuffer;
	double dX = round((pPoint->m_X - m_dLaserOriginX) * m_dCorrectionFactor);
	double dY = round((pPoint->m_Y - m_dLaserOriginY) * m_dCorrectionFactor);
	pPoint++;

	int intX = (int)dX;
	int intY = (int)dY;

	m_pScanLabSDK->n_jump_abs(m_CardNo, intX, intY);
	m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));

	for (uint64_t index = 1; index < nPointsBufferSize; index++) {
		dX = round((pPoint->m_X - m_dLaserOriginX) * m_dCorrectionFactor);
		dY = round((pPoint->m_Y - m_dLaserOriginY) * m_dCorrectionFactor);
		pPoint++;

		intX = (int)dX;
		intY = (int)dY;

		m_pScanLabSDK->n_mark_abs(m_CardNo, intX, intY);
		m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));

	}

}

void CRTCContext::DrawHatchesOIE(const LibMCDriver_ScanLab_uint64 nHatchesBufferSize, const LibMCDriver_ScanLab::sHatch2D* pHatchesBuffer, const LibMCDriver_ScanLab_single fMarkSpeed, const LibMCDriver_ScanLab_single fJumpSpeed, const LibMCDriver_ScanLab_single fPower, const LibMCDriver_ScanLab_single fZValue, const uint32_t nOIEPIDControlIndex)
{
	if (!pHatchesBuffer)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);

	if (nHatchesBufferSize == 0)
		return;

	writeSpeeds(fMarkSpeed, fJumpSpeed, fPower, (nOIEPIDControlIndex != 0));

	// Z Plane
	double defocusZ = round(fZValue * m_dZCorrectionFactor);
	int intDefocusZ = (int)defocusZ;
	m_pScanLabSDK->n_set_defocus_list(m_CardNo, intDefocusZ);

	const sHatch2D* pHatch = pHatchesBuffer;

	for (uint64_t index = 0; index < nHatchesBufferSize; index++) {
		double dX = round((pHatch->m_X1 - m_dLaserOriginX) * m_dCorrectionFactor);
		double dY = round((pHatch->m_Y1 - m_dLaserOriginY) * m_dCorrectionFactor);
	
		int intX = (int)dX;
		int intY = (int)dY;
		m_pScanLabSDK->n_jump_abs(m_CardNo, intX, intY);
		m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));

		dX = round((pHatch->m_X2 - m_dLaserOriginX) * m_dCorrectionFactor);
		dY = round((pHatch->m_Y2 - m_dLaserOriginY) * m_dCorrectionFactor);
		
		intX = (int)dX;
		intY = (int)dY;
		m_pScanLabSDK->n_mark_abs(m_CardNo, intX, intY);
		m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));

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
	double dX = round((dTargetX - m_dLaserOriginX) * m_dCorrectionFactor);
	double dY = round((dTargetY - m_dLaserOriginY) * m_dCorrectionFactor);

	int32_t intX = (int32_t)dX;
	int32_t intY = (int32_t)dY;

	m_pScanLabSDK->n_jump_abs(m_CardNo, intX, intY);
	m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));

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
	double dOldInitialTimeout = 0.0;
	double dOldMaxTimeout = 0.0;
	double dOldMultiplier = 0.0;
	uint32_t nOldMode = 0;
	m_pScanLabSDK->n_eth_get_com_timeouts_auto(m_CardNo, &dOldInitialTimeout, &dOldMaxTimeout, &dOldMultiplier, &nOldMode);
	m_pScanLabSDK->n_eth_set_com_timeouts_auto(m_CardNo, dInitialTimeout, dMaxTimeout, dMultiplier, nOldMode);
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

	// Set signal channels
	uint32_t nTransferLaserOnFlag = (1UL << 31);
	m_pScanLabSDK->n_set_mcbsp_out_ptr(m_CardNo, (uint32_t)m_MCBSPSignalChannels.size () | nTransferLaserOnFlag, m_MCBSPSignalChannels.data ());
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

	// No PID control for now
	m_pScanLabSDK->n_set_multi_mcbsp_in(m_CardNo, 0, 0, 0);

	// Check Error
	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);

}


uint32_t CRTCContext::getCurrentFreeVariable0()
{
	return m_nCurrentFreeVariable0;
}


void CRTCContext::sendFreeVariable0(uint32_t nValue)
{
	m_pScanLabSDK->n_long_delay(m_CardNo, (uint32_t)m_MCBSPSignalChannels.size());
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

	m_pScanLabSDK->n_set_free_variable_list(m_CardNo, 0, nValue);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

	m_nCurrentFreeVariable0 = nValue;
}

void CRTCContext::EnableOIE()
{
	if (m_OIEOperationMode == LibMCDriver_ScanLab::eOIEOperationMode::OIENotInitialized)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_OIEHASNOTBEENINITIALIZED);

	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);

	sendFreeVariable0(1);

	m_pScanLabSDK->n_set_trigger4(m_CardNo, 1, 20, 21, 1, 2);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

}

void CRTCContext::DisableOIE()
{
	if (m_OIEOperationMode == LibMCDriver_ScanLab::eOIEOperationMode::OIENotInitialized)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_OIEHASNOTBEENINITIALIZED);

	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);

	m_pScanLabSDK->n_set_trigger4(m_CardNo, 0, 20, 21, 1, 2);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

	sendFreeVariable0(0);

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

	

	m_pScanLabSDK->n_list_nop(m_CardNo);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);
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

	m_pScanLabSDK->n_long_delay(m_CardNo, (uint32_t)m_MCBSPSignalChannels.size() + 2);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);
	
	//m_pScanLabSDK->n_set_free_variable_list(m_CardNo, 1, 1234);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

	m_pScanLabSDK->n_long_delay(m_CardNo, (uint32_t)m_MCBSPSignalChannels.size() + 2);
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

	// Check: is that 0xFFFFF03F
	uint32_t nCurrentVariable0 = getCurrentFreeVariable0() & (0xffffffff ^ (63UL << 6));

	sendFreeVariable0 (nCurrentVariable0 | (nOIEPIDIndex << 6));



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


void CRTCContext::PrepareRecording()
{
	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);

	m_HeadTransform.resize(528520 * 4);
	
	m_pScanLabSDK->checkError(m_pScanLabSDK->n_upload_transform(m_CardNo, 1, m_HeadTransform.data()));
	 
	double CalibrationFactorXY = 10000;  // get_table_para(1,1)

	uint32_t nHeadNoXY = 1;
	uint32_t nHeadNoZ = 2;
	uint32_t nAxisX = 1;
	uint32_t nAxisY = 2;
	uint32_t nAxisZ = 2;

	// check whether scan head is connected
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

	uint32_t ControlCommand = 0x0501; // activates actual position recording
	//activate porition recording for each axis induvidually, x-axis = 1, y-axis = 2
	m_pScanLabSDK->n_control_command(m_CardNo, nHeadNoXY, nAxisX, ControlCommand);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

	m_pScanLabSDK->n_control_command(m_CardNo, nHeadNoXY, nAxisY, ControlCommand);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

	m_pScanLabSDK->n_control_command(m_CardNo, 2, 1, ControlCommand);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

	m_pScanLabSDK->n_control_command(m_CardNo, 2, 2, ControlCommand);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

}

void CRTCContext::EnableRecording ()
{
	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);
	m_pScanLabSDK->n_set_trigger4(m_CardNo, 1, 1, 2, 4, 32); // record signals: 1=StatusAX (actual x position of first scanhead), 2=StatusAY (actual y position of first scanhead), 0=LaserOn
	//m_pScanLabSDK->n_set_trigger4(m_CardNo, 1, 7, 8, 9, 0); // 
	//m_pScanLabSDK->n_set_trigger4(m_CardNo, 1, 10, 11, 12, 0); // 
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

}

void CRTCContext::DisableRecording()
{
	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);
	m_pScanLabSDK->n_long_delay(m_CardNo, 1200); // add long delay ~ tracking error or preview time
	m_pScanLabSDK->n_set_trigger4(m_CardNo, 0, 1, 2, 4, 32); // deactivates signal recording
	//m_pScanLabSDK->n_set_trigger4(m_CardNo, 0, 7, 8, 9, 0); // deactivates signal recording
	//m_pScanLabSDK->n_set_trigger4(m_CardNo, 1, 10, 11, 12, 0); // deactivates signal recording
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

}

constexpr uint32_t MAXMESPOSITION = (1 << 23) - 1;

uint32_t CRTCContext::saveRecordedDataBlock(std::ofstream& MyFile, uint32_t DataStart, uint32_t DataEnd, double CalibrationFactorXY)
{
	uint32_t Error = 0;
	uint32_t nDataLength = DataEnd - DataStart;

	std::cout << "Saving RTC Data Block (" << nDataLength << " bytes" << std::endl;

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
	std::cout << "Executing list position" << std::endl;

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

	double CalibrationFactorXY = 10000;  // get_table_para(1,1)

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
		}
		else if (MesPosition < LastPosition)
		{
			saveRecordedDataBlock(MyFile, LastPosition, MAXMESPOSITION, CalibrationFactorXY);
			LastPosition = 0;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));

	} while (Busy);           // Wait for the job to be finished executing

}

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
	m_pScanLabSDK->n_jump_abs(m_CardNo, 0, 0);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

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
	m_pScanLabSDK->n_set_fly_2d(m_CardNo, 0.0, 0.0);

	m_pScanLabSDK->n_jump_abs(m_CardNo, 0, 0);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);
}


bool CRTCContext::MarkOnTheFly2DIsEnabled()
{
	return m_b2DMarkOnTheFlyEnabled;
}

void CRTCContext::AddLayerToList(LibMCEnv::PToolpathLayer pLayer, bool bFailIfNonAssignedDataExists)
{

	if (pLayer.get() == nullptr)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);

	LibMCDriver_ScanLab::eOIERecordingMode oieRecordingMode = LibMCDriver_ScanLab::eOIERecordingMode::OIERecordingDisabled;
	double dMaxLaserPowerInWatts = 100.0;

	int64_t nAttributeFilterValue = 0;
	std::string sAttributeFilterNameSpace;
	std::string sAttributeFilterName;
	m_pOwnerData->getAttributeFilters(sAttributeFilterNameSpace, sAttributeFilterName, nAttributeFilterValue);
	uint32_t nAttributeFilterID = 0;
	if ((!sAttributeFilterNameSpace.empty()) && (!sAttributeFilterName.empty())) {
		nAttributeFilterID = pLayer->FindCustomSegmentAttributeID(sAttributeFilterNameSpace, sAttributeFilterName);
	}

	m_pOwnerData->getExposureParameters(dMaxLaserPowerInWatts, oieRecordingMode);

	addLayerToListEx(pLayer, oieRecordingMode, nAttributeFilterID, nAttributeFilterValue, (float)dMaxLaserPowerInWatts, bFailIfNonAssignedDataExists);
}


void CRTCContext::addLayerToListEx(LibMCEnv::PToolpathLayer pLayer, eOIERecordingMode oieRecordingMode, uint32_t nAttributeFilterID, int64_t nAttributeFilterValue, float fMaxLaserPowerInWatts, bool bFailIfNonAssignedDataExists)
{
	if (pLayer.get() == nullptr)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);

	double dUnits = pLayer->GetUnits();
	bool bEnableOIEMeasurementPerHatch = (oieRecordingMode == eOIERecordingMode::OIELaserActiveMeasurement) || (oieRecordingMode == eOIERecordingMode::OIEEnableAndLaserActiveMeasurement);

	switch (oieRecordingMode) {
	case eOIERecordingMode::OIEEnableAndContinuousMeasurement:
	case eOIERecordingMode::OIEEnableAndLaserActiveMeasurement:
		EnableOIE();
		break;
	}

	switch (oieRecordingMode) {
	case eOIERecordingMode::OIEContinuousMeasurement:
	case eOIERecordingMode::OIEEnableAndContinuousMeasurement:
		StartOIEMeasurement();
		break;
	}




	uint32_t nSegmentCount = pLayer->GetSegmentCount();
	for (uint32_t nSegmentIndex = 0; nSegmentIndex < nSegmentCount; nSegmentIndex++) {

		LibMCEnv::eToolpathSegmentType eSegmentType;
		uint32_t nPointCount;
		pLayer->GetSegmentInfo(nSegmentIndex, eSegmentType, nPointCount);

		bool bDrawSegment = true;
		if (nAttributeFilterID != 0) {
			int64_t segmentAttributeValue = pLayer->GetSegmentIntegerAttribute(nSegmentIndex, nAttributeFilterID);
			bDrawSegment = (segmentAttributeValue == nAttributeFilterValue);
		}

		if (bDrawSegment && (nPointCount >= 2)) {

			float fJumpSpeedInMMPerSecond = (float)pLayer->GetSegmentProfileTypedValue(nSegmentIndex, LibMCEnv::eToolpathProfileValueType::JumpSpeed);
			float fMarkSpeedInMMPerSecond = (float)pLayer->GetSegmentProfileTypedValue(nSegmentIndex, LibMCEnv::eToolpathProfileValueType::Speed);
			float fPowerInWatts = (float)pLayer->GetSegmentProfileTypedValue(nSegmentIndex, LibMCEnv::eToolpathProfileValueType::LaserPower);
			float fPowerInPercent = (fPowerInWatts * 100.f) / fMaxLaserPowerInWatts;
			float fLaserFocus = (float)pLayer->GetSegmentProfileTypedValue(nSegmentIndex, LibMCEnv::eToolpathProfileValueType::LaserFocus);

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
					if (nSkywritingMode == 3) {
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

					DrawPolylineOIE(nPointCount, ContourPoints.data(), fMarkSpeedInMMPerSecond, fJumpSpeedInMMPerSecond, fPowerInPercent, fLaserFocus, bEnableOIEMeasurementPerHatch);

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

					DrawHatchesOIE(Hatches.size(), Hatches.data(), fMarkSpeedInMMPerSecond, fJumpSpeedInMMPerSecond, fPowerInPercent, fLaserFocus, bEnableOIEMeasurementPerHatch);

					break;
				}

				}
			}

		}

	}

	if ((oieRecordingMode != eOIERecordingMode::OIERecordingDisabled))
		StopOIEMeasurement();

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