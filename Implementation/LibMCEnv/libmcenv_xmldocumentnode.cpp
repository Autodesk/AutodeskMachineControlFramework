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
#include "libmcenv_interfaceexception.hpp"

// Include custom headers here.


using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CXMLDocumentNode 
**************************************************************************************************************************/

CXMLDocumentNode::CXMLDocumentNode(AMC::PXMLDocumentNodeInstance pXMLDocumentNode)
	: m_pXMLDocumentNode (pXMLDocumentNode)
{
	if (pXMLDocumentNode.get())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

}

CXMLDocumentNode::~CXMLDocumentNode()
{

}


std::string CXMLDocumentNode::GetName()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

std::string CXMLDocumentNode::GetNameSpace()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

LibMCEnv_uint64 CXMLDocumentNode::GetAttributeCount()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

IXMLDocumentAttribute * CXMLDocumentNode::GetAttribute(const LibMCEnv_uint64 nIndex)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

bool CXMLDocumentNode::HasAttribute(const std::string & sName)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

IXMLDocumentAttribute * CXMLDocumentNode::FindAttribute(const std::string & sName, const bool bMustExist)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CXMLDocumentNode::RemoveAttribute(const std::string & sName)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CXMLDocumentNode::AddAttribute(const std::string & sName, const std::string & sNameSpace, const std::string & sValue)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CXMLDocumentNode::AddIntegerAttribute(const std::string & sName, const std::string & sNameSpace, const LibMCEnv_int64 nValue)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CXMLDocumentNode::AddDoubleAttribute(const std::string & sName, const std::string & sNameSpace, const LibMCEnv_double dValue)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CXMLDocumentNode::AddBoolAttribute(const std::string & sName, const std::string & sNameSpace, const bool bValue)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

IXMLDocumentNodes * CXMLDocumentNode::GetChildren()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

LibMCEnv_uint64 CXMLDocumentNode::CountChildrenByName(const std::string & sName)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

IXMLDocumentNodes * CXMLDocumentNode::GetChildrenByName(const std::string & sName)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

bool CXMLDocumentNode::HasChild(const std::string & sName)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

bool CXMLDocumentNode::HasUniqueChild(const std::string & sName)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

IXMLDocumentNode * CXMLDocumentNode::FindChild(const std::string & sName, const bool bMustExist)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

IXMLDocumentNode * CXMLDocumentNode::AddChild(const std::string & sName, const std::string & sNameSpace)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CXMLDocumentNode::RemoveChild(IXMLDocumentNode* pChildInstance)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CXMLDocumentNode::RemoveChildrenWithName(const std::string & sName)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CXMLDocumentNode::Remove()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

