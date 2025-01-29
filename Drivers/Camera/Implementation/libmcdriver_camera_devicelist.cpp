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


Abstract: This is a stub class definition of CDeviceList

*/

#include "libmcdriver_camera_devicelist.hpp"
#include "libmcdriver_camera_interfaceexception.hpp"
#include "libmcdriver_camera_deviceinfo.hpp"

// Include custom headers here.


using namespace LibMCDriver_Camera::Impl;

/*************************************************************************************************************************
 Class definition of CDeviceList 
**************************************************************************************************************************/

CDeviceList::CDeviceList()
{

}

CDeviceList::~CDeviceList()
{

}

LibMCDriver_Camera_uint32 CDeviceList::GetCount()
{
    return (uint32_t) m_DeviceInfos.size ();
}

IDeviceInfo * CDeviceList::GetDeviceInfo(const LibMCDriver_Camera_uint32 nIndex)
{
    if (nIndex >= m_DeviceInfos.size ())
	    throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_INVALIDDEVICEINDEX);

    return new CDeviceInfo(m_DeviceInfos.at(nIndex));
}

IDeviceInfo * CDeviceList::FindDeviceInfoByOperatingSystemName(const std::string & sOperatingSystemName, const bool bMustExist)
{
    auto iIter = m_OperatingSystemNameMap.find (sOperatingSystemName);
    if (iIter != m_OperatingSystemNameMap.end())
        return new CDeviceInfo(iIter->second);

    if (bMustExist)
	    throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_CAMERADEVICENOTFOUND, "camera device not found: " + sOperatingSystemName);

    return nullptr;
}

void CDeviceList::addInfo(PDeviceInfoInstance pDeviceInfo)
{
    if (pDeviceInfo.get () == nullptr)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_INVALIDPARAM);

    if (m_DeviceInfos.size () >= CAMERADEVICELIST_MAXCOUNT)
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_CAMERADEVICELISTOVERRUN);

    m_DeviceInfos.push_back(pDeviceInfo);
    m_OperatingSystemNameMap.insert(std::make_pair(pDeviceInfo->getOperatingSystemName(), pDeviceInfo));

}


