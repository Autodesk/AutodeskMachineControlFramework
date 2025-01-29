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


#ifndef __AMC_XMLDOCUMENTNODE
#define __AMC_XMLDOCUMENTNODE

#include <string>
#include <memory>
#include <vector>
#include <map>

#include "amc_xmldocumentattribute.hpp"

namespace pugi {
	class xml_document;
	class xml_node;
}

namespace AMC {

	class CXMLDocumentInstance;
	class CXMLDocumentNodeInstance;
	class CXMLDocumentNameSpace;

	typedef std::shared_ptr<CXMLDocumentNodeInstance> PXMLDocumentNodeInstance;

	class CXMLDocumentNodeInstance {
	private:

		CXMLDocumentInstance* m_pDocument;
		CXMLDocumentNodeInstance* m_pParentNode;
		std::string m_sNodeName;
		std::string m_sTextContent;
		PXMLDocumentNameSpace m_pNameSpace;

		std::vector<PXMLDocumentAttributeInstance> m_Attributes;
		std::map<std::pair <CXMLDocumentNameSpace *, std::string>, PXMLDocumentAttributeInstance> m_AttributeMap;

		std::vector<PXMLDocumentNodeInstance> m_Children;
		std::map<std::pair <CXMLDocumentNameSpace*, std::string>, PXMLDocumentNodeInstance> m_ChildMap;
		std::map<std::pair <CXMLDocumentNameSpace*, std::string>, uint64_t> m_ChildMapCounter;

		void addChildEx(PXMLDocumentNodeInstance pNode);

		void splitNameSpaceName(const std::string & sPrefixedName, std::string & sUnprefixedName, std::string & sNameSpacePrefix);

	public:

		CXMLDocumentNodeInstance(CXMLDocumentInstance* pDocument, CXMLDocumentNodeInstance * pParentNode, PXMLDocumentNameSpace pNameSpace, const std::string& sNodeName);
		
		virtual ~CXMLDocumentNodeInstance();

		void extractFromPugiNode (pugi::xml_document * pXMLDocument, pugi::xml_node * pXMLNode, bool bIsRoot);

		void storeToPugiNode(pugi::xml_document* pXMLDocument, pugi::xml_node* pXMLNode);

		CXMLDocumentInstance* getDocument();

		std::vector<PXMLDocumentNodeInstance> getChildren ();

		static bool checkXMLNodeName(const std::string & sNodeName);

		std::string GetName();

		std::string getPrefixedName();

		bool compareName(const std::string& sNameSpace, const std::string& sName);

		PXMLDocumentNameSpace GetNameSpace();

		std::string GetTextContent();

		void SetTextContent(const std::string & sTextContent);

		uint64_t GetAttributeCount();

		bool HasAttribute(CXMLDocumentNameSpace * pNameSpace, const std::string& sName);

		void RemoveAttribute(CXMLDocumentNameSpace* pNameSpace, const std::string& sName);

		void RemoveAttributeByIndex(uint64_t nIndex);

		PXMLDocumentAttributeInstance GetAttribute(uint64_t nIndex);

		PXMLDocumentAttributeInstance FindAttribute(CXMLDocumentNameSpace* pNameSpace, const std::string& sName, const bool bMustExist);

		PXMLDocumentAttributeInstance AddAttribute(PXMLDocumentNameSpace pNameSpace, const std::string& sName, const std::string& sValue);

		uint64_t CountChildrenByName(CXMLDocumentNameSpace* pNameSpace, const std::string& sName);

		bool HasChild(CXMLDocumentNameSpace* pNameSpace, const std::string& sName);

		bool HasUniqueChild(CXMLDocumentNameSpace* pNameSpace, const std::string& sName);

		PXMLDocumentNodeInstance FindChild(CXMLDocumentNameSpace* pNameSpace, const std::string& sName, const bool bMustExist);

		PXMLDocumentNodeInstance AddChild(PXMLDocumentNameSpace pNameSpace, const std::string& sName);

		void RemoveChild(CXMLDocumentNodeInstance * pChildInstance);

		void RemoveChildrenWithName(CXMLDocumentNameSpace* pNameSpace, const std::string& sName);

		void CopyFrom(CXMLDocumentNodeInstance* pFromInstance);

		static void splitNameSpaceName(const std::string& sPrefixedName, std::string& sUnprefixedName, std::string& sNameSpacePrefix);
	};

	


	
}


#endif //__AMC_XMLDOCUMENTNODE

