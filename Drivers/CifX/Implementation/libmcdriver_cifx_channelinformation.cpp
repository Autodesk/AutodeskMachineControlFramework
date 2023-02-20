/*++

Copyright (C) 2023 Autodesk Inc.

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


Abstract: This is a stub class definition of CChannelInformation

*/

#include "libmcdriver_cifx_channelinformation.hpp"
#include "libmcdriver_cifx_interfaceexception.hpp"

// Include custom headers here.


using namespace LibMCDriver_CifX::Impl;

/*************************************************************************************************************************
 Class definition of CChannelInformation 
**************************************************************************************************************************/

CChannelInformation::CChannelInformation(PDriver_CifXChannel pChannel)
	: m_pChannel (pChannel)
{
	if (pChannel.get () == nullptr)
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDPARAM);

}

CChannelInformation::~CChannelInformation()
{

}


std::string CChannelInformation::GetBoardName()
{
	return m_pChannel->getBoardName();
}

LibMCDriver_CifX_uint32 CChannelInformation::GetChannelIndex()
{
	return m_pChannel->getChannelIndex();
}

bool CChannelInformation::IsConnected()
{
	return m_pChannel->isConnected();
}

LibMCDriver_CifX_uint32 CChannelInformation::GetMillisecondsSinceLastUpdate()
{
	throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_NOTIMPLEMENTED);
}

void CChannelInformation::GetConnectionStatistics(LibMCDriver_CifX_uint32 & nNumberOfSucceededUpdates, LibMCDriver_CifX_uint32 & nNumberOfUpdateErrors, LibMCDriver_CifX_double & dMinimumUpdateDurationInMs, LibMCDriver_CifX_double & dMaximumUpdateDurationInMs, LibMCDriver_CifX_double & dAverageUpdateDurationInMs, LibMCDriver_CifX_double & dUpdateDurationVarianceInMs)
{
	throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_NOTIMPLEMENTED);
}

bool CChannelInformation::ValueExists(const std::string & sName)
{
	auto pInputValue = m_pChannel->findInputValue(sName);
	if (pInputValue.get() != nullptr)
		return true;

	auto pOutputValue = m_pChannel->findOutputValue(sName);
	if (pOutputValue.get() != nullptr)
		return true;

	return false;
}

void CChannelInformation::GetValueType(const std::string & sName, LibMCDriver_CifX::eValueType & eValueType, bool & bIsInput, bool & bIsOutput)
{
	PDriver_CifXParameter pValue;
	bIsInput = false;
	bIsOutput = false;
	eValueType = LibMCDriver_CifX::eValueType::Unknown;

	auto pInputValue = m_pChannel->findInputValue(sName);
	if (pInputValue.get() != nullptr) {
		pValue = pInputValue;
		bIsInput = true;
	}

	auto pOutputValue = m_pChannel->findOutputValue(sName);
	if (pOutputValue.get() != nullptr) {
		pValue = pOutputValue;
		bIsOutput = true;
	}

	if (pValue.get () == nullptr)
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_VALUENOTFOUND, "value not found: " + sName);

	switch (pValue->getAbstractType()) {
		case eDriver_AbstractParameterType::CifXAbstractParameter_BOOL:
			eValueType = LibMCDriver_CifX::eValueType::BoolValue;
			break;
		case eDriver_AbstractParameterType::CifXAbstractParameter_INT:
			eValueType = LibMCDriver_CifX::eValueType::IntegerValue;
			break;
		case eDriver_AbstractParameterType::CifXAbstractParameter_DOUBLE:
			eValueType = LibMCDriver_CifX::eValueType::DoubleValue;
			break;
		default:
			break;
	}
	

}

void CChannelInformation::GetIntegerValueRange(const std::string & sName, LibMCDriver_CifX_int64 & nMinValue, LibMCDriver_CifX_int64 & nMaxValue)
{
	PDriver_CifXParameter pValue;

	auto pInputValue = m_pChannel->findInputValue(sName);
	if (pInputValue.get() != nullptr) 
		pValue = pInputValue;

	auto pOutputValue = m_pChannel->findOutputValue(sName);
	if (pOutputValue.get() != nullptr) 
		pValue = pOutputValue;

	if (pValue.get() == nullptr)
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_VALUENOTFOUND, "value not found: " + sName);

	pValue->getValueRange(nMinValue, nMaxValue);
}

LibMCDriver_CifX_uint32 CChannelInformation::GetInputValueCount()
{
	return m_pChannel->getInputCount();
}

std::string CChannelInformation::GetInputValueName(const LibMCDriver_CifX_uint32 nIndex)
{
	auto pInput = m_pChannel->getInputByIndex(nIndex);
	return pInput->getName ();
}

LibMCDriver_CifX_uint32 CChannelInformation::GetOutputValueCount()
{
	return m_pChannel->getOutputCount();
}

std::string CChannelInformation::GetOutputValueName(const LibMCDriver_CifX_uint32 nIndex)
{
	auto pOutput = m_pChannel->getInputByIndex(nIndex);
	return pOutput->getName();
}

