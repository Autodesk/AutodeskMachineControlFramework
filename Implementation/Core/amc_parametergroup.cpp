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
#include "amc_constants.hpp"

#include "amc_jsonwriter.hpp"
#include "common_utils.hpp"

#include "libmc_exceptiontypes.hpp"
#include "RapidJSON/document.h"
#include "RapidJSON/stringbuffer.h"
#include "RapidJSON/writer.h"

#define AMC_MAXPARAMETERCOUNT (1024 * 1024)

namespace AMC {
	

	CParameterGroup::CParameterGroup(AMCCommon::PChrono pGlobalChrono)
		: m_pStateJournal (nullptr), m_pGlobalChrono (pGlobalChrono)
	{

	}

	CParameterGroup::CParameterGroup(const std::string& sName, const std::string& sDescription, AMCCommon::PChrono pGlobalChrono)
		: m_sName(sName), m_sDescription(sDescription), m_pStateJournal (nullptr), m_pGlobalChrono(pGlobalChrono)
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
		LibMCAssertNotNull(pParameter.get());

		auto sName = pParameter->getName();
		if (m_Parameters.find(sName) != m_Parameters.end())
			throw ELibMCCustomException(LIBMC_ERROR_DUPLICATEPARAMETERNAME, sName);

		if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sName))
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDPARAMETERNAME, sName);


		if (m_ParameterList.size() >= AMC_MAXPARAMETERCOUNT)
			throw ELibMCCustomException(LIBMC_ERROR_TOOMANYPARAMETERS, sName);

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
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDINDEX, sName);

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
			throw ELibMCCustomException(LIBMC_ERROR_PARAMETERNOTFOUND, m_sName + "/" + sName);

		sDescription = iIter->second->getDescription();
		sDefaultValue = iIter->second->getDefaultValue();

	}


	std::string CParameterGroup::getParameterValueByIndex(const uint32_t nIndex)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);
		if (nIndex >= m_ParameterList.size())
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDINDEX, m_sName);

		auto pParameter = m_ParameterList[nIndex];
		return pParameter->getStringValue();
	}

	std::string CParameterGroup::getParameterValueByName(const std::string& sName)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);
		auto iIter = m_Parameters.find(sName);

		if (iIter == m_Parameters.end())
			throw ELibMCCustomException(LIBMC_ERROR_PARAMETERNOTFOUND, m_sName + "/" + sName);

		return iIter->second->getStringValue();
	}

	double CParameterGroup::getDoubleParameterValueByIndex(const uint32_t nIndex)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);
		if (nIndex >= m_ParameterList.size())
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDINDEX, m_sName);

		auto pParameter = m_ParameterList[nIndex];
		return pParameter->getDoubleValue();
	}

	double CParameterGroup::getDoubleParameterValueByName(const std::string& sName)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);
		auto iIter = m_Parameters.find(sName);

		if (iIter == m_Parameters.end())
			throw ELibMCCustomException(LIBMC_ERROR_PARAMETERNOTFOUND, m_sName + "/" + sName);

		return iIter->second->getDoubleValue();
	}

	int64_t CParameterGroup::getIntParameterValueByIndex(const uint32_t nIndex)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);
		if (nIndex >= m_ParameterList.size())
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDINDEX, m_sName);

		auto pParameter = m_ParameterList[nIndex];
		return pParameter->getIntValue();
	}

	int64_t CParameterGroup::getIntParameterValueByName(const std::string& sName)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);
		auto iIter = m_Parameters.find(sName);

		if (iIter == m_Parameters.end())
			throw ELibMCCustomException(LIBMC_ERROR_PARAMETERNOTFOUND, m_sName + "/" + sName);

		return iIter->second->getIntValue();
	}

	bool CParameterGroup::getBoolParameterValueByIndex(const uint32_t nIndex)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);
		if (nIndex >= m_ParameterList.size())
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDINDEX, m_sName);

		auto pParameter = m_ParameterList[nIndex];
		return pParameter->getBoolValue();
	}

	bool CParameterGroup::getBoolParameterValueByName(const std::string& sName)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);
		auto iIter = m_Parameters.find(sName);

		if (iIter == m_Parameters.end())
			throw ELibMCCustomException(LIBMC_ERROR_PARAMETERNOTFOUND, m_sName + "/" + sName);

		return iIter->second->getBoolValue();
	}


	eParameterDataType CParameterGroup::getParameterDataTypeByIndex(const uint32_t nIndex)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);
		if (nIndex >= m_ParameterList.size())
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDINDEX, m_sName);

		auto pParameter = m_ParameterList[nIndex];
		return pParameter->getDataType();

	}

	eParameterDataType CParameterGroup::getParameterDataTypeByName(const std::string& sName)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);
		auto iIter = m_Parameters.find(sName);

		if (iIter == m_Parameters.end())
			throw ELibMCCustomException(LIBMC_ERROR_PARAMETERNOTFOUND, m_sName + "/" + sName);

		return iIter->second->getDataType();
	}


	void CParameterGroup::setParameterValueByIndex(const uint32_t nIndex, const std::string& sValue)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);
		if (nIndex >= m_ParameterList.size())
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDINDEX, m_sName);

		uint64_t nAbsoluteTimeStamp = m_pGlobalChrono->getUTCTimeStampInMicrosecondsSince1970();

		auto pParameter = m_ParameterList[nIndex];
		pParameter->setStringValue(sValue, nAbsoluteTimeStamp);
	}

	void CParameterGroup::setParameterValueByName(const std::string& sName, const std::string& sValue)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);
		auto iIter = m_Parameters.find(sName);

		if (iIter == m_Parameters.end())
			throw ELibMCCustomException(LIBMC_ERROR_PARAMETERNOTFOUND, m_sName + "/" + sName);

		uint64_t nAbsoluteTimeStamp = m_pGlobalChrono->getUTCTimeStampInMicrosecondsSince1970();

		iIter->second->setStringValue(sValue, nAbsoluteTimeStamp);
	}

	void CParameterGroup::setDoubleParameterValueByIndex(const uint32_t nIndex, const double dValue)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);
		if (nIndex >= m_ParameterList.size())
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDINDEX, m_sName);

		uint64_t nAbsoluteTimeStamp = m_pGlobalChrono->getUTCTimeStampInMicrosecondsSince1970();

		auto pParameter = m_ParameterList[nIndex];
		pParameter->setDoubleValue(dValue, nAbsoluteTimeStamp);
	}

	void CParameterGroup::setDoubleParameterValueByName(const std::string& sName, const double dValue)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);
		auto iIter = m_Parameters.find(sName);

		if (iIter == m_Parameters.end())
			throw ELibMCCustomException(LIBMC_ERROR_PARAMETERNOTFOUND, m_sName + "/" + sName);

		uint64_t nAbsoluteTimeStamp = m_pGlobalChrono->getUTCTimeStampInMicrosecondsSince1970();

		iIter->second->setDoubleValue(dValue, nAbsoluteTimeStamp);

	}

	void CParameterGroup::setIntParameterValueByIndex(const uint32_t nIndex, const int64_t nValue)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);
		if (nIndex >= m_ParameterList.size())
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDINDEX, m_sName);

		uint64_t nAbsoluteTimeStamp = m_pGlobalChrono->getUTCTimeStampInMicrosecondsSince1970();

		auto pParameter = m_ParameterList[nIndex];
		pParameter->setIntValue(nValue, nAbsoluteTimeStamp);

	}

	void CParameterGroup::setIntParameterValueByName(const std::string& sName, const int64_t nValue)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);
		auto iIter = m_Parameters.find(sName);

		if (iIter == m_Parameters.end())
			throw ELibMCCustomException(LIBMC_ERROR_PARAMETERNOTFOUND, m_sName + "/" + sName);

		uint64_t nAbsoluteTimeStamp = m_pGlobalChrono->getUTCTimeStampInMicrosecondsSince1970();

		iIter->second->setIntValue(nValue, nAbsoluteTimeStamp);

	}

	void CParameterGroup::setBoolParameterValueByIndex(const uint32_t nIndex, const bool bValue)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);
		if (nIndex >= m_ParameterList.size())
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDINDEX, m_sName);

		uint64_t nAbsoluteTimeStamp = m_pGlobalChrono->getUTCTimeStampInMicrosecondsSince1970();

		auto pParameter = m_ParameterList[nIndex];
		pParameter->setBoolValue(bValue, nAbsoluteTimeStamp);
	}

	void CParameterGroup::setBoolParameterValueByName(const std::string& sName, const bool bValue)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);
		auto iIter = m_Parameters.find(sName);

		if (iIter == m_Parameters.end())
			throw ELibMCCustomException(LIBMC_ERROR_PARAMETERNOTFOUND, m_sName + "/" + sName);

		uint64_t nAbsoluteTimeStamp = m_pGlobalChrono->getUTCTimeStampInMicrosecondsSince1970();

		iIter->second->setBoolValue(bValue, nAbsoluteTimeStamp);

	}

	std::string CParameterGroup::serializeToJSON()
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);

		CJSONWriter writer;

		for (auto pParameter : m_ParameterList) 
			writer.addString (pParameter->getName(), pParameter->getStringValue());		

		return writer.saveToString();
	}

	void CParameterGroup::deserializeJSON(const std::string& sJSON, uint64_t nAbsoluteTimeStamp)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);

		rapidjson::Document document;
		document.Parse(sJSON.c_str ());

		if (!document.IsObject())
			throw ELibMCCustomException(LIBMC_ERROR_COULDNOTPARSEJSON, m_sName);

		for (rapidjson::Value::ConstMemberIterator itr = document.MemberBegin();
			itr != document.MemberEnd(); ++itr)
		{
			if (!itr->value.IsString()) 
				throw ELibMCCustomException(LIBMC_ERROR_INVALIDJSONFORMAT, m_sName);

			std::string sName = itr->name.GetString();
			std::string sValue = itr->value.GetString();

			auto iIter = m_Parameters.find(sName);
			if (iIter == m_Parameters.end())
				throw ELibMCCustomException(LIBMC_ERROR_PARAMETERNOTFOUND, m_sName + "/" + sName);

			iIter->second->setStringValue(sValue, nAbsoluteTimeStamp);
		}
	}

	void CParameterGroup::copyToGroup (CParameterGroup* pParameterGroup)
	{
		LibMCAssertNotNull(pParameterGroup);

		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);
		for (auto pParameter : m_ParameterList) {
			pParameterGroup->addParameterInternal (pParameter->duplicate ());
		}
		
	}


	void CParameterGroup::addDerivativesFromGroup(PParameterGroup pParameterGroup)
	{
		LibMCAssertNotNull(pParameterGroup);

		auto nCount = pParameterGroup->getParameterCount();

		for (uint32_t nIndex = 0; nIndex < nCount; nIndex++) {
			std::string sName, sDescription, sDefaultValue;
			pParameterGroup->getParameterInfo (nIndex, sName, sDescription, sDefaultValue);
			addNewDerivedParameter (sName, pParameterGroup, sName);

		}

	}

	void CParameterGroup::addDuplicatesFromGroup(CParameterGroup* pParameterGroup)
	{
		LibMCAssertNotNull(pParameterGroup);

		pParameterGroup->copyToGroup(this);

	}

	std::string CParameterGroup::getLocalParameterPath(const std::string& sName)
	{
		return (m_sInstanceName + "." + m_sName + "." + sName);
	}

	std::string CParameterGroup::getOriginalParameterPath(const std::string & sName)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);
		auto iIter = m_Parameters.find(sName);

		if (iIter == m_Parameters.end())
			throw ELibMCCustomException(LIBMC_ERROR_PARAMETERNOTFOUND, m_sName + "/" + sName);

		return iIter->second->getOriginalPath();
	}

	uint64_t CParameterGroup::getChangeCounterOf(const std::string& sName)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);
		auto iIter = m_Parameters.find(sName);

		if (iIter == m_Parameters.end())
			throw ELibMCCustomException(LIBMC_ERROR_PARAMETERNOTFOUND, m_sName + "/" + sName);

		return iIter->second->getChangeCounter();
	}


	void CParameterGroup::addNewStringParameter(const std::string& sName, const std::string& sDescription, const std::string& sDefaultValue)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);

		std::string sParameterPath = getLocalParameterPath(sName);

		uint32_t nVariableID = 0;
		if (m_pStateJournal != nullptr)
			nVariableID = m_pStateJournal->registerStringValue(sParameterPath, sDefaultValue);

		addParameterInternal(std::make_shared<CParameter_Valued> (sName, sDescription, sDefaultValue, eParameterDataType::String, m_pStateJournal, nVariableID, sParameterPath));
	}

	void CParameterGroup::addNewDoubleParameter(const std::string& sName, const std::string& sDescription, const double dDefaultValue, const double dUnits)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);

		std::string sParameterPath = getLocalParameterPath(sName);

		uint32_t nVariableID = 0;
		if (m_pStateJournal != nullptr)
			nVariableID = m_pStateJournal->registerDoubleValue(sParameterPath, dDefaultValue, dUnits);

		addParameterInternal(std::make_shared<CParameter_Valued>(sName, sDescription, dDefaultValue, eParameterDataType::Double, m_pStateJournal, nVariableID, sParameterPath));
	}

	void CParameterGroup::addNewIntParameter(const std::string& sName, const std::string& sDescription, const int64_t nDefaultValue)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);

		std::string sParameterPath = getLocalParameterPath(sName);

		uint32_t nVariableID = 0;
		if (m_pStateJournal != nullptr)
			nVariableID = m_pStateJournal->registerIntegerValue(sParameterPath, nDefaultValue);

		addParameterInternal(std::make_shared<CParameter_Valued>(sName, sDescription, nDefaultValue, eParameterDataType::Integer, m_pStateJournal, nVariableID, sParameterPath));
	}

	void CParameterGroup::addNewBoolParameter(const std::string& sName, const std::string& sDescription, const bool bDefaultValue)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);

		std::string sParameterPath = getLocalParameterPath(sName);

		uint32_t nVariableID = 0;
		if (m_pStateJournal != nullptr)
			nVariableID = m_pStateJournal->registerBooleanValue(sParameterPath, bDefaultValue);

		addParameterInternal(std::make_shared<CParameter_Valued>(sName, sDescription, bDefaultValue, eParameterDataType::Bool, m_pStateJournal, nVariableID, sParameterPath));
	}

	void CParameterGroup::addNewUUIDParameter(const std::string& sName, const std::string& sDescription, const std::string& sDefaultValue)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);

		std::string sParameterPath = getLocalParameterPath(sName);

		uint32_t nVariableID = 0;
		if (m_pStateJournal != nullptr)
			nVariableID = m_pStateJournal->registerStringValue(sParameterPath, sDefaultValue);

		addParameterInternal(std::make_shared<CParameter_Valued>(sName, sDescription, AMCCommon::CUtils::normalizeUUIDString (sDefaultValue), eParameterDataType::UUID, m_pStateJournal, nVariableID, sParameterPath));
	}


	void CParameterGroup::addNewTypedParameter(const std::string& sName, const std::string& sType, const std::string& sDescription, const std::string& sDefaultValue, const std::string& sUnits)
	{
		if (sType == "string") {

			addNewStringParameter(sName, sDescription, sDefaultValue);

		}
		else if (sType == "uuid") {

			addNewUUIDParameter(sName, sDescription, sDefaultValue);

		}
		else if ((sType == "int") || (sType == "integer")) {

			int64_t nValue = 0;			
			if (sDefaultValue.length() > 0)
				nValue = AMCCommon::CUtils::stringToInteger(sDefaultValue);

			addNewIntParameter(sName, sDescription, nValue);

		}
		else if (sType == "bool") {

			bool bValue = false;
			if (sDefaultValue.length() > 0)
				bValue = AMCCommon::CUtils::stringToBool(sDefaultValue) != 0;
			addNewBoolParameter(sName, sDescription, bValue);

		}
		else if (sType == "double") {

			double dValue = 0.0;
			if (sDefaultValue.length() > 0)
				dValue = AMCCommon::CUtils::stringToDouble(sDefaultValue);

			double dUnits = AMC_PARAMETERUNITS_DEFAULT;
			if (sUnits.length() > 0)
				dUnits = AMCCommon::CUtils::stringToDouble(sUnits);

			if ((dUnits < AMC_PARAMETERUNITS_MINIMUM) || (dUnits > AMC_PARAMETERUNITS_MAXIMUM))
				throw ELibMCCustomException(LIBMC_ERROR_UNITSAREOUTOFRANGE, sName);


			addNewDoubleParameter(sName, sDescription, dValue, dUnits);
		}
		else
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDPARAMETERTYPE, sName);
	}


	void CParameterGroup::addNewDerivedParameter(const std::string& sName, AMC::PParameterGroup pParameterGroup, const std::string& sSourceParameterName)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);
		LibMCAssertNotNull(pParameterGroup.get());

		auto pDerivedParameter = std::make_shared<CParameter_Derived>(sName, pParameterGroup, sSourceParameterName);
		if (m_pStateJournal.get () != nullptr) {
			m_pStateJournal->registerAlias (getLocalParameterPath(sName), pDerivedParameter->getOriginalPath ());
		}

		addParameterInternal(pDerivedParameter);

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


	void CParameterGroup::setParameterPersistentUUID(const std::string& sParameterName, const std::string& sPersistentUUID)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);

		auto iIter = m_Parameters.find(sParameterName);
		if (iIter == m_Parameters.end())
			throw ELibMCCustomException(LIBMC_ERROR_PARAMETERNOTFOUND, m_sName + "/" + sParameterName);
		auto pValuedParameter = std::dynamic_pointer_cast<CParameter_Valued> (iIter->second);
		if (pValuedParameter.get() == nullptr)
			throw ELibMCCustomException(LIBMC_ERROR_ONLYVALUEDPARAMETERSCANBEPERSISTENT, m_sName + "/" + sParameterName);

		if (!sPersistentUUID.empty()) {
			std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sPersistentUUID);
			pValuedParameter->enablePersistency(m_sName + "." + pValuedParameter->getName(), sNormalizedUUID);

		}
		else {

			pValuedParameter->disablePersistency();
		}

	}

	void CParameterGroup::updateParameterPersistencyHandler(LibMCData::PPersistencyHandler pPersistencyHandler, uint64_t nAbsoluteTimeStamp)
	{
		std::lock_guard <std::mutex> lockGuard(m_GroupMutex);
		for (auto pParameter : m_ParameterList) {
			auto pValuedParameter = std::dynamic_pointer_cast<CParameter_Valued> (pParameter);
			if (pValuedParameter.get() != nullptr)
				pValuedParameter->setPersistencyHandler (pPersistencyHandler, nAbsoluteTimeStamp);
		}

	}


}


