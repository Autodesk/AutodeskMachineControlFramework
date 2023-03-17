/*++

Copyright (C) 2020 Autodesk Inc.

All rights reserved.

Abstract: This is the class declaration of CDriver_ScanLab_RTC6

*/


#ifndef __LIBMCDRIVER_SCANLAB_DRIVER_SCANLAB_RTC6
#define __LIBMCDRIVER_SCANLAB_DRIVER_SCANLAB_RTC6

#include "libmcdriver_scanlab_interfaces.hpp"
#include "libmcdriver_scanlab_driver_scanlab.hpp"
#include "libmcdriver_scanlab_rtccontext.hpp"
#include "libmcdriver_scanlab_rtcselector.hpp"
#include "libmcdriver_scanlab_configurationpreset.hpp"

// Parent classes
#include "libmcdriver_scanlab_driver.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include <map>

namespace LibMCDriver_ScanLab {
namespace Impl {


	template<class T>
	class act_managed_ptr
	{
	private:
		T* ptr = nullptr;		

	public:
		act_managed_ptr() : ptr(nullptr)
		{
		}

		act_managed_ptr(T* ptr) : ptr(ptr)
		{
			if (ptr != nullptr)
				ptr->IncRefCount();
		}

		act_managed_ptr(const act_managed_ptr& obj) // copy constructor
		{
			this->ptr = obj.ptr; // share the underlying pointer
			if (this->ptr != nullptr)
				this->ptr->IncRefCount();

		}

		act_managed_ptr& operator=(const act_managed_ptr& obj) // copy assignment
		{
			__cleanup__(); // cleanup any existing data

			// Assign incoming object's data to this object
			this->ptr = obj.ptr; // share the underlying pointer
			if (this->ptr != nullptr)
				this->ptr->IncRefCount();
		}

		/*** Move Semantics ***/
		act_managed_ptr(act_managed_ptr&& dyingObj) // move constructor
		{
			this->ptr = dyingObj.ptr; 
			dyingObj.ptr = nullptr;
		}

		act_managed_ptr& operator=(act_managed_ptr&& dyingObj) // move assignment
		{
			__cleanup__(); // cleanup any existing data

			this->ptr = dyingObj.ptr;
			dyingObj.ptr = nullptr;

			return *this;
		}

		T* get() const
		{
			return this->ptr;
		}

		T* operator->() const
		{
			return this->ptr;
		}

		T& operator*() const
		{
			return this->ptr;
		}

		~act_managed_ptr() // destructor
		{
			__cleanup__();
		}

	private:
		void __cleanup__()
		{
			if (this->ptr != nullptr)
				this->ptr->DecRefCount();
			this->ptr = nullptr;
		}
	};

/*************************************************************************************************************************
 Class declaration of CDriver_ScanLab_RTC6 
**************************************************************************************************************************/

class CDriver_ScanLab_RTC6 : public virtual IDriver_ScanLab_RTC6, public virtual CDriver_ScanLab {
private:

	std::string m_sName; 
	std::string m_sType;

	bool m_SimulationMode;

	float m_fMaxLaserPowerInWatts;

	act_managed_ptr<IRTCSelector> m_pRTCSelector;
	act_managed_ptr<IRTCContext> m_pRTCContext;

	std::map<std::string, PDriver_ScanLab_RTC6ConfigurationPreset> m_ConfigurationPresets;

	void updateCardStatus(LibMCEnv::PDriverStatusUpdateSession pDriverUpdateInstance);

	

protected:

public:

	CDriver_ScanLab_RTC6(const std::string& sName, const std::string& sType, LibMCEnv::PDriverEnvironment pDriverEnvironment);

	void Configure(const std::string& sConfigurationString) override;

	std::string GetName() override;

	std::string GetType() override;

	void QueryParameters() override;

	void QueryParametersEx(LibMCEnv::PDriverStatusUpdateSession pDriverUpdateInstance) override;

	void SetToSimulationMode() override;

