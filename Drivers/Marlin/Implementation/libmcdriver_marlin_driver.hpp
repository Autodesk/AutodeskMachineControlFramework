/*++

Copyright (C) 2020 Autodesk Inc.

All rights reserved.

Abstract: This is the class declaration of CDriver

*/


#ifndef __LIBMCDRIVER_MARLIN_DRIVER
#define __LIBMCDRIVER_MARLIN_DRIVER

#include "libmcdriver_marlin_interfaces.hpp"

// Parent classes
#include "libmcdriver_marlin_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.


namespace LibMCDriver_Marlin {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CDriver 
**************************************************************************************************************************/

class CDriver : public virtual IDriver, public virtual CBase {
private:


protected:

	std::string m_sName;
	std::string m_sType;

public:

	CDriver(const std::string& sName, const std::string& sType);

	std::string GetName() override;

	std::string GetType() override;

	void GetVersion(LibMCDriver_Marlin_uint32 & nMajor, LibMCDriver_Marlin_uint32 & nMinor, LibMCDriver_Marlin_uint32 & nMicro, std::string & sBuild) override;

	void GetHeaderInformation(std::string& sNameSpace, std::string& sBaseName) override;

};

} // namespace Impl
} // namespace LibMCDriver_Marlin

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_MARLIN_DRIVER
