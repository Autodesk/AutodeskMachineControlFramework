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

#include "amc_parameter_instancestate.hpp"
#include "libmc_exceptiontypes.hpp"

namespace AMC {




	CParameter_InstanceState::CParameter_InstanceState(const std::string& sName, const std::string& sDescription)
		: m_sName(sName), m_sDescription (sDescription)
	{
	}

	CParameter_InstanceState::~CParameter_InstanceState()
	{

	}

	std::string CParameter_InstanceState::getName() const
	{
		return m_sName;
	}

	std::string CParameter_InstanceState::getDescription() const
	{
		return m_sDescription;
	}

	std::string CParameter_InstanceState::getDefaultValue() const
	{

		return "";
	}

	std::string CParameter_InstanceState::getStringValue() const
	{
		return "";

	}

	void CParameter_InstanceState::setStringValue(const std::string& sValue)
	{
		throw ELibMCCustomException(LIBMC_ERROR_DERIVEDPARAMETERSREADONLY, m_sName);
	}


	double CParameter_InstanceState::getDoubleValue() const
	{
		return 0.0;
	}

	void CParameter_InstanceState::setDoubleValue(const double dValue)
	{
		throw ELibMCCustomException(LIBMC_ERROR_DERIVEDPARAMETERSREADONLY, m_sName);
	}

	int64_t CParameter_InstanceState::getIntValue() const
	{
		return 0;
	}

	void CParameter_InstanceState::setIntValue(const int64_t nValue)
	{
		throw ELibMCCustomException(LIBMC_ERROR_DERIVEDPARAMETERSREADONLY, m_sName);
	}

	bool CParameter_InstanceState::getBoolValue() const
	{
		return false;
	}

	void CParameter_InstanceState::setBoolValue(const bool bValue)
	{
		throw ELibMCCustomException(LIBMC_ERROR_DERIVEDPARAMETERSREADONLY, m_sName);
	}


	PParameter CParameter_InstanceState::duplicate()
	{
		return std::make_shared<CParameter_InstanceState>(m_sName, m_sDescription);
	}




}


