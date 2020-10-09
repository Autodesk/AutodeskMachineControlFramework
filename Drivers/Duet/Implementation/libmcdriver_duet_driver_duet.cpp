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

void CDriver_Duet::Connect(const std::string & sCOMPort, const LibMCDriver_Duet_uint32 nBaudrate, const LibMCDriver_Duet_double dStatusUpdateInterval, const LibMCDriver_Duet_uint32 nConnectTimeout)
{
	throw ELibMCDriver_DuetInterfaceException(LIBMCDRIVER_DUET_ERROR_NOTIMPLEMENTED);
}

void CDriver_Duet::Disconnect()
{
	throw ELibMCDriver_DuetInterfaceException(LIBMCDRIVER_DUET_ERROR_NOTIMPLEMENTED);
}

void CDriver_Duet::SetAbsolutePositioning(const bool bAbsolute)
{
	throw ELibMCDriver_DuetInterfaceException(LIBMCDRIVER_DUET_ERROR_NOTIMPLEMENTED);
}

void CDriver_Duet::UpdatePositionState()
{
	throw ELibMCDriver_DuetInterfaceException(LIBMCDRIVER_DUET_ERROR_NOTIMPLEMENTED);
}

void CDriver_Duet::GetCurrentPosition(LibMCDriver_Duet_double & dX, LibMCDriver_Duet_double & dY, LibMCDriver_Duet_double & dZ)
{
	throw ELibMCDriver_DuetInterfaceException(LIBMCDRIVER_DUET_ERROR_NOTIMPLEMENTED);
}

void CDriver_Duet::GetTargetPosition(LibMCDriver_Duet_double & dX, LibMCDriver_Duet_double & dY, LibMCDriver_Duet_double & dZ)
{
	throw ELibMCDriver_DuetInterfaceException(LIBMCDRIVER_DUET_ERROR_NOTIMPLEMENTED);
}

bool CDriver_Duet::CanExecuteMovement()
{
	throw ELibMCDriver_DuetInterfaceException(LIBMCDRIVER_DUET_ERROR_NOTIMPLEMENTED);
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

void CDriver_Duet::MoveToXY(const LibMCDriver_Duet_double dX, const LibMCDriver_Duet_double dY, const LibMCDriver_Duet_double dE, const LibMCDriver_Duet_double dSpeed)
{
	throw ELibMCDriver_DuetInterfaceException(LIBMCDRIVER_DUET_ERROR_NOTIMPLEMENTED);
}

void CDriver_Duet::MoveFastToXY(const LibMCDriver_Duet_double dX, const LibMCDriver_Duet_double dY, const LibMCDriver_Duet_double dSpeed)
{
	throw ELibMCDriver_DuetInterfaceException(LIBMCDRIVER_DUET_ERROR_NOTIMPLEMENTED);
}

void CDriver_Duet::MoveToZ(const LibMCDriver_Duet_double dZ, const LibMCDriver_Duet_double dSpeed)
{
	throw ELibMCDriver_DuetInterfaceException(LIBMCDRIVER_DUET_ERROR_NOTIMPLEMENTED);
}

void CDriver_Duet::MoveFastToZ(const LibMCDriver_Duet_double dZ, const LibMCDriver_Duet_double dSpeed)
{
	throw ELibMCDriver_DuetInterfaceException(LIBMCDRIVER_DUET_ERROR_NOTIMPLEMENTED);
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

