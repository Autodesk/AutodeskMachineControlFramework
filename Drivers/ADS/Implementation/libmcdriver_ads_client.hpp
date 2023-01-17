/*++

Copyright (C) 2020 Autodesk Inc.

All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/


#ifndef __LIBMCDRIVER_ADS_CLIENT
#define __LIBMCDRIVER_ADS_CLIENT

#include "libmcdriver_ads_sdk.hpp"

#include <memory>
#include <string>
#include <map>
#include <vector>


#define ADS_MINSTRINGLENGTH 2
#define ADS_MAXSTRINGLENGTH 65536

namespace LibMCDriver_ADS {
	namespace Impl {

		class CADSClientConnection {
		private:
			PADSSDK m_pSDK;
			AdsPort m_Port;
			sAmsAddr m_LocalAddress;

		public:
			CADSClientConnection(PADSSDK pSDK, AdsPort nPort, sAmsAddr localAddress);

			CADSSDK* getSDK();
			AdsPort getPort();
			sAmsAddr* getAddressP();

			void disconnect();
		};

		typedef std::shared_ptr<CADSClientConnection> PADSClientConnection;

		class CADSClientVariable {
		private:
			PADSClientConnection m_pConnection;

			std::string m_sName;
			uint32_t m_Handle;
		protected:

			void readBuffer(void* pData, uint32_t nLength);
			void writeBuffer(void* pData, uint32_t nLength);

		public:
			CADSClientVariable(PADSClientConnection pConnection, const std::string& sName, uint32_t Handle);
			virtual ~CADSClientVariable();

			std::string getName();
		};

		class CADSClientStringVariable : public CADSClientVariable {
		private:
			size_t m_nStringSize;
		public:
			CADSClientStringVariable(PADSClientConnection pConnection, const std::string& sName, uint32_t Handle, size_t nStringSize);
			virtual ~CADSClientStringVariable();

			virtual std::string readValueFromPLC();
			virtual void writeValueToPLC(const std::string& sValue);

		};

		class CADSClientIntegerVariable : public CADSClientVariable {
		public:
			CADSClientIntegerVariable(PADSClientConnection pConnection, const std::string& sName, uint32_t Handle);
			virtual ~CADSClientIntegerVariable();

			virtual int64_t readValueFromPLC() = 0;
			virtual void writeValueToPLC(const int64_t nValue) = 0;

			virtual void getBounds(int64_t& minValue, int64_t& maxValue) = 0;
		};


		class CADSClientInt8Variable : public CADSClientIntegerVariable {
		public:
			CADSClientInt8Variable(PADSClientConnection pConnection, const std::string& sName, uint32_t Handle);
			virtual ~CADSClientInt8Variable();

			virtual int64_t readValueFromPLC() override;
			virtual void writeValueToPLC(const int64_t nValue) override;

			virtual void getBounds(int64_t& minValue, int64_t& maxValue) override;
		};

		class CADSClientBoolVariable : public CADSClientIntegerVariable {
		public:
			CADSClientBoolVariable(PADSClientConnection pConnection, const std::string& sName, uint32_t Handle);
			virtual ~CADSClientBoolVariable();

			virtual int64_t readValueFromPLC() override;
			virtual void writeValueToPLC(const int64_t nValue) override;

			virtual bool readBooleanValueFromPLC();
			virtual void writeBooleanValueToPLC(const bool bValue);

			virtual void getBounds(int64_t& minValue, int64_t& maxValue) override;
		};


		class CADSClientUint8Variable : public CADSClientIntegerVariable {
		public:
			CADSClientUint8Variable(PADSClientConnection pConnection, const std::string& sName, uint32_t Handle);
			virtual ~CADSClientUint8Variable();

			virtual int64_t readValueFromPLC() override;
			virtual void writeValueToPLC(const int64_t nValue) override;

			virtual void getBounds(int64_t& minValue, int64_t& maxValue) override;
		};

		class CADSClientInt16Variable : public CADSClientIntegerVariable {
		public:
			CADSClientInt16Variable(PADSClientConnection pConnection, const std::string& sName, uint32_t Handle);
			virtual ~CADSClientInt16Variable();

			virtual int64_t readValueFromPLC() override;
			virtual void writeValueToPLC(const int64_t nValue) override;

			virtual void getBounds(int64_t& minValue, int64_t& maxValue) override;
		};

		class CADSClientUint16Variable : public CADSClientIntegerVariable {
		public:
			CADSClientUint16Variable(PADSClientConnection pConnection, const std::string& sName, uint32_t Handle);
			virtual ~CADSClientUint16Variable();

			virtual int64_t readValueFromPLC() override;
			virtual void writeValueToPLC(const int64_t nValue) override;

			virtual void getBounds(int64_t& minValue, int64_t& maxValue) override;
		};


		class CADSClientInt32Variable : public CADSClientIntegerVariable {
		public:
			CADSClientInt32Variable(PADSClientConnection pConnection, const std::string& sName, uint32_t Handle);
			virtual ~CADSClientInt32Variable();

			virtual int64_t readValueFromPLC() override;
			virtual void writeValueToPLC(const int64_t nValue) override;

			virtual void getBounds(int64_t& minValue, int64_t& maxValue) override;
		};

		class CADSClientUint32Variable : public CADSClientIntegerVariable {
		public:
			CADSClientUint32Variable(PADSClientConnection pConnection, const std::string& sName, uint32_t Handle);
			virtual ~CADSClientUint32Variable();

			virtual int64_t readValueFromPLC() override;
			virtual void writeValueToPLC(const int64_t nValue) override;

			virtual void getBounds(int64_t& minValue, int64_t& maxValue) override;
		};

		class CADSClientFloatVariable : public CADSClientVariable {
		public:
			CADSClientFloatVariable(PADSClientConnection pConnection, const std::string& sName, uint32_t Handle);
			virtual ~CADSClientFloatVariable();

			virtual double readValueFromPLC() = 0;
			virtual void writeValueToPLC(const double nValue) = 0;
		};

		class CADSClientFloat32Variable : public CADSClientFloatVariable {
		public:
			CADSClientFloat32Variable(PADSClientConnection pConnection, const std::string& sName, uint32_t Handle);
			virtual ~CADSClientFloat32Variable();

			virtual double readValueFromPLC() override;
			virtual void writeValueToPLC(const double nValue) override;
		};

		class CADSClientFloat64Variable : public CADSClientFloatVariable {
		public:
			CADSClientFloat64Variable(PADSClientConnection pConnection, const std::string& sName, uint32_t Handle);
			virtual ~CADSClientFloat64Variable();

			virtual double readValueFromPLC() override;
			virtual void writeValueToPLC(const double nValue) override;
		};

		typedef std::shared_ptr<CADSClientVariable> PADSClientVariable;
		typedef std::shared_ptr<CADSClientBoolVariable> PADSClientBoolVariable;
		typedef std::shared_ptr<CADSClientStringVariable> PADSClientStringVariable;
		typedef std::shared_ptr<CADSClientInt8Variable> PADSClientInt8Variable;
		typedef std::shared_ptr<CADSClientUint8Variable> PADSClientUint8Variable;
		typedef std::shared_ptr<CADSClientInt16Variable> PADSClientInt16Variable;
		typedef std::shared_ptr<CADSClientUint16Variable> PADSClientUint16Variable;
		typedef std::shared_ptr<CADSClientInt32Variable> PADSClientInt32Variable;
		typedef std::shared_ptr<CADSClientUint32Variable> PADSClientUint32Variable;
		typedef std::shared_ptr<CADSClientFloat32Variable> PADSClientFloat32Variable;
		typedef std::shared_ptr<CADSClientFloat64Variable> PADSClientFloat64Variable;

		class CADSClient {
		private:
			PADSSDK m_pSDK;
			AdsVersion m_Version;
			std::vector<PADSClientVariable> m_Variables;
			std::map<std::string, PADSClientVariable> m_VariableMap;

			PADSClientConnection m_pCurrentConnection;

			uint32_t getVariableHandle(const std::string& sName);
			void registerVariable(PADSClientVariable pVariable);

		public:
			CADSClient(PADSSDK pSDK);
			virtual ~CADSClient();

			void connect(uint32_t nPortNumber);
			void disconnect();

			std::string getVersionString();

			PADSClientBoolVariable registerBoolVariable(const std::string& sName);
			PADSClientInt8Variable registerInt8Variable(const std::string& sName);
			PADSClientUint8Variable registerUint8Variable(const std::string& sName);
			PADSClientInt16Variable registerInt16Variable(const std::string& sName);
			PADSClientUint16Variable registerUint16Variable(const std::string& sName);
			PADSClientInt32Variable registerInt32Variable(const std::string& sName);
			PADSClientUint32Variable registerUint32Variable(const std::string& sName);

			PADSClientFloat32Variable registerFloat32Variable(const std::string& sName);
			PADSClientFloat64Variable registerFloat64Variable(const std::string& sName);

			PADSClientStringVariable registerStringVariable(const std::string& sName, const size_t nStringBufferSize);

			CADSClientVariable* findVariable(const std::string& sName, bool bFailIfNotExisting);
			CADSClientIntegerVariable* findIntegerVariable(const std::string& sName, bool bFailIfNotExisting);
			CADSClientBoolVariable* findBoolVariable(const std::string& sName, bool bFailIfNotExisting);
			CADSClientFloatVariable* findFloatVariable(const std::string& sName, bool bFailIfNotExisting);
			CADSClientStringVariable* findStringVariable(const std::string& sName, bool bFailIfNotExisting);

		};

		typedef std::shared_ptr<CADSClient> PADSClient;

	}
}

#endif // __LIBMCDRIVER_ADS_CLIENT
