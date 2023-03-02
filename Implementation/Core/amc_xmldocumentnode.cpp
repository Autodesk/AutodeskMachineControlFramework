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

#include "amc_xmldocumentnode.hpp"
#include "amc_xmldocument.hpp"

#include "libmc_exceptiontypes.hpp"

using namespace AMC;

CXMLDocumentNodeInstance::CXMLDocumentNodeInstance(CXMLDocumentInstance* pDocument, pugi::xml_node xmlNode)
	: m_pDocument (pDocument), m_XMLNode (xmlNode)
{
	if (pDocument == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

}

CXMLDocumentNodeInstance::~CXMLDocumentNodeInstance()
{
}

bool CXMLDocumentNodeInstance::checkXMLNamespaceName(const std::string& sNodeName)
{
	return true;
}

bool CXMLDocumentNodeInstance::checkXMLNodeName(const std::string& sNodeName)
{
	return true;
}

bool CXMLDocumentNodeInstance::checkXMLNamespacePrefixName(const std::string& sNodeName)
{
	return true;
}

std::string CXMLDocumentNodeInstance::GetName()
{
	std::string sName = m_XMLNode.name ();
	return sName;
}

std::string CXMLDocumentNodeInstance::GetNameSpace()
{
	return "";
}

uint64_t CXMLDocumentNodeInstance::GetAttributeCount()
{
	uint64_t nCount = 0;
	auto attributes = m_XMLNode.attributes();
	for (auto attribute : attributes)
		nCount++;
	return nCount;
}

std::string CXMLDocumentNodeInstance::GetAttributeName(const uint64_t nIndex)
{
	uint64_t nCount = 0;
	auto attributes = m_XMLNode.attributes();
	for (auto attribute : attributes)
	{
		if (nIndex == nCount)
			return attribute.name();
	}

	throw ELibMCCustomException(LIBMC_ERROR_INVALIDATTRIBUTEINDEX, std::to_string (nIndex));

}

bool CXMLDocumentNodeInstance::HasAttribute(const std::string& sName)
{
	auto attribute = m_XMLNode.attribute(sName.c_str());
	return !attribute.empty();
}

void CXMLDocumentNodeInstance::RemoveAttribute(const std::string& sName)
{
	throw ELibMCInterfaceException(LIBMC_ERROR_NOTIMPLEMENTED);
}

void CXMLDocumentNodeInstance::AddAttribute(const std::string& sName, const std::string& sNameSpace, const std::string& sValue)
{
	throw ELibMCInterfaceException(LIBMC_ERROR_NOTIMPLEMENTED);
}

void CXMLDocumentNodeInstance::AddIntegerAttribute(const std::string& sName, const std::string& sNameSpace, const int64_t nValue)
{
	AddAttribute(sName, sNameSpace, std::to_string(nValue));

}

void CXMLDocumentNodeInstance::AddDoubleAttribute(const std::string& sName, const std::string& sNameSpace, const double dValue)
{
	AddAttribute(sName, sNameSpace, std::to_string(dValue));
}

void CXMLDocumentNodeInstance::AddBoolAttribute(const std::string& sName, const std::string& sNameSpace, const bool bValue)
{
	if (bValue)
		AddAttribute(sName, sNameSpace, "true");
	else
		AddAttribute(sName, sNameSpace, "false");

}

uint64_t CXMLDocumentNodeInstance::CountChildrenByName(const std::string& sName)
{
	throw ELibMCInterfaceException(LIBMC_ERROR_NOTIMPLEMENTED);
}

bool CXMLDocumentNodeInstance::HasChild(const std::string& sName)
{
	throw ELibMCInterfaceException(LIBMC_ERROR_NOTIMPLEMENTED);
}

bool CXMLDocumentNodeInstance::HasUniqueChild(const std::string& sName)
{
	throw ELibMCInterfaceException(LIBMC_ERROR_NOTIMPLEMENTED);
}

PXMLDocumentNodeInstance CXMLDocumentNodeInstance::FindChild(const std::string& sName, const bool bMustExist)
{
	throw ELibMCInterfaceException(LIBMC_ERROR_NOTIMPLEMENTED);
}

PXMLDocumentNodeInstance CXMLDocumentNodeInstance::AddChild(const std::string& sName, const std::string& sNameSpace)
{
	throw ELibMCInterfaceException(LIBMC_ERROR_NOTIMPLEMENTED);
}

void CXMLDocumentNodeInstance::RemoveChild(CXMLDocumentNodeInstance* pChildInstance)
{
	throw ELibMCInterfaceException(LIBMC_ERROR_NOTIMPLEMENTED);
}

void CXMLDocumentNodeInstance::RemoveChildrenWithName(const std::string& sName)
{
	throw ELibMCInterfaceException(LIBMC_ERROR_NOTIMPLEMENTED);
}

void CXMLDocumentNodeInstance::Remove()
{
	throw ELibMCInterfaceException(LIBMC_ERROR_NOTIMPLEMENTED);
}

