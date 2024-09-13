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


Abstract: This is a stub class definition of CNLightAFXProfileSelector

*/

#include "libmcdriver_scanlab_nlightafxprofileselector.hpp"
#include "libmcdriver_scanlab_interfaceexception.hpp"

// Include custom headers here.


using namespace LibMCDriver_ScanLab::Impl;

/*************************************************************************************************************************
 Class definition of CNLightAFXProfileSelector 
**************************************************************************************************************************/

CNLightAFXProfileSelectorInstance::CNLightAFXProfileSelectorInstance(PScanLabSDK pSDK, uint32_t nCardNo)
	:  m_bIsEnabled (false),
	   m_nEnableDigitalOutputBit (0),
	   m_nStartDigitalOutputBit (1),
	   m_nStartIndexSelection0OutputBit (2),
	   m_nStartIndexSelection1OutputBit (3),
	   m_nStartIndexSelection2OutputBit (4),
       m_nSelectionAcknowledgeInputBit (0),
	   m_nSelectionDelayInMilliseconds (30),
	   m_nAcknowledgeTimeoutInMilliseconds(500),
	   m_pSDK (pSDK),
	   m_nCardNo (nCardNo)
{
	if (pSDK.get() == nullptr)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);
}

CNLightAFXProfileSelectorInstance::~CNLightAFXProfileSelectorInstance()
{
}

void CNLightAFXProfileSelectorInstance::setControlOutputPins(const LibMCDriver_ScanLab_uint32 nEnableDigitalOutputBit, const LibMCDriver_ScanLab_uint32 nStartDigitalOutputBit)
{
	if (m_bIsEnabled)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CANNOTCHANGENLIGHTSETTINGSWHILEENABLED);

	if (nEnableDigitalOutputBit > 15)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDNLIGHTCONTROLPIN);
	if (nStartDigitalOutputBit > 15)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDNLIGHTCONTROLPIN);

	m_nEnableDigitalOutputBit = nEnableDigitalOutputBit;
	m_nStartDigitalOutputBit = nStartDigitalOutputBit;
}

void CNLightAFXProfileSelectorInstance::getControlOutputPins(LibMCDriver_ScanLab_uint32& nEnableDigitalOutputBit, LibMCDriver_ScanLab_uint32& nStartDigitalOutputBit)
{
	nEnableDigitalOutputBit = m_nEnableDigitalOutputBit;
	nStartDigitalOutputBit = m_nStartDigitalOutputBit;

}

void CNLightAFXProfileSelectorInstance::setSelectionOutputPins(const LibMCDriver_ScanLab_uint32 nStartIndexSelection0OutputBit, const LibMCDriver_ScanLab_uint32 nStartIndexSelection1OutputBit, const LibMCDriver_ScanLab_uint32 nStartIndexSelection2OutputBit)
{
	if (m_bIsEnabled)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CANNOTCHANGENLIGHTSETTINGSWHILEENABLED);
	if (nStartIndexSelection0OutputBit > 15)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDNLIGHTSELECTIONPIN);
	if (nStartIndexSelection1OutputBit > 15)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDNLIGHTSELECTIONPIN);
	if (nStartIndexSelection2OutputBit > 15)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDNLIGHTSELECTIONPIN);

	m_nStartIndexSelection0OutputBit = nStartIndexSelection0OutputBit;
	m_nStartIndexSelection1OutputBit = nStartIndexSelection1OutputBit;
	m_nStartIndexSelection2OutputBit = nStartIndexSelection2OutputBit;

}

void CNLightAFXProfileSelectorInstance::getSelectionOutputPins(LibMCDriver_ScanLab_uint32& nStartIndexSelection0OutputBit, LibMCDriver_ScanLab_uint32& nStartIndexSelection1OutputBit, LibMCDriver_ScanLab_uint32& nStartIndexSelection2OutputBit)
{
	nStartIndexSelection0OutputBit = m_nStartIndexSelection0OutputBit;
	nStartIndexSelection1OutputBit = m_nStartIndexSelection1OutputBit;
	nStartIndexSelection2OutputBit = m_nStartIndexSelection2OutputBit;

}

