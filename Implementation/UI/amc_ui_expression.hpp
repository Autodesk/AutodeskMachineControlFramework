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


#ifndef __AMC_UI_EXPRESSION
#define __AMC_UI_EXPRESSION

#include "header_protection.hpp"

#include <memory>
#include <string>

#include <pugixml.hpp>

namespace AMC {

	amcDeclareDependingClass(CStateMachineData, PStateMachineData);

	class CUIExpression {
	private:
		std::string m_sFixedValue;
		std::string m_sExpressionValue;

		void readFromXML(const pugi::xml_node& xmlNode, const std::string& attributeName, const std::string& defaultValue, bool bValueMustExist);
	public:

		CUIExpression();
		CUIExpression (const pugi::xml_node & xmlNode, const std::string & attributeName);
		CUIExpression(const pugi::xml_node& xmlNode, const std::string& attributeName, bool bValueMustExist);
		CUIExpression(const pugi::xml_node& xmlNode, const std::string& attributeName, const std::string& defaultValue);

		void setFixedValue(const std::string & sValue);

		std::string evaluateStringValue(CStateMachineData * pStateMachineData);
		std::string evaluateStringValue(PStateMachineData pStateMachineData);
		double evaluateNumberValue(CStateMachineData* pStateMachineData);
		double evaluateNumberValue(PStateMachineData pStateMachineData);
		int64_t evaluateIntegerValue(CStateMachineData* pStateMachineData);
		int64_t evaluateIntegerValue(PStateMachineData pStateMachineData);
		bool evaluateBoolValue(CStateMachineData* pStateMachineData);
		bool evaluateBoolValue(PStateMachineData pStateMachineData);

		void checkExpressionSyntax(CStateMachineData* pStateMachineData);
		void checkExpressionSyntax(PStateMachineData pStateMachineData);

		bool needsSync();
		bool isEmpty(CStateMachineData* pStateMachineData);
		bool isEmpty(PStateMachineData pStateMachineData);

	};
	
}


#endif //__AMC_UI_EXPRESSION

