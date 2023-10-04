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

#include "lib3mf/lib3mf_dynamic.hpp"

namespace AMC {

	typedef struct _sMeshEntityNode {
		float m_fCoordinates[3];
	} sMeshEntityNode;

	typedef struct _sMeshEntityEdge {
		uint32_t m_nID;
		uint32_t m_NodeIndices[2];		
		uint32_t m_nFlags;
	} sMeshEntityEdge;

	typedef struct _sMeshEntityFace {
		uint32_t m_nNodeIndices[3];
	} sMeshEntityFace;

	class CMeshEntity;
	typedef std::shared_ptr<CMeshEntity> PMeshEntity;

	class CMeshEntity {
	private:

		std::string m_sUUID;
		std::vector<sMeshEntityNode> m_Vertices;
		std::map<std::pair<uint32_t, uint32_t>, sMeshEntityEdge> m_Edges;
		std::vector<sMeshEntityFace> m_Faces;		

	public:

		CMeshEntity(const std::string & sUUID);

		virtual ~CMeshEntity();

		std::string getUUID();

		void IncRef();

		bool DecRef();

		void loadFrom3MF(Lib3MF::CLib3MFMeshObject * pMeshObject);

		size_t getVertexCount();
		size_t getEdgeCount();
		size_t getFaceCount();

		void getFaceVertices(uint32_t nFaceIndex, sMeshEntityNode & sNode1, sMeshEntityNode& sNode2, sMeshEntityNode& sNode3);

	};

	
}


#endif //__AMC_MESHENTITY

