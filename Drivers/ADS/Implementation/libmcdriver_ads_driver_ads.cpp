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


CDriver_ADSParameter::CDriver_ADSParameter(const std::string& sName, const std::string& sDescription, const std::string& sADSName, const eDriver_ADSParameterType eType, const eDriver_ADSParameterAccess eAccess, const uint32_t nFieldSize, const std::string& sClassName)
    : m_sName (sName), m_sDescription (sDescription), m_sADSName (sADSName), m_eAccess (eAccess), m_eType (eType), m_nFieldSize (nFieldSize), m_sClassName (sClassName)
{

}

CDriver_ADSParameter::~CDriver_ADSParameter()
{

}

std::string CDriver_ADSParameter::getName()
{
    return m_sName;
}

std::string CDriver_ADSParameter::getADSName()
{
    return m_sADSName;
}

eDriver_ADSParameterType CDriver_ADSParameter::getType()
{
    return m_eType;
}

eDriver_ADSParameterAccess CDriver_ADSParameter::getAccess()
{
    return m_eAccess;
}


uint32_t CDriver_ADSParameter::getFieldSize()
{
    return m_nFieldSize;
}

std::string CDriver_ADSParameter::getClassName()
{
    return m_sClassName;
}

std::string CDriver_ADSParameter::getDescription()
{
    return m_sDescription;
}


CDriver_ADSStruct::CDriver_ADSStruct(const std::string& sName, const std::string& sDescription, const std::string& sADSName)
    : m_sName (sName), m_sDescription(sDescription), m_sADSName (sADSName)
{

}

CDriver_ADSStruct::~CDriver_ADSStruct()
{

}

std::string CDriver_ADSStruct::getName()
{
    return m_sName;
}

std::string CDriver_ADSStruct::getADSName()
{
    return m_sADSName;
}

std::string CDriver_ADSStruct::getDescription()
{
    return m_sDescription;
}



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

