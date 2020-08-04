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

public:

	CDriver_Marlin(const std::string& sName, const std::string& sType);

	void Connect(const std::string& sCOMPort, const LibMCDriver_Marlin_uint32 nBaudrate) override;

	void Disconnect() override;

	void SetAbsolutePositioning(const bool bAbsolute) override;

	void UpdateState() override;

	void GetCurrentPosition(LibMCDriver_Marlin_double& dX, LibMCDriver_Marlin_double& dY, LibMCDriver_Marlin_double& dZ) override;

	void GetTargetPosition(LibMCDriver_Marlin_double& dX, LibMCDriver_Marlin_double& dY, LibMCDriver_Marlin_double& dZ) override;

	bool CanExecuteMovement() override;

	bool IsMoving() override;

	void MoveTo(const LibMCDriver_Marlin_double dX, const LibMCDriver_Marlin_double dY, const LibMCDriver_Marlin_double dZ, const LibMCDriver_Marlin_double dSpeed) override;

	void MoveFastTo(const LibMCDriver_Marlin_double dX, const LibMCDriver_Marlin_double dY, const LibMCDriver_Marlin_double dZ, const LibMCDriver_Marlin_double dSpeed) override;

	void GetExtruderTemperature(const LibMCDriver_Marlin_uint32 nExtruderID, LibMCDriver_Marlin_double& dCurrentTemperature, LibMCDriver_Marlin_double& dTargetTemperature) override;

};

} // namespace Impl
} // namespace LibMCDriver_Marlin

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_MARLIN_DRIVER_MARLIN
