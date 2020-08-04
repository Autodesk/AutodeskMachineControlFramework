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

#include "amc_parameter.hpp"
#include "libmc_interfaceexception.hpp"

namespace AMC {

	CParameter::CParameter(const std::string& sName, const std::string& sDescription, const std::string & sDefaultValue)
		: m_sName(sName), m_sDescription (sDescription), m_sDefaultValue (sDefaultValue)
	{
		if (sName.length() == 0)
			throw ELibMCInterfaceException(LIBMC_ERROR_EMPTYPARAMETERNAME);

		setStringValue(sDefaultValue);
	}

	CParameter::CParameter(const std::string& sName, const std::string& sDescription, const double dDefaultValue)
		: m_sName(sName), m_sDescription(sDescription), m_sDefaultValue(std::to_string (dDefaultValue))
	{
		if (sName.length() == 0)
			throw ELibMCInterfaceException(LIBMC_ERROR_EMPTYPARAMETERNAME);

		setDoubleValue(dDefaultValue);
	}

	CParameter::CParameter(const std::string& sName, const std::string& sDescription, const int64_t nDefaultValue)
		: m_sName(sName), m_sDescription(sDescription), m_sDefaultValue(std::to_string(nDefaultValue))
	{
		if (sName.length() == 0)
			throw ELibMCInterfaceException(LIBMC_ERROR_EMPTYPARAMETERNAME);

		setIntValue(nDefaultValue);
	}

	CParameter::CParameter(const std::string& sName, const std::string& sDescription, const bool bDefaultValue)
		: m_sName(sName), m_sDescription(sDescription), m_sDefaultValue(bDefaultValue ? "1" : "0")
	{
		if (sName.length() == 0)
			throw ELibMCInterfaceException(LIBMC_ERROR_EMPTYPARAMETERNAME);

		setBoolValue(bDefaultValue);
	}


	CParameter::~CParameter()
	{
	}

	std::string CParameter::getName() const
	{
		return std::string(m_sName.c_str());
	}

	std::string CParameter::getDescription() const
	{
		return std::string(m_sDescription.c_str());
	}

	std::string CParameter::getDefaultValue() const
	{
		return std::string(m_sDefaultValue.c_str());
	}

	// The following calls are not thread-safe and need to be mutexed in ParameterGroup!
	std::string CParameter::getStringValue() const
	{
		return std::string(m_sValue.c_str());
	}

	void CParameter::setStringValue(const std::string& sValue)
	{
		m_sValue = sValue;
	}

	double CParameter::getDoubleValue() const
	{
		return std::stod (m_sValue);
	}

	void CParameter::setDoubleValue(const double dValue)
	{
		m_sValue = std::to_string(dValue);
	}

	int64_t CParameter::getIntValue() const
	{
		return std::stoi(m_sValue);
	}

	void CParameter::setIntValue(const int64_t nValue)
	{
		m_sValue = std::to_string(nValue);
	}

	bool CParameter::getBoolValue() const
	{
		return getIntValue() != 0;
	}

	void CParameter::setBoolValue(const bool bValue) 
	{
		if (bValue)
			setIntValue(1);
		else
			setIntValue(0);
	}


}


