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
#include "amc_parametergroup.hpp"
#include "amc_parameter.hpp"
#include "amc_parameter_valued.hpp"
#include "amc_parameter_derived.hpp"
#include "amc_statejournal.hpp"

#include "amc_jsonwriter.hpp"

#include "libmc_interfaceexception.hpp"
#include "RapidJSON/document.h"
#include "RapidJSON/stringbuffer.h"
#include "RapidJSON/writer.h"

#define AMC_MAXPARAMETERCOUNT (1024 * 1024)

namespace AMC {
	

	CParameterGroup::CParameterGroup()
		: m_pStateJournal (nullptr)
	{

	}

	CParameterGroup::CParameterGroup(const std::string& sName, const std::string& sDescription)
		: m_sName(sName), m_sDescription(sDescription), m_pStateJournal (nullptr)
	{
	}

	CParameterGroup::~CParameterGroup()
	{
	}

	std::string CParameterGroup::getName()
	{
		return std::string (m_sName.c_str());
	}

	std::string CParameterGroup::getDescription()
	{
		return std::string (m_sDescription.c_str());
	}

	bool CParameterGroup::hasParameter(const std::string& sName)
	{
		std::lock_guard <std::mutex> lockGuard (m_GroupMutex);

		auto iter = m_Parameters.find(sName);
		return (iter != m_Parameters.end());
	}

	// No Mutex here!
	void CParameterGroup::addParameterInternal(PParameter pParameter)
	{
		if (pParameter.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

		auto sName = pParameter->getName();
		if (m_Parameters.find(sName) != m_Parameters.end())
			throw ELibMCInterfaceException(LIBMC_ERROR_DUPLICATEPARAMETERNAME);

		if (m_ParameterList.size() >= AMC_MAXPARAMETERCOUNT)
			throw ELibMCInterfaceException(LIBMC_ERROR_TOOMANYPARAMETERS);

		m_Parameters.insert(std::make_pair(sName, pParameter));
		m_ParameterList.push_back(pParameter);
	}

	uint32_t CParameterGroup::getParameterCount()
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);

