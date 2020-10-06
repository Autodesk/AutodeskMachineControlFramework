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


#ifndef __LIBMC_MCCONTEXT
#define __LIBMC_MCCONTEXT

#include "libmc_interfaces.hpp"
#include "amc_statemachineinstance.hpp"
#include "amc_logger_multi.hpp"

#include "amc_statesignalhandler.hpp"
#include "amc_resourcepackage.hpp"

#include "API/amc_api_handler_root.hpp"
#include "API/amc_api.hpp"

// Parent classes
#include "libmc_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

#include <map>  
#include <memory>
#include <vector> 

#include "amc_driverhandler.hpp"

#include "pugixml.hpp"
#include "libmcplugin_dynamic.hpp"

// Include custom headers here.


namespace LibMC {
namespace Impl {


#define MAXSTATEMACHINEINSTANCECOUNT (1024 * 1024)

/*************************************************************************************************************************
 Class declaration of CMCContext 
**************************************************************************************************************************/

class CMCContext : public virtual IMCContext, public virtual CBase {
private:

	std::map <std::string, AMC::PStateMachineInstance> m_Instances;
	std::vector <AMC::PStateMachineInstance> m_InstanceList;

	std::map <std::string, LibMCPlugin::PWrapper> m_Plugins;

	LibMCPlugin::PWrapper loadPlugin (std::string sPluginName);
	LibMCEnv::PWrapper m_pEnvironmentWrapper;

	// All Global Handler objects of the system
 	AMC::PSystemState m_pSystemState;
	AMC::PStateJournal m_pStateJournal;

	// API Objects
	AMC::PAPI m_pAPI;
	AMC::PAPIHandler_Root m_pClientDistHandler;

	AMC::PResourcePackage m_pCoreResourcePackage;

	void loadParameterGroup (const pugi::xml_node& xmlNode, AMC::PParameterGroup pGroup);
	void loadParameterGroupDerives (const pugi::xml_node& xmlNode, AMC::PParameterGroup pGroup, const std::string & sStateMachineInstance);
	void loadDriverParameterGroup (const pugi::xml_node& xmlNode, AMC::PParameterGroup pGroup);

	void readSignalParameters(const pugi::xml_node& xmlNode, std::list<AMC::CStateSignalParameter> & Parameters, std::list<AMC::CStateSignalParameter>& Results);


protected:


public:

	CMCContext(LibMCData::PDataModel pDataModel);

	void ParseConfiguration(const std::string & sXMLString) override;

	void RegisterLibraryPath(const std::string& sLibraryName, const std::string& sLibraryPath, const std::string& sLibraryResource) override;

	void StartAllThreads() override;	

	void TerminateAllThreads() override;

	void Log(const std::string& sMessage, const LibMC::eLogSubSystem eSubsystem, const LibMC::eLogLevel eLogLevel) override;

	void LoadClientPackage(const std::string& sResourcePath) override;

	IAPIRequestHandler* CreateAPIRequestHandler(const std::string& sURI, const std::string& sRequestMethod, const std::string& sAuthorization) override;

	AMC::PStateMachineInstance addMachineInstance (const pugi::xml_node & xmlNode);
	AMC::PStateMachineInstance findMachineInstance (std::string sName, bool bFailIfNotExisting);	

	void addDriver(const pugi::xml_node& xmlNode);

	


};

} // namespace Impl
} // namespace LibMC

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMC_MCCONTEXT
