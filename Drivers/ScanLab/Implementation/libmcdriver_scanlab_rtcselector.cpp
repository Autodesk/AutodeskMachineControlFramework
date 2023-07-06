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

CRTCSelector::CRTCSelector(IRTCContextOwner* pRTCContextOwner, LibMCEnv::PDriverEnvironment pDriverEnvironment) :
	 m_pDriverEnvironment (pDriverEnvironment)

{
	if (pRTCContextOwner == nullptr)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);

	m_pScanLabSDK = pRTCContextOwner->getScanLabSDK();
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

	m_pScanLabSDK->eth_set_search_cards_timeout(nTimeout);

	uint32_t nCount = m_pScanLabSDK->eth_search_cards(nIP, nNetMask);

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

	uint32_t cardNo = m_pScanLabSDK->acquire_rtc(nNumber);
	if (cardNo == 0) {
		m_pScanLabSDK->checkError(m_pScanLabSDK->get_last_error());

		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CARDALREADYACQUIRED);
	}

	return new CRTCContext(m_pRTCContextOwner, cardNo, bIsNetworkCard, m_pDriverEnvironment);
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

	for (nCardNo = 1; nCardNo <= nCardCount; nCardNo++) {
		uint32_t nCardSerial = m_pScanLabSDK->n_get_serial_number(nCardNo);
		if (nCardSerial == nSerialNumber)
			return AcquireCard(nCardNo);
	}

	throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CARDNOTFOUND);
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

	for (nSearchNo = 1; nSearchNo <= nSearchCount; nSearchNo++) {
		uint32_t nCardSerial = m_pScanLabSDK->eth_get_serial_search(nSearchNo);
		if (nCardSerial == 0)
			m_pScanLabSDK->checkError(m_pScanLabSDK->get_last_error());

		if (nCardSerial == nSerialNumber)
			return AcquireEthernetCard(nSearchNo);
	}

	throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CARDNOTFOUND);
}

