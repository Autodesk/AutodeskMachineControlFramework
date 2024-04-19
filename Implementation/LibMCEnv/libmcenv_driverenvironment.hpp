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


Abstract: This is the class declaration of CDriverEnvironment

*/


#ifndef __LIBMCENV_DRIVERENVIRONMENT
#define __LIBMCENV_DRIVERENVIRONMENT

#include "libmcenv_interfaces.hpp"

#include "amc_parametergroup.hpp"
#include "amc_resourcepackage.hpp"
#include "amc_toolpathhandler.hpp"
#include "common_chrono.hpp"
#include "amc_logger.hpp"

// Parent classes
#include "libmcenv_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.


namespace LibMCEnv {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CDriverEnvironment 
**************************************************************************************************************************/

class CDriverEnvironment : public virtual IDriverEnvironment, public virtual CBase {
private:


protected:

	bool m_bIsInitializing;

	std::string m_sBaseTempPath;
	std::string m_sDriverName;

	AMC::PParameterGroup m_pParameterGroup;
	AMC::PResourcePackage m_pDriverResourcePackage;
	AMC::PResourcePackage m_pMachineResourcePackage;
	AMC::PToolpathHandler m_pToolpathHandler;
	LibMCData::PDataModel m_pDataModel;
	std::string m_sSystemUserID;
	AMC::PLogger m_pLogger;

	AMCCommon::PChrono m_pGlobalChrono;

public:

	CDriverEnvironment(AMC::PParameterGroup pParameterGroup, AMC::PResourcePackage pDriverResourcePackage, AMC::PResourcePackage pMachineResourcePackage, AMC::PToolpathHandler pToolpathHandler, const std::string& sBaseTempPath, AMC::PLogger pLogger, LibMCData::PDataModel pDataModel, AMCCommon::PChrono pGlobalChrono, std::string sSystemUserID, const std::string& sDriverName);

	virtual ~CDriverEnvironment();

	IDriverStatusUpdateSession* CreateStatusUpdateSession() override;

	IWorkingDirectory* CreateWorkingDirectory() override;

	ITCPIPConnection* CreateTCPIPConnection(const std::string& sIPAddress, const LibMCEnv_uint32 nPort, const LibMCEnv_uint32 nTimeOutInMS) override;

	IModbusTCPConnection* CreateModbusTCPConnection(const std::string& sIPAddress, const LibMCEnv_uint32 nPort, const LibMCEnv_uint32 nTimeOutInMS) override;

	void retrieveDriverDataFromPackage(AMC::PResourcePackage pResourcePackage, const std::string& sIdentifier, LibMCEnv_uint64 nDataBufferBufferSize, LibMCEnv_uint64* pDataBufferNeededCount, LibMCEnv_uint8* pDataBufferBuffer);

	bool DriverHasResourceData(const std::string& sIdentifier) override;

	bool MachineHasResourceData(const std::string& sIdentifier) override;

	void RetrieveDriverData(const std::string& sIdentifier, LibMCEnv_uint64 nDataBufferBufferSize, LibMCEnv_uint64* pDataBufferNeededCount, LibMCEnv_uint8* pDataBufferBuffer) override;

	void RetrieveDriverResourceData(const std::string& sIdentifier, LibMCEnv_uint64 nDataBufferBufferSize, LibMCEnv_uint64* pDataBufferNeededCount, LibMCEnv_uint8* pDataBufferBuffer) override;

	void RetrieveMachineResourceData(const std::string& sIdentifier, LibMCEnv_uint64 nDataBufferBufferSize, LibMCEnv_uint64* pDataBufferNeededCount, LibMCEnv_uint8* pDataBufferBuffer) override;

	IToolpathAccessor* CreateToolpathAccessor(const std::string& sBuildUUID) override;

	bool ParameterNameIsValid(const std::string& sParameterName) override;

	void RegisterStringParameter(const std::string& sParameterName, const std::string& sDescription, const std::string& sDefaultValue) override;

	void RegisterUUIDParameter(const std::string& sParameterName, const std::string& sDescription, const std::string& sDefaultValue) override;

