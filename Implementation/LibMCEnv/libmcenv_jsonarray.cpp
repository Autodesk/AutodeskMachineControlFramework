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


Abstract: This is a stub class definition of CJSONArray

*/

#include "libmcenv_jsonarray.hpp"
#include "libmcenv_interfaceexception.hpp"
#include "libmcenv_jsonobject.hpp"

#include "common_utils.hpp"

#include "RapidJSON/writer.h"

using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CJSONArray 
**************************************************************************************************************************/

CJSONArray::CJSONArray(std::shared_ptr<rapidjson::Document> pDocument, rapidjson::Value* pReferencedArray)
	: m_pInstance(nullptr)
{
	if (pDocument.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
	if (pReferencedArray == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

	if (!pReferencedArray->IsArray())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_REFERENCEDJSONVALUEISNOTARRAY);

	m_pDocument = pDocument;
	m_pInstance = pReferencedArray;
}

CJSONArray::~CJSONArray()
{

}


LibMCEnv_uint64 CJSONArray::GetElementCount()
{
	return m_pInstance->MemberCount();
}

LibMCEnv::eJSONObjectType CJSONArray::GetElementType(const LibMCEnv_uint64 nIndex)
{
	if (nIndex >= m_pInstance->MemberCount())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDJSONMEMBERINDEX, "invalid JSON member index: #" + std::to_string (nIndex));

	auto& member = (*m_pInstance)[(rapidjson::SizeType)nIndex];

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
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_JSONMEMBERHASINVALIDTYPE, "JSON member has invalid type: #" + std::to_string (nIndex));
	}
}

std::string CJSONArray::GetValue(const LibMCEnv_uint64 nIndex)
{
	if (nIndex >= m_pInstance->MemberCount())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDJSONMEMBERINDEX, "invalid JSON member index: #" + std::to_string(nIndex));

	auto& member = (*m_pInstance)[(rapidjson::SizeType)nIndex];
	return member.GetString();
}

std::string CJSONArray::GetUUIDValue(const LibMCEnv_uint64 nIndex)
{
	if (nIndex >= m_pInstance->MemberCount())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDJSONMEMBERINDEX, "invalid JSON member index: #" + std::to_string(nIndex));

	auto& member = (*m_pInstance)[(rapidjson::SizeType)nIndex];
	std::string sUUIDValue(member.GetString());

	return AMCCommon::CUtils::normalizeUUIDString(sUUIDValue);

}


LibMCEnv_int64 CJSONArray::GetIntegerValue(const LibMCEnv_uint64 nIndex)
{
	if (nIndex >= m_pInstance->MemberCount())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDJSONMEMBERINDEX);

	auto& member = (*m_pInstance)[(rapidjson::SizeType)nIndex];
	if (member.IsInt64()) {
		return member.GetInt64();
	}

	if (member.IsString()) {
		return AMCCommon::CUtils::stringToInteger(member.GetString());
	}

	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_JSONMEMBERISNOTINTEGER, "JSON member is not integer: #" + std::to_string(nIndex));

}

LibMCEnv_double CJSONArray::GetDoubleValue(const LibMCEnv_uint64 nIndex)
{
	if (nIndex >= m_pInstance->MemberCount())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDJSONMEMBERINDEX, "invalid JSON member index: #" + std::to_string(nIndex));

	auto& member = (*m_pInstance)[(rapidjson::SizeType) nIndex];
	if (member.IsDouble()) {
		return member.GetDouble();
	}

	if (member.IsInt64() || member.IsInt()) {
		return (double)member.GetInt64();
	}

	if (member.IsString()) {
		return AMCCommon::CUtils::stringToDouble(member.GetString());
	}
	
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_JSONMEMBERISNOTDOUBLE, "JSON member is not double: #" + std::to_string(nIndex));

}

