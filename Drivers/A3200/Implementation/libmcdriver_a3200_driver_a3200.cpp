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


Abstract: This is a stub class definition of CDriver_A3200

*/

#include "libmcdriver_a3200_driver_a3200.hpp"
#include "libmcdriver_a3200_interfaceexception.hpp"

#define __STRINGIZE(x) #x
#define __STRINGIZE_VALUE_OF(x) __STRINGIZE(x)

#include <array>

using namespace LibMCDriver_A3200::Impl;

#define STATUSITEM_PositionFeedback 0
#define STATUSITEM_PositionCommand 1
#define STATUSITEM_PositionError 2
#define STATUSITEM_VelocityFeedback 3
#define STATUSITEM_VelocityCommand 4
#define STATUSITEM_VelocityError 5

/*************************************************************************************************************************
 Class definition of CDriver_A3200 
**************************************************************************************************************************/

CDriver_A3200::CDriver_A3200(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment)
	: m_bSimulationMode (false), 
	m_sName (sName), 
	m_pDriverEnvironment (pDriverEnvironment), 
	m_pHandle (nullptr)
{

}

CDriver_A3200::~CDriver_A3200()
{
	unloadSDK();
}


void CDriver_A3200::Configure(const std::string& sConfigurationString)
{

}

std::string CDriver_A3200::GetName()
{
	return m_sName;
}

std::string CDriver_A3200::GetType()
{
	return "a3200-1.0";
}

void CDriver_A3200::GetVersion(LibMCDriver_A3200_uint32& nMajor, LibMCDriver_A3200_uint32& nMinor, LibMCDriver_A3200_uint32& nMicro, std::string& sBuild)
{
	nMajor = LIBMCDRIVER_A3200_VERSION_MAJOR;
	nMinor = LIBMCDRIVER_A3200_VERSION_MINOR;
	nMicro = LIBMCDRIVER_A3200_VERSION_MICRO;
	sBuild = __STRINGIZE_VALUE_OF(__GITHASH);
}

void CDriver_A3200::QueryParameters()
{
	QueryParametersEx(m_pDriverEnvironment->CreateStatusUpdateSession());
}

void CDriver_A3200::QueryParametersEx(LibMCEnv::PDriverStatusUpdateSession pDriverUpdateInstance)
{

}


void CDriver_A3200::SetToSimulationMode()
{
	m_bSimulationMode = true;
}

bool CDriver_A3200::IsSimulationMode()
{
	return m_bSimulationMode;
}

void CDriver_A3200::SetCustomSDKResource(const std::string & sCoreResourceName, const std::string & sSystemResourceName, const std::string & sCompilerResourceName, const std::string & sUtilitiesResourceName, const std::string & sLicenseDecoderResourceName)
{
	m_sCoreSystemResourceName = sCoreResourceName;
	m_sSystemSDKResourceName = sSystemResourceName;
	m_sCompilerSDKResourceName = sCompilerResourceName;
	m_sUtilitiesSDKResourceName = sUtilitiesResourceName;
	m_sLicenseDecoderSDKResourceName = sLicenseDecoderResourceName;

	m_CoreSystemBuffer.resize(0);
	m_SystemSDKBuffer.resize(0);
	m_CompilerSDKBuffer.resize(0);
	m_UtilitiesSDKBuffer.resize(0);
	m_LicenseDecoderSDKBuffer.resize(0);
}

