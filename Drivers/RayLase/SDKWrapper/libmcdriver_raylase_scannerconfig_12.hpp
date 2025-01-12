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
      registerVariable ("FieldSize.X", 8, evtDouble);
      registerVariable ("FieldSize.Y", 16, evtDouble);
      registerVariable ("FieldSize.Z", 24, evtDouble);
      registerVariable ("HeadCount", 32, evtInt32);
      registerVariable ("Head0.FieldTransform.A00", 48, evtDouble);
      registerVariable ("Head0.FieldTransform.A01", 56, evtDouble);
      registerVariable ("Head0.FieldTransform.A02", 64, evtDouble);
      registerVariable ("Head0.FieldTransform.A10", 72, evtDouble);
      registerVariable ("Head0.FieldTransform.A11", 80, evtDouble);
      registerVariable ("Head0.FieldTransform.A12", 88, evtDouble);
      registerVariable ("Head0.FieldTransform.A20", 96, evtDouble);
      registerVariable ("Head0.FieldTransform.A21", 104, evtDouble);
      registerVariable ("Head0.FieldTransform.A22", 112, evtDouble);
      registerVariable ("Head0.FieldTransform.b0", 120, evtDouble);
      registerVariable ("Head0.FieldTransform.b1", 128, evtDouble);
      registerVariable ("Head0.FieldTransform.b2", 136, evtDouble);
      registerVariable ("Head0.Axes0.AccelerationTime", 144, evtDouble);
      registerVariable ("Head0.Axes0.TrackingError", 152, evtDouble);
      registerVariable ("Head0.Axes0.HeadBias", 160, evtDouble);
      registerVariable ("Head0.Axes1.AccelerationTime", 168, evtDouble);
      registerVariable ("Head0.Axes1.TrackingError", 176, evtDouble);
      registerVariable ("Head0.Axes1.HeadBias", 184, evtDouble);
      registerVariable ("Head0.Axes2.AccelerationTime", 192, evtDouble);
      registerVariable ("Head0.Axes2.TrackingError", 200, evtDouble);
      registerVariable ("Head0.Axes2.HeadBias", 208, evtDouble);
      registerVariable ("Head0.Axes3.AccelerationTime", 216, evtDouble);
      registerVariable ("Head0.Axes3.TrackingError", 224, evtDouble);
      registerVariable ("Head0.Axes3.HeadBias", 232, evtDouble);
      registerVariable ("Head0.Axes4.AccelerationTime", 240, evtDouble);
      registerVariable ("Head0.Axes4.TrackingError", 248, evtDouble);
      registerVariable ("Head0.Axes4.HeadBias", 256, evtDouble);
      registerVariable ("Head0.EnableFieldCorrection", 264, evtBool);
      registerVariable ("Head1.FieldTransform.A00", 280, evtDouble);
      registerVariable ("Head1.FieldTransform.A01", 288, evtDouble);
      registerVariable ("Head1.FieldTransform.A02", 296, evtDouble);
      registerVariable ("Head1.FieldTransform.A10", 304, evtDouble);
      registerVariable ("Head1.FieldTransform.A11", 312, evtDouble);
      registerVariable ("Head1.FieldTransform.A12", 320, evtDouble);
      registerVariable ("Head1.FieldTransform.A20", 328, evtDouble);
      registerVariable ("Head1.FieldTransform.A21", 336, evtDouble);
      registerVariable ("Head1.FieldTransform.A22", 344, evtDouble);
      registerVariable ("Head1.FieldTransform.b0", 352, evtDouble);
      registerVariable ("Head1.FieldTransform.b1", 360, evtDouble);
      registerVariable ("Head1.FieldTransform.b2", 368, evtDouble);
      registerVariable ("Head1.Axes0.AccelerationTime", 376, evtDouble);
      registerVariable ("Head1.Axes0.TrackingError", 384, evtDouble);
      registerVariable ("Head1.Axes0.HeadBias", 392, evtDouble);
      registerVariable ("Head1.Axes1.AccelerationTime", 400, evtDouble);
      registerVariable ("Head1.Axes1.TrackingError", 408, evtDouble);
      registerVariable ("Head1.Axes1.HeadBias", 416, evtDouble);
      registerVariable ("Head1.Axes2.AccelerationTime", 424, evtDouble);
      registerVariable ("Head1.Axes2.TrackingError", 432, evtDouble);
      registerVariable ("Head1.Axes2.HeadBias", 440, evtDouble);
      registerVariable ("Head1.Axes3.AccelerationTime", 448, evtDouble);
      registerVariable ("Head1.Axes3.TrackingError", 456, evtDouble);
      registerVariable ("Head1.Axes3.HeadBias", 464, evtDouble);
      registerVariable ("Head1.Axes4.AccelerationTime", 472, evtDouble);
      registerVariable ("Head1.Axes4.TrackingError", 480, evtDouble);
      registerVariable ("Head1.Axes4.HeadBias", 488, evtDouble);
      registerVariable ("Head1.EnableFieldCorrection", 496, evtBool);
      registerVariable ("ScanHeadDelay", 504, evtDouble);
      registerVariable ("HeadFormat", 512, evtEnum);
      registerVariable ("MaxMagnification", 520, evtDouble);
    }
    
    virtual ~CRaylaseAPIField_rlScannerConfig_v12 ()
    {
    }
};


#endif //__LIBMCDRIVER_RAYLASE_RLSCANNERCONFIG_12