PDriver_ADSParameter readParameterFromXMLNode(pugi::xml_node& node)
{
    std::string sNodeName = node.name();

    auto attribName = node.attribute("name");
    std::string sName = attribName.as_string();
    if (sName.empty ())
        throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_VARIABLENAMEMISSING);

    auto attribDescription = node.attribute("description");
    std::string sDescription = attribDescription.as_string();
    if (sDescription.empty())
        throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_VARIABLEDESCRIPTIONMISSING, "variable description missing: " + sName);

    auto attribADSName = node.attribute("adsname");
    std::string sADSName = attribADSName.as_string();
    if (sADSName.empty())
        throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_VARIABLEADSNAMEMISSING, "variable ads name missing: " + sName);

    auto attribAccess = node.attribute("access");
    std::string sAccess = attribAccess.as_string();
    if (sAccess.empty())
        throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_VARIABLEACCESSMISSING, "variable access missing: " + sName);

    eDriver_ADSParameterAccess eAccess = eDriver_ADSParameterAccess::ADSParameterAccess_Unknown;
    if (sAccess == "read")
        eAccess = eDriver_ADSParameterAccess::ADSParameterAccess_Read;
    if (sAccess == "write")
        eAccess = eDriver_ADSParameterAccess::ADSParameterAccess_Write;
    if (sAccess == "readwrite")
        eAccess = eDriver_ADSParameterAccess::ADSParameterAccess_ReadWrite;

    if (eAccess == eDriver_ADSParameterAccess::ADSParameterAccess_Unknown)
        throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_INVALIDVARIABLEACCESS, "invalid variable access: " + sName + "/" + sAccess);

    if (sNodeName == "sint")
        return std::make_shared<CDriver_ADSParameter>(sName, sDescription, sADSName, eDriver_ADSParameterType::ADSParameter_SINT, eAccess);
    if (sNodeName == "int")
        return std::make_shared<CDriver_ADSParameter>(sName, sDescription, sADSName, eDriver_ADSParameterType::ADSParameter_INT, eAccess);
    if (sNodeName == "dint")
        return std::make_shared<CDriver_ADSParameter>(sName, sDescription, sADSName, eDriver_ADSParameterType::ADSParameter_DINT, eAccess);
    if (sNodeName == "usint")
        return std::make_shared<CDriver_ADSParameter>(sName, sDescription, sADSName, eDriver_ADSParameterType::ADSParameter_USINT, eAccess);
    if (sNodeName == "uint")
        return std::make_shared<CDriver_ADSParameter>(sName, sDescription, sADSName, eDriver_ADSParameterType::ADSParameter_UINT, eAccess);
    if (sNodeName == "udint")
        return std::make_shared<CDriver_ADSParameter>(sName, sDescription, sADSName, eDriver_ADSParameterType::ADSParameter_UDINT, eAccess);
    if (sNodeName == "bool")
        return std::make_shared<CDriver_ADSParameter>(sName, sDescription, sADSName, eDriver_ADSParameterType::ADSParameter_BOOL, eAccess);
    if (sNodeName == "real")
        return std::make_shared<CDriver_ADSParameter>(sName, sDescription, sADSName, eDriver_ADSParameterType::ADSParameter_REAL, eAccess);
    if (sNodeName == "lreal")
        return std::make_shared<CDriver_ADSParameter>(sName, sDescription, sADSName, eDriver_ADSParameterType::ADSParameter_LREAL, eAccess);

    if (sNodeName == "string") {

        auto lengthAttrib = node.attribute("length");
        if (lengthAttrib.empty ())
            throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_STRINGLENGTHMISSING, "string length missing: " + sName);

        uint32_t nLength = lengthAttrib.as_uint(0);
        if ((nLength < ADS_MINSTRINGLENGTH) || (nLength > ADS_MAXSTRINGLENGTH))
            throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_INVALIDSTRINGLENGTH, "invalid string length: " + sName + " / " + std::to_string (nLength));

        return std::make_shared<CDriver_ADSParameter>(sName, sDescription, sADSName, eDriver_ADSParameterType::ADSParameter_STRING, eAccess, nLength);
    }

    throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_INVALIDVARIABLETYPE, "invalid variable type: " + sNodeName);

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

    pugi::xml_node adsprotocolNode = doc.child("driverconfiguration");
    // Depreciated fallback: root Node was called "adsprotocol" in earlier versions.
    // Going forward, the driver configurations should all be "driverconfiguration"
    if (adsprotocolNode.empty())
        adsprotocolNode = doc.child("adsprotocol");

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

    auto statusNodes = variablesNode.children();
    for (pugi::xml_node childNode : statusNodes)
    {
        PDriver_ADSParameter pParameter = readParameterFromXMLNode(childNode);

        if (pParameter.get() != nullptr) {

            m_Parameters.push_back(pParameter);
            m_ParameterMap.insert(std::make_pair (pParameter->getName (), pParameter));

            switch (pParameter->getType()) {
                case eDriver_ADSParameterType::ADSParameter_BOOL:
                    m_pDriverEnvironment->RegisterBoolParameter(pParameter->getName(), pParameter->getDescription(), false);
                    break;

                case eDriver_ADSParameterType::ADSParameter_INT:
                case eDriver_ADSParameterType::ADSParameter_SINT:
                case eDriver_ADSParameterType::ADSParameter_DINT:
                case eDriver_ADSParameterType::ADSParameter_UINT:
                case eDriver_ADSParameterType::ADSParameter_USINT:
                case eDriver_ADSParameterType::ADSParameter_UDINT:
                    m_pDriverEnvironment->RegisterIntegerParameter(pParameter->getName(), pParameter->getDescription(), 0);
                    break;

                case eDriver_ADSParameterType::ADSParameter_REAL:
                case eDriver_ADSParameterType::ADSParameter_LREAL:
                    m_pDriverEnvironment->RegisterDoubleParameter(pParameter->getName(), pParameter->getDescription(), 0.0);
                    break;

                case eDriver_ADSParameterType::ADSParameter_STRING:
                    m_pDriverEnvironment->RegisterStringParameter(pParameter->getName(), pParameter->getDescription(), "");
                    break;

            }

        }

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
    nMajor = LIBMCDRIVER_ADS_VERSION_MAJOR;
    nMinor = LIBMCDRIVER_ADS_VERSION_MINOR;
    nMicro = LIBMCDRIVER_ADS_VERSION_MICRO;
    sBuild = __STRINGIZE_VALUE_OF(__GITHASH);
}