void CDriver_A3200::SetCustomSDK(const LibMCDriver_A3200_uint64 nCoreSDKBufferBufferSize, const LibMCDriver_A3200_uint8 * pCoreSDKBufferBuffer, const LibMCDriver_A3200_uint64 nSystemSDKBufferBufferSize, const LibMCDriver_A3200_uint8 * pSystemSDKBufferBuffer, const LibMCDriver_A3200_uint64 nCompilerSDKBufferBufferSize, const LibMCDriver_A3200_uint8 * pCompilerSDKBufferBuffer, const LibMCDriver_A3200_uint64 nUtilitiesSDKBufferBufferSize, const LibMCDriver_A3200_uint8 * pUtilitiesSDKBufferBuffer, const LibMCDriver_A3200_uint64 nLicenseDecoderSDKBufferBufferSize, const LibMCDriver_A3200_uint8 * pLicenseDecoderSDKBufferBuffer)
{
	m_sCoreSystemResourceName = "";
	m_sSystemSDKResourceName = "";
	m_sCompilerSDKResourceName = "";
	m_sUtilitiesSDKResourceName = "";
	m_sLicenseDecoderSDKResourceName = "";

	storeCustomSDK(m_CoreSystemBuffer, nCoreSDKBufferBufferSize, pCoreSDKBufferBuffer);
	storeCustomSDK(m_SystemSDKBuffer, nSystemSDKBufferBufferSize, pSystemSDKBufferBuffer);
	storeCustomSDK(m_CompilerSDKBuffer, nCompilerSDKBufferBufferSize, pCompilerSDKBufferBuffer);
	storeCustomSDK(m_UtilitiesSDKBuffer, nUtilitiesSDKBufferBufferSize, pUtilitiesSDKBufferBuffer);
	storeCustomSDK(m_LicenseDecoderSDKBuffer, nLicenseDecoderSDKBufferBufferSize, pLicenseDecoderSDKBufferBuffer);
}


void CDriver_A3200::storeCustomSDK(std::vector<uint8_t>& buffer, uint64_t nArraySize, const uint8_t* pData)
{
	buffer.resize(nArraySize);
	if (nArraySize > 0) {
		if (pData == nullptr)
			throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_INVALIDPARAM);

		const uint8_t* pSource = pData;
		uint8_t* pTarget = buffer.data();

		for (uint64_t nIndex = 0; nIndex < nArraySize; nIndex++) {
			*pTarget = *pSource;
			pTarget++;
			pSource++;
		}
	}
}

LibMCEnv::PWorkingFile CDriver_A3200::createCustomDLL(const std::string& sFileNameOnDisk, std::vector<uint8_t>& buffer, const std::string& sResourceName, const std::string& sDefaultResourceName)
{
	if (m_pWorkingDirectory.get() == nullptr)
		throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_NOWORKINGDIRECTORY);

	if (buffer.size() > 0) {
		// Custom SDK Data has been given
		return m_pWorkingDirectory->StoreCustomData(sFileNameOnDisk, buffer);
	}
	else {
		// Load SDK Data from resource
		std::string sResourceNameToUse = sResourceName;
		if (sResourceNameToUse.empty())
			sResourceNameToUse = sDefaultResourceName;			

		std::vector<uint8_t> resourceDataBuffer;
		if (m_pDriverEnvironment->MachineHasResourceData(sResourceNameToUse)) {
			// Either machine resources
			m_pDriverEnvironment->RetrieveMachineResourceData(sResourceNameToUse, resourceDataBuffer);
		}
		else {
			// Or driver resources
			m_pDriverEnvironment->RetrieveDriverResourceData(sResourceNameToUse, resourceDataBuffer);
		}

		return m_pWorkingDirectory->StoreCustomData(sFileNameOnDisk, resourceDataBuffer);

	}
		
	
}


void CDriver_A3200::loadSDK()
{
	unloadSDK();

	m_pWorkingDirectory = m_pDriverEnvironment->CreateWorkingDirectory();

	// store DLLs in Temp directory as the SDK mandates them
	m_pCoreSystemDLL = createCustomDLL("A3200C64.dll", m_CoreSystemBuffer, m_sCoreSystemResourceName, "a3200c64");
	m_pSystemSDKDLL = createCustomDLL("A32Sys64.dll", m_SystemSDKBuffer, m_sSystemSDKResourceName, "a32sys64");
	m_pCompilerSDKDLL = createCustomDLL("A32Cmplr64.dll", m_CompilerSDKBuffer, m_sCompilerSDKResourceName, "a32cmplr64");
	m_pUtilitiesSDKDLL = createCustomDLL("AerUtilities64.dll", m_UtilitiesSDKBuffer, m_sUtilitiesSDKResourceName, "aerutilities64");;
	m_pLicenseDecoderSDKDLL = createCustomDLL("LicenseDecoder64.dll", m_LicenseDecoderSDKBuffer, m_sLicenseDecoderSDKResourceName, "licensedecoder64");

	m_pSDK = std::make_shared<CA3200SDK>(m_pCoreSystemDLL->GetAbsoluteFileName (), m_pWorkingDirectory->GetAbsoluteFilePath ());
}

