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


Abstract: This is a stub class definition of CDriver_S7Net

*/

#include "libmcdriver_s7net_driver_s7net.hpp"
#include "libmcdriver_s7net_interfaceexception.hpp"
#include "libs7com_abi.hpp"
#include "libs7com_interfaces.hpp"

// Include custom headers here.


using namespace LibMCDriver_S7Net::Impl;

/*************************************************************************************************************************
 Class definition of CDriver_S7Net 
**************************************************************************************************************************/

CDriver_S7Net::CDriver_S7Net(const std::string& sName, const std::string& sType, LibMCEnv::PDriverEnvironment pDriverEnvironment)
    : CDriver (sName, sType), m_pDriverEnvironment (pDriverEnvironment)
{
    if (pDriverEnvironment.get() == nullptr)
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_INVALIDPARAM);

    std::vector<uint8_t> Buffer;
    pDriverEnvironment->RetrieveDriverData("s7protocol", Buffer);

    if (Buffer.size () == 0)
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_INVALIDDRIVERPROTOCOL);

    std::string sProtocolXML (Buffer.begin(), Buffer.end());
    m_sProtocolXML = sProtocolXML;

    pDriverEnvironment->RegisterBoolParameter("door_closed", "Door closed", false);
    pDriverEnvironment->RegisterBoolParameter("laserclearance_plc", "Laser Clearance PLC", false);
    pDriverEnvironment->RegisterBoolParameter("laserclearance_safety", "Laser Clearance Safety", false);
    pDriverEnvironment->RegisterBoolParameter("cooling_ready", "Cooling Ready", false);
    pDriverEnvironment->RegisterStringParameter("heating_platform_state", "Platform Heating State", "off"); // off, heating, ready, failure
    pDriverEnvironment->RegisterDoubleParameter("heating_platform_sensor1", "Platform Heating Temperature 1", 0.0);
    pDriverEnvironment->RegisterDoubleParameter("heating_platform_sensor2", "Platform Heating Temperature 2", 0.0);
    pDriverEnvironment->RegisterDoubleParameter("heating_platform_sensor3", "Platform Heating Temperature 3", 0.0);
    pDriverEnvironment->RegisterDoubleParameter("heating_platform_sensor4", "Platform Heating Temperature 4", 0.0);
    pDriverEnvironment->RegisterStringParameter("heating_chamber_state", "Chamber Heating State", "off"); // off, heating, ready, failure
    pDriverEnvironment->RegisterDoubleParameter("heating_chamber_sensor1", "Chamber Heating Temperature 1", 0.0);
    pDriverEnvironment->RegisterDoubleParameter("heating_chamber_sensor2", "Chamber Heating Temperature 2", 0.0);
    pDriverEnvironment->RegisterDoubleParameter("heating_chamber_sensor3", "Chamber Heating Temperature 3", 0.0);
    pDriverEnvironment->RegisterDoubleParameter("heating_chamber_sensor4", "Chamber Heating Temperature 4", 0.0);
    pDriverEnvironment->RegisterStringParameter("vacuum_pumpingunit_state", "Vacuum Pumping Unit State", "off"); // off, evacuated, ready, failure
    pDriverEnvironment->RegisterDoubleParameter("vacuum_pumpingunit_pressure", "Vacuum Pumping Pressure", 0.0);
    pDriverEnvironment->RegisterStringParameter("gas_circulation_state", "Gas Circulation State", "off"); // off, on, failure
    pDriverEnvironment->RegisterDoubleParameter("gas_circulation_o2pressure1", "Gas Circulation O2 pressure 1", 0.0);
    pDriverEnvironment->RegisterDoubleParameter("gas_circulation_o2pressure2", "Gas Circulation O2 pressure 2", 0.0);
    pDriverEnvironment->RegisterDoubleParameter("gas_circulation_o2pressure3", "Gas Circulation O2 pressure 3", 0.0);
    pDriverEnvironment->RegisterDoubleParameter("gas_circulation_dewpoint1", "Gas Circulation dew point 1", 0.0);
    pDriverEnvironment->RegisterDoubleParameter("gas_circulation_dewpoint2", "Gas Circulation dew point 2", 0.0);
    pDriverEnvironment->RegisterDoubleParameter("gas_circulation_dewpoint3", "Gas Circulation dew point 3", 0.0);
    pDriverEnvironment->RegisterDoubleParameter("gas_circulation_flowspeed1", "Gas Circulation flow speed 1", 0.0);
    pDriverEnvironment->RegisterDoubleParameter("gas_circulation_flowspeed2", "Gas Circulation flow speed 2", 0.0);
    pDriverEnvironment->RegisterDoubleParameter("gas_circulation_flowspeed3", "Gas Circulation flow speed 3", 0.0);
    pDriverEnvironment->RegisterDoubleParameter("gas_circulation_prefiltertemperature", "Gas Circulation pre filter temperature", 0.0);

    pDriverEnvironment->RegisterStringParameter("gas_flow_controller_state", "Mass Flow Controller state", "off"); // off, on, failure
    pDriverEnvironment->RegisterDoubleParameter("gas_flow_controller_argon", "Gas Flow Controller argon", 0.0);
    pDriverEnvironment->RegisterBoolParameter("emergency_flooding_system_ready", "Emergency Flooding System ready", false);
    pDriverEnvironment->RegisterBoolParameter("drive_systems_ready", "Drive Systems ready", false);
    pDriverEnvironment->RegisterDoubleParameter("position_recoater", "Position Recoater X", 0.0);
    pDriverEnvironment->RegisterDoubleParameter("position_platform_z", "Position Platform Z", 0.0);
    pDriverEnvironment->RegisterDoubleParameter("position_powderrefill", "Position Powderrefill B", 0.0);
}