	bool IsInitialized() override;

	bool IsSimulationMode() override;

	void Initialise(const std::string& sIP, const std::string& sNetmask, const LibMCDriver_ScanLab_uint32 nTimeout, const LibMCDriver_ScanLab_uint32 nSerialNumber) override;

	void InitialiseFromConfiguration(const std::string& sPresetName) override;

	std::string GetIPAddress() override;

	std::string GetNetmask() override;

	LibMCDriver_ScanLab_uint32 GetSerialNumber() override;

	IRTCContext* GetContext() override;

	IRTCSelector* GetSelector() override;

	void LoadFirmware(const std::string& sFirmwareResource, const std::string& sFPGAResource, const std::string& sAuxiliaryResource) override;

	void LoadCustomFirmware(const LibMCDriver_ScanLab_uint64 nFirmwareDataBufferSize, const LibMCDriver_ScanLab_uint8* pFirmwareDataBuffer, const LibMCDriver_ScanLab_uint64 nFPGADataBufferSize, const LibMCDriver_ScanLab_uint8* pFPGADataBuffer, const LibMCDriver_ScanLab_uint64 nAuxiliaryDataBufferSize, const LibMCDriver_ScanLab_uint8* pAuxiliaryDataBuffer) override;

	void SetCorrectionFile(const LibMCDriver_ScanLab_uint64 nCorrectionFileBufferSize, const LibMCDriver_ScanLab_uint8* pCorrectionFileBuffer, const LibMCDriver_ScanLab_uint32 nTableNumber, const LibMCDriver_ScanLab_uint32 nDimension, const LibMCDriver_ScanLab_uint32 nTableNumberHeadA, const LibMCDriver_ScanLab_uint32 nTableNumberHeadB) override;

	void ConfigureLaserMode(const LibMCDriver_ScanLab::eLaserMode eLaserMode, const LibMCDriver_ScanLab::eLaserPort eLaserPort, const LibMCDriver_ScanLab_double dMaxLaserPower, const bool bFinishLaserPulseAfterOn, const bool bPhaseShiftOfLaserSignal, const bool bLaserOnSignalLowActive, const bool bLaserHalfSignalsLowActive, const bool bSetDigitalInOneHighActive, const bool bOutputSynchronizationActive) override;

	void ConfigureDelays(const LibMCDriver_ScanLab_double dLaserOnDelay, const LibMCDriver_ScanLab_double dLaserOffDelay, const LibMCDriver_ScanLab_double dMarkDelay, const LibMCDriver_ScanLab_double dJumpDelay, const LibMCDriver_ScanLab_double dPolygonDelay) override;

	void DrawLayer(const std::string& sStreamUUID, const LibMCDriver_ScanLab_uint32 nLayerIndex) override;

	void AddLayerToCurrentList(const std::string& sStreamUUID, const LibMCDriver_ScanLab_uint32 nLayerIndex) override;

	void SetCommunicationTimeouts(const LibMCDriver_ScanLab_double dInitialTimeout, const LibMCDriver_ScanLab_double dMaxTimeout, const LibMCDriver_ScanLab_double dMultiplier) override;

	void GetCommunicationTimeouts(LibMCDriver_ScanLab_double& dInitialTimeout, LibMCDriver_ScanLab_double& dMaxTimeout, LibMCDriver_ScanLab_double& dMultiplier) override;

	PDriver_ScanLab_RTC6ConfigurationPreset findPresetByName(const std::string & sPresetName, bool bMustExist);

	void SetStartList(const LibMCDriver_ScanLab_uint32 nListIndex, const LibMCDriver_ScanLab_uint32 nPosition) override;

	void SetEndOfList() override;

	void ExecuteList(const LibMCDriver_ScanLab_uint32 nListIndex, const LibMCDriver_ScanLab_uint32 nPosition) override;


};

} // namespace Impl
} // namespace LibMCDriver_ScanLab

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_SCANLAB_DRIVER_SCANLAB_RTC6
