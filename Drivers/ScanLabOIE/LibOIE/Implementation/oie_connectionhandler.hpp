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

#ifndef __AMC_OIE_CONNECTIONHANDLER
#define __AMC_OIE_CONNECTIONHANDLER

#include <memory>
#include <string>
#include <vector>
#include <mutex>

#include "oie_packet.hpp"

namespace LibOIE::Impl {
	
	class CConnectionHandler {
	private:
		uint64_t m_nID;
		std::string m_sIPAddress;

		bool m_bNeedsToTerminate;
		std::mutex m_Mutex;

		std::vector<uint8_t> m_Buffer1;
		std::vector<uint8_t> m_Buffer2;

		std::vector<uint8_t> & m_CurrentBuffer;
		std::vector<uint8_t> & m_OtherBuffer;

		void pushToBuffer(std::vector<uint8_t>& Buffer, const uint8_t* pData, size_t nSize);

		bool predictPacketSize(const uint8_t* pReadData, size_t nReadSize, size_t & predictedSize);

	public:

		CConnectionHandler(const uint64_t nID, const std::string& sIPAddress);
		virtual ~CConnectionHandler ();

		uint64_t getID();

		void receivedData (const uint8_t * pData, size_t nSize);

		bool needsToTerminate();

		std::string getIPAddress();

		void handlePacket (PPacket pPacket);

	};
	
	typedef std::shared_ptr<CConnectionHandler> PConnectionHandler;

}


#endif //__AMC_OIE_CONNECTIONHANDLER

