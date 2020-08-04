/*++

Copyright (C) 2020 Autodesk Inc.

All rights reserved.

Abstract: This is the class declaration of CState

*/


#ifndef __LIBMCPLUGIN_STATE
#define __LIBMCPLUGIN_STATE

#include "libmcplugin_interfaces.hpp"
#include "libmcplugin_data.hpp"

// Parent classes
#include "libmcplugin_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.


namespace LibMCPlugin {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CState 
**************************************************************************************************************************/

template<class DataClass> class CState : public virtual IState, public virtual CBase {
private:

protected:

	std::shared_ptr<DataClass> m_pPluginData;

public:

	CState(const std::string& sClassStateName, const std::string& sPassedStateName, PPluginData pPluginData)
	{
		if (sClassStateName != sPassedStateName)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDSTATENAME);

		if (pPluginData.get () == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);

		m_pPluginData = std::dynamic_pointer_cast<DataClass> (pPluginData);
		if (m_pPluginData == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDCAST);

	}

	virtual ~CState()
	{

	}


};

} // namespace Impl
} // namespace LibMCPlugin

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCPLUGIN_STATE

