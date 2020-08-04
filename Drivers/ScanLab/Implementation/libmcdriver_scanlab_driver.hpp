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

	/**
	* Put private members here.
	*/

protected:

	/**
	* Put protected members here.
	*/

public:

	/**
	* Put additional public members here. They will not be visible in the external API.
	*/


	/**
	* Public member functions to implement.
	*/


};

} // namespace Impl
} // namespace LibMCDriver_ScanLab

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_SCANLAB_DRIVER
