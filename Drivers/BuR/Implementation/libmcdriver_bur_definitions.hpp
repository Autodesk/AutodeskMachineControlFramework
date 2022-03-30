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

*/


#ifndef __LIBMCDRIVER_BUR_DRIVER_DEFINITIONS
#define __LIBMCDRIVER_BUR_DRIVER_DEFINITIONS

#include "libmcdriver_bur_interfaces.hpp"

// Parent classes
#include "libmcdriver_bur_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include <map>
#include <memory>
#include <functional>

namespace LibMCDriver_BuR {
namespace Impl {

    class CDriver_BuRPacket;
    typedef std::function<void(CDriver_BuRPacket* pPacket)> BurPacketCallback;

#pragma pack(push)
#pragma pack(1)

    struct sAMCFToPLCPacketPayload {
        uint8_t m_nData[24];
    };

    struct sAMCFToPLCPacketToSend {
        uint32_t m_CommandID;
        BurPacketCallback m_Callback;
        sAMCFToPLCPacketPayload m_Payload;
    };



#pragma pack(pop)

    enum class ePLCFieldType : int32_t {
        ftUnknown = 0,
        ftString = 1,
        ftInt = 2,
        ftDInt = 3,
        ftBool = 4,
        ftReal = 5,
    };

    class ITimeStampGenerator {
    public:
        virtual uint64_t generateTimeStamp() = 0;

    };

    class CDriver_BuRValue;
    typedef std::shared_ptr<CDriver_BuRValue> PDriver_BuRValue;

    class CDriver_BuRValue {
    protected:
        std::string m_sName;
        std::string m_sDescription;
        uint32_t m_nAddress;

        void writeDataToPayload(sAMCFToPLCPacketPayload& payLoad, uint8_t * pData, size_t nSize);
    public:

        CDriver_BuRValue(const std::string& sName, const std::string& sDescription, const uint32_t nAddress);
        virtual ~CDriver_BuRValue();

        std::string getName();
        std::string getDescription();
        uint32_t getAddress();

        virtual ePLCFieldType getFieldType() = 0;

        virtual PDriver_BuRValue duplicate() = 0;

        virtual void setIntegerValue (uint64_t nValue);
        virtual void setBoolValue(bool bValue);
        virtual void setDoubleValue(double dValue);

        virtual void writeToPayload(sAMCFToPLCPacketPayload& payLoad);

    };


    class CDriver_BuRRealValue : public CDriver_BuRValue {
    protected:
        double m_dValue;

    public:

        CDriver_BuRRealValue(const std::string& sName, const std::string& sDescription, const uint32_t nAddress);

        ePLCFieldType getFieldType() override;

        PDriver_BuRValue duplicate() override;

        void setDoubleValue(double dValue) override;

        void writeToPayload(sAMCFToPLCPacketPayload& payLoad) override;

    };


    class CDriver_BuRStringValue : public CDriver_BuRValue {
    protected:
        std::string m_sValue;
        uint32_t m_nLength;
    public:

        CDriver_BuRStringValue(const std::string& sName, const std::string& sDescription, const uint32_t nAddress, const uint32_t nLength);

        ePLCFieldType getFieldType() override;

        PDriver_BuRValue duplicate() override;

        uint32_t getLength();

    };


    class CDriver_BuRLRealValue : public CDriver_BuRValue {
    protected:
        double m_dValue;

    public:

        CDriver_BuRLRealValue(const std::string& sName, const std::string& sDescription, const uint32_t nAddress);

        ePLCFieldType getFieldType() override;

        PDriver_BuRValue duplicate() override;

        void setDoubleValue(double dValue) override;

        void writeToPayload(sAMCFToPLCPacketPayload& payLoad) override;

    };

    class CDriver_BuRDIntValue : public CDriver_BuRValue {
    protected:
        int64_t m_nValue;

    public:

        CDriver_BuRDIntValue(const std::string& sName, const std::string& sDescription, const uint32_t nAddress);

        ePLCFieldType getFieldType() override;

        PDriver_BuRValue duplicate() override;

        void setIntegerValue(uint64_t nValue) override;

        void writeToPayload(sAMCFToPLCPacketPayload& payLoad) override;

    };


    class CDriver_BuRIntValue : public CDriver_BuRValue {
    protected:
        int64_t m_nValue;

    public:

        CDriver_BuRIntValue(const std::string& sName, const std::string& sDescription, const uint32_t nAddress);

        ePLCFieldType getFieldType() override;

        PDriver_BuRValue duplicate() override;

        void setIntegerValue(uint64_t nValue) override;

        void writeToPayload(sAMCFToPLCPacketPayload& payLoad) override;

    };



    class CDriver_BuRBoolValue : public CDriver_BuRValue {
    protected:
        bool m_bValue;

    public:

        CDriver_BuRBoolValue(const std::string& sName, const std::string& sDescription, const uint32_t nAddress);

        ePLCFieldType getFieldType() override;

        PDriver_BuRValue duplicate() override;

        void setBoolValue(bool bValue) override;

        void writeToPayload(sAMCFToPLCPacketPayload& payLoad) override;
    };

    class CDriver_BuRCommandDefinition;
    typedef std::shared_ptr<CDriver_BuRCommandDefinition> PDriver_BuRCommandDefinition;


    class CDriver_BuRCommandDefinition {
    protected:
        std::string m_sName;
        uint32_t m_nCommandID;
        std::list<std::string> m_ParameterNames;
        std::map<std::string, PDriver_BuRValue> m_Parameters;

    public:

        CDriver_BuRCommandDefinition(const std::string& sName, const uint32_t nCommandID);
        virtual ~CDriver_BuRCommandDefinition();

        std::string getName();
        uint32_t getCommandID();

        void addParameter(PDriver_BuRValue pValue);
        std::list <std::string> getParameterNames();
        CDriver_BuRValue* findParameter(const std::string& sName);

        PDriver_BuRCommandDefinition duplicate();
    };

    typedef std::shared_ptr<CDriver_BuRCommandDefinition> PDriver_BuRCommandDefinition;



} // namespace Impl
} // namespace LibMCDriver_BuR

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_BUR_DRIVER_DEFINITIONS
