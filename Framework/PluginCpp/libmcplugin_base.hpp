/*++

Copyright (C) 2020 Autodesk Inc.

All rights reserved.

Abstract: This is the class declaration of CBase

*/


#ifndef __LIBMCPLUGIN_BASE
#define __LIBMCPLUGIN_BASE

#include "libmcplugin_interfaces.hpp"
#include <vector>
#include <list>
#include <memory>


// Include custom headers here.


namespace LibMCPlugin {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CBase 
**************************************************************************************************************************/

class CBase : public virtual IBase {
private:

	std::unique_ptr<std::string> m_pLastError;
	LibMCPlugin_uint32 m_nReferenceCount = 1;

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
} // namespace LibMCPlugin

#endif // __LIBMCPLUGIN_BASE
