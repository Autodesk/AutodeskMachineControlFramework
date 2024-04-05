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


#ifndef __AMC_UI_SYSTEMSTATE
#define __AMC_UI_SYSTEMSTATE

#include "header_protection.hpp"
#include "header_pugixml.hpp"
#include "common_chrono.hpp"

namespace LibMCData {
	class CDataModel;

	typedef std::shared_ptr<CDataModel> PDataModel;
}



namespace AMC {

	class CStateMachineData;
	class CParameterHandler;
	class CToolpathHandler;
	class CMeshHandler;
	class CAccessControl;
	class CAlertHandler;
	class CLanguageHandler;

	amcDeclareDependingClass(CLogger, PLogger);
	amcDeclareDependingClass(CStateJournal, PStateJournal);
	amcDeclareDependingClass(CStateSignalHandler, PStateSignalHandler);
	amcDeclareDependingClass(CStateMachineData, PStateMachineData);
	amcDeclareDependingClass(CParameterHandler, PParameterHandler);
	amcDeclareDependingClass(CToolpathHandler, PToolpathHandler);
	amcDeclareDependingClass(CMeshHandler, PMeshHandler);
	amcDeclareDependingClass(CDataSeriesHandler, PDataSeriesHandler);
	amcDeclareDependingClass(CAccessControl, PAccessControl);
	amcDeclareDependingClass(CAlertHandler, PAlertHandler);
	amcDeclareDependingClass(CLanguageHandler, PLanguageHandler);
	amcDeclareDependingClass(CUISystemState, PUISystemState);

	class CUISystemState {
	private:

		PStateMachineData m_pStateMachineData;
		PStateSignalHandler m_pSignalHandler;
		PLogger m_pLogger;
		PStateJournal m_pStateJournal;

		PAccessControl m_pAccessControl;
		PLanguageHandler m_pLanguageHandler;
		PDataSeriesHandler m_pDataSeriesHandler;
		PAlertHandler m_pAlertHandler;
				
		// LibMCData::PDataModel is thread safe, so it can be shared between contexts.
		LibMCData::PDataModel m_pDataModel;

		PToolpathHandler m_pToolpathHandler;
		PMeshHandler m_pMeshHandler;
		AMCCommon::PChrono m_pGlobalChronoInstance;

		std::string m_sTestOutputPath;
		std::string m_sSystemUserID;

	public:
		CUISystemState(PStateMachineData pStateMachineData, AMC::PToolpathHandler pToolpathHandler, PStateSignalHandler pSignalHandler, PLogger pLogger, PStateJournal pStateJournal, const std::string& sTestOutputPath, const std::string& sSystemUserID, PAccessControl pAccessControl, PLanguageHandler pLanguageHandler, PMeshHandler pMeshHandler, PDataSeriesHandler pDataSeriesHandler, AMCCommon::PChrono pGlobalChronoInstance, PAlertHandler pAlertHandler, LibMCData::PDataModel pDataModel);

		virtual ~CUISystemState();

		PStateMachineData getStateMachineData();
		PStateSignalHandler getSignalHandler();
		PLogger getLogger();
		PStateJournal getStateJournal();
		PAlertHandler getAlertHandler();

		PAccessControl getAccessControl();
		PLanguageHandler getLanguageHandler();

		LibMCData::PDataModel getDataModel();

		PToolpathHandler getToolpathHandler();
		PMeshHandler getMeshHandler();
		PDataSeriesHandler getDataSeriesHandler();
		AMCCommon::PChrono getGlobalChronoInstance();

		std::string getTestOutputPath();
		std::string getSystemUserID();
	};

}

#endif //__AMC_UI_SYSTEMSTATE