		return (uint32_t)m_ParameterList.size();
	}


	void CParameterGroup::getParameterInfo(const uint32_t nIndex, std::string& sName, std::string& sDescription, std::string& sDefaultValue)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);
		if (nIndex >= m_ParameterList.size())
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDINDEX);

		auto pParameter = m_ParameterList[nIndex];
		sName = pParameter->getName();
		sDescription = pParameter->getDescription();
		sDefaultValue = pParameter->getDefaultValue();
	}

	void CParameterGroup::getParameterInfoByName(const std::string& sName, std::string& sDescription, std::string& sDefaultValue)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);
		auto iIter = m_Parameters.find(sName);

		if (iIter == m_Parameters.end())
			throw ELibMCInterfaceException(LIBMC_ERROR_PARAMETERNOTFOUND);

		sDescription = iIter->second->getDescription();
		sDefaultValue = iIter->second->getDefaultValue();

	}


	std::string CParameterGroup::getParameterValueByIndex(const uint32_t nIndex)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);
		if (nIndex >= m_ParameterList.size())
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDINDEX);

		auto pParameter = m_ParameterList[nIndex];
		return pParameter->getStringValue();
	}

	std::string CParameterGroup::getParameterValueByName(const std::string& sName)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);
		auto iIter = m_Parameters.find(sName);

		if (iIter == m_Parameters.end())
			throw ELibMCInterfaceException(LIBMC_ERROR_PARAMETERNOTFOUND);

		return iIter->second->getStringValue();
	}

	double CParameterGroup::getDoubleParameterValueByIndex(const uint32_t nIndex)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);
		if (nIndex >= m_ParameterList.size())
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDINDEX);

		auto pParameter = m_ParameterList[nIndex];
		return pParameter->getDoubleValue();
	}

	double CParameterGroup::getDoubleParameterValueByName(const std::string& sName)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);
		auto iIter = m_Parameters.find(sName);

		if (iIter == m_Parameters.end())
			throw ELibMCInterfaceException(LIBMC_ERROR_PARAMETERNOTFOUND);

		return iIter->second->getDoubleValue();
	}

	int64_t CParameterGroup::getIntParameterValueByIndex(const uint32_t nIndex)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);
		if (nIndex >= m_ParameterList.size())
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDINDEX);

		auto pParameter = m_ParameterList[nIndex];
		return pParameter->getIntValue();
	}

	int64_t CParameterGroup::getIntParameterValueByName(const std::string& sName)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);
		auto iIter = m_Parameters.find(sName);

		if (iIter == m_Parameters.end())
			throw ELibMCInterfaceException(LIBMC_ERROR_PARAMETERNOTFOUND);

		return iIter->second->getIntValue();
	}

	bool CParameterGroup::getBoolParameterValueByIndex(const uint32_t nIndex)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);
		if (nIndex >= m_ParameterList.size())
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDINDEX);

		auto pParameter = m_ParameterList[nIndex];
		return pParameter->getBoolValue();
	}

	bool CParameterGroup::getBoolParameterValueByName(const std::string& sName)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);
		auto iIter = m_Parameters.find(sName);

		if (iIter == m_Parameters.end())
			throw ELibMCInterfaceException(LIBMC_ERROR_PARAMETERNOTFOUND);

		return iIter->second->getBoolValue();
	}


	void CParameterGroup::setParameterValueByIndex(const uint32_t nIndex, const std::string& sValue)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);
		if (nIndex >= m_ParameterList.size())
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDINDEX);

		auto pParameter = m_ParameterList[nIndex];
		pParameter->setStringValue(sValue);
	}

	void CParameterGroup::setParameterValueByName(const std::string& sName, const std::string& sValue)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);
		auto iIter = m_Parameters.find(sName);

		if (iIter == m_Parameters.end())
			throw ELibMCInterfaceException(LIBMC_ERROR_PARAMETERNOTFOUND);

		iIter->second->setStringValue(sValue);
	}

	void CParameterGroup::setDoubleParameterValueByIndex(const uint32_t nIndex, const double dValue)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);
		if (nIndex >= m_ParameterList.size())
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDINDEX);

		auto pParameter = m_ParameterList[nIndex];
		pParameter->setDoubleValue(dValue);
	}

	void CParameterGroup::setDoubleParameterValueByName(const std::string& sName, const double dValue)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);
		auto iIter = m_Parameters.find(sName);

		if (iIter == m_Parameters.end())
			throw ELibMCInterfaceException(LIBMC_ERROR_PARAMETERNOTFOUND);

		iIter->second->setDoubleValue(dValue);

	}

	void CParameterGroup::setIntParameterValueByIndex(const uint32_t nIndex, const int64_t nValue)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);
		if (nIndex >= m_ParameterList.size())
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDINDEX);

		auto pParameter = m_ParameterList[nIndex];
		pParameter->setIntValue(nValue);

	}

	void CParameterGroup::setIntParameterValueByName(const std::string& sName, const int64_t nValue)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);
		auto iIter = m_Parameters.find(sName);

		if (iIter == m_Parameters.end())
			throw ELibMCInterfaceException(LIBMC_ERROR_PARAMETERNOTFOUND);

		iIter->second->setIntValue(nValue);

	}

	void CParameterGroup::setBoolParameterValueByIndex(const uint32_t nIndex, const bool bValue)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);
		if (nIndex >= m_ParameterList.size())
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDINDEX);

		auto pParameter = m_ParameterList[nIndex];
		pParameter->setBoolValue(bValue);
	}

	void CParameterGroup::setBoolParameterValueByName(const std::string& sName, const bool bValue)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);
		auto iIter = m_Parameters.find(sName);

		if (iIter == m_Parameters.end())
			throw ELibMCInterfaceException(LIBMC_ERROR_PARAMETERNOTFOUND);

		iIter->second->setBoolValue(bValue);

	}

	std::string CParameterGroup::serializeToJSON()
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);

		CJSONWriter writer;

		for (auto pParameter : m_ParameterList) 
			writer.addString (pParameter->getName(), pParameter->getStringValue());		

		return writer.saveToString();
	}

	void CParameterGroup::deserializeJSON(const std::string& sJSON)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);

		rapidjson::Document document;
		document.Parse(sJSON.c_str ());

		if (!document.IsObject())
			throw ELibMCInterfaceException(LIBMC_ERROR_COULDNOTPARSEJSON);

		for (rapidjson::Value::ConstMemberIterator itr = document.MemberBegin();
			itr != document.MemberEnd(); ++itr)
		{
			if (!itr->value.IsString()) 
				throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDJSONFORMAT);

			std::string sName = itr->name.GetString();
			std::string sValue = itr->value.GetString();

			auto iIter = m_Parameters.find(sName);
			if (iIter == m_Parameters.end())
				throw ELibMCInterfaceException(LIBMC_ERROR_PARAMETERNOTFOUND);

			iIter->second->setStringValue(sValue);
		}
	}

	void CParameterGroup::copyToGroup (CParameterGroup* pParameterGroup)
	{
		if (pParameterGroup == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);
		for (auto pParameter : m_ParameterList) {
			pParameterGroup->addParameterInternal (pParameter->duplicate ());
		}
		
	}


	void CParameterGroup::addNewStringParameter(const std::string& sName, const std::string& sDescription, const std::string& sDefaultValue)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);

		uint32_t nVariableID = 0;
		if (m_pStateJournal != nullptr)
			nVariableID = m_pStateJournal->registerStringValue(m_sInstanceName + "." + m_sName + "." + sName, sDefaultValue);

		addParameterInternal(std::make_shared<CParameter_Valued> (sName, sDescription, sDefaultValue, m_pStateJournal, nVariableID));
	}

	void CParameterGroup::addNewDoubleParameter(const std::string& sName, const std::string& sDescription, const double dDefaultValue, const double dUnits)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);

		uint32_t nVariableID = 0;
		if (m_pStateJournal != nullptr)
			nVariableID = m_pStateJournal->registerDoubleValue(m_sInstanceName + "." + m_sName + "." + sName, dDefaultValue, dUnits);

		addParameterInternal(std::make_shared<CParameter_Valued>(sName, sDescription, dDefaultValue, m_pStateJournal, nVariableID));
	}

	void CParameterGroup::addNewIntParameter(const std::string& sName, const std::string& sDescription, const int64_t nDefaultValue)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);

		uint32_t nVariableID = 0;
		if (m_pStateJournal != nullptr)
			nVariableID = m_pStateJournal->registerIntegerValue(m_sInstanceName + "." + m_sName + "." + sName, nDefaultValue);

		addParameterInternal(std::make_shared<CParameter_Valued>(sName, sDescription, nDefaultValue, m_pStateJournal, nVariableID));
	}

	void CParameterGroup::addNewBoolParameter(const std::string& sName, const std::string& sDescription, const bool bDefaultValue)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);

		uint32_t nVariableID = 0;
		if (m_pStateJournal != nullptr)
			nVariableID = m_pStateJournal->registerBooleanValue(m_sInstanceName + "." + m_sName + "." + sName, bDefaultValue);

		addParameterInternal(std::make_shared<CParameter_Valued>(sName, sDescription, bDefaultValue, m_pStateJournal, nVariableID));
	}


	void CParameterGroup::addNewTypedParameter(const std::string& sName, const std::string& sType, const std::string& sDescription, const std::string& sDefaultValue, const std::string& sUnits)
	{
		if (sType == "string") {

			addNewStringParameter(sName, sDescription, sDefaultValue);

		}
		else if (sType == "int") {

			int64_t nValue = 0;			
			if (sDefaultValue.length() > 0)
				nValue = std::stoll(sDefaultValue);

			addNewIntParameter(sName, sDescription, nValue);

		}
		else if (sType == "bool") {

			bool bValue = false;
			if (sDefaultValue.length() > 0)
				bValue = std::stoi(sDefaultValue) != 0;
			addNewBoolParameter(sName, sDescription, bValue);

		}
		else if (sType == "double") {

			double dValue = 0.0;
			if (sDefaultValue.length() > 0)
				dValue = std::stod(sDefaultValue);

			double dUnits = 0.0;
			if (sUnits.length() > 0)
				dUnits = std::stod(sUnits);

			addNewDoubleParameter(sName, sDescription, dValue, dUnits);
		}
		else
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAMETERTYPE);
	}


	void CParameterGroup::addNewDerivedParameter(const std::string& sName, AMC::PParameterGroup pParameterGroup, const std::string& sSourceParameterName)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);
		if (pParameterGroup.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

		addParameterInternal(std::make_shared<CParameter_Derived>(sName, pParameterGroup, sSourceParameterName));

	}


	void CParameterGroup::removeValue(const std::string& sName)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);

		for (auto iIter = m_ParameterList.begin(); iIter != m_ParameterList.end(); iIter++) {
			if ((*iIter)->getName() == sName) {
				m_ParameterList.erase(iIter);
				break;
			}
		}

		m_Parameters.erase(sName);		

	}

	void CParameterGroup::setJournal(PStateJournal pStateJournal, const std::string& sInstanceName)
	{
		m_pStateJournal = pStateJournal;
		m_sInstanceName = sInstanceName;
	}


}


