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
      registerVariable ("Module0.Enabled", 12, evtBool);
      registerVariable ("Module0.SpiSyncMode", 16, evtEnum);
      registerVariable ("Module0.BitsPerWord", 20, evtUint16);
      registerVariable ("Module0.PreDelay", 24, evtDouble);
      registerVariable ("Module0.PostDelay", 32, evtDouble);
      registerVariable ("Module0.FrameDelay", 40, evtDouble);
      registerVariable ("Module0.OutputSource", 48, evtEnum);
      registerVariable ("Module0.BitOrder", 52, evtEnum);
      registerVariable ("Module0.ClockPeriod", 56, evtDouble);
      registerVariable ("Module1.Enabled", 68, evtBool);
      registerVariable ("Module1.SpiSyncMode", 72, evtEnum);
      registerVariable ("Module1.BitsPerWord", 76, evtUint16);
      registerVariable ("Module1.PreDelay", 80, evtDouble);
      registerVariable ("Module1.PostDelay", 88, evtDouble);
      registerVariable ("Module1.FrameDelay", 96, evtDouble);
      registerVariable ("Module1.OutputSource", 104, evtEnum);
      registerVariable ("Module1.BitOrder", 108, evtEnum);
      registerVariable ("Module1.ClockPeriod", 112, evtDouble);
      registerVariable ("Module2.Enabled", 124, evtBool);
      registerVariable ("Module2.SpiSyncMode", 128, evtEnum);
      registerVariable ("Module2.BitsPerWord", 132, evtUint16);
      registerVariable ("Module2.PreDelay", 136, evtDouble);
      registerVariable ("Module2.PostDelay", 144, evtDouble);
      registerVariable ("Module2.FrameDelay", 152, evtDouble);
      registerVariable ("Module2.OutputSource", 160, evtEnum);
      registerVariable ("Module2.BitOrder", 164, evtEnum);
      registerVariable ("Module2.ClockPeriod", 168, evtDouble);
      registerVariable ("Module3.Enabled", 180, evtBool);
      registerVariable ("Module3.SpiSyncMode", 184, evtEnum);
      registerVariable ("Module3.BitsPerWord", 188, evtUint16);
      registerVariable ("Module3.PreDelay", 192, evtDouble);
      registerVariable ("Module3.PostDelay", 200, evtDouble);
      registerVariable ("Module3.FrameDelay", 208, evtDouble);
      registerVariable ("Module3.OutputSource", 216, evtEnum);
      registerVariable ("Module3.BitOrder", 220, evtEnum);
      registerVariable ("Module3.ClockPeriod", 224, evtDouble);
    }
    
    virtual ~CRaylaseAPIField_rlSpiConfig_v3 ()
    {
    }
};


#endif //__LIBMCDRIVER_RAYLASE_RLSPICONFIG_3

