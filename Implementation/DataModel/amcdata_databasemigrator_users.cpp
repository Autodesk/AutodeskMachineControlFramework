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

#include "amcdata_databasemigrator_users.hpp"
#include "libmcdata_interfaceexception.hpp"
#include "common_utils.hpp"

namespace AMCData {
		
	void CDatabaseMigrationClass_Users::increaseSchemaVersion(PSQLTransaction pTransaction, uint32_t nCurrentVersionIndex)
	{

		if (pTransaction.get() == nullptr)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

		switch (nCurrentVersionIndex) {
		case 0: {
				std::string sStreamsQuery = "CREATE TABLE `users` (";
				sStreamsQuery += "`uuid`  varchar ( 64 ) UNIQUE NOT NULL,";
				sStreamsQuery += "`login`  varchar ( 256 ) NOT NULL,";
				sStreamsQuery += "`salt`  varchar ( 256 ) NOT NULL,";
				sStreamsQuery += "`description`  varchar ( 256 ) NOT NULL,";
				sStreamsQuery += "`passwordhash`  varchar ( 256 ) NOT NULL,";
				sStreamsQuery += "`updateuuid`  varchar ( 64 ))";
				pTransaction->executeStatement(sStreamsQuery);

				// Temporarily create test user
				std::string sLogin = "test";
				std::string sPassword = "test";
				std::string sDescription = "Initial test user";
				std::string sUUID = AMCCommon::CUtils::createUUID();
				std::string sSalt = AMCCommon::CUtils::calculateRandomSHA256String(16);
				std::string sHash = AMCCommon::CUtils::calculateSHA256FromString(sSalt + sPassword);

				std::string sInsertQuery = "INSERT INTO users (uuid, login, salt, passwordhash, description) VALUES (?, ?, ?, ?, ?)";
				auto pInsertQuery = pTransaction->prepareStatement(sInsertQuery);
				pInsertQuery->setString(1, sUUID);
				pInsertQuery->setString(2, sLogin);
				pInsertQuery->setString(3, sSalt);
				pInsertQuery->setString(4, sHash);
				pInsertQuery->setString(5, sDescription);
				pInsertQuery->execute();
				pInsertQuery = nullptr;

				break;
			}

			case 3: {
				std::string sStreamsQuery = "ALTER TABLE `users` (";
				sStreamsQuery += "ADD `role` varchar ( 256 ))";
				pTransaction->executeStatement(sStreamsQuery);
			}

		}
	}


}


