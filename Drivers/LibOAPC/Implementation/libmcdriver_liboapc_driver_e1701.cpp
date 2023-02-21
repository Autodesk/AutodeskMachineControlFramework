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


Abstract: This is a stub class definition of CDriver_E1701

*/

#include "libmcdriver_liboapc_driver_e1701.hpp"
#include "libmcdriver_liboapc_interfaceexception.hpp"
// Include custom headers here.

#define __STRINGIZE(x) #x
#define __STRINGIZE_VALUE_OF(x) __STRINGIZE(x)

#define E1701_MINFIELDSIZE 1E-4

using namespace LibMCDriver_LibOAPC::Impl;

/*************************************************************************************************************************
 Class definition of CDriver_E1701 
**************************************************************************************************************************/

CDriver_E1701::CDriver_E1701(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment)
    : m_sName(sName),
    m_pDriverEnvironment(pDriverEnvironment),
    m_bLibE1701Loaded(false),
    m_HardwareVersion(0),
    m_FirmwareVersion(0),
    m_dOpticalFieldMinX(0.0),
    m_dOpticalFieldMinY(0.0),
    m_dOpticalFieldMaxX(100.0),
    m_dOpticalFieldMaxY(100.0),
    m_nLogicalFieldMinX(0),
    m_nLogicalFieldMinY(0),
    m_nLogicalFieldMaxX(100),
    m_nLogicalFieldMaxY(100),
    m_bPositionIsInvalid(true)

{
    if (pDriverEnvironment.get() == nullptr)
        throw ELibMCDriver_LibOAPCInterfaceException(LIBMCDRIVER_LIBOAPC_ERROR_INVALIDPARAM);

    clearLibraryBindings();
}


std::string CDriver_E1701::GetName()
{
    return m_sName;
}

std::string CDriver_E1701::GetType()
{
    return "e1701";
}

void CDriver_E1701::GetVersion(LibMCDriver_LibOAPC_uint32& nMajor, LibMCDriver_LibOAPC_uint32& nMinor, LibMCDriver_LibOAPC_uint32& nMicro, std::string& sBuild)
{
    nMajor = LIBMCDRIVER_LIBOAPC_VERSION_MAJOR;
    nMinor = LIBMCDRIVER_LIBOAPC_VERSION_MINOR;
    nMicro = LIBMCDRIVER_LIBOAPC_VERSION_MICRO;
    sBuild = __STRINGIZE_VALUE_OF (__GITHASH);
}


void CDriver_E1701::QueryParameters(LibMCEnv::PDriverStatusUpdateSession pDriverUpdateInstance)
{    
}


void CDriver_E1701::SetCorrectionFile(const LibMCDriver_LibOAPC_uint64 nCorrectionFileBufferSize, const LibMCDriver_LibOAPC_uint8 * pCorrectionFileBuffer)
{
    m_CorrectionFileData.clear();

    if (pCorrectionFileBuffer != nullptr) {
        const uint8_t * pSrc = pCorrectionFileBuffer;

        m_CorrectionFileData.reserve(nCorrectionFileBufferSize);
        for (uint64_t nIndex = 0; nIndex < nCorrectionFileBufferSize; nIndex++) {
            m_CorrectionFileData.push_back(*pSrc);
            pSrc++;
        }
    }
}

void CDriver_E1701::Connect(const std::string & sConnectionString, const std::string & sPassword)
{

    loadLibraryBindings("libe1701", "liboapc");

    auto nLibraryVersion = m_LibE1701.m_get_library_version();

    m_ConnectionID = m_LibE1701.m_set_connection(sConnectionString.c_str());
    if (!sPassword.empty ())
        m_LibE1701.m_set_password(m_ConnectionID, sPassword.c_str());

    LibMCEnv::PWorkingFile pWorkFile;
    std::string sCorrectionTempFileName;
    uint8_t tableNum = 0;
    
    if (m_CorrectionFileData.size() > 0) {
        pWorkFile = m_pDriverDirectory->StoreCustomData("correctionfile.dat", m_CorrectionFileData);
        sCorrectionTempFileName = pWorkFile->GetAbsoluteFileName();        
    }

    CheckE1701Error (m_LibE1701.m_load_correction(m_ConnectionID, sCorrectionTempFileName.c_str(), tableNum));
    
    unsigned short hwVersion = 0;
    unsigned short fwVersion = 0;
    m_LibE1701.m_get_version(m_ConnectionID, &hwVersion, &fwVersion);

    m_HardwareVersion = hwVersion;
    m_FirmwareVersion = fwVersion;


}

