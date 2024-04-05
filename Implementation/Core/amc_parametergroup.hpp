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


#ifndef __AMC_PARAMETERGROUP
#define __AMC_PARAMETERGROUP


#include <memory>
#include <vector>
#include <map>
#include <string>
#include <mutex>

#include "amc_parametertype.hpp"

namespace LibMCData {

	class CPersistencyHandler;
	typedef std::shared_ptr<CPersistencyHandler> PPersistencyHandler;

}


namespace AMC {

	class CParameter;
	typedef std::shared_ptr<CParameter> PParameter;

	class CParameterGroup;
	typedef std::shared_ptr<CParameterGroup> PParameterGroup;

	class CStateJournal;
	typedef std::shared_ptr<CStateJournal> PStateJournal;

	class CParameterGroup {
	private:
		
		std::string m_sName;
		std::string m_sDescription;
		std::map<std::string, PParameter> m_Parameters;
		std::vector<PParameter> m_ParameterList;

		PStateJournal m_pStateJournal;
		std::string m_sInstanceName;

		std::mutex m_GroupMutex;

		void addParameterInternal(PParameter pParameter);

	public:

		CParameterGroup();
		CParameterGroup(const std::string & sName, const std::string& sDescription);
		
		virtual ~CParameterGroup();		
		
		std::string getName ();
		std::string getDescription();

		bool hasParameter (const std::string & sName);

		std::string getParameterPath(const std::string& sName);

		void addNewStringParameter(const std::string& sName, const std::string& sDescription, const std::string& sDefaultValue);
		void addNewDoubleParameter(const std::string& sName, const std::string& sDescription, const double dDefaultValue, const double dUnits);
		void addNewIntParameter(const std::string& sName, const std::string& sDescription, const int64_t nDefaultValue);
		void addNewBoolParameter(const std::string& sName, const std::string& sDescription, const bool bDefaultValue);
		void addNewUUIDParameter(const std::string& sName, const std::string& sDescription, const std::string& sDefaultValue);
		void addNewTypedParameter(const std::string& sName, const std::string& sType, const std::string& sDescription, const std::string& sDefaultValue, const std::string& sUnits);

		void addNewDerivedParameter(const std::string& sName, AMC::PParameterGroup pParameterGroup, const std::string& sSourceParameterName);

		uint32_t getParameterCount();
		void getParameterInfo(const uint32_t nIndex, std::string & sName, std::string & sDescription, std::string & sDefaultValue);		
		void getParameterInfoByName(const std::string& sName, std::string& sDescription, std::string& sDefaultValue);

		std::string getParameterValueByIndex(const uint32_t nIndex);
		std::string getParameterValueByName(const std::string & sName);
		double getDoubleParameterValueByIndex(const uint32_t nIndex);
		double getDoubleParameterValueByName(const std::string& sName);
		int64_t getIntParameterValueByIndex(const uint32_t nIndex);
		int64_t getIntParameterValueByName(const std::string& sName);
		bool getBoolParameterValueByIndex(const uint32_t nIndex);
		bool getBoolParameterValueByName(const std::string& sName);

		eParameterDataType getParameterDataTypeByIndex(const uint32_t nIndex);
		eParameterDataType getParameterDataTypeByName(const std::string& sName);

		void setParameterValueByIndex(const uint32_t nIndex, const std::string & sValue);
		void setParameterValueByName(const std::string& sName, const std::string& sValue);
		void setDoubleParameterValueByIndex(const uint32_t nIndex, const double dValue);
		void setDoubleParameterValueByName(const std::string& sName, const double dValue);
		void setIntParameterValueByIndex(const uint32_t nIndex, const int64_t nValue);
		void setIntParameterValueByName(const std::string& sName, const int64_t nValue);
		void setBoolParameterValueByIndex(const uint32_t nIndex, const bool bValue);
		void setBoolParameterValueByName(const std::string& sName, const bool bValue);

		void removeValue(const std::string& sName);

		std::string serializeToJSON();
		void deserializeJSON(const std::string & sJSON);

		void copyToGroup (CParameterGroup * pParameterGroup);

		void addDerivativesFromGroup(PParameterGroup pParameterGroup);
		void addDuplicatesFromGroup(CParameterGroup * pParameterGroup);

		void setJournal(PStateJournal pStateJournal, const std::string & sInstanceName);

		void setParameterPersistentUUID (const std::string& sParameterName, const std::string& sPersistentUUID);
		void updateParameterPersistencyHandler (LibMCData::PPersistencyHandler pPersistencyHandler);

	};

	
}


#endif //__AMC_PARAMETERGROUP

