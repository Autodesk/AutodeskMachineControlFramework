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

#include "amcdata_databasemigrator_sessions.hpp"
#include "libmcdata_interfaceexception.hpp"

namespace AMCData {
		
	void CDatabaseMigrationClass_Sessions::increaseSchemaVersion(PSQLTransaction pTransaction, uint32_t nCurrentVersionIndex)
	{

		if (pTransaction.get() == nullptr)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

		switch (nCurrentVersionIndex) {
			case 19: {
				std::string sQuery = "CREATE TABLE `login_sessions` (";
				sQuery += "`uuid`  varchar ( 64 ) UNIQUE NOT NULL,";
				sQuery += "`useruuid`  varchar ( 64 ) DEFAULT ``,";
				sQuery += "`username`  varchar ( 256 ) DEFAULT ``,";
				sQuery += "`userrole`  varchar ( 256 ) DEFAULT ``,";
				sQuery += "`createtime`  INTEGER NOT NULL,";
				sQuery += "`lastactivity`  INTEGER NOT NULL,";
				sQuery += "`authenticated`  INTEGER DEFAULT 0,";
				sQuery += "`active`  INTEGER DEFAULT 1,";
				sQuery += "`updateuuid`  varchar ( 64 ))";
				pTransaction->executeStatement(sQuery);
				break;
			}
		}
	}


}