void CDriver_E1701::Disconnect()
{

    if ((m_bLibE1701Loaded) && (m_ConnectionID != 0)) {
        if (m_LibE1701.m_close != nullptr)
            m_LibE1701.m_close(m_ConnectionID);

        m_ConnectionID = 0;
    }

    // FreeLibrary?

    clearLibraryBindings();
	
}

void CDriver_E1701::DrawLayer(const std::string & sStreamUUID, const LibMCDriver_LibOAPC_uint32 nLayerIndex)
{
    if ((!m_bLibE1701Loaded) || (m_ConnectionID == 0)) 
        throw ELibMCDriver_LibOAPCInterfaceException(LIBMCDRIVER_LIBOAPC_ERROR_NOTCONNECTED);
    
    auto pToolpathAccessor = m_pDriverEnvironment->CreateToolpathAccessor(sStreamUUID);
    auto pLayer = pToolpathAccessor->LoadLayer(nLayerIndex);

    double dUnits = pToolpathAccessor->GetUnits();

    double dJumpSpeedInMMPerSecond = 100.0;
    double dMarkSpeedInMMPerSecond = 100.0;

    internalBegin ();

    uint32_t nSegmentCount = pLayer->GetSegmentCount();
    for (uint32_t nSegmentIndex = 0; nSegmentIndex < nSegmentCount; nSegmentIndex++) {

        LibMCEnv::eToolpathSegmentType eSegmentType;
        uint32_t nPointCount;
        pLayer->GetSegmentInfo(nSegmentIndex, eSegmentType, nPointCount);

        if (nPointCount >= 2) {

            std::vector<LibMCEnv::sPosition2D> Points;
            pLayer->GetSegmentPointData(nSegmentIndex, Points);

            if (nPointCount != Points.size ())
                throw ELibMCDriver_LibOAPCInterfaceException(LIBMCDRIVER_LIBOAPC_ERROR_INVALIDPOINTCOUNT);

            switch (eSegmentType) {
            case LibMCEnv::eToolpathSegmentType::Loop:
            case LibMCEnv::eToolpathSegmentType::Polyline:
                {
        
                    internalJumpTo(Points[0].m_Coordinates[0] * dUnits, Points[0].m_Coordinates[1] * dUnits, dJumpSpeedInMMPerSecond);
                    for (uint32_t nPointIndex = 1; nPointIndex < nPointCount; nPointIndex++) {
                        internalMarkTo(Points[nPointIndex].m_Coordinates[0] * dUnits, Points[nPointIndex].m_Coordinates[1] * dUnits, dMarkSpeedInMMPerSecond);
                    }
                    break;
                }

            case LibMCEnv::eToolpathSegmentType::Hatch:
            {
                if (nPointCount % 2 == 1)
                    throw ELibMCDriver_LibOAPCInterfaceException(LIBMCDRIVER_LIBOAPC_ERROR_INVALIDPOINTCOUNT);

                uint64_t nHatchCount = nPointCount / 2;

                for (uint64_t nHatchIndex = 0; nHatchIndex < nHatchCount; nHatchIndex++) {
                    internalJumpTo(Points[nHatchIndex * 2].m_Coordinates[0] * dUnits, Points[nHatchIndex * 2].m_Coordinates[1] * dUnits, dJumpSpeedInMMPerSecond);
                    internalMarkTo(Points[nHatchIndex * 2 + 1].m_Coordinates[0] * dUnits, Points[nHatchIndex * 2 + 1].m_Coordinates[1] * dUnits, dMarkSpeedInMMPerSecond);
                }
                break;
            }

        }

        }

    }

    internalExecute();

}


void CDriver_E1701::clearLibraryBindings()
{
    m_ConnectionID = 0;
    m_bLibE1701Loaded = false;
    m_LibE1701.m_close = nullptr;
    m_LibE1701.m_execute = nullptr;
    m_LibE1701.m_get_library_version = nullptr;
    m_LibE1701.m_get_version = nullptr;
    m_LibE1701.m_load_correction = nullptr;
    m_LibE1701.m_set_connection = nullptr;
    m_LibE1701.m_set_password = nullptr;
    m_LibE1701.m_switch_correction = nullptr;
}


