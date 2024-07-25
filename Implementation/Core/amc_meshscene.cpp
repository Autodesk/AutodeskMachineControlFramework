/*++

Copyright (C) 2023 Autodesk Inc.

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


#include "amc_meshscene.hpp"
#include "libmc_exceptiontypes.hpp"
#include "common_utils.hpp"

namespace AMC {

	CMeshScene::CMeshScene(const std::string& sSceneUUID)
		: m_sSceneUUID (AMCCommon::CUtils::normalizeUUIDString (sSceneUUID))
	{

	}

	CMeshScene::~CMeshScene()
	{

	}

	std::string CMeshScene::getSceneUUID()
	{
		return m_sSceneUUID;
	}

	bool CMeshScene::isBoundToLoginSession()
	{
		// TODO
		return false;
	}


	PMeshSceneItem CMeshScene::findItem(const std::string& sItemUUID, bool bFailIfNotExistent)
	{
		std::string sNormalizedItemUUID = AMCCommon::CUtils::normalizeUUIDString(sItemUUID);
		auto iIter = m_ItemMap.find(sNormalizedItemUUID);

		if (iIter != m_ItemMap.end())
			return iIter->second;

		if (bFailIfNotExistent)
			throw ELibMCCustomException(LIBMC_ERROR_COULDNOTFINDMESHSCENEITEM, sNormalizedItemUUID);

		return nullptr;

	}

	size_t CMeshScene::getItemCount()
	{
		return m_Items.size();
	}

	PMeshSceneItem CMeshScene::getItem(const size_t nIndex)
	{
		if (nIndex >= m_Items.size())
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDMESHSCENEITEMINDEX, std::to_string (nIndex));

		return m_Items.at(nIndex);
	}

	PMeshSceneItem CMeshScene::addItem(const std::string& sMeshEntityUUID, LibMCEnv::sModelDataTransform transform)
	{
		std::string sNormalizedMeshEntityUUID = AMCCommon::CUtils::normalizeUUIDString(sMeshEntityUUID);

		std::string sNewUUID = AMCCommon::CUtils::createUUID();

		auto pResultItem = std::make_shared<CMeshSceneItem>(sNewUUID, sNormalizedMeshEntityUUID);
		pResultItem->updateTransform(transform);

		m_Items.push_back(pResultItem);
		m_ItemMap.insert(std::make_pair (pResultItem->getUUID (), pResultItem));

		return pResultItem;
		

	}

	void CMeshScene::removeItem(const std::string& sUUID)
	{
		std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);
		m_ItemMap.erase(sNormalizedUUID);
		for (auto iIter = m_Items.begin(); iIter != m_Items.end(); iIter++) {
			if ((*iIter)->getUUID() == sNormalizedUUID) {
				m_Items.erase(iIter);
				break;
			}
		}
	}


}


