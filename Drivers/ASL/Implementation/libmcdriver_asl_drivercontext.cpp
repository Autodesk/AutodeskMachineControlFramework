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


Abstract: This is a stub class definition of CDriverContext

*/

#include "libmcdriver_asl_drivercontext.hpp"
#include "libmcdriver_asl_interfaceexception.hpp"

// Include custom headers here.


using namespace LibMCDriver_ASL::Impl;

/*************************************************************************************************************************
 Class definition of CDriverContext 
**************************************************************************************************************************/

CDriverContext::CDriverContext(std::shared_ptr<CDriverContextInstance> pInstance)
	: m_pInstance (pInstance)
{
	if (pInstance.get () == nullptr)
		throw ELibMCDriver_ASLInterfaceException(LIBMCDRIVER_ASL_ERROR_INVALIDPARAM);
}

CDriverContext::~CDriverContext()
{

}

std::string CDriverContext::GetSerialNumber()
{
	return m_pInstance->GetSerialNumber();
}

void CDriverContext::SetPower(const bool bPower)
{
	m_pInstance->SetPower(bPower);
}

void CDriverContext::SetPrintheadMode(const LibMCDriver_ASL::eBoardMode eMode)
{
	m_pInstance->SetPrintheadMode(eMode);
}

void CDriverContext::SetFrequency(const LibMCDriver_ASL_uint32 nFrequency)
{
	m_pInstance->SetFrequency(nFrequency);
}

void CDriverContext::SetTemperature(const LibMCDriver_ASL_uint8 nIndex, const LibMCDriver_ASL_double dTemperature)
{
	m_pInstance->SetTemperature(nIndex, dTemperature);
}

void CDriverContext::HomeLocation()
{
	throw ELibMCDriver_ASLInterfaceException(LIBMCDRIVER_ASL_ERROR_NOTIMPLEMENTED);
}

void CDriverContext::SetPrintStart(const LibMCDriver_ASL_uint32 nStartLocation)
{
	m_pInstance->SetPrintStart(nStartLocation);
}

void CDriverContext::SendImage(const LibMCDriver_ASL_uint8 nIndex, const LibMCDriver_ASL_uint32 nPadding, LibMCEnv::PImageData pImageObject)
{
	m_pInstance->SendImage(nIndex, nPadding, pImageObject);
}

void CDriverContext::Poll()
{
	m_pInstance->Poll();
}

LibMCDriver_ASL_double CDriverContext::GetTemperature(const LibMCDriver_ASL_uint8 nIndex, const bool bSet)
{
	return m_pInstance->GetTemperature(nIndex, bSet);
}

LibMCDriver_ASL_double CDriverContext::GetPrintCounts(const LibMCDriver_ASL_uint8 nIndex)
{
	return m_pInstance->GetPrintCounts(nIndex);
}

LibMCDriver_ASL_double CDriverContext::GetImageLength(const LibMCDriver_ASL_uint8 nIndex)
{
	return m_pInstance->GetImageLength(nIndex);
}

LibMCDriver_ASL_double CDriverContext::GetHeadState(const LibMCDriver_ASL_uint8 nIndex)
{
	return m_pInstance->GetHeadState(nIndex);
}

bool CDriverContext::IsHeating(const LibMCDriver_ASL_uint8 nIndex)
{
	return m_pInstance->IsHeating(nIndex);
}

bool CDriverContext::GetPower()
{
	return m_pInstance->GetPower();
}

LibMCDriver_ASL_uint32 CDriverContext::GetHeadTimeOn()
{
	return m_pInstance->GetHeadTimeOn();
}

bool CDriverContext::VerifyImages()
{
	return m_pInstance->VerifyImages();
}