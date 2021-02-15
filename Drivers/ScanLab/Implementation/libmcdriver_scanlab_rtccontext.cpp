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

using namespace LibMCDriver_ScanLab::Impl;

/*************************************************************************************************************************
 Class definition of CRTCContext 
**************************************************************************************************************************/

CRTCContext::CRTCContext(PScanLabSDK pScanLabSDK, uint32_t nCardNo, bool bIsNetwork, LibMCEnv::PDriverEnvironment pDriverEnvironment)
	: m_pScanLabSDK (pScanLabSDK), 
	m_CardNo (nCardNo), 
	m_dCorrectionFactor(10000.0), 
	m_LaserPort(eLaserPort::Port12BitAnalog1), 
	m_pDriverEnvironment (pDriverEnvironment),
	m_bIsNetwork (bIsNetwork)
{
	if (pScanLabSDK.get() == nullptr)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);
	if (pDriverEnvironment.get () == nullptr)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);
	if (nCardNo == 0)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);

}

CRTCContext::~CRTCContext()
{
	if (m_pScanLabSDK.get() != nullptr) {
		m_pScanLabSDK->release_rtc(m_CardNo);
		m_CardNo = 0;
	}
}



void CRTCContext::LoadFirmware(const std::string& sFirmwareResource, const std::string& sFPGAResource, const std::string& sAuxiliaryResource)
{

	auto pWorkingDirectory = m_pDriverEnvironment->CreateWorkingDirectory();

	std::string sFirmwareName;
	if (m_bIsNetwork) {
		sFirmwareName = "RTC6ETH.out";
	}
	else {
		sFirmwareName = "RTC6OUT.out";
	}

	auto pFirmwareFile = pWorkingDirectory->StoreDriverData (sFirmwareName, sFirmwareResource);
	auto pFPGAFile = pWorkingDirectory->StoreDriverData ("RTC6RBF.rbf", sFPGAResource);
	auto pAuxiliaryFile = pWorkingDirectory->StoreDriverData("RTC6DAT.dat", sAuxiliaryResource);

	// TODO: Convert to ANSI
	uint32_t nErrorCode = m_pScanLabSDK->n_load_program_file (m_CardNo, pWorkingDirectory->GetAbsoluteFilePath().c_str ());

	pFirmwareFile = nullptr;
	pFPGAFile = nullptr;
	pAuxiliaryFile = nullptr;

	pWorkingDirectory->CleanUp();

	if (nErrorCode != 0)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_COULDNOTLOADPROGRAMFILE, "could not load program file: #" + std::to_string (nErrorCode));

}

void CRTCContext::LoadCorrectionFile(const LibMCDriver_ScanLab_uint64 nCorrectionFileBufferSize, const LibMCDriver_ScanLab_uint8* pCorrectionFileBuffer, const LibMCDriver_ScanLab_uint32 nTableNumber, const LibMCDriver_ScanLab_uint32 nDimension)
{

	auto pWorkingDirectory = m_pDriverEnvironment->CreateWorkingDirectory();

	auto pCorrectionFile = pWorkingDirectory->StoreCustomData("cor.ct5", LibMCEnv::CInputVector<uint8_t>(pCorrectionFileBuffer, nCorrectionFileBufferSize));

	// TODO: Convert to ANSI
	uint32_t nErrorCode = m_pScanLabSDK->n_load_correction_file(m_CardNo, pCorrectionFile->GetAbsoluteFileName ().c_str(), nTableNumber, nDimension);

	pCorrectionFile = nullptr;
	pWorkingDirectory->CleanUp();

	if (nErrorCode != 0)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_COULDNOTLOADCORRECTIONFILE, "Could not load correction file #" + std::to_string (nErrorCode));

}


void CRTCContext::SelectCorrectionTable(const LibMCDriver_ScanLab_uint32 nTableNumberHeadA, const LibMCDriver_ScanLab_uint32 nTableNumberHeadB)
{
	m_pScanLabSDK->n_select_cor_table(m_CardNo, nTableNumberHeadA, nTableNumberHeadB);
	m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));

	double dCorrectionFactor = m_pScanLabSDK->n_get_table_para(m_CardNo, nTableNumberHeadA, 1);
	m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));

	if (dCorrectionFactor > 0.0)
		m_dCorrectionFactor = dCorrectionFactor;
}

void CRTCContext::ConfigureLists(const LibMCDriver_ScanLab_uint32 nSizeListA, const LibMCDriver_ScanLab_uint32 nSizeListB)
{
	m_pScanLabSDK->n_config_list(m_CardNo, nSizeListA, nSizeListB);
	m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));
}

void CRTCContext::SetLaserMode(const LibMCDriver_ScanLab::eLaserMode eLaserMode, const LibMCDriver_ScanLab::eLaserPort eLaserPort)
{
	m_pScanLabSDK->n_set_laser_mode(m_CardNo, (uint32_t)eLaserMode);
	m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));

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
}

