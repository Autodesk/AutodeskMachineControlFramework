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


#include "amc_meshhandler.hpp"
#include "libmc_exceptiontypes.hpp"
#include "common_utils.hpp"

namespace AMC {


	CMeshEntity::CMeshEntity(const std::string& sUUID)
		: m_sUUID (AMCCommon::CUtils::normalizeUUIDString (sUUID))
	{

	}

	CMeshEntity::~CMeshEntity()
	{

	}

	std::string CMeshEntity::getUUID()
	{
		return m_sUUID;
	}

	void CMeshEntity::IncRef()
	{

	}

	bool CMeshEntity::DecRef()
	{
		return false;
	}

	size_t CMeshEntity::getVertexCount()
	{
		return m_Vertices.size();	
	}

	size_t CMeshEntity::getEdgeCount()
	{
		return m_Edges.size();
	}

	size_t CMeshEntity::getFaceCount()
	{
		return m_Faces.size();
	}

	void CMeshEntity::getFaceVertices(uint32_t nFaceIndex, sMeshEntityNode& sNode1, sMeshEntityNode& sNode2, sMeshEntityNode& sNode3)
	{

	}


	void CMeshEntity::loadFrom3MF(Lib3MF::CLib3MFMeshObject* pMeshObject)
	{
		if (pMeshObject == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

		std::vector<Lib3MF::sPosition> vertices;
		pMeshObject->GetVertices(vertices);

		std::vector<Lib3MF::sTriangle> triangles;
		pMeshObject->GetTriangleIndices(triangles);

		size_t nVertexCount = vertices.size();
		size_t nTriangleCount = triangles.size();
		m_Vertices.resize(nVertexCount);
		m_Faces.resize(nTriangleCount);

		for (size_t nVertexIndex = 0; nVertexIndex < nVertexCount; nVertexIndex++) {
			auto& source = vertices.at(nVertexIndex);
			auto& target = m_Vertices.at(nVertexIndex);
			for (uint32_t nCoordIndex = 0; nCoordIndex < 3; nCoordIndex++)
				target.m_fCoordinates[nCoordIndex] = source.m_Coordinates[nCoordIndex];
		}

		for (size_t nTriangleIndex = 0; nTriangleIndex < nTriangleCount; nTriangleIndex++) {
			auto& source = triangles.at(nTriangleIndex);
			auto& target = m_Faces.at(nTriangleIndex);
			for (uint32_t nNodeIndex = 0; nNodeIndex < 3; nNodeIndex++) {
				target.m_nNodeIndices[nNodeIndex] = source.m_Indices[nNodeIndex];
			}
			
		}

	}


}


