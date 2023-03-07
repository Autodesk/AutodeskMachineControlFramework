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


Abstract: This is a stub class definition of CXMLDocumentNode

*/

#include "libmcenv_xmldocumentnode.hpp"
#include "libmcenv_xmldocumentnodes.hpp"
#include "libmcenv_interfaceexception.hpp"
#include "libmcenv_xmldocumentattribute.hpp"

// Include custom headers here.


using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CXMLDocumentNode 
**************************************************************************************************************************/

CXMLDocumentNode::CXMLDocumentNode(AMC::PXMLDocumentInstance pXMLDocument, AMC::PXMLDocumentNodeInstance pXMLDocumentNode)
	: m_pXMLDocument (pXMLDocument), m_pXMLDocumentNode (pXMLDocumentNode)
{
    if (pXMLDocument.get () == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

	if (pXMLDocumentNode.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

}

CXMLDocumentNode::~CXMLDocumentNode()
{

}


std::string CXMLDocumentNode::GetName()
{
	return m_pXMLDocumentNode->GetName();
}

std::string CXMLDocumentNode::GetNameSpace()
{
	return m_pXMLDocumentNode->GetNameSpace()->getNameSpaceName ();
}

LibMCEnv_uint64 CXMLDocumentNode::GetAttributeCount()
{
	return m_pXMLDocumentNode->GetAttributeCount();
}

IXMLDocumentAttribute * CXMLDocumentNode::GetAttribute(const LibMCEnv_uint64 nIndex)
{
    return new CXMLDocumentAttribute(m_pXMLDocumentNode->GetAttribute (nIndex));
}


bool CXMLDocumentNode::HasAttribute(const std::string& sNameSpace, const std::string& sName)
{
    auto pNameSpace = m_pXMLDocument->FindNamespace(sNameSpace, true);
    return m_pXMLDocumentNode->HasAttribute(pNameSpace.get(), sName);
}

IXMLDocumentAttribute* CXMLDocumentNode::FindAttribute(const std::string& sNameSpace, const std::string& sName, const bool bMustExist)
{
    auto pNameSpace = m_pXMLDocument->FindNamespace(sNameSpace, true);
    auto pAttribute = m_pXMLDocumentNode->FindAttribute(pNameSpace.get(), sName, bMustExist);
    if (pAttribute.get() != nullptr)
        return new CXMLDocumentAttribute (pAttribute);

    return nullptr;
}

void CXMLDocumentNode::RemoveAttribute(const std::string& sNameSpace, const std::string& sName)
{
    auto pNameSpace = m_pXMLDocument->FindNamespace(sNameSpace, true);
    m_pXMLDocumentNode->RemoveAttribute(pNameSpace.get(), sName);
}

void CXMLDocumentNode::RemoveAttributeByIndex(const LibMCEnv_uint64 nIndex)
{
    m_pXMLDocumentNode->RemoveAttributeByIndex(nIndex);
}

void CXMLDocumentNode::AddAttribute(const std::string& sNameSpace, const std::string& sName, const std::string& sValue)
{
    auto pNameSpace = m_pXMLDocument->FindNamespace(sNameSpace, true);
    m_pXMLDocumentNode->AddAttribute(pNameSpace, sName, sValue);
}

void CXMLDocumentNode::AddIntegerAttribute(const std::string& sNameSpace, const std::string& sName, const LibMCEnv_int64 nValue)
{
    auto pNameSpace = m_pXMLDocument->FindNamespace(sNameSpace, true);
    m_pXMLDocumentNode->AddAttribute(pNameSpace, sName, std::to_string (nValue));
}

void CXMLDocumentNode::AddDoubleAttribute(const std::string& sNameSpace, const std::string& sName, const LibMCEnv_double dValue)
{
    auto pNameSpace = m_pXMLDocument->FindNamespace(sNameSpace, true);
    m_pXMLDocumentNode->AddAttribute(pNameSpace, sName, std::to_string(dValue));
}

void CXMLDocumentNode::AddBoolAttribute(const std::string& sNameSpace, const std::string& sName, const bool bValue)
{
    auto pNameSpace = m_pXMLDocument->FindNamespace(sNameSpace, true);
    if (bValue)
        m_pXMLDocumentNode->AddAttribute(pNameSpace, sName, "true");
    else
        m_pXMLDocumentNode->AddAttribute(pNameSpace, sName, "false");
}

IXMLDocumentNodes* CXMLDocumentNode::GetChildren()
{
    auto resultNodes = std::make_unique<CXMLDocumentNodes>(m_pXMLDocument);

    auto children = m_pXMLDocumentNode->getChildren();

    for (auto child : children)
        resultNodes->addNode(child);

    return resultNodes.release();
}

LibMCEnv_uint64 CXMLDocumentNode::CountChildrenByName(const std::string& sNameSpace, const std::string& sName)
{
    uint64_t nCount = 0;
    auto children = m_pXMLDocumentNode->getChildren();

    for (auto child : children) {
        if (child->compareName(sNameSpace, sName))
            nCount++;
    }

    return nCount;

}

IXMLDocumentNodes* CXMLDocumentNode::GetChildrenByName(const std::string& sNameSpace, const std::string& sName)
{
    auto resultNodes = std::make_unique<CXMLDocumentNodes>(m_pXMLDocument);

    auto children = m_pXMLDocumentNode->getChildren();

    for (auto child : children) {
        if (child->compareName(sNameSpace, sName))
            resultNodes->addNode(child);
    }

    return resultNodes.release();
}

bool CXMLDocumentNode::HasChild(const std::string& sNameSpace, const std::string& sName)
{
    auto pNameSpace = m_pXMLDocument->FindNamespace(sNameSpace, true);
    return m_pXMLDocumentNode->HasChild(pNameSpace.get(), sName);
}

bool CXMLDocumentNode::HasUniqueChild(const std::string& sNameSpace, const std::string& sName)
{
    auto pNameSpace = m_pXMLDocument->FindNamespace(sNameSpace, true);
    return m_pXMLDocumentNode->HasUniqueChild(pNameSpace.get(), sName);
}

IXMLDocumentNode* CXMLDocumentNode::FindChild(const std::string& sNameSpace, const std::string& sName, const bool bMustExist)
{
    auto pNameSpace = m_pXMLDocument->FindNamespace(sNameSpace, true);
    auto pChild = m_pXMLDocumentNode->FindChild(pNameSpace.get(), sName, bMustExist);

    if (pChild != nullptr)
        return new CXMLDocumentNode(m_pXMLDocument, pChild);

    return nullptr;    
}

IXMLDocumentNode* CXMLDocumentNode::AddChild(const std::string& sNameSpace, const std::string& sName)
{
    auto pNameSpace = m_pXMLDocument->FindNamespace(sNameSpace, true);
    auto pChild = m_pXMLDocumentNode->AddChild(pNameSpace, sName);
    return new CXMLDocumentNode(m_pXMLDocument, pChild);
}

void CXMLDocumentNode::RemoveChild(IXMLDocumentNode* pChildInstance)
{
    throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CXMLDocumentNode::RemoveChildrenWithName(const std::string& sNameSpace, const std::string& sName)
{
    auto pNameSpace = m_pXMLDocument->FindNamespace(sNameSpace, true);
    m_pXMLDocumentNode->RemoveChildrenWithName(pNameSpace.get(), sName);
}

void CXMLDocumentNode::Remove()
{
    throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}


