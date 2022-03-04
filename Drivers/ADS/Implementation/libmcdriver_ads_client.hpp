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

			void readBuffer(void * pData, uint32_t nLength);
			void writeBuffer(void* pData, uint32_t nLength);

		public:
			CADSClientVariable(PADSClientConnection pConnection, const std::string& sName, uint32_t Handle);
			virtual ~CADSClientVariable();

			std::string getName();
		};

		class CADSClientIntegerVariable : public CADSClientVariable {
		public:
			CADSClientIntegerVariable(PADSClientConnection pConnection, const std::string& sName, uint32_t Handle);
			virtual ~CADSClientIntegerVariable();

			virtual int64_t readValueFromPLC() = 0;
			virtual void writeValueToPLC(const int64_t nValue) = 0;
		};

		class CADSClientInt8Variable : public CADSClientIntegerVariable {
		public:
			CADSClientInt8Variable(PADSClientConnection pConnection, const std::string& sName, uint32_t Handle);
			virtual ~CADSClientInt8Variable();

			virtual int64_t readValueFromPLC() override;
			virtual void writeValueToPLC(const int64_t nValue) override;
		};

		class CADSClientUint8Variable : public CADSClientIntegerVariable {
		public:
			CADSClientUint8Variable(PADSClientConnection pConnection, const std::string& sName, uint32_t Handle);
			virtual ~CADSClientUint8Variable();

			virtual int64_t readValueFromPLC() override;
			virtual void writeValueToPLC(const int64_t nValue) override;
		};

		class CADSClientInt16Variable : public CADSClientIntegerVariable {
		public:
			CADSClientInt16Variable(PADSClientConnection pConnection, const std::string& sName, uint32_t Handle);
			virtual ~CADSClientInt16Variable();

			virtual int64_t readValueFromPLC() override;
			virtual void writeValueToPLC(const int64_t nValue) override;
		};

		class CADSClientUint16Variable : public CADSClientIntegerVariable {
		public:
			CADSClientUint16Variable(PADSClientConnection pConnection, const std::string& sName, uint32_t Handle);
			virtual ~CADSClientUint16Variable();

			virtual int64_t readValueFromPLC() override;
			virtual void writeValueToPLC(const int64_t nValue) override;
		};


		class CADSClientInt32Variable : public CADSClientIntegerVariable {
		public:
			CADSClientInt32Variable(PADSClientConnection pConnection, const std::string& sName, uint32_t Handle);
			virtual ~CADSClientInt32Variable();

			virtual int64_t readValueFromPLC() override;
			virtual void writeValueToPLC(const int64_t nValue) override;
		};

		class CADSClientUint32Variable : public CADSClientIntegerVariable {
		public:
			CADSClientUint32Variable(PADSClientConnection pConnection, const std::string& sName, uint32_t Handle);
			virtual ~CADSClientUint32Variable();

			virtual int64_t readValueFromPLC() override;
			virtual void writeValueToPLC(const int64_t nValue) override;
		};

		typedef std::shared_ptr<CADSClientVariable> PADSClientVariable;
		typedef std::shared_ptr<CADSClientInt8Variable> PADSClientInt8Variable;
		typedef std::shared_ptr<CADSClientUint8Variable> PADSClientUint8Variable;
		typedef std::shared_ptr<CADSClientInt16Variable> PADSClientInt16Variable;
		typedef std::shared_ptr<CADSClientUint16Variable> PADSClientUint16Variable;
		typedef std::shared_ptr<CADSClientInt32Variable> PADSClientInt32Variable;
		typedef std::shared_ptr<CADSClientUint32Variable> PADSClientUint32Variable;

		class CADSClient {
		private:
			PADSSDK m_pSDK;
			AdsVersion m_Version;

			PADSClientConnection m_pCurrentConnection;

			uint32_t getVariableHandle(const std::string & sName);

		public:
			CADSClient (PADSSDK pSDK);
			virtual ~CADSClient();

			void connect();
			void disconnect();

			std::string getVersionString();

			PADSClientInt8Variable registerInt8Variable(const std::string & sName);
			PADSClientUint8Variable registerUint8Variable(const std::string& sName);
			PADSClientInt16Variable registerInt16Variable(const std::string& sName);
			PADSClientUint16Variable registerUint16Variable(const std::string& sName);
			PADSClientInt32Variable registerInt32Variable(const std::string& sName);
			PADSClientUint32Variable registerUint32Variable(const std::string& sName);

		};

		typedef std::shared_ptr<CADSClient> PADSClient;

	} 
} 

#endif // __LIBMCDRIVER_ADS_CLIENT
