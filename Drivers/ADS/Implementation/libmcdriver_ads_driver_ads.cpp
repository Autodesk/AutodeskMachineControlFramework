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


Abstract: This is a stub class definition of CDriver_ADS

*/

#include "libmcdriver_ads_driver_ads.hpp"
#include "libmcdriver_ads_interfaceexception.hpp"
#include "pugixml.hpp"

using namespace LibMCDriver_ADS::Impl;

/*************************************************************************************************************************
 Class definition of CDriver_ADS 
**************************************************************************************************************************/
#define __STRINGIZE(x) #x
#define __STRINGIZE_VALUE_OF(x) __STRINGIZE(x)


CDriver_ADS::CDriver_ADS(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment)
	: m_bSimulationMode (false),
	m_sName(sName),
	m_pDriverEnvironment(pDriverEnvironment),
    m_nMajorVersion (0),
    m_nMinorVersion (0),
    m_nPatchVersion (0)
{

}

CDriver_ADS::~CDriver_ADS()
{

}

void CDriver_ADS::Configure(const std::string& sConfigurationString)
{
    m_pDriverEnvironment->LogMessage("Configuring ADS driver!");

    if (sConfigurationString.length() == 0)
        throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_INVALIDDRIVERPROTOCOL);

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(sConfigurationString.c_str());
    if (!result)
        throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_COULDNOTPARSEDRIVERPROTOCOL);

    pugi::xml_node adsprotocolNode = doc.child("adsprotocol");
    if (adsprotocolNode.empty())
        throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_INVALIDDRIVERPROTOCOL);

    pugi::xml_node versionNode = adsprotocolNode.child("version");
    if (versionNode.empty())
        throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_NOVERSIONDEFINITION);

    pugi::xml_attribute majorversionAttrib = versionNode.attribute("major");
    if (majorversionAttrib.empty())
        throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_NOMAJORVERSION);
    m_nMajorVersion = majorversionAttrib.as_uint(0);

    pugi::xml_attribute minorversionAttrib = versionNode.attribute("minor");
    if (minorversionAttrib.empty())
        throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_NOMINORVERSION);
    m_nMinorVersion = minorversionAttrib.as_uint(0);

    pugi::xml_attribute patchversionAttrib = versionNode.attribute("patch");
    if (patchversionAttrib.empty())
        throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_NOPATCHVERSION);
    m_nPatchVersion = patchversionAttrib.as_uint(0);

    pugi::xml_node variablesNode = adsprotocolNode.child("variables");
    if (variablesNode.empty())
        throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_NOVARIABLEDEFINITION);

    m_pWorkingDirectory = m_pDriverEnvironment->CreateWorkingDirectory();
    m_pADSDLLFile = m_pWorkingDirectory->StoreDriverData("tcadsdll_win64.dll", "tcadsdll_win64");

    m_pADSSDK = std::make_shared<CADSSDK>(m_pADSDLLFile->GetAbsoluteFileName ());
    m_pADSClient = std::make_shared<CADSClient>(m_pADSSDK);


    auto statusNodes = variablesNode.children();
    for (pugi::xml_node childNode : statusNodes)
    {
        /*auto pValue = readParameterFromXMLNode(childNode);

        if (dynamic_cast<CDriver_BuRBoolValue*> (pValue.get()) != nullptr)
            m_pDriverEnvironment->RegisterBoolParameter(pValue->getName(), pValue->getDescription(), false);

        if (dynamic_cast<CDriver_BuRStringValue*> (pValue.get()) != nullptr)
            m_pDriverEnvironment->RegisterStringParameter(pValue->getName(), pValue->getDescription(), "");

        if (dynamic_cast<CDriver_BuRLRealValue*> (pValue.get()) != nullptr)
            m_pDriverEnvironment->RegisterDoubleParameter(pValue->getName(), pValue->getDescription(), 0.0);

        if (dynamic_cast<CDriver_BuRRealValue*> (pValue.get()) != nullptr)
            m_pDriverEnvironment->RegisterDoubleParameter(pValue->getName(), pValue->getDescription(), 0.0);

        if (dynamic_cast<CDriver_BuRIntValue*> (pValue.get()) != nullptr)
            m_pDriverEnvironment->RegisterIntegerParameter(pValue->getName(), pValue->getDescription(), 0);

        if (dynamic_cast<CDriver_BuRDIntValue*> (pValue.get()) != nullptr)
            m_pDriverEnvironment->RegisterIntegerParameter(pValue->getName(), pValue->getDescription(), 0);

        m_DriverParameters.push_back(pValue);
        m_DriverParameterMap.insert(std::make_pair(pValue->getName(), pValue)); */

    }



}


