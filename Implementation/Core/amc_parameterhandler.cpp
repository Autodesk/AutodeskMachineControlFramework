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


#include "amc_parameterhandler.hpp"
#include "libmc_exceptiontypes.hpp"

#define AMC_MAXPARAMETERGROUPCOUNT (1024 * 1024)

namespace AMC {

	CParameterHandler::CParameterHandler(std::string sDescription)
		: m_sDescription (sDescription)
	{
	}

	CParameterHandler::~CParameterHandler()
	{
	}


	bool CParameterHandler::hasGroup(const std::string& sName)
	{
		std::lock_guard <std::mutex> lockGuard(m_Mutex);

		auto iter = m_Groups.find(sName);
		return (iter != m_Groups.end());
	}

	void CParameterHandler::addGroup(PParameterGroup pGroup)
	{
		std::lock_guard <std::mutex> lockGuard(m_Mutex);
		LibMCAssertNotNull(pGroup.get());

		auto sName = pGroup->getName();
		if (m_Groups.find(sName) != m_Groups.end())
			throw ELibMCCustomException(LIBMC_ERROR_DUPLICATEPARAMETERGROUPNAME, m_sDescription + "/" + sName);

		if (m_GroupList.size() >= AMC_MAXPARAMETERGROUPCOUNT)
			throw ELibMCCustomException(LIBMC_ERROR_TOOMANYPARAMETERGROUPS, m_sDescription + "/" + sName);

		m_Groups.insert(std::make_pair(sName, pGroup));
		m_GroupList.push_back(pGroup);
	}

	PParameterGroup CParameterHandler::addGroup(const std::string& sName, const std::string& sDescription)
	{
		PParameterGroup pGroup = std::make_shared<CParameterGroup>(sName, sDescription);
		addGroup(pGroup);
		return pGroup;
	}


	uint32_t CParameterHandler::getGroupCount()
	{
		std::lock_guard <std::mutex> lockGuard(m_Mutex);

		return (uint32_t)m_GroupList.size();

	}

	PParameterGroup CParameterHandler::getGroup(const uint32_t nIndex)
	{
		std::lock_guard <std::mutex> lockGuard(m_Mutex);

		if (nIndex >= m_GroupList.size())
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDINDEX, m_sDescription);

		return m_GroupList[nIndex];
	}

	PParameterGroup CParameterHandler::findGroup(const std::string& sName, const bool bFailIfNotExisting)
	{
		std::lock_guard <std::mutex> lockGuard(m_Mutex);

		auto iter = m_Groups.find(sName);
		if (iter != m_Groups.end())
			return iter->second;

		if (bFailIfNotExisting)
			throw ELibMCCustomException(LIBMC_ERROR_PARAMETERGROUPNOTFOUND, sName);

		return nullptr;	
	}



	std::string CParameterHandler::getDescription()
	{
		std::lock_guard <std::mutex> lockGuard(m_Mutex);
		// Return thread safe copy of instance description
		return std::string(m_sDescription.c_str());
	}

	void CParameterHandler::setDescription(const std::string& sDescription)
	{
		std::lock_guard <std::mutex> lockGuard(m_Mutex);
		m_sDescription = sDescription.c_str();

	}


	PParameterHandler CParameterHandler::duplicate()
	{
		auto pResult = std::make_shared<CParameterHandler>(m_sDescription);
		for (auto pGroup : m_GroupList) {
			auto pNewGroup = pResult->addGroup(pGroup->getName(), pGroup->getDescription());
			pNewGroup->addDuplicatesFromGroup(pGroup.get());
		}		

		

		return pResult;
	}


}


