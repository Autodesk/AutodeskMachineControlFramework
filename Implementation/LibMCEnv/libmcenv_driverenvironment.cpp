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


Abstract: This is a stub class definition of CDriverEnvironment

*/

#include "libmcenv_driverenvironment.hpp"
#include "libmcenv_interfaceexception.hpp"
#include "libmcenv_workingdirectory.hpp"
#include "libmcenv_toolpathaccessor.hpp"
#include "libmcenv_imagedata.hpp"
#include "libmcenv_tcpipconnection.hpp"
#include "libmcenv_modbustcpconnection.hpp"
#include "libmcenv_driverstatusupdatesession.hpp"
#include "libmcenv_xmldocument.hpp"
#include "libmcenv_discretefielddata2d.hpp"
#include "libmcenv_build.hpp"
#include "libmcenv_cryptocontext.hpp"

// Include custom headers here.
#include "common_utils.hpp"
#include "amc_xmldocument.hpp"
#include "amc_xmldocumentnode.hpp"

// Include custom headers here.


using namespace LibMCEnv::Impl;
/*************************************************************************************************************************
 Class definition of CDriverEnvironment
**************************************************************************************************************************/


CDriverEnvironment::CDriverEnvironment(AMC::PParameterGroup pParameterGroup, AMC::PResourcePackage pDriverResourcePackage, AMC::PResourcePackage pMachineResourcePackage, AMC::PToolpathHandler pToolpathHandler, const std::string& sBaseTempPath, AMC::PLogger pLogger, LibMCData::PDataModel pDataModel, AMCCommon::PChrono pGlobalChrono, std::string sSystemUserID, const std::string& sDriverName)
    : m_bIsInitializing(false), 
    m_pParameterGroup(pParameterGroup), 
    m_pDriverResourcePackage (pDriverResourcePackage), 
    m_pMachineResourcePackage (pMachineResourcePackage),
    m_sBaseTempPath(sBaseTempPath), 
    m_pToolpathHandler (pToolpathHandler), 
    m_pLogger (pLogger), 
    m_sDriverName (sDriverName), 
    m_pDataModel (pDataModel), 
    m_sSystemUserID (sSystemUserID), 
    m_pGlobalChrono (pGlobalChrono)
{
    if (pParameterGroup.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
    if (pParameterGroup.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
    if (pToolpathHandler.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
    if (pDataModel.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
    if (pLogger.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
    if (pGlobalChrono.get () == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
    if (sBaseTempPath.empty ())
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
    if (sDriverName.empty())
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
    if (pDriverResourcePackage.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
    if (pMachineResourcePackage.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

}

CDriverEnvironment::~CDriverEnvironment()
{

}


IDriverStatusUpdateSession* CDriverEnvironment::CreateStatusUpdateSession()
{
    return new CDriverStatusUpdateSession(m_pParameterGroup, m_pLogger, m_sDriverName);

}

IWorkingDirectory* CDriverEnvironment::CreateWorkingDirectory()
{
    return new CWorkingDirectory(m_sBaseTempPath, m_pDriverResourcePackage);
}

ITCPIPConnection* CDriverEnvironment::CreateTCPIPConnection(const std::string& sIPAddress, const LibMCEnv_uint32 nPort, const LibMCEnv_uint32 nTimeOutInMS)
{
    return new CTCPIPConnection(sIPAddress, nPort, nTimeOutInMS);
}

IModbusTCPConnection* CDriverEnvironment::CreateModbusTCPConnection(const std::string& sIPAddress, const LibMCEnv_uint32 nPort, const LibMCEnv_uint32 nTimeOutInMS)
{
    return new CModbusTCPConnection(sIPAddress, nPort, nTimeOutInMS);
}


void CDriverEnvironment::retrieveDriverDataFromPackage(AMC::PResourcePackage pResourcePackage, const std::string& sIdentifier, LibMCEnv_uint64 nDataBufferBufferSize, LibMCEnv_uint64* pDataBufferNeededCount, LibMCEnv_uint8* pDataBufferBuffer)
{
    if (pResourcePackage.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

    auto pEntry = pResourcePackage->findEntryByName(sIdentifier, false);
    if (pEntry.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_RESOURCEENTRYNOTFOUND, "resource entry not found: " + sIdentifier);

    size_t nSize = pEntry->getSize();
    if (pDataBufferNeededCount != nullptr)
        *pDataBufferNeededCount = nSize;

    if (pDataBufferBuffer != nullptr) {
        if (nDataBufferBufferSize < nSize)
            throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_BUFFERTOOSMALL);

        std::vector <uint8_t> Buffer;

        pResourcePackage->readEntry(sIdentifier, Buffer);

        if (Buffer.size() != nSize)
            throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INTERNALERROR);


        const uint8_t* pSrc = Buffer.data();
        uint8_t* pDst = pDataBufferBuffer;

        for (size_t nIndex = 0; nIndex < nSize; nIndex++) {
            *pDst = *pSrc;
            pDst++;
            pSrc++;
        }

    }

}

bool CDriverEnvironment::DriverHasResourceData(const std::string& sIdentifier)
{
    auto pEntry = m_pDriverResourcePackage->findEntryByName(sIdentifier, false);
    return (pEntry.get() != nullptr);
}

bool CDriverEnvironment::MachineHasResourceData(const std::string& sIdentifier)
{
    auto pEntry = m_pMachineResourcePackage->findEntryByName(sIdentifier, false);
    return (pEntry.get() != nullptr);
}

void CDriverEnvironment::RetrieveDriverData(const std::string& sIdentifier, LibMCEnv_uint64 nDataBufferBufferSize, LibMCEnv_uint64* pDataBufferNeededCount, LibMCEnv_uint8* pDataBufferBuffer)
{
    retrieveDriverDataFromPackage(m_pDriverResourcePackage, sIdentifier, nDataBufferBufferSize, pDataBufferNeededCount, pDataBufferBuffer);
}

void CDriverEnvironment::RetrieveDriverResourceData(const std::string& sIdentifier, LibMCEnv_uint64 nDataBufferBufferSize, LibMCEnv_uint64* pDataBufferNeededCount, LibMCEnv_uint8* pDataBufferBuffer)
{
    retrieveDriverDataFromPackage(m_pDriverResourcePackage, sIdentifier, nDataBufferBufferSize, pDataBufferNeededCount, pDataBufferBuffer);
}

void CDriverEnvironment::RetrieveMachineResourceData(const std::string& sIdentifier, LibMCEnv_uint64 nDataBufferBufferSize, LibMCEnv_uint64* pDataBufferNeededCount, LibMCEnv_uint8* pDataBufferBuffer)
{
    retrieveDriverDataFromPackage(m_pMachineResourcePackage, sIdentifier, nDataBufferBufferSize, pDataBufferNeededCount, pDataBufferBuffer);
}


IToolpathAccessor* CDriverEnvironment::CreateToolpathAccessor(const std::string& sStreamUUID)
{
    return new CToolpathAccessor (sStreamUUID, AMCCommon::CUtils::createEmptyUUID(), m_pToolpathHandler);
}

bool CDriverEnvironment::ParameterNameIsValid(const std::string& sParameterName)
{
    return AMCCommon::CUtils::stringIsValidAlphanumericNameString(sParameterName);
}

void CDriverEnvironment::RegisterStringParameter(const std::string& sParameterName, const std::string& sDescription, const std::string& sDefaultValue)
{
    if (!m_bIsInitializing)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_DRIVERISNOTINITIALISING);

    m_pParameterGroup->addNewStringParameter(sParameterName, sDescription, sDefaultValue);
}

void CDriverEnvironment::RegisterUUIDParameter(const std::string& sParameterName, const std::string& sDescription, const std::string& sDefaultValue)
{
    if (!m_bIsInitializing)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_DRIVERISNOTINITIALISING);

    m_pParameterGroup->addNewUUIDParameter(sParameterName, sDescription, AMCCommon::CUtils::normalizeUUIDString(sDefaultValue));
}

void CDriverEnvironment::RegisterDoubleParameter(const std::string& sParameterName, const std::string& sDescription, const LibMCEnv_double dDefaultValue)
{
    if (!m_bIsInitializing)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_DRIVERISNOTINITIALISING);

    m_pParameterGroup->addNewDoubleParameter(sParameterName, sDescription, dDefaultValue, 1.0);
}

void CDriverEnvironment::RegisterIntegerParameter(const std::string& sParameterName, const std::string& sDescription, const LibMCEnv_int64 nDefaultValue)
{
    if (!m_bIsInitializing)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_DRIVERISNOTINITIALISING);

    m_pParameterGroup->addNewIntParameter(sParameterName, sDescription, nDefaultValue);
}

void CDriverEnvironment::RegisterBoolParameter(const std::string& sParameterName, const std::string& sDescription, const bool bDefaultValue)
{
    if (!m_bIsInitializing)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_DRIVERISNOTINITIALISING);

    m_pParameterGroup->addNewBoolParameter(sParameterName, sDescription, bDefaultValue);
}

void CDriverEnvironment::SetStringParameter(const std::string& sParameterName, const std::string& sValue)
{
    m_pParameterGroup->setParameterValueByName(sParameterName, sValue);
}

void CDriverEnvironment::SetUUIDParameter(const std::string& sParameterName, const std::string& sValue)
{
    m_pParameterGroup->setParameterValueByName(sParameterName, AMCCommon::CUtils::normalizeUUIDString(sValue));
}

void CDriverEnvironment::SetDoubleParameter(const std::string& sParameterName, const LibMCEnv_double dValue)
{
    m_pParameterGroup->setDoubleParameterValueByName(sParameterName, dValue);
}

void CDriverEnvironment::SetIntegerParameter(const std::string& sParameterName, const LibMCEnv_int64 nValue)
{
    m_pParameterGroup->setIntParameterValueByName(sParameterName, nValue);
}

void CDriverEnvironment::SetBoolParameter(const std::string& sParameterName, const bool bValue)
{
    m_pParameterGroup->setBoolParameterValueByName(sParameterName, bValue);
}


void CDriverEnvironment::setIsInitializing(bool bIsInitializing)
{
    m_bIsInitializing = bIsInitializing;
}


void CDriverEnvironment::Sleep(const LibMCEnv_uint32 nDelay)
{
    m_pGlobalChrono->sleepMilliseconds(nDelay);
}

LibMCEnv_uint64 CDriverEnvironment::GetGlobalTimerInMilliseconds()
{
    return m_pGlobalChrono->getExistenceTimeInMilliseconds();
}

LibMCEnv_uint64 CDriverEnvironment::GetGlobalTimerInMicroseconds()
{
    return m_pGlobalChrono->getExistenceTimeInMicroseconds();
}

void CDriverEnvironment::LogMessage(const std::string& sLogString)
{
    m_pLogger->logMessage(sLogString, m_sDriverName, AMC::eLogLevel::Message);
}

void CDriverEnvironment::LogWarning(const std::string& sLogString)
{
    m_pLogger->logMessage(sLogString, m_sDriverName, AMC::eLogLevel::Warning);
}

void CDriverEnvironment::LogInfo(const std::string& sLogString)
{
    m_pLogger->logMessage(sLogString, m_sDriverName, AMC::eLogLevel::Info);
}


IImageData* CDriverEnvironment::CreateEmptyImage(const LibMCEnv_uint32 nPixelSizeX, const LibMCEnv_uint32 nPixelSizeY, const LibMCEnv_double dDPIValueX, const LibMCEnv_double dDPIValueY, const LibMCEnv::eImagePixelFormat ePixelFormat)
{
    return CImageData::createEmpty(nPixelSizeX, nPixelSizeY, dDPIValueX, dDPIValueY, ePixelFormat);
}

IImageData* CDriverEnvironment::LoadPNGImage(const LibMCEnv_uint64 nPNGDataBufferSize, const LibMCEnv_uint8* pPNGDataBuffer, const LibMCEnv_double dDPIValueX, const LibMCEnv_double dDPIValueY, const LibMCEnv::eImagePixelFormat ePixelFormat)
{
    return CImageData::createFromPNG (pPNGDataBuffer, nPNGDataBufferSize, dDPIValueX, dDPIValueY, ePixelFormat);
}


LibMCEnv::Impl::IXMLDocument* CDriverEnvironment::CreateXMLDocument(const std::string& sRootNodeName, const std::string& sDefaultNamespace)
{
    auto pDocument = std::make_shared<AMC::CXMLDocumentInstance>();

    pDocument->createEmptyDocument(sRootNodeName, sDefaultNamespace);

    return new CXMLDocument(pDocument);
}

LibMCEnv::Impl::IXMLDocument* CDriverEnvironment::ParseXMLString(const std::string& sXMLString)
{
    auto pDocument = std::make_shared<AMC::CXMLDocumentInstance>();

    try {
        pDocument->parseXMLString(sXMLString);
    }
    catch (std::exception& E) {
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNOTPARSEXMLSTRING, "could not parse XML string: " + std::string(E.what()));
    }

    return new CXMLDocument(pDocument);

}

LibMCEnv::Impl::IXMLDocument* CDriverEnvironment::ParseXMLData(const LibMCEnv_uint64 nXMLDataBufferSize, const LibMCEnv_uint8* pXMLDataBuffer)
{
    auto pDocument = std::make_shared<AMC::CXMLDocumentInstance>();

    try {
        pDocument->parseXMLData(nXMLDataBufferSize, pXMLDataBuffer);
    }
    catch (std::exception& E) {
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNOTPARSEXMLDATA, "could not parse XML data: " + std::string(E.what()));
    }

    return new CXMLDocument(pDocument);

}

