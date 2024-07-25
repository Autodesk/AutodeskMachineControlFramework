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


Abstract: This is the class declaration of CModelDataComponentInstance

*/


#ifndef __LIBMCENV_MODELDATACOMPONENTINSTANCE
#define __LIBMCENV_MODELDATACOMPONENTINSTANCE

#include "libmcenv_interfaces.hpp"
#include "amc_resourcepackage.hpp"

// Parent classes
#include "libmcenv_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "lib3mf/lib3mf_dynamic.hpp"
#include "amc_meshhandler.hpp"

namespace LibMCEnv {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CModelDataComponentInstance 
**************************************************************************************************************************/

	
class CModelDataComponentInstance : public virtual IModelDataComponentInstance, public virtual CBase {
private:

	std::string m_sUUID;
	std::string m_sName;
	LibMCEnv::sModelDataTransform m_LocalTransform;
	LibMCEnv::sModelDataTransform m_ParentTransform;
	Lib3MF::PModel m_pModel;
	AMC::PMeshHandler m_pMeshHandler;

	std::vector<std::pair <Lib3MF::PMeshObject, LibMCEnv::sModelDataTransform>> m_Solids;
	std::vector<std::pair <std::pair<Lib3MF::PMeshObject, bool>, LibMCEnv::sModelDataTransform>> m_Supports;
	std::vector<std::pair <Lib3MF::PObject, LibMCEnv::sModelDataTransform>> m_SubComponents;

	void addSubObjectWithTransform (Lib3MF::PObject pSubObject, LibMCEnv::sModelDataTransform transform);
	void addBuildItem (Lib3MF::PBuildItem pBuildItem);

public:

	// Creates a Component instance for a specific Object
	CModelDataComponentInstance(Lib3MF::PModel pModel, Lib3MF::PObject p3MFObject, LibMCEnv::sModelDataTransform transform, AMC::PMeshHandler pMeshHandler);

	// Creates a Component instance for a specific Build Item
	CModelDataComponentInstance(Lib3MF::PModel pModel, Lib3MF::PBuildItem p3MFBuildItem, AMC::PMeshHandler pMeshHandler);

	// Creates a Component instance for the full model
	CModelDataComponentInstance(Lib3MF::PModel pModel, AMC::PMeshHandler pMeshHandler);

	// Creates a Component instance for a 3MF Resource Package
	CModelDataComponentInstance(Lib3MF::CWrapper* p3MFWrapper, AMC::CResourcePackage* pResourcePackage, const std::string sResourceName, AMC::PMeshHandler pMeshHandler);

	virtual ~CModelDataComponentInstance();

	std::string GetName() override;

	std::string GetUUID() override;

	LibMCEnv::sModelDataTransform GetLocalTransform() override;

	LibMCEnv::sModelDataTransform GetAbsoluteTransform() override;

	LibMCEnv_uint32 GetSolidCount() override;

	IModelDataMeshInstance * GetSolidMesh(const LibMCEnv_uint32 nIndex) override;

	LibMCEnv_uint32 GetSupportCount() override;

	IModelDataMeshInstance * GetSupportMesh(const LibMCEnv_uint32 nIndex) override;

	LibMCEnv_uint32 GetSubComponentCount() override;

	IModelDataComponentInstance * GetSubComponent(const LibMCEnv_uint32 nIndex) override;

};

} // namespace Impl
} // namespace LibMCEnv

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCENV_MODELDATACOMPONENTINSTANCE
