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

#include "amc_api_handler_mesh.hpp"
#include "amc_api_jsonrequest.hpp"

#include "libmc_interfaceexception.hpp"
#include "libmcdata_dynamic.hpp"

#include "common_utils.hpp"

#include <vector>
#include <memory>
#include <string>
#include <iostream>

using namespace AMC;

class CAPIRenderGeometryResponse : public CAPIFixedFloatBufferResponse {
private:

public:

	CAPIRenderGeometryResponse(const std::string& sContentType, CMeshEntity* pMeshEntity)
		: CAPIFixedFloatBufferResponse (sContentType)
	{

		if (pMeshEntity == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

		size_t nFaceCount = pMeshEntity->getFaceCount();
		resizeTo(nFaceCount * 9);

		for (size_t nFaceIndex = 0; nFaceIndex < nFaceCount; nFaceIndex++) {
			//pMeshEntity->getFacePositions (nFaceIndex, );
		}
	}

};


CAPIHandler_Mesh::CAPIHandler_Mesh(const std::string& sClientHash, PMeshHandler pMeshHandler)
	: CAPIHandler (sClientHash), m_pMeshHandler (pMeshHandler)
{
	if (pMeshHandler.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

}

CAPIHandler_Mesh::~CAPIHandler_Mesh()
{
	
}
				
std::string CAPIHandler_Mesh::getBaseURI ()
{
	return "api/mesh";
}

APIHandler_MeshType CAPIHandler_Mesh::parseRequest(const std::string& sURI, const eAPIRequestType requestType, std::string& paramUUID)
{
	// Leave away base URI
 	auto sParameterString = AMCCommon::CUtils::toLowerString (sURI.substr(9));
	paramUUID = "";

	if (requestType == eAPIRequestType::rtGet) {

		if (sParameterString.substr (0, 15) == "rendergeometry/") {
			paramUUID = AMCCommon::CUtils::normalizeUUIDString (sParameterString.substr(15));
			return APIHandler_MeshType::mtRenderGeometry;
		}

	}


	return APIHandler_MeshType::mtUnknown;
}


PAPIResponse CAPIHandler_Mesh::handleGetRenderGeometryRequest(PAPIAuth pAuth, std::string& meshUUID)
{
	auto pAPIResponse = std::make_shared<CAPIFixedFloatBufferResponse> ("application/binary");

	auto pMeshEntity = m_pMeshHandler->findMeshEntity(meshUUID, true);


	float dWidth = 5.0f;
	float dHeight = 3.0f;

	pAPIResponse->resizeTo (18);

	pAPIResponse->addFloat (dWidth);
	pAPIResponse->addFloat (dHeight);
	pAPIResponse->addFloat (0.0f);

	pAPIResponse->addFloat(dWidth);
	pAPIResponse->addFloat(0.0f);
	pAPIResponse->addFloat(0.0f);

	pAPIResponse->addFloat(0.0f);
	pAPIResponse->addFloat(0.0f);
	pAPIResponse->addFloat(2.0f);

	pAPIResponse->addFloat(0.0f);
	pAPIResponse->addFloat(0.0f);
	pAPIResponse->addFloat(2.0f);

	pAPIResponse->addFloat(0.0f);
	pAPIResponse->addFloat(dHeight);
	pAPIResponse->addFloat(0.0f);

	pAPIResponse->addFloat(dWidth);
	pAPIResponse->addFloat(dHeight);
	pAPIResponse->addFloat(0.0f);

	return pAPIResponse;
}



PAPIResponse CAPIHandler_Mesh::handleRequest(const std::string& sURI, const eAPIRequestType requestType, CAPIFormFields & pFormFields, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth)
{
	std::string paramUUID;

	auto buildType = parseRequest(sURI, requestType, paramUUID);

	CJSONWriter writer;
	writeJSONHeader(writer, AMC_API_PROTOCOL_MESH);

	switch (buildType) {
	case APIHandler_MeshType::mtRenderGeometry:
		return handleGetRenderGeometryRequest (pAuth, paramUUID);
		break;

	default:
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	}

	return std::make_shared<CAPIStringResponse>(AMC_API_HTTP_SUCCESS, AMC_API_CONTENTTYPE, writer.saveToString());

	return nullptr;
}



		

