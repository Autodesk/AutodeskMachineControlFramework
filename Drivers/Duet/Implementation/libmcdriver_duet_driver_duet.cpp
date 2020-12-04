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


Abstract: This is a stub class definition of CDriver_Duet

*/

#include "libmcdriver_duet_driver_duet.hpp"
#include "libmcdriver_duet_interfaceexception.hpp"

// Include custom headers here.


using namespace LibMCDriver_Duet::Impl;

/*************************************************************************************************************************
 Class definition of CDriver_Duet
**************************************************************************************************************************/
CDriver_Duet::CDriver_Duet(const std::string& sName, const std::string& sType, LibMCEnv::PDriverEnvironment pDriverEnvironment)
	: CDriver(sName, sType), m_pDriverEnvironment (pDriverEnvironment)
{
	if (pDriverEnvironment.get() == nullptr)
		throw ELibMCDriver_DuetInterfaceException(LIBMCDRIVER_DUET_ERROR_INVALIDPARAM);

	pDriverEnvironment->RegisterDoubleParameter("targetx", "Target X Position", 0.0);
	pDriverEnvironment->RegisterDoubleParameter("targety", "Target Y Position", 0.0);
	pDriverEnvironment->RegisterDoubleParameter("targetz", "Target Z Position", 0.0);
	pDriverEnvironment->RegisterDoubleParameter("targeta", "Target A Position", 0.0);
	pDriverEnvironment->RegisterDoubleParameter("targetb", "Target B Position", 0.0);
	pDriverEnvironment->RegisterDoubleParameter("currentx", "X Position", 0.0);
	pDriverEnvironment->RegisterDoubleParameter("currenty", "Y Position", 0.0);
	pDriverEnvironment->RegisterDoubleParameter("currentz", "Z Position", 0.0);
	pDriverEnvironment->RegisterDoubleParameter("currenta", "A Position", 0.0);
	pDriverEnvironment->RegisterDoubleParameter("currentb", "B Position", 0.0);
	pDriverEnvironment->RegisterBoolParameter("ismoving", "Moving", false);
	pDriverEnvironment->RegisterBoolParameter("ishomed", "Homed", false);
	pDriverEnvironment->RegisterBoolParameter("isconnected", "Connected", false);
}


void CDriver_Duet::Connect(const std::string & sCOMPort, const LibMCDriver_Duet_uint32 nBaudrate, const LibMCDriver_Duet_double dStatusUpdateInterval, const LibMCDriver_Duet_uint32 nConnectTimeout)
{
	Disconnect ();

	auto pSerialController = std::make_shared<AMC::CSerialController_Duet>();
	pSerialController->setCOMPort(sCOMPort);
	pSerialController->setBaudrate(nBaudrate);
	pSerialController->setConnectTimeout(nConnectTimeout);
	pSerialController->setStatusUpdateTimerInterval(dStatusUpdateInterval);
	pSerialController->initializeController();
	m_pSerialController = pSerialController;
}

void CDriver_Duet::Disconnect()
{
	if (m_pSerialController.get() != nullptr)
		m_pSerialController->disconnectController();

	m_pSerialController = nullptr;
}

void CDriver_Duet::SetAbsolutePositioning(const bool bAbsolute)
{
	if (bAbsolute) {
		m_pSerialController->setPositioningAbsolute();
	} else {
		m_pSerialController->setPositioningRelative();
	}
}

void CDriver_Duet::QueryParameters()
{
	// m_pSerialController->queryPrinterState();

	// double dX, dY, dZ, dA, dB;
	// m_pSerialController->getTargetPosition(dX, dY, dZ, dA, dB);
	// m_pDriverEnvironment->SetDoubleParameter("targetx", 0.0);
	// m_pDriverEnvironment->SetDoubleParameter("targety", dY);
	// m_pDriverEnvironment->SetDoubleParameter("targetz", dZ);
	// m_pDriverEnvironment->SetDoubleParameter("targeta", dA);
	// m_pDriverEnvironment->SetDoubleParameter("targetb", dB);

	// m_pSerialController->getCurrentPosition(dX, dY, dZ, dA, dB);
	// m_pDriverEnvironment->SetDoubleParameter("currentx", dX);
	// m_pDriverEnvironment->SetDoubleParameter("currenty", dY);
	// m_pDriverEnvironment->SetDoubleParameter("currentz", dZ);
	// m_pDriverEnvironment->SetDoubleParameter("currenta", dA);
	// m_pDriverEnvironment->SetDoubleParameter("currentb", dB);

	// pDriverEnvironment->SetBoolParameter("ismoving", false);
	// pDriverEnvironment->SetBoolParameter("ishomed", false);
}