void CDriver_A3200::unloadSDK()
{
	if ((m_pHandle != nullptr) && (m_pSDK.get () != nullptr)) {
		m_pSDK->A3200Disconnect(m_pHandle);
	}

	m_pHandle = nullptr;
	m_pSDK = nullptr;

	m_pCoreSystemDLL = nullptr;
	m_pSystemSDKDLL = nullptr;
	m_pCompilerSDKDLL = nullptr;
	m_pUtilitiesSDKDLL = nullptr;
	m_pLicenseDecoderSDKDLL = nullptr;
	m_pWorkingDirectory = nullptr;

}


void CDriver_A3200::Connect()
{
	if (m_pSDK.get () == nullptr) 
		loadSDK();

	if ((m_pHandle != nullptr) && (m_pSDK.get() != nullptr)) {
		m_pSDK->A3200Disconnect(m_pHandle);
	}

	m_pHandle = nullptr;
	m_pSDK->checkError(m_pSDK->A3200Connect(&m_pHandle));

}

void CDriver_A3200::Disconnect()
{
	unloadSDK();

}

void CDriver_A3200::RunScript(const LibMCDriver_A3200_uint32 nTaskID, const std::string& sScript)
{
	if ((m_pHandle == nullptr) || (m_pSDK.get() == nullptr))
		throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_NOTCONNECTED);
	if ((nTaskID < A3200_MINTASKID) || (nTaskID > A3200_MAXTASKID))
		throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_INVALIDTASKID);

	auto pTempFile = m_pWorkingDirectory->StoreCustomStringInTempFile("pgm", sScript);
	std::string sTempFileName = pTempFile->GetAbsoluteFileName();

	m_pSDK->checkError (m_pSDK->A3200ProgramRun(m_pHandle, nTaskID, sTempFileName.c_str ()));

}

void CDriver_A3200::RunCommand(const LibMCDriver_A3200_uint32 nTaskID, const std::string& sCommand)
{
	if ((m_pHandle == nullptr) || (m_pSDK.get() == nullptr))
		throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_NOTCONNECTED);
	if ((nTaskID < A3200_MINTASKID) || (nTaskID > A3200_MAXTASKID))
		throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_INVALIDTASKID);

	m_pSDK->checkError(m_pSDK->A3200CommandExecute(m_pHandle, nTaskID, sCommand.c_str(), nullptr));
}


void CDriver_A3200::StopProgram(const LibMCDriver_A3200_uint32 nTaskID, const LibMCDriver_A3200_uint32 nTimeout)
{
	if ((m_pHandle == nullptr) || (m_pSDK.get () == nullptr))
		throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_NOTCONNECTED);
	if ((nTaskID < A3200_MINTASKID) || (nTaskID > A3200_MAXTASKID))
		throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_INVALIDTASKID);

	m_pSDK->checkError (m_pSDK->A3200ProgramStopAndWait(m_pHandle, nTaskID, nTimeout));
}




LibMCDriver_A3200_double CDriver_A3200::ReadGlobalNumberVariable(const std::string& sName)
{
	if ((m_pHandle == nullptr) || (m_pSDK.get() == nullptr))
		throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_NOTCONNECTED);

	double dValue = 0.0;
	m_pSDK->checkError(m_pSDK->A3200VariableGetValueByName(m_pHandle, 0, sName.c_str(), &dValue));

	return dValue;
}


void CDriver_A3200::WriteGlobalNumberVariable(const std::string& sName, const LibMCDriver_A3200_double dValue) 
{
	if ((m_pHandle == nullptr) || (m_pSDK.get() == nullptr))
		throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_NOTCONNECTED);

	m_pSDK->checkError(m_pSDK->A3200VariableSetValueByName(m_pHandle, 0, sName.c_str(), dValue));
}

std::string CDriver_A3200::ReadGlobalStringVariable(const std::string& sName) 
{
	if ((m_pHandle == nullptr) || (m_pSDK.get() == nullptr))
		throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_NOTCONNECTED);

	std::vector<char> stringBuffer;
	stringBuffer.resize(A3200_MAXSTRINGLENGTH + 1);
	for (auto& ch : stringBuffer)
		ch = 0;


	m_pSDK->checkError(m_pSDK->A3200VariableGetValueStringByName(m_pHandle, 0, sName.c_str(), stringBuffer.data (), A3200_MAXSTRINGLENGTH));

	stringBuffer.at(A3200_MAXSTRINGLENGTH) = 0;
	return stringBuffer.data ();
}

