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

using namespace LibMCDriver_ScanLab::Impl;

/*************************************************************************************************************************
 Class definition of CRTCContext 
**************************************************************************************************************************/

CRTCContext::CRTCContext(PScanLabSDK pScanLabSDK, uint32_t nCardNo, bool bIsNetwork, LibMCEnv::PDriverEnvironment pDriverEnvironment)
	: m_pScanLabSDK (pScanLabSDK), 
	m_CardNo (nCardNo), 
	m_dCorrectionFactor(10000.0), 
	m_dZCorrectionFactor(10000.0),
	m_nLaserIndex (0),
	m_LaserPort(eLaserPort::Port12BitAnalog1), 
	m_pDriverEnvironment (pDriverEnvironment),
	m_OIEOperationMode (LibMCDriver_ScanLab::eOIEOperationMode::OIENotInitialized),
	m_bIsNetwork (bIsNetwork)
{
	if (pScanLabSDK.get() == nullptr)
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

	if ((LaserOnDelayBits < 1) || (LaserOnDelayBits >= (double)(1UL << 31)))
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDLASERDELAY);
	if ((LaserOffDelayBits < 1) || (LaserOffDelayBits >= (double)(1UL << 31)))
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDLASERDELAY);

	SetLaserDelaysInBits((uint32_t)LaserOnDelayBits, (uint32_t)LaserOffDelayBits);

	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);

}

void CRTCContext::SetLaserDelaysInBits(const LibMCDriver_ScanLab_uint32 nLaserOnDelay, const LibMCDriver_ScanLab_uint32 nLaserOffDelay)
{
	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);

	m_pScanLabSDK->n_set_laser_delays(m_CardNo, nLaserOnDelay, nLaserOffDelay);
	m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));

	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);

}

void CRTCContext::writeSpeeds(const LibMCDriver_ScanLab_single fMarkSpeed, const LibMCDriver_ScanLab_single fJumpSpeed, const LibMCDriver_ScanLab_single fPower)
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

	// TODO: Brennt die maschine ab?
	double fClippedPowerFactor = fPower / 100.0f;
	if (fClippedPowerFactor > 1.0f)
		fClippedPowerFactor = 1.0f;
	if (fClippedPowerFactor < 0.0f)
		fClippedPowerFactor = 0.0f;

	int digitalPowerValue;

	switch (m_LaserPort) {
	case eLaserPort::Port16bitDigital:
		digitalPowerValue = (int)round(fClippedPowerFactor * 65535.0);
		m_pScanLabSDK->n_write_io_port_list(m_CardNo, digitalPowerValue);
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

	}

}

void CRTCContext::DrawPolyline(const LibMCDriver_ScanLab_uint64 nPointsBufferSize, const LibMCDriver_ScanLab::sPoint2D * pPointsBuffer, const LibMCDriver_ScanLab_single fMarkSpeed, const LibMCDriver_ScanLab_single fJumpSpeed, const LibMCDriver_ScanLab_single fPower, const LibMCDriver_ScanLab_single fZValue)
{
	if (!pPointsBuffer)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);

	if (nPointsBufferSize == 0)
		return;

	writeSpeeds(fMarkSpeed, fJumpSpeed, fPower);

	// Z Plane
	double defocusZ = round(fZValue * m_dZCorrectionFactor);
	int intDefocusZ = (int)defocusZ;
	m_pScanLabSDK->n_set_defocus_list (m_CardNo, intDefocusZ);

	const sPoint2D* pPoint = pPointsBuffer;
	double dX = round(pPoint->m_X * m_dCorrectionFactor);
	double dY = round(pPoint->m_Y * m_dCorrectionFactor);
	pPoint++;

	int intX = (int)dX;
	int intY = (int)dY;
	m_pScanLabSDK->n_jump_abs(m_CardNo, intX, intY);
	m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));

	for (uint64_t index = 1; index < nPointsBufferSize; index++) {
		dX = round(pPoint->m_X * m_dCorrectionFactor);
		dY = round(pPoint->m_Y * m_dCorrectionFactor);
		pPoint++;
		// TODO: Brennt die maschine ab?

		intX = (int)dX;
		intY = (int)dY;
		m_pScanLabSDK->n_mark_abs(m_CardNo, intX, intY);
		m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));

	}
}

