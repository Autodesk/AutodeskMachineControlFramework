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

#include "amc_api_factory.hpp"
#include "libmc_interfaceexception.hpp"

#include "API/amc_api.hpp"
#include "API/amc_api_handler_logs.hpp"
#include "API/amc_api_handler_version.hpp"
#include "API/amc_api_handler_upload.hpp"
#include "API/amc_api_handler_setup.hpp"
#include "API/amc_api_handler_status.hpp"
#include "API/amc_api_handler_build.hpp"
#include "API/amc_api_handler_root.hpp"
#include "API/amc_api_handler_frontend.hpp"
#include "API/amc_api_handler_external.hpp"
#include "API/amc_api_handler_auth.hpp"
#include "API/amc_api_handler_executions.hpp"
#include "API/amc_api_handler_alerts.hpp"
#include "API/amc_api_handler_configurations.hpp"
#include "API/amc_api_handler_dataseries.hpp"
#include "API/amc_api_handler_journal.hpp"
#include "API/amc_api_handler_config.hpp"
#include "API/amc_api_handler_state.hpp"
#include "API/amc_api_handler_pointcloud.hpp"
#include "API/amc_api_handler_pointchanneldata.hpp"
#include "API/amc_api_handler_download.hpp"
#include "API/amc_api_handler_meshgeometry.hpp"
#include "API/amc_api_handler_meshedges.hpp"
#include "API/amc_api_handler_event.hpp"
#include "API/amc_api_handler_validate.hpp"
#include "API/amc_api_handler_widget.hpp"
#include "API/amc_api_handler_module.hpp"
#include "API/amc_api_handler_contentitem.hpp"
#include "API/amc_api_handler_image.hpp"

using namespace AMC;
CAPIFactory::CAPIFactory(PAPI pAPI, PSystemState pSystemState, std::vector <AMC::PStateMachineInstance>& MachineInstanceList)
{
	if (pAPI.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
	if (pSystemState.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	pAPI->registerHandler(std::make_shared <CAPIHandler_Logs>(pSystemState->getLoggerInstance(), pSystemState->getClientHash()));
	pAPI->registerHandler(std::make_shared <CAPIHandler_Setup>(MachineInstanceList, pSystemState->getClientHash()));
	pAPI->registerHandler(std::make_shared <CAPIHandler_Status>(MachineInstanceList, pSystemState->getStateMachineData (), pSystemState->getClientHash()));
	pAPI->registerHandler(std::make_shared <CAPIHandler_Upload>(pSystemState));
	pAPI->registerHandler(std::make_shared <CAPIHandler_Build>(pSystemState));
	pAPI->registerHandler(std::make_shared <CAPIHandler_Frontend>(pSystemState));
	pAPI->registerHandler(std::make_shared <CAPIHandler_Version>(pSystemState));
	pAPI->registerHandler(std::make_shared <CAPIHandler_External>(pSystemState));
	pAPI->registerHandler(std::make_shared <CAPIHandler_Executions>(pSystemState));
	pAPI->registerHandler(std::make_shared <CAPIHandler_Alerts>(pSystemState));
	pAPI->registerHandler(std::make_shared <CAPIHandler_Configurations>(pSystemState));
	pAPI->registerHandler(std::make_shared <CAPIHandler_DataSeries>(pSystemState));
	pAPI->registerHandler(std::make_shared <CAPIHandler_Journal>(pSystemState));
	pAPI->registerHandler(std::make_shared <CAPIHandler_Config>(pSystemState));
	pAPI->registerHandler(std::make_shared <CAPIHandler_State>(pSystemState));
	pAPI->registerHandler(std::make_shared <CAPIHandler_PointCloud>(pSystemState));
	pAPI->registerHandler(std::make_shared <CAPIHandler_PointChannelData>(pSystemState));
	pAPI->registerHandler(std::make_shared <CAPIHandler_Download>(pSystemState));
	pAPI->registerHandler(std::make_shared <CAPIHandler_MeshGeometry>(pSystemState));
	pAPI->registerHandler(std::make_shared <CAPIHandler_MeshEdges>(pSystemState));
	pAPI->registerHandler(std::make_shared <CAPIHandler_Event>(pSystemState));
	pAPI->registerHandler(std::make_shared <CAPIHandler_Validate>(pSystemState));
	pAPI->registerHandler(std::make_shared <CAPIHandler_Widget>(pSystemState));
	pAPI->registerHandler(std::make_shared <CAPIHandler_Module>(pSystemState));
	pAPI->registerHandler(std::make_shared <CAPIHandler_ContentItem>(pSystemState));
	pAPI->registerHandler(std::make_shared <CAPIHandler_Image>(pSystemState));
	pAPI->registerHandler(std::make_shared <CAPIHandler_Auth>(pAPI->getSessionHandler (), pSystemState->getDataModelInstance(), pSystemState->getInstallationSecret (), pSystemState->getGitHash (), pSystemState->getClientHash (), pSystemState->getAccessControlInstance ()));

}
