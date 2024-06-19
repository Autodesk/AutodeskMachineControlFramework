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

#include "libmcdriver_scanlab_rtcselector.hpp"
#include "libmcdriver_scanlab_rtccontext.hpp"
#include "libmcdriver_scanlab_interfaceexception.hpp"


using namespace LibMCDriver_ScanLab::Impl;

/*************************************************************************************************************************
 Class definition of CRTCSelector 
**************************************************************************************************************************/

CRTCSelector::CRTCSelector(PRTCContextOwnerData pRTCContextOwnerData, LibMCEnv::PDriverEnvironment pDriverEnvironment, double dDefaultInitialTimeout, double dDefaultMaxTimeout, double dDefaultMultiplier, const std::vector<uint8_t>& FirmwareData, const std::vector<uint8_t>& FPGAData, const std::vector<uint8_t>& AuxiliaryData) :
	 m_pDriverEnvironment (pDriverEnvironment), m_pRTCContextOwnerData (pRTCContextOwnerData),
	m_dDefaultInitialTimeout (dDefaultInitialTimeout), m_dDefaultMaxTimeout (dDefaultMaxTimeout), m_dDefaultMultiplier (dDefaultMultiplier),
	m_FirmwareData (FirmwareData),
	m_FPGAData (FPGAData),
	m_AuxiliaryData (AuxiliaryData)

{
	if (pRTCContextOwnerData == nullptr)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);

	m_pScanLabSDK = pRTCContextOwnerData->getScanLabSDK();
	if (m_pScanLabSDK.get() == nullptr)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);
	if (pDriverEnvironment.get () == nullptr)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);
}


CRTCSelector::~CRTCSelector()
{
}


LibMCDriver_ScanLab_uint32 CRTCSelector::SearchCards(const std::string & sIP, const std::string & sNetmask, const LibMCDriver_ScanLab_uint32 nTimeout)
{
	m_pScanLabSDK->initDLL();

	uint32_t nIP = m_pScanLabSDK->eth_convert_string_to_ip(sIP.c_str());
	if (nIP == 0)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDIPADDRESS);
	uint32_t nNetMask = m_pScanLabSDK->eth_convert_string_to_ip(sNetmask.c_str());
	if (nNetMask == 0)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDNETMASK);

	// eth search cards timeout ethernet
	m_pScanLabSDK->eth_set_search_cards_timeout(nTimeout);

	uint32_t nCount = m_pScanLabSDK->eth_search_cards_range(nIP, nIP);

	return nCount;

}

LibMCDriver_ScanLab_uint32 CRTCSelector::SearchCardsByRange(const std::string& sStartIP, const std::string& sEndIP, const LibMCDriver_ScanLab_uint32 nTimeout)
{
	m_pScanLabSDK->initDLL();

	uint32_t nStartIP = m_pScanLabSDK->eth_convert_string_to_ip(sStartIP.c_str());
	if (nStartIP == 0)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDIPADDRESS);
	uint32_t nEndIP = m_pScanLabSDK->eth_convert_string_to_ip(sEndIP.c_str());
	if (nEndIP == 0)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDIPADDRESS);

	m_pScanLabSDK->eth_set_search_cards_timeout(nTimeout);
	uint32_t nCount = m_pScanLabSDK->eth_search_cards_range(nStartIP, nEndIP);

	return nCount;
}

LibMCDriver_ScanLab_uint32 CRTCSelector::GetCardCount()
{
	m_pScanLabSDK->initDLL();

	return m_pScanLabSDK->rtc6_count_cards();
}

LibMCDriver_ScanLab_uint32 CRTCSelector::GetEthernetCardCount()
{
	m_pScanLabSDK->initDLL();

	return m_pScanLabSDK->eth_found_cards();
}

IRTCContext* CRTCSelector::acquireCardEx(const LibMCDriver_ScanLab_uint32 nNumber, bool bIsNetworkCard)
{
	m_pScanLabSDK->initDLL();

	setCommunicationTimeoutsBeforeAcquire(nNumber);

	loadFirmwareBeforeAcquisition(nNumber, bIsNetworkCard, false);

	uint32_t cardNo = m_pScanLabSDK->acquire_rtc(nNumber);
	if (cardNo == 0) {
		m_pScanLabSDK->checkError(m_pScanLabSDK->get_last_error());

		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CARDALREADYACQUIRED);
	}

	if (cardNo != nNumber) {
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INTERNALACQUISITIONERROR);

	}

	return new CRTCContext(m_pRTCContextOwnerData, cardNo, bIsNetworkCard, m_pDriverEnvironment);
}

IRTCContext* CRTCSelector::AcquireCard(const LibMCDriver_ScanLab_uint32 nNumber)
{
	return acquireCardEx(nNumber, false);
}


