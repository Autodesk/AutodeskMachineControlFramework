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

#include "amc_geometryutils.hpp"

#define MESHENTITY_ZEROEPSILON 1E-6

namespace AMC {


	CMeshEntity::CMeshEntity(const std::string& sUUID, const std::string& sName)
		: m_sUUID (AMCCommon::CUtils::normalizeUUIDString (sUUID)), m_sName (sName)
	{

	}

	CMeshEntity::~CMeshEntity()
	{

	}

	std::string CMeshEntity::getUUID()
	{
		return m_sUUID;
	}

	std::string CMeshEntity::getName()
	{
		return m_sName;
	}

	void CMeshEntity::IncRef()
	{

	}

	bool CMeshEntity::DecRef()
	{
		return false;
	}

	size_t CMeshEntity::getNodeCount()
	{
		return m_Nodes.size();	
	}

	size_t CMeshEntity::getEdgeCount()
	{
		return m_Edges.size();
	}

	size_t CMeshEntity::getFaceCount()
	{
		return m_Faces.size();
	}

	sMeshEntityNode& CMeshEntity::getNode(size_t nNodeID)
	{
		if ((nNodeID == 0) || (nNodeID > m_Nodes.size()))
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDMESHNODEID, "invalid node id: " + std::to_string (nNodeID));