	void RegisterDoubleParameter(const std::string& sParameterName, const std::string& sDescription, const LibMCEnv_double dDefaultValue) override;

	void RegisterDoubleParameterWithUnits(const std::string& sParameterName, const std::string& sDescription, const LibMCEnv_double dDefaultValue, const LibMCEnv_double dUnits) override;

	void RegisterIntegerParameter(const std::string& sParameterName, const std::string& sDescription, const LibMCEnv_int64 nDefaultValue) override;

	void RegisterBoolParameter(const std::string& sParameterName, const std::string& sDescription, const bool bDefaultValue) override;

	void SetStringParameter(const std::string& sParameterName, const std::string& sValue) override;

	void SetUUIDParameter(const std::string& sParameterName, const std::string& sValue) override;

	void SetDoubleParameter(const std::string& sParameterName, const LibMCEnv_double dValue) override;

	void SetIntegerParameter(const std::string& sParameterName, const LibMCEnv_int64 nValue) override;

	void SetBoolParameter(const std::string& sParameterName, const bool bValue) override;

	void setIsInitializing(bool bIsInitializing);

	void Sleep(const LibMCEnv_uint32 nDelay) override;

	LibMCEnv_uint64 GetGlobalTimerInMilliseconds() override;

	LibMCEnv_uint64 GetGlobalTimerInMicroseconds() override;

	virtual void LogMessage(const std::string& sLogString) override;

	virtual void LogWarning(const std::string& sLogString) override;

	virtual void LogInfo(const std::string& sLogString) override;

	IImageData* CreateEmptyImage(const LibMCEnv_uint32 nPixelSizeX, const LibMCEnv_uint32 nPixelSizeY, const LibMCEnv_double dDPIValueX, const LibMCEnv_double dDPIValueY, const LibMCEnv::eImagePixelFormat ePixelFormat) override;

	IImageData* LoadPNGImage(const LibMCEnv_uint64 nPNGDataBufferSize, const LibMCEnv_uint8* pPNGDataBuffer, const LibMCEnv_double dDPIValueX, const LibMCEnv_double dDPIValueY, const LibMCEnv::eImagePixelFormat ePixelFormat) override;

	IXMLDocument* CreateXMLDocument(const std::string& sRootNodeName, const std::string& sDefaultNamespace) override;

	IXMLDocument* ParseXMLString(const std::string& sXMLString) override;

	IXMLDocument* ParseXMLData(const LibMCEnv_uint64 nXMLDataBufferSize, const LibMCEnv_uint8* pXMLDataBuffer) override;

	IDataTable* CreateDataTable() override;

	IDiscreteFieldData2D* CreateDiscreteField2D(const LibMCEnv_uint32 nPixelSizeX, const LibMCEnv_uint32 nPixelSizeY, const LibMCEnv_double dDPIValueX, const LibMCEnv_double dDPIValueY, const LibMCEnv_double dOriginX, const LibMCEnv_double dOriginY, const LibMCEnv_double dDefaultValue) override;

	IDiscreteFieldData2D* CreateDiscreteField2DFromImage(IImageData* pImageDataInstance, const LibMCEnv_double dBlackValue, const LibMCEnv_double dWhiteValue, const LibMCEnv_double dOriginX, const LibMCEnv_double dOriginY) override;

	bool HasBuildJob(const std::string& sBuildUUID) override;

	IBuild* GetBuildJob(const std::string& sBuildUUID) override;

	ICryptoContext* CreateCryptoContext() override;

	IDateTime* GetCurrentDateTime() override;

	IDateTime* GetCustomDateTime(const LibMCEnv_uint32 nYear, const LibMCEnv_uint32 nMonth, const LibMCEnv_uint32 nDay, const LibMCEnv_uint32 nHour, const LibMCEnv_uint32 nMinute, const LibMCEnv_uint32 nSecond, const LibMCEnv_uint32 nMicrosecond)  override;

	IDateTime* GetStartDateTime() override;

};

} // namespace Impl
} // namespace LibMCEnv

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCENV_DRIVERENVIRONMENT
