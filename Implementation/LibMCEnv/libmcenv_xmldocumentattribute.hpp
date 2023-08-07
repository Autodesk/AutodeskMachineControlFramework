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


Abstract: This is the class declaration of CXMLDocumentAttribute

*/


#ifndef __LIBMCENV_XMLDOCUMENTATTRIBUTE
#define __LIBMCENV_XMLDOCUMENTATTRIBUTE

#include "libmcenv_interfaces.hpp"

// Parent classes
#include "libmcenv_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "amc_xmldocumentattribute.hpp"

namespace LibMCEnv {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CXMLDocumentAttribute 
**************************************************************************************************************************/

class CXMLDocumentAttribute : public virtual IXMLDocumentAttribute, public virtual CBase {
private:

	AMC::PXMLDocumentAttributeInstance m_pAttributeInstance;

public:

	CXMLDocumentAttribute(AMC::PXMLDocumentAttributeInstance pAttributeInstance);

	virtual ~CXMLDocumentAttribute();

	std::string GetNameSpace() override;

	std::string GetName() override;

	std::string GetValue() override;

	bool IsValidUUID() override;

	std::string GetUUIDValue() override;

	bool IsValidInteger(const LibMCEnv_int64 nMinValue, const LibMCEnv_int64 nMaxValue) override;

	LibMCEnv_int64 GetIntegerValue(const LibMCEnv_int64 nMinValue, const LibMCEnv_int64 nMaxValue) override;

	bool IsValidDouble(const LibMCEnv_double dMinValue, const LibMCEnv_double dMaxValue) override;

	LibMCEnv_double GetDoubleValue(const LibMCEnv_double dMinValue, const LibMCEnv_double dMaxValue) override;

	bool IsValidBool() override;

	bool GetBoolValue() override;

	void SetValue(const std::string & sValue) override;

	void SetUUIDValue(const std::string& sValue) override;

	void SetIntegerValue(const LibMCEnv_int64 nValue) override;

	void SetDoubleValue(const LibMCEnv_double dValue) override;

	void SetBoolValue(const bool bValue) override;

	void Remove() override;

};

} // namespace Impl
} // namespace LibMCEnv

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCENV_XMLDOCUMENTATTRIBUTE