void CNLightAFXProfileSelectorInstance::setAcknowledgeInputPin(const LibMCDriver_ScanLab_uint32 nSelectionAcknowledgeInputBit)
{
	if (m_bIsEnabled)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CANNOTCHANGENLIGHTSETTINGSWHILEENABLED);
	if (nSelectionAcknowledgeInputBit > 15)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDNLIGHTINPUTPIN);

	m_nSelectionAcknowledgeInputBit = nSelectionAcknowledgeInputBit;

}

LibMCDriver_ScanLab_uint32 CNLightAFXProfileSelectorInstance::getAcknowledgeInputPin()
{
	return m_nSelectionAcknowledgeInputBit;
}

void CNLightAFXProfileSelectorInstance::setSelectionDelay(const LibMCDriver_ScanLab_uint32 nSelectionDelayInMilliseconds)
{
	if (m_bIsEnabled)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CANNOTCHANGENLIGHTSETTINGSWHILEENABLED);

	if ((nSelectionDelayInMilliseconds < NLIGHTAFX_MINSELECTIONDELAY) || (nSelectionDelayInMilliseconds > NLIGHTAFX_MAXSELECTIONDELAY))
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDNLIGHTAFXSELECTIONDELAY);

}

LibMCDriver_ScanLab_uint32 CNLightAFXProfileSelectorInstance::getSelectionDelay()
{
	return m_nSelectionDelayInMilliseconds;
}

void CNLightAFXProfileSelectorInstance::setAcknowledgeTimeout(const LibMCDriver_ScanLab_uint32 nAcknowledgeInMilliseconds)
{
	if (m_bIsEnabled)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CANNOTCHANGENLIGHTSETTINGSWHILEENABLED);

	if ((nAcknowledgeInMilliseconds < NLIGHTAFX_MINACKNOWLEDGETIMEOUT) || (nAcknowledgeInMilliseconds > NLIGHTAFX_MAXACKNOWLEDGETIMEOUT))
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDNLIGHTAFXACKNOWLEDGETIMEOUT);

}

LibMCDriver_ScanLab_uint32 CNLightAFXProfileSelectorInstance::getAcknowledgeTimeout()
{
	return m_nAcknowledgeTimeoutInMilliseconds;

}

void CNLightAFXProfileSelectorInstance::enableSelection()
{
	if (!m_bIsEnabled) {
		m_bIsEnabled = true;
	}
}

void CNLightAFXProfileSelectorInstance::disableSelection()
{
	if (m_bIsEnabled) {
		m_bIsEnabled = false;
	}
}

void CNLightAFXProfileSelectorInstance::addCustomSelection(const LibMCDriver_ScanLab_uint32 nAFXModeIndex)
{
	if (nAFXModeIndex > NLIGHTAFX_MAXMODEINDEX)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDNLIGHTAFXMODE);

	bool bBit0 = (nAFXModeIndex & 0x01) != 0;
	bool bBit1 = (nAFXModeIndex & 0x02) != 0;
	bool bBit2 = (nAFXModeIndex & 0x04) != 0;

	uint32_t nBitmask = (1UL << m_nEnableDigitalOutputBit) | (1UL << m_nStartDigitalOutputBit) | (1UL << m_nStartIndexSelection0OutputBit) | (1UL << m_nStartIndexSelection1OutputBit) | (1UL << m_nStartIndexSelection2OutputBit);

	uint32_t nSelectionValue = (1UL << m_nEnableDigitalOutputBit);
	if (bBit0)
		nSelectionValue |= (1UL << m_nStartIndexSelection0OutputBit);
	if (bBit1)
		nSelectionValue |= (1UL << m_nStartIndexSelection1OutputBit);
	if (bBit2)
		nSelectionValue |= (1UL << m_nStartIndexSelection2OutputBit);

	// We have a base frequency of 100kHz, so 100 cycles per milliseconds
	uint32_t nDelayInCycles = m_nSelectionDelayInMilliseconds * 100; 
	uint32_t nTimeOutInCycles = m_nAcknowledgeTimeoutInMilliseconds * 100;

	uint32_t nStartValue = nSelectionValue | (1UL << m_nStartDigitalOutputBit);

	m_pSDK->n_write_io_port_mask_list(m_nCardNo, nSelectionValue, nBitmask);
	m_pSDK->n_long_delay(m_nCardNo, nDelayInCycles);
	m_pSDK->n_write_io_port_mask_list(m_nCardNo, nStartValue, nBitmask);
	m_pSDK->n_long_delay(m_nCardNo, nTimeOutInCycles);

}

