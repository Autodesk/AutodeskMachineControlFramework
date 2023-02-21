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

*/


#ifndef __LIBMCDRIVER_CIFX_PARAMETER
#define __LIBMCDRIVER_CIFX_PARAMETER

#include "libmcdriver_cifx_interfaces.hpp"

#include <thread>
#include <mutex>
#include <atomic>
#include <map>
#include <vector>


namespace LibMCDriver_CifX {
namespace Impl {


	enum class eDriver_CifXParameterType : int32_t {
		CifXParameter_Unknown = 0,
		CifXParameter_BOOL = 1,
		CifXParameter_UINT8 = 2,
		CifXParameter_UINT16 = 3,
		CifXParameter_UINT32 = 4,
		CifXParameter_INT8 = 5,
		CifXParameter_INT16 = 6,
		CifXParameter_INT32 = 7,
		CifXParameter_FLOAT = 8,
		CifXParameter_DOUBLE = 9,
	};

	enum class eDriver_AbstractParameterType : int32_t {
		CifXAbstractParameter_Unknown = 0,
		CifXAbstractParameter_BOOL = 1,
		CifXAbstractParameter_INT = 2,
		CifXAbstractParameter_DOUBLE = 3
	};

	class CDriver_CifXParameter {
	private:
		std::string m_sName;
		std::string m_sDescription;
		eDriver_CifXParameterType m_eType;
		uint32_t m_nAddress;
	public:
		CDriver_CifXParameter(const std::string& sName, const std::string& sDescription, eDriver_CifXParameterType Type, const uint32_t nAddress);
		virtual ~CDriver_CifXParameter();

		std::string getName();
		std::string getDescription();

		virtual double GetActualDoubleValue();
		virtual int64_t GetActualIntegerValue();
		bool GetActualBoolValue();
		virtual void SetActualDoubleValue(double dValue);
		virtual void SetActualIntegerValue(int64_t nValue);
		void SetActualBoolValue(bool bValue);
		virtual double GetTargetDoubleValue();
		virtual int64_t GetTargetIntegerValue();
		bool GetTargetBoolValue();
		virtual void SetTargetDoubleValue(double dValue);
		virtual void SetTargetIntegerValue(int64_t nValue);
		void SetTargetBoolValue(bool bValue);

		eDriver_CifXParameterType getType();
		eDriver_AbstractParameterType getAbstractType();
		void getValueRange(int64_t& nMinValue, int64_t& nMaxValue);

		uint32_t getAddress();

		static void getValueRangeFromType(eDriver_CifXParameterType parameterType, int64_t& nMinValue, int64_t& nMaxValue);

	};

	typedef std::shared_ptr<CDriver_CifXParameter> PDriver_CifXParameter;


	class CDriver_CifXParameter_Integer : public CDriver_CifXParameter {
	private:
		int64_t m_nActualValue;
		int64_t m_nTargetValue;

	public:
		CDriver_CifXParameter_Integer(const std::string& sName, const std::string& sDescription, eDriver_CifXParameterType Type, const uint32_t nAddress);

		virtual ~CDriver_CifXParameter_Integer();

		virtual int64_t GetActualIntegerValue() override;

		virtual void SetActualIntegerValue(int64_t nValue) override;

		virtual int64_t GetTargetIntegerValue() override;

		virtual void SetTargetIntegerValue(int64_t nValue) override;
	};


	class CDriver_CifXParameter_Double : public CDriver_CifXParameter {
	private:
		double m_dActualValue;
		double m_dTargetValue;

	public:
		CDriver_CifXParameter_Double(const std::string& sName, const std::string& sDescription, eDriver_CifXParameterType Type, const uint32_t nAddress);

		virtual ~CDriver_CifXParameter_Double();

		virtual double GetActualDoubleValue() override;

		virtual void SetActualDoubleValue(double dValue) override;

		virtual double GetTargetDoubleValue() override;

		virtual void SetTargetDoubleValue(double dValue) override;
	};


	class CDriver_CifXParameter_Bool : public CDriver_CifXParameter_Integer {
	private:
		uint32_t m_nBit;

	public:
		CDriver_CifXParameter_Bool(const std::string& sName, const std::string& sDescription, eDriver_CifXParameterType Type, const uint32_t nAddress, const uint32_t nBit);

		virtual ~CDriver_CifXParameter_Bool();

		uint32_t getBit();

	};

} // namespace Impl
} // namespace LibMCDriver_CifX

#endif // __LIBMCDRIVER_CIFX_PARAMETER
