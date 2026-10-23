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


#ifndef __LIBMCDATA_MACHINECONFIGURATIONVERSION
#define __LIBMCDATA_MACHINECONFIGURATIONVERSION

#include "libmcdata_interfaces.hpp"

// Parent classes
#include "libmcdata_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "amcdata_sqlhandler.hpp"

namespace LibMCData {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CMachineConfigurationVersion 
**************************************************************************************************************************/

class CMachineConfigurationVersion : public virtual IMachineConfigurationVersion, public virtual CBase {
private:

	/**
	* Put private members here.
	*/

	AMCData::PSQLHandler m_pSQLHandler;

	std::string	m_sUUID;
	std::string	m_sXSDUUID;
	LibMCData_uint32 m_nConfigurationVersionNumber;
	std::string m_sParentUUID;
	std::string	m_sConfigurationXML;
	std::string	m_sUserUUID;
	std::string	m_sTimestampUTC;
	std::string	m_sName;

public:


	CMachineConfigurationVersion(AMCData::PSQLHandler pSQLHandler, const std::string& sUUID, const std::string& sXSDUUID, LibMCData_uint32 nConfigurationVersionNumber, const std::string& sParentUUID, const std::string& sConfigurationXML, const std::string& sUserUUID, const std::string& sTimestampUTC, const std::string& sName);

	// Trims the name and checks that it is not empty and at most 256 characters long.
	static std::string checkVersionName(const std::string& sName);

	virtual ~CMachineConfigurationVersion();

	static CMachineConfigurationVersion* makeFrom(CMachineConfigurationVersion* pMachineConfigurationVersion);

	static std::shared_ptr<CMachineConfigurationVersion> makeSharedFrom(CMachineConfigurationVersion* pMachineConfigurationVersion);

	static CMachineConfigurationVersion* createDefaultConfigurationForXSD( AMCData::PSQLHandler pSQLHandler, const std::string& sXSDUUID, const std::string& sConfigurationXML,	const std::string& sTimestampUTC);

	static LibMCData_uint32 getLatestConfigurationNumericVersionForXSD(AMCData::PSQLHandler pSQLHandler, const std::string& sXSDUUID);

	static CMachineConfigurationVersion* getLatestConfigurationVersionForXSD(AMCData::PSQLHandler pSQLHandler, const std::string& sXSDUUID);

	static CMachineConfigurationVersion* findConfigurationVersionByUUID(AMCData::PSQLHandler pSQLHandler, const std::string& sVersionUUID);
	
	static IMachineConfigurationVersionIterator* listConfigurationVersionsForXSD(AMCData::PSQLHandler pSQLHandler, const std::string& sXSDUUID);

	AMCData::PSQLHandler GetSQLHandler();

	std::string GetVersionUUID() override;

	std::string GetXSDUUID() override;

	LibMCData_uint32 GetNumericVersion() override;

	std::string GetParentUUID() override;

	std::string GetName() override;

	std::string GetConfigurationXMLString() override;

	std::string GetUserUUID() override;

	std::string GetTimestamp() override;

	IMachineConfigurationVersion* CreateNewVersion(const std::string & sXMLString, const std::string & sUserUUID, const std::string & sName) override;

	IMachineConfigurationVersion* MigrateToNewXSD(IMachineConfigurationXSD* pNewXSD, const std::string & sXMLString, const std::string & sUserUUID, const std::string & sName) override;

};

} // namespace Impl
} // namespace LibMCData

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDATA_MACHINECONFIGURATIONVERSION
