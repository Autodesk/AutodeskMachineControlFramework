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


Abstract: This is the class declaration of CMachineConfigurationVersion

*/


#ifndef __LIBMCENV_MACHINECONFIGURATIONVERSION
#define __LIBMCENV_MACHINECONFIGURATIONVERSION

#include "libmcenv_interfaces.hpp"

// Parent classes
#include "libmcenv_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "libmcdata_dynamic.hpp"

namespace LibMCEnv {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CMachineConfigurationVersion 
**************************************************************************************************************************/

class CMachineConfigurationVersion : public virtual IMachineConfigurationVersion, public virtual CBase {
private:

	/**
	* Put private members here.
	*/

	LibMCData::PMachineConfigurationVersion m_pMachineConfigurationVersion;

protected:

	/**
	* Put protected members here.
	*/

public:

	/**
	* Put additional public members here. They will not be visible in the external API.
	*/


	/**
	* Public member functions to implement.
	*/

	CMachineConfigurationVersion(LibMCData::PMachineConfigurationVersion pMachineConfigurationVersion);

	virtual ~CMachineConfigurationVersion();

	virtual std::string GetVersionUUID() override;

	virtual std::string GetXSDUUID() override;

	virtual LibMCEnv_uint32 GetNumericVersion() override;

	virtual std::string GetParentUUID() override;

	virtual std::string GetConfigurationXMLString() override;

	virtual IXMLDocument* GetConfigurationXML() override;

	virtual std::string GetUserUUID() override;

	virtual std::string GetTimestamp() override;

	virtual IMachineConfigurationVersion* CreateNewVersion(const std::string& sXMLString, const std::string& sUserUUID) override;

	virtual IMachineConfigurationVersion* MigrateToNewXSD(IMachineConfigurationXSD* pNewXSD, const std::string& sXMLString, const std::string& sUserUUID) override;

};

} // namespace Impl
} // namespace LibMCEnv

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCENV_MACHINECONFIGURATIONVERSION