void CDriver_E1701::loadLibraryBindings(const std::string& sLibE1701Resource, const std::string& sLibOAPCResource)
{
    clearLibraryBindings();
    std::string sExtension;

#ifdef _WIN32
    sExtension = "dll";
#else
    sExtension = "so";
#endif

    m_pDriverDirectory = m_pDriverEnvironment->CreateWorkingDirectory();
    m_pLibE1701DLL = m_pDriverDirectory->StoreDriverData("libe1701." + sExtension, sLibE1701Resource);
    m_pLibOAPCDLL = m_pDriverDirectory->StoreDriverData("liboapc." + sExtension, sLibOAPCResource);

    auto sDLLPath = m_pDriverDirectory->GetAbsoluteFilePath();
    auto sLibE1701FileName = m_pLibE1701DLL->GetAbsoluteFileName();
        
#ifdef _WIN32

    // Convert dllPath to UTF16-string
    int nPathLength = (int)sDLLPath.length();
    int nPathBufferSize = nPathLength * 2 + 2;
    std::vector<wchar_t> wsPathName((int64_t)nPathBufferSize + 1);
    int nPathResult = MultiByteToWideChar(CP_UTF8, 0, sDLLPath.c_str(), nPathLength, wsPathName.data(), nPathBufferSize);
    if (nPathResult == 0)
        throw ELibMCDriver_LibOAPCInterfaceException(LIBMCDRIVER_LIBOAPC_ERROR_COULDNOTLOADLIBRARY);

    // Convert filename to UTF16-string
    int nNameLength = (int)sLibE1701FileName.length ();
    int nNameBufferSize = nNameLength * 2 + 2;
    std::vector<wchar_t> wsLibraryFileName((int64_t)nNameBufferSize + 1);
    int nNameResult = MultiByteToWideChar(CP_UTF8, 0, sLibE1701FileName.c_str (), nNameLength, wsLibraryFileName.data(), nNameBufferSize);
    if (nNameResult == 0)
        throw ELibMCDriver_LibOAPCInterfaceException(LIBMCDRIVER_LIBOAPC_ERROR_COULDNOTLOADLIBRARY);

    // To be sure, add zeros at the end of the string
    wsPathName[nPathBufferSize] = 0;
    wsLibraryFileName[nNameBufferSize] = 0;

    DWORD nPathBufferLength = GetDllDirectoryW(0, nullptr);
    auto nError = GetLastError();
    if (nPathBufferLength == 0) {
        if (GetLastError() != 0)
            throw ELibMCDriver_LibOAPCInterfaceException(LIBMCDRIVER_LIBOAPC_ERROR_COULDNOTLOADLIBRARY);
    }

    std::vector<wchar_t> wsOldPathName((uint64_t)nPathBufferLength + 1);

    DWORD nPathBufferReceived = GetDllDirectoryW(nPathBufferLength, wsOldPathName.data ());
    if (nPathBufferLength == 0) {
        if (GetLastError() != 0)
            throw ELibMCDriver_LibOAPCInterfaceException(LIBMCDRIVER_LIBOAPC_ERROR_COULDNOTLOADLIBRARY);
    }
    if (nPathBufferReceived > nPathBufferLength)
        throw ELibMCDriver_LibOAPCInterfaceException(LIBMCDRIVER_LIBOAPC_ERROR_COULDNOTLOADLIBRARY);
    wsOldPathName[nPathBufferLength] = 0;

    SetDllDirectoryW(wsPathName.data ());
    HMODULE hLibrary = LoadLibraryW(wsLibraryFileName.data());
    SetDllDirectoryW(wsOldPathName.data ());

    if (hLibrary == 0)
        throw ELibMCDriver_LibOAPCInterfaceException(LIBMCDRIVER_LIBOAPC_ERROR_COULDNOTLOADLIBRARY);
#else // _WIN32
    void* hLibrary = dlopen(sLibE1701FileName.c_str(), RTLD_LAZY);
    if (hLibrary == 0)
        throw ELibMCDriver_LibOAPCInterfaceException(LIBMCDRIVER_LIBOAPC_ERROR_COULDNOTLOADLIBRARY);
    dlerror();
#endif // _WIN32

#ifdef _WIN32
    m_LibE1701.m_get_library_version = (libE1701::E1701_get_library_version)GetProcAddress(hLibrary, "E1701_get_library_version");
#else // _WIN32
    m_LibE1701.m_get_library_version = (libE1701::E1701_get_library_version)dlsym(hLibrary, "E1701_get_library_version");
    dlerror();
#endif // _WIN32
    if (m_LibE1701.m_get_library_version == nullptr)
        throw ELibMCDriver_LibOAPCInterfaceException(LIBMCDRIVER_LIBOAPC_ERROR_COULDNOTFINDLIBRARYEXPORT);

#ifdef _WIN32
    m_LibE1701.m_set_connection = (libE1701::E1701_set_connection)GetProcAddress(hLibrary, "E1701_set_connection");
#else // _WIN32
    m_LibE1701.m_set_connection = (libE1701::E1701_set_connection)dlsym(hLibrary, "E1701_set_connection");
    dlerror();
#endif // _WIN32
    if (m_LibE1701.m_set_connection == nullptr)
        throw ELibMCDriver_LibOAPCInterfaceException(LIBMCDRIVER_LIBOAPC_ERROR_COULDNOTFINDLIBRARYEXPORT);

#ifdef _WIN32
    m_LibE1701.m_set_password = (libE1701::E1701_set_password)GetProcAddress(hLibrary, "E1701_set_password");
#else // _WIN32
    m_LibE1701.m_set_password = (libE1701::E1701_set_password)dlsym(hLibrary, "E1701_set_password");
    dlerror();
#endif // _WIN32
    if (m_LibE1701.m_set_password == nullptr)
        throw ELibMCDriver_LibOAPCInterfaceException(LIBMCDRIVER_LIBOAPC_ERROR_COULDNOTFINDLIBRARYEXPORT);

#ifdef _WIN32
    m_LibE1701.m_get_version = (libE1701::E1701_get_version)GetProcAddress(hLibrary, "E1701_get_version");
#else // _WIN32
    m_LibE1701.m_get_version = (libE1701::E1701_get_version)dlsym(hLibrary, "E1701_get_version");
    dlerror();
#endif // _WIN32
    if (m_LibE1701.m_get_version == nullptr)
        throw ELibMCDriver_LibOAPCInterfaceException(LIBMCDRIVER_LIBOAPC_ERROR_COULDNOTFINDLIBRARYEXPORT);

#ifdef _WIN32
    m_LibE1701.m_load_correction = (libE1701::E1701_load_correction)GetProcAddress(hLibrary, "E1701_load_correction");
#else // _WIN32
    m_LibE1701.m_load_correction = (libE1701::E1701_load_correction)dlsym(hLibrary, "E1701_load_correction");
    dlerror();
#endif // _WIN32
    if (m_LibE1701.m_load_correction == nullptr)
        throw ELibMCDriver_LibOAPCInterfaceException(LIBMCDRIVER_LIBOAPC_ERROR_COULDNOTFINDLIBRARYEXPORT);

#ifdef _WIN32
    m_LibE1701.m_switch_correction = (libE1701::E1701_switch_correction)GetProcAddress(hLibrary, "E1701_switch_correction");
#else // _WIN32
    m_LibE1701.m_switch_correction = (libE1701::E1701_switch_correction)dlsym(hLibrary, "E1701_switch_correction");
    dlerror();
#endif // _WIN32
    if (m_LibE1701.m_switch_correction == nullptr)
        throw ELibMCDriver_LibOAPCInterfaceException(LIBMCDRIVER_LIBOAPC_ERROR_COULDNOTFINDLIBRARYEXPORT);

#ifdef _WIN32
    m_LibE1701.m_execute = (libE1701::E1701_execute)GetProcAddress(hLibrary, "E1701_execute");
#else // _WIN32
    m_LibE1701.m_execute = (libE1701::E1701_execute)dlsym(hLibrary, "E1701_execute");
    dlerror();
#endif // _WIN32
    if (m_LibE1701.m_execute == nullptr)
        throw ELibMCDriver_LibOAPCInterfaceException(LIBMCDRIVER_LIBOAPC_ERROR_COULDNOTFINDLIBRARYEXPORT);

#ifdef _WIN32
    m_LibE1701.m_close = (libE1701::E1701_close)GetProcAddress(hLibrary, "E1701_close");
#else // _WIN32
    m_LibE1701.m_close = (libE1701::E1701_close)dlsym(hLibrary, "E1701_close");
    dlerror();
#endif // _WIN32
    if (m_LibE1701.m_close == nullptr)
        throw ELibMCDriver_LibOAPCInterfaceException(LIBMCDRIVER_LIBOAPC_ERROR_COULDNOTFINDLIBRARYEXPORT);


#ifdef _WIN32
    m_LibE1701.m_jump_abs = (libE1701::E1701_jump_abs)GetProcAddress(hLibrary, "E1701_jump_abs");
#else // _WIN32
    m_LibE1701.m_jump_abs = (libE1701::E1701_jump_abs)dlsym(hLibrary, "E1701_jump_abs");
    dlerror();
#endif // _WIN32
    if (m_LibE1701.m_jump_abs == nullptr)
        throw ELibMCDriver_LibOAPCInterfaceException(LIBMCDRIVER_LIBOAPC_ERROR_COULDNOTFINDLIBRARYEXPORT);

#ifdef _WIN32
    m_LibE1701.m_mark_abs = (libE1701::E1701_mark_abs)GetProcAddress(hLibrary, "E1701_mark_abs");
#else // _WIN32
    m_LibE1701.m_mark_abs = (libE1701::E1701_mark_abs)dlsym(hLibrary, "E1701_mark_abs");
    dlerror();
#endif // _WIN32
    if (m_LibE1701.m_mark_abs == nullptr)
        throw ELibMCDriver_LibOAPCInterfaceException(LIBMCDRIVER_LIBOAPC_ERROR_COULDNOTFINDLIBRARYEXPORT);

#ifdef _WIN32
    m_LibE1701.m_set_speeds = (libE1701::E1701_set_speeds)GetProcAddress(hLibrary, "E1701_set_speeds");
#else // _WIN32
    m_LibE1701.m_set_speeds = (libE1701::E1701_set_speeds)dlsym(hLibrary, "E1701_set_speeds");
    dlerror();
#endif // _WIN32
    if (m_LibE1701.m_set_speeds == nullptr)
        throw ELibMCDriver_LibOAPCInterfaceException(LIBMCDRIVER_LIBOAPC_ERROR_COULDNOTFINDLIBRARYEXPORT);

#ifdef _WIN32
    m_LibE1701.m_stop_execution = (libE1701::E1701_stop_execution)GetProcAddress(hLibrary, "E1701_stop_execution");
#else // _WIN32
    m_LibE1701.m_stop_execution = (libE1701::E1701_stop_execution)dlsym(hLibrary, "E1701_stop_execution");
    dlerror();
#endif // _WIN32
    if (m_LibE1701.m_stop_execution == nullptr)
        throw ELibMCDriver_LibOAPCInterfaceException(LIBMCDRIVER_LIBOAPC_ERROR_COULDNOTFINDLIBRARYEXPORT);

#ifdef _WIN32
    m_LibE1701.m_get_card_state2 = (libE1701::E1701_get_card_state2)GetProcAddress(hLibrary, "E1701_get_card_state2");
#else // _WIN32
    m_LibE1701.m_get_card_state2 = (libE1701::E1701_get_card_state2)dlsym(hLibrary, "E1701_get_card_state2");
    dlerror();
#endif // _WIN32
    if (m_LibE1701.m_get_card_state2 == nullptr)
        throw ELibMCDriver_LibOAPCInterfaceException(LIBMCDRIVER_LIBOAPC_ERROR_COULDNOTFINDLIBRARYEXPORT);

    m_bLibE1701Loaded = true;

}


