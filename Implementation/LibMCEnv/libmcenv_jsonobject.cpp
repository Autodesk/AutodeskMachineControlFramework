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

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL AUTODESK INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


Abstract: This is a stub class definition of CJSONObject

*/

#include "libmcenv_jsonobject.hpp"
#include "libmcenv_interfaceexception.hpp"
#include "libmcenv_jsonarray.hpp"

#include "RapidJSON/writer.h"
#include "common_utils.hpp"

using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CJSONObject 
**************************************************************************************************************************/
CJSONObject::CJSONObject()	
	: m_pInstance(nullptr)
{
	m_pDocument = std::make_shared<rapidjson::Document>();	
	m_pDocument->SetObject();
	m_pInstance = m_pDocument.get();

}

CJSONObject::CJSONObject(std::shared_ptr<rapidjson::Document> pDocument, rapidjson::Value * pReferencedObject)
	: m_pInstance(nullptr)
{
	if (pDocument.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
	if (pReferencedObject == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

	if (!pReferencedObject->IsObject ())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_REFERENCEDJSONVALUEISNOTOBJECT );

	m_pDocument = pDocument;	
	m_pInstance = pReferencedObject;
}


CJSONObject::CJSONObject(const std::string& sJSONString)
	: m_pInstance(nullptr)
{
	m_pDocument = std::make_shared<rapidjson::Document>();
	m_pDocument->SetObject();

	if (!sJSONString.empty()) {

		m_pDocument->Parse(sJSONString.c_str (), sJSONString.length ());
		if (m_pDocument->HasParseError())
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNOTPARSEJSONSTRING);

		if (!m_pDocument->IsObject ())
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_JSONSTRINGISNOTOFTYPEOBJECT);

	}

	m_pInstance = m_pDocument.get();
}

CJSONObject::CJSONObject(const uint8_t* pData, uint64_t nDataSize)
	: m_pInstance(nullptr)
{
	if ((pData == nullptr) && (nDataSize > 0))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

	m_pDocument = std::make_shared<rapidjson::Document>();
	m_pDocument->SetObject();

	if (nDataSize > 0) {
		m_pDocument->Parse((const char*)pData, nDataSize);
		if (m_pDocument->HasParseError())
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNOTPARSEJSONDATA);

		if (!m_pDocument->IsObject())
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_JSONDATAISNOTOFTYPEOBJECT);

	}

	m_pInstance = m_pDocument.get();

}

CJSONObject::~CJSONObject()
{

}

bool CJSONObject::HasMember(const std::string & sName)
{
	return m_pInstance->HasMember(sName.c_str());

}

LibMCEnv_uint64 CJSONObject::GetMemberCount()
{
	return m_pInstance->MemberCount();
}

std::string CJSONObject::GetMemberName(const LibMCEnv_uint64 nIndex)
{
	if (nIndex >= m_pInstance->MemberCount())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDJSONMEMBERINDEX);

	auto member = m_pInstance->MemberBegin() + nIndex;
	return member->name.GetString ();
}

LibMCEnv::eJSONObjectType CJSONObject::GetMemberType(const std::string & sName)
{
	if (m_pInstance->HasMember(sName.c_str())) {
		auto& member = (*m_pInstance)[sName.c_str()];
		rapidjson::Type memberType = member.GetType();
		switch (memberType) {
			case rapidjson::Type::kStringType:
				return LibMCEnv::eJSONObjectType::StringType;
			case rapidjson::Type::kNumberType:
				return LibMCEnv::eJSONObjectType::DoubleType;
			case rapidjson::Type::kTrueType:
			case rapidjson::Type::kFalseType:
				return LibMCEnv::eJSONObjectType::BoolType;
			case rapidjson::Type::kObjectType:
				return LibMCEnv::eJSONObjectType::ObjectType;
			case rapidjson::Type::kArrayType:
				return LibMCEnv::eJSONObjectType::ArrayType;
			default:
				throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_JSONMEMBERHASINVALIDTYPE, "JSON member has invalid type: " + sName);
		}
	}
	else {
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_JSONMEMBERNOTFOUND, "JSON member not found: " + sName);
	}
}

std::string CJSONObject::GetValue(const std::string & sName)
{
	if (m_pInstance->HasMember(sName.c_str())) {
		auto & member = (*m_pInstance)[sName.c_str()];
		return member.GetString();
	}
	else {
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_JSONMEMBERNOTFOUND, "JSON member not found: " + sName);
	}

}

std::string CJSONObject::GetUUIDValue(const std::string& sName)
{
	if (m_pInstance->HasMember(sName.c_str())) {
		auto& member = (*m_pInstance)[sName.c_str()];
		std::string sUUIDValue (member.GetString());

		return AMCCommon::CUtils::normalizeUUIDString(sUUIDValue);
	}
	else {
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_JSONMEMBERNOTFOUND, "JSON member not found: " + sName);
	}

}


LibMCEnv_int64 CJSONObject::GetIntegerValue(const std::string & sName)
{
	if (m_pInstance->HasMember(sName.c_str())) {
		auto& member = (*m_pInstance)[sName.c_str()];
		if (member.IsInt64()) {
			return member.GetInt64();
		}

		if (member.IsString()) {
			return AMCCommon::CUtils::stringToInteger(member.GetString());
		}
		
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_JSONMEMBERISNOTINTEGER, "JSON member is not integer: " + sName);

	}
	else {
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_JSONMEMBERNOTFOUND, "JSON member not found: " + sName);
	}
}

