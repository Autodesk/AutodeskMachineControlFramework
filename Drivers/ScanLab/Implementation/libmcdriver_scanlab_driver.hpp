/*++

Copyright (C) 2020 Autodesk Inc.

All rights reserved.

Abstract: This is the class declaration of CDriver

*/


#ifndef __LIBMCDRIVER_SCANLAB_DRIVER
#define __LIBMCDRIVER_SCANLAB_DRIVER

#include "libmcdriver_scanlab_interfaces.hpp"

// Parent classes
#include "libmcdriver_scanlab_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.


namespace LibMCDriver_ScanLab {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CDriver 
**************************************************************************************************************************/

class CDriver : public virtual IDriver, public virtual CBase {
private:

protected:

public:

	void GetVersion(LibMCDriver_ScanLab_uint32& nMajor, LibMCDriver_ScanLab_uint32& nMinor, LibMCDriver_ScanLab_uint32& nMicro, std::string& sBuild) override;

};

} // namespace Impl
} // namespace LibMCDriver_ScanLab

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_SCANLAB_DRIVER
