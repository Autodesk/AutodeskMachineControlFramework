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


Abstract: This is the class declaration of CDriver_Duet

*/


#ifndef __LIBMCDRIVER_DUET_DRIVER_DUET
#define __LIBMCDRIVER_DUET_DRIVER_DUET

#include "libmcdriver_duet_interfaces.hpp"

// Parent classes
#include "libmcdriver_duet_driver.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "AMC_SerialController_Duet.hpp"


namespace LibMCDriver_Duet {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CDriver_Duet
**************************************************************************************************************************/

class CDriver_Duet : public virtual IDriver_Duet, public virtual CDriver {
private:

	/**
	* Put private members here.
	*/

protected:

	AMC::PSerialController m_pSerialController;
	LibMCEnv::PDriverEnvironment m_pDriverEnvironment;

public:

	CDriver_Duet(const std::string& sName, const std::string& sType, LibMCEnv::PDriverEnvironment pDriverEnvironment);

	/**
	* Put additional public members here. They will not be visible in the external API.
	*/


	/**
	* Public member functions to implement.
	*/

	void Configure(const std::string& sConfigurationString) override;

	void Connect(const std::string & sCOMPort, const LibMCDriver_Duet_uint32 nBaudrate, const LibMCDriver_Duet_double dStatusUpdateInterval, const LibMCDriver_Duet_uint32 nConnectTimeout) override;

	void Disconnect() override;

	void SetAbsolutePositioning(const bool bAbsolute) override;

	void QueryParameters() override;

	void GetTargetPosition(LibMCDriver_Duet_double & dX, LibMCDriver_Duet_double & dY, LibMCDriver_Duet_double & dZ, LibMCDriver_Duet_double & dA, LibMCDriver_Duet_double & dB) override;

	void GetCurrentPosition(LibMCDriver_Duet_double & dX, LibMCDriver_Duet_double & dY, LibMCDriver_Duet_double & dZ, LibMCDriver_Duet_double & dA, LibMCDriver_Duet_double & dB) override;

	bool CanExecuteMovement() override;

	bool IsMoving() override;

	bool IsHomed() override;

	bool IsConnected() override;

	void MoveToXY(const LibMCDriver_Duet_double dX, const LibMCDriver_Duet_double dY, const LibMCDriver_Duet_double dLaserPower, const LibMCDriver_Duet_double dSpeed) override;

	void MoveFastToXY(const LibMCDriver_Duet_double dX, const LibMCDriver_Duet_double dY, const LibMCDriver_Duet_double dSpeed) override;

	void MoveToZ(const LibMCDriver_Duet_double dZ, const LibMCDriver_Duet_double dSpeed) override;

	void MoveToA(const LibMCDriver_Duet_double dA, const LibMCDriver_Duet_double dSpeed) override;

	void MoveToB(const LibMCDriver_Duet_double dB, const LibMCDriver_Duet_double dSpeed) override;

	void StartHoming() override;

	void EmergencyStop() override;

	void SetAxisPosition(const std::string & sAxis, const LibMCDriver_Duet_double dValue) override;

	void PowerOff() override;

};

} // namespace Impl
} // namespace LibMCDriver_Duet

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_DUET_DRIVER_DUET