void CDriver_ADS::GetHeaderInformation(std::string& sNameSpace, std::string& sBaseName)
{
    sNameSpace = "LibMCDriver_ADS";
    sBaseName = "libmcdriver_ads";
}

void CDriver_ADS::QueryParameters()
{
    if (m_bSimulationMode)
        return;

    if (m_pADSClient.get() != nullptr) {

        for (auto pParameter : m_Parameters) {

            auto eAccessType = pParameter->getAccess();
            if ((eAccessType == eDriver_ADSParameterAccess::ADSParameterAccess_Read) ||
                (eAccessType == eDriver_ADSParameterAccess::ADSParameterAccess_ReadWrite)) {

                auto pVariable = m_pADSClient->findVariable(pParameter->getADSName(), false);
                if (pVariable != nullptr) {

                    switch (pParameter->getType()) {
                    case eDriver_ADSParameterType::ADSParameter_BOOL: {
                        auto pBoolVariable = dynamic_cast<CADSClientBoolVariable*> (pVariable);
                        if (pBoolVariable != nullptr)
                            m_pDriverEnvironment->SetBoolParameter(pParameter->getName(), pBoolVariable->readBooleanValueFromPLC());
                        break;
                    }

                    case eDriver_ADSParameterType::ADSParameter_INT:
                    case eDriver_ADSParameterType::ADSParameter_SINT:
                    case eDriver_ADSParameterType::ADSParameter_DINT:
                    case eDriver_ADSParameterType::ADSParameter_UINT:
                    case eDriver_ADSParameterType::ADSParameter_USINT:
                    case eDriver_ADSParameterType::ADSParameter_UDINT: {
                        auto pIntegerVariable = dynamic_cast<CADSClientIntegerVariable*> (pVariable);
                        if (pIntegerVariable != nullptr)
                            m_pDriverEnvironment->SetIntegerParameter(pParameter->getName(), pIntegerVariable->readValueFromPLC());
                        break;
                    }

                    case eDriver_ADSParameterType::ADSParameter_REAL:
                    case eDriver_ADSParameterType::ADSParameter_LREAL: {
                        auto pFloatVariable = dynamic_cast<CADSClientFloatVariable*> (pVariable);
                        if (pFloatVariable != nullptr)
                            m_pDriverEnvironment->SetDoubleParameter(pParameter->getName(), pFloatVariable->readValueFromPLC());
                        break;
                    }

                    case eDriver_ADSParameterType::ADSParameter_STRING: {
                        auto pStringVariable = dynamic_cast<CADSClientStringVariable*> (pVariable);
                        if (pStringVariable != nullptr)
                            m_pDriverEnvironment->SetStringParameter(pParameter->getName(), pStringVariable->readValueFromPLC());
                        break;
                    }

                    }

                }

            }

        }

    }


}


void CDriver_ADS::SetToSimulationMode()
{
	m_bSimulationMode = true;
}

bool CDriver_ADS::IsSimulationMode()
{
	return m_bSimulationMode;
}

void CDriver_ADS::SetCustomSDKResource(const std::string& sResourceName)
{
    m_sCustomSDKResource = sResourceName;
}

