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


Abstract: This is the class declaration of CWorkingFile

*/


#ifndef __LIBMCENV_WORKINGFILE
#define __LIBMCENV_WORKINGFILE

#include "libmcenv_interfaces.hpp"

// Parent classes
#include "libmcenv_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include <set>
#include <string>

#include "amc_logger.hpp"

namespace LibMCEnv {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CWorkingFile 
**************************************************************************************************************************/

class CWorkingFileMonitor {

private:

    bool m_bIsActive;

    std::string m_sWorkingDirectory;
    std::set<std::string> m_MonitoredFileNames;

public:

    CWorkingFileMonitor(const std::string & sWorkingDirectory);

    std::string getWorkingDirectory();

    std::string getAbsoluteFileName(const std::string& sFileName);

    void addNewMonitoredFile(const std::string& sFileName);

    bool fileIsMonitored(const std::string& sFileName);

    void cleanUpDirectory(AMC::CLogger* pLoggerForUnmanagedFileWarnings);

    bool isActive();

    std::set<std::string> getFileNames();

};


typedef std::shared_ptr<CWorkingFileMonitor> PWorkingFileMonitor;


class CWorkingFile : public virtual IWorkingFile, public virtual CBase {
private:

protected:
    std::string m_sFileName;
    std::string m_sAbsolutePath;

    PWorkingFileMonitor m_pWorkingFileMonitor;

public:

    static CWorkingFile * makeFrom(CWorkingFile* pWorkingFile);

    static std::shared_ptr<CWorkingFile> makeSharedFrom(CWorkingFile* pWorkingFile);

    CWorkingFile(const std::string& sFileName, PWorkingFileMonitor pWorkingFileMonitor);

	std::string GetAbsoluteFileName() override;

	LibMCEnv_uint64 GetSize() override;

	std::string CalculateSHA2() override;

	IWorkingFileExecution * ExecuteFile() override;

	bool IsManaged() override;

	void MakeManaged() override;

	bool FileExists() override;

    bool DeleteFromDisk() override;

};

} // namespace Impl
} // namespace LibMCEnv

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCENV_WORKINGFILE
