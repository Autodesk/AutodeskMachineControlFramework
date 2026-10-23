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


Abstract: This is the class definition of CSMCCSVParser

*/

#include "libmcdriver_scanlabsmc_smccsvparser.hpp"
#include "libmcdriver_scanlabsmc_interfaceexception.hpp"

#include <fstream>
#include <stdexcept>
#include <charconv>
#include <string_view>
#include <ctime>
#include <cmath>

#include <cctype> // for std::tolower
using namespace LibMCDriver_ScanLabSMC::Impl;

CSMCCSVParser::CSMCCSVParser(const std::string& sAbsoluteFileNameUTF8, char delimiter)
    : m_delimiter(delimiter)
{

#ifdef _WIN32
    if (sAbsoluteFileNameUTF8.length() > 65536)
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPARAM);

    int nLength = (int)sAbsoluteFileNameUTF8.length();
    int nBufferSize = nLength * 2 + 2;
    std::vector<wchar_t> wsLibraryFileName(nBufferSize);
    int nResult = MultiByteToWideChar(CP_UTF8, 0, sAbsoluteFileNameUTF8.c_str(), nLength, &wsLibraryFileName[0], nBufferSize);
    if (nResult == 0)
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDSIMULATIONFILENAME);

    std::ifstream file(wsLibraryFileName.data(), std::ios::binary);
#else
    std::ifstream file(sAbsoluteFileNameUTF8, std::ios::binary);
#endif

    if (!file.is_open())
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_COULDNOTOOPENSIMULATIONFILE);

    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    m_fileBuff.resize(static_cast<size_t>(size));
    if (!file.read(m_fileBuff.data(), size)) {
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_COULDNOTREADSIMULATIONFILE);
    }
}

void CSMCCSVParser::Parse(const std::vector<FieldBinding>& field_bindings)
{
    const char* data = m_fileBuff.data();
    const char* end = data + m_fileBuff.size();

    const char* lineStart = data;
    const char* ptr = data;

    double timestamp = 0.0;

    size_t prevTsIdx = 0;
    size_t currTsIdx = 0;

    std::vector<ParserFunc> parsers;
    std::vector<ExtenderFunc> extenders;
    std::vector<InterpolatorFunc> interpolators;

    void* ts_target = nullptr;

    for (size_t i = 0; i < field_bindings.size(); ++i) {
        if (field_bindings[i].meta.type == FieldParserType::Timestamp)
            ts_target = field_bindings[i].target;
        else {
            parsers.push_back(GetParser(field_bindings[i].meta.type));
            extenders.push_back(GetExtender(field_bindings[i].meta.type));
            interpolators.push_back(GetInterpolator(field_bindings[i].meta.type));
        }
    }

    while (ptr < end) {
        const char* lineEnd = nullptr;
        bool isLineEnd = false;

        if (*ptr == '\r') {
            if ((ptr + 1 < end) && *(ptr + 1) == '\n') {
                lineEnd = ptr;
                ptr += 2;
            }
            else {
                lineEnd = ptr;
                ptr += 1;
            }
            isLineEnd = true;
        }
        else if (*ptr == '\n') {
            lineEnd = ptr;
            ptr += 1;
            isLineEnd = true;
        }
        else if (ptr == end - 1) {
            lineEnd = ptr + 1;
            ++ptr;
            isLineEnd = true;
        }
        else {
            ++ptr;
        }

        if (isLineEnd) {
            size_t lineLength = lineEnd - lineStart;

            if (lineLength > 0 && *lineStart == '#') {
                ParseComment(lineStart, lineLength);
            }
            else if (lineLength > 0) {
                const char* fieldStart = lineStart;
                size_t fieldLength = 0;
                size_t parserIndex = 0;

                PushTimestamp(ts_target, timestamp);

                prevTsIdx = currTsIdx;

                auto vec_ts = static_cast<std::vector<double>*>(ts_target);
                if (vec_ts)
                    currTsIdx = vec_ts->size() - 1;

                timestamp += CSMCCSVParser::TimestampDefaultInc;

                const char* p = lineStart;
                while (p <= lineEnd && parserIndex < parsers.size()) {
                    if (p == lineEnd || *p == m_delimiter) {
                        fieldLength = p - fieldStart;

                        if (parsers[parserIndex] && field_bindings[parserIndex].target) {
                            parsers[parserIndex](fieldStart, fieldLength, field_bindings[parserIndex].target, ts_target);
                        }

                        ++parserIndex;
                        fieldStart = p + 1;
                    }
                    ++p;
                }

                if (currTsIdx != prevTsIdx) {
                    parserIndex = 0;
                    while (parserIndex < extenders.size()) {
                        if (interpolators[parserIndex] && field_bindings[parserIndex].target) {
                            interpolators[parserIndex](prevTsIdx, currTsIdx, field_bindings[parserIndex].target, ts_target);
                        }
                        ++parserIndex;
                    }
                }

                parserIndex = 0;
                while (parserIndex < extenders.size()) {
                    if (extenders[parserIndex] && field_bindings[parserIndex].target) {
                        extenders[parserIndex](field_bindings[parserIndex].target, ts_target);
                    }
                    ++parserIndex;
                }

                ++m_rowCount;
            }

            lineStart = ptr;
        }
    }
}

