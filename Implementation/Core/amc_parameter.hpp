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


#ifndef __AMC_PARAMETER
#define __AMC_PARAMETER

#ifndef _PARAMETER_HEADERPROTECTION
#error Never include amc_parameter.hpp from outside of amc_parameter.cpp and amc_parametergroup.cpp
#endif

#include <memory>
#include <string>

namespace AMC {

	class CParameter;
	typedef std::shared_ptr<CParameter> PParameter;

	class CParameter {
	private:
		std::string m_sName;
		std::string m_sDescription;
		std::string m_sDefaultValue;
		std::string m_sValue;
	public:

		CParameter(const std::string & sName, const std::string& sDescription, const std::string& sDefaultValue);
		CParameter(const std::string& sName, const std::string& sDescription, const double dDefaultValue);
		CParameter(const std::string& sName, const std::string& sDescription, const int64_t nDefaultValue);
		CParameter(const std::string& sName, const std::string& sDescription, const bool bDefaultValue);

		virtual ~CParameter();		

		// The following calls are not thread-safe and need to be mutexed in ParameterGroup!

		std::string getName() const;
		std::string getDescription() const;
		std::string getDefaultValue() const;

		std::string getStringValue() const;
		void setStringValue(const std::string& sValue);

		double getDoubleValue() const;
		void setDoubleValue(const double dValue);

		int64_t getIntValue() const;
		void setIntValue(const int64_t nValue);

		bool getBoolValue() const;
		void setBoolValue(const bool bValue);

	};

	
}


#endif //__AMC_PARAMETER

