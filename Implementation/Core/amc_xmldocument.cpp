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


#include "amc_xmldocument.hpp"
#include "amc_xmldocumentnode.hpp"

#include "libmc_exceptiontypes.hpp"
#include <sstream>

using namespace AMC;

CXMLDocumentInstance::CXMLDocumentInstance()
{

}

CXMLDocumentInstance::~CXMLDocumentInstance()
{

}

void CXMLDocumentInstance::createEmptyDocument(const std::string& sRootNodeName, const std::string& sDefaultNamespace)
{
	if (!CXMLDocumentNodeInstance::checkXMLNamespaceName(sDefaultNamespace))
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDDEFAULTXMLNAMESPACE, sDefaultNamespace);
	if (!CXMLDocumentNodeInstance::checkXMLNodeName (sRootNodeName))
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDXMLROOTNODENAME, sRootNodeName);

	m_Document = pugi::xml_document();
	pugi::xml_node rootNode = m_Document.append_child(sRootNodeName.c_str());

	auto xmlNSAttribute = rootNode.append_attribute("xmlns");
	xmlNSAttribute.set_value(sDefaultNamespace.c_str());

	m_pRootNodeInstance = std::make_shared<CXMLDocumentNodeInstance>(this, rootNode);

	m_sDefaultNamespace = sDefaultNamespace;

	registerNamespaceEx(m_sDefaultNamespace, "");
}

void CXMLDocumentInstance::extractDocumentNamespaces()
{
	auto children = m_Document.children();
	if (children.empty())
		throw ELibMCInterfaceException(LIBMC_ERROR_XMLDOESNOTCONTAINROOTNODE);

	pugi::xml_node rootNode;
	size_t nChildCount = 0;
	for (auto child : children) {
		rootNode = child;
		nChildCount++;
	}

	if (nChildCount > 1)
		throw ELibMCInterfaceException(LIBMC_ERROR_XMLCONTAINSAMBIGOUSROOTNODES);

	pugi::xml_attribute xmlnsAttrib = rootNode.attribute("xmlns");
	m_sDefaultNamespace = xmlnsAttrib.as_string();
	if (m_sDefaultNamespace.empty())
		throw ELibMCInterfaceException(LIBMC_ERROR_XMLDOESNOTCONTAINNAMESPACE);

	m_pRootNodeInstance = std::make_shared<CXMLDocumentNodeInstance>(this, rootNode);
}

void CXMLDocumentInstance::registerNamespaceEx(const std::string& sNamespace, const std::string& sPrefix)
{
	auto iPrefixIter = m_PrefixToNamespaceMap.find(sPrefix);
	auto iNamespaceIter = m_NamespaceToPrefixMap.find(sNamespace);

	if (iPrefixIter != m_PrefixToNamespaceMap.end ())
		throw ELibMCCustomException(LIBMC_ERROR_XMLNAMESPACEPREFIXALREADYREGISTERED, sPrefix);
	if (iNamespaceIter != m_NamespaceToPrefixMap.end())
		throw ELibMCCustomException(LIBMC_ERROR_XMLNAMESPACEALREADYREGISTERED, sNamespace);

	m_Namespaces.push_back(sNamespace);
	m_NamespaceToPrefixMap.insert(std::make_pair (sNamespace, sPrefix));
	m_PrefixToNamespaceMap.insert(std::make_pair (sPrefix, sNamespace));

}


void CXMLDocumentInstance::parseXMLString(const std::string& sXMLString)
{
	m_Document = pugi::xml_document();

	pugi::xml_parse_result result = m_Document.load_string(sXMLString.c_str(), pugi::parse_minimal | pugi::parse_escapes | pugi::parse_eol );
	if (!result)
		throw ELibMCInterfaceException(LIBMC_ERROR_COULDNOTPARSEXMLSTRING);

	extractDocumentNamespaces();	 
}

void CXMLDocumentInstance::parseXMLData(uint64_t nDataSize, const uint8_t* pData)
{
	if ((pData == nullptr) || (nDataSize == 0))
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDXMLDATA);

	m_Document = pugi::xml_document();

	pugi::xml_parse_result result = m_Document.load_buffer(pData, nDataSize, pugi::parse_minimal | pugi::parse_escapes | pugi::parse_eol);
	if (!result)
		throw ELibMCInterfaceException(LIBMC_ERROR_COULDNOTPARSEXMLDATA);

	extractDocumentNamespaces();
}

std::string CXMLDocumentInstance::GetDefaultNamespace()
{
	return m_sDefaultNamespace;
}

uint64_t CXMLDocumentInstance::GetNamespaceCount()
{
	return m_Namespaces.size ();
}

void CXMLDocumentInstance::GetNamespace(const uint64_t nIndex, std::string& sNamespace, std::string& sNamespacePrefix)
{
	if (nIndex >= m_Namespaces.size ())
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDNAMESPACEINDEX, std::to_string (nIndex));

	sNamespace = m_Namespaces.at(nIndex);
	sNamespacePrefix = GetNamespacePrefix(sNamespace);

}

bool CXMLDocumentInstance::HasNamespace(const std::string& sNamespace)
{
	auto iNamespaceIter = m_NamespaceToPrefixMap.find(sNamespace);
	return (iNamespaceIter != m_NamespaceToPrefixMap.end());
		
}

std::string CXMLDocumentInstance::GetNamespacePrefix(const std::string& sNamespace)
{
	auto iNamespaceIter = m_NamespaceToPrefixMap.find(sNamespace);
	if (iNamespaceIter == m_NamespaceToPrefixMap.end())
		throw ELibMCCustomException(LIBMC_ERROR_XMLNAMESPACENOTFOUND, sNamespace);

	return iNamespaceIter->second;
}

void CXMLDocumentInstance::RegisterNamespace(const std::string& sNamespace, const std::string& sNamespacePrefix)
{
	if (!CXMLDocumentNodeInstance::checkXMLNamespaceName(sNamespace))
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDNAMESPACENAME, sNamespace);

	if (!CXMLDocumentNodeInstance::checkXMLNamespacePrefixName(sNamespacePrefix))
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDNAMESPACEPREFIX, sNamespacePrefix);

	registerNamespaceEx(sNamespace, sNamespacePrefix);
}

PXMLDocumentNodeInstance CXMLDocumentInstance::GetRootNode()
{
	if (m_pRootNodeInstance.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDXMLROOTNODEINSTANCE);

	return m_pRootNodeInstance;
}

struct pugi_xml_string_writer : pugi::xml_writer
{
	std::stringstream m_Stream;

	virtual void write(const void* data, size_t size)
	{
		m_Stream.write (static_cast<const char*>(data), size);
	}
};

std::string CXMLDocumentInstance::SaveToString(const bool bAddLineBreaks)
{
	pugi_xml_string_writer xmlWriter;
	uint32_t nFormat;
	if (bAddLineBreaks)
		nFormat = pugi::format_indent;
	else
		nFormat = pugi::format_raw;

	m_Document.save(xmlWriter, "  ", nFormat, pugi::encoding_utf8);

	return xmlWriter.m_Stream.str();
}