void CDriver_A3200::WriteGlobalStringVariable(const std::string& sName, const std::string& sValue) 
{
	if ((m_pHandle == nullptr) || (m_pSDK.get() == nullptr))
		throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_NOTCONNECTED);

	m_pSDK->checkError(m_pSDK->A3200VariableSetValueStringByName(m_pHandle, 0, sName.c_str(), sValue.c_str ()));
}

LibMCDriver_A3200_double CDriver_A3200::ReadTaskNumberVariable(const LibMCDriver_A3200_uint32 nTaskID, const std::string& sName) 
{
	if ((m_pHandle == nullptr) || (m_pSDK.get() == nullptr))
		throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_NOTCONNECTED);
	if ((nTaskID < A3200_MINTASKID) || (nTaskID > A3200_MAXTASKID))
		throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_INVALIDTASKID);

	double dValue = 0.0;
	m_pSDK->checkError(m_pSDK->A3200VariableGetValueByName(m_pHandle, nTaskID, sName.c_str(), &dValue));

	return dValue;
}

void CDriver_A3200::WriteTaskNumberVariable(const LibMCDriver_A3200_uint32 nTaskID, const std::string& sName, const LibMCDriver_A3200_double dValue)
{
	if ((m_pHandle == nullptr) || (m_pSDK.get() == nullptr))
		throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_NOTCONNECTED);
	if ((nTaskID < A3200_MINTASKID) || (nTaskID > A3200_MAXTASKID))
		throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_INVALIDTASKID);

	m_pSDK->checkError(m_pSDK->A3200VariableSetValueByName(m_pHandle, nTaskID, sName.c_str(), dValue));
}

std::string CDriver_A3200::ReadTaskStringVariable(const LibMCDriver_A3200_uint32 nTaskID, const std::string& sName) 
{
	if ((m_pHandle == nullptr) || (m_pSDK.get() == nullptr))
		throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_NOTCONNECTED);
	if ((nTaskID < A3200_MINTASKID) || (nTaskID > A3200_MAXTASKID))
		throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_INVALIDTASKID);

	std::vector<char> stringBuffer;
	stringBuffer.resize (A3200_MAXSTRINGLENGTH + 1);
	for (auto& ch : stringBuffer)
		ch = 0;

	m_pSDK->checkError(m_pSDK->A3200VariableGetValueStringByName(m_pHandle, nTaskID, sName.c_str(), stringBuffer.data(), A3200_MAXSTRINGLENGTH));

	stringBuffer.at(A3200_MAXSTRINGLENGTH) = 0;
	return stringBuffer.data();
}

void CDriver_A3200::WriteTaskStringVariable(const LibMCDriver_A3200_uint32 nTaskID, const std::string& sName, const std::string& sValue) 
{
	if ((m_pHandle == nullptr) || (m_pSDK.get() == nullptr))
		throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_NOTCONNECTED);
	if ((nTaskID < A3200_MINTASKID) || (nTaskID > A3200_MAXTASKID))
		throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_INVALIDTASKID);

	m_pSDK->checkError(m_pSDK->A3200VariableSetValueStringByName(m_pHandle, nTaskID, sName.c_str(), sValue.c_str()));
}


LibMCDriver_A3200_double CDriver_A3200::ReadAxisPosition(const LibMCDriver_A3200_uint32 nAxisID)
{
	if ((m_pHandle == nullptr) || (m_pSDK.get() == nullptr))
		throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_NOTCONNECTED);
	if ((nAxisID < A3200_MINAXISID) || (nAxisID > A3200_MINAXISID))
		throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_INVALIDAXISID);

	double dValue = 0.0;
	m_pSDK->checkError(m_pSDK->A3200StatusGetItem (m_pHandle, (uint32_t) nAxisID, STATUSITEM_PositionFeedback, 0, &dValue));

	return dValue;
}