LibMCEnv_double CJSONObject::GetDoubleValue(const std::string & sName)
{
	if (m_pInstance->HasMember(sName.c_str())) {
		auto& member = (*m_pInstance)[sName.c_str()];
		if (member.IsDouble()) {
			return member.GetDouble();
		}

		if (member.IsInt64() || member.IsInt ()) {
			return (double)member.GetInt64();
		}

		if (member.IsString()) {
			return AMCCommon::CUtils::stringToDouble(member.GetString());
		}


		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_JSONMEMBERISNOTDOUBLE, "JSON member is not double: " + sName);

	}
	else {
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_JSONMEMBERNOTFOUND, "JSON member not found: " + sName);
	}
}

bool CJSONObject::GetBoolValue(const std::string & sName)
{
	if (m_pInstance->HasMember(sName.c_str())) {
		auto& member = (*m_pInstance)[sName.c_str()];
		if (!member.IsBool())
			return member.GetBool();

		if (member.IsInt64() || member.IsInt()) {
			return (member.GetInt64() != 0);
		}

		if (member.IsString()) {
			return AMCCommon::CUtils::stringToBool(member.GetString());
		}

		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_JSONMEMBERISNOTBOOL, "JSON member is not bool: " + sName);

	}
	else {
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_JSONMEMBERNOTFOUND, "JSON member not found: " + sName);
	}
}

IJSONObject * CJSONObject::GetObjectValue(const std::string & sName)
{
	if (m_pInstance->HasMember(sName.c_str())) {
		auto pMember = &(*m_pInstance)[sName.c_str()];
		if (!pMember->IsObject())
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_JSONMEMBERISNOTOBJECT, "JSON member is not object: " + sName);

		return new CJSONObject (m_pDocument, pMember);
	}
	else {
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_JSONMEMBERNOTFOUND, "JSON member not found: " + sName);
	}
}

IJSONArray * CJSONObject::GetArrayValue(const std::string & sName)
{
	if (m_pInstance->HasMember(sName.c_str())) {
		auto pMember = &(*m_pInstance)[sName.c_str()];
		if (!pMember->IsArray())
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_JSONMEMBERISNOTARRAY, "JSON member is not array: " + sName);

		return new CJSONArray(m_pDocument, pMember);
	}
	else {
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_JSONMEMBERNOTFOUND, "JSON member not found: " + sName);
	}
}

void CJSONObject::RemoveMember(const std::string & sName)
{
	if (m_pInstance->HasMember(sName.c_str())) {
		auto iIterator = m_pInstance->FindMember(sName.c_str());
		m_pInstance->RemoveMember(iIterator);
	}
}

void CJSONObject::AddValue(const std::string & sName, const std::string & sValue)
{
	rapidjson::Value jsonName;
	rapidjson::Value jsonValue;

	jsonName.SetString(sName.c_str(), m_pDocument->GetAllocator ());
	jsonValue.SetString(sValue.c_str(), m_pDocument->GetAllocator());

	m_pInstance->AddMember(jsonName, jsonValue, m_pDocument->GetAllocator());
}


void CJSONObject::AddIntegerValue(const std::string& sName, const LibMCEnv_int64 nValue)
{
	rapidjson::Value jsonName;
	rapidjson::Value jsonValue;

	jsonName.SetString(sName.c_str(), m_pDocument->GetAllocator());
	jsonValue.SetInt64(nValue);

	m_pInstance->AddMember(jsonName, jsonValue, m_pDocument->GetAllocator());

}

void CJSONObject::AddDoubleValue(const std::string& sName, const LibMCEnv_double dValue)
{
	rapidjson::Value jsonName;
	rapidjson::Value jsonValue;

	jsonName.SetString(sName.c_str(), m_pDocument->GetAllocator());
	jsonValue.SetDouble(dValue);

	m_pInstance->AddMember(jsonName, jsonValue, m_pDocument->GetAllocator());
}

void CJSONObject::AddBoolValue(const std::string& sName, const bool bValue)
{
	rapidjson::Value jsonName;
	rapidjson::Value jsonValue;

	jsonName.SetString(sName.c_str(), m_pDocument->GetAllocator());
	jsonValue.SetBool(bValue);

	m_pInstance->AddMember(jsonName, jsonValue, m_pDocument->GetAllocator());
}

IJSONObject * CJSONObject::AddObjectValue(const std::string & sName)
{
	rapidjson::Value jsonName;
	rapidjson::Value jsonValue;

	jsonName.SetString(sName.c_str(), m_pDocument->GetAllocator());
	jsonValue.SetObject ();

	m_pInstance->AddMember(jsonName, jsonValue, m_pDocument->GetAllocator());

	// Get pointer to the newly added member value (not the parent object)
	auto pMember = &(*m_pInstance)[sName.c_str()];

	return new CJSONObject (m_pDocument, pMember);

}

IJSONArray * CJSONObject::AddArrayValue(const std::string & sName)
{
	rapidjson::Value jsonName;
	rapidjson::Value jsonValue;

	jsonName.SetString(sName.c_str(), m_pDocument->GetAllocator());
	jsonValue.SetArray();

	m_pInstance->AddMember(jsonName, jsonValue, m_pDocument->GetAllocator());

	// Get pointer to the newly added member value (not the parent object)
	auto pMember = &(*m_pInstance)[sName.c_str()];

	return new CJSONArray(m_pDocument, pMember);
}

std::string CJSONObject::SerializeToString()
{
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	m_pInstance->Accept(writer);

	return buffer.GetString();

}
