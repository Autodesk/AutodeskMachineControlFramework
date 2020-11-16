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


Abstract: This is a stub class definition of CWorkingFile

*/

#include "libmcenv_workingfile.hpp"
#include "libmcenv_interfaceexception.hpp"

// Include custom headers here.
#include "common_utils.hpp"
#include "common_importstream_native.hpp"

using namespace LibMCEnv::Impl;


CWorkingFileMonitor::CWorkingFileMonitor(const std::string& sWorkingDirectory)
    : m_sWorkingDirectory (sWorkingDirectory), m_bIsActive (true)
{
    if (sWorkingDirectory.empty())
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
}


std::string CWorkingFileMonitor::getWorkingDirectory()
{
    return m_sWorkingDirectory;
}


std::string CWorkingFileMonitor::getAbsoluteFileName(const std::string& sFileName)
{
    if (!m_bIsActive)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_WORKINGDIRECTORYHASBEENCLEANED);

    if (sFileName.find(":") != std::string::npos)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDCHARACTERINFILENAME);
    if (sFileName.find(">") != std::string::npos)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDCHARACTERINFILENAME);
    if (sFileName.find("<") != std::string::npos)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDCHARACTERINFILENAME);
    if (sFileName.find("\"") != std::string::npos)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDCHARACTERINFILENAME);
    if (sFileName.find("/") != std::string::npos)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDCHARACTERINFILENAME);
    if (sFileName.find("\\") != std::string::npos)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDCHARACTERINFILENAME);
    if (sFileName.find("|") != std::string::npos)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDCHARACTERINFILENAME);
    if (sFileName.find("?") != std::string::npos)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDCHARACTERINFILENAME);
    if (sFileName.find("*") != std::string::npos)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDCHARACTERINFILENAME);
    if (sFileName.find("..") != std::string::npos)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDCHARACTERINFILENAME);

    return m_sWorkingDirectory + "/" + sFileName;
}

void CWorkingFileMonitor::addNewMonitoredFile(const std::string& sFileName)
{
    if (!m_bIsActive)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_WORKINGDIRECTORYHASBEENCLEANED);

    // Check if file name has the right character set.
    getAbsoluteFileName(sFileName);

    // Store file name
    m_MonitoredFileNames.insert(sFileName);
}

bool CWorkingFileMonitor::fileIsMonitored(const std::string& sFileName)
{
    if (!m_bIsActive)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_WORKINGDIRECTORYHASBEENCLEANED);

    auto iIter = m_MonitoredFileNames.find(sFileName);
    return (iIter != m_MonitoredFileNames.end());
}

void CWorkingFileMonitor::cleanUpDirectory(AMC::CLogger* pLoggerForUnmanagedFileWarnings)
{

    if (AMCCommon::CUtils::fileOrPathExistsOnDisk(m_sWorkingDirectory)) {
        for (auto sFileName : m_MonitoredFileNames) {
            auto sAbsoluteFileName = getAbsoluteFileName(sFileName);
            if (AMCCommon::CUtils::fileOrPathExistsOnDisk(sAbsoluteFileName)) {
                try
                {
                    AMCCommon::CUtils::deleteFileFromDisk(sAbsoluteFileName, true);
                }
                catch (std::exception& E)
                {
                    if (pLoggerForUnmanagedFileWarnings != nullptr)
                        pLoggerForUnmanagedFileWarnings->logMessage("Could not delete temporary file: " + sAbsoluteFileName + "(" + E.what() + ")", LOG_SUBSYSTEM_WORKINGDIRECTORIES, AMC::eLogLevel::Warning);
                }

            }
        }
    }


    try {
        if (AMCCommon::CUtils::fileOrPathExistsOnDisk(m_sWorkingDirectory)) {
            AMCCommon::CUtils::deleteDirectoryFromDisk(m_sWorkingDirectory, true);
        }
    }
    catch (std::exception& E)
    {
        if (pLoggerForUnmanagedFileWarnings != nullptr)
            pLoggerForUnmanagedFileWarnings->logMessage("Could not delete working directory: " + m_sWorkingDirectory + "(" + E.what() + ")", LOG_SUBSYSTEM_WORKINGDIRECTORIES, AMC::eLogLevel::Warning);
    }

    m_MonitoredFileNames.clear();
    m_bIsActive = false;

}

bool CWorkingFileMonitor::isActive()
{
    return m_bIsActive;
}


std::set<std::string> CWorkingFileMonitor::getFileNames()
{
    return m_MonitoredFileNames;
}

/*************************************************************************************************************************
 Class definition of CWorkingFile 
**************************************************************************************************************************/

CWorkingFile::CWorkingFile(const std::string & sFileName, PWorkingFileMonitor pWorkingFileMonitor)
    : m_pWorkingFileMonitor (pWorkingFileMonitor)
{
    if (pWorkingFileMonitor.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

    m_sFileName = sFileName;
    m_sAbsolutePath = pWorkingFileMonitor->getAbsoluteFileName(sFileName);
}


CWorkingFile* CWorkingFile::makeFrom(CWorkingFile* pWorkingFile)
{
    if (pWorkingFile == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

    return new CWorkingFile (pWorkingFile->m_sFileName, pWorkingFile->m_pWorkingFileMonitor);
}


std::shared_ptr<CWorkingFile> CWorkingFile::makeSharedFrom(CWorkingFile* pWorkingFile)
{
    return std::shared_ptr <CWorkingFile> (makeFrom (pWorkingFile));
}

std::string CWorkingFile::GetAbsoluteFileName()
{
    return m_sAbsolutePath;
}

LibMCEnv_uint64 CWorkingFile::GetSize()
{
    if (!AMCCommon::CUtils::fileOrPathExistsOnDisk(m_sAbsolutePath))
        return 0;

    auto pStream = std::make_shared<AMCCommon::CImportStream_Native> (m_sAbsolutePath);
    return pStream->retrieveSize();
}

std::string CWorkingFile::CalculateSHA2()
{
    return AMCCommon::CUtils::calculateSHA256FromFile(m_sAbsolutePath);
}


IWorkingFileExecution * CWorkingFile::ExecuteFile()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}


bool CWorkingFile::IsManaged()
{
    return m_pWorkingFileMonitor->fileIsMonitored (m_sFileName);
}

void CWorkingFile::MakeManaged()
{
    m_pWorkingFileMonitor->addNewMonitoredFile(m_sFileName);
}

bool CWorkingFile::FileExists()
{
    return AMCCommon::CUtils::fileOrPathExistsOnDisk(m_sAbsolutePath);
}

bool CWorkingFile::DeleteFile()
{
    if (AMCCommon::CUtils::fileOrPathExistsOnDisk(m_sAbsolutePath)) {
        AMCCommon::CUtils::deleteFileFromDisk(m_sAbsolutePath, false);

        return !AMCCommon::CUtils::fileOrPathExistsOnDisk(m_sAbsolutePath);
    }

    return true;
}

