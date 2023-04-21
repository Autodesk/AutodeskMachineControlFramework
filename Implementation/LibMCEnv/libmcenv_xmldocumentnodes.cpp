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


Abstract: This is a stub class definition of CXMLDocumentNodes

*/

#include "libmcenv_xmldocumentnodes.hpp"
#include "libmcenv_xmldocumentnode.hpp"
#include "libmcenv_interfaceexception.hpp"

// Include custom headers here.
#include "libmc_exceptiontypes.hpp"

using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CXMLDocumentNodes 
**************************************************************************************************************************/


CXMLDocumentNodes::CXMLDocumentNodes(AMC::PXMLDocumentInstance pDocument)
    : m_pDocument (pDocument)
{
    if (pDocument.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
}

CXMLDocumentNodes::~CXMLDocumentNodes()
{

}

AMC::PXMLDocumentInstance CXMLDocumentNodes::getDocument()
{
    return m_pDocument;
}


void CXMLDocumentNodes::addNode(AMC::PXMLDocumentNodeInstance pNode)
{
    if (pNode.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
    if (pNode->getDocument () != m_pDocument.get())
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_XMLDOCUMENTMISMATCH);

    auto mapKey = std::make_pair(pNode->GetNameSpace().get(), pNode->GetName ());

    m_Nodes.push_back(pNode);

    m_NodeMap.insert(std::make_pair(mapKey, pNode));

    auto iChildIter = m_NodeMapCounter.find(mapKey);
    if (iChildIter != m_NodeMapCounter.end()) {
        iChildIter->second++;
    }
    else
    {
        m_NodeMapCounter.insert(std::make_pair(mapKey, 1));
    }


}

LibMCEnv_uint64 CXMLDocumentNodes::GetNodeCount()
{
    return m_Nodes.size();
}

IXMLDocumentNode * CXMLDocumentNodes::GetNode(const LibMCEnv_uint64 nIndex)
{
	if (nIndex >= m_Nodes.size ())
        throw ELibMCEnvInterfaceException(LIBMC_ERROR_INVALIDXMLNODEINDEX);

    return new CXMLDocumentNode(m_pDocument, m_Nodes.at(nIndex));

}

LibMCEnv_uint64 CXMLDocumentNodes::CountNodesByName(const std::string& sNameSpace, const std::string& sName)
{
    auto pNameSpace = m_pDocument->FindNamespace(sNameSpace, true);

    uint64_t nCount = 0;
    for (auto node : m_Nodes) {
        if ((node->GetNameSpace().get() == pNameSpace.get()) && (node->GetName() == sName))
            nCount++;
    }

    return nCount;

}

IXMLDocumentNodes* CXMLDocumentNodes::GetNodesByName(const std::string& sNameSpace, const std::string& sName)
{
    auto pNameSpace = m_pDocument->FindNamespace(sNameSpace, true);

    auto returnNodes = std::make_unique<CXMLDocumentNodes> (m_pDocument);

    for (auto node : m_Nodes) {
        if ((node->GetNameSpace().get() == pNameSpace.get()) && (node->GetName() == sName))
            returnNodes->addNode(node);
    }

    return returnNodes.release();

}

bool CXMLDocumentNodes::HasNode(const std::string& sNameSpace, const std::string& sName)
{
    auto pNameSpace = m_pDocument->FindNamespace(sNameSpace, true);

    auto iIter = m_NodeMap.find(std::make_pair(pNameSpace.get(), sName));
    return (iIter != m_NodeMap.end());
}

bool CXMLDocumentNodes::HasUniqueNode(const std::string& sNameSpace, const std::string& sName)
{
    auto pNameSpace = m_pDocument->FindNamespace(sNameSpace, true);

    auto iIter = m_NodeMapCounter.find(std::make_pair(pNameSpace.get(), sName));
    if (iIter != m_NodeMapCounter.end()) {
        return (iIter->second == 1);
    }
    else {
        return false;
    }


}

IXMLDocumentNode* CXMLDocumentNodes::FindNode(const std::string& sNameSpace, const std::string& sName, const bool bMustExist)
{
    auto pNameSpace = m_pDocument->FindNamespace(sNameSpace, true);

    auto iIter = m_NodeMapCounter.find(std::make_pair(pNameSpace.get(), sName));
    if (iIter != m_NodeMapCounter.end()) {
        if (iIter->second == 1)
        {
            auto iChildIter = m_NodeMap.find(std::make_pair(pNameSpace.get(), sName));
            if (iChildIter == m_NodeMap.end())
                throw ELibMCCustomException(LIBMC_ERROR_INTERNALNODEERROR, sName);

            return new CXMLDocumentNode (m_pDocument, iChildIter->second);

        }
    }

    if (bMustExist)
        throw ELibMCCustomException(LIBMC_ERROR_XMLNODENOTFOUNDINLIST, sName);

    return nullptr;

}


