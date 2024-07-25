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


Abstract: This is the class declaration of CRTCJob

*/


#ifndef __LIBMCDRIVER_SCANLAB_RTCJOB
#define __LIBMCDRIVER_SCANLAB_RTCJOB

#include "libmcdriver_scanlab_interfaces.hpp"

// Parent classes
#include "libmcdriver_scanlab_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.


namespace LibMCDriver_ScanLab {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CRTCJob 
**************************************************************************************************************************/

class CRTCJob : public virtual IRTCJob, public virtual CBase {
private:

	/**
	* Put private members here.
	*/

protected:

	/**
	* Put protected members here.
	*/

public:

	/**
	* Put additional public members here. They will not be visible in the external API.
	*/


	/**
	* Public member functions to implement.
	*/

	void DrawPolyline(const LibMCDriver_ScanLab_uint64 nPointsBufferSize, const LibMCDriver_ScanLab::sPoint2D * pPointsBuffer, const LibMCDriver_ScanLab_single fMarkSpeed, const LibMCDriver_ScanLab_single fJumpSpeed, const LibMCDriver_ScanLab_single fPower, const LibMCDriver_ScanLab_single fZValue) override;

	void DrawPolylineOIE(const LibMCDriver_ScanLab_uint64 nPointsBufferSize, const LibMCDriver_ScanLab::sPoint2D * pPointsBuffer, const LibMCDriver_ScanLab_single fMarkSpeed, const LibMCDriver_ScanLab_single fJumpSpeed, const LibMCDriver_ScanLab_single fPower, const LibMCDriver_ScanLab_single fZValue, const LibMCDriver_ScanLab_uint32 nOIEPIDControlIndex) override;

	void DrawHatches(const LibMCDriver_ScanLab_uint64 nHatchesBufferSize, const LibMCDriver_ScanLab::sHatch2D * pHatchesBuffer, const LibMCDriver_ScanLab_single fMarkSpeed, const LibMCDriver_ScanLab_single fJumpSpeed, const LibMCDriver_ScanLab_single fPower, const LibMCDriver_ScanLab_single fZValue) override;

	void AddSetPower(const LibMCDriver_ScanLab_single fPowerInPercent) override;

	void AddSetAnalogOut(const LibMCDriver_ScanLab::eLaserPort eLaserPort, const LibMCDriver_ScanLab_single fOutputValue) override;

	void AddSetDigitalOut(const LibMCDriver_ScanLab::eLaserPort eLaserPort, const LibMCDriver_ScanLab_single fOutputValue) override;

	void AddSetPowerForPIDControl(const LibMCDriver_ScanLab_single fPowerInPercent) override;

	void AddSetJumpSpeed(const LibMCDriver_ScanLab_single fJumpSpeedInMMPerSecond) override;

	void AddSetMarkSpeed(const LibMCDriver_ScanLab_single fMarkSpeedInMMPerSecond) override;

	void AddJumpMovement(const LibMCDriver_ScanLab_double dTargetX, const LibMCDriver_ScanLab_double dTargetY) override;

	void AddMarkMovement(const LibMCDriver_ScanLab_double dTargetX, const LibMCDriver_ScanLab_double dTargetY) override;

	void AddTimedMarkMovement(const LibMCDriver_ScanLab_double dTargetX, const LibMCDriver_ScanLab_double dTargetY, const LibMCDriver_ScanLab_double dDurationInMicroseconds) override;

	void AddFreeVariable(const LibMCDriver_ScanLab_uint32 nVariableNo, const LibMCDriver_ScanLab_uint32 nValue) override;

};

} // namespace Impl
} // namespace LibMCDriver_ScanLab

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_SCANLAB_RTCJOB
