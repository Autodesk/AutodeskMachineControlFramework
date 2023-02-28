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
#include "libmc_exceptiontypes.hpp"

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
	auto rootNode = m_Document.root();
	rootNode.set_name(sRootNodeName.c_str());

	auto xmlNSAttribute = rootNode.append_attribute("xmlns");
	xmlNSAttribute.set_value(sDefaultNamespace.c_str());

	m_sDefaultNamespace = sDefaultNamespace;
}

void CXMLDocumentInstance::parseXMLString(const std::string& sXMLString)
{
	throw ELibMCInterfaceException(LIBMC_ERROR_NOTIMPLEMENTED);
}

void CXMLDocumentInstance::parseXMLData(uint64_t nDataSize, const uint8_t* pData)
{
	throw ELibMCInterfaceException(LIBMC_ERROR_NOTIMPLEMENTED);
}

std::string CXMLDocumentInstance::GetDefaultNamespace()
{
	throw ELibMCInterfaceException(LIBMC_ERROR_NOTIMPLEMENTED);
}

uint64_t CXMLDocumentInstance::GetNamespaceCount()
{
	throw ELibMCInterfaceException(LIBMC_ERROR_NOTIMPLEMENTED);
}

void CXMLDocumentInstance::GetNamespace(const uint64_t nIndex, std::string& sNamespace, std::string& sNamespacePrefix)
{
	throw ELibMCInterfaceException(LIBMC_ERROR_NOTIMPLEMENTED);
}

bool CXMLDocumentInstance::HasNamespace(const std::string& sNamespace)
{
	throw ELibMCInterfaceException(LIBMC_ERROR_NOTIMPLEMENTED);
}

void CXMLDocumentInstance::GetNamespacePrefix(const std::string& sNamespace, std::string& sNamespacePrefix)
{
	throw ELibMCInterfaceException(LIBMC_ERROR_NOTIMPLEMENTED);
}

void CXMLDocumentInstance::RegisterNamespace(const std::string& sNamespace, const std::string& sNamespacePrefix)
{
	throw ELibMCInterfaceException(LIBMC_ERROR_NOTIMPLEMENTED);
}

PXMLDocumentNodeInstance CXMLDocumentInstance::GetRootNode()
{
	throw ELibMCInterfaceException(LIBMC_ERROR_NOTIMPLEMENTED);
}

