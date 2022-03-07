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


#ifndef __AMC_PARAMETER_DERIVED
#define __AMC_PARAMETER_DERIVED

#ifndef _PARAMETER_HEADERPROTECTION
#error Never include amc_parameter.hpp from outside of amc_parameter.cpp and amc_parametergroup.cpp
#endif

#include <memory>
#include <string>

#include "amc_parameter.hpp"

namespace AMC {

	class CParameter_Derived : public CParameter {
	private:

		std::string m_sName;
		PParameterGroup m_pParameterGroup;
		std::string m_sSourceParameterName;

	public:
		CParameter_Derived(const std::string & sName, PParameterGroup pParameterGroup, std::string sSourceParameterName);
		virtual ~CParameter_Derived();

		virtual std::string getName() const override;
		virtual std::string getDescription() const override;
		virtual std::string getDefaultValue() const override;

		virtual std::string getStringValue() const override;
		virtual void setStringValue(const std::string& sValue) override;

		virtual double getDoubleValue() const override;
		virtual void setDoubleValue(const double dValue) override;

		virtual int64_t getIntValue() const override;
		virtual void setIntValue(const int64_t nValue) override;

		virtual bool getBoolValue() const override;
		virtual void setBoolValue(const bool bValue) override;

		virtual PParameter duplicate() override;

	};
	
}


#endif //__AMC_PARAMETER_DERIVED

