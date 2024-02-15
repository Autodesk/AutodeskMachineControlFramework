/*++

Copyright (C) 2024 ASL Inc.

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


Abstract: This is the class declaration of CDriverContext

*/


#ifndef __LIBMCDRIVER_ASL_DRIVERCONTEXTINSTANCE
#define __LIBMCDRIVER_ASL_DRIVERCONTEXTINSTANCE

#include "libmcdriver_asl_interfaces.hpp"

#include "serial/serial.h"

#include <memory>

namespace LibMCDriver_ASL {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CDriverContextInstance
**************************************************************************************************************************/

class CDriverContextInstance {
private:

	std::string m_sIdentifier;
	
	std::string m_sCOMPort;
	
	std::unique_ptr<serial::Serial> m_pConnection;
	
	LibMCEnv::PDriverEnvironment m_pDriverEnvironment;

	double m_dSetTemperatures[4];
	double m_dCurrentTemperatures[4];
	double m_dPrintCounts[4];
	double m_dImageLength[4];
	double m_dStates[4];

		
public:

	CDriverContextInstance (const std::string & sIdentifier, const std::string & sCOMPort, LibMCEnv::PDriverEnvironment pDriverEnvironment);
	
	virtual ~CDriverContextInstance ();

	std::string GetSerialNumber();

	void SetPower(const bool bPower);

	void SetPrintheadMode(const LibMCDriver_ASL::eBoardMode eMode);

	void SetFrequency(const LibMCDriver_ASL_uint32 nFrequency);

	void SetTemperature(const LibMCDriver_ASL_uint8 nIndex, const LibMCDriver_ASL_double dTemperature);

	void HomeLocation();

	void SetPrintStart(const LibMCDriver_ASL_uint32 nStartLocation);

	void SendImage(LibMCEnv::PImageData pImageObject);

	void Poll();

	LibMCDriver_ASL_double GetTemperature(const LibMCDriver_ASL_uint8 nIndex);

	LibMCDriver_ASL_double GetPrintCounts(const LibMCDriver_ASL_uint8 nIndex);

	LibMCDriver_ASL_double GetImageLength(const LibMCDriver_ASL_uint8 nIndex);

	LibMCDriver_ASL_double GetHeadState(const LibMCDriver_ASL_uint8 nIndex);

	bool IsHeating(const LibMCDriver_ASL_uint8 nIndex);

	bool GetPower();

};

} // namespace Impl
} // namespace LibMCDriver_ASL

#endif // __LIBMCDRIVER_ASL_DRIVERCONTEXTINSTANCE
