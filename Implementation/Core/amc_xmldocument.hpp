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


#ifndef __AMC_XMLDOCUMENT
#define __AMC_XMLDOCUMENT

#include <string>
#include <memory>
#include <map>
#include <vector>

#include "amc_xmldocumentnode.hpp"

namespace AMC {


	class CXMLDocumentInstance {
	private:

		PXMLDocumentNodeInstance m_pRootNodeInstance;
		PXMLDocumentNameSpace m_pDefaultNameSpace;

		std::vector<PXMLDocumentNameSpace> m_Namespaces;
		std::map<std::string, PXMLDocumentNameSpace> m_NamespaceMap;
		std::map<std::string, PXMLDocumentNameSpace> m_PrefixToNamespaceMap;

		PXMLDocumentNameSpace registerNamespaceEx(const std::string& sNamespace, const std::string & sPrefix);

		void extractPugiDocument(std::shared_ptr<pugi::xml_document> pXMLDocument);

	public:

		CXMLDocumentInstance();
		
		virtual ~CXMLDocumentInstance();

		void createEmptyDocument(const std::string& sRootNodeName, const std::string& sDefaultNamespace);

		void parseXMLString(const std::string & sXMLString);

		void parseXMLData(uint64_t nDataSize, const uint8_t * pData);

		PXMLDocumentNameSpace GetDefaultNamespace();

		uint64_t GetNamespaceCount();

		void GetNamespace(const uint64_t nIndex, std::string & sNamespace, std::string & sNamespacePrefix);

		bool HasNamespace(const std::string & sNamespace);

		PXMLDocumentNameSpace FindNamespace(const std::string& sNamespace, bool bMustExist);

		std::string GetNamespacePrefix(const std::string & sNamespace);

		void RegisterNamespace(const std::string & sNamespace, const std::string & sNamespacePrefix);

		PXMLDocumentNodeInstance GetRootNode();

		std::string SaveToString(const bool bAddLineBreaks);

	};

	typedef std::shared_ptr<CXMLDocumentInstance> PXMLDocumentInstance;
	
}


#endif //__AMC_XMLDOCUMENT

