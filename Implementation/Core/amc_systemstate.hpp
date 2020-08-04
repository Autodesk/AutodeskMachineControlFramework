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

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL AUTODESK INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/


#ifndef __AMC_SYSTEMSTATE
#define __AMC_SYSTEMSTATE

#include "amc_logger.hpp"
#include "amc_parameterhandler.hpp"
#include "amc_statesignalhandler.hpp"
#include "amc_driverhandler.hpp"
#include "amc_toolpathhandler.hpp"

#include "libmcdata_dynamic.hpp"


namespace AMC {

	class CSystemState {
	private:
		AMC::PLogger m_pLogger;
		AMC::PStateSignalHandler m_pSignalHandler;
		AMC::PDriverHandler m_pDriverHandler;
		AMC::PToolpathHandler m_pToolpathHandler;
		
		LibMCData::PDataModel m_pDataModel;
		LibMCData::PStorage m_pStorage;
		LibMCData::PBuildJobHandler m_pBuildJobHandler;

		std::map <std::string, std::string> m_LibraryPathes;

	public:
		CSystemState(AMC::PLogger pLogger, LibMCData::PDataModel pDataModel, LibMCDriverEnv::PWrapper pDriverEnvWrapper);

		virtual ~CSystemState();

		CLogger* logger ();
		CStateSignalHandler* stateSignalHandler();
		CDriverHandler* driverHandler();
		CToolpathHandler* toolpathHandler();

		PLogger getLoggerInstance();
		PStateSignalHandler getStateSignalHandlerInstance();
		PDriverHandler getDriverHandlerInstance();
		PToolpathHandler getToolpathHandlerInstance();

		void addLibraryPath(const std::string & sLibraryName, const std::string & sLibraryPath);
		std::string getLibraryPath(const std::string& sLibraryName);


	};
	
	typedef std::shared_ptr<CSystemState> PSystemState;
	
}


#endif //__AMC_SYSTEMSTATE

