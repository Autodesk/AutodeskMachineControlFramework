/*++

Copyright (C) 2024 Autodesk Inc.

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Autodesk Inc. nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL AUTODESK INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


Abstract: This is the class declaration of CDeviceList

*/


#ifndef __LIBMCDRIVER_CAMERA_DEVICELIST
#define __LIBMCDRIVER_CAMERA_DEVICELIST

#include "libmcdriver_camera_interfaces.hpp"

// Parent classes
#include "libmcdriver_camera_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "libmcdriver_camera_deviceinfoinstance.hpp"

#include <vector>
#include <map>


namespace LibMCDriver_Camera {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CDeviceList 
**************************************************************************************************************************/

#define CAMERADEVICELIST_MAXCOUNT (1024UL * 1024UL)

class CDeviceList : public virtual IDeviceList, public virtual CBase {
private:

    std::vector<PDeviceInfoInstance> m_DeviceInfos;
    std::map<std::string, PDeviceInfoInstance> m_OperatingSystemNameMap;

public:

    CDeviceList();

    virtual ~CDeviceList();

	LibMCDriver_Camera_uint32 GetCount() override;

	IDeviceInfo * GetDeviceInfo(const LibMCDriver_Camera_uint32 nIndex) override;

	IDeviceInfo * FindDeviceInfoByOperatingSystemName(const std::string & sOperatingSystemName, const bool bMustExist) override;

    void addInfo(PDeviceInfoInstance pDeviceInfo);

};

} // namespace Impl
} // namespace LibMCDriver_Camera

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_CAMERA_DEVICELIST