void CRTCContext::SetLaserDelaysInMicroseconds(const LibMCDriver_ScanLab_double dLaserOnDelay, const LibMCDriver_ScanLab_double dLaserOffDelay)
{
	double LaserOnDelayBits = round(dLaserOnDelay * 64.0);
	double LaserOffDelayBits = round(dLaserOffDelay * 64.0);

	if ((LaserOnDelayBits < 1) || (LaserOnDelayBits >= (double)(1UL << 31)))
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);
	if ((LaserOffDelayBits < 1) || (LaserOffDelayBits >= (double)(1UL << 31)))
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);

	SetLaserDelaysInBits((uint32_t)LaserOnDelayBits, (uint32_t)LaserOffDelayBits);
}

void CRTCContext::SetLaserDelaysInBits(const LibMCDriver_ScanLab_uint32 nLaserOnDelay, const LibMCDriver_ScanLab_uint32 nLaserOffDelay)
{
	m_pScanLabSDK->n_set_laser_delays(m_CardNo, nLaserOnDelay, nLaserOffDelay);
	m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));
}

void CRTCContext::writeSpeeds(const LibMCDriver_ScanLab_single fMarkSpeed, const LibMCDriver_ScanLab_single fJumpSpeed, const LibMCDriver_ScanLab_single fPower)
{

	double dBitsPerMM = m_dCorrectionFactor;

	double dMarkSpeedInMMPerMilliSecond = (double)fMarkSpeed / 1000.0;
	m_pScanLabSDK->n_set_mark_speed(m_CardNo, dMarkSpeedInMMPerMilliSecond * dBitsPerMM);
	m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));

	double dJumpSpeedInMMPerMilliSecond = (double)fJumpSpeed / 1000.0;
	m_pScanLabSDK->n_set_jump_speed(m_CardNo, dJumpSpeedInMMPerMilliSecond * dBitsPerMM);
	m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));

	// TODO: Brennt die maschine ab?
	double fClippedPowerFactor = fPower / 100.0f;
	if (fClippedPowerFactor > 1.0f)
		fClippedPowerFactor = 1.0f;
	if (fClippedPowerFactor < 0.0f)
		fClippedPowerFactor = 0.0f;

	switch (m_LaserPort) {
	case eLaserPort::Port16bitDigital:
		m_pScanLabSDK->n_write_io_port(m_CardNo, (int)round(fClippedPowerFactor * 65535.0));
		break;
	case eLaserPort::Port8bitDigital:
		m_pScanLabSDK->n_write_8bit_port(m_CardNo, (int)round(fClippedPowerFactor * 255.0));
		break;
	case eLaserPort::Port12BitAnalog1:
		m_pScanLabSDK->n_write_da_1(m_CardNo, (int)round(fClippedPowerFactor * 4095.0));
		break;
	case eLaserPort::Port12BitAnalog2:
		m_pScanLabSDK->n_write_da_2(m_CardNo, (int)round(fClippedPowerFactor * 4095.0));
		break;

	}

}

void CRTCContext::DrawPolyline(const LibMCDriver_ScanLab_uint64 nPointsBufferSize, const LibMCDriver_ScanLab::sPoint2D * pPointsBuffer, const LibMCDriver_ScanLab_single fMarkSpeed, const LibMCDriver_ScanLab_single fJumpSpeed, const LibMCDriver_ScanLab_single fPower)
{
	if (!pPointsBuffer)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);

	if (nPointsBufferSize == 0)
		return;

	writeSpeeds(fMarkSpeed, fJumpSpeed, fPower);

	const sPoint2D* pPoint = pPointsBuffer;
	double dX = round(pPoint->m_X * m_dCorrectionFactor);
	double dY = round(pPoint->m_Y * m_dCorrectionFactor);
	pPoint++;
	// TODO: Brennt die maschine ab?

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

void CRTCContext::DrawHatches(const LibMCDriver_ScanLab_uint64 nHatchesBufferSize, const LibMCDriver_ScanLab::sHatch2D * pHatchesBuffer, const LibMCDriver_ScanLab_single fMarkSpeed, const LibMCDriver_ScanLab_single fJumpSpeed, const LibMCDriver_ScanLab_single fPower)
{
	if (!pHatchesBuffer)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);

	if (nHatchesBufferSize == 0)
		return;

	writeSpeeds(fMarkSpeed, fJumpSpeed, fPower);

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

LibMCDriver_ScanLab_uint32 CRTCContext::GetInputPointer()
{
	uint32_t Pos = m_pScanLabSDK->n_get_input_pointer(m_CardNo);
	m_pScanLabSDK->checkError(m_pScanLabSDK->n_get_last_error(m_CardNo));
	return Pos;
}