void CDriver_E1701::CheckE1701Error(libE1701::e1701Error errorCode)
{
    if (errorCode != 0) {
        // Only known errors are propagated. Unknown errors are "generic"
        if (errorCode < LIBMCDRIVER_LIBOAPC_ERROR_E1701_ERROR_GENERIC)
            errorCode = LIBMCDRIVER_LIBOAPC_ERROR_E1701_ERROR_GENERIC;
        if (errorCode > LIBMCDRIVER_LIBOAPC_ERROR_E1701_ERROR_NO_DATA_AVAILABLE)
            errorCode = LIBMCDRIVER_LIBOAPC_ERROR_E1701_ERROR_GENERIC;

        throw ELibMCDriver_LibOAPCInterfaceException(errorCode);
    }
}


void CDriver_E1701::internalBegin()
{
    CheckE1701Error(m_LibE1701.m_set_speeds(m_ConnectionID, 25000.0, 12500.0));

}

void CDriver_E1701::internalJumpTo(double dX, double dY, double dSpeedInMM)
{
    double relativeX = (dX - m_dOpticalFieldMinX) / (m_dOpticalFieldMaxX - m_dOpticalFieldMinX);
    double relativeY = (dY - m_dOpticalFieldMinY) / (m_dOpticalFieldMaxY - m_dOpticalFieldMinY);

    if ((relativeX < 0.0) || (relativeY < 0.0) || (relativeX > 1.0) || (relativeY > 1.0)) {
        m_bPositionIsInvalid = true;
    }
    else {
        int iX = (int32_t) round(relativeX * m_nLogicalFieldMinX + (1.0 - relativeX) * m_nLogicalFieldMaxX);
        int iY = (int32_t) round(relativeY * m_nLogicalFieldMinY + (1.0 - relativeY) * m_nLogicalFieldMaxY);

        libE1701::e1701Error nError = m_LibE1701.m_jump_abs(m_ConnectionID, iX, iY, 0);
        m_bPositionIsInvalid = (nError == 0);
        CheckE1701Error(nError);

    }

}

