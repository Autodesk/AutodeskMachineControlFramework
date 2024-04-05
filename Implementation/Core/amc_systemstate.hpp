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
#include <mutex>
#include <string>

namespace LibMCData {
	class CDataModel;

	typedef std::shared_ptr<CDataModel> PDataModel;
}


namespace AMCCommon {
	class CChrono;
	typedef std::shared_ptr<CChrono> PChrono;
}

namespace LibMCEnv {
	class CWrapper;
	typedef std::shared_ptr<CWrapper> PWrapper;
}

namespace AMC {

	class CLogger;
	class CStateSignalHandler;
	class CDriverHandler;
	class CToolpathHandler;
	class CServiceHandler;
	class CStateJournal;
	class CUIHandler;
	class CParameterHandler;
	class CStateMachineData;
	class CAccessControl;
	class CStringResourceHandler;
	class CLanguageHandler;
	class CMeshHandler;
	class CDataSeriesHandler;
	class CAlertHandler;

	typedef std::shared_ptr<CLogger> PLogger;
	typedef std::shared_ptr<CStateSignalHandler> PStateSignalHandler;
	typedef std::shared_ptr<CDriverHandler> PDriverHandler;
	typedef std::shared_ptr<CToolpathHandler> PToolpathHandler;
	typedef std::shared_ptr<CServiceHandler> PServiceHandler;
	typedef std::shared_ptr<CUIHandler> PUIHandler;
	typedef std::shared_ptr<CStateJournal> PStateJournal;
	typedef std::shared_ptr<CParameterHandler> PParameterHandler;
	typedef std::shared_ptr<CStateMachineData> PStateMachineData;
	typedef std::shared_ptr<CAccessControl> PAccessControl;
	typedef std::shared_ptr<CStringResourceHandler> PStringResourceHandler;
	typedef std::shared_ptr<CLanguageHandler> PLanguageHandler;
	typedef std::shared_ptr<CAlertHandler> PAlertHandler;
	typedef std::shared_ptr<CMeshHandler> PMeshHandler;
	typedef std::shared_ptr<CDataSeriesHandler> PDataSeriesHandler;

	class CSystemState {
	private:
		AMC::PLogger m_pLogger;
		AMC::PStateSignalHandler m_pSignalHandler;
		AMC::PDriverHandler m_pDriverHandler;
		AMC::PToolpathHandler m_pToolpathHandler;
		AMC::PServiceHandler m_pServiceHandler;
		AMC::PUIHandler m_pUIHandler;
		AMC::PStateJournal m_pStateJournal;
		AMC::PStateMachineData m_pStateMachineData;
		AMC::PAccessControl m_pAccessControl;
		AMC::PStringResourceHandler m_pStringResourceHandler;
		AMC::PLanguageHandler m_pLanguageHandler;
		AMC::PMeshHandler m_pMeshHandler;
		AMC::PAlertHandler m_pAlertHandler;
		AMC::PDataSeriesHandler m_pDataSeriesHandler;

		AMCCommon::PChrono m_pGlobalChrono;

		LibMCData::PDataModel m_pDataModel;

		std::map <std::string, std::pair <std::string, std::string>> m_LibraryPathes;

		std::string m_sInstallationUUID;
		std::string m_sInstallationSecret;		
		std::string m_sTestEnvironmentPath;


	public:
		CSystemState(AMC::PLogger pLogger, LibMCData::PDataModel pDataModel, LibMCEnv::PWrapper pEnvWrapper, PStateJournal pStateJournal, const std::string & sTestEnvironmentPath);

		virtual ~CSystemState();

		CLogger* logger ();
		CStateSignalHandler* stateSignalHandler();
		CDriverHandler* driverHandler();
		CToolpathHandler* toolpathHandler();
		CServiceHandler* serviceHandler();
		CUIHandler* uiHandler();
		CStateMachineData* stateMachineData();
		CAccessControl * accessControl ();
		CStringResourceHandler * stringResourceHandler ();
		CAlertHandler* alertHandler();

		AMCCommon::CChrono * globalChrono();

		PLogger getLoggerInstance();		
		PStateSignalHandler getStateSignalHandlerInstance();
		PDriverHandler getDriverHandlerInstance();
		PToolpathHandler getToolpathHandlerInstance();
		PStateMachineData getStateMachineData ();
		PStateJournal getStateJournalInstance();
		PAccessControl getAccessControlInstance();
		PLanguageHandler getLanguageHandlerInstance();
		PMeshHandler getMeshHandlerInstance();
		PDataSeriesHandler getDataSeriesHandlerInstance();
		PAlertHandler getAlertHandlerInstance();

		LibMCData::PDataModel getDataModelInstance ();

		AMCCommon::PChrono getGlobalChronoInstance();

		void addLibraryPath(const std::string & sLibraryName, const std::string & sLibraryPath, const std::string& sLibraryResource);
		std::string getLibraryPath(const std::string& sLibraryName);
		std::string getLibraryResourcePath(const std::string& sLibraryName);

		std::string getSystemUserID();
		std::string getInstallationUUID(); // Returns a unique UUID of the installation
		std::string getInstallationSecret(); // Returns a unique Secret SHA256 String of the installation. MUST NOT be shared externally.
		std::string getGitHash();
		std::string getClientHash();

		std::string getTestEnvironmentPath();



	};
	
	typedef std::shared_ptr<CSystemState> PSystemState;
	
}


#endif //__AMC_SYSTEMSTATE

