/*++

Copyright (C) 2020 Autodesk Inc.

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


Abstract: This is a stub class definition of CRaspiCameraDeviceIterator

*/

#include "libmcdriver_camera_raspicameradeviceiterator.hpp"
#include "libmcdriver_camera_raspicameradevice.hpp"
#include "libmcdriver_camera_interfaceexception.hpp"

// Include custom headers here.


using namespace LibMCDriver_Camera::Impl;

/*************************************************************************************************************************
 Class definition of CRaspiCameraDeviceIterator 
**************************************************************************************************************************/

CRaspiCameraDeviceIterator::CRaspiCameraDeviceIterator()
    : CIterator(), m_nCurrentIndex (-1)
{

}

bool CRaspiCameraDeviceIterator::MoveNext()
{
    if ((m_nCurrentIndex + 1) < (int64_t) m_DeviceStrings.size()) {
        m_nCurrentIndex++;
        return true;
    }

    return false;
}

bool CRaspiCameraDeviceIterator::MovePrevious()
{
    if (m_nCurrentIndex > 0) {
        m_nCurrentIndex--;
        return true;
    } 

    return false;
}

IBase* CRaspiCameraDeviceIterator::GetCurrent()
{
    return (IBase*)GetCameraDevice();
}

IRaspiCameraDevice* CRaspiCameraDeviceIterator::GetCameraDevice()
{
    if ((m_nCurrentIndex >= 0) && (m_nCurrentIndex < (int64_t)m_DeviceStrings.size())) {
        return new CRaspiCameraDevice(m_DeviceStrings [m_nCurrentIndex]);
    }

    throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_ITERATORERROR);
}

IIterator* CRaspiCameraDeviceIterator::Clone()
{
    std::unique_ptr<CRaspiCameraDeviceIterator> pNewIterator(new CRaspiCameraDeviceIterator());
    pNewIterator->m_DeviceStrings = m_DeviceStrings;

    return pNewIterator.release();
}

LibMCDriver_Camera_uint64 CRaspiCameraDeviceIterator::Count()
{
    return m_DeviceStrings.size();
}

void CRaspiCameraDeviceIterator::addDevice(const std::string& sDeviceString)
{
    if (sDeviceString.empty())
        throw ELibMCDriver_CameraInterfaceException(LIBMCDRIVER_CAMERA_ERROR_INVALIDPARAM);

    m_DeviceStrings.push_back(sDeviceString);
}