CNLightAFXProfileSelector::CNLightAFXProfileSelector(PNLightAFXProfileSelectorInstance pSelectorInstance)
	: m_pSelectorInstance (pSelectorInstance)
{
	if (pSelectorInstance.get() == nullptr)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);
}

CNLightAFXProfileSelector::~CNLightAFXProfileSelector()
{

}


void CNLightAFXProfileSelector::SetControlOutputPins(const LibMCDriver_ScanLab_uint32 nEnableDigitalOutputBit, const LibMCDriver_ScanLab_uint32 nStartDigitalOutputBit)
{
	m_pSelectorInstance->setControlOutputPins(nEnableDigitalOutputBit, nStartDigitalOutputBit);
}

void CNLightAFXProfileSelector::GetControlOutputPins(LibMCDriver_ScanLab_uint32 & nEnableDigitalOutputBit, LibMCDriver_ScanLab_uint32 & nStartDigitalOutputBit)
{
	m_pSelectorInstance->getControlOutputPins(nEnableDigitalOutputBit, nStartDigitalOutputBit);
}

void CNLightAFXProfileSelector::SetSelectionOutputPins(const LibMCDriver_ScanLab_uint32 nStartIndexSelection0OutputBit, const LibMCDriver_ScanLab_uint32 nStartIndexSelection1OutputBit, const LibMCDriver_ScanLab_uint32 nStartIndexSelection2OutputBit)
{
	m_pSelectorInstance->setSelectionOutputPins(nStartIndexSelection0OutputBit, nStartIndexSelection1OutputBit, nStartIndexSelection2OutputBit);
}

void CNLightAFXProfileSelector::GetSelectionOutputPins(LibMCDriver_ScanLab_uint32 & nStartIndexSelection0OutputBit, LibMCDriver_ScanLab_uint32 & nStartIndexSelection1OutputBit, LibMCDriver_ScanLab_uint32 & nStartIndexSelection2OutputBit)
{
	m_pSelectorInstance->getSelectionOutputPins(nStartIndexSelection0OutputBit, nStartIndexSelection1OutputBit, nStartIndexSelection2OutputBit);
}

void CNLightAFXProfileSelector::SetAcknowledgeInputPin(const LibMCDriver_ScanLab_uint32 nSelectionAcknowledgeInputBit)
{
	m_pSelectorInstance->setAcknowledgeInputPin(nSelectionAcknowledgeInputBit);
}

LibMCDriver_ScanLab_uint32 CNLightAFXProfileSelector::GetAcknowledgeInputPin()
{
	return m_pSelectorInstance->getAcknowledgeInputPin();
}

void CNLightAFXProfileSelector::SetSelectionDelay(const LibMCDriver_ScanLab_uint32 nSelectionDelayInMilliseconds)
{
	m_pSelectorInstance->setSelectionDelay(nSelectionDelayInMilliseconds);
}

LibMCDriver_ScanLab_uint32 CNLightAFXProfileSelector::GetSelectionDelay()
{
	return m_pSelectorInstance->getSelectionDelay();
}

void CNLightAFXProfileSelector::SetAcknowledgeTimeout(const LibMCDriver_ScanLab_uint32 nAcknowledgeInMilliseconds)
{
	m_pSelectorInstance->setAcknowledgeTimeout(nAcknowledgeInMilliseconds);
}

LibMCDriver_ScanLab_uint32 CNLightAFXProfileSelector::GetAcknowledgeTimeout()
{
	return m_pSelectorInstance->getAcknowledgeTimeout();
}

void CNLightAFXProfileSelector::EnableAutomaticSelection()
{
	m_pSelectorInstance->enableSelection();
}

void CNLightAFXProfileSelector::DisableAutomaticSelection()
{
	m_pSelectorInstance->disableSelection();
}

void CNLightAFXProfileSelector::AddCustomSelection(const LibMCDriver_ScanLab_uint32 nAFXModeIndex)
{
	m_pSelectorInstance->addCustomSelection(nAFXModeIndex);
}

