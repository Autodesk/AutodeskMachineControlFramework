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


#ifndef __AMC_PARAMETER_VALUED
#define __AMC_PARAMETER_VALUED

#ifndef _PARAMETER_HEADERPROTECTION
#error Never include amc_parameter.hpp from outside of amc_parameter.cpp and amc_parametergroup.cpp
#endif

#include <memory>
#include <string>

#include "amc_parameter.hpp"
#include "amc_statejournal.hpp"

namespace LibMCData {

	class CPersistencyHandler;
	typedef std::shared_ptr<CPersistencyHandler> PPersistencyHandler;

}

namespace AMC {

	class CParameter_Valued : public CParameter {
	private:
		std::string m_sName;
		std::string m_sDescription;
		std::string m_sDefaultValue;
		
		std::string m_sValue;

		// The global original path of the parameter..
		std::string m_sOriginalPath;
		
		// update value including persistency storage
		void setValueEx(const std::string& sValue, uint64_t nAbsoluteTimeStamp);

		eParameterDataType m_DataType;
		
		PStateJournal m_pJournal;
		uint32_t m_nJournalVariableID;

		// For handling persistency, store Data Objects
		std::string m_sPersistentName;
		std::string m_sPersistentUUID;
		LibMCData::PPersistencyHandler m_pPersistencyHandler;

	public:

		CParameter_Valued(const std::string & sName, const std::string& sDescription, const std::string& sDefaultValue, eParameterDataType eDataType, PStateJournal pJournal, uint32_t nJournalVariableID, const std::string & sOriginalPath);
		CParameter_Valued(const std::string& sName, const std::string& sDescription, const double dDefaultValue, eParameterDataType eDataType, PStateJournal pJournal, uint32_t nJournalVariableID, const std::string& sOriginalPath);
		CParameter_Valued(const std::string& sName, const std::string& sDescription, const int64_t nDefaultValue, eParameterDataType eDataType, PStateJournal pJournal, uint32_t nJournalVariableID, const std::string& sOriginalPath);
		CParameter_Valued(const std::string& sName, const std::string& sDescription, const bool bDefaultValue, eParameterDataType eDataType, PStateJournal pJournal, uint32_t nJournalVariableID, const std::string& sOriginalPath);

		virtual ~CParameter_Valued();

		// The following calls are not thread-safe and need to be mutexed in ParameterGroup!

		std::string getName() const override;
		std::string getDescription() const override;
		std::string getDefaultValue() const override;

		eParameterDataType getDataType() const override;

		std::string getStringValue() const override;
		void setStringValue(const std::string& sValue, uint64_t nAbsoluteTimeStamp) override;

		double getDoubleValue() const override;
		void setDoubleValue(const double dValue, uint64_t nAbsoluteTimeStamp) override;

		int64_t getIntValue() const override;
		void setIntValue(const int64_t nValue, uint64_t nAbsoluteTimeStamp) override;

		bool getBoolValue() const override;
		void setBoolValue(const bool bValue, uint64_t nAbsoluteTimeStamp) override;

		virtual PParameter duplicate() override;

		virtual std::string getOriginalPath() override;

		void enablePersistency (const std::string& sPersistentName, const std::string& sPersistentUUID);
		void disablePersistency();
		void setPersistencyHandler(LibMCData::PPersistencyHandler pPersistencyHandler, uint64_t nAbsoluteTimeStamp);

	};
	
}


#endif //__AMC_PARAMETER_VALUED