		return m_Nodes.at(nNodeID - 1);
	}

	sMeshEntityEdge& CMeshEntity::getEdge(size_t nEdgeID)
	{
		if ((nEdgeID == 0) || (nEdgeID > m_Edges.size()))
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDMESHEDGEID, "invalid edge id: " + std::to_string(nEdgeID));

		return m_Edges.at(nEdgeID - 1);

	}

	sMeshEntityFace& CMeshEntity::getFace(size_t nFaceID)
	{
		if ((nFaceID == 0) || (nFaceID > m_Faces.size()))
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDMESHFACEID, "invalid face id: " + std::to_string(nFaceID));

		return m_Faces.at(nFaceID - 1);
	}


	void CMeshEntity::getFaceNodes(size_t nFaceID, sMeshEntityNode& sNode1, sMeshEntityNode& sNode2, sMeshEntityNode& sNode3)
	{

		auto& face = getFace(nFaceID);

		sNode1 = getNode(face.m_nNodeIDs[0]);
		sNode2 = getNode(face.m_nNodeIDs[1]);
		sNode3 = getNode(face.m_nNodeIDs[2]);

	}

	void CMeshEntity::getEdgeNodes(size_t nEdgeID, sMeshEntityNode& sNode1, sMeshEntityNode& sNode2)
	{
		auto& edge = getEdge(nEdgeID);

		sNode1 = getNode(edge.m_nNodeIDs[0]);
		sNode2 = getNode(edge.m_nNodeIDs[1]);

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
		m_Nodes.resize(nVertexCount);
		m_Faces.resize(nTriangleCount);

		std::map<std::pair<uint32_t, uint32_t>, sMeshEntityEdge> edgeMap;

		for (size_t nVertexIndex = 0; nVertexIndex < nVertexCount; nVertexIndex++) {
			auto& source = vertices.at(nVertexIndex);
			auto& target = m_Nodes.at(nVertexIndex);
			target.m_nNodeID = (uint32_t) nVertexIndex + 1;

			for (uint32_t nCoordIndex = 0; nCoordIndex < 3; nCoordIndex++)
				target.m_fCoordinates[nCoordIndex] = source.m_Coordinates[nCoordIndex];
		}

		for (size_t nTriangleIndex = 0; nTriangleIndex < nTriangleCount; nTriangleIndex++) {
			auto& source = triangles.at(nTriangleIndex);
			auto& target = m_Faces.at(nTriangleIndex);
			target.m_nFaceID = (uint32_t) nTriangleIndex + 1;

			for (uint32_t nNodeIndex = 0; nNodeIndex < 3; nNodeIndex++) {
				target.m_nNodeIDs[nNodeIndex] = source.m_Indices[nNodeIndex] + 1;
			}

			for (uint32_t nEdgeIndex = 0; nEdgeIndex < 3; nEdgeIndex++) {
				uint32_t nNode1 = source.m_Indices[nEdgeIndex] + 1;
				uint32_t nNode2 = source.m_Indices[(nEdgeIndex + 1) % 3] + 1;

				if (nNode1 != nNode2) {

					std::pair<uint32_t, uint32_t> key;

					if (nNode1 < nNode2) {
						key = std::make_pair(nNode1, nNode2);
					}
					else {
						key = std::make_pair(nNode2, nNode1);
					}

					auto iIter = edgeMap.find(key);
					if (iIter == edgeMap.end()) {
						sMeshEntityEdge newEdge;
						newEdge.m_nFlags = 0;
						newEdge.m_nEdgeID = 0;
						newEdge.m_nNodeIDs[0] = key.first;
						newEdge.m_nNodeIDs[1] = key.second;
						newEdge.m_nFaceIDs[0] = target.m_nFaceID;
						newEdge.m_nFaceIDs[1] = 0;
					
						edgeMap.insert(std::make_pair(key, newEdge));
					}
					else {
						iIter->second.m_nFaceIDs[1] = target.m_nFaceID;
					}
				}
				
			}
			
		}	

		m_Edges.reserve(edgeMap.size());
		uint32_t nID = 1;
		for (auto & edgeIter : edgeMap) {
			auto newEdge = edgeIter.second;
			newEdge.m_nEdgeID = nID;
			nID++;

			if ((newEdge.m_nFaceIDs[0] != 0) && (newEdge.m_nFaceIDs[1] != 0)) {
				newEdge.m_nAngleInDegrees = (uint32_t)round(calcFaceAngleInDegree (newEdge.m_nFaceIDs[0], newEdge.m_nFaceIDs[1]));
			}
			

			m_Edges.push_back(newEdge);
		}
	}

	double CMeshEntity::calcFaceAngleInDegree(size_t nFaceID1, size_t nFaceID2)
	{
		auto vNormal1 = calcFaceNormal(nFaceID1);
		auto vNormal2 = calcFaceNormal(nFaceID2);

		return CVectorUtils::vectorAngleInDegree(vNormal1, vNormal2, MESHENTITY_ZEROEPSILON);
	}


	sMeshVector3D CMeshEntity::calcFaceNormal(size_t nFaceID)
	{
		auto& face = getFace(nFaceID);
		auto& node1 = getNode(face.m_nNodeIDs[0]);
		auto& node2 = getNode(face.m_nNodeIDs[1]);
		auto& node3 = getNode(face.m_nNodeIDs[2]);

		sMeshVector3D vPos1 = CVectorUtils::vectorMake(node1.m_fCoordinates[0], node1.m_fCoordinates[1], node1.m_fCoordinates[2]);
		sMeshVector3D vPos2 = CVectorUtils::vectorMake(node2.m_fCoordinates[0], node2.m_fCoordinates[1], node2.m_fCoordinates[2]);
		sMeshVector3D vPos3 = CVectorUtils::vectorMake(node3.m_fCoordinates[0], node3.m_fCoordinates[1], node3.m_fCoordinates[2]);

		auto vVec1 = CVectorUtils::vectorSubtract(vPos2, vPos1);
		auto vVec2 = CVectorUtils::vectorSubtract(vPos3, vPos1);

		auto vCrossProduct = CVectorUtils::vectorCrossProduct(vVec1, vVec2);

		return CVectorUtils::vectorNormalize(vCrossProduct, MESHENTITY_ZEROEPSILON);
	}

	void CMeshEntity::loadFrom3MFResource(Lib3MF::CWrapper* p3MFWrapper, AMC::CResourcePackage* pResourcePackage, const std::string sResourceName)
	{
		if (pResourcePackage == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
		if (p3MFWrapper == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

		std::vector<uint8_t> Buffer;
		pResourcePackage->readEntry(sResourceName, Buffer);

		auto pModel = p3MFWrapper->CreateModel();
		auto pReader = pModel->QueryReader("3mf");

		pReader->ReadFromBuffer (Buffer);

		auto pMergedModel = pModel->MergeToModel();
		auto pMeshIterator = pMergedModel->GetMeshObjects();
		if (pMeshIterator->MoveNext()) {
			auto pMeshObject = pMeshIterator->GetCurrentMeshObject();
			loadFrom3MF(pMeshObject.get());
		}
		else {
			throw ELibMCInterfaceException(LIBMC_ERROR_MESH3MFRESOURCEISEMPTY, "3mf resource is empty: " + sResourceName);
		}


	}

}


