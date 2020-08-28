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


#ifndef __AMC_TOOLPATHHANDLER
#define __AMC_TOOLPATHHANDLER

#include <memory>
#include <map>
#include <string>

#include "amc_toolpathentity.hpp"
#include "libmcdata_dynamic.hpp"

namespace AMC {

	

	class CToolpathHandler;
	typedef std::shared_ptr<CToolpathHandler> PToolpathHandler;

	class CToolpathHandler {
	private:
		
		std::map<std::string, PToolpathEntity> m_Entities;

		std::string m_sLib3MFPath;
		Lib3MF::PWrapper m_pLib3MFWrapper;

		LibMCData::PStorage m_pStorage;
		LibMCData::PBuildJobHandler m_pBuildJobHandler;

	public:

		CToolpathHandler(LibMCData::PStorage pStorage, LibMCData::PBuildJobHandler pBuildJobHandler);
		virtual ~CToolpathHandler();

		CToolpathEntity * findToolpathEntity(const std::string & sStreamUUID, bool bFailIfNotExistent);
		CToolpathEntity* loadToolpathEntity(const std::string& sStreamUUID);
		
		void unloadToolpathEntity (const std::string& sStreamUUID);
		void unloadAllEntities();

		void setLibraryPath(const std::string& sLibraryName, const std::string sLibraryPath);

		Lib3MF::PWrapper getLib3MFWrapper();
		
	};

	
}


#endif //__AMC_TOOLPATHHANDLER