void CDriver_ADS::Connect(const LibMCDriver_ADS_uint32 nPort, const LibMCDriver_ADS_uint32 nTimeout)
{
    Disconnect();

    if (m_bSimulationMode)
        return;

    m_pWorkingDirectory = m_pDriverEnvironment->CreateWorkingDirectory();

#ifdef _WIN32
    std::string sFileNameOnDisk = "tcadsdll_win64.dll";
    std::string sResourceName = "tcadsdll_win64";
#else
    std::string sFileNameOnDisk = "tcadsdll_linux64.so";
    std::string sResourceName = "tcadsdll_linux64";
#endif
   
    if (!m_sCustomSDKResource.empty())
        sResourceName = m_sCustomSDKResource;

    std::vector<uint8_t> DataBuffer;
    if (m_pDriverEnvironment->MachineHasResourceData(sResourceName)) {

        m_pDriverEnvironment->RetrieveMachineResourceData(sResourceName, DataBuffer);

    }
    else {

        m_pDriverEnvironment->RetrieveDriverResourceData(sResourceName, DataBuffer);
    }

    if (DataBuffer.empty())
        throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_INVALIDADSSDKRESOURCE);

    m_pADSDLLFile = m_pWorkingDirectory->StoreCustomData (sFileNameOnDisk, DataBuffer);

    m_pADSSDK = std::make_shared<CADSSDK>(m_pADSDLLFile->GetAbsoluteFileName());
    m_pADSClient = std::make_shared<CADSClient>(m_pADSSDK);

    m_pADSClient->connect(nPort);

    for (auto pParameter : m_Parameters) {
        auto eAccessType = pParameter->getAccess();

        if ((eAccessType == eDriver_ADSParameterAccess::ADSParameterAccess_Read) ||
            (eAccessType == eDriver_ADSParameterAccess::ADSParameterAccess_ReadWrite)) {

                switch (pParameter->getType()) {
                case eDriver_ADSParameterType::ADSParameter_BOOL:
                    m_pADSClient->registerBoolVariable(pParameter->getADSName());
                    break;
                case eDriver_ADSParameterType::ADSParameter_SINT:
                    m_pADSClient->registerInt8Variable(pParameter->getADSName());
                    break;
                case eDriver_ADSParameterType::ADSParameter_USINT:
                    m_pADSClient->registerUint8Variable(pParameter->getADSName());
                    break;
                case eDriver_ADSParameterType::ADSParameter_INT:
                    m_pADSClient->registerInt16Variable(pParameter->getADSName());
                    break;
                case eDriver_ADSParameterType::ADSParameter_UINT:
                    m_pADSClient->registerUint16Variable(pParameter->getADSName());
                    break;
                case eDriver_ADSParameterType::ADSParameter_DINT:
                    m_pADSClient->registerInt32Variable(pParameter->getADSName());
                    break;
                case eDriver_ADSParameterType::ADSParameter_UDINT:
                    m_pADSClient->registerUint32Variable(pParameter->getADSName());
                    break;
                case eDriver_ADSParameterType::ADSParameter_REAL:
                    m_pADSClient->registerFloat32Variable(pParameter->getADSName());
                    break;
                case eDriver_ADSParameterType::ADSParameter_LREAL:
                    m_pADSClient->registerFloat64Variable(pParameter->getADSName());
                    break;
                case eDriver_ADSParameterType::ADSParameter_STRING:
                    m_pADSClient->registerStringVariable(pParameter->getADSName(), pParameter->getFieldSize ());
                    break;
                default:
                    throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_INVALIDVARIABLETYPE, "invalid variable type: " + pParameter->getName());

            }

        }

    }
}

void CDriver_ADS::Disconnect()
{
    if (m_pADSClient != nullptr) {
        m_pADSClient->disconnect();
    }

    m_pADSClient = nullptr;
    m_pADSSDK = nullptr;
    m_pADSDLLFile = nullptr;
}

PDriver_ADSParameter CDriver_ADS::findParameter(const std::string& sVariableName, bool bFailIfNotExisting)
{
    auto iIter = m_ParameterMap.find(sVariableName);
    if (iIter == m_ParameterMap.end()) {
        if (bFailIfNotExisting)
            throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_VARIABLENOTFOUND, "variable not found:" + sVariableName);

        return nullptr;
    }

    return iIter->second;
}


bool CDriver_ADS::VariableExists(const std::string& sVariableName)
{
    auto iIter = m_ParameterMap.find(sVariableName);
    if (iIter == m_ParameterMap.end()) {
        return false;
    }

    return true;
}

LibMCDriver_ADS_int64 CDriver_ADS::ReadIntegerValue(const std::string& sVariableName)
{
    if (m_bSimulationMode) 
        return 0;

    if (m_pADSClient.get() == nullptr)
        throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_DRIVERNOTCONFIGURED);

    auto pParameter = findParameter(sVariableName, true);
    auto pIntegerVariable = m_pADSClient->findIntegerVariable(pParameter->getADSName (), true);
    return pIntegerVariable->readValueFromPLC();

}

