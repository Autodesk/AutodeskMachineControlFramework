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
#include "amcdata_databasemigrator_userpreferences.hpp"
#include "libmcdata_interfaceexception.hpp"

namespace AMCData {
		
	void CDatabaseMigrationClass_UserPreferences::increaseSchemaVersion(PSQLTransaction pTransaction, uint32_t nCurrentVersionIndex)
	{

		if (pTransaction.get() == nullptr)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

		switch (nCurrentVersionIndex) {
		case 21: {
			// Generic per-user preference store. A preference is identified by the
			// tuple (useruuid, prefdomain, prefkey) and holds an arbitrary JSON
			// document in `value`. Used by the parameter list to persist favorites,
			// sorting, category filters and saved view presets, but not limited to it.
			std::string sUserPreferences = "CREATE TABLE `userpreferences` (";
			sUserPreferences += "`uuid` varchar ( 64 ) UNIQUE NOT NULL,";
			sUserPreferences += "`useruuid` varchar ( 64 ) NOT NULL,";
			sUserPreferences += "`prefdomain` varchar ( 256 ) NOT NULL,";
			sUserPreferences += "`prefkey` varchar ( 256 ) NOT NULL,";
			sUserPreferences += "`value` text NOT NULL,";
			sUserPreferences += "`active` integer DEFAULT 1,";
			sUserPreferences += "`updateuuid` varchar ( 64 ),";
			sUserPreferences += "`timestamp` varchar ( 64 ) NOT NULL)";
			pTransaction->executeStatement(sUserPreferences);

			std::string sUserPreferencesIndex = "CREATE UNIQUE INDEX `idx_userpreferences_userdomainkey` ";
			sUserPreferencesIndex += "ON `userpreferences` (`useruuid`, `prefdomain`, `prefkey`)";
			pTransaction->executeStatement(sUserPreferencesIndex);

			break;
		}

		}
	}



}

