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


Abstract: This is a stub class definition of CMachineConfigurationHandler

*/

#include "libmcenv_machineconfigurationhandler.hpp"
#include "libmcenv_machineconfigurationtype.hpp"
#include "libmcenv_interfaceexception.hpp"

// Include custom headers here.
#include "libmcenv_machineconfigurationtypeiterator.hpp"
#include "common_utils.hpp"

using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CMachineConfigurationHandler 
**************************************************************************************************************************/
CMachineConfigurationHandler::CMachineConfigurationHandler(LibMCData::PDataModel pDataModel, AMC::PStateMachineData pStateMachineData)
    : m_pDataModel (pDataModel), m_pStateMachineData (pStateMachineData)
{
    if (pDataModel.get () == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
}

CMachineConfigurationHandler::~CMachineConfigurationHandler()
{

}

IMachineConfigurationType * CMachineConfigurationHandler::RegisterMachineConfigurationType(const std::string & sSchemaType, const std::string & sName)
{
    auto pConfigurationType = m_pDataModel->FindConfigurationTypeBySchema(sSchemaType);
    if (pConfigurationType.get() != nullptr) {
		// Check if the name is equal
		if (pConfigurationType->GetName() == sName) {
			// Already registered
			return new CMachineConfigurationType(pConfigurationType, m_pStateMachineData);
		}
		else {
			// Schema type already registered, but with a different name
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_MACHINECONFIGURATIONSCHEMATYPEALREADYREGISTERED, "Schema type = " + sSchemaType + "is already registered, but with a different name = " + pConfigurationType->GetName());
		}
    }
    else {
		pConfigurationType = m_pDataModel->CreateConfigurationType(sSchemaType, sName);
    }

    return new CMachineConfigurationType(pConfigurationType, m_pStateMachineData);		
}

bool CMachineConfigurationHandler::HasMachineConfigurationType(const std::string & sSchemaType)
{
    if (sSchemaType.empty())
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM, "Schema type is empty.");

    auto pConfigurationType = m_pDataModel->FindConfigurationTypeBySchema(sSchemaType);

    return (pConfigurationType.get() != nullptr);
}

IMachineConfigurationTypeIterator* CMachineConfigurationHandler::ListRegisteredTypes()
{
	auto pIterator = m_pDataModel->ListRegisteredConfigurationTypes();

	if (pIterator.get() == nullptr)
		return nullptr;

    return new CMachineConfigurationTypeIterator(pIterator, m_pStateMachineData);
}

IMachineConfigurationType* CMachineConfigurationHandler::FindConfigurationTypeByUUID(const std::string& sTypeUUID)
{
    if (sTypeUUID.empty())
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM, "Configuration Type UUID is empty.");

    std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sTypeUUID);

    auto pConfigurationType = m_pDataModel->FindConfigurationTypeByUUID(sNormalizedUUID);
    if (pConfigurationType.get() == nullptr)
        return nullptr;

    return new CMachineConfigurationType(pConfigurationType, m_pStateMachineData);
}

IMachineConfigurationType* CMachineConfigurationHandler::FindConfigurationTypeBySchema(const std::string& sSchemaType)
{
    auto pConfigurationType = m_pDataModel->FindConfigurationTypeBySchema(sSchemaType);
    if (pConfigurationType.get() == nullptr)
        return nullptr;

    return new CMachineConfigurationType(pConfigurationType, m_pStateMachineData);
}