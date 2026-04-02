/*++

Copyright (C) 2026 Autodesk Inc.

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

#include "amc_api_handler_meshgeometry.hpp"

#include "amc_meshentity.hpp"
#include "amc_meshhandler.hpp"

#include "libmc_interfaceexception.hpp"
#include "common_utils.hpp"

using namespace AMC;

class CAPIRenderGeometryResponse : public CAPIFixedFloatBufferResponse {
public:
	CAPIRenderGeometryResponse(CMeshEntity* pMeshEntity)
		: CAPIFixedFloatBufferResponse("application/binary")
	{
		if (pMeshEntity == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

		size_t nFaceCount = pMeshEntity->getFaceCount();
		resizeTo(nFaceCount * 9);

		for (size_t nFaceID = 1; nFaceID <= nFaceCount; nFaceID++) {
			std::array<sMeshEntityNode, 3> Nodes;
			pMeshEntity->getFaceNodes(nFaceID, Nodes.at(0), Nodes.at(1), Nodes.at(2));
			for (uint32_t nNodeIndex = 0; nNodeIndex < 3; nNodeIndex++) {
				for (uint32_t nCoordIndex = 0; nCoordIndex < 3; nCoordIndex++)
					addFloat(Nodes[nNodeIndex].m_fCoordinates[nCoordIndex]);
			}
		}
	}
};

CAPIHandler_MeshGeometry::CAPIHandler_MeshGeometry(PSystemState pSystemState)
	: CAPIHandler(pSystemState->getClientHash()), m_pSystemState(pSystemState)
{
	if (pSystemState.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
}

CAPIHandler_MeshGeometry::~CAPIHandler_MeshGeometry()
{
}

std::string CAPIHandler_MeshGeometry::getBaseURI()
{
	return "api/meshgeometry";
}

PAPIResponse CAPIHandler_MeshGeometry::handleRequest(const std::string& sURI, const eAPIRequestType requestType, CAPIFormFields& pFormFields, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth)
{
	pFormFields;
	pBodyData;
	nBodyDataSize;

	if (pAuth.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
	if (requestType != eAPIRequestType::rtGet)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	auto sParameterString = sURI.substr(getBaseURI().length());
	if ((sParameterString.length() < 37) || (sParameterString[0] != '/'))
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	std::string sMeshUUID = AMCCommon::CUtils::normalizeUUIDString(sParameterString.substr(1, 36));
	auto pMeshHandler = m_pSystemState->getMeshHandlerInstance();
	auto pMeshEntity = pMeshHandler->findMeshEntity(sMeshUUID, true);

	return std::make_shared<CAPIRenderGeometryResponse>(pMeshEntity.get());
}