IRTCContext * CRTCSelector::AcquireCardBySerial(const LibMCDriver_ScanLab_uint32 nSerialNumber)
{
	m_pScanLabSDK->initDLL();

	uint32_t nCardCount = GetCardCount();
	uint32_t nCardNo;
	uint32_t nFoundCardNo = 0;

	bool bSerialAcquisitionError = false;
	for (nCardNo = 1; nCardNo <= nCardCount; nCardNo++) {
		uint32_t nCardSerial = m_pScanLabSDK->n_get_serial_number(nCardNo);

		if (nCardSerial == nSerialNumber) {
			nFoundCardNo = nCardNo;
		}

		if (nCardSerial == 0) {
			// If Card Serial is 0, we need to flash new firmware on card...
			loadFirmwareBeforeAcquisition(nCardNo, false, false);

			bSerialAcquisitionError = true;
		}

	}

	// If card was found, acquire card...
	if (nFoundCardNo != 0)
		return AcquireCard(nFoundCardNo);

	// If we did not find a card
	// Trigger complete retry of acquisition, in case of one serial number was 0
	if (bSerialAcquisitionError) {
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_COULDNOTDETERMINESERIALNUMBER);
	}
	else {
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CARDNOTFOUND);
	}

}

IRTCContext * CRTCSelector::AcquireEthernetCard(const LibMCDriver_ScanLab_uint32 nNumber)
{
	m_pScanLabSDK->initDLL();

	uint32_t nNewCardNo = m_pScanLabSDK->eth_assign_card(nNumber, 0);
	if (nNewCardNo == 0) {
		m_pScanLabSDK->checkError(m_pScanLabSDK->get_last_error());

		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CARDNOTFOUND);
	}

	return acquireCardEx(nNewCardNo, true);
}

IRTCContext * CRTCSelector::AcquireEthernetCardBySerial(const LibMCDriver_ScanLab_uint32 nSerialNumber)
{
	m_pScanLabSDK->initDLL();

	uint32_t nSearchCount = GetEthernetCardCount();
	uint32_t nSearchNo;
	uint32_t nFoundSearchNo = 0;

	bool bSerialAcquisitionError = false;


	for (nSearchNo = 1; nSearchNo <= nSearchCount; nSearchNo++) {
		uint32_t nCardSerial = m_pScanLabSDK->eth_get_serial_search(nSearchNo);
		if (nCardSerial == 0) {
			// LoadFirmware
			loadFirmwareForSearchNumber(nSearchNo, true);
			
			// Trigger complete retry
			bSerialAcquisitionError = true;
		}

		if (nCardSerial == nSerialNumber) {
			nFoundSearchNo = nSearchNo;
		}
	}

	// LoadFirmware first
	if (nFoundSearchNo != 0) {
		return AcquireEthernetCard(nFoundSearchNo);
	}

	if (bSerialAcquisitionError) {
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_COULDNOTDETERMINESERIALNUMBER);
	}
	else {
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CARDNOTFOUND);
	}
}


void CRTCSelector::setCommunicationTimeoutsBeforeAcquire(uint32_t nCardNo)
{
	// ATTENTION: Please sync this code with CRTCContext::SetCommunicationTimeouts
	double dOldInitialTimeout = 0.0;
	double dOldMaxTimeout = 0.0;
	double dOldMultiplier = 0.0;
	uint32_t nOldMode = 0;

	// Turn on high performance mode...
	m_pScanLabSDK->n_eth_set_high_performance_mode(nCardNo, 1);

	// Set Timeouts, but keep old mode
	m_pScanLabSDK->n_eth_get_com_timeouts_auto(nCardNo, &dOldInitialTimeout, &dOldMaxTimeout, &dOldMultiplier, &nOldMode);
	m_pScanLabSDK->n_eth_set_com_timeouts_auto(nCardNo, m_dDefaultInitialTimeout, m_dDefaultMaxTimeout, m_dDefaultMultiplier, nOldMode);

}

void CRTCSelector::loadFirmwareBeforeAcquisition(uint32_t nCardNo, bool bIsNetwork, bool bMustHaveData)
{
	bool bHasFirmwareData = ((m_FirmwareData.size() > 0) || (m_FPGAData.size() > 0) || (m_AuxiliaryData.size() > 0));

	if (bHasFirmwareData) {
		if (m_FirmwareData.size() == 0)
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_MISSINGFIRMWAREDATA);
		if (m_FPGAData.size() == 0)
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_MISSINGFPGADATA);
		if (m_AuxiliaryData.size() == 0)
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_MISSINGAUXILIARYDATA);

		CRTCContext::loadFirmwareEx(m_pScanLabSDK, nCardNo, m_FirmwareData.size(), m_FirmwareData.data(), m_FPGAData.size(), m_FPGAData.data(), m_AuxiliaryData.size(), m_AuxiliaryData.data(), bIsNetwork, m_pDriverEnvironment);

	}
	else {
		if (bMustHaveData)
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_MISSINGFIRMWAREINITIZATIONDATA);
	}
}

void CRTCSelector::loadFirmwareForSearchNumber(uint32_t nSearchNo, bool bMustHaveData)
{
	uint32_t nNewCardNo = m_pScanLabSDK->eth_assign_card(nSearchNo, 0);
	if (nNewCardNo == 0) {
		m_pScanLabSDK->checkError(m_pScanLabSDK->get_last_error());

		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_COULDNOTASSIGNETHERNETCARD);
	}
	
	try {
		loadFirmwareBeforeAcquisition(nNewCardNo, true, bMustHaveData);

		m_pScanLabSDK->eth_remove_card(nNewCardNo);
	}
	catch (...) {
		m_pScanLabSDK->eth_remove_card(nNewCardNo);
		throw;
	}


}
