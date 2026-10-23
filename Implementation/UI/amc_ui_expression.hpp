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

	enum class eUIExpressionFormatType 
	{
		eftString = 1,
		eftDouble = 2,
		eftInteger = 3
	};

	// Resolves client session dependent references of UI expressions:
	//   session.<name>             declared session variable of the current client session
	//   $session.uuid              UUID of the current client session
	//   $user.login / role / uuid  identity of the logged-in user
	//   $user.permission.<id>      true if the user's role grants the permission
	class CUIExpressionSessionContext {
	public:
		virtual ~CUIExpressionSessionContext() {}

		virtual std::string evaluateSessionReference(const std::string& sReference) = 0;
		virtual bool evaluateSessionReferenceAsBool(const std::string& sReference) = 0;
		virtual int64_t evaluateSessionReferenceAsInteger(const std::string& sReference) = 0;
		virtual double evaluateSessionReferenceAsNumber(const std::string& sReference) = 0;
	};

	class CUIExpression {
	private:
		std::string m_sFixedValue;
		std::string m_sExpressionValue;

		std::string m_sFormatString;

		void readFromXML(const pugi::xml_node& xmlNode, const std::string& attributeName, const std::string& defaultValue, bool bValueMustExist);

		std::string evaluateValueEx(CStateMachineData* pStateMachineData, CUIExpressionSessionContext* pSessionContext);
	public:

		// Returns true if the (trimmed, non-inverted) reference is resolved by a session context
		// instead of the state machine data.
		static bool isSessionReference(const std::string& sReference);

		CUIExpression();
		CUIExpression (const pugi::xml_node & xmlNode, const std::string & attributeName);
		CUIExpression(const pugi::xml_node& xmlNode, const std::string& attributeName, bool bValueMustExist);
		CUIExpression(const pugi::xml_node& xmlNode, const std::string& attributeName, const std::string& defaultValue);
		CUIExpression(const pugi::xml_node& xmlNode, const std::string& attributeName, const char* defaultValue);

		void setFixedValue(const std::string & sValue);
		void setSyncValue(const std::string & sValue);

		// Session references evaluate to empty/false/0 when no session context is given.
		std::string evaluateStringValue(CStateMachineData * pStateMachineData, CUIExpressionSessionContext* pSessionContext = nullptr);
		std::string evaluateStringValue(PStateMachineData pStateMachineData, CUIExpressionSessionContext* pSessionContext = nullptr);
		double evaluateNumberValue(CStateMachineData* pStateMachineData, CUIExpressionSessionContext* pSessionContext = nullptr);
		double evaluateNumberValue(PStateMachineData pStateMachineData, CUIExpressionSessionContext* pSessionContext = nullptr);
		int64_t evaluateIntegerValue(CStateMachineData* pStateMachineData, CUIExpressionSessionContext* pSessionContext = nullptr);
		int64_t evaluateIntegerValue(PStateMachineData pStateMachineData, CUIExpressionSessionContext* pSessionContext = nullptr);
		bool evaluateBoolValue(CStateMachineData* pStateMachineData, CUIExpressionSessionContext* pSessionContext = nullptr);
		bool evaluateBoolValue(PStateMachineData pStateMachineData, CUIExpressionSessionContext* pSessionContext = nullptr);
		std::string evaluateUUIDValue(CStateMachineData* pStateMachineData, CUIExpressionSessionContext* pSessionContext = nullptr);
		std::string evaluateUUIDValue(PStateMachineData pStateMachineData, CUIExpressionSessionContext* pSessionContext = nullptr);

		void checkExpressionSyntax(CStateMachineData* pStateMachineData);
		void checkExpressionSyntax(PStateMachineData pStateMachineData);

		bool needsSync();
		bool isEmpty(CStateMachineData* pStateMachineData, CUIExpressionSessionContext* pSessionContext = nullptr);
		bool isEmpty(PStateMachineData pStateMachineData, CUIExpressionSessionContext* pSessionContext = nullptr);

		// Returns the session reference of a sync expression (without a leading "!"),
		// or an empty string if the expression does not reference the session.
		std::string getSessionReference();

	};
	
}


#endif //__AMC_UI_EXPRESSION

