/*++

Copyright (C) 2024 Autodesk Inc.

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and /or other materials provided with the distribution.
 * Neither the name of the Autodesk Inc. nor the
  names of its contributors may be used to endorse or promote products
  derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED.IN NO EVENT SHALL AUTODESK INC.BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/


#ifndef __LIBMCDRIVER_RAYLASE_RLSPICONFIG_3
#define __LIBMCDRIVER_RAYLASE_RLSPICONFIG_3

#include <string>
#include <memory>

class CRaylaseAPIField_rlSpiConfig_v3 : public CRaylaseAPIField {
  public:
    CRaylaseAPIField_rlSpiConfig_v3 ()
      : CRaylaseAPIField ("rlSpiConfig", 3, 232)
    {
      registerVariable ("Module0.Enabled", 12, eRaylaseAPIVariableType::evtBool);
      registerVariable ("Module0.SpiSyncMode", 16, eRaylaseAPIVariableType::evtEnum32);
      registerVariable ("Module0.BitsPerWord", 20, eRaylaseAPIVariableType::evtUint16);
      registerVariable ("Module0.PreDelay", 24, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Module0.PostDelay", 32, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Module0.FrameDelay", 40, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Module0.OutputSource", 48, eRaylaseAPIVariableType::evtEnum32);
      registerVariable ("Module0.BitOrder", 52, eRaylaseAPIVariableType::evtEnum32);
      registerVariable ("Module0.ClockPeriod", 56, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Module1.Enabled", 68, eRaylaseAPIVariableType::evtBool);
      registerVariable ("Module1.SpiSyncMode", 72, eRaylaseAPIVariableType::evtEnum32);
      registerVariable ("Module1.BitsPerWord", 76, eRaylaseAPIVariableType::evtUint16);
      registerVariable ("Module1.PreDelay", 80, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Module1.PostDelay", 88, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Module1.FrameDelay", 96, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Module1.OutputSource", 104, eRaylaseAPIVariableType::evtEnum32);
      registerVariable ("Module1.BitOrder", 108, eRaylaseAPIVariableType::evtEnum32);
      registerVariable ("Module1.ClockPeriod", 112, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Module2.Enabled", 124, eRaylaseAPIVariableType::evtBool);
      registerVariable ("Module2.SpiSyncMode", 128, eRaylaseAPIVariableType::evtEnum32);
      registerVariable ("Module2.BitsPerWord", 132, eRaylaseAPIVariableType::evtUint16);
      registerVariable ("Module2.PreDelay", 136, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Module2.PostDelay", 144, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Module2.FrameDelay", 152, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Module2.OutputSource", 160, eRaylaseAPIVariableType::evtEnum32);
      registerVariable ("Module2.BitOrder", 164, eRaylaseAPIVariableType::evtEnum32);
      registerVariable ("Module2.ClockPeriod", 168, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Module3.Enabled", 180, eRaylaseAPIVariableType::evtBool);
      registerVariable ("Module3.SpiSyncMode", 184, eRaylaseAPIVariableType::evtEnum32);
      registerVariable ("Module3.BitsPerWord", 188, eRaylaseAPIVariableType::evtUint16);
      registerVariable ("Module3.PreDelay", 192, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Module3.PostDelay", 200, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Module3.FrameDelay", 208, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Module3.OutputSource", 216, eRaylaseAPIVariableType::evtEnum32);
      registerVariable ("Module3.BitOrder", 220, eRaylaseAPIVariableType::evtEnum32);
      registerVariable ("Module3.ClockPeriod", 224, eRaylaseAPIVariableType::evtDouble);
    }
    
    virtual ~CRaylaseAPIField_rlSpiConfig_v3 ()
    {
    }
};


#endif //__LIBMCDRIVER_RAYLASE_RLSPICONFIG_3