bool CJSONArray::GetBoolValue(const LibMCEnv_uint64 nIndex)
{
	if (nIndex >= m_pInstance->MemberCount())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDJSONMEMBERINDEX, "invalid JSON member index: #" + std::to_string(nIndex));

	auto& member = (*m_pInstance)[(rapidjson::SizeType)nIndex];
	if (!member.IsBool())
		return member.GetBool();

	if (member.IsInt64() || member.IsInt()) {
		return (member.GetInt64() != 0);
	}

	if (member.IsString()) {
		return AMCCommon::CUtils::stringToBool(member.GetString());
	}
	
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_JSONMEMBERISNOTBOOL, "JSON member is not bool: #" + std::to_string(nIndex));

}

IJSONObject * CJSONArray::GetObjectValue(const LibMCEnv_uint64 nIndex)
{
	if (nIndex >= m_pInstance->MemberCount())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDJSONMEMBERINDEX, "invalid JSON member index: #" + std::to_string(nIndex));

	auto pMember = &(*m_pInstance)[(rapidjson::SizeType)nIndex];
	if (!pMember->IsObject())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_JSONMEMBERISNOTOBJECT, "JSON member is not object: #" + std::to_string(nIndex));

	return new CJSONObject(m_pDocument, pMember);


}

IJSONArray * CJSONArray::GetArrayValue(const LibMCEnv_uint64 nIndex)
{
	if (nIndex >= m_pInstance->MemberCount())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDJSONMEMBERINDEX);

	auto pMember = &(*m_pInstance)[(rapidjson::SizeType)nIndex];
	if (!pMember->IsArray())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_JSONMEMBERISNOTARRAY, "JSON member is not array: #" + std::to_string(nIndex));

	return new CJSONArray(m_pDocument, pMember);

}

void CJSONArray::RemoveElement(const LibMCEnv_uint64 nIndex)
{
	if (nIndex >= m_pInstance->MemberCount())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDJSONMEMBERINDEX, "invalid JSON member index: #" + std::to_string(nIndex));

	auto iter = m_pInstance->MemberBegin() + nIndex;

	m_pInstance->RemoveMember(iter);

}

void CJSONArray::AddValue(const std::string & sValue)
{
	rapidjson::Value jsonValue;

	jsonValue.SetString(sValue.c_str(), m_pDocument->GetAllocator());

	m_pInstance->PushBack (jsonValue, m_pDocument->GetAllocator());
}

void CJSONArray::AddIntegerValue(const LibMCEnv_int64 nValue)
{
	rapidjson::Value jsonValue;

	jsonValue.SetInt64(nValue);

	m_pInstance->PushBack(jsonValue, m_pDocument->GetAllocator());
}

void CJSONArray::AddDoubleValue(const LibMCEnv_double dValue)
{
	rapidjson::Value jsonValue;

	jsonValue.SetDouble(dValue);

	m_pInstance->PushBack(jsonValue, m_pDocument->GetAllocator());
}

void CJSONArray::AddBoolValue(const bool bValue)
{
	rapidjson::Value jsonValue;

	jsonValue.SetBool(bValue);

	m_pInstance->PushBack(jsonValue, m_pDocument->GetAllocator());

}

IJSONObject * CJSONArray::AddObjectValue()
{
	rapidjson::Value jsonValue;

	jsonValue.SetObject();

	m_pInstance->PushBack(jsonValue, m_pDocument->GetAllocator());

	// Get pointer to the newly added element (last element in array)
	auto pMember = &(*m_pInstance)[m_pInstance->Size() - 1];

	return new CJSONObject(m_pDocument, pMember);
}

IJSONArray * CJSONArray::AddArrayValue()
{
	rapidjson::Value jsonValue;

	jsonValue.SetArray();

	m_pInstance->PushBack(jsonValue, m_pDocument->GetAllocator());

	// Get pointer to the newly added element (last element in array)
	auto pMember = &(*m_pInstance)[m_pInstance->Size() - 1];

	return new CJSONArray(m_pDocument, pMember);
}

std::string CJSONArray::SerializeToString()
{
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	m_pInstance->Accept(writer);

	return buffer.GetString();

}
