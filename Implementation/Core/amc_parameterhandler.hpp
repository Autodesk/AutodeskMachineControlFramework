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

#ifndef __AMC_PARAMETERHANDLER
#define __AMC_PARAMETERHANDLER

#include "amc_parametergroup.hpp"

#include <memory>
#include <vector>
#include <map>
#include <string>
#include <mutex>

namespace AMC {

	class CParameterHandler;
	typedef std::shared_ptr<CParameterHandler> PParameterHandler;

	class CParameterHandler {
	private:

		PParameterGroup m_DataStore;
		std::map<std::string, PParameterGroup> m_Groups;
		std::vector<PParameterGroup> m_GroupList;

		std::mutex m_Mutex;
		
	public:

		CParameterHandler();
		
		virtual ~CParameterHandler();		
		
		bool hasGroup (const std::string & sName);
		void addGroup (PParameterGroup pGroup);
		PParameterGroup addGroup(const std::string& sName, const std::string& sDescription);

		uint32_t getGroupCount();
		CParameterGroup* getGroup(const uint32_t nIndex);
		CParameterGroup* findGroup(const std::string& sName, const bool bFailIfNotExisting);

		CParameterGroup * getDataStore ();

	};

	
}


#endif //__AMC_PARAMETERHANDLER