void CRTCContext::DrawHatches(const LibMCDriver_ScanLab_uint64 nHatchesBufferSize, const LibMCDriver_ScanLab::sHatch2D * pHatchesBuffer, const LibMCDriver_ScanLab_single fMarkSpeed, const LibMCDriver_ScanLab_single fJumpSpeed, const LibMCDriver_ScanLab_single fPower, const LibMCDriver_ScanLab_single fZValue)
{
	if (!pHatchesBuffer)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);

	if (nHatchesBufferSize == 0)
		return;

	writeSpeeds(fMarkSpeed, fJumpSpeed, fPower);

	// Z Plane
	double defocusZ = round(fZValue * m_dZCorrectionFactor);
	int intDefocusZ = (int)defocusZ;
	m_pScanLabSDK->n_set_defocus_list(m_CardNo, intDefocusZ);

	const sHatch2D* pHatch = pHatchesBuffer;

	for (uint64_t index = 0; index < nHatchesBufferSize; index++) {
		double dX = round(pHatch->m_X1 * m_dCorrectionFactor);
		double dY = round(pHatch->m_Y1 * m_dCorrectionFactor);
		// TODO: Brennt die maschine ab?

		int intX = (int)dX;
		int intY = (int)dY;
		m_pScanLabSDK->n_jump_abs(m_CardNo, intX, intY);
		m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));

		dX = round(pHatch->m_X2 * m_dCorrectionFactor);
		dY = round(pHatch->m_Y2 * m_dCorrectionFactor);
		// TODO: Brennt die maschine ab?

		intX = (int)dX;
		intY = (int)dY;
		m_pScanLabSDK->n_mark_abs(m_CardNo, intX, intY);
		m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));

		pHatch++;
	}
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

	// Magic number parameters, MUST be 1, 1
	m_pScanLabSDK->n_mcbsp_init(m_CardNo, 1, 1);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

	// Frequency should be 6MHz by specification
	uint32_t nMCBSPSignalFrequency = 6000000;
	m_pScanLabSDK->n_set_mcbsp_freq(m_CardNo, nMCBSPSignalFrequency);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

	// Set signal channels
	m_pScanLabSDK->n_set_mcbsp_out_ptr(m_CardNo, (uint32_t)m_MCBSPSignalChannels.size (), m_MCBSPSignalChannels.data ());
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

	// No PID control for now
	m_pScanLabSDK->n_set_multi_mcbsp_in(m_CardNo, 0, 0, 0);

	// Check Error
	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);

}


void CRTCContext::EnableOIE()
{
	if (m_OIEOperationMode == LibMCDriver_ScanLab::eOIEOperationMode::OIENotInitialized)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_OIEHASNOTBEENINITIALIZED);

	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);

	m_pScanLabSDK->n_set_free_variable_list(m_CardNo, 0, 1);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

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

	m_pScanLabSDK->n_long_delay(m_CardNo, (uint32_t)m_MCBSPSignalChannels.size() + 2);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

	m_pScanLabSDK->n_set_free_variable_list(m_CardNo, 0, 0);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

}

void CRTCContext::StartOIEMeasurement()
{
	if (m_OIEOperationMode == LibMCDriver_ScanLab::eOIEOperationMode::OIENotInitialized)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_OIEHASNOTBEENINITIALIZED);

	m_pScanLabSDK->checkGlobalErrorOfCard(m_CardNo);

	m_pScanLabSDK->n_long_delay(m_CardNo, (uint32_t)m_MCBSPSignalChannels.size());
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

	m_pScanLabSDK->n_list_nop(m_CardNo);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

	switch (m_OIEOperationMode) {
	case LibMCDriver_ScanLab::eOIEOperationMode::OIEVersion2:
		// Bit 8 triggers OIE Measurement Start
		m_pScanLabSDK->n_set_free_variable_list(m_CardNo, 0, 1UL | (1UL << 8));
		break;

	case LibMCDriver_ScanLab::eOIEOperationMode::OIEVersion3Compatibility:
	case LibMCDriver_ScanLab::eOIEOperationMode::OIEVersion3:
		// Bit 4 triggers OIE Measurement Start
		m_pScanLabSDK->n_set_free_variable_list(m_CardNo, 0, 1UL | (1UL << 4));
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

	m_pScanLabSDK->n_long_delay(m_CardNo, (uint32_t)m_MCBSPSignalChannels.size() + 2);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

	m_pScanLabSDK->n_set_free_variable_list(m_CardNo, 0, 1);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

	m_pScanLabSDK->n_long_delay(m_CardNo, (uint32_t)m_MCBSPSignalChannels.size() + 2);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

	m_pScanLabSDK->n_set_free_variable_list(m_CardNo, 1, 0);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);

	m_pScanLabSDK->n_long_delay(m_CardNo, (uint32_t)m_MCBSPSignalChannels.size() + 2);
	m_pScanLabSDK->checkLastErrorOfCard(m_CardNo);
}

void CRTCContext::StartOIEPIDControl()
{
	if (m_OIEOperationMode == LibMCDriver_ScanLab::eOIEOperationMode::OIENotInitialized)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_OIEHASNOTBEENINITIALIZED);

}

void CRTCContext::StopOIEPIDControl()
{
	if (m_OIEOperationMode == LibMCDriver_ScanLab::eOIEOperationMode::OIENotInitialized)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_OIEHASNOTBEENINITIALIZED);

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

