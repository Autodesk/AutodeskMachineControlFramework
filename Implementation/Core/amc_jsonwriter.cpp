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


#include "amc_jsonwriter.hpp"
#include "libmc_exceptiontypes.hpp"

using namespace AMC;

CJSONWriterObject::CJSONWriterObject(CJSONWriter& writer)
	: m_allocator(writer.m_allocator)
{
	m_Value.SetObject();
}


bool CJSONWriterObject::isEmpty()
{
	return m_Value.ObjectEmpty();
}

void CJSONWriterObject::addString(const std::string& sName, const std::string& sValue) {
	rapidjson::Value strName;
	strName.SetString(sName.c_str(), m_allocator);
	rapidjson::Value strValue;
	strValue.SetString(sValue.c_str(), m_allocator);

	m_Value.AddMember(strName, strValue, m_allocator);
}

void CJSONWriterObject::addInteger(const std::string& sName, int64_t nValue) {
	rapidjson::Value strName;
	strName.SetString(sName.c_str(), m_allocator);
	rapidjson::Value strValue;
	strValue.SetInt64(nValue);

	m_Value.AddMember(strName, strValue, m_allocator);
}

void CJSONWriterObject::addBool(const std::string& sName, bool bValue)
{
	rapidjson::Value strName;
	strName.SetString(sName.c_str(), m_allocator);
	rapidjson::Value strValue;
	strValue.SetBool(bValue);

	m_Value.AddMember(strName, strValue, m_allocator);

}


void CJSONWriterObject::addDouble(const std::string& sName, double dValue)
{
	rapidjson::Value strName;
	strName.SetString(sName.c_str(), m_allocator);
	rapidjson::Value strValue;
	strValue.SetDouble(dValue);

	m_Value.AddMember(strName, strValue, m_allocator);
}


void CJSONWriterObject::addObject(const std::string& sName, CJSONWriterObject& object)
{
	rapidjson::Value strName;
	strName.SetString(sName.c_str(), m_allocator);

	m_Value.AddMember(strName, object.m_Value, m_allocator);
}

void CJSONWriterObject::addArray(const std::string& sName, CJSONWriterArray& array)
{
	rapidjson::Value strName;
	strName.SetString(sName.c_str(), m_allocator);

	m_Value.AddMember(strName, array.m_Value, m_allocator);
}

void CJSONWriterObject::copyFromObject(const rapidjson::Value& objectValue)
{
	m_Value.CopyFrom(objectValue, m_allocator);
}

void CJSONWriterObject::addMembersFrom(const rapidjson::Value& objectValue)
{
	if (!objectValue.IsObject())
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	for (auto iMember = objectValue.MemberBegin(); iMember != objectValue.MemberEnd(); iMember++) {
		rapidjson::Value nameValue(iMember->name, m_allocator);
		rapidjson::Value memberValue(iMember->value, m_allocator);
		m_Value.AddMember(nameValue, memberValue, m_allocator);
	}
}

const rapidjson::Value& CJSONWriterObject::getValue() const
{
	return m_Value;
}

void CJSONWriterObject::addRawJSON(const std::string& sName, const std::string& sJSON)
{
	rapidjson::Document parsedDocument;
	parsedDocument.Parse(sJSON.c_str());
	if (parsedDocument.HasParseError())
		throw ELibMCCustomException(LIBMC_ERROR_COULDNOTPARSEJSON, sName);

	rapidjson::Value nameValue;
	nameValue.SetString(sName.c_str(), m_allocator);
	rapidjson::Value copiedValue(parsedDocument, m_allocator);

	m_Value.AddMember(nameValue, copiedValue, m_allocator);
}

void CJSONWriterObject::addNull(const std::string& sName)
{
	rapidjson::Value nameValue;
	nameValue.SetString(sName.c_str(), m_allocator);
	rapidjson::Value nullValue(rapidjson::kNullType);

	m_Value.AddMember(nameValue, nullValue, m_allocator);
}



CJSONWriterArray::CJSONWriterArray(CJSONWriter& writer)
	: m_allocator(writer.m_allocator), m_Value((rapidjson::kArrayType))
{
}

bool CJSONWriterArray::isEmpty()
{
	return (m_Value.Empty());
}

void CJSONWriterArray::addString(const std::string& sValue) {
	rapidjson::Value strValue;
	strValue.SetString(sValue.c_str(), m_allocator);

	m_Value.PushBack(strValue, m_allocator);
}

void CJSONWriterArray::addInteger(int64_t nValue) {
	rapidjson::Value strValue;
	strValue.SetInt64(nValue);

	m_Value.PushBack(strValue, m_allocator);
}

void CJSONWriterArray::addDouble(const std::string& sName, double dValue)
{
	rapidjson::Value strValue;
	strValue.SetDouble(dValue);

	m_Value.PushBack(strValue, m_allocator);
}


void CJSONWriterArray::addObject(CJSONWriterObject& object)
{
	m_Value.PushBack (object.m_Value, m_allocator);
}

void CJSONWriterArray::addArray(CJSONWriterArray& array)
{
	m_Value.PushBack(array.m_Value, m_allocator);
}

void CJSONWriterArray::copyFromArray(const rapidjson::Value& arrayValue)
{
	m_Value.CopyFrom(arrayValue, m_allocator);
}



CJSONWriter::CJSONWriter()
	: m_allocator(m_document.GetAllocator())
{
	m_document.SetObject();
}

std::string CJSONWriter::saveToString()
{
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	m_document.Accept(writer);

	return buffer.GetString();
}

void CJSONWriter::addString(const std::string& sName, const std::string& sValue) {
	rapidjson::Value strName;
	strName.SetString(sName.c_str(), m_allocator);
	rapidjson::Value strValue;
	strValue.SetString(sValue.c_str(), m_allocator);

	m_document.AddMember(strName, strValue, m_allocator);
}

void CJSONWriter::addInteger(const std::string& sName, int64_t nValue) {
	rapidjson::Value strName;
	strName.SetString(sName.c_str(), m_allocator);
	rapidjson::Value strValue;
	strValue.SetInt64(nValue);

	m_document.AddMember(strName, strValue, m_allocator);
}

void CJSONWriter::addDouble(const std::string& sName, double dValue)
{
	rapidjson::Value strName;
	strName.SetString(sName.c_str(), m_allocator);
	rapidjson::Value strValue;
	strValue.SetDouble(dValue);

	m_document.AddMember(strName, strValue, m_allocator);
}

void CJSONWriter::addBoolean(const std::string& sName, bool bValue)
{
	rapidjson::Value strName;
	strName.SetString(sName.c_str(), m_allocator);
	rapidjson::Value strValue;
	strValue.SetBool(bValue);

	m_document.AddMember(strName, strValue, m_allocator);

}


void CJSONWriter::addObject(const std::string& sName, CJSONWriterObject& object)
{
	rapidjson::Value strName;
	strName.SetString(sName.c_str(), m_allocator);

	m_document.AddMember(strName, object.m_Value, m_allocator);
}

void CJSONWriter::addArray(const std::string& sName, CJSONWriterArray& array)
{
	rapidjson::Value strName;
	strName.SetString(sName.c_str(), m_allocator);

	m_document.AddMember(strName, array.m_Value, m_allocator);

}

void CJSONWriter::copyFromDocument(const rapidjson::Document& document)
{
	m_document.CopyFrom(document, m_document.GetAllocator());
}

const rapidjson::Document& CJSONWriter::getDocument() const
{
	return m_document;
}


