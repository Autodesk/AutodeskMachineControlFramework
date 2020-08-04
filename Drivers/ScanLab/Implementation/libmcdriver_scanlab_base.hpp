/*++

Copyright (C) 2020 Autodesk Inc.

All rights reserved.

Abstract: This is the class declaration of CBase

*/


#ifndef __LIBMCDRIVER_SCANLAB_BASE
#define __LIBMCDRIVER_SCANLAB_BASE

#include "libmcdriver_scanlab_interfaces.hpp"
#include <vector>
#include <list>
#include <memory>


// Include custom headers here.


namespace LibMCDriver_ScanLab {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CBase 
**************************************************************************************************************************/

class CBase : public virtual IBase {
private:

	uint32_t m_nReferenceCount = 1;
	std::unique_ptr<std::string> m_pLastError;

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

	bool GetLastErrorMessage(std::string & sErrorMessage) override;

	void ClearErrorMessages() override;

	void RegisterErrorMessage(const std::string & sErrorMessage) override;

	void IncRefCount() override;

	bool DecRefCount() override;


	/**
	* Public member functions to implement.
	*/

};

} // namespace Impl
} // namespace LibMCDriver_ScanLab

#endif // __LIBMCDRIVER_SCANLAB_BASE
