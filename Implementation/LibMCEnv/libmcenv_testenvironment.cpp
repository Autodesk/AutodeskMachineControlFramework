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


Abstract: This is a stub class definition of CTestEnvironment

*/

#include "libmcenv_testenvironment.hpp"
#include "libmcenv_interfaceexception.hpp"
#include "common_utils.hpp"
#include "common_exportstream_native.hpp"

using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CTestEnvironment 
**************************************************************************************************************************/

CTestEnvironment::CTestEnvironment(const std::string& sTestOutputDirectory)
{
    std::string sFullPath = AMCCommon::CUtils::getFullPathName(sTestOutputDirectory, true);
    if (!AMCCommon::CUtils::fileOrPathExistsOnDisk(sFullPath))
        AMCCommon::CUtils::createDirectoryOnDisk(sFullPath);

    m_sTestOutputDirectory = AMCCommon::CUtils::includeTrailingPathDelimiter (sFullPath);
}

CTestEnvironment::~CTestEnvironment()
{

}

void CTestEnvironment::WriteTestOutput(const std::string & sOutputName, const LibMCEnv_uint64 nDataBufferSize, const LibMCEnv_uint8 * pDataBuffer)
{
    for (auto ch : sOutputName) {
        bool bIsValid = ((ch >= 'a') && (ch <= 'z')) ||
            ((ch >= 'A') && (ch <= 'Z')) ||
            ((ch >= '0') && (ch <= '9')) ||
            (ch == '.') || (ch == '-') || (ch == '_');

        if (!bIsValid)
            throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDTESTOUTPUTNAME, "invalid test output name: " + sOutputName);
    }

    AMCCommon::CExportStream_Native exportStream(sOutputName);
    if (nDataBufferSize > 0) {
        exportStream.writeBuffer((void*)pDataBuffer, nDataBufferSize);
    }
	
}

