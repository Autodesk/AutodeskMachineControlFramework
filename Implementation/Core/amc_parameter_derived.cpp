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


#define _PARAMETER_HEADERPROTECTION

#include "amc_parameter_derived.hpp"
#include "amc_parametergroup.hpp"
#include "libmc_exceptiontypes.hpp"

namespace AMC {




	CParameter_Derived::CParameter_Derived(const std::string& sName, PParameterGroup pParameterGroup, std::string sSourceParameterName)
		: m_sName (sName), m_pParameterGroup(pParameterGroup), m_sSourceParameterName(sSourceParameterName)
	{
		LibMCAssertNotNull(pParameterGroup.get());

		// Ensure that source parameter exists
		std::string sDescription, sDefaultValue;
		m_pParameterGroup->getParameterInfoByName(sSourceParameterName, sDescription, sDefaultValue);
	}

	CParameter_Derived::~CParameter_Derived()
	{

	}

	std::string CParameter_Derived::getName() const
	{
		return m_sName;
	}

	std::string CParameter_Derived::getDescription() const
	{
		std::string sDescription, sDefaultValue;
		m_pParameterGroup->getParameterInfoByName(m_sSourceParameterName, sDescription, sDefaultValue);

		return sDescription;
	}

	std::string CParameter_Derived::getDefaultValue() const
	{
		std::string sDescription, sDefaultValue;
		m_pParameterGroup->getParameterInfoByName(m_sSourceParameterName, sDescription, sDefaultValue);

		return sDefaultValue;

	}

	std::string CParameter_Derived::getStringValue() const
	{
		return m_pParameterGroup->getParameterValueByName(m_sSourceParameterName);

	}

	void CParameter_Derived::setStringValue(const std::string& sValue)
	{
		throw ELibMCCustomException (LIBMC_ERROR_DERIVEDPARAMETERSREADONLY, m_sName);
	}


	double CParameter_Derived::getDoubleValue() const
	{
		return m_pParameterGroup->getDoubleParameterValueByName(m_sSourceParameterName);
	}

	void CParameter_Derived::setDoubleValue(const double dValue)
	{
		throw ELibMCCustomException(LIBMC_ERROR_DERIVEDPARAMETERSREADONLY, m_sName);
	}

	int64_t CParameter_Derived::getIntValue() const
	{
		return m_pParameterGroup->getIntParameterValueByName(m_sSourceParameterName);
	}

	void CParameter_Derived::setIntValue(const int64_t nValue)
	{
		throw ELibMCCustomException(LIBMC_ERROR_DERIVEDPARAMETERSREADONLY, m_sName);
	}

	bool CParameter_Derived::getBoolValue() const
	{
		return m_pParameterGroup->getBoolParameterValueByName(m_sSourceParameterName);
	}

	void CParameter_Derived::setBoolValue(const bool bValue)
	{
		throw ELibMCCustomException(LIBMC_ERROR_DERIVEDPARAMETERSREADONLY, m_sName);
	}


	PParameter CParameter_Derived::duplicate()
	{
		return std::make_shared<CParameter_Derived>(m_sName, m_pParameterGroup, m_sSourceParameterName);
	}




}


