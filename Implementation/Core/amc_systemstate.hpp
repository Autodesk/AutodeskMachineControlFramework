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

#include <memory>
#include <map>
#include <string>

namespace LibMCData {
	class CDataModel;
	class CStorage;
	class CBuildJobHandler;

	typedef std::shared_ptr<CDataModel> PDataModel;
	typedef std::shared_ptr<CStorage> PStorage;
	typedef std::shared_ptr<CBuildJobHandler> PBuildJobHandler;
}

namespace LibMCDriverEnv {
	class CWrapper;
	typedef std::shared_ptr<CWrapper> PWrapper;
}


namespace AMC {

	class CLogger;
	class CStateSignalHandler;
	class CDriverHandler;
	class CToolpathHandler;
	class CServiceHandler;

	typedef std::shared_ptr<CLogger> PLogger;
	typedef std::shared_ptr<CStateSignalHandler> PStateSignalHandler;
	typedef std::shared_ptr<CDriverHandler> PDriverHandler;
	typedef std::shared_ptr<CToolpathHandler> PToolpathHandler;
	typedef std::shared_ptr<CServiceHandler> PServiceHandler;

	class CSystemState {
	private:
		AMC::PLogger m_pLogger;
		AMC::PStateSignalHandler m_pSignalHandler;
		AMC::PDriverHandler m_pDriverHandler;
		AMC::PToolpathHandler m_pToolpathHandler;
		AMC::PServiceHandler m_pServiceHandler;
		
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
		CServiceHandler* serviceHandler();

		LibMCData::CStorage * storage();
		LibMCData::CBuildJobHandler * buildJobHandler();

		PLogger getLoggerInstance();
		PStateSignalHandler getStateSignalHandlerInstance();
		PDriverHandler getDriverHandlerInstance();
		PToolpathHandler getToolpathHandlerInstance();

		void addLibraryPath(const std::string & sLibraryName, const std::string & sLibraryPath);
		std::string getLibraryPath(const std::string& sLibraryName);

		std::string getSystemUserID();


	};
	
	typedef std::shared_ptr<CSystemState> PSystemState;
	
}


#endif //__AMC_SYSTEMSTATE

