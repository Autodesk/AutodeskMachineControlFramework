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

#include "amcdata_databasemigrator_buildjobs.hpp"
#include "libmcdata_interfaceexception.hpp"

namespace AMCData {
		
	void CDatabaseMigrationClass_BuildJobs::increaseSchemaVersion(PSQLTransaction pTransaction, uint32_t nCurrentVersionIndex)
	{

		if (pTransaction.get() == nullptr)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

		switch (nCurrentVersionIndex) {
		case 0: {
			std::string sBuildJobsQuery = "CREATE TABLE `buildjobs` (";
			sBuildJobsQuery += "`uuid`  varchar ( 64 ) UNIQUE NOT NULL,";
			sBuildJobsQuery += "`name`  varchar ( 256 ) NOT NULL,";
			sBuildJobsQuery += "`status`  varchar ( 64 ) NOT NULL,";
			sBuildJobsQuery += "`storagestreamuuid`  varchar ( 64 ) NOT NULL,";
			sBuildJobsQuery += "`layercount` integer DEFAULT 0,";
			sBuildJobsQuery += "`userid`  varchar ( 64 ) NOT NULL,";
			sBuildJobsQuery += "`updateuuid`  varchar ( 64 ),";
			sBuildJobsQuery += "`timestamp`  varchar ( 64 ) NOT NULL)";
			pTransaction->executeStatement(sBuildJobsQuery);

			std::string sBuildJobDataQuery = "CREATE TABLE `buildjobdata` (";
			sBuildJobDataQuery += "`uuid`  varchar ( 64 ) UNIQUE NOT NULL,";
			sBuildJobDataQuery += "`jobuuid`  varchar ( 64 ) NOT NULL,";
			sBuildJobDataQuery += "`storagestreamuuid`  varchar ( 64 ) NOT NULL,";
			sBuildJobDataQuery += "`name`  varchar ( 256 ) NOT NULL,";
			sBuildJobDataQuery += "`datatype`  varchar ( 64 ) NOT NULL,";
			sBuildJobDataQuery += "`userid`  varchar ( 64 ) NOT NULL,";
			sBuildJobDataQuery += "`updateuuid`  varchar ( 64 ),";
			sBuildJobDataQuery += "`active`  integer DEFAULT 0,";
			sBuildJobDataQuery += "`timestamp`  varchar ( 64 ) NOT NULL)";
			pTransaction->executeStatement(sBuildJobDataQuery);


			break;
		}

		case 4: {
			std::string sIdentifierAddQuery = "ALTER TABLE `buildjobdata` ADD `identifier` varchar ( 256 ) DEFAULT ``";
			pTransaction->executeStatement(sIdentifierAddQuery);
		}
		}

		
	}



}


