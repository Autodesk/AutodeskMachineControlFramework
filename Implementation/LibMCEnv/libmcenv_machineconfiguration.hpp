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


Abstract: This is the class declaration of CMachineConfiguration

*/


#ifndef __LIBMCENV_MACHINECONFIGURATION
#define __LIBMCENV_MACHINECONFIGURATION

#include "libmcenv_interfaces.hpp"

// Parent classes
#include "libmcenv_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "libmcdata_dynamic.hpp"
#include "amc_xmldocument.hpp"

namespace LibMCEnv {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CMachineConfiguration 
**************************************************************************************************************************/

class CMachineConfiguration : public virtual IMachineConfiguration, public virtual CBase {
private:

	// Owning configuration type, used to activate newly committed versions. May be null.
	LibMCData::PMachineConfigurationType m_pDataType;

	// The configuration version this working copy is based on. Rebased after each commit.
	LibMCData::PMachineConfigurationVersion m_pBaseVersion;

	// In-memory, mutable copy of the configuration XML.
	AMC::PXMLDocumentInstance m_pXMLDocument;

	// Cached default namespace of the document.
	std::string m_sDefaultNamespace;

	bool m_bDirty;

	// Resolves a slash-separated path to a leaf node. Returns null if a segment is missing and bCreate is false.
	AMC::PXMLDocumentNodeInstance resolvePathNode(const std::string& sPath, bool bCreate);

public:

	CMachineConfiguration(LibMCData::PMachineConfigurationType pDataType, LibMCData::PMachineConfigurationVersion pBaseVersion);

	virtual ~CMachineConfiguration();

	std::string GetVersionUUID() override;

	LibMCEnv_uint32 GetNumericVersion() override;

	std::string GetXSDUUID() override;

	IXMLDocument * GetXMLDocument() override;

	bool HasParameter(const std::string & sPath) override;

	std::string GetStringParameter(const std::string & sPath) override;

	LibMCEnv_int64 GetIntegerParameter(const std::string & sPath, const LibMCEnv_int64 nMinValue, const LibMCEnv_int64 nMaxValue) override;

	LibMCEnv_double GetDoubleParameter(const std::string & sPath, const LibMCEnv_double dMinValue, const LibMCEnv_double dMaxValue) override;

	bool GetBoolParameter(const std::string & sPath) override;

	std::string GetStringParameterDef(const std::string & sPath, const std::string & sDefaultValue) override;

	LibMCEnv_int64 GetIntegerParameterDef(const std::string & sPath, const LibMCEnv_int64 nMinValue, const LibMCEnv_int64 nMaxValue, const LibMCEnv_int64 nDefaultValue) override;

	LibMCEnv_double GetDoubleParameterDef(const std::string & sPath, const LibMCEnv_double dMinValue, const LibMCEnv_double dMaxValue, const LibMCEnv_double dDefaultValue) override;

	bool GetBoolParameterDef(const std::string & sPath, const bool bDefaultValue) override;

	void SetStringParameter(const std::string & sPath, const std::string & sValue) override;

	void SetIntegerParameter(const std::string & sPath, const LibMCEnv_int64 nValue) override;

	void SetDoubleParameter(const std::string & sPath, const LibMCEnv_double dValue) override;

	void SetBoolParameter(const std::string & sPath, const bool bValue) override;

	bool HasChanges() override;

	IMachineConfigurationVersion * Commit(const std::string & sUserUUID) override;

	IMachineConfigurationVersion * CommitAndActivate(const std::string & sUserUUID) override;

};

} // namespace Impl
} // namespace LibMCEnv

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCENV_MACHINECONFIGURATION
