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


Abstract: This is a stub class definition of CGPIOSequence

*/

#include "libmcdriver_scanlab_gpiosequenceinstance.hpp"
#include "libmcdriver_scanlab_interfaceexception.hpp"

using namespace LibMCDriver_ScanLab::Impl;

#define GPIOSEQUENCE_MAXIDENTIFIERLENGTH 512
#define GPIOSEQUENCE_MAXDELAYINMS 120000000
#define GPIOSEQUENCE_MAXINPUTBIT 15
#define GPIOSEQUENCE_MAXOUTPUTBIT 15

CGPIOTask::CGPIOTask(uint32_t nRelativeStartAddress)
    : m_nRelativeStartAddress (nRelativeStartAddress)
{

}

CGPIOTask::~CGPIOTask()
{

}

CGPIOTask_Output::CGPIOTask_Output(uint32_t nRelativeStartAddress, const uint32_t nOutputBit, const bool bOutputValue)
    : CGPIOTask (nRelativeStartAddress), m_nOutputBit (nOutputBit), m_bOutputValue (bOutputValue)
{

}

CGPIOTask_Output::~CGPIOTask_Output()
{

}

uint32_t CGPIOTask_Output::getOutputBit()
{
    return m_nOutputBit;
}

bool CGPIOTask_Output::getOutputValue()
{
    return m_bOutputValue;
}

uint32_t CGPIOTask_Output::getNeededListCapacity()
{
    return 1;
}


void CGPIOTask_Output::writeToSDKList(CScanLabSDK* pSDK, uint32_t nCardNo)
{
    if (pSDK == nullptr)
        throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);

    uint32_t nBitmask = 1UL << m_nOutputBit;
    uint32_t nSelectionValue = 0;
    if (m_bOutputValue)
        nSelectionValue = nBitmask;

    pSDK->n_write_io_port_mask_list(nCardNo, nSelectionValue, nBitmask);
}



CGPIOTask_Delay::CGPIOTask_Delay(uint32_t nRelativeStartAddress, const uint32_t nDelayInMilliseconds)
    : CGPIOTask (nRelativeStartAddress), m_nDelayInMilliseconds (nDelayInMilliseconds)
{
    if ((nDelayInMilliseconds == 0) || (nDelayInMilliseconds > GPIOSEQUENCE_MAXDELAYINMS))
        throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDGPIOTASKDELAY);
    if ((nDelayInMilliseconds % 10) != 0)
        throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_GPIOTASKDELAYMUSTBEMULTIPLEOF10);
}

CGPIOTask_Delay::~CGPIOTask_Delay()
{

}

uint32_t CGPIOTask_Delay::getDelayInMilliseconds()
{
    return m_nDelayInMilliseconds;
}

uint32_t CGPIOTask_Delay::getNeededListCapacity()
{
    return 1;
}

void CGPIOTask_Delay::writeToSDKList(CScanLabSDK* pSDK, uint32_t nCardNo)
{
    if (pSDK == nullptr)
        throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);

    uint32_t nDelayInTicks = m_nDelayInMilliseconds / 10;

    pSDK->n_long_delay(nCardNo, nDelayInTicks);
}


CGPIOTask_WaitforInput::CGPIOTask_WaitforInput(uint32_t nRelativeStartAddress, const uint32_t nInputBit, const bool bInputValue, const uint32_t nMaxDelayInMilliseconds)
    : CGPIOTask (nRelativeStartAddress), m_nInputBit (nInputBit), m_bInputValue (bInputValue), m_nMaxDelayInMilliseconds (nMaxDelayInMilliseconds)
{
    if ((nMaxDelayInMilliseconds == 0) || (nMaxDelayInMilliseconds > GPIOSEQUENCE_MAXDELAYINMS))
        throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDGPIOTASKDELAY);
    if ((nMaxDelayInMilliseconds % 10) != 0)
        throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_GPIOTASKDELAYMUSTBEMULTIPLEOF10);

}

CGPIOTask_WaitforInput::~CGPIOTask_WaitforInput()
{

}

uint32_t CGPIOTask_WaitforInput::getInputBit()
{
    return m_nInputBit;
}

bool CGPIOTask_WaitforInput::getInputValue()
{
    return m_bInputValue;
}

uint32_t CGPIOTask_WaitforInput::getMaxDelayInMilliseconds()
{
    return m_nMaxDelayInMilliseconds;
}

uint32_t CGPIOTask_WaitforInput::getNeededListCapacity()
{
    return 0;
}

void CGPIOTask_WaitforInput::writeToSDKList(CScanLabSDK* pSDK, uint32_t nCardNo)
{
    if (pSDK == nullptr)
        throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);

}


CGPIOTask_Label::CGPIOTask_Label(uint32_t nRelativeStartAddress, const std::string& sLabelName, const uint32_t nMaxPasses)
    : CGPIOTask (nRelativeStartAddress), m_sLabelName (sLabelName), m_nMaxPasses (nMaxPasses)
{

}

CGPIOTask_Label::~CGPIOTask_Label()
{

}

std::string CGPIOTask_Label::getLabelName()
{
    return m_sLabelName;
}

uint32_t CGPIOTask_Label::getMaxPasses()
{
    return m_nMaxPasses;
}

uint32_t CGPIOTask_Label::getNeededListCapacity()
{
    return 0;
}