void CSMCCSVParser::ParseComment(const char* lineStart, size_t length)
{
    (void)lineStart;
    (void)length;
}

void CSMCCSVParser::PushTimestamp(void* ts_target, double ts)
{
    auto* vec = static_cast<std::vector<double>*>(ts_target);
    vec->push_back(ts);
}

double CSMCCSVParser::LastTimestamp(void* ts_target)
{
    auto* vec = static_cast<std::vector<double>*>(ts_target);
    double ts = 0.0;
    if (vec->size())
        ts = vec->back();    
    return ts;
}

void CSMCCSVParser::ParseTimestamp(const char* data, size_t length, void* target, void* ts_target)
{
    // Parse the timestamp as a Unix timestamp or in another format
    std::string timestampStr(data, length);

    // If parsing Unix timestamp (integer)
    try {
        long timestamp = std::stol(timestampStr);
        *reinterpret_cast<std::time_t*>(target) = static_cast<std::time_t>(timestamp);
    }
    catch (const std::exception& e) {
        // Handle invalid format if needed
        *reinterpret_cast<std::time_t*>(target) = 0;  // or throw an exception
    }
}

template<typename T>
void CSMCCSVParser::ParseValue(const char* data, size_t length, void* target, void* ts_target)
{
    auto* vec = static_cast<std::vector<T>*>(target);
    T value = 0;

    // Skip leading whitespace
    while (length > 0 && std::isspace(static_cast<unsigned char>(*data))) {
        ++data;
        --length;
    }

    if constexpr (std::is_same_v<T, int> || std::is_same_v<T, uint32_t>)
    {
        auto [ptr, ec] = std::from_chars(data, data + length, value);
        vec->push_back((ec == std::errc()) ? value : 0);
    }
    else if constexpr (std::is_same_v<T, double>)
    {
        try {
            // Convert to std::string to support + or - and decimal parsing via stod
            std::string str(data, length);
            value = std::stod(str);
            vec->push_back(value);
        }
        catch (...) {
            vec->push_back(0.0); // fallback on failure
        }
    }
    else
    {
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_CSVPARSERUNKNOWNFIELDPARSERTYPE);
    }
}

template<typename T>
void CSMCCSVParser::ExtendVector(void* target, void* ts_target)
{
    auto* vec = static_cast<std::vector<T>*>(target);
    auto* vec_ts = static_cast<std::vector<double>*>(ts_target);

    auto diff = vec_ts->size() - vec->size();
    if (diff > 0 && !vec->empty()) {
        const T copy = vec->back();
        for (size_t i = 0; i < diff; ++i)
            vec->push_back(copy);
    }
}

