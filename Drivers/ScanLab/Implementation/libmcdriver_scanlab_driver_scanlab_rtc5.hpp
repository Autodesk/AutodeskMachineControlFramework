/*++

Copyright (C) 2020 Autodesk Inc.

All rights reserved.

Abstract: This is the class declaration of CDriver_ScanLab_RTC5

*/


#ifndef __LIBMCDRIVER_SCANLAB_DRIVER_SCANLAB_RTC5
#define __LIBMCDRIVER_SCANLAB_DRIVER_SCANLAB_RTC5

#include "libmcdriver_scanlab_interfaces.hpp"

// Parent classes
#include "libmcdriver_scanlab_driver.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.


namespace LibMCDriver_ScanLab {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CDriver_ScanLab_RTC5 
**************************************************************************************************************************/

class CDriver_ScanLab_RTC5 : public virtual IDriver_ScanLab_RTC5, public virtual CDriver {
private:

	std::string m_sName; 
	std::string m_sType;

protected:

public:

	CDriver_ScanLab_RTC5(const std::string& sName, const std::string& sType);

	IRTCSelector * CreateRTCSelector() override;

	std::string GetName() override;

	void GetVersion(LibMCDriver_ScanLab_uint32& nMajor, LibMCDriver_ScanLab_uint32& nMinor, LibMCDriver_ScanLab_uint32& nMicro, std::string& sBuild) override;

	std::string GetType() override;

	void GetHeaderInformation(std::string& sNameSpace, std::string& sBaseName) override;
};

} // namespace Impl
} // namespace LibMCDriver_ScanLab

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_SCANLAB_DRIVER_SCANLAB_RTC5
