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

#include "libmcdriver_marlin_driver_marlin.hpp"
#include "libmcdriver_marlin_interfaceexception.hpp"

// Include custom headers here.


using namespace LibMCDriver_Marlin::Impl;

/*************************************************************************************************************************
 Class definition of CDriver_Marlin 
**************************************************************************************************************************/

CDriver_Marlin::CDriver_Marlin(const std::string& sName, const std::string& sType)
	: CDriver (sName, sType)
{

}


void CDriver_Marlin::Connect(const std::string& sCOMPort, const LibMCDriver_Marlin_uint32 nBaudrate)
{
	Disconnect ();

	auto pSerialController = std::make_shared<AMC::CSerialController_Marlin>(false);
	pSerialController->setCOMPort(sCOMPort);
	pSerialController->setBaudrate(nBaudrate);
	pSerialController->initializeController();
	m_pSerialController = pSerialController;

}

void CDriver_Marlin::Disconnect()
{
	if (m_pSerialController.get() != nullptr)
		m_pSerialController->disconnectController();

	m_pSerialController = nullptr;
}

void CDriver_Marlin::SetAbsolutePositioning(const bool bAbsolute)
{
	if (m_pSerialController.get() == nullptr)
		throw ELibMCDriver_MarlinInterfaceException(LIBMCDRIVER_MARLIN_ERROR_NOTCONNECTED);

	if (bAbsolute) {
		m_pSerialController->setPositioningAbolute();
	}
	else {
		m_pSerialController->setPositioningRelative();
	}
}

void CDriver_Marlin::UpdateState() 
{
	if (m_pSerialController.get() == nullptr)
		throw ELibMCDriver_MarlinInterfaceException(LIBMCDRIVER_MARLIN_ERROR_NOTCONNECTED);

	m_pSerialController->queryPositionState();
	m_pSerialController->queryTemperatureState(0);
}

void CDriver_Marlin::GetCurrentPosition(LibMCDriver_Marlin_double& dX, LibMCDriver_Marlin_double& dY, LibMCDriver_Marlin_double& dZ)
{
	m_pSerialController->getCurrentPosition(dX, dY, dZ);
}

void CDriver_Marlin::GetTargetPosition(LibMCDriver_Marlin_double& dX, LibMCDriver_Marlin_double& dY, LibMCDriver_Marlin_double& dZ)
{
	m_pSerialController->getTargetPosition(dX, dY, dZ);
}

bool CDriver_Marlin::CanExecuteMovement()
{
	return m_pSerialController->canReceiveMovement();
}

bool CDriver_Marlin::IsMoving()
{
	return m_pSerialController->isMoving();
}

void CDriver_Marlin::MoveTo(const LibMCDriver_Marlin_double dX, const LibMCDriver_Marlin_double dY, const LibMCDriver_Marlin_double dZ, const LibMCDriver_Marlin_double dSpeed)
{
	m_pSerialController->move(dX, dY, dZ, dSpeed);
}

void CDriver_Marlin::MoveFastTo(const LibMCDriver_Marlin_double dX, const LibMCDriver_Marlin_double dY, const LibMCDriver_Marlin_double dZ, const LibMCDriver_Marlin_double dSpeed)
{
	m_pSerialController->moveFast(dX, dY, dZ, dSpeed);
}

void CDriver_Marlin::GetExtruderTemperature(const LibMCDriver_Marlin_uint32 nExtruderID, LibMCDriver_Marlin_double& dCurrentTemperature, LibMCDriver_Marlin_double& dTargetTemperature)
{
	m_pSerialController->getExtruderTemperature(nExtruderID, dTargetTemperature, dCurrentTemperature);
}