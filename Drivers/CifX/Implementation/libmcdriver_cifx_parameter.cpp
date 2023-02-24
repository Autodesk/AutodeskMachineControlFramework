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

#include "libmcdriver_cifx_parameter.hpp"
#include "libmcdriver_cifx_interfaceexception.hpp"

// Include custom headers here.


using namespace LibMCDriver_CifX::Impl;

/*************************************************************************************************************************
 Class definition of CChannelInformation 
**************************************************************************************************************************/

CDriver_CifXParameter::CDriver_CifXParameter(const std::string& sName, const std::string& sDescription, eDriver_CifXParameterType Type, const uint32_t nAddress)
	: m_sName(sName),
	m_sDescription(sDescription),
	m_eType(Type),
	m_nAddress(nAddress),
	m_bIsBigEndian(true)
{

}

CDriver_CifXParameter::~CDriver_CifXParameter()
{

}

std::string CDriver_CifXParameter::getName()
{
	return m_sName;
}

std::string CDriver_CifXParameter::getDescription()
{
	return m_sDescription;
}

eDriver_CifXParameterType CDriver_CifXParameter::getType()
{
	return m_eType;
}

void CDriver_CifXParameter::getValueRange(int64_t& nMinValue, int64_t& nMaxValue)
{
	getValueRangeFromType(m_eType, nMinValue, nMaxValue);
}

void CDriver_CifXParameter::getValueRangeFromType(eDriver_CifXParameterType parameterType, int64_t& nMinValue, int64_t& nMaxValue)
{
	switch (parameterType) {

	case eDriver_CifXParameterType::CifXParameter_BOOL:
		nMinValue = 0;
		nMaxValue = 1;
		break;

	case eDriver_CifXParameterType::CifXParameter_UINT8:
		nMinValue = 0;
		nMaxValue = UINT8_MAX;
		break;

	case eDriver_CifXParameterType::CifXParameter_UINT16:
		nMinValue = 0;
		nMaxValue = UINT16_MAX;
		break;

	case eDriver_CifXParameterType::CifXParameter_UINT32:
		nMinValue = 0;
		nMaxValue = UINT32_MAX;
		break;

	case eDriver_CifXParameterType::CifXParameter_INT8:
		nMinValue = INT8_MIN;
		nMaxValue = INT8_MAX;
		break;

	case eDriver_CifXParameterType::CifXParameter_INT16:
		nMinValue = INT16_MIN;
		nMaxValue = INT16_MAX;
		break;

	case eDriver_CifXParameterType::CifXParameter_INT32:
		nMinValue = INT32_MIN;
		nMaxValue = INT32_MAX;
		break;

	default:
		nMinValue = 0;
		nMaxValue = 0;
	}
}

eDriver_AbstractParameterType CDriver_CifXParameter::getAbstractType()
{
	switch (m_eType) {
	case eDriver_CifXParameterType::CifXParameter_BOOL:
		return eDriver_AbstractParameterType::CifXAbstractParameter_BOOL;

	case eDriver_CifXParameterType::CifXParameter_UINT8:
	case eDriver_CifXParameterType::CifXParameter_UINT16:
	case eDriver_CifXParameterType::CifXParameter_UINT32:
	case eDriver_CifXParameterType::CifXParameter_INT8:
	case eDriver_CifXParameterType::CifXParameter_INT16:
	case eDriver_CifXParameterType::CifXParameter_INT32:
		return eDriver_AbstractParameterType::CifXAbstractParameter_INT;

	case eDriver_CifXParameterType::CifXParameter_FLOAT:
	case eDriver_CifXParameterType::CifXParameter_DOUBLE:
		return eDriver_AbstractParameterType::CifXAbstractParameter_DOUBLE;

	default:
		return eDriver_AbstractParameterType::CifXAbstractParameter_Unknown;
	}
}


double CDriver_CifXParameter::GetActualDoubleValue()
{
	return 0.0;
}

