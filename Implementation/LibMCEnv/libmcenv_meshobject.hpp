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

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL AUTODESK INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


Abstract: This is the class declaration of CMeshObject

*/


#ifndef __LIBMCENV_MESHOBJECT
#define __LIBMCENV_MESHOBJECT

#include "libmcenv_interfaces.hpp"

// Parent classes
#include "libmcenv_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "amc_meshhandler.hpp"

namespace LibMCEnv {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CMeshObject 
**************************************************************************************************************************/

class CMeshObject : public virtual IMeshObject, public virtual CBase {
private:

    AMC::PMeshHandler m_pMeshHandler;
    std::string m_sMeshUUID;

    AMC::PMeshEntity getMeshEntity();

public:

    CMeshObject(AMC::PMeshHandler pMeshHandler, const std::string & sMeshUUID);

    virtual ~CMeshObject();

	std::string GetName() override;

	std::string GetUUID() override;

	LibMCEnv_uint32 GetTriangleCount() override;

	LibMCEnv_uint32 GetVertexCount() override;

	bool IsManifold() override;

	bool IsOriented() override;

	bool IsWatertight() override;

	LibMCEnv_uint32 GetMaxVertexID() override;

	bool VertexExists(const LibMCEnv_uint32 nVertexID) override;

	bool GetVertex(const LibMCEnv_uint32 nVertexID, LibMCEnv_double& dX, LibMCEnv_double& dY, LibMCEnv_double& dZ) override;

	void GetVertexIDs(LibMCEnv_uint64 nVertexIDsBufferSize, LibMCEnv_uint64* pVertexIDsNeededCount, LibMCEnv_uint32* pVertexIDsBuffer) override;

	void GetAllVertices(LibMCEnv_uint64 nVerticesBufferSize, LibMCEnv_uint64* pVerticesNeededCount, LibMCEnv::sMeshVertex3D* pVerticesBuffer) override;

	LibMCEnv_uint32 GetMaxTriangleID() override;

	bool TriangeExists(const LibMCEnv_uint32 nTriangleID) override;

	bool GetTriangle(const LibMCEnv_uint32 nTriangleID, LibMCEnv_uint32& nVertex1ID, LibMCEnv_uint32& nVertex2ID, LibMCEnv_uint32& nVertex3ID) override;

	void GetTriangleIDs(LibMCEnv_uint64 nTriangleIDsBufferSize, LibMCEnv_uint64* pTriangleIDsNeededCount, LibMCEnv_uint32* pTriangleIDsBuffer) override;

	void GetAllTriangles(LibMCEnv_uint64 nTrianglesBufferSize, LibMCEnv_uint64* pTrianglesNeededCount, LibMCEnv::sMeshTriangle3D* pTrianglesBuffer) override;

};

} // namespace Impl
} // namespace LibMCEnv

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCENV_MESHOBJECT