std::string CDriver_ADS::GetName()
{
    return m_sName;
}

std::string CDriver_ADS::GetType()
{
    return "ads-1.0";
}

void CDriver_ADS::GetVersion(LibMCDriver_ADS_uint32& nMajor, LibMCDriver_ADS_uint32& nMinor, LibMCDriver_ADS_uint32& nMicro, std::string& sBuild)
{
    nMajor = 1;
    nMinor = 0;
    nMicro = 0;
}

void CDriver_ADS::GetHeaderInformation(std::string& sNameSpace, std::string& sBaseName)
{
    sNameSpace = "LibMCDriver_ADS";
    sBaseName = "libmcdriver_ads";
}

void CDriver_ADS::QueryParameters()
{

}


void CDriver_ADS::SetToSimulationMode()
{
	m_bSimulationMode = true;
}

bool CDriver_ADS::IsSimulationMode()
{
	return m_bSimulationMode;
}

void CDriver_ADS::Connect(const LibMCDriver_ADS_uint32 nPort, const LibMCDriver_ADS_uint32 nTimeout)
{
    if (m_pADSClient.get() == nullptr)
        throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_DRIVERNOTCONFIGURED);

    m_pADSClient->connect(nPort);
}

void CDriver_ADS::Disconnect()
{
    m_pADSClient->disconnect();
}


bool CDriver_ADS::VariableExists(const std::string& sVariableName)
{
    if (m_pADSClient.get() == nullptr)
        throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_DRIVERNOTCONFIGURED);

    auto pVariable = m_pADSClient->findVariable(sVariableName);

    return (pVariable != nullptr);    

}

LibMCDriver_ADS_int64 CDriver_ADS::ReadIntegerValue(const std::string& sVariableName)
{
    if (m_pADSClient.get() == nullptr)
        throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_DRIVERNOTCONFIGURED);

    auto pVariable = m_pADSClient->findVariable(sVariableName);

    if (pVariable == nullptr)
        throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_VARIABLENOTFOUND, "variable not found: " + sVariableName);

    auto pIntegerVariable = dynamic_cast<CADSClientIntegerVariable*> (pVariable);
    if (pIntegerVariable == nullptr)
        throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_VARIABLEISNOTINTEGER, "variable is not of integer value: " + sVariableName);

    return pIntegerVariable->readValueFromPLC();

}

void CDriver_ADS::WriteIntegerValue(const std::string& sVariableName, const LibMCDriver_ADS_int64 nValue)
{
    if (m_pADSClient.get() == nullptr)
        throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_DRIVERNOTCONFIGURED);

    auto pVariable = m_pADSClient->findVariable(sVariableName);

    if (pVariable == nullptr)
        throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_VARIABLENOTFOUND, "variable not found: " + sVariableName);

    auto pIntegerVariable = dynamic_cast<CADSClientIntegerVariable*> (pVariable);
    if (pIntegerVariable == nullptr)
        throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_VARIABLEISNOTINTEGER, "variable is not of integer value: " + sVariableName);

    pIntegerVariable->writeValueToPLC(nValue);


}

void CDriver_ADS::GetVariableBounds(const std::string& sVariableName, LibMCDriver_ADS_int64& nMinValue, LibMCDriver_ADS_int64& nMaxValue)
{
    if (m_pADSClient.get() == nullptr)
        throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_DRIVERNOTCONFIGURED);

    auto pVariable = m_pADSClient->findVariable(sVariableName);

    if (pVariable == nullptr)
        throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_VARIABLENOTFOUND, "variable not found: " + sVariableName);

    nMinValue = -100000;
    nMaxValue = +100000;
}
