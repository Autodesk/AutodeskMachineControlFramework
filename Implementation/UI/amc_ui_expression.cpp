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

#include "amc_ui_expression.hpp"
#include "amc_statemachinedata.hpp"
#include "common_utils.hpp"
#include "libmc_exceptiontypes.hpp"
#include <sstream>
#include <iomanip>

using namespace AMC;

CUIExpression::CUIExpression()
{

}

CUIExpression::CUIExpression(const pugi::xml_node& xmlNode, const std::string& attributeName, bool bValueMustExist)
{
	readFromXML(xmlNode, attributeName, "", bValueMustExist);
}

CUIExpression::CUIExpression(const pugi::xml_node& xmlNode, const std::string& attributeName)
{
	readFromXML(xmlNode, attributeName, "", false);
}

CUIExpression::CUIExpression(const pugi::xml_node& xmlNode, const std::string& attributeName, const std::string& defaultValue)
{
	readFromXML(xmlNode, attributeName, defaultValue, false);
}

void CUIExpression::setFixedValue(const std::string& sValue)
{
	m_sFixedValue = sValue;
	m_sExpressionValue = "";
}


void CUIExpression::readFromXML(const pugi::xml_node& xmlNode, const std::string& attributeName, const std::string& defaultValue, bool bValueMustExist)
{
	auto valueAttrib = xmlNode.attribute(attributeName.c_str());
	auto syncvalueAttrib = xmlNode.attribute(("sync:" + attributeName).c_str());
	auto formatvalueAttrib = xmlNode.attribute(("format:" + attributeName).c_str());

	std::string sValue = valueAttrib.as_string();
	std::string sSyncValue = syncvalueAttrib.as_string();
	m_sFormatString = formatvalueAttrib.as_string();

	if ((!sValue.empty()) && (!sSyncValue.empty()))
		throw ELibMCCustomException(LIBMC_ERROR_EXPRESSIONVALUEGIVENTWICE, attributeName);

	if (!sSyncValue.empty()) {
		m_sExpressionValue = sSyncValue;
	}
	else {
		if (!valueAttrib.empty()) {
			m_sFixedValue = sValue;
		}
		else {

			if (bValueMustExist) 
				throw ELibMCCustomException(LIBMC_ERROR_EXPRESSIONVALUEMISSING, attributeName);

			m_sFixedValue = defaultValue;
		}
	}
}

std::string CUIExpression::evaluateValueEx(CStateMachineData* pStateMachineData)
{
	if (!m_sExpressionValue.empty()) {
		LibMCAssertNotNull(pStateMachineData);

		std::string sParameterInstanceName, sParameterGroupName, sParameterName;
		CStateMachineData::extractParameterDetailsFromDotString(m_sExpressionValue, sParameterInstanceName, sParameterGroupName, sParameterName, true, true);

		if (!sParameterName.empty()) {

			auto pParameterHandler = pStateMachineData->getParameterHandler(sParameterInstanceName);

			auto pParameterGroup = pParameterHandler->findGroup(sParameterGroupName, true);
			return pParameterGroup->getParameterValueByName(sParameterName);
		}
		else {

			if (sParameterGroupName == "$state") {
				return pStateMachineData->getInstanceStateName(sParameterInstanceName);
			}
			else {
				throw ELibMCCustomException(LIBMC_ERROR_INVALIDEXPRESSIONVALUE, m_sExpressionValue);
			}
		}
	}
	else {
		return m_sFixedValue;
	}
}

const char* parseDotString(const char* pChar, uint32_t& nPrecision)
{
	LibMCAssertNotNull(pChar);

	nPrecision = 0;

	while (true) {
		char ch = *pChar;
		if ((ch >= '0') && (ch <= '9')) {
			nPrecision = nPrecision * 10 + (uint32_t) (ch - '0');
		}
		else {
			if (ch == 'f') {
				return pChar;
			}
			else {
				throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDUIFORMAT);
			}
		} 


		pChar++;
	}

	throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDUIFORMAT);

}

std::string CUIExpression::evaluateStringValue(CStateMachineData* pStateMachineData)
{
	if (m_sFormatString.empty ())
		return evaluateValueEx(pStateMachineData);

	std::stringstream sStream;
	const char* pChar = m_sFormatString.c_str();

	uint32_t nPrecision = 0;

	while (*pChar) {
		char ch = *pChar;
		pChar++;

		if (ch != '%') {
			sStream << ch;
		}
		else {
			switch (*pChar) {
			case '%':
				sStream << '%';
				break;

			case 'd':
				sStream << evaluateIntegerValue (pStateMachineData);
				break;

			case 'f':
				sStream << evaluateNumberValue(pStateMachineData);
				break;

			case 's':
				sStream << evaluateValueEx(pStateMachineData);
				break;

			case '.': 
				pChar = parseDotString(pChar + 1, nPrecision);

				sStream << std::fixed << std::setprecision(nPrecision) << evaluateNumberValue(pStateMachineData);
				break;

			default:
				throw ELibMCCustomException(LIBMC_ERROR_INVALIDUIFORMAT, m_sFormatString);
			}

			pChar++;
		}
	}

	return sStream.str();
	
	
}

std::string CUIExpression::evaluateStringValue(PStateMachineData pStateMachineData)
{
	return evaluateStringValue(pStateMachineData.get());
}

