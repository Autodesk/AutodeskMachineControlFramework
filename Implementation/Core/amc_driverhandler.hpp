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


#ifndef __AMC_DRIVERHANDLER
#define __AMC_DRIVERHANDLER

#include <map>
#include <list>
#include <string>
#include <thread>
#include <mutex>

#include "libmcenv_dynamic.hpp"

namespace AMC {

	typedef void* HDriverHandle;
	typedef void* HSymbolLookupHandle;

	class CDriver;
	typedef std::shared_ptr<CDriver> PDriver;

	class CDriverHandler {
	private:

		LibMCEnv::PWrapper m_pEnvironmentWrapper;
		std::string m_sTempBasePath;

		// List and Map of registered drivers
		std::list<PDriver> m_DriverList;
		std::map<std::string, PDriver> m_DriverMap;

		// Mutex for safe Multi-Thread-Handling
		std::mutex m_Mutex;

		CDriver* findDriver(const std::string& sName, bool bFailIfNotExisting);

	public:

		CDriverHandler(LibMCEnv::PWrapper pEnvironmentWrapper);

		virtual ~CDriverHandler();

		void registerDriver(const std::string& sName, const std::string& sType, const std::string& sLibraryPath, const std::string & sResourcePath);

		void GetDriverInformation (const std::string& sName, std::string& sType, HSymbolLookupHandle & pSymbolLookup);

		HDriverHandle acquireDriver (const std::string& sName, const std::string& sInstanceName);

		PParameterGroup getDriverParameterGroup(const std::string& sName);

		void releaseDriverLocks (const std::string& sInstanceName);

		void setTempBasePath (const std::string& sTempBasePath);
				
	};
	
	typedef std::shared_ptr<CDriverHandler> PDriverHandler;

	
}


#endif //__AMC_DRIVERHANDLER

