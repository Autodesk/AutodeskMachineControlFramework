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


Abstract: This is the class declaration of CXMLDocumentNode

*/


#ifndef __LIBMCENV_XMLDOCUMENTNODE
#define __LIBMCENV_XMLDOCUMENTNODE

#include "libmcenv_interfaces.hpp"

// Parent classes
#include "libmcenv_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "amc_xmldocument.hpp"
#include "amc_xmldocumentnode.hpp"

namespace LibMCEnv {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CXMLDocumentNode 
**************************************************************************************************************************/

class CXMLDocumentNode : public virtual IXMLDocumentNode, public virtual CBase {
private:
	AMC::PXMLDocumentNodeInstance m_pXMLDocumentNode;
	AMC::PXMLDocumentInstance m_pXMLDocument;


public:
	CXMLDocumentNode(AMC::PXMLDocumentInstance pXMLDocument, AMC::PXMLDocumentNodeInstance pXMLDocumentNode);

	~CXMLDocumentNode();

	std::string GetName() override;

	std::string GetNameSpace() override;

	std::string GetTextContent() override;

	void SetTextContent(const std::string& sTextContent) override;

	LibMCEnv_uint64 GetAttributeCount() override;

	IXMLDocumentAttribute* GetAttribute(const LibMCEnv_uint64 nIndex) override;

	bool HasAttribute(const std::string& sNameSpace, const std::string& sName) override;

	IXMLDocumentAttribute* FindAttribute(const std::string& sNameSpace, const std::string& sName, const bool bMustExist) override;
	
	std::string GetAttributeValue(const std::string& sNameSpace, const std::string& sName) override;

	LibMCEnv_int64 GetAttributeIntegerValue(const std::string& sNameSpace, const std::string& sName, const LibMCEnv_int64 nMinValue, const LibMCEnv_int64 nMaxValue) override;

	LibMCEnv_double GetAttributeDoubleValue(const std::string& sNameSpace, const std::string& sName, const LibMCEnv_double dMinValue, const LibMCEnv_double dMaxValue) override;

	bool GetAttributeBoolValue(const std::string& sNameSpace, const std::string& sName) override;

	std::string GetAttributeUUIDValue(const std::string& sNameSpace, const std::string& sName) override;

	std::string GetAttributeValueDef(const std::string& sNameSpace, const std::string& sName, const std::string& sDefaultValue) override;

	LibMCEnv_int64 GetAttributeIntegerValueDef(const std::string& sNameSpace, const std::string& sName, const LibMCEnv_int64 nMinValue, const LibMCEnv_int64 nMaxValue, const LibMCEnv_int64 nDefaultValue) override;

	LibMCEnv_double GetAttributeDoubleValueDef(const std::string& sNameSpace, const std::string& sName, const LibMCEnv_double dMinValue, const LibMCEnv_double dMaxValue, const LibMCEnv_double dDefaultValue) override;

	bool GetAttributeBoolValueDef(const std::string& sNameSpace, const std::string& sName, const bool bDefaultValue) override;

	std::string GetAttributeUUIDValueDef(const std::string& sNameSpace, const std::string& sName, const std::string& sDefaultValue) override;

	void RemoveAttribute(const std::string& sNameSpace, const std::string& sName) override;

	void RemoveAttributeByIndex(const LibMCEnv_uint64 nIndex) override;

	void AddAttribute(const std::string& sNameSpace, const std::string& sName, const std::string& sValue) override;

	void AddIntegerAttribute(const std::string& sNameSpace, const std::string& sName, const LibMCEnv_int64 nValue) override;

	void AddDoubleAttribute(const std::string& sNameSpace, const std::string& sName, const LibMCEnv_double dValue) override;

	void AddBoolAttribute(const std::string& sNameSpace, const std::string& sName, const bool bValue) override;

	IXMLDocumentNodes* GetChildren() override;
	
	LibMCEnv_uint64 CountChildrenByName(const std::string& sNameSpace, const std::string& sName) override;

	IXMLDocumentNodes* GetChildrenByName(const std::string& sNameSpace, const std::string& sName) override;

	bool HasChild(const std::string& sNameSpace, const std::string& sName) override;

	bool HasUniqueChild(const std::string& sNameSpace, const std::string& sName) override;

	IXMLDocumentNode* FindChild(const std::string& sNameSpace, const std::string& sName, const bool bMustExist) override;
	
	IXMLDocumentNode* AddChild(const std::string& sNameSpace, const std::string& sName) override;

	IXMLDocumentNode* AddChildText(const std::string& sNameSpace, const std::string& sName, const std::string& sTextContent) override;

	void RemoveChild(IXMLDocumentNode* pChildInstance) override;

	void RemoveChildrenWithName(const std::string& sNameSpace, const std::string& sName) override;

	void Remove() override;

	void CopyFrom(IXMLDocumentNode* pOtherNode) override;

};

} // namespace Impl
} // namespace LibMCEnv

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCENV_XMLDOCUMENTNODE
