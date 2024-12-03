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

#include "libmcdriver_scanlab_gpiosequence.hpp"
#include "libmcdriver_scanlab_interfaceexception.hpp"

using namespace LibMCDriver_ScanLab::Impl;

/*************************************************************************************************************************
 Class definition of CGPIOSequence 
**************************************************************************************************************************/

CGPIOSequence::CGPIOSequence(PGPIOSequenceInstance pInstance)
	: m_pInstance (pInstance)
{
	if (pInstance.get() == nullptr)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_NOTIMPLEMENTED);

}

CGPIOSequence::~CGPIOSequence()
{

}


std::string CGPIOSequence::GetIdentifier()
{
	return m_pInstance->GetIdentifier();
}

void CGPIOSequence::Clear()
{
	m_pInstance->Clear();
}

void CGPIOSequence::AddOutput(const LibMCDriver_ScanLab_uint32 nOutputBit, const bool bOutputValue)
{
	m_pInstance->AddOutput(nOutputBit, bOutputValue);
}

void CGPIOSequence::AddDelay(const LibMCDriver_ScanLab_uint32 nDelayInMilliseconds)
{
	m_pInstance->AddDelay(nDelayInMilliseconds);
}

void CGPIOSequence::WaitforInput(const LibMCDriver_ScanLab_uint32 nInputBit, const bool bInputValue, const LibMCDriver_ScanLab_uint32 nMaxDelayInMilliseconds)
{
	m_pInstance->WaitforInput(nInputBit, bInputValue, nMaxDelayInMilliseconds);
}

void CGPIOSequence::AddLabel(const std::string & sLabelName, const LibMCDriver_ScanLab_uint32 nMaxPasses)
{
	m_pInstance->AddLabel(sLabelName, nMaxPasses);
}

void CGPIOSequence::GoToLabel(const std::string & sLabelName)
{
	m_pInstance->GoToLabel(sLabelName);
}

void CGPIOSequence::ConditionalGoToLabel(const LibMCDriver_ScanLab_uint32 nInputBit, const bool bInputValue, const std::string & sLabelName)
{
	m_pInstance->ConditionalGoToLabel (nInputBit, bInputValue, sLabelName);
}

