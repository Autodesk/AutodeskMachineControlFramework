/*++

Copyright (C) 2024 Autodesk Inc.

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


Abstract: This is the class declaration of CGPIOSequence

*/


#ifndef __LIBMCDRIVER_SCANLAB_GPIOSEQUENCEINSTANCE
#define __LIBMCDRIVER_SCANLAB_GPIOSEQUENCEINSTANCE

#include "libmcdriver_scanlab_interfaces.hpp"
#include "libmcdriver_scanlab_sdk.hpp"

namespace LibMCDriver_ScanLab {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CGPIOSequence 
**************************************************************************************************************************/


class CGPIOTask
{
private:

	uint32_t m_nRelativeStartAddress;

public:

	CGPIOTask(uint32_t nRelativeStartAddress);

	virtual ~CGPIOTask();

	virtual uint32_t getNeededListCapacity() = 0;

	virtual void writeToSDKList (CScanLabSDK * pSDK, uint32_t nCardNo) = 0;
};


class CGPIOTask_Output : public CGPIOTask
{
private:

	uint32_t m_nOutputBit;
	bool m_bOutputValue;

public:

	CGPIOTask_Output(uint32_t nRelativeStartAddress, const uint32_t nOutputBit, const bool bOutputValue);

	virtual ~CGPIOTask_Output();

	uint32_t getOutputBit();

	bool getOutputValue();

	uint32_t getNeededListCapacity() override;

	void writeToSDKList(CScanLabSDK* pSDK, uint32_t nCardNo) override;

};



class CGPIOTask_Delay : public CGPIOTask
{
private:

	uint32_t m_nDelayInMilliseconds;

public:

	CGPIOTask_Delay(uint32_t nRelativeStartAddress, const uint32_t nDelayInMilliseconds);

	virtual ~CGPIOTask_Delay();

	uint32_t getDelayInMilliseconds();

	uint32_t getNeededListCapacity() override;

	void writeToSDKList(CScanLabSDK* pSDK, uint32_t nCardNo) override;

};


class CGPIOTask_WaitforInput : public CGPIOTask
{
private:

	uint32_t m_nInputBit;
	bool m_bInputValue;
	uint32_t m_nMaxDelayInMilliseconds;
public:

	CGPIOTask_WaitforInput(uint32_t nRelativeStartAddress, const uint32_t nInputBit, const bool bInputValue, const uint32_t nMaxDelayInMilliseconds);

	virtual ~CGPIOTask_WaitforInput();

	uint32_t getInputBit ();

	bool getInputValue ();

	uint32_t getMaxDelayInMilliseconds ();

	uint32_t getNeededListCapacity() override;

	void writeToSDKList(CScanLabSDK* pSDK, uint32_t nCardNo) override;

};


class CGPIOTask_Label : public CGPIOTask
{
private:
	
	std::string m_sLabelName;

	uint32_t m_nMaxPasses;

public:

	CGPIOTask_Label(uint32_t nRelativeStartAddress, const std::string& sLabelName, const uint32_t nMaxPasses);

	virtual ~CGPIOTask_Label();

	std::string getLabelName ();

	uint32_t getMaxPasses ();

	uint32_t getNeededListCapacity() override;

	void writeToSDKList(CScanLabSDK* pSDK, uint32_t nCardNo) override;

};


class CGPIOTask_GoToLabel : public CGPIOTask
{
private:

	std::string m_sLabelName;

public:

	CGPIOTask_GoToLabel(uint32_t nRelativeStartAddress, const std::string& sLabelName);

	virtual ~CGPIOTask_GoToLabel();

	std::string getLabelName();

	uint32_t getNeededListCapacity() override;

	void writeToSDKList(CScanLabSDK* pSDK, uint32_t nCardNo) override;

};


class CGPIOTask_ConditionalGoToLabel : public CGPIOTask
{
private:

	uint32_t m_nInputBit;

	bool m_bInputValue;

	std::string m_sLabelName;

public:

	CGPIOTask_ConditionalGoToLabel(uint32_t nRelativeStartAddress, const uint32_t nInputBit, const bool bInputValue, const std::string& sLabelName);

	virtual ~CGPIOTask_ConditionalGoToLabel();

	uint32_t getInputBit ();

	bool getInputValue ();

	std::string getLabelName();

	uint32_t getNeededListCapacity() override;

	void writeToSDKList(CScanLabSDK* pSDK, uint32_t nCardNo) override;

};

typedef std::shared_ptr<CGPIOTask> PGPIOTask;


class CGPIOSequenceInstance {
private:

	std::string m_sIdentifier;

	uint32_t m_nListSize;

	std::vector<PGPIOTask> m_Tasks;

public:

	CGPIOSequenceInstance (const std::string & sIdentifier);
	
	virtual ~CGPIOSequenceInstance ();

	std::string GetIdentifier();

	void Clear();

	void AddOutput(const uint32_t nOutputBit, const bool bOutputValue);

	void AddDelay(const uint32_t nDelayInMilliseconds);

	void WaitforInput(const uint32_t nInputBit, const bool bInputValue, const uint32_t nMaxDelayInMilliseconds);

	void AddLabel(const std::string & sLabelName, const uint32_t nMaxPasses);

	void GoToLabel(const std::string & sLabelName);

	void ConditionalGoToLabel(const uint32_t nInputBit, const bool bInputValue, const std::string & sLabelName);

};

typedef std::shared_ptr<CGPIOSequenceInstance> PGPIOSequenceInstance;

}
}

#endif // __LIBMCDRIVER_SCANLAB_GPIOSEQUENCEINSTANCE
