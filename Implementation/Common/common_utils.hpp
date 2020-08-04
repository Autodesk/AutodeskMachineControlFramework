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


#ifndef __AMC_UTILS
#define __AMC_UTILS

#include <memory>
#include <string>


namespace AMCCommon {


	class CUtils {
	private:

		static void characterIDToUTF16(uint32_t nCharacterID, uint16_t& nHighSurrogate, uint16_t& nLowSurrogate);

	public:

		static std::string getCurrentISO8601TimeUTC();
		static std::string getCurrentTimeFileName();
		static std::string createUUID();
		static std::string normalizeUUIDString(std::string sRawString);
		
		static std::string UTF16toUTF8(std::wstring sString);
		static std::wstring UTF8toUTF16(std::string sString);

		static void sleepMilliseconds(const uint32_t milliSeconds);
		static void deleteFileFromDisk(const std::string & sFileName, bool MustSucceed);
	};

	
}


#endif //__AMC_UTILS