template<typename T>
void CSMCCSVParser::InterpolateVector(size_t idx_from, size_t idx_to, void* target, void* ts_target)
{
    if (idx_from + 1 == idx_to)
        return; // nothing to interpolate

    auto* position = static_cast<std::vector<T>*>(target);
    auto* timestamp = static_cast<std::vector<double>*>(ts_target);

    if (idx_from >= timestamp->size() || idx_to >= timestamp->size() ||
        idx_from >= position->size() || idx_to >= position->size()) {
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_CSVPARSERINTERPOLATEINDEXOUTOFRANGE);
    }

    double t0 = timestamp->at(idx_from);
    double t1 = timestamp->at(idx_to);

    T x0 = position->at(idx_from);
    T x1 = position->at(idx_to);

    for (size_t i = idx_from + 1; i < idx_to; ++i)
    {
        double target_time = timestamp->at(i);
        double t = (target_time - t0) / (t1 - t0);

        T interpolated;
        if constexpr (std::is_floating_point<T>::value) {
            interpolated = static_cast<T>(x0 + t * (x1 - x0));
        }
        else {
            interpolated = static_cast<T>(std::round(x0 + t * (x1 - x0)));
        }

        position->at(i) = interpolated;
    }
}

void CSMCCSVParser::ParseBool(const char* data, size_t length, void* target, void* ts_target)
{
    auto* vec = static_cast<std::vector<bool>*>(target);
    bool value = false;

    if (length == 1) {
        value = (data[0] == '1' || std::tolower(data[0]) == 't');
    }
    else {
        std::string_view sv(data, length);
        if (sv == "true" || sv == "True" || sv == "TRUE" ||
            sv == "yes" || sv == "Yes" || sv == "YES") {
            value = true;
        }
    }

    vec->push_back(value);
}

void CSMCCSVParser::ParseString(const char* data, size_t length, void* target, void* ts_target)
{
    auto* vec = static_cast<std::vector<std::string>*>(target);
    vec->emplace_back(data, length);
}

void CSMCCSVParser::ParseLaserSignal(const char* data, size_t length, void* target, void* ts_target)
{
    auto* vec = static_cast<std::vector<uint32_t>*>(target);

    std::vector<SubCycle> sub_cycles;

    ParseLaserSignal_Internal(data, length, (void*)&sub_cycles, nullptr);
    vec->push_back(sub_cycles[0].Toggle);

    if (sub_cycles.size() > 1)
    {
        double ts = LastTimestamp(ts_target);

        for (int i = 1; i < sub_cycles.size(); ++i)
        {
            vec->push_back(sub_cycles[i].Toggle);

            if (sub_cycles.size() > 1)
                CSMCCSVParser::PushTimestamp(ts_target, ts + sub_cycles[i].TimeOffset);

        }
    }
}

CSMCCSVParser::ParserFunc CSMCCSVParser::GetParser(FieldParserType type)
{
    switch (type) {
    case FieldParserType::None:
        return nullptr;
    case FieldParserType::Timestamp:
        return nullptr; //return &CSMCCSVParser::ParseTimestamp;
    case FieldParserType::Int:
        return static_cast<ParserFunc>(&CSMCCSVParser::ParseValue<int32_t>);    //return &CSMCCSVParser::ParseInt32;
    case FieldParserType::UInt32:
        return static_cast<ParserFunc>(&CSMCCSVParser::ParseValue<uint32_t>);   //return &CSMCCSVParser::ParseUInt32;
    case FieldParserType::Double:
        return static_cast<ParserFunc>(&CSMCCSVParser::ParseValue<double>);     // return &CSMCCSVParser::ParseDouble;
    case FieldParserType::Bool:
        return &CSMCCSVParser::ParseBool;
    case FieldParserType::String:
        return &CSMCCSVParser::ParseString;
    case FieldParserType::LaserSignal:
        return &CSMCCSVParser::ParseLaserSignal;
    default:
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_CSVPARSERUNKNOWNFIELDPARSERTYPE);
    }
}

CSMCCSVParser::ExtenderFunc CSMCCSVParser::GetExtender(FieldParserType type) {
    switch (type) {
    case FieldParserType::None:
        return nullptr;
    case FieldParserType::Timestamp:
        return nullptr; //return &CSMCCSVParser::ParseTimestamp;
    case FieldParserType::Int:
        return static_cast<ExtenderFunc>(&CSMCCSVParser::ExtendVector<int32_t>);
    case FieldParserType::UInt32:
        return static_cast<ExtenderFunc>(&CSMCCSVParser::ExtendVector<uint32_t>);
    case FieldParserType::Double:
        return static_cast<ExtenderFunc>(&CSMCCSVParser::ExtendVector<double>);
    case FieldParserType::Bool:
        return static_cast<ExtenderFunc>(&CSMCCSVParser::ExtendVector<bool>);
    case FieldParserType::String:
        return static_cast<ExtenderFunc>(&CSMCCSVParser::ExtendVector<std::string>);
    case FieldParserType::LaserSignal:
        return nullptr;
    default:
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_CSVPARSERUNKNOWNFIELDPARSERTYPE);
    }
}

