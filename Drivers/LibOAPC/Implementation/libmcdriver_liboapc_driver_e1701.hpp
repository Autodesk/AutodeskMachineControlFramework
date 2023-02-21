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


Abstract: This is the class declaration of CDriver_E1701

*/


#ifndef __LIBMCDRIVER_LIBOAPC_DRIVER_E1701
#define __LIBMCDRIVER_LIBOAPC_DRIVER_E1701

#include "libmcdriver_liboapc_interfaces.hpp"

// Parent classes
#include "libmcdriver_liboapc_driver.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "libe1701_header.hpp"


namespace LibMCDriver_LibOAPC {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CDriver_E1701 
**************************************************************************************************************************/

class CDriver_E1701 : public virtual IDriver_E1701, public virtual CDriver {
private:


protected:

    std::string m_sName;
    LibMCEnv::PDriverEnvironment m_pDriverEnvironment;
    LibMCEnv::PWorkingDirectory m_pDriverDirectory;
    LibMCEnv::PWorkingFile m_pLibE1701DLL;
    LibMCEnv::PWorkingFile m_pLibOAPCDLL;

    std::vector<uint8_t> m_CorrectionFileData;
    int32_t m_HardwareVersion;
    int32_t m_FirmwareVersion;

    bool m_bLibE1701Loaded;
    libE1701::sLibE1701 m_LibE1701;
    libE1701::e1701ID m_ConnectionID;

    double m_dOpticalFieldMinX;
    double m_dOpticalFieldMinY;
    double m_dOpticalFieldMaxX;
    double m_dOpticalFieldMaxY;

    int32_t m_nLogicalFieldMinX;
    int32_t m_nLogicalFieldMinY;
    int32_t m_nLogicalFieldMaxX;
    int32_t m_nLogicalFieldMaxY;

    bool m_bPositionIsInvalid;

    void clearLibraryBindings();
    void loadLibraryBindings (const std::string & sLibE1701Resource, const std::string& sLibOAPCResource);

    void CheckE1701Error (libE1701::e1701Error errorCode);

    void internalBegin();
    void internalExecute();
    void internalJumpTo(double dXInMM, double dYInMM, double dSpeedInMM);
    void internalMarkTo(double dXInMM, double dYInMM, double dSpeedInMM);

public:

    CDriver_E1701(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment);

    std::string GetName() override;

    std::string GetType() override;

    void GetVersion(LibMCDriver_LibOAPC_uint32& nMajor, LibMCDriver_LibOAPC_uint32& nMinor, LibMCDriver_LibOAPC_uint32& nMicro, std::string& sBuild) override;

    void QueryParameters() override;

    void QueryParametersEx(LibMCEnv::PDriverStatusUpdateSession pDriverUpdateInstance) override;

    void SetCorrectionFile(const LibMCDriver_LibOAPC_uint64 nCorrectionFileBufferSize, const LibMCDriver_LibOAPC_uint8 * pCorrectionFileBuffer) override;

	void Connect(const std::string & sConnectionString, const std::string & sPassword) override;

	void Disconnect() override;

	void DrawLayer(const std::string & sStreamUUID, const LibMCDriver_LibOAPC_uint32 nLayerIndex) override;

    void SetOpticalField(const LibMCDriver_LibOAPC_double dMinX, const LibMCDriver_LibOAPC_double dMinY, const LibMCDriver_LibOAPC_double dMaxX, const LibMCDriver_LibOAPC_double dMaxY) override;

    void SetLogicalField(const LibMCDriver_LibOAPC_int32 nMinX, const LibMCDriver_LibOAPC_int32 nMinY, const LibMCDriver_LibOAPC_int32 nMaxX, const LibMCDriver_LibOAPC_int32 nMaxY) override;


};

} // namespace Impl
} // namespace LibMCDriver_LibOAPC

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_LIBOAPC_DRIVER_E1701
