/*++

Copyright (C) 2020 Autodesk Inc.

All rights reserved.

Abstract: This is the class declaration of CStateFactory

*/


#ifndef __LIBMCPLUGIN_STATEFACTORY
#define __LIBMCPLUGIN_STATEFACTORY

#include "libmcplugin_interfaces.hpp"
// Parent classes
#include "libmcplugin_base.hpp"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "libmcplugin_data.hpp"


namespace LibMCPlugin {
namespace Impl {


/*************************************************************************************************************************
	Factory State creation template
**************************************************************************************************************************/

template <class StateClass> bool createStateInstanceByName(const std::string& sStateName, IState*& pStateInstance, PPluginData pPluginData)
{
	if (sStateName == StateClass::getStateName()) {
		pStateInstance = new StateClass(sStateName, pPluginData);
		return true;
	}

	return false;
}


/*************************************************************************************************************************
 Class declaration of CStateFactory 
**************************************************************************************************************************/

class CStateFactory : public virtual IStateFactory, public virtual CBase {

protected:
	PPluginData m_pPluginData;

public:

	CStateFactory(const std::string& sInstanceName);
	IState * CreateState(const std::string & sStateName) override;

};

} // namespace Impl
} // namespace LibMCPlugin

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCPLUGIN_STATEFACTORY
