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

#include "amc_xmldocumentattribute.hpp"
#include "amc_xmldocumentnode.hpp"
#include "amc_xmldocument.hpp"

#include "libmc_exceptiontypes.hpp"
#include "PugiXML/pugixml.hpp"

using namespace AMC;

CXMLDocumentAttributeInstance::CXMLDocumentAttributeInstance(CXMLDocumentInstance* pDocument, CXMLDocumentNodeInstance* pNode, PXMLDocumentNameSpace pNameSpace, const std::string& sAttributeName)
	: m_pDocument (pDocument), m_pNode (pNode), m_sAttributeName (sAttributeName), m_pNameSpace(pNameSpace)
{
	if (pNameSpace == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
}

CXMLDocumentAttributeInstance::~CXMLDocumentAttributeInstance()
{

}

std::string CXMLDocumentAttributeInstance::getAttributeName()
{
	return m_sAttributeName;
}

PXMLDocumentNameSpace CXMLDocumentAttributeInstance::getNameSpace()
{
	return m_pNameSpace;
}

std::string CXMLDocumentAttributeInstance::getValue()
{
	return m_sValue;
}

void CXMLDocumentAttributeInstance::setValue(const std::string& sValue)
{
	m_sValue = sValue;
}

std::string CXMLDocumentAttributeInstance::getPrefixedName()
{
	std::string sNameSpacePrefix = m_pNameSpace->getPrefix();
	std::string sNodeNameSpacePrefix = m_pNode->GetNameSpace()->getPrefix();

	if (sNameSpacePrefix != sNodeNameSpacePrefix) {
		if (!sNameSpacePrefix.empty())
			return sNameSpacePrefix + ":" + m_sAttributeName;
	}
	
	return m_sAttributeName;

}

CXMLDocumentNodeInstance* CXMLDocumentAttributeInstance::getNode()
{
	return m_pNode;
}
