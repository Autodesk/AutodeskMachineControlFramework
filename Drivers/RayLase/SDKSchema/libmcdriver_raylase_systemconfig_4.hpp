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


#ifndef __LIBMCDRIVER_RAYLASE_RLSYSTEMCONFIG_4
#define __LIBMCDRIVER_RAYLASE_RLSYSTEMCONFIG_4

#include <string>
#include <memory>

class CRaylaseAPIField_rlSystemConfig_v4 : public CRaylaseAPIField {
  public:
    CRaylaseAPIField_rlSystemConfig_v4 ()
      : CRaylaseAPIField ("rlSystemConfig", 4, 184)
    {
      registerVariable ("TimeZone.IdLen", 4, eRaylaseAPIVariableType::evtUint32);
      registerVariable ("EnableStandAloneMode", 136, eRaylaseAPIVariableType::evtBool);
      registerVariable ("StandAloneListID", 140, eRaylaseAPIVariableType::evtInt32);
      registerVariable ("StandAloneExecuteOnErrorListID", 144, eRaylaseAPIVariableType::evtInt32);
      registerVariable ("ScannerMonitoring.Enabled", 148, eRaylaseAPIVariableType::evtBool);
      registerVariable ("ScannerMonitoring.Command", 152, eRaylaseAPIVariableType::evtUint32);
      registerVariable ("ScannerMonitoring.Mask", 156, eRaylaseAPIVariableType::evtUint32);
      registerVariable ("ScannerMonitoring.GoodValue", 160, eRaylaseAPIVariableType::evtUint32);
      registerVariable ("ScannerMonitoring.Period", 164, eRaylaseAPIVariableType::evtInt32);
      registerVariable ("ScannerMonitoring.MinConsecutiveErrors", 168, eRaylaseAPIVariableType::evtInt32);
      registerVariable ("ScannerMonitoring.ContinueOnError", 172, eRaylaseAPIVariableType::evtBool);
      registerVariable ("ScannerMonitoring.GoodValue", 160, eRaylaseAPIVariableType::evtUint32);
      registerVariable ("ScannerMonitoring.ScanHeadAxes0", 176, eRaylaseAPIVariableType::evtUint32);
      registerVariable ("ScannerMonitoring.ScanHeadAxes1", 180, eRaylaseAPIVariableType::evtUint32);
    }
    
    virtual ~CRaylaseAPIField_rlSystemConfig_v4 ()
    {
    }
};


#endif //__LIBMCDRIVER_RAYLASE_RLSYSTEMCONFIG_4

