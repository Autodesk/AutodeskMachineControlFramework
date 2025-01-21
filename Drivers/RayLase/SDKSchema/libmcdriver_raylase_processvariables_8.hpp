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


#ifndef __LIBMCDRIVER_RAYLASE_RLPROCESSVARIABLES_17
#define __LIBMCDRIVER_RAYLASE_RLPROCESSVARIABLES_17

#include <string>
#include <memory>

namespace LibMCDriver_Raylase {
    namespace Impl {

        class CRaylaseAPIField_rlProcessVariables_v17 : public CRaylaseAPIField {
        public:
            CRaylaseAPIField_rlProcessVariables_v17()
                : CRaylaseAPIField("rlProcessVariables", 17, 1408)
            {
                registerVariable("SpeedScale", 8, eRaylaseAPIVariableType::evtDouble);
                registerVariable("JumpSpeed", 16, eRaylaseAPIVariableType::evtDouble);
                registerVariable("JumpDelay", 24, eRaylaseAPIVariableType::evtDouble);
                registerVariable("EnableVariableJumpDelay", 32, eRaylaseAPIVariableType::evtBool);
                registerVariable("MinJumpDelay", 40, eRaylaseAPIVariableType::evtDouble);
                registerVariable("JumpLengthLimit", 48, eRaylaseAPIVariableType::evtDouble);
                registerVariable("MarkSpeed", 56, eRaylaseAPIVariableType::evtDouble);
                registerVariable("MarkDelay", 64, eRaylaseAPIVariableType::evtDouble);
                registerVariable("MagnificationSpeed", 72, eRaylaseAPIVariableType::evtDouble);
                registerVariable("MagnificationSpeed", 72, eRaylaseAPIVariableType::evtDouble);
                registerVariable("EnableVariablePolyDelay", 80, eRaylaseAPIVariableType::evtBool);
                registerVariable("PolyDelay", 88, eRaylaseAPIVariableType::evtDouble);
                registerVariable("LaserOnDelay", 96, eRaylaseAPIVariableType::evtDouble);
                registerVariable("LaserOffDelay", 104, eRaylaseAPIVariableType::evtDouble);
                registerVariable("MaxDwellTime", 112, eRaylaseAPIVariableType::evtDouble);
                registerVariable("LmFrequency", 120, eRaylaseAPIVariableType::evtDouble);
                registerVariable("LmWidth", 128, eRaylaseAPIVariableType::evtDouble);
                registerVariable("Power", 136, eRaylaseAPIVariableType::evtUint16);
                registerVariable("Power1", 138, eRaylaseAPIVariableType::evtUint16);
                registerVariable("Transformation.A00", 144, eRaylaseAPIVariableType::evtDouble);
                registerVariable("Transformation.A01", 152, eRaylaseAPIVariableType::evtDouble);
                registerVariable("Transformation.A02", 160, eRaylaseAPIVariableType::evtDouble);
                registerVariable("Transformation.A10", 168, eRaylaseAPIVariableType::evtDouble);
                registerVariable("Transformation.A11", 176, eRaylaseAPIVariableType::evtDouble);
                registerVariable("Transformation.A12", 184, eRaylaseAPIVariableType::evtDouble);
                registerVariable("Transformation.A20", 192, eRaylaseAPIVariableType::evtDouble);
                registerVariable("Transformation.A21", 200, eRaylaseAPIVariableType::evtDouble);
                registerVariable("Transformation.A22", 208, eRaylaseAPIVariableType::evtDouble);
                registerVariable("Transformation.b0", 216, eRaylaseAPIVariableType::evtDouble);
                registerVariable("Transformation.b1", 224, eRaylaseAPIVariableType::evtDouble);
                registerVariable("Transformation.b2", 232, eRaylaseAPIVariableType::evtDouble);
                registerVariable("TileOffset", 240, eRaylaseAPIVariableType::evtDouble);
                registerVariable("OnOffPatternCount", 248, eRaylaseAPIVariableType::evtUint32);
                registerVariable("EnableDashedLine", 1280, eRaylaseAPIVariableType::evtBool);
                registerVariable("EvaluationLeadTime", 1288, eRaylaseAPIVariableType::evtDouble);
                registerVariable("SkyWriting.Mode", 1296, eRaylaseAPIVariableType::evtEnum32);
                registerVariable("SkyWriting.MinCoH", 1304, eRaylaseAPIVariableType::evtDouble);
                registerVariable("SkyWriting.AccelerationDelay", 1312, eRaylaseAPIVariableType::evtDouble);
                registerVariable("SkyWriting.DecelerationDelay", 1320, eRaylaseAPIVariableType::evtDouble);
                registerVariable("SkyWriting.ExtensionTime", 1328, eRaylaseAPIVariableType::evtDouble);
                registerVariable("SkyWriting.LaserOnDelay", 1336, eRaylaseAPIVariableType::evtDouble);
                registerVariable("SkyWriting.LaserOffDelay", 1344, eRaylaseAPIVariableType::evtDouble);
                registerVariable("Wobble.Amplitude.X", 1360, eRaylaseAPIVariableType::evtDouble);
                registerVariable("Wobble.Amplitude.Y", 1368, eRaylaseAPIVariableType::evtDouble);
                registerVariable("Wobble.Frequency.X", 1376, eRaylaseAPIVariableType::evtDouble);
                registerVariable("Wobble.Frequency.Y", 1384, eRaylaseAPIVariableType::evtDouble);
                registerVariable("Wobble.Phase.X", 1392, eRaylaseAPIVariableType::evtDouble);
                registerVariable("Wobble.Phase.Y", 1400, eRaylaseAPIVariableType::evtDouble);
            }

            virtual ~CRaylaseAPIField_rlProcessVariables_v17()
            {

            }

        };

    }

}
#endif //__LIBMCDRIVER_RAYLASE_RLPROCESSVARIABLES_17