int64_t CDriver_CifXParameter::GetActualIntegerValue()
{
	return 0;
}

bool CDriver_CifXParameter::GetActualBoolValue()
{
	return GetActualIntegerValue() != 0;
}

void CDriver_CifXParameter::SetActualDoubleValue(double dValue)
{

}

void CDriver_CifXParameter::SetActualIntegerValue(int64_t nValue)
{

}



void CDriver_CifXParameter::SetActualBoolValue(bool bValue)
{
	if (bValue)
		SetActualIntegerValue(1);
	else
		SetActualIntegerValue(0);

}

double CDriver_CifXParameter::GetTargetDoubleValue()
{
	return 0.0;
}

int64_t CDriver_CifXParameter::GetTargetIntegerValue()
{
	return 0;
}

bool CDriver_CifXParameter::GetTargetBoolValue()
{
	return GetTargetIntegerValue() != 0;
}

void CDriver_CifXParameter::SetTargetDoubleValue(double dValue)
{

}

void CDriver_CifXParameter::SetTargetIntegerValue(int64_t nValue)
{

}

void CDriver_CifXParameter::SetTargetBoolValue(bool bValue)
{
	if (bValue)
		SetTargetIntegerValue(1);
	else
		SetTargetIntegerValue(0);
}

uint32_t CDriver_CifXParameter::getAddress()
{
	return m_nAddress;
}

bool CDriver_CifXParameter::isBigEndian()
{
	return m_bIsBigEndian;
}


CDriver_CifXParameter_Integer::CDriver_CifXParameter_Integer(const std::string& sName, const std::string& sDescription, eDriver_CifXParameterType Type, const uint32_t nAddress)
	: CDriver_CifXParameter(sName, sDescription, Type, nAddress), m_nActualValue(0), m_nTargetValue(0)
{

}

CDriver_CifXParameter_Integer::~CDriver_CifXParameter_Integer()
{

}


int64_t CDriver_CifXParameter_Integer::GetActualIntegerValue()
{
	return m_nActualValue;
}

void CDriver_CifXParameter_Integer::SetActualIntegerValue(int64_t nValue)
{
	m_nActualValue = nValue;
}

int64_t CDriver_CifXParameter_Integer::GetTargetIntegerValue()
{
	return m_nTargetValue;
}

void CDriver_CifXParameter_Integer::SetTargetIntegerValue(int64_t nValue)
{
	m_nTargetValue = nValue;
}

CDriver_CifXParameter_Double::CDriver_CifXParameter_Double(const std::string& sName, const std::string& sDescription, eDriver_CifXParameterType Type, const uint32_t nAddress)
	: CDriver_CifXParameter(sName, sDescription, Type, nAddress), m_dActualValue(0.0), m_dTargetValue(0.0)
{

}

CDriver_CifXParameter_Double::~CDriver_CifXParameter_Double()
{

}

double CDriver_CifXParameter_Double::GetActualDoubleValue()
{
	return m_dActualValue;
}

void CDriver_CifXParameter_Double::SetActualDoubleValue(double dValue)
{
	m_dActualValue = dValue;
}

double CDriver_CifXParameter_Double::GetTargetDoubleValue()
{
	return m_dTargetValue;
}

void CDriver_CifXParameter_Double::SetTargetDoubleValue(double dValue)
{
	m_dTargetValue = dValue;
}

CDriver_CifXParameter_Bool::CDriver_CifXParameter_Bool(const std::string& sName, const std::string& sDescription, eDriver_CifXParameterType Type, const uint32_t nAddress, const uint32_t nBit)
	: CDriver_CifXParameter_Integer(sName, sDescription, Type, nAddress), m_nBit(nBit)
{
	if (nBit >= 8)
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDBITINDEX);
}

CDriver_CifXParameter_Bool::~CDriver_CifXParameter_Bool()
{

}

uint32_t CDriver_CifXParameter_Bool::getBit()
{
	return m_nBit;
}

