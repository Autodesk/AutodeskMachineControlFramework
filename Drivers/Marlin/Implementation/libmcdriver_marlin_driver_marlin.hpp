/*++

Copyright (C) 2020 Autodesk Inc.

All rights reserved.

Abstract: This is the class declaration of CDriver_Marlin

*/


#ifndef __LIBMCDRIVER_MARLIN_DRIVER_MARLIN
#define __LIBMCDRIVER_MARLIN_DRIVER_MARLIN

#include "libmcdriver_marlin_interfaces.hpp"

// Parent classes
#include "libmcdriver_marlin_driver.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "AMC_SerialController_Marlin.hpp"

namespace LibMCDriver_Marlin {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CDriver_Marlin 
**************************************************************************************************************************/

class CDriver_Marlin : public virtual IDriver_Marlin, public virtual CDriver {
private:

protected:

	AMC::PSerialController m_pSerialController;
	bool m_doQueryFirmwareInfo;
	bool m_bDisableHoming;
	bool m_bDebug;

public:

	CDriver_Marlin(const std::string& sName, const std::string& sType, const bool doQueryFirmwareInfo, const bool bDisableHoming, const bool bDebug);

	void Connect(const std::string& sCOMPort, const LibMCDriver_Marlin_uint32 nBaudrate, const LibMCDriver_Marlin_double dStatusUpdateInterval, const LibMCDriver_Marlin_uint32 nConnectTimeout) override;

	void Disconnect() override;

	void SetAbsolutePositioning(const bool bAbsolute) override;

	void SetHeatedBedTargetTemperature(const LibMCDriver_Marlin_double dTemperatureInDegreeCelcius, const bool bWaitForTemp) override;

	void SetExtruderTargetTemperature(const LibMCDriver_Marlin_uint32 nExtruderID, const LibMCDriver_Marlin_double dTemperatureInDegreeCelcius, const bool bWaitForTemp) override;

	void SetFanSpeed(const LibMCDriver_Marlin_uint32 nFanID, const LibMCDriver_Marlin_uint32 dSpeed) override;

	void SetPidParameters(const LibMCDriver_Marlin_double dP, const LibMCDriver_Marlin_double dI, const LibMCDriver_Marlin_double dD) override;

	void UpdateState(const LibMCDriver_Marlin_uint32 nExtruderID) override;

	void GetCurrentPosition(LibMCDriver_Marlin_double& dX, LibMCDriver_Marlin_double& dY, LibMCDriver_Marlin_double& dZ) override;

	void GetTargetPosition(LibMCDriver_Marlin_double& dX, LibMCDriver_Marlin_double& dY, LibMCDriver_Marlin_double& dZ) override;

	void GetExtruderTargetPosition(LibMCDriver_Marlin_double& dE) override;

	void GetHeatedBedTemperature(LibMCDriver_Marlin_double& dTargetTemperature, LibMCDriver_Marlin_double& dCurrentTemperature) override;

	void GetExtruderTemperature(const LibMCDriver_Marlin_uint32 nExtruderID, LibMCDriver_Marlin_double& dTargetTemperature, LibMCDriver_Marlin_double& dCurrentTemperature) override;

	void GetPidParameters(LibMCDriver_Marlin_double& dP, LibMCDriver_Marlin_double& dI, LibMCDriver_Marlin_double& dD) override;

	bool CanExecuteMovement() override;

	bool IsMoving() override;

	bool IsHomed() override;
	
	bool IsConnected() override;

	void MoveToXY(const LibMCDriver_Marlin_double dX, const LibMCDriver_Marlin_double dY, const LibMCDriver_Marlin_double dE, const LibMCDriver_Marlin_double dSpeed) override;

	void MoveFastToXY(const LibMCDriver_Marlin_double dX, const LibMCDriver_Marlin_double dY, const LibMCDriver_Marlin_double dSpeed) override;
	
	void MoveToZ(const LibMCDriver_Marlin_double dZ, const LibMCDriver_Marlin_double dE, const LibMCDriver_Marlin_double dSpeed) override;
	
	void MoveFastToZ(const LibMCDriver_Marlin_double dZ, const LibMCDriver_Marlin_double dSpeed) override;

	void StartHoming() override;

	void EmergencyStop() override;

};

} // namespace Impl
} // namespace LibMCDriver_Marlin

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_MARLIN_DRIVER_MARLIN
