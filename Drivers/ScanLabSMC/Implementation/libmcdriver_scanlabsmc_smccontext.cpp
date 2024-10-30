/*++

Copyright (C) 2023 Autodesk Inc.

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


Abstract: This is a stub class definition of CSMCContext

*/

#include "libmcdriver_scanlabsmc_smccontext.hpp"
#include "libmcdriver_scanlabsmc_smcconfiguration.hpp"
#include "libmcdriver_scanlabsmc_interfaceexception.hpp"
#include "libmcdriver_scanlabsmc_smcjob.hpp"
#include "libmcdriver_scanlabsmc_smcjobinstance.hpp"

// Include custom headers here.


using namespace LibMCDriver_ScanLabSMC::Impl;

/*************************************************************************************************************************
 Class definition of CSMCContext 
**************************************************************************************************************************/

CSMCContext::CSMCContext(PSMCContextInstance pContextInstance, LibMCEnv::PDriverEnvironment pDriverEnvironment)
	: m_pContextInstance(pContextInstance), m_pDriverEnvironment (pDriverEnvironment)
{
	if (pContextInstance.get() == nullptr)
		throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPARAM);
	if (pDriverEnvironment.get () == nullptr)
		throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPARAM);
}

CSMCContext::~CSMCContext()
{
	m_pContextInstance = nullptr;
}


void CSMCContext::SetToSimulationMode()
{
	m_pContextInstance->SetToSimulationMode();
}

bool CSMCContext::IsSimulationMode()
{
	return m_pContextInstance->IsSimulationMode();
}


void CSMCContext::ReinitializeInstance()
{
	m_pContextInstance->ReinitializeInstance();
}

std::string CSMCContext::GetIPAddress()
{
	return m_pContextInstance->GetIPAddress();
}

std::string CSMCContext::GetNetmask()
{
	return m_pContextInstance->GetNetmask();
}

LibMCDriver_ScanLabSMC_uint32 CSMCContext::GetSerialNumber()
{
	return m_pContextInstance->GetSerialNumber();
}

LibMCDriver_ScanLabSMC_uint32 CSMCContext::GetLaserIndex()
{
	return m_pContextInstance->GetLaserIndex();
}

void CSMCContext::SetLaserOrigin(const LibMCDriver_ScanLabSMC_double dOriginX, const LibMCDriver_ScanLabSMC_double dOriginY)
{
}

void CSMCContext::GetLaserOrigin(LibMCDriver_ScanLabSMC_double& dOriginX, LibMCDriver_ScanLabSMC_double& dOriginY)
{
	dOriginX = 0.0;
	dOriginY = 0.0;
}

void CSMCContext::SetLaserField(const LibMCDriver_ScanLabSMC_double dMinX, const LibMCDriver_ScanLabSMC_double dMinY, const LibMCDriver_ScanLabSMC_double dMaxX, const LibMCDriver_ScanLabSMC_double dMaxY)
{
}

void CSMCContext::ResetLaserField()
{
}

bool CSMCContext::GetLaserField(LibMCDriver_ScanLabSMC_double& dMinX, LibMCDriver_ScanLabSMC_double& dMinY, LibMCDriver_ScanLabSMC_double& dMaxX, LibMCDriver_ScanLabSMC_double& dMaxY)
{
	dMinX = 0.0;
	dMinY = 0.0;
	dMaxX = 0.0;
	dMaxY = 0.0;
	return false;
}

ISMCJob* CSMCContext::BeginJob(const LibMCDriver_ScanLabSMC_double dStartPositionX, const LibMCDriver_ScanLabSMC_double dStartPositionY, const LibMCDriver_ScanLabSMC::eBlendMode eBlendMode)
{
	auto pJobInstance = m_pContextInstance->BeginJob(dStartPositionX, dStartPositionY, eBlendMode);
	return new CSMCJob (pJobInstance);
}

ISMCJob* CSMCContext::GetUnfinishedJob()
{
	auto pJobInstance = m_pContextInstance->GetUnfinishedJob();
	return new CSMCJob(pJobInstance);
}


void CSMCContext::DrawLayer(const std::string& sStreamUUID, const LibMCDriver_ScanLabSMC_uint32 nLayerIndex)
{
	auto pToolpathAccessor = m_pDriverEnvironment->CreateToolpathAccessor(sStreamUUID);

	auto pLayer = pToolpathAccessor->LoadLayer(nLayerIndex);	

	auto pJob = m_pContextInstance->BeginJob(0.0, 0.0, eBlendMode::MaxAccuracy);
	pJob->AddLayerToList(pLayer);
	pJob->Finalize();
	pJob->Execute(true);

}

