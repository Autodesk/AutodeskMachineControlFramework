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


Abstract: This is a stub class definition of CWorkingDirectory

*/

#include "libmcenv_workingdirectory.hpp"
#include "libmcenv_interfaceexception.hpp"
#include "libmcenv_workingfile.hpp"
#include "libmcenv_workingfileiterator.hpp"

// Include custom headers here.
#include "common_utils.hpp"
#include "common_exportstream_native.hpp"

using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CWorkingDirectory 
**************************************************************************************************************************/

CWorkingDirectory::CWorkingDirectory(const std::string& sBasePath, AMC::PResourcePackage pResourcePackage)
    : m_pResourcePackage (pResourcePackage), m_sTempFileNamePrefix ("amcf_")
{
    if (pResourcePackage.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
    if (sBasePath.empty ())
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

    auto sWorkingDirectoryPath = AMCCommon::CUtils::findTemporaryFileName(sBasePath, m_sTempFileNamePrefix, "", 1024);
    AMCCommon::CUtils::createDirectoryOnDisk(sWorkingDirectoryPath);

    m_pWorkingFileMonitor = std::make_shared<CWorkingFileMonitor> (sWorkingDirectoryPath);

}


CWorkingDirectory::~CWorkingDirectory()
{
    try
    {
        if (m_pWorkingFileMonitor.get () != nullptr)
            m_pWorkingFileMonitor->cleanUpDirectory(nullptr);
    }
    catch (...)
    {
        // Last resort: Never let exceptions pass through destructor
    }
}


bool CWorkingDirectory::IsActive()
{
    return m_pWorkingFileMonitor->isActive();
}

std::string CWorkingDirectory::GetAbsoluteFilePath()
{
    return m_pWorkingFileMonitor->getWorkingDirectory ();
}

IWorkingFile * CWorkingDirectory::StoreCustomData(const std::string & sFileName, const LibMCEnv_uint64 nDataBufferBufferSize, const LibMCEnv_uint8 * pDataBufferBuffer)
{
    if (pDataBufferBuffer == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
 
    std::string sAbsoluteFileName = m_pWorkingFileMonitor->getAbsoluteFileName (sFileName);
    m_pWorkingFileMonitor->addNewMonitoredFile(sFileName);

    auto pStream = std::make_unique<AMCCommon::CExportStream_Native>(sAbsoluteFileName);
    if (nDataBufferBufferSize > 0)
        pStream->writeBuffer(pDataBufferBuffer, nDataBufferBufferSize);
    pStream.reset();

    return new CWorkingFile (sFileName, m_pWorkingFileMonitor);
}


IWorkingFile* CWorkingDirectory::StoreCustomString(const std::string& sFileName, const std::string& sDataString)
{
    std::string sAbsoluteFileName = m_pWorkingFileMonitor->getAbsoluteFileName(sFileName);
    m_pWorkingFileMonitor->addNewMonitoredFile(sFileName);

    auto pStream = std::make_unique<AMCCommon::CExportStream_Native>(sAbsoluteFileName);
    if (sDataString.length () > 0)
        pStream->writeBuffer(sDataString.c_str (), sDataString.length ());
    pStream.reset();

    return new CWorkingFile(sFileName, m_pWorkingFileMonitor);
}


IWorkingFile * CWorkingDirectory::StoreDriverData(const std::string & sFileName, const std::string & sIdentifier)
{
    std::string sAbsoluteFileName = m_pWorkingFileMonitor->getAbsoluteFileName(sFileName);
    m_pWorkingFileMonitor->addNewMonitoredFile(sFileName);

    std::vector<uint8_t> Buffer;
    m_pResourcePackage->readEntry(sIdentifier, Buffer);

    auto pStream = std::make_unique<AMCCommon::CExportStream_Native>(sAbsoluteFileName);
    if (Buffer.size () > 0)
        pStream->writeBuffer(Buffer.data(), Buffer.size ());
    pStream.reset();

    return new CWorkingFile(sFileName, m_pWorkingFileMonitor);

}

std::string CWorkingDirectory::generateFileNameForExtension(const std::string& sExtension)
{
    std::string sFilteredExtension = sExtension;
    if (!sFilteredExtension.empty()) {
        if (sExtension.length () > 64)
            throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_TEMPFILEEXTENSIONEXCEEDS64CHARACTERS);

        if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString (sExtension))
            throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_TEMPFILEEXTENSIONCONTAINSINVALIDCHARACTERS);

        sFilteredExtension = "." + sExtension;
    }
    
    uint32_t nMaxIterations = 64;
    for (uint32_t nIndex = 0; nIndex < nMaxIterations; nIndex++) {
        std::string sFileName = m_sTempFileNamePrefix + AMCCommon::CUtils::createUUID() + sFilteredExtension;
        std::string sAbsoluteFileName = m_pWorkingFileMonitor->getAbsoluteFileName(sFileName);

        if (!AMCCommon::CUtils::fileOrPathExistsOnDisk(sAbsoluteFileName))
            return sFileName;
    }

    throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNOTGENERATETEMPFILENAME);

}

