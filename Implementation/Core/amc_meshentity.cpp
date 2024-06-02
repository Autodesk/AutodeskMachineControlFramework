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
						newEdge.m_nValence = 1;
					
						edgeMap.insert(std::make_pair(key, newEdge));
					}
					else {
						iIter->second.m_nFaceIDs[1] = target.m_nFaceID;
						iIter->second.m_nValence++;
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


	bool CMeshEntity::isManifold()
	{
		if (m_Edges.size() == 0)
			return false;

		for (auto& edge : m_Edges)
			if (edge.m_nValence != 2)
				return false;

		return true;
	}

	bool CMeshEntity::isOriented()
	{
		if (m_Edges.size() == 0)
			return false;

		for (auto& edge : m_Edges) {
			// Valence 0 should not exist..
			if (edge.m_nValence == 0)
				return false;

			// Too many neighbors
			if (edge.m_nValence > 2)
				return false;

			// Check for consistent orientations
			if (edge.m_nValence == 2) {
				if (!adjacentFacesAreConsistentlyOriented (edge)) {
					return false;
				}
			}

			// Valence 1 is a mesh border, which has always a unique orientation
		}

		return true;

	}

	uint32_t CMeshEntity::getMaxVertexID()
	{
		return (uint32_t) m_Nodes.size();
	}

	bool CMeshEntity::vertexExists(const uint32_t nVertexID)
	{
		return (nVertexID > 0) && (nVertexID <= m_Nodes.size());
	}

	bool CMeshEntity::getVertex(const uint32_t nVertexID, double& dX, double& dY, double& dZ)
	{
		if ((nVertexID > 0) && (nVertexID <= m_Nodes.size()))
		{
			auto & node = getNode(nVertexID);
			dX = node.m_fCoordinates[0];
			dY = node.m_fCoordinates[1];
			dZ = node.m_fCoordinates[2];
			return true;
		}
		else {
			dX = 0.0;
			dY = 0.0;
			dZ = 0.0;
			return false;
		}
	}

	void CMeshEntity::getVertexIDs(uint64_t nVertexIDsBufferSize, uint64_t* pVertexIDsNeededCount, uint32_t* pVertexIDsBuffer)
	{
		size_t nNodeCount = m_Nodes.size();
		if (pVertexIDsNeededCount)
			*pVertexIDsNeededCount = nNodeCount;

		if ((nNodeCount > 0) && (pVertexIDsBuffer != nullptr)) {
			if (nVertexIDsBufferSize < nNodeCount)
				throw ELibMCInterfaceException(LIBMC_ERROR_BUFFERTOOSMALL);

			uint32_t* pPtr = pVertexIDsBuffer;
			for (size_t nVertexID = 1; nVertexID <= nNodeCount; nVertexID++) {
				*pPtr = (uint32_t)nVertexID;
				pPtr++;
			}
		}
	}

	void CMeshEntity::getAllVertices(uint64_t nVerticesBufferSize, uint64_t* pVerticesNeededCount, LibMCEnv::sMeshVertex3D* pVerticesBuffer)
	{
		size_t nNodeCount = m_Nodes.size();
		if (pVerticesNeededCount)
			*pVerticesNeededCount = nNodeCount;

		if ((nNodeCount > 0) && (pVerticesBuffer != nullptr)) {
			if (nVerticesBufferSize < nNodeCount)
				throw ELibMCInterfaceException(LIBMC_ERROR_BUFFERTOOSMALL);

			LibMCEnv::sMeshVertex3D* pPtr = pVerticesBuffer;
			for (size_t nVertexID = 1; nVertexID <= nNodeCount; nVertexID++) {
				pPtr->m_VertexID = nVertexID;
				auto& node = getNode(nVertexID);
				for (uint32_t nCoordinateIndex = 0; nCoordinateIndex < 3; nCoordinateIndex++)
					pPtr->m_Coordinates[nCoordinateIndex] = node.m_fCoordinates[nCoordinateIndex];

				pPtr++;
			}
		}

	}

	uint32_t CMeshEntity::getMaxTriangleID()
	{
		return (uint32_t)m_Faces.size();
	}

	bool CMeshEntity::triangeExists(const uint32_t nTriangleID)
	{
		return (nTriangleID > 0) && (nTriangleID <= m_Faces.size());
	}

	bool CMeshEntity::getTriangle(const uint32_t nTriangleID, uint32_t& nVertex1ID, uint32_t& nVertex2ID, uint32_t& nVertex3ID)
	{
		if ((nTriangleID > 0) && (nTriangleID <= m_Faces.size()))
		{
			auto &face = getFace (nTriangleID);
			nVertex1ID = face.m_nNodeIDs[0];
			nVertex2ID = face.m_nNodeIDs[1];
			nVertex3ID = face.m_nNodeIDs[2];
			return true;
		}
		else {
			nVertex1ID = 0;
			nVertex2ID = 0;
			nVertex3ID = 0;
			return false;
		}

	}

	void CMeshEntity::getTriangleIDs(uint64_t nTriangleIDsBufferSize, uint64_t* pTriangleIDsNeededCount, uint32_t* pTriangleIDsBuffer)
	{
		size_t nFaceCount = m_Faces.size();
		if (pTriangleIDsNeededCount)
			*pTriangleIDsNeededCount = nFaceCount;

		if ((nFaceCount > 0) && (pTriangleIDsBuffer != nullptr)) {
			if (nTriangleIDsBufferSize < nFaceCount)
				throw ELibMCInterfaceException(LIBMC_ERROR_BUFFERTOOSMALL);

			uint32_t* pPtr = pTriangleIDsBuffer;
			for (size_t nFaceID = 1; nFaceID <= nFaceCount; nFaceID++) {
				*pPtr = (uint32_t)nFaceID;
				pPtr++;
			}
		}

	}

	void CMeshEntity::getAllTriangles(uint64_t nTrianglesBufferSize, uint64_t* pTrianglesNeededCount, LibMCEnv::sMeshTriangle3D* pTrianglesBuffer)
	{
		size_t nFaceCount = m_Faces.size();
		if (pTrianglesNeededCount)
			*pTrianglesNeededCount = nFaceCount;

		if ((nFaceCount > 0) && (pTrianglesBuffer != nullptr)) {
			if (nTrianglesBufferSize < nFaceCount)
				throw ELibMCInterfaceException(LIBMC_ERROR_BUFFERTOOSMALL);

			LibMCEnv::sMeshTriangle3D* pPtr = pTrianglesBuffer;
			for (size_t nTriangleID = 1; nTriangleID <= nFaceCount; nTriangleID++) {
				pPtr->m_TriangleID = nTriangleID;
				auto& face = getFace(nTriangleID);
				for (uint32_t nCornerIndex = 0; nCornerIndex < 3; nCornerIndex++)
					pPtr->m_Vertices[nCornerIndex] = face.m_nNodeIDs[nCornerIndex];

				pPtr++;
			}
		}

	}


	bool CMeshEntity::checkEdgeOrientationWithFace(const sMeshEntityEdge& edge, const sMeshEntityFace& face, bool bEdgeIsOrientedAlongFace)
	{
		uint32_t nNodeID1 = edge.m_nNodeIDs[0];
		uint32_t nNodeID2 = edge.m_nNodeIDs[1];

		for (uint32_t nCornerIndex = 0; nCornerIndex < 3; nCornerIndex++) {
			if (face.m_nNodeIDs[nCornerIndex] == nNodeID1) {
				if (face.m_nNodeIDs[(nCornerIndex + 1) % 3] == nNodeID2) {
					// Edge and face are oriented the same way.
					bEdgeIsOrientedAlongFace = true;
					return true;
				}

				if (face.m_nNodeIDs[(nCornerIndex + 2) % 3] == nNodeID2) {
					// Edge and face are oriented the opposite way.
					bEdgeIsOrientedAlongFace = false;
					return true;
				}

			}
		}

		// False means, edge is not part of face!
		bEdgeIsOrientedAlongFace = false;
		return false;

	}

	bool CMeshEntity::adjacentFacesAreConsistentlyOriented(const sMeshEntityEdge& edge)
	{
		sMeshEntityFace & face1 = getFace(edge.m_nFaceIDs[0]);
		sMeshEntityFace& face2 = getFace(edge.m_nFaceIDs[1]);

		bool bEdgeIsAlignedWithFace1 = false;
		bool bEdgeIsAlignedWithFace2 = false;

		if (!checkEdgeOrientationWithFace (edge, face1, bEdgeIsAlignedWithFace1))
			return false;
		if (!checkEdgeOrientationWithFace(edge, face2, bEdgeIsAlignedWithFace2))
			return false;

		// Orientation must be the exact opposite.
		return (bEdgeIsAlignedWithFace1 != bEdgeIsAlignedWithFace2);

	}

}