void CDriver_ADS::WriteIntegerValue(const std::string& sVariableName, const LibMCDriver_ADS_int64 nValue)
{
    if (m_bSimulationMode)
        return;
    
    if (m_pADSClient.get() == nullptr)
        throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_DRIVERNOTCONFIGURED);

    auto pParameter = findParameter(sVariableName, true);
    auto pIntegerVariable = m_pADSClient->findIntegerVariable(pParameter->getADSName (), true);
    pIntegerVariable->writeValueToPLC(nValue);
}


LibMCDriver_ADS_double CDriver_ADS::ReadFloatValue(const std::string& sVariableName)
{
    if (m_bSimulationMode)
        return 0.0;

    if (m_pADSClient.get() == nullptr)
        throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_DRIVERNOTCONFIGURED);

    auto pParameter = findParameter(sVariableName, true);
    auto pFloatVariable = m_pADSClient->findFloatVariable(pParameter->getADSName(), true);
    return pFloatVariable->readValueFromPLC();


}

void CDriver_ADS::WriteFloatValue(const std::string& sVariableName, const LibMCDriver_ADS_double dValue)
{
    if (m_bSimulationMode)
        return;

    if (m_pADSClient.get() == nullptr)
        throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_DRIVERNOTCONFIGURED);

    auto pParameter = findParameter(sVariableName, true);
    auto pFloatVariable = m_pADSClient->findFloatVariable(pParameter->getADSName(), true);
    pFloatVariable->writeValueToPLC(dValue);
}

bool CDriver_ADS::ReadBoolValue(const std::string& sVariableName)
{
    if (m_bSimulationMode)
        return false;

    if (m_pADSClient.get() == nullptr)
        throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_DRIVERNOTCONFIGURED);

    auto pParameter = findParameter(sVariableName, true);
    auto pBoolVariable = m_pADSClient->findBoolVariable(pParameter->getADSName(), true);
    return pBoolVariable->readBooleanValueFromPLC();
}

void CDriver_ADS::WriteBoolValue(const std::string& sVariableName, const bool bValue)
{
    if (m_bSimulationMode)
        return;

    if (m_pADSClient.get() == nullptr)
        throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_DRIVERNOTCONFIGURED);

    auto pParameter = findParameter(sVariableName, true);
    auto pBoolVariable = m_pADSClient->findBoolVariable(pParameter->getADSName(), true);
    pBoolVariable->writeBooleanValueToPLC(bValue);
}

std::string CDriver_ADS::ReadStringValue(const std::string& sVariableName)
{
    if (m_bSimulationMode)
        return "";

    if (m_pADSClient.get() == nullptr)
        throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_DRIVERNOTCONFIGURED);

    auto pParameter = findParameter(sVariableName, true);
    auto pStringVariable = m_pADSClient->findStringVariable(pParameter->getADSName(), true);
    return pStringVariable->readValueFromPLC();
}

void CDriver_ADS::WriteStringValue(const std::string& sVariableName, const std::string& sValue)
{
    if (m_bSimulationMode)
        return;

    if (m_pADSClient.get() == nullptr)
        throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_DRIVERNOTCONFIGURED);

    auto pParameter = findParameter(sVariableName, true);
    auto pStringVariable = m_pADSClient->findStringVariable(pParameter->getADSName(), true);
    pStringVariable->writeValueToPLC(sValue);
}


void CDriver_ADS::GetVariableBounds(const std::string& sVariableName, LibMCDriver_ADS_int64& nMinValue, LibMCDriver_ADS_int64& nMaxValue)
{
    if (m_bSimulationMode) {
        nMinValue = INT32_MIN;
        nMaxValue = INT32_MAX;
        return;
    }

    if (m_pADSClient.get() == nullptr)
        throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_DRIVERNOTCONFIGURED);

    auto pIntegerVariable = m_pADSClient->findIntegerVariable(sVariableName, true);
    pIntegerVariable->getBounds(nMinValue, nMaxValue);

}
