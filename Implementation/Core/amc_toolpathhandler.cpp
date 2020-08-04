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


#include "amc_toolpathhandler.hpp"
#include "libmc_interfaceexception.hpp"


#define LIBRARYNAME_LIB3MF "lib3mf"

namespace AMC {


	CToolpathHandler::CToolpathHandler(LibMCData::PStorage pStorage, LibMCData::PBuildJobHandler pBuildJobHandler)
		: m_pStorage (pStorage), m_pBuildJobHandler (pBuildJobHandler)
	{
		if (pStorage.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
		if (pBuildJobHandler.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	}

	CToolpathHandler::~CToolpathHandler()
	{

	}

	CToolpathEntity* CToolpathHandler::findToolpathEntity(const std::string& sUUID, bool bFailIfNotExistent)
	{
		auto iIter = m_Entities.find(sUUID);
		if (iIter != m_Entities.end()) {
			return iIter->second.get();
		}

		if (bFailIfNotExistent)
			throw ELibMCInterfaceException(LIBMC_ERROR_TOOLPATHENTITYNOTLOADED);

		return nullptr;
	}
	
	CToolpathEntity* CToolpathHandler::loadToolpathEntity(const std::string& sJobUUID)
	{
		if (sJobUUID.empty())
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

		auto pToolpathEntity = findToolpathEntity(sJobUUID, false);
		if (pToolpathEntity == nullptr) {

			if (m_pLib3MFWrapper.get() == nullptr) {
				if (m_sLib3MFPath.empty ())
					throw ELibMCInterfaceException(LIBMC_ERROR_NO3MFLIBRARY);
				m_pLib3MFWrapper = Lib3MF::CWrapper::loadLibrary(m_sLib3MFPath);
			}

			auto pBuildJob = m_pBuildJobHandler->RetrieveJob(sJobUUID);			
			auto pNewToolpathEntity = std::make_shared<CToolpathEntity>(pBuildJob->GetStorageStream(), m_pLib3MFWrapper);
			pNewToolpathEntity->IncRef();
			m_Entities.insert(std::make_pair(sJobUUID, pNewToolpathEntity));
			return pNewToolpathEntity.get();

		}
		else {
			pToolpathEntity->IncRef();
			return pToolpathEntity;
		}
	}

	void CToolpathHandler::unloadToolpathEntity(const std::string& sUUID)
	{
		auto pToolpathEntity = findToolpathEntity(sUUID, true);
		if (pToolpathEntity->DecRef())
			m_Entities.erase(sUUID);
	}


	void CToolpathHandler::unloadAllEntities()
	{
		m_Entities.clear();
	}

	void CToolpathHandler::setLibraryPath(const std::string& sLibraryName, const std::string sLibraryPath)
	{
		if (sLibraryName == LIBRARYNAME_LIB3MF)
			m_sLib3MFPath = sLibraryPath;
	}


}