IWorkingFile* CWorkingDirectory::StoreCustomDataInTempFile(const std::string& sExtension, const LibMCEnv_uint64 nDataBufferBufferSize, const LibMCEnv_uint8* pDataBufferBuffer)
{
    std::string sFileName = generateFileNameForExtension(sExtension);
    return StoreCustomData(sFileName, nDataBufferBufferSize, pDataBufferBuffer);
}

IWorkingFile* CWorkingDirectory::StoreCustomStringInTempFile(const std::string& sExtension, const std::string& sDataString)
{
    std::string sFileName = generateFileNameForExtension(sExtension);
    return StoreCustomString(sFileName, sDataString);
}

IWorkingFile* CWorkingDirectory::StoreDriverDataInTempFile(const std::string& sExtension, const std::string& sIdentifier)
{
    std::string sFileName = generateFileNameForExtension(sExtension);
    return StoreDriverData (sFileName, sIdentifier);

}


bool CWorkingDirectory::CleanUp()
{
    m_pWorkingFileMonitor->cleanUpDirectory (nullptr);

    return !HasUnmanagedFiles();
}

IWorkingFile* CWorkingDirectory::AddManagedFile(const std::string& sFileName)
{
    m_pWorkingFileMonitor->addNewMonitoredFile(sFileName);

    return new CWorkingFile(sFileName, m_pWorkingFileMonitor);
}

bool CWorkingDirectory::HasUnmanagedFiles()
{
    return false;
}

IWorkingFileIterator* CWorkingDirectory::RetrieveUnmanagedFiles()
{
    auto pIterator = std::make_unique<CWorkingFileIterator>();

    auto fileNames = AMCCommon::CUtils::findContentOfDirectory(m_pWorkingFileMonitor->getWorkingDirectory(), true, false);
    for (auto sFileName : fileNames) {
        if (!m_pWorkingFileMonitor->fileIsMonitored (sFileName))
            pIterator->AddWorkingFile(std::make_shared<CWorkingFile>(sFileName, m_pWorkingFileMonitor));
    }

    return pIterator.release();
}

IWorkingFileIterator* CWorkingDirectory::RetrieveManagedFiles()
{
    auto pIterator = std::make_unique<CWorkingFileIterator>();

    auto fileNames = m_pWorkingFileMonitor->getFileNames ();
    for (auto sFileName : fileNames) {
        pIterator->AddWorkingFile(std::make_shared<CWorkingFile>(sFileName, m_pWorkingFileMonitor));
    }

    return pIterator.release();

}

IWorkingFileIterator* CWorkingDirectory::RetrieveAllFiles()
{

    auto pIterator = std::make_unique<CWorkingFileIterator>();

    auto fileNames = AMCCommon::CUtils::findContentOfDirectory (m_pWorkingFileMonitor->getWorkingDirectory(), true, false);
    for (auto sFileName : fileNames) {
        pIterator->AddWorkingFile(std::make_shared<CWorkingFile>(sFileName, m_pWorkingFileMonitor));
    }

    return pIterator.release();
}



