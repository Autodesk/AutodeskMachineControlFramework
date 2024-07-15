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


#ifndef __AMC_MESHHANDLER
#define __AMC_MESHHANDLER

#include <memory>
#include <map>
#include <string>

#include "amc_meshentity.hpp"
#include "amc_resourcepackage.hpp"
#include "libmcdata_dynamic.hpp"

namespace AMC {

	

	class CMeshHandler;
	typedef std::shared_ptr<CMeshHandler> PMeshHandler;

	class CMeshScene;
	typedef std::shared_ptr<CMeshScene> PMeshScene;

	class CMeshHandler {
	private:
	
		std::map<std::string, PMeshEntity> m_Entities;

		std::map<std::string, PMeshScene> m_Scenes;

	public:

		CMeshHandler();
		virtual ~CMeshHandler();

		bool hasMeshEntity(const std::string& sEntityUUID);
		PMeshEntity findMeshEntity(const std::string & sEntityUUID, bool bFailIfNotExistent);
		
		void unloadMeshEntity (const std::string& sEntityUUID);
		void unloadAllEntities();

		void registerEntity(PMeshEntity pMeshEntity);

		PMeshScene findScene(const std::string & sSceneUUID, bool bFailIfNotExistent);

		//PMeshEntity register3MFMesh(const std::string& sEntityUUID, Lib3MF::CLib3MFMeshObject* pMeshObject);

		//PMeshEntity register3MFResource(Lib3MF::CLib3MFWrapper * pWrapper, AMC::CResourcePackage * pResourcePackage, const std::string & sResourceName);
		
	};

	
}


#endif //__AMC_MESHHANDLER