LibMCDriver_A3200_double CDriver_A3200::ReadAxisTargetPosition(const LibMCDriver_A3200_uint32 nAxisID)
{
	if ((m_pHandle == nullptr) || (m_pSDK.get() == nullptr))
		throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_NOTCONNECTED);
	if ((nAxisID < A3200_MINAXISID) || (nAxisID > A3200_MINAXISID))
		throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_INVALIDAXISID);

	double dValue = 0.0;
	m_pSDK->checkError(m_pSDK->A3200StatusGetItem(m_pHandle, (uint32_t)nAxisID, STATUSITEM_PositionCommand, 0, &dValue));

	return dValue;

}

LibMCDriver_A3200_double CDriver_A3200::ReadAxisVelocity(const LibMCDriver_A3200_uint32 nAxisID)
{
	if ((m_pHandle == nullptr) || (m_pSDK.get() == nullptr))
		throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_NOTCONNECTED);
	if ((nAxisID < A3200_MINAXISID) || (nAxisID > A3200_MINAXISID))
		throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_INVALIDAXISID);

	double dValue = 0.0;
	m_pSDK->checkError(m_pSDK->A3200StatusGetItem(m_pHandle, (uint32_t)nAxisID, STATUSITEM_VelocityFeedback, 0, &dValue));

	return dValue;
}

LibMCDriver_A3200_double CDriver_A3200::ReadAxisTargetVelocity(const LibMCDriver_A3200_uint32 nAxisID)
{
	if ((m_pHandle == nullptr) || (m_pSDK.get() == nullptr))
		throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_NOTCONNECTED);
	if ((nAxisID < A3200_MINAXISID) || (nAxisID > A3200_MINAXISID))
		throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_INVALIDAXISID);

	double dValue = 0.0;
	m_pSDK->checkError(m_pSDK->A3200StatusGetItem(m_pHandle, (uint32_t)nAxisID, STATUSITEM_VelocityCommand, 0, &dValue));

	return dValue;
}

void CDriver_A3200::ReadAxisInformation(const LibMCDriver_A3200_uint32 nAxisID, LibMCDriver_A3200_double& dCurrentPositionValue, LibMCDriver_A3200_double& dTargetPositionValue, LibMCDriver_A3200_double& dPositionErrorValue, LibMCDriver_A3200_double& dCurrentVelocityValue, LibMCDriver_A3200_double& dTargetVelocityValue, LibMCDriver_A3200_double& dVelocityErrorValue)
{
	{
		if ((m_pHandle == nullptr) || (m_pSDK.get() == nullptr))
			throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_NOTCONNECTED);
		if ((nAxisID < A3200_MINAXISID) || (nAxisID > A3200_MINAXISID))
			throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_INVALIDAXISID);

		double dCurrentPositionValue = 0.0;
		std::vector<uint16_t> axisIDs;
		std::vector<uint32_t> itemCodes;
		std::vector<uint32_t> itemExtras;
		std::vector<double> itemValues;

		axisIDs.push_back(nAxisID);
		itemCodes.push_back(STATUSITEM_PositionFeedback);
		itemExtras.push_back(0);
		itemValues.push_back(0.0);

		axisIDs.push_back(nAxisID);
		itemCodes.push_back(STATUSITEM_PositionCommand);
		itemExtras.push_back(0);
		itemValues.push_back(0.0);

		axisIDs.push_back(nAxisID);
		itemCodes.push_back(STATUSITEM_PositionError);
		itemExtras.push_back(0);
		itemValues.push_back(0.0);

		axisIDs.push_back(nAxisID);
		itemCodes.push_back(STATUSITEM_VelocityFeedback);
		itemExtras.push_back(0);
		itemValues.push_back(0.0);

		axisIDs.push_back(nAxisID);
		itemCodes.push_back(STATUSITEM_VelocityCommand);
		itemExtras.push_back(0);
		itemValues.push_back(0.0);

		axisIDs.push_back(nAxisID);
		itemCodes.push_back(STATUSITEM_VelocityError);
		itemExtras.push_back(0);
		itemValues.push_back(0.0);

		m_pSDK->checkError(m_pSDK->A3200StatusGetItems(m_pHandle, 6, axisIDs.data (), itemCodes.data (), itemExtras.data(), itemValues.data ()));

		dCurrentPositionValue = itemValues.at(0);
		dTargetPositionValue = itemValues.at(1);
		dPositionErrorValue = itemValues.at(2);
		dCurrentVelocityValue = itemValues.at(3);
		dTargetVelocityValue = itemValues.at(4);
		dVelocityErrorValue = itemValues.at(5);

	}

}