void CDriver_E1701::internalMarkTo(double dX, double dY, double dSpeedInMM)
{
    if (m_bPositionIsInvalid) {
        internalJumpTo(dX, dY, dSpeedInMM);

    } else {

        double relativeX = (dX - m_dOpticalFieldMinX) / (m_dOpticalFieldMaxX - m_dOpticalFieldMinX);
        double relativeY = (dY - m_dOpticalFieldMinY) / (m_dOpticalFieldMaxY - m_dOpticalFieldMinY);

        if ((relativeX < 0.0) || (relativeY < 0.0) || (relativeX > 1.0) || (relativeY > 1.0)) {
            m_bPositionIsInvalid = true;
        }
        else {
            int iX = (int32_t)round(relativeX * m_nLogicalFieldMinX + (1.0 - relativeX) * m_nLogicalFieldMaxX);
            int iY = (int32_t)round(relativeY * m_nLogicalFieldMinY + (1.0 - relativeY) * m_nLogicalFieldMaxY);

            libE1701::e1701Error nError = m_LibE1701.m_mark_abs(m_ConnectionID, iX, iY, 0);
            m_bPositionIsInvalid = (nError == 0);
            CheckE1701Error(nError);
        }
    }


}

void CDriver_E1701::internalExecute()
{
    CheckE1701Error(m_LibE1701.m_execute(m_ConnectionID));

    unsigned int bitFlag = 0;
    CheckE1701Error (m_LibE1701.m_get_card_state2(m_ConnectionID, &bitFlag));
    CheckE1701Error (m_LibE1701.m_get_card_state2(m_ConnectionID, &bitFlag));

    while (bitFlag & 1) {
        CheckE1701Error (m_LibE1701.m_get_card_state2(m_ConnectionID, &bitFlag));
    }

}



