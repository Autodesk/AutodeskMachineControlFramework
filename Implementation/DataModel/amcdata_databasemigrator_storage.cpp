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

#include "amcdata_databasemigrator_storage.hpp"
#include "libmcdata_interfaceexception.hpp"

namespace AMCData {
		
	void CDatabaseMigrationClass_Storage::increaseSchemaVersion(PSQLTransaction pTransaction, uint32_t nCurrentVersionIndex)
	{

		if (pTransaction.get() == nullptr)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

		switch (nCurrentVersionIndex) {
		case 0: {
				std::string sStreamsQuery = "CREATE TABLE `storage_streams` (";
				sStreamsQuery += "`uuid`  varchar ( 64 ) UNIQUE NOT NULL,";
				sStreamsQuery += "`name`  varchar ( 256 ) NOT NULL,";
				sStreamsQuery += "`mimetype`  varchar ( 256 ) NOT NULL,";
				sStreamsQuery += "`sha2`  varchar ( 64 ) NOT NULL,";
				sStreamsQuery += "`size`  integer DEFAULT 0,";
				sStreamsQuery += "`status`  varchar ( 256 ) NOT NULL,";
				sStreamsQuery += "`userid`  varchar ( 64 ) DEFAULT '',";
				sStreamsQuery += "`updateuuid`  varchar ( 64 ),";
				sStreamsQuery += "`timestamp`  varchar ( 64 ) NOT NULL)";
				pTransaction->executeStatement(sStreamsQuery);

				break;
			}


			case 1: {
				std::string sStreamsQuery = "ALTER TABLE `storage_streams` ";
				sStreamsQuery += "ADD `sha256_block64k` varchar (64) DEFAULT ``;";
				pTransaction->executeStatement(sStreamsQuery);
				break;
			}

			case 2: {
				std::string sStreamsQuery = "ALTER TABLE `storage_streams` ";
				sStreamsQuery += "ADD `identifier` varchar (256) DEFAULT `legacy`;";
				pTransaction->executeStatement(sStreamsQuery);
				break;
			}


			case 8: {
				std::string sTicketsQuery = "CREATE TABLE `storage_downloadtickets` (";
				sTicketsQuery += "`ticketuuid`  varchar ( 64 ) UNIQUE NOT NULL,";
				sTicketsQuery += "`streamuuid`  varchar ( 64 ) NOT NULL,";
				sTicketsQuery += "`clientfilename`  varchar ( 64 ) NOT NULL,";
				sTicketsQuery += "`sessionuuid`  varchar ( 64 ) NOT NULL,";
				sTicketsQuery += "`useruuid`  varchar ( 64 ) NOT NULL,";
				sTicketsQuery += "`timestamp`  varchar ( 64 ) NOT NULL)";
				pTransaction->executeStatement(sTicketsQuery);

				std::string sDownloadsQuery = "CREATE TABLE `storage_downloads` (";
				sDownloadsQuery += "`downloaduuid`  varchar ( 64 ) UNIQUE NOT NULL,";
				sDownloadsQuery += "`ticketuuid`  varchar ( 64 ) NOT NULL,";
				sDownloadsQuery += "`ipaddress`  varchar ( 128 ) NOT NULL,";
				sDownloadsQuery += "`timestamp`  varchar ( 64 ) NOT NULL)";
				pTransaction->executeStatement(sDownloadsQuery);

				break;
			}

			case 9: {
				std::string sDropAccessQuery = "DROP TABLE IF EXISTS `storage_access`";
				pTransaction->executeStatement(sDropAccessQuery);

				std::string sDropContextQuery = "DROP TABLE IF EXISTS `storage_context`";
				pTransaction->executeStatement(sDropContextQuery);

				break;
			}

			case 12: {
				std::string sStreamsQuery = "ALTER TABLE `storage_streams` ";
				sStreamsQuery += "ADD `useruuid` varchar (256) DEFAULT `00000000-0000-0000-0000-000000000000`;";
				pTransaction->executeStatement(sStreamsQuery);
				break;
			}
		}
	}


}


