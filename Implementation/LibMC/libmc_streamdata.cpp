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


Abstract: This is a stub class definition of CStreamData

*/

#include "libmc_streamdata.hpp"
#include "libmc_interfaceexception.hpp"

// Include custom headers here.
#include <iostream>
#include "common_importstream_native.hpp"

using namespace LibMC::Impl;

/*************************************************************************************************************************
 Class definition of CStreamData 
**************************************************************************************************************************/


CStreamData::CStreamData()
    : m_sMIMEType("image/jpeg")
{
    throw ELibMCInterfaceException(LIBMC_ERROR_NOTIMPLEMENTED);

    //AMCCommon::CImportStream_Native importStream("T:/temp/frame1.jpg") ;
    //importStream.readIntoMemory(m_Buffer);
}

CStreamData::~CStreamData()
{

}


void CStreamData::GetData(LibMC_uint64 nDataBufferSize, LibMC_uint64* pDataNeededCount, LibMC_uint8 * pDataBuffer)
{
    std::cout << "GetData" << std::endl;

    if (pDataNeededCount != nullptr)
        *pDataNeededCount = m_Buffer.size();

    if (pDataBuffer != nullptr) {
        if (nDataBufferSize < m_Buffer.size())
            throw LIBMC_ERROR_BUFFERTOOSMALL;

        uint8_t* pTarget = pDataBuffer;
        for (auto value : m_Buffer) {
            *pTarget = value;
            pTarget++;
        }
    }
}

std::string CStreamData::GetMIMEType()
{
    return m_sMIMEType;
}

