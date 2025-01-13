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


#ifndef __LIBMCDRIVER_RAYLASE_RLSCANNERCONFIG_12
#define __LIBMCDRIVER_RAYLASE_RLSCANNERCONFIG_12

#include <string>
#include <memory>

class CRaylaseAPIField_rlScannerConfig_v12 : public CRaylaseAPIField {
  public:
    CRaylaseAPIField_rlScannerConfig_v12 ()
      : CRaylaseAPIField ("rlScannerConfig", 12, 528)
    {
      registerVariable ("FieldSize.X", 8, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("FieldSize.Y", 16, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("FieldSize.Z", 24, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("HeadCount", 32, eRaylaseAPIVariableType::evtInt32);
      registerVariable ("Head0.FieldTransform.A00", 48, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head0.FieldTransform.A01", 56, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head0.FieldTransform.A02", 64, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head0.FieldTransform.A10", 72, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head0.FieldTransform.A11", 80, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head0.FieldTransform.A12", 88, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head0.FieldTransform.A20", 96, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head0.FieldTransform.A21", 104, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head0.FieldTransform.A22", 112, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head0.FieldTransform.b0", 120, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head0.FieldTransform.b1", 128, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head0.FieldTransform.b2", 136, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head0.Axes0.AccelerationTime", 144, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head0.Axes0.TrackingError", 152, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head0.Axes0.HeadBias", 160, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head0.Axes1.AccelerationTime", 168, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head0.Axes1.TrackingError", 176, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head0.Axes1.HeadBias", 184, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head0.Axes2.AccelerationTime", 192, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head0.Axes2.TrackingError", 200, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head0.Axes2.HeadBias", 208, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head0.Axes3.AccelerationTime", 216, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head0.Axes3.TrackingError", 224, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head0.Axes3.HeadBias", 232, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head0.Axes4.AccelerationTime", 240, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head0.Axes4.TrackingError", 248, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head0.Axes4.HeadBias", 256, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head0.EnableFieldCorrection", 264, eRaylaseAPIVariableType::evtBool);
      registerVariable ("Head1.FieldTransform.A00", 280, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head1.FieldTransform.A01", 288, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head1.FieldTransform.A02", 296, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head1.FieldTransform.A10", 304, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head1.FieldTransform.A11", 312, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head1.FieldTransform.A12", 320, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head1.FieldTransform.A20", 328, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head1.FieldTransform.A21", 336, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head1.FieldTransform.A22", 344, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head1.FieldTransform.b0", 352, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head1.FieldTransform.b1", 360, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head1.FieldTransform.b2", 368, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head1.Axes0.AccelerationTime", 376, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head1.Axes0.TrackingError", 384, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head1.Axes0.HeadBias", 392, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head1.Axes1.AccelerationTime", 400, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head1.Axes1.TrackingError", 408, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head1.Axes1.HeadBias", 416, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head1.Axes2.AccelerationTime", 424, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head1.Axes2.TrackingError", 432, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head1.Axes2.HeadBias", 440, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head1.Axes3.AccelerationTime", 448, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head1.Axes3.TrackingError", 456, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head1.Axes3.HeadBias", 464, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head1.Axes4.AccelerationTime", 472, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head1.Axes4.TrackingError", 480, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head1.Axes4.HeadBias", 488, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("Head1.EnableFieldCorrection", 496, eRaylaseAPIVariableType::evtBool);
      registerVariable ("ScanHeadDelay", 504, eRaylaseAPIVariableType::evtDouble);
      registerVariable ("HeadFormat", 512, eRaylaseAPIVariableType::evtEnum32);
      registerVariable ("MaxMagnification", 520, eRaylaseAPIVariableType::evtDouble);
    }
    
    virtual ~CRaylaseAPIField_rlScannerConfig_v12 ()
    {
    }
};


#endif //__LIBMCDRIVER_RAYLASE_RLSCANNERCONFIG_12