CDriver_S7Net::~CDriver_S7Net()
{
    m_pCommunication = nullptr;
    m_pCommunicationWrapper = nullptr;
    m_pPLC = nullptr;
    m_pPLCWrapper = nullptr;
    m_pWorkingDirectory = nullptr;
}


void CDriver_S7Net::Initialise()
{

    m_pCommunication = nullptr;
    m_pCommunicationWrapper = nullptr;
    m_pPLC = nullptr;
    m_pPLCWrapper = nullptr;
    m_pWorkingDirectory = nullptr;

    m_pWorkingDirectory = m_pDriverEnvironment->CreateWorkingDirectory();
    auto pLibS7NetDLL = m_pWorkingDirectory->StoreDriverData("libs7net.dll", "libs7net");
    auto pLibS7NetCOMHost = m_pWorkingDirectory->StoreDriverData("libs7net_managed.comhost.dll", "libs7net_managed.comhost");
    m_pWorkingDirectory->StoreDriverData("libs7net_managed.dll", "libs7net_managed");
    m_pWorkingDirectory->StoreDriverData("libs7net.dll", "libs7net");
    m_pWorkingDirectory->StoreDriverData("libs7net_managed.runtimeconfig.json", "libs7net_managed.runtimeconfig");
    m_pWorkingDirectory->StoreDriverData("libs7net_managed.runtimeconfig.dev.json", "libs7net_managed.runtimeconfig.dev");

    m_pPLCWrapper = LibS7Net::CWrapper::loadLibrary (pLibS7NetDLL->GetAbsoluteFileName());
    m_pPLC = m_pPLCWrapper->CreatePLC(pLibS7NetCOMHost->GetAbsoluteFileName());

    m_pCommunicationWrapper = LibS7Com::CWrapper::loadLibraryFromSymbolLookupMethod((void*) &LibS7Com::Impl::LibS7Com_GetProcAddress);
    m_pCommunicationWrapper->InjectComponent("LibS7Net", m_pPLCWrapper->GetSymbolLookupMethod());
    m_pCommunication = m_pCommunicationWrapper->CreatePLCCommunication();
    m_pCommunication->SetProtocolConfiguration(m_sProtocolXML);
    
}

void CDriver_S7Net::Connect(const std::string& sIPAddress, const LibMCDriver_S7Net_uint32 nRack, const LibMCDriver_S7Net_uint32 nSlot)
{
    if (m_pPLC.get() == nullptr)
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_DRIVERNOTINITIALISED);
    if (m_pCommunication.get() == nullptr)
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_DRIVERNOTINITIALISED);

    //m_pCommunication->StartCommunication(m_pPLC);
}


void CDriver_S7Net::Disconnect()
{	
    //m_pCommunication->StopCommunication();
}

void CDriver_S7Net::QueryParameters()
{
}