IDiscreteFieldData2D* CDriverEnvironment::CreateDiscreteField2D(const LibMCEnv_uint32 nPixelSizeX, const LibMCEnv_uint32 nPixelSizeY, const LibMCEnv_double dDPIValueX, const LibMCEnv_double dDPIValueY, const LibMCEnv_double dOriginX, const LibMCEnv_double dOriginY, const LibMCEnv_double dDefaultValue)
{
    AMC::PDiscreteFieldData2DInstance pInstance = std::make_shared<AMC::CDiscreteFieldData2DInstance>(nPixelSizeX, nPixelSizeY, dDPIValueX, dDPIValueY, dOriginX, dOriginY, dDefaultValue, true);
    return new CDiscreteFieldData2D(pInstance);
}

IDiscreteFieldData2D* CDriverEnvironment::CreateDiscreteField2DFromImage(IImageData* pImageDataInstance, const LibMCEnv_double dBlackValue, const LibMCEnv_double dWhiteValue, const LibMCEnv_double dOriginX, const LibMCEnv_double dOriginY)
{
    if (pImageDataInstance == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

    auto pImageDataImpl = dynamic_cast<CImageData*> (pImageDataInstance);
    if (pImageDataImpl == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDCAST);

    uint32_t nPixelSizeX, nPixelSizeY;
    pImageDataImpl->GetSizeInPixels(nPixelSizeX, nPixelSizeY);

    double dDPIValueX, dDPIValueY;
    pImageDataImpl->GetDPI(dDPIValueX, dDPIValueY);

    AMC::PDiscreteFieldData2DInstance pFieldInstance = std::make_shared<AMC::CDiscreteFieldData2DInstance>(nPixelSizeX, nPixelSizeY, dDPIValueX, dDPIValueY, dOriginX, dOriginY, 0.0, false);

    auto pixelFormat = pImageDataImpl->GetPixelFormat();
    auto& rawPixelData = pImageDataImpl->getPixelData();

    pFieldInstance->loadFromRawPixelData (rawPixelData, pixelFormat, dBlackValue, dWhiteValue);

    return new CDiscreteFieldData2D(pFieldInstance);

}

bool CDriverEnvironment::HasBuildJob(const std::string& sBuildUUID)
{
    std::string sNormalizedBuildUUID = AMCCommon::CUtils::normalizeUUIDString(sBuildUUID);

    try {
        auto pBuildJobHandler = m_pDataModel->CreateBuildJobHandler();
        pBuildJobHandler->RetrieveJob(sNormalizedBuildUUID);
        return true;
    }
    catch (std::exception) {
        return false;
    }
}

IBuild* CDriverEnvironment::GetBuildJob(const std::string& sBuildUUID)
{
    std::string sNormalizedBuildUUID = AMCCommon::CUtils::normalizeUUIDString(sBuildUUID);

    auto pBuildJobHandler = m_pDataModel->CreateBuildJobHandler();
    auto pBuildJob = pBuildJobHandler->RetrieveJob(sNormalizedBuildUUID);
    return new CBuild(m_pDataModel, pBuildJob->GetUUID (), m_pToolpathHandler, m_sSystemUserID);
}

ICryptoContext* CDriverEnvironment::CreateCryptoContext()
{
    return new CCryptoContext();
}

