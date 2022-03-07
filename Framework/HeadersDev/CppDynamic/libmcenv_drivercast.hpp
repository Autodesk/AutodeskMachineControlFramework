
/*++

Copyright (C) 2020 Autodesk Inc.

All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/


#ifndef __LIBMCENV_DRIVERCAST
#define __LIBMCENV_DRIVERCAST

namespace LibMCEnv {

	template <class DriverClass, class WrapperClass> class CDriverCast {
	private:		
		std::shared_ptr<WrapperClass> m_pWrapper;
	public:

		std::shared_ptr<DriverClass> acquireDriver(LibMCEnv::PStateEnvironment pStateEnvironment, const std::string & sDriverName) {

			if (pStateEnvironment.get() == nullptr)
				throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);

			// Get the global Driver wrapper, if not cached yet.
			if (m_pWrapper.get() == nullptr) {
				std::string sDriverType;
				LibMCEnv_pvoid pDriverLookup;

				pStateEnvironment->GetDriverLibrary(sDriverName, sDriverType, pDriverLookup);
				m_pWrapper = WrapperClass::loadLibraryFromSymbolLookupMethod(pDriverLookup);
			}

			// "Cast" Driver to DriverClass
			LibMCEnv_pvoid pDriverHandle;
			pStateEnvironment->CreateDriverAccess(sDriverName, pDriverHandle);
			std::shared_ptr<DriverClass> pDriver = std::make_shared<DriverClass>(m_pWrapper.get(), pDriverHandle);

			return pDriver;
		}

	};

}

#endif //__LIBMCENV_DRIVERCAST