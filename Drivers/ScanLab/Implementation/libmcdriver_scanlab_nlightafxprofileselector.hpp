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


Abstract: This is the class declaration of CNLightAFXProfileSelector

*/


#ifndef __LIBMCDRIVER_SCANLAB_NLIGHTAFXPROFILESELECTOR
#define __LIBMCDRIVER_SCANLAB_NLIGHTAFXPROFILESELECTOR

#include "libmcdriver_scanlab_interfaces.hpp"

// Parent classes
#include "libmcdriver_scanlab_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#define NLIGHTAFX_MAXMODEINDEX 6
#define NLIGHTAFX_MINSELECTIONDELAY 1
#define NLIGHTAFX_MAXSELECTIONDELAY 1000
#define NLIGHTAFX_MINACKNOWLEDGETIMEOUT 1
#define NLIGHTAFX_MAXACKNOWLEDGETIMEOUT 1000
#define NLIGHTAFX_MODEUNSELECTED 0xffffffff

#include "libmcdriver_scanlab_sdk.hpp"

namespace LibMCDriver_ScanLab {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CNLightAFXProfileSelector 
**************************************************************************************************************************/

class CNLightAFXProfileSelectorInstance {
private:

	PScanLabSDK m_pSDK;
	uint32_t m_nCardNo;

	bool m_bIsEnabled;

	uint32_t m_nEnableDigitalOutputBit;
	uint32_t m_nStartDigitalOutputBit;
	uint32_t m_nStartIndexSelection0OutputBit;
	uint32_t m_nStartIndexSelection1OutputBit;
	uint32_t m_nStartIndexSelection2OutputBit;
	uint32_t m_nSelectionAcknowledgeInputBit;

	uint32_t m_nAcknowledgeTimeoutInMilliseconds;
	uint32_t m_nSelectionDelayInMilliseconds;

	uint32_t m_nCurrentAFXMode;

public:

	CNLightAFXProfileSelectorInstance(PScanLabSDK pSDK, uint32_t nCardNo);
		
	virtual ~CNLightAFXProfileSelectorInstance();

	void setControlOutputPins(const LibMCDriver_ScanLab_uint32 nEnableDigitalOutputBit, const LibMCDriver_ScanLab_uint32 nStartDigitalOutputBit);

	void getControlOutputPins(LibMCDriver_ScanLab_uint32& nEnableDigitalOutputBit, LibMCDriver_ScanLab_uint32& nStartDigitalOutputBit);

	void setSelectionOutputPins(const LibMCDriver_ScanLab_uint32 nStartIndexSelection0OutputBit, const LibMCDriver_ScanLab_uint32 nStartIndexSelection1OutputBit, const LibMCDriver_ScanLab_uint32 nStartIndexSelection2OutputBit);

	void getSelectionOutputPins(LibMCDriver_ScanLab_uint32& nStartIndexSelection0OutputBit, LibMCDriver_ScanLab_uint32& nStartIndexSelection1OutputBit, LibMCDriver_ScanLab_uint32& nStartIndexSelection2OutputBit);

	void setAcknowledgeInputPin(const LibMCDriver_ScanLab_uint32 nSelectionAcknowledgeInputBit);

	LibMCDriver_ScanLab_uint32 getAcknowledgeInputPin();

	void setSelectionDelay(const LibMCDriver_ScanLab_uint32 nSelectionDelayInMilliseconds);

	LibMCDriver_ScanLab_uint32 getSelectionDelay();

	void setAcknowledgeTimeout(const LibMCDriver_ScanLab_uint32 nAcknowledgeInMilliseconds);

	LibMCDriver_ScanLab_uint32 getAcknowledgeTimeout();

	void enableSelection();

	void disableSelection();

	bool isEnabled();

	void addCustomSelection(uint32_t nAFXModeIndex);

	void selectAFXModeIfNecessary(uint32_t nAFXModeIndex);

	static uint32_t getMaxAFXMode();


};

typedef std::shared_ptr<CNLightAFXProfileSelectorInstance> PNLightAFXProfileSelectorInstance;

class CNLightAFXProfileSelector : public virtual INLightAFXProfileSelector, public virtual CBase {
private:

	PNLightAFXProfileSelectorInstance m_pSelectorInstance;

public:

	CNLightAFXProfileSelector(PNLightAFXProfileSelectorInstance pSelectorInstance);

	virtual ~CNLightAFXProfileSelector();

	void SetControlOutputPins(const LibMCDriver_ScanLab_uint32 nEnableDigitalOutputBit, const LibMCDriver_ScanLab_uint32 nStartDigitalOutputBit) override;

	void GetControlOutputPins(LibMCDriver_ScanLab_uint32 & nEnableDigitalOutputBit, LibMCDriver_ScanLab_uint32 & nStartDigitalOutputBit) override;

	void SetSelectionOutputPins(const LibMCDriver_ScanLab_uint32 nStartIndexSelection0OutputBit, const LibMCDriver_ScanLab_uint32 nStartIndexSelection1OutputBit, const LibMCDriver_ScanLab_uint32 nStartIndexSelection2OutputBit) override;

	void GetSelectionOutputPins(LibMCDriver_ScanLab_uint32 & nStartIndexSelection0OutputBit, LibMCDriver_ScanLab_uint32 & nStartIndexSelection1OutputBit, LibMCDriver_ScanLab_uint32 & nStartIndexSelection2OutputBit) override;

	void SetAcknowledgeInputPin(const LibMCDriver_ScanLab_uint32 nSelectionAcknowledgeInputBit) override;

	LibMCDriver_ScanLab_uint32 GetAcknowledgeInputPin() override;

	void SetSelectionDelay(const LibMCDriver_ScanLab_uint32 nSelectionDelayInMilliseconds) override;

	LibMCDriver_ScanLab_uint32 GetSelectionDelay() override;

	void SetAcknowledgeTimeout(const LibMCDriver_ScanLab_uint32 nAcknowledgeInMilliseconds) override;

	LibMCDriver_ScanLab_uint32 GetAcknowledgeTimeout() override;

	void EnableAutomaticSelection() override;

	void DisableAutomaticSelection() override;

	void AddCustomSelection(const LibMCDriver_ScanLab_uint32 nAFXModeIndex) override;

};

} // namespace Impl
} // namespace LibMCDriver_ScanLab

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_SCANLAB_NLIGHTAFXPROFILESELECTOR
