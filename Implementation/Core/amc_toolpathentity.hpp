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


#ifndef __AMC_TOOLPATHENTITY
#define __AMC_TOOLPATHENTITY

#include <memory>
#include <string>
#include <thread>
#include <mutex>

#include "amc_toolpathlayerdata.hpp"
#include "lib3mf/lib3mf_dynamic.hpp"
#include "libmcdata_dynamic.hpp"


#define AMC_TOOLPATH_MAXREFCOUNT (1024 * 1024 * 1024)

namespace AMC {

	class CToolpathEntity;
	typedef std::shared_ptr<CToolpathEntity> PToolpathEntity;

	class CToolpathEntity {
	private:		
		uint32_t m_ReferenceCount;

		std::mutex m_Mutex;
		LibMCData::PStorageStream m_pStorageStream;
		Lib3MF::PModel m_p3MFModel;
		Lib3MF::PReader m_p3MFReader;
		Lib3MF::PToolpath m_pToolpath;

	public:

		CToolpathEntity(LibMCData::PStorageStream pStorageStream, Lib3MF::PWrapper p3MFWrapper);
		virtual ~CToolpathEntity();		

		void IncRef();
		bool DecRef();

		uint32_t getLayerCount();	

		PToolpathLayerData readLayer(uint32_t nLayerIndex);

		double getUnits();

	};

	
}


#endif //__AMC_TOOLPATHENTITY

