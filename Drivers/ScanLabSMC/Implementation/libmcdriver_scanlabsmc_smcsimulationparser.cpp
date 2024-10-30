/*++

Copyright (C) 2023 Autodesk Inc.

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


Abstract: This is a stub class definition of CSMCJob

*/

#include "libmcdriver_scanlabsmc_smcsimulationparser.hpp"
#include "libmcdriver_scanlabsmc_interfaceexception.hpp"
#include "libmcdriver_scanlabsmc_sdk.hpp"

// Include custom headers here.

#include <array>
#include <thread>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>

#ifdef _WIN32
#endif

using namespace LibMCDriver_ScanLabSMC::Impl;

CSMCSimulationParser::CSMCSimulationParser(const std::string& sAbsoluteFileNameUTF8)
{
    
#ifdef _WIN32
    if (sAbsoluteFileNameUTF8.length () > 65536)
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPARAM);

    int nLength = (int)sAbsoluteFileNameUTF8.length();
    int nBufferSize = nLength * 2 + 2;
    std::vector<wchar_t> wsLibraryFileName(nBufferSize);
    int nResult = MultiByteToWideChar(CP_UTF8, 0, sAbsoluteFileNameUTF8.c_str(), nLength, &wsLibraryFileName[0], nBufferSize);
    if (nResult == 0)
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDSIMULATIONFILENAME);

    std::ifstream file(wsLibraryFileName.data (), std::ios::in);
#else
    std::ifstream file(sAbsoluteFileNameUTF8, std::ios::in);
#endif


    if (!file.is_open ()) 
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_COULDNOTOOPENSIMULATIONFILE);
    
    std::string line;
    double timestamp = 0;
    int numberOfCoordinates = 0;
    int scanDevices = 0;
    int stages = 0;

        while (std::getline(file, line)) {

            if (!line.empty()) {
                if (line.rfind("<!--Simulation output", 0) == 0) {
                    std::istringstream iss(line);
                    std::string item;
                    while (iss >> item) {
                        if (item.find("ScanDevices") == 0) {
                            scanDevices = std::stoi(item.substr(13, item.size() - 14));
                        }
                        else if (item.find("Stage") == 0 && item.find("StageDelay") != 0) {
                            std::string stagesRaw = item.substr(7, item.size() - 8);
                            stages = (stagesRaw == "None") ? 0 : std::stoi(stagesRaw.substr(5));
                        }
                    }
                    numberOfCoordinates = scanDevices * 2 + stages * 2;
                }

                if (line[0] == '+' || line[0] == '-') {
                    std::istringstream iss(line);
                    std::string token;
                    std::vector<std::string> data;

                    sSMCSimulationEntry newEntry;

                    while (std::getline(iss, token, ';')) {
                        data.push_back(token);
                    }

                    if (data.size() > 3) {
                        int numLaserOnDelays = std::stoi(data[numberOfCoordinates]);
                        int numLaserOffDelays = std::stoi(data[numberOfCoordinates + 1 + numLaserOnDelays]);

                        newEntry.m_dCoordinates[0] = std::stod(data[0]);
                        newEntry.m_dCoordinates[1] = std::stod(data[1]);
                        newEntry.m_dCoordinates[2] = 0.0;

                        newEntry.m_dLaserToggle = std::stoi(data[numberOfCoordinates + 2 + numLaserOnDelays + numLaserOffDelays]);
                        newEntry.m_dActiveChannel1 = std::stod(data[numberOfCoordinates + 3 + numLaserOnDelays + numLaserOffDelays]);
                        newEntry.m_dActiveChannel2 = std::stod(data[numberOfCoordinates + 4 + numLaserOnDelays + numLaserOffDelays]);
                        newEntry.m_CommandIndex = std::stoi(data[numberOfCoordinates + 5 + numLaserOnDelays + numLaserOffDelays]);

                        double tempTimestamp = timestamp + 10;
                        if (numLaserOnDelays > 0) {
                            for (int i = 0; i < numLaserOnDelays; i++) {
                                double timeStep = std::stod(data[numberOfCoordinates + 1 + i]);
                                timestamp = tempTimestamp + timeStep;

                                newEntry.m_dTimestamp = timestamp;
                                m_Entries.push_back(newEntry);
                            }
                            timestamp = tempTimestamp;
                        }
                        if (numLaserOffDelays > 0) {
                            for (int i = 0; i < numLaserOffDelays; i++) {
                                double timeStep = std::stod(data[numberOfCoordinates + 2 + numLaserOnDelays + i]);
                                timestamp = tempTimestamp + timeStep;

                                newEntry.m_dTimestamp = timestamp;
                                m_Entries.push_back(newEntry);
                            }
                            timestamp = tempTimestamp;
                        }
                        if (numLaserOnDelays == 0 && numLaserOffDelays == 0) {

                            newEntry.m_dTimestamp = tempTimestamp;
                            m_Entries.push_back(newEntry);

                            timestamp += 10;
                        }
                    }
                }
            }
        }

        std::sort(m_Entries.begin(), m_Entries.end(),
            [](const sSMCSimulationEntry& x, const sSMCSimulationEntry& y) { return x.m_dTimestamp < y.m_dTimestamp; });

}

CSMCSimulationParser::~CSMCSimulationParser()
{

}

size_t CSMCSimulationParser::getCount()
{
    return m_Entries.size();
}

sSMCSimulationEntry& CSMCSimulationParser::getEntry(size_t nIndex)
{
    return m_Entries.at(nIndex);
}

void CSMCSimulationParser::readTimestampValues(std::vector<double>& values)
{
    values.resize(m_Entries.size());

    if (m_Entries.size() > 0) {
        double* pTarget = values.data();

        for (auto& entry : m_Entries) {
            *pTarget = entry.m_dTimestamp;
            pTarget++;
        }
    }
}

void CSMCSimulationParser::readXValues(std::vector<double>& values)
{
    values.resize(m_Entries.size());

    if (m_Entries.size() > 0) {
        double* pTarget = values.data();

        for (auto& entry : m_Entries) {
            *pTarget = entry.m_dCoordinates[0];
            pTarget++;
        }
    }
}

void CSMCSimulationParser::readYValues(std::vector<double>& values)
{
    values.resize(m_Entries.size());

    if (m_Entries.size() > 0) {
        double* pTarget = values.data();

        for (auto& entry : m_Entries) {
            *pTarget = entry.m_dCoordinates[1];
            pTarget++;
        }
    }

}
