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


#ifndef __LIBMCDRIVER_RAYLASE_RLLASERCONFIG_8
#define __LIBMCDRIVER_RAYLASE_RLLASERCONFIG_8

#include <string>
#include <memory>

class CRaylaseAPIField_rlLaserConfig_v8 : public CRaylaseAPIField {
  public:
    CRaylaseAPIField_rlLaserConfig_v8 ()
      : CRaylaseAPIField ("rlLaserConfig", 8, 144)
    {
      registerVariable ("FpsPolarity", 4, eRaylaseAPIVariableType::evtEnum32);
      registerVariable ("GatePolarity", 8, eRaylaseAPIVariableType::evtEnum32);
      registerVariable ("LMPolarity", 12, eRaylaseAPIVariableType::evtEnum32);
      registerVariable ("FpsStart", 16, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("FpsWidth", 24, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("GateSetup", 32, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("GateHold", 40, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("LaserTriggerDelay", 48, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("PowerScale", 56, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("PowerScale1", 64, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("PowerChangeSetup", 72, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("PowerWriteDelay", 80, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("PowerWriteWidth", 88, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("HotPowerTarget", 96, eRaylaseAPIVariableType::evtEnum32);
      registerVariable ("HotPowerTarget1", 100, eRaylaseAPIVariableType::evtEnum32);
      registerVariable ("SimmerPower", 108, eRaylaseAPIVariableType::evtUint16);
      registerVariable ("EnableTickle", 110, eRaylaseAPIVariableType::evtBool);
      registerVariable ("TickleFrequency", 112, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("TickleWidth", 120, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("EnableLaserSync", 128, eRaylaseAPIVariableType::evtBool);
      registerVariable ("EnableGateModulatesLm", 129, eRaylaseAPIVariableType::evtBool);
      registerVariable ("EnablePowerCorrection", 130, eRaylaseAPIVariableType::evtBool);
      registerVariable ("EnablePowerCorrection1", 131, eRaylaseAPIVariableType::evtBool);
      registerVariable ("EnableVelocityCorrection", 132, eRaylaseAPIVariableType::evtBool);
      registerVariable ("EnableVelocityCorrection1", 133, eRaylaseAPIVariableType::evtBool);
      registerVariable ("EnablePowerCalibration", 134, eRaylaseAPIVariableType::evtBool);
      registerVariable ("EnablePowerCalibration1", 135, eRaylaseAPIVariableType::evtBool);
      registerVariable ("EnableEquallySpacedPulses", 136, eRaylaseAPIVariableType::evtBool);
    }
    
    virtual ~CRaylaseAPIField_rlLaserConfig_v8 ()
    {
    }
};


#endif //__LIBMCDRIVER_RAYLASE_RLLASERCONFIG_8