void CDriver_E1701::SetOpticalField(const LibMCDriver_LibOAPC_double dMinX, const LibMCDriver_LibOAPC_double dMinY, const LibMCDriver_LibOAPC_double dMaxX, const LibMCDriver_LibOAPC_double dMaxY)
{
    if (abs(dMaxX - dMinX) < E1701_MINFIELDSIZE)
        throw ELibMCDriver_LibOAPCInterfaceException(LIBMCDRIVER_LIBOAPC_ERROR_INVALIDFIELDSIZE);
    if (abs(dMaxY - dMinY) < E1701_MINFIELDSIZE)
        throw ELibMCDriver_LibOAPCInterfaceException(LIBMCDRIVER_LIBOAPC_ERROR_INVALIDFIELDSIZE);

    m_dOpticalFieldMinX = dMinX;
    m_dOpticalFieldMinY = dMinY;
    m_dOpticalFieldMinX = dMaxX,
    m_dOpticalFieldMinY = dMaxY;
}

void CDriver_E1701::SetLogicalField(const LibMCDriver_LibOAPC_int32 nMinX, const LibMCDriver_LibOAPC_int32 nMinY, const LibMCDriver_LibOAPC_int32 nMaxX, const LibMCDriver_LibOAPC_int32 nMaxY)
{
    if (nMinX == nMaxX)
        throw ELibMCDriver_LibOAPCInterfaceException(LIBMCDRIVER_LIBOAPC_ERROR_INVALIDFIELDSIZE);
    if (nMinY == nMaxY)
        throw ELibMCDriver_LibOAPCInterfaceException(LIBMCDRIVER_LIBOAPC_ERROR_INVALIDFIELDSIZE);

    m_nLogicalFieldMinX = nMinX;
    m_nLogicalFieldMinY = nMinY;
    m_nLogicalFieldMaxX = nMaxX;
    m_nLogicalFieldMaxY = nMaxY;

}

