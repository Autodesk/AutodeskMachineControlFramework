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


#define _STATESIGNAL_HEADERPROTECTION 

#include "amc_statesignal.hpp"
#include "common_utils.hpp"
#include "libmc_exceptiontypes.hpp"

namespace AMC {
	
	CStateSignal::CStateSignal(const std::string& sInstanceName, const std::string & sName, const std::list<CStateSignalParameter>& Parameters, const std::list<CStateSignalParameter>& Results)
		: m_sInstanceName (sInstanceName), m_sName (sName), m_bIsTriggered (false), m_ParameterDefinitions(Parameters), m_ResultDefinitions(Results)
	{
	}
	
	CStateSignal::~CStateSignal()
	{
	}

	std::string CStateSignal::getNameInternal()
	{
		return m_sName;
	}
	
	std::string CStateSignal::getInstanceNameInternal()
	{
		return m_sInstanceName;
	}

	std::string CStateSignal::getParameterDataInternal()
	{
		return m_sCurrentParameterData;
	}

	bool CStateSignal::triggerSignalInternal(const std::string& sParameterData, std::string& sNewSignalUUID)
	{
		if (m_bIsTriggered) {
			return false;
		}

		m_sCurrentParameterData = sParameterData;
		m_sCurrentSignalUUID = AMCCommon::CUtils::createUUID();
		sNewSignalUUID = m_sCurrentSignalUUID;

		m_bIsTriggered = true;
		return true;
	}

	bool CStateSignal::checkSignalInternal(std::string& sSignalUUID)
	{
		if (!m_bIsTriggered) {
			sSignalUUID = "";
			return false;
		}

		sSignalUUID = m_sCurrentSignalUUID;

		return true;

	}

	bool CStateSignal::canTriggerInternal()
	{
		return (!m_bIsTriggered);			
	}

	void CStateSignal::markSignalAsHandledInternal(const std::string& sResultData)
	{
		if (m_sCurrentSignalUUID.length() > 0) {
			m_ResultMap.insert(std::make_pair(m_sCurrentSignalUUID, sResultData));
		}

		m_bIsTriggered = false;
		m_sCurrentSignalUUID = "";
		m_sCurrentParameterData = "";
	}

	bool CStateSignal::signalHasBeenHandledInternal(const std::string& sSignalUUID, bool clearAllResults, std::string& sResultData)
	{
		auto iIter = m_ResultMap.find(sSignalUUID);

		if (iIter != m_ResultMap.end()) {

			sResultData = iIter->second;
			if (clearAllResults)
				m_ResultMap.erase(sSignalUUID);

			return true;
		}

		return false;

	}

	std::string CStateSignal::getCurrentUUID()
	{
		return m_sCurrentSignalUUID;
	}

	void CStateSignal::populateParameterGroup(CParameterGroup* pParameterGroup)
	{
		LibMCAssertNotNull(pParameterGroup);

		for (auto Definition : m_ParameterDefinitions) {
			std::string sType = Definition.getType();
			std::string sName = Definition.getName();
			pParameterGroup->addNewTypedParameter(sName, sType, "", "", "");			
		}

	}

	void CStateSignal::populateResultGroup(CParameterGroup* pResultGroup)
	{
		LibMCAssertNotNull(pResultGroup);

		for (auto Definition : m_ResultDefinitions) {
			std::string sType = Definition.getType();
			std::string sName = Definition.getName();
			pResultGroup->addNewTypedParameter(sName, sType, "", "", "");
		}
	}


}


