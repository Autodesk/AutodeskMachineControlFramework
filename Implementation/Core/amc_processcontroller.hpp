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


Abstract: This is the class declaration of CProcessController

*/


#ifndef __AMC_PROCESSCONTROLLER
#define __AMC_PROCESSCONTROLLER


#include <cstdint>
#include "Common/common_chrono.hpp"
#include "amc_logger.hpp"
#include "amc_processdirectory.hpp"

#include <map>
#include <vector>
#include <thread>
#include <atomic>
#include <list>

namespace AMC {

enum class eProcessControllerStatus : uint32_t {
	Unknown = 0,
	ProcessNotStarted = 1,
	ProcessStarted = 2,
	ProcessFinished = 3,
	ProcessTerminated = 4,
	ProcessTimedOut = 5
};


/*************************************************************************************************************************
 Class declaration of CProcessController 
**************************************************************************************************************************/

class CProcessController  {
private:

	std::string m_sAbsoluteExecutableName;
	WProcessDirectory m_pExecutableDirectory;
	WProcessDirectory m_pWorkingDirectory;
	std::string m_sArgumentString;
	std::string m_sProcessSubsystemName;

	std::atomic<eProcessControllerStatus> m_Status;
	std::atomic<bool> m_bVerboseLogging;
	std::atomic<bool> m_bTerminateThread;
	std::atomic<int32_t> m_nExitCode;
	std::atomic<uint32_t> m_nTimeoutInMs;
	AMCCommon::PChrono m_pGlobalChrono;

	std::list<std::string> m_StdOutBuffer;
	std::list<std::string> m_StdErrBuffer;

	uint64_t m_nSystemStartTime;

	std::vector<std::string> m_EnvironmentVariableKeys;
	std::map<std::string, std::string> m_EnvironmentVariables;
	
	std::unique_ptr<std::thread> m_Thread;
	PLogger m_pLogger;

	void processControllerRunProcessWinAPI ();

	void processControllerRunProcessLinux();

public:


	CProcessController(const std::string & sAbsoluteExecutableName, WProcessDirectory sExecutableDirectory, AMCCommon::PChrono pGlobalChrono, AMC::PLogger pLogger);

	virtual ~CProcessController();

	eProcessControllerStatus getStatus();

	uint64_t getRunTimeInMilliseconds();

	uint64_t getRunTimeInMicroseconds();

	void setWorkingDirectory(WProcessDirectory pDirectory);

	void addEnvironmentVariable(const std::string & sVariableName, const std::string & sValue);

	bool environmentVariableExists(const std::string & sVariableName);

	void removeEnvironmentVariable(const std::string & sVariableName);

	void clearEnvironmentVariables();

	uint32_t getEnvironmentVariableCount();

	void getEnvironmentVariableByIndex(const uint32_t nVariableIndex, std::string & sVariableName, std::string & sValue);

	void startProcess(const std::string & sArgumentString, uint32_t nTimeOutInMs);

	void terminateProcess();

	int32_t getExitCode();

	void printToStdOut (const std::string & sLine);

	void printToStdErr (const std::string & sLine);

	void clearOutputBuffers();

	void setVerboseLogging(bool bVerboseLogging);


};

typedef std::shared_ptr<CProcessController> PProcessController;


} // namespace AMC

#endif // __AMC_PROCESSCONTROLLER
