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

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL AUTODESK INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#define __AMCIMPL_UI_MODULE

#include "amc_ui_systemstate.hpp"
#include "libmc_exceptiontypes.hpp"

using namespace AMC;

CUISystemState::CUISystemState(PStateMachineData pStateMachineData, AMC::PToolpathHandler pToolpathHandler, PStateSignalHandler pSignalHandler, PLogger pLogger, PStateJournal pStateJournal, const std::string& sTestOutputPath, PAccessControl pAccessControl, PLanguageHandler pLanguageHandler, PMeshHandler pMeshHandler, PDataSeriesHandler pDataSeriesHandler, AMCCommon::PChrono pGlobalChronoInstance, PAlertHandler pAlertHandler, LibMCData::PDataModel pDataModel)
    : m_pStateMachineData(pStateMachineData),
    m_pSignalHandler(pSignalHandler),
    m_pDataModel(pDataModel),
    m_pToolpathHandler(pToolpathHandler),
    m_pStateJournal(pStateJournal),
    m_sTestOutputPath(sTestOutputPath),
    m_pLogger(pLogger),
    m_pAccessControl(pAccessControl),
    m_pLanguageHandler(pLanguageHandler),
    m_pMeshHandler(pMeshHandler),
    m_pDataSeriesHandler (pDataSeriesHandler),
    m_pGlobalChronoInstance (pGlobalChronoInstance),
    m_pAlertHandler (pAlertHandler)
{
    if (pStateMachineData.get() == nullptr)
        throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
    if (pSignalHandler.get() == nullptr)
        throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
    if (pDataModel.get() == nullptr)
        throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
    if (pToolpathHandler.get() == nullptr)
        throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
    if (pLogger.get() == nullptr)
        throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
    if (pStateJournal.get() == nullptr)
        throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
    if (pAccessControl.get() == nullptr)
        throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
    if (pLanguageHandler.get() == nullptr)
        throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
    if (pMeshHandler.get() == nullptr)
        throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
    if (pDataSeriesHandler.get() == nullptr)
        throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
    if (pGlobalChronoInstance.get () == nullptr)
        throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
    if (pAlertHandler.get() == nullptr)
        throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);


}

CUISystemState::~CUISystemState()
{

}

PStateMachineData CUISystemState::getStateMachineData()
{
    return m_pStateMachineData;
}

PStateSignalHandler CUISystemState::getSignalHandler()
{
    return m_pSignalHandler;
}

PLogger CUISystemState::getLogger()
{
    return m_pLogger;
}

PStateJournal CUISystemState::getStateJournal()
{
    return m_pStateJournal;
}

PAlertHandler CUISystemState::getAlertHandler()
{
    return m_pAlertHandler;
}


std::string CUISystemState::getTestOutputPath()
{
    return m_sTestOutputPath;
}

PAccessControl CUISystemState::getAccessControl()
{
    return m_pAccessControl;
}

PLanguageHandler CUISystemState::getLanguageHandler()
{
    return m_pLanguageHandler;
}


PToolpathHandler CUISystemState::getToolpathHandler()
{
    return m_pToolpathHandler;
}

PMeshHandler CUISystemState::getMeshHandler()
{
    return m_pMeshHandler;
}

LibMCData::PDataModel CUISystemState::getDataModel()
{
    return m_pDataModel;
}


PDataSeriesHandler CUISystemState::getDataSeriesHandler()
{
    return m_pDataSeriesHandler;
}

AMCCommon::PChrono CUISystemState::getGlobalChronoInstance()
{
    return m_pGlobalChronoInstance;
}

uint64_t CUISystemState::getAbsoluteTimeStamp()
{
    return m_pGlobalChronoInstance->getUTCTimeStampInMicrosecondsSince1970();
}