void CGPIOTask_Label::writeToSDKList(CScanLabSDK* pSDK, uint32_t nCardNo)
{
    if (pSDK == nullptr)
        throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);

}


CGPIOTask_GoToLabel::CGPIOTask_GoToLabel(uint32_t nRelativeStartAddress, const std::string& sLabelName)
: CGPIOTask (nRelativeStartAddress), m_sLabelName (sLabelName)
{

}

CGPIOTask_GoToLabel::~CGPIOTask_GoToLabel()
{

}

std::string CGPIOTask_GoToLabel::getLabelName()
{
     return m_sLabelName;
}


uint32_t CGPIOTask_GoToLabel::getNeededListCapacity()
{
    return 0;
}

void CGPIOTask_GoToLabel::writeToSDKList(CScanLabSDK* pSDK, uint32_t nCardNo)
{
    if (pSDK == nullptr)
        throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);

}


CGPIOTask_ConditionalGoToLabel::CGPIOTask_ConditionalGoToLabel(uint32_t nRelativeStartAddress, const uint32_t nInputBit, const bool bInputValue, const std::string& sLabelName)
    : CGPIOTask (nRelativeStartAddress), m_nInputBit (nInputBit), m_bInputValue (bInputValue), m_sLabelName (sLabelName)
{
    
}

CGPIOTask_ConditionalGoToLabel::~CGPIOTask_ConditionalGoToLabel()
{

}

uint32_t CGPIOTask_ConditionalGoToLabel::getInputBit()
{
    return m_nInputBit;
}

bool CGPIOTask_ConditionalGoToLabel::getInputValue()
{
    return m_bInputValue;
}

std::string CGPIOTask_ConditionalGoToLabel::getLabelName()
{
    return m_sLabelName;
}

uint32_t CGPIOTask_ConditionalGoToLabel::getNeededListCapacity()
{
    return 0;
}

void CGPIOTask_ConditionalGoToLabel::writeToSDKList(CScanLabSDK* pSDK, uint32_t nCardNo)
{
    if (pSDK == nullptr)
        throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);

}


CGPIOSequenceInstance::CGPIOSequenceInstance(const std::string& sIdentifier)
    : m_sIdentifier(sIdentifier), m_nListSize (0)
{
    if (sIdentifier.empty () || (sIdentifier.length () > GPIOSEQUENCE_MAXIDENTIFIERLENGTH))
        throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDSEQUENCEIDENTIFIER);

    for (auto ch : sIdentifier) {
        if (!(isalnum(ch) || (ch == '-') || (ch == '_')))
            throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDSEQUENCEIDENTIFIER);
    }

}

CGPIOSequenceInstance::~CGPIOSequenceInstance()
{

}

std::string CGPIOSequenceInstance::GetIdentifier()
{
    return m_sIdentifier;
}

void CGPIOSequenceInstance::Clear()
{
    m_Tasks.clear();
    m_nListSize = 0;
}

void CGPIOSequenceInstance::AddOutput(const LibMCDriver_ScanLab_uint32 nOutputBit, const bool bOutputValue)
{
    auto pTask = std::make_shared<CGPIOTask_Output>(m_nListSize, nOutputBit, bOutputValue);
    m_nListSize += pTask->getNeededListCapacity();
    m_Tasks.push_back (pTask);
}

void CGPIOSequenceInstance::AddDelay(const LibMCDriver_ScanLab_uint32 nDelayInMilliseconds)
{
    auto pTask = std::make_shared<CGPIOTask_Delay> (m_nListSize, nDelayInMilliseconds);
    m_nListSize += pTask->getNeededListCapacity();
    m_Tasks.push_back(pTask);
}


void CGPIOSequenceInstance::WaitforInput(const LibMCDriver_ScanLab_uint32 nInputBit, const bool bInputValue, const LibMCDriver_ScanLab_uint32 nMaxDelayInMilliseconds)
{
    auto pTask = std::make_shared<CGPIOTask_WaitforInput> (m_nListSize, nInputBit, bInputValue, nMaxDelayInMilliseconds);
    m_nListSize += pTask->getNeededListCapacity();
    m_Tasks.push_back(pTask);
}

void CGPIOSequenceInstance::AddLabel(const std::string& sLabelName, const LibMCDriver_ScanLab_uint32 nMaxPasses)
{
    auto pTask = std::make_shared<CGPIOTask_Label> (m_nListSize, sLabelName, nMaxPasses);
    m_nListSize += pTask->getNeededListCapacity();
    m_Tasks.push_back(pTask);
}

void CGPIOSequenceInstance::GoToLabel(const std::string& sLabelName)
{
    auto pTask = std::make_shared<CGPIOTask_GoToLabel> (m_nListSize, sLabelName);
    m_nListSize += pTask->getNeededListCapacity();
    m_Tasks.push_back(pTask);
}

void CGPIOSequenceInstance::ConditionalGoToLabel(const LibMCDriver_ScanLab_uint32 nInputBit, const bool bInputValue, const std::string& sLabelName)
{
    auto pTask = std::make_shared<CGPIOTask_ConditionalGoToLabel>(m_nListSize, nInputBit, bInputValue, sLabelName);
    m_nListSize += pTask->getNeededListCapacity();
    m_Tasks.push_back(pTask);
}

