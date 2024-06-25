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


Abstract: This is a stub class definition of CRTCRecording

*/

#include "libmcdriver_scanlab_rtcrecording.hpp"
#include "libmcdriver_scanlab_interfaceexception.hpp"


using namespace LibMCDriver_ScanLab::Impl;


/*************************************************************************************************************************
 Class definition of CRTCRecording 
**************************************************************************************************************************/
CRTCRecording::CRTCRecording(PRTCRecordingInstance pInstance)
	: m_pInstance(pInstance)
{
	if (pInstance.get() == nullptr)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);

}

CRTCRecording::~CRTCRecording()
{

}


bool CRTCRecording::ScanheadConnectionCheckIsEnabled()
{
	return m_pInstance->getScanheadConnectionCheckIsEnabled ();
}

void CRTCRecording::EnableScanheadConnectionCheck()
{
	m_pInstance->setScanheadConnectionCheckIsEnabled(true);
}

void CRTCRecording::DisableScanheadConnectionCheck()
{
	m_pInstance->setScanheadConnectionCheckIsEnabled(false);
}

void CRTCRecording::Clear()
{
	m_pInstance->clear();
}

void CRTCRecording::AddChannel(const std::string & sChannelName, const LibMCDriver_ScanLab::eRTCChannelType eChannelType)
{
	m_pInstance->addChannel(sChannelName, eChannelType);
}

void CRTCRecording::RemoveChannel(const std::string & sChannelName)
{
	m_pInstance->removeChannel(sChannelName);
}

bool CRTCRecording::HasChannel(const std::string & sChannelName)
{
	return m_pInstance->hasChannel(sChannelName);
}

LibMCDriver_ScanLab::eRTCChannelType CRTCRecording::GetChannelType(const std::string & sChannelName)
{
	return m_pInstance->getChannelType(sChannelName);
}

LibMCDriver_ScanLab_uint64 CRTCRecording::GetRecordCount(const std::string& sChannelName)
{
	return m_pInstance->getRecordCount(sChannelName);
}

LibMCDriver_ScanLab_int32 CRTCRecording::GetRecordEntry(const std::string& sChannelName, const LibMCDriver_ScanLab_uint64 nRecordIndex)
{
	return m_pInstance->getRecordEntry(sChannelName, nRecordIndex);
}

void CRTCRecording::GetAllRecordEntries(const std::string & sChannelName, LibMCDriver_ScanLab_uint64 nValuesBufferSize, LibMCDriver_ScanLab_uint64* pValuesNeededCount, LibMCDriver_ScanLab_int32 * pValuesBuffer)
{
	m_pInstance->getAllRecordEntries(sChannelName, nValuesBufferSize, pValuesNeededCount, pValuesBuffer);
}

void CRTCRecording::EnableRecording()
{
	m_pInstance->enableRecording();
}

void CRTCRecording::DisableRecording()
{
	m_pInstance->disableRecording();
}

void CRTCRecording::ExecuteListWithRecording()
{
	m_pInstance->executeListWithRecording();
}

void CRTCRecording::AddRecordsToDataTable(const std::string & sChannelName, LibMCEnv::PDataTable pDataTable, const std::string & sColumnIdentifier, const std::string & sColumnDescription)
{
	m_pInstance->addRecordsToDataTable(sChannelName, pDataTable, sColumnIdentifier, sColumnDescription);
}

void CRTCRecording::AddScaledRecordsToDataTable(const std::string & sChannelName, LibMCEnv::PDataTable pDataTable, const std::string & sColumnIdentifier, const std::string & sColumnDescription, const LibMCDriver_ScanLab_double dScaleFactor, const LibMCDriver_ScanLab_double dOffset)
{
	m_pInstance->addScaledRecordsToDataTable(sChannelName, pDataTable, sColumnIdentifier, sColumnDescription, dScaleFactor, dOffset);
}

