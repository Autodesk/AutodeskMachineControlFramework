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

#include "amcdata_databasemigrator.hpp"
#include "libmcdata_interfaceexception.hpp"

namespace AMCData {
		
	CDatabaseMigrator::CDatabaseMigrator()
	{

	}

	void CDatabaseMigrator::addMigrationClass(PDatabaseMigrationClass pMigrationClass)
	{
		if (pMigrationClass.get() == nullptr)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

		m_MigrationClasses.push_back(pMigrationClass);
	}


	uint32_t CDatabaseMigrator::getCurrentSchemaVersion()
	{
		return 1;
	}

	void CDatabaseMigrator::migrateDatabaseSchemas(PSQLHandler pSQLHandler)
	{
		if (pSQLHandler.get() == nullptr)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

		uint32_t currentSchemaVersion = 0;
		std::string schemaTableName = "dbschema";

		auto pTransaction = pSQLHandler->beginTransaction();

		auto pStatement1 = pTransaction->prepareStatement("SELECT COUNT(*) FROM sqlite_master WHERE type = ? AND name = ?");
		pStatement1->setString(1, "table");
		pStatement1->setString(2, schemaTableName);
		if (!pStatement1->nextRow())
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INTERNALERROR);
		bool bSchemaTableExists = (pStatement1->getColumnInt(1) != 0);
		pStatement1 = nullptr;

		if (!bSchemaTableExists) {
			pTransaction->executeStatement("CREATE TABLE " + schemaTableName + " (schemaversion INTEGER DEFAULT 0)");
		}

		auto pStatement2 = pTransaction->prepareStatement("SELECT MAX (schemaversion) FROM " + schemaTableName);
		if (pStatement2->nextRow()) {
			currentSchemaVersion = pStatement2->getColumnInt(1);
		}

		uint32_t targetSchemaVersion = getCurrentSchemaVersion ();
		if (currentSchemaVersion < targetSchemaVersion) {

			for (uint32_t versionIndex = currentSchemaVersion; versionIndex < targetSchemaVersion; versionIndex++) {
				for (auto pMigrator : m_MigrationClasses)
					pMigrator->increaseSchemaVersion(pTransaction, versionIndex);
			}

			auto pStatement3 = pTransaction->prepareStatement("INSERT INTO " + schemaTableName + " (schemaversion) VALUES (?)");
			pStatement3->setInt(1, targetSchemaVersion);
			pStatement3->execute();
		}

		pTransaction->commit();			
		pTransaction = nullptr;
	}


}