double CUIExpression::evaluateNumberValue(CStateMachineData* pStateMachineData)
{
	if (!m_sExpressionValue.empty()) {
		LibMCAssertNotNull(pStateMachineData);

		std::string sParameterInstanceName, sParameterGroupName, sParameterName;
		CStateMachineData::extractParameterDetailsFromDotString(m_sExpressionValue, sParameterInstanceName, sParameterGroupName, sParameterName, false, false);

		auto pParameterHandler = pStateMachineData->getParameterHandler(sParameterInstanceName);
		auto pParameterGroup = pParameterHandler->findGroup(sParameterGroupName, true);
		return pParameterGroup->getDoubleParameterValueByName(sParameterName);

	}
	else {
		std::string sExpression = AMCCommon::CUtils::trimString(m_sFixedValue);
		if (sExpression.empty())
			return 0;

		std::string::const_iterator it = sExpression.begin();
		while (it != sExpression.end() && (std::isdigit(*it) || (*it == '+') || (*it == '-') || (*it == '.'))) ++it;
		bool bIsNumber = !sExpression.empty() && it == sExpression.end();

		if (!bIsNumber)
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDNUMBEREXPRESSION, sExpression);

		return AMCCommon::CUtils::stringToDouble(sExpression);
	}

}

double CUIExpression::evaluateNumberValue(PStateMachineData pStateMachineData)
{
	return evaluateNumberValue(pStateMachineData.get());
}

int64_t CUIExpression::evaluateIntegerValue(CStateMachineData* pStateMachineData)
{
	if (!m_sExpressionValue.empty()) {
		LibMCAssertNotNull(pStateMachineData);

		std::string sParameterInstanceName, sParameterGroupName, sParameterName;
		CStateMachineData::extractParameterDetailsFromDotString(m_sExpressionValue, sParameterInstanceName, sParameterGroupName, sParameterName, false, false);

		auto pParameterHandler = pStateMachineData->getParameterHandler(sParameterInstanceName);
		auto pParameterGroup = pParameterHandler->findGroup(sParameterGroupName, true);
		return pParameterGroup->getIntParameterValueByName(sParameterName);

	}
	else {
		std::string sExpression = AMCCommon::CUtils::trimString(m_sFixedValue);
		if (sExpression.empty())
			return 0;

		std::string::const_iterator it = sExpression.begin();
		while (it != sExpression.end() && (std::isdigit(*it) || (*it == '+') || (*it == '-'))) ++it;
		bool bIsNumber = !sExpression.empty() && it == sExpression.end();

		if (!bIsNumber)
			throw ELibMCCustomException (LIBMC_ERROR_INVALIDINTEGEREXPRESSION, sExpression);

		return AMCCommon::CUtils::stringToInteger(sExpression);
	}

}

int64_t CUIExpression::evaluateIntegerValue(PStateMachineData pStateMachineData)
{
	return evaluateIntegerValue(pStateMachineData.get());
}

bool CUIExpression::evaluateBoolValue(CStateMachineData* pStateMachineData)
{
	if (!m_sExpressionValue.empty()) {
		LibMCAssertNotNull(pStateMachineData);

		std::string sTrimmedExpression = AMCCommon::CUtils::trimString(m_sExpressionValue);

		if (sTrimmedExpression.empty())
			return false;

		std::string sExpression;
		bool bInvert;
		if (sTrimmedExpression.at(0) == '!') {
			sExpression = sTrimmedExpression.substr(1);
			bInvert = true;
		}
		else {
			sExpression = sTrimmedExpression;
			bInvert = false;
		}

		std::string sParameterInstanceName, sParameterGroupName, sParameterName;
		CStateMachineData::extractParameterDetailsFromDotString(sExpression, sParameterInstanceName, sParameterGroupName, sParameterName, false, false);

		auto pParameterHandler = pStateMachineData->getParameterHandler(sParameterInstanceName);
		auto pParameterGroup = pParameterHandler->findGroup(sParameterGroupName, true);
		
		bool bValue = (pParameterGroup->getBoolParameterValueByName(sParameterName));

		if (bInvert)
			return !bValue;

		return bValue;

	}
	else {
		std::string sTrimmedExpression = AMCCommon::CUtils::trimString(m_sFixedValue);
		if (sTrimmedExpression.empty())
			return false;

		std::string sExpression;
		bool bInvert;
		if (sTrimmedExpression.at(0) == '!') {
			sExpression = sTrimmedExpression.substr(1);
			bInvert = true;
		}
		else {
			sExpression = sTrimmedExpression;
			bInvert = false;
		}

		std::string::const_iterator it = sExpression.begin();
		while (it != sExpression.end() && (std::isdigit(*it) || (*it == '+') || (*it == '-'))) ++it;
		bool bIsNumber = !sExpression.empty() && it == sExpression.end();

		if (!bIsNumber)
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDBOOLEANEXPRESSION, sExpression);

		int64_t nValue = AMCCommon::CUtils::stringToInteger(sExpression);
		if (bInvert)
			return (nValue == 0);

		return (nValue != 0);
	}




}

bool CUIExpression::evaluateBoolValue(PStateMachineData pStateMachineData)
{
	return evaluateBoolValue(pStateMachineData.get ());
}


void CUIExpression::checkExpressionSyntax(CStateMachineData* pStateMachineData)
{
	evaluateValueEx(pStateMachineData);
}

void CUIExpression::checkExpressionSyntax(PStateMachineData pStateMachineData)
{
	checkExpressionSyntax(pStateMachineData.get());
}

bool CUIExpression::needsSync()
{
	return !m_sExpressionValue.empty();
}

bool CUIExpression::isEmpty(CStateMachineData* pStateMachineData)
{
	std::string sValue = evaluateValueEx(pStateMachineData);
	return sValue.empty();
}

bool CUIExpression::isEmpty(PStateMachineData pStateMachineData)
{
	return isEmpty(pStateMachineData.get ());
}
