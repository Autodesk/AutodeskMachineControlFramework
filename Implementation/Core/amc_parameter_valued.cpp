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

#include "amc_parameter_valued.hpp"
#include "amc_statejournal.hpp"
#include "libmc_exceptiontypes.hpp"

namespace AMC {

	CParameter_Valued::CParameter_Valued(const std::string& sName, const std::string& sDescription, const std::string & sDefaultValue, PStateJournal pJournal, uint32_t nJournalVariableID)
		: m_sName(sName), m_sDescription (sDescription), m_sDefaultValue (sDefaultValue), m_nJournalVariableID(nJournalVariableID), m_pJournal (pJournal)
	{
		if (sName.length() == 0)
			throw ELibMCInterfaceException(LIBMC_ERROR_EMPTYPARAMETERNAME);

		m_sValue = m_sDefaultValue;
	}

	CParameter_Valued::CParameter_Valued(const std::string& sName, const std::string& sDescription, const double dDefaultValue, PStateJournal pJournal, uint32_t nJournalVariableID)
		: m_sName(sName), m_sDescription(sDescription), m_sDefaultValue(std::to_string (dDefaultValue)), m_nJournalVariableID(nJournalVariableID), m_pJournal (pJournal)
	{
		if (sName.length() == 0)
			throw ELibMCInterfaceException(LIBMC_ERROR_EMPTYPARAMETERNAME);

		m_sValue = m_sDefaultValue;
	}

	CParameter_Valued::CParameter_Valued(const std::string& sName, const std::string& sDescription, const int64_t nDefaultValue, PStateJournal pJournal, uint32_t nJournalVariableID)
		: m_sName(sName), m_sDescription(sDescription), m_sDefaultValue(std::to_string(nDefaultValue)), m_nJournalVariableID(nJournalVariableID), m_pJournal (pJournal)
	{
		if (sName.length() == 0)
			throw ELibMCInterfaceException(LIBMC_ERROR_EMPTYPARAMETERNAME);

		m_sValue = m_sDefaultValue;
	}

	CParameter_Valued::CParameter_Valued(const std::string& sName, const std::string& sDescription, const bool bDefaultValue, PStateJournal pJournal, uint32_t nJournalVariableID)
		: m_sName(sName), m_sDescription(sDescription), m_sDefaultValue(bDefaultValue ? "1" : "0"), m_nJournalVariableID(nJournalVariableID), m_pJournal (pJournal)
	{
		if (sName.length() == 0)
			throw ELibMCInterfaceException(LIBMC_ERROR_EMPTYPARAMETERNAME);

		m_sValue = m_sDefaultValue;
	}


	CParameter_Valued::~CParameter_Valued()
	{
	}

	std::string CParameter_Valued::getName() const
	{
		return std::string(m_sName.c_str());
	}

	std::string CParameter_Valued::getDescription() const
	{
		return std::string(m_sDescription.c_str());
	}

	std::string CParameter_Valued::getDefaultValue() const
	{
		return std::string(m_sDefaultValue.c_str());
	}

	// The following calls are not thread-safe and need to be mutexed in ParameterGroup!
	std::string CParameter_Valued::getStringValue() const
	{
		return std::string(m_sValue.c_str());
	}

	void CParameter_Valued::setStringValue(const std::string& sValue)
	{
		if (m_pJournal.get () != nullptr)
			m_pJournal->updateStringValue(m_nJournalVariableID, sValue);
		m_sValue = sValue;
	}

	double CParameter_Valued::getDoubleValue() const
	{
		return std::stod (m_sValue);
	}

	void CParameter_Valued::setDoubleValue(const double dValue)
	{
		if (m_pJournal.get() != nullptr)
			m_pJournal->updateDoubleValue(m_nJournalVariableID, dValue);
		m_sValue = std::to_string(dValue);
	}

	int64_t CParameter_Valued::getIntValue() const
	{
		return std::stoi(m_sValue);
	}

	void CParameter_Valued::setIntValue(const int64_t nValue)
	{
		if (m_pJournal.get() != nullptr)
			m_pJournal->updateIntegerValue(m_nJournalVariableID, nValue);
		m_sValue = std::to_string(nValue);
	}

	bool CParameter_Valued::getBoolValue() const
	{
		return getIntValue() != 0;
	}

	void CParameter_Valued::setBoolValue(const bool bValue)
	{
		if (m_pJournal.get() != nullptr)
			m_pJournal->updateBoolValue(m_nJournalVariableID, bValue);
		if (bValue)
			m_sValue = "1";
		else
			m_sValue = "0";
	}

	PParameter CParameter_Valued::duplicate()
	{
		auto pParameter = std::make_shared<CParameter_Valued>(m_sName, m_sDescription, m_sDefaultValue, m_pJournal, m_nJournalVariableID);
		pParameter->m_sValue = m_sValue;
		return pParameter;
	}


}