void CDriver_Duet::GetTargetPosition(LibMCDriver_Duet_double & dX, LibMCDriver_Duet_double & dY, LibMCDriver_Duet_double & dZ, LibMCDriver_Duet_double & dA, LibMCDriver_Duet_double & dB)
{
	throw ELibMCDriver_DuetInterfaceException(LIBMCDRIVER_DUET_ERROR_NOTIMPLEMENTED);
}

void CDriver_Duet::GetCurrentPosition(LibMCDriver_Duet_double & dX, LibMCDriver_Duet_double & dY, LibMCDriver_Duet_double & dZ, LibMCDriver_Duet_double & dA, LibMCDriver_Duet_double & dB)
{
	throw ELibMCDriver_DuetInterfaceException(LIBMCDRIVER_DUET_ERROR_NOTIMPLEMENTED);
}

bool CDriver_Duet::CanExecuteMovement()
{
	return m_pSerialController->canReceiveMovement();
}

bool CDriver_Duet::IsMoving()
{
	throw ELibMCDriver_DuetInterfaceException(LIBMCDRIVER_DUET_ERROR_NOTIMPLEMENTED);
}

bool CDriver_Duet::IsHomed()
{
	throw ELibMCDriver_DuetInterfaceException(LIBMCDRIVER_DUET_ERROR_NOTIMPLEMENTED);
}

bool CDriver_Duet::IsConnected()
{
	throw ELibMCDriver_DuetInterfaceException(LIBMCDRIVER_DUET_ERROR_NOTIMPLEMENTED);
}

void CDriver_Duet::MoveToXY(const LibMCDriver_Duet_double dX, const LibMCDriver_Duet_double dY, const LibMCDriver_Duet_double dLaserPower, const LibMCDriver_Duet_double dSpeed)
{
	if (m_pSerialController.get() == nullptr)
		throw ELibMCDriver_DuetInterfaceException(LIBMCDRIVER_DUET_ERROR_NOTCONNECTED);

	m_pSerialController->moveXY(dX, dY, dLaserPower, dSpeed);
}

void CDriver_Duet::MoveFastToXY(const LibMCDriver_Duet_double dX, const LibMCDriver_Duet_double dY, const LibMCDriver_Duet_double dSpeed)
{
	if (m_pSerialController.get() == nullptr)
		throw ELibMCDriver_DuetInterfaceException(LIBMCDRIVER_DUET_ERROR_NOTCONNECTED);

	m_pSerialController->moveFastXY(dX, dY, dSpeed);
}

void CDriver_Duet::MoveToZ(const LibMCDriver_Duet_double dZ, const LibMCDriver_Duet_double dSpeed)
{
	if (m_pSerialController.get() == nullptr)
		throw ELibMCDriver_DuetInterfaceException(LIBMCDRIVER_DUET_ERROR_NOTCONNECTED);

	m_pSerialController->moveZ(dZ, dSpeed);
}

void CDriver_Duet::MoveToA(const LibMCDriver_Duet_double dA, const LibMCDriver_Duet_double dSpeed)
{
	if (m_pSerialController.get() == nullptr)
		throw ELibMCDriver_DuetInterfaceException(LIBMCDRIVER_DUET_ERROR_NOTCONNECTED);

	m_pSerialController->moveA(dA, dSpeed);
}

void CDriver_Duet::MoveToB(const LibMCDriver_Duet_double dB, const LibMCDriver_Duet_double dSpeed)
{
	if (m_pSerialController.get() == nullptr)
		throw ELibMCDriver_DuetInterfaceException(LIBMCDRIVER_DUET_ERROR_NOTCONNECTED);

	m_pSerialController->moveB(dB, dSpeed);
}

void CDriver_Duet::StartHoming()
{
	throw ELibMCDriver_DuetInterfaceException(LIBMCDRIVER_DUET_ERROR_NOTIMPLEMENTED);
}

void CDriver_Duet::EmergencyStop()
{
	throw ELibMCDriver_DuetInterfaceException(LIBMCDRIVER_DUET_ERROR_NOTIMPLEMENTED);
}

void CDriver_Duet::SetAxisPosition(const std::string & sAxis, const LibMCDriver_Duet_double dValue)
{
	throw ELibMCDriver_DuetInterfaceException(LIBMCDRIVER_DUET_ERROR_NOTIMPLEMENTED);
}

void CDriver_Duet::PowerOff()
{
	throw ELibMCDriver_DuetInterfaceException(LIBMCDRIVER_DUET_ERROR_NOTIMPLEMENTED);
}

