/*++

Copyright (C) 2020 Autodesk Inc.

All rights reserved.

Abstract: This is the class declaration for persistent plugin data

*/


#ifndef __LIBMCPLUGIN_DATA
#define __LIBMCPLUGIN_DATA

#include "libmcplugin_interfaces.hpp"
#include <memory>

namespace LibMCPlugin {
namespace Impl {

	class CPluginData {

	public:
		virtual ~CPluginData ()
		{
		}
		
	};

	typedef std::shared_ptr<CPluginData> PPluginData;


} // namespace Impl
} // namespace LibMCPlugin

#endif // __LIBMCPLUGIN_DATA
