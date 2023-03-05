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
#include "PugiXML/pugixml.hpp"

using namespace AMC;

CXMLDocumentNodeInstance::CXMLDocumentNodeInstance(CXMLDocumentInstance* pDocument, CXMLDocumentNodeInstance* pParentNode, PXMLDocumentNameSpace pNameSpace, const std::string& sNodeName)
	: m_pDocument (pDocument), m_sNodeName(sNodeName), m_pParentNode (pParentNode), m_pNameSpace (pNameSpace)
{
	if (pDocument == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
	if (!checkXMLNodeName (sNodeName)) 
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDXMLNODENAME, sNodeName);
	if (pNameSpace.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

}

CXMLDocumentNodeInstance::~CXMLDocumentNodeInstance()
{
}

bool CXMLDocumentNodeInstance::checkXMLNodeName(const std::string& sNodeName)
{
	return true;
}

void CXMLDocumentNodeInstance::extractFromPugiNode(pugi::xml_document* pXMLDocument, pugi::xml_node* pXMLNode)
{

}

void CXMLDocumentNodeInstance::storeToPugiNode(pugi::xml_document* pXMLDocument, pugi::xml_node* pXMLNode)
{

}

CXMLDocumentInstance* CXMLDocumentNodeInstance::getDocument()
{
	return m_pDocument;
}


std::vector<PXMLDocumentNodeInstance> CXMLDocumentNodeInstance::getChildren()
{
	return m_Children;
}


std::string CXMLDocumentNodeInstance::GetName()
{
	return m_sNodeName;
}

PXMLDocumentNameSpace CXMLDocumentNodeInstance::GetNameSpace()
{
	return m_pNameSpace;
}

uint64_t CXMLDocumentNodeInstance::GetAttributeCount()
{	
	return m_Attributes.size ();
}

bool CXMLDocumentNodeInstance::HasAttribute(CXMLDocumentNameSpace* pNameSpace, const std::string& sName)
{
	if (pNameSpace == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	auto iIter = m_AttributeMap.find(std::make_pair(pNameSpace, sName));
	return iIter != m_AttributeMap.end();
}

void CXMLDocumentNodeInstance::RemoveAttribute(CXMLDocumentNameSpace* pNameSpace, const std::string& sName)
{
	if (pNameSpace == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	auto iIter = m_AttributeMap.find(std::make_pair(pNameSpace, sName));
	if (iIter != m_AttributeMap.end()) {
		std::remove_if(m_Attributes.begin(), m_Attributes.end(), [this, pNameSpace, sName](PXMLDocumentAttributeInstance pAttribute) {
			return (pAttribute->getNameSpace().get() == pNameSpace) && (pAttribute->getAttributeName () == sName);
		});

		m_AttributeMap.erase(std::make_pair(pNameSpace, sName));
	}

}

void CXMLDocumentNodeInstance::RemoveAttributeByIndex(uint64_t nIndex)
{
	if (nIndex >= m_Attributes.size ())
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDATTRIBUTEINDEX, std::to_string(nIndex));

	auto pAttribute = m_Attributes.at(nIndex);
	m_Attributes.erase(m_Attributes.begin () + nIndex);
	m_AttributeMap.erase(std::make_pair(pAttribute->getNameSpace ().get(), pAttribute->getAttributeName ()));

}


PXMLDocumentAttributeInstance CXMLDocumentNodeInstance::GetAttribute(uint64_t nIndex)
{
	if (nIndex >= m_Attributes.size())
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDATTRIBUTEINDEX, std::to_string(nIndex));

	return m_Attributes.at(nIndex);

}

PXMLDocumentAttributeInstance CXMLDocumentNodeInstance::FindAttribute(CXMLDocumentNameSpace* pNameSpace, const std::string& sName, const bool bMustExist)
{
	if (pNameSpace == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	auto iIter = m_AttributeMap.find(std::make_pair(pNameSpace, sName));
	if (iIter != m_AttributeMap.end()) {
		return iIter->second;
	}
	else {
		if (bMustExist) 
			throw ELibMCCustomException(LIBMC_ERROR_COULDNOTFINDATTRIBUTE, sName);

		return nullptr;

	}

}

PXMLDocumentAttributeInstance CXMLDocumentNodeInstance::AddAttribute(PXMLDocumentNameSpace pNameSpace, const std::string& sName, const std::string& sValue)
{
	if (pNameSpace == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	auto iIter = m_AttributeMap.find(std::make_pair(pNameSpace.get(), sName));
	if (iIter != m_AttributeMap.end()) 
		throw ELibMCCustomException(LIBMC_ERROR_DUPLICATEATTRIBUTE, sName);

	auto pAttribute = std::make_shared<CXMLDocumentAttributeInstance>(m_pDocument, this, pNameSpace, sName);
	pAttribute->setValue(sValue);

	return pAttribute;
}

uint64_t CXMLDocumentNodeInstance::CountChildrenByName(CXMLDocumentNameSpace* pNameSpace, const std::string& sName)
{
	if (pNameSpace == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	uint64_t nCount = 0;
	for (auto child : m_Children) {
		if ((child->GetNameSpace ().get() == pNameSpace) && (child->GetName() == sName))
			nCount++;
	}

	return nCount;
}

bool CXMLDocumentNodeInstance::HasChild(CXMLDocumentNameSpace* pNameSpace, const std::string& sName)
{
	if (pNameSpace == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	auto iIter = m_ChildMapCounter.find(std::make_pair (pNameSpace, sName));
	return (iIter != m_ChildMapCounter.end());
}

bool CXMLDocumentNodeInstance::HasUniqueChild(CXMLDocumentNameSpace* pNameSpace, const std::string& sName)
{
	if (pNameSpace == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	auto iIter = m_ChildMapCounter.find(std::make_pair(pNameSpace, sName));
	if (iIter != m_ChildMapCounter.end()) {
		return (iIter->second == 1);
	}
	else {
		return false;
	}

}

PXMLDocumentNodeInstance CXMLDocumentNodeInstance::FindChild(CXMLDocumentNameSpace* pNameSpace, const std::string& sName, const bool bMustExist)
{
	if (pNameSpace == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	auto iIter = m_ChildMapCounter.find(std::make_pair(pNameSpace, sName));
	if (iIter != m_ChildMapCounter.end()) {
		if (iIter->second == 1)
		{
			auto iChildIter = m_ChildMap.find(std::make_pair(pNameSpace, sName));
			if (iChildIter == m_ChildMap.end ())
				throw ELibMCCustomException(LIBMC_ERROR_INTERNALNODEERROR, sName);

			return iChildIter->second;

		}
	}

	if (bMustExist)
		throw ELibMCCustomException(LIBMC_ERROR_XMLNODECHILDNOTFOUND, sName);

	return nullptr;
}

PXMLDocumentNodeInstance CXMLDocumentNodeInstance::AddChild(PXMLDocumentNameSpace pNameSpace, const std::string& sName)
{
	if (pNameSpace.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	auto pNode = std::make_shared<CXMLDocumentNodeInstance>(m_pDocument, this, pNameSpace, sName);

	auto mapKey = std::make_pair(pNameSpace.get(), sName);

	m_Children.push_back(pNode);
	m_ChildMap.insert(std::make_pair (mapKey, pNode));

	auto iChildIter = m_ChildMapCounter.find(mapKey);
	if (iChildIter != m_ChildMapCounter.end()) {
		iChildIter->second++;
	}
	else
	{
		m_ChildMapCounter.insert(std::make_pair (mapKey, 1));
	}

	return pNode;
}

void CXMLDocumentNodeInstance::RemoveChild(CXMLDocumentNodeInstance* pChildInstance)
{
	throw ELibMCInterfaceException(LIBMC_ERROR_NOTIMPLEMENTED);
}

void CXMLDocumentNodeInstance::RemoveChildrenWithName(CXMLDocumentNameSpace* pNameSpace, const std::string& sName)
{
	throw ELibMCInterfaceException(LIBMC_ERROR_NOTIMPLEMENTED);
}

