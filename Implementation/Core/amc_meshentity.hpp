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


#ifndef __AMC_MESHENTITY
#define __AMC_MESHENTITY

#include <memory>
#include <map>
#include <string>
#include <cstdint>
#include <vector>
#include "amc_resourcepackage.hpp"
#include "amc_geometryutils.hpp"
#include "libmcenv_types.hpp"

#include "lib3mf/lib3mf_dynamic.hpp"

namespace AMC {

	typedef struct _sMeshEntityNode {
		uint32_t m_nNodeID;
		float m_fCoordinates[3];
	} sMeshEntityNode;

	typedef struct _sMeshEntityEdge {
		uint32_t m_nEdgeID;
		uint32_t m_nNodeIDs[2];		
		uint32_t m_nFaceIDs[2];
		uint32_t m_nAngleInDegrees;
		uint32_t m_nValence;
		uint32_t m_nFlags;
	} sMeshEntityEdge;

	typedef struct _sMeshEntityFace {
		uint32_t m_nFaceID;
		uint32_t m_nNodeIDs[3];
	} sMeshEntityFace;

	

	class CMeshEntity;
	typedef std::shared_ptr<CMeshEntity> PMeshEntity;

	class CMeshEntity {
	private:

		std::string m_sUUID;
		std::string m_sName;
		std::vector<sMeshEntityNode> m_Nodes;
		std::vector<sMeshEntityEdge> m_Edges;
		std::vector<sMeshEntityFace> m_Faces;		

		bool checkEdgeOrientationWithFace(const sMeshEntityEdge& edge, const sMeshEntityFace& face, bool bEdgeIsOrientedAlongFace);
		bool adjacentFacesAreConsistentlyOriented(const sMeshEntityEdge & edge);

	public:

		CMeshEntity(const std::string & sUUID, const std::string & sName);

		virtual ~CMeshEntity();

		std::string getUUID();

		std::string getName();

		void loadFrom3MF(Lib3MF::CLib3MFMeshObject * pMeshObject);

		void loadFrom3MFResource(Lib3MF::CWrapper * p3MFWrapper, AMC::CResourcePackage* pResourcePackage, const std::string sResourceName);

		size_t getNodeCount();
		size_t getEdgeCount();
		size_t getFaceCount();

		sMeshEntityNode& getNode(size_t nNodeID);
		sMeshEntityEdge& getEdge(size_t nEdgeID);
		sMeshEntityFace& getFace(size_t nFaceID);

		void getFaceNodes(size_t nFaceID, sMeshEntityNode & sNode1, sMeshEntityNode& sNode2, sMeshEntityNode& sNode3);
		void getEdgeNodes(size_t nEdgeID, sMeshEntityNode& sNode1, sMeshEntityNode& sNode2);

		double calcFaceAngleInDegree (size_t nFaceID1, size_t nFaceID2);
		sMeshVector3D calcFaceNormal(size_t nFaceID);

		bool isManifold();

		bool isOriented();

		uint32_t getMaxVertexID();

		bool vertexExists(const uint32_t nVertexID);

		bool getVertex(const uint32_t nVertexID, double& dX, double& dY, double& dZ);

		void getVertexIDs(uint64_t nVertexIDsBufferSize, uint64_t* pVertexIDsNeededCount, uint32_t* pVertexIDsBuffer);

		void getAllVertices(uint64_t nVerticesBufferSize, uint64_t* pVerticesNeededCount, LibMCEnv::sMeshVertex3D* pVerticesBuffer);

		uint32_t getMaxTriangleID();

		bool triangeExists(const uint32_t nTriangleID);

		bool getTriangle(const uint32_t nTriangleID, uint32_t& nVertex1ID, uint32_t& nVertex2ID, uint32_t& nVertex3ID);

		void getTriangleIDs(uint64_t nTriangleIDsBufferSize, uint64_t* pTriangleIDsNeededCount, uint32_t* pTriangleIDsBuffer);

		void getAllTriangles(uint64_t nTrianglesBufferSize, uint64_t* pTrianglesNeededCount, LibMCEnv::sMeshTriangle3D* pTrianglesBuffer);


	};

	
}


#endif //__AMC_MESHENTITY

