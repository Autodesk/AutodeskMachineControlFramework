/*++

Copyright (C) 2022 Autodesk Inc.

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


#include "amc_unittests.hpp"

#include "amc_unittests_xmldocument.hpp"
#include "amc_unittests_xmldocumentnode.hpp"
#include "amc_unittests_xmldocumentattribute.hpp"
#include "amc_unittests_xmldocumentnamespace.hpp"

#include "amc_unittests_accesscontrol.hpp"
#include "amc_unittests_accessrole.hpp"
#include "amc_unittests_accesspermission.hpp"

#include "amc_unittests_signalslot.hpp"
#include "amc_unittests_chrono.hpp"
#include "amc_unittests_statejournal.hpp"
#include "amc_unittests_common_utils.hpp"
#include "amc_unittests_resourcepackage.hpp"
#include "amc_unittests_jpeg.hpp"
#include "amc_unittests_streams.hpp"
#include "amc_unittests_alerts.hpp"
#include "amc_unittests_dataseries.hpp"
#include "amc_unittests_discretefielddata2d.hpp"
#include "amc_unittests_jsonwriter.hpp"
#include "amc_unittests_geometryutils.hpp"
#include "amc_unittests_logger.hpp"
#include "amc_unittests_mesh.hpp"
#include "amc_unittests_parameters.hpp"
#include "amc_unittests_telemetry.hpp"

// LibMCData unit tests
#include "amc_unittests_libmcdata_storage.hpp"
#include "amc_unittests_libmcdata_logging.hpp"
#include "amc_unittests_libmcdata_journal.hpp"
#include "amc_unittests_libmcdata_alerts.hpp"
#include "amc_unittests_libmcdata_telemetryreader.hpp"
#include "amc_unittests_libmcdata_buildjobs.hpp"
#include "amc_unittests_libmcdata_users.hpp"
#include "amc_unittests_libmcdata_persistency.hpp"

// New LibMCData unit tests for coverage
#include "amc_unittests_libmcdata_datamodel.hpp"
#include "amc_unittests_libmcdata_installationinfo.hpp"
#include "amc_unittests_libmcdata_machineconfig.hpp"
#include "amc_unittests_libmcdata_customdata.hpp"


using namespace AMCUnitTest;


CUnitTests::CUnitTests (PUnitTestIO pIO)
	: CUnitTestFramework (pIO)
{

	registerTestGroup(std::make_shared <CUnitTestGroup_XMLDocument>());
	registerTestGroup(std::make_shared <CUnitTestGroup_XMLDocumentNode>());
	registerTestGroup(std::make_shared <CUnitTestGroup_XMLDocumentAttribute>());
	registerTestGroup(std::make_shared <CUnitTestGroup_XMLDocumentNamespace>());

	registerTestGroup(std::make_shared <CUnitTestGroup_AccessControl>());
	registerTestGroup(std::make_shared <CUnitTestGroup_AccessRole>());
	registerTestGroup(std::make_shared <CUnitTestGroup_AccessPermission>());

	registerTestGroup(std::make_shared <CUnitTestGroup_SignalSlot>());
	registerTestGroup(std::make_shared <CUnitTestGroup_Chrono>());
	registerTestGroup(std::make_shared <CUnitTestGroup_StateJournal>());
	registerTestGroup(std::make_shared <CUnitTestGroup_CommonUtils>());
	registerTestGroup(std::make_shared <CUnitTestGroup_ResourcePackage>());
	registerTestGroup(std::make_shared <CUnitTestGroup_JPEG>());
	registerTestGroup(std::make_shared <CUnitTestGroup_Streams>());
	registerTestGroup(std::make_shared <CUnitTestGroup_Alerts>());
	registerTestGroup(std::make_shared <CUnitTestGroup_DataSeries>());
	registerTestGroup(std::make_shared <CUnitTestGroup_DiscreteFieldData2D>());
	registerTestGroup(std::make_shared <CUnitTestGroup_JSONWriter>());
	registerTestGroup(std::make_shared <CUnitTestGroup_GeometryUtils>());
	registerTestGroup(std::make_shared <CUnitTestGroup_Logger>());
	registerTestGroup(std::make_shared <CUnitTestGroup_Mesh>());
	registerTestGroup(std::make_shared <CUnitTestGroup_Parameters>());
	registerTestGroup(std::make_shared <CUnitTestGroup_Telemetry>());

	// LibMCData unit tests
	registerTestGroup(std::make_shared <CUnitTestGroup_LibMCData_Storage>());
	registerTestGroup(std::make_shared <CUnitTestGroup_LibMCData_Logging>());
	registerTestGroup(std::make_shared <CUnitTestGroup_LibMCData_Journal>());
	registerTestGroup(std::make_shared <CUnitTestGroup_LibMCData_Alerts>());
	registerTestGroup(std::make_shared <CUnitTestGroup_LibMCData_TelemetryReader>());
	registerTestGroup(std::make_shared <CUnitTestGroup_LibMCData_BuildJobs>());
	registerTestGroup(std::make_shared <CUnitTestGroup_LibMCData_Users>());
	registerTestGroup(std::make_shared <CUnitTestGroup_LibMCData_Persistency>());

	// New LibMCData unit tests for coverage
	registerTestGroup(std::make_shared <CUnitTestGroup_LibMCData_DataModel>());
	registerTestGroup(std::make_shared <CUnitTestGroup_LibMCData_InstallationInfo>());
	registerTestGroup(std::make_shared <CUnitTestGroup_LibMCData_MachineConfig>());
	registerTestGroup(std::make_shared <CUnitTestGroup_LibMCData_CustomData>());
}
