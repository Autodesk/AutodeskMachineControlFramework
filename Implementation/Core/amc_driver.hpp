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


#ifndef __AMC_DRIVER
#define __AMC_DRIVER

#include <memory>
#include <vector>
#include <string>

#ifndef _AMC_DRIVER_HEADERPROTECTION
#error Never include amc_driver.hpp from outside of amc_driver.cpp and amc_driverhandler.cpp
#endif

#include "amc_parametergroup.hpp"
#include "libmcdriver_dynamic.hpp" 

namespace AMC {

	typedef void* HDriverHandle;
	typedef void* HSymbolLookupHandle;

	class CDriver {
	protected:
		std::string m_sName;
		std::string m_sType;
		std::string m_sLibrary;	

		// If not empty, a state instance has reserved the use of this driver
		std::string m_sReservationInstance;

		LibMCDriver::PWrapper m_pDriverWrapper;
		LibMCDriver::PDriver m_pDriverInstance;

		PParameterGroup m_pParameterGroup;
		
	public:

		CDriver(const std::string & sName, const std::string & sType, const std::string & sLibrary, PParameterGroup pParameterGroup, LibMCEnv::PDriverEnvironment pDriverEnvironment);

		virtual ~CDriver();

		std::string getName();
		std::string getType();

		HSymbolLookupHandle getSymbolLookup();

		HDriverHandle acquireDriverHandle(const std::string & sInstanceName);
		void releaseDriverHandle(const std::string& sInstanceName);

		PParameterGroup getParameterGroup();
				
	};
	
	typedef std::shared_ptr<CDriver> PDriver;
	
}


#endif //__AMC_DRIVER

