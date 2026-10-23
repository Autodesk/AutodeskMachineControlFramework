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

#include "amcdata_databasemigrator_machineconfiguration.hpp"
#include "libmcdata_interfaceexception.hpp"

namespace AMCData {
		
	void CDatabaseMigrationClass_MachineConfiguration::increaseSchemaVersion(PSQLTransaction pTransaction, uint32_t nCurrentVersionIndex)
	{

		if (pTransaction.get() == nullptr)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

		switch (nCurrentVersionIndex) {
		case 17: {
			std::string sConfigurationTypes = "CREATE TABLE `machineconfiguration_types` (";
			sConfigurationTypes += "`uuid` varchar ( 64 ) UNIQUE NOT NULL,";
			sConfigurationTypes += "`name` varchar ( 256 ) NOT NULL,";
			sConfigurationTypes += "`schematype` varchar ( 64 ) NOT NULL,";
			sConfigurationTypes += "`activeversionuuid` varchar ( 64 ) NOT NULL,";
			sConfigurationTypes += "`timestamp` varchar ( 64 ) NOT NULL)";
			pTransaction->executeStatement(sConfigurationTypes);

			std::string sConfigurationXSDs = "CREATE TABLE `machineconfiguration_xsds` (";
			sConfigurationXSDs += "`uuid` varchar ( 64 ) UNIQUE NOT NULL,";
			sConfigurationXSDs += "`typeuuid` varchar ( 64 ) NOT NULL,";
			sConfigurationXSDs += "`schemaversionnumber` INTEGER NOT NULL,";
			sConfigurationXSDs += "`configurationxsdstring` text NOT NULL,";
			//sConfigurationXSDs += "`defaultconfigurationxml` text NOT NULL,";
			sConfigurationXSDs += "`timestamp` varchar ( 64 ) NOT NULL)";
			pTransaction->executeStatement(sConfigurationXSDs);

			std::string sVersions = "CREATE TABLE `machineconfiguration_versions` (";
			sVersions += "`uuid` varchar ( 64 ) UNIQUE NOT NULL,";
			sVersions += "`xsduuid` varchar ( 64 ) NOT NULL,";
			sVersions += "`configurationversionnumber` INTEGER NOT NULL,";
			sVersions += "`parentversionuuid` varchar ( 64 ) NOT NULL,";
			sVersions += "`configurationxmlstring` text NOT NULL,";
			sVersions += "`useruuid` varchar ( 64 ) NOT NULL,";
			sVersions += "`timestamp` varchar ( 64 ) NOT NULL)";
			pTransaction->executeStatement(sVersions);

			break;
		}

		case 22: {
			pTransaction->executeStatement("ALTER TABLE `machineconfiguration_versions` ADD `name` varchar ( 256 ) NOT NULL DEFAULT ''");

			// Version 0 of an XSD is always its default configuration.
			pTransaction->executeStatement("UPDATE `machineconfiguration_versions` SET `name` = 'Default configuration' WHERE `configurationversionnumber` = 0");

			break;
		}

		}
	}



}