CSMCCSVParser::InterpolatorFunc CSMCCSVParser::GetInterpolator(FieldParserType type)
{
    switch (type) {
    case FieldParserType::None:
        return nullptr;
    case FieldParserType::Timestamp:
        return nullptr;
    case FieldParserType::Int:
        return static_cast<InterpolatorFunc>(&CSMCCSVParser::InterpolateVector<int32_t>);
    case FieldParserType::UInt32:
        return static_cast<InterpolatorFunc>(&CSMCCSVParser::InterpolateVector<uint32_t>);
    case FieldParserType::Double:
        return static_cast<InterpolatorFunc>(&CSMCCSVParser::InterpolateVector<double>);
    case FieldParserType::Bool:
        return nullptr;
    case FieldParserType::String:
        return nullptr;
    case FieldParserType::LaserSignal:
        return nullptr;
    default:
        throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_CSVPARSERUNKNOWNFIELDPARSERTYPE);
    }
}

void CSMCCSVParser::ParseLaserSignal_Internal(const char* data, size_t length, void* target, void* ts_target)
{
    if (!target || !data || length == 0)
        return;

    auto* outVec = static_cast<std::vector<SubCycle>*>(target);
    outVec->clear();

    const char* ptr = data;
    const char* end = data + length;

    // Step 1: Parse LaserActive
    const char* token_start = ptr;
    while (ptr < end && *ptr != '_') ++ptr;
    bool laserActive = (ptr - token_start == 1 && token_start[0] == '1');

    if (!laserActive) {
        outVec->push_back({ false, 0.0 });
        return;
    }

    // Move to next token (InitToggleState)
    if (ptr < end && *ptr == '_') ++ptr;

    // Step 2: Parse InitToggleState
    token_start = ptr;
    while (ptr < end && *ptr != '_') ++ptr;
    bool initState = false;

    if (ptr - token_start == 1 && token_start[0] == '1') initState = true;
    else if (ptr - token_start == 1 && token_start[0] == '0') initState = false;
    else return; // invalid InitToggleState
    
    outVec->push_back({ initState, 0.0 });

    // Move to first time offset
    if (ptr < end && *ptr == '_')
        ++ptr;
    else        
        return;

    SubCycle sub_cycle;
    sub_cycle.Toggle = !initState;

    while (ptr <= end) {
        // Step 5.1: Parse next token (time offset or empty)
        token_start = ptr;
        while (ptr < end && *ptr != '_') ++ptr;
        size_t token_len = ptr - token_start;

        if (token_len == 0) {
            sub_cycle.TimeOffset = 0.0;
        }
        else {
            std::vector<double> temp;
            //CSMCCSVParser::ParseDouble(token_start, token_len, &temp, ts_target);
            CSMCCSVParser::ParseValue<double>(token_start, token_len, &temp, ts_target);
            sub_cycle.TimeOffset = temp.empty() ? 0.0 : temp.back();
        }

        outVec->push_back(sub_cycle);

        // Step 5.2: Flip toggle state
        sub_cycle.Toggle = !sub_cycle.Toggle;

        if (ptr < end && *ptr == '_') ++ptr;
        else break;
    }

    // If no time offsets, still add initial with offset = 0
    if (outVec->empty()) {
        sub_cycle.TimeOffset = 0.0;
        outVec->push_back(sub_cycle);
    }
}

std::string CSMCCSVParser::FormatDouble(double value)
{
    char buffer[40]; // safe size
    std::snprintf(buffer, sizeof(buffer), "%+.9f", value);

    // Ensure "-0.000000000;" is preserved correctly (for -0.0)
    if (value == 0.0 && std::signbit(value)) {
        // replace first character with '-' if signbit is set
        buffer[0] = '-';
    }

    return std::string(buffer);
}