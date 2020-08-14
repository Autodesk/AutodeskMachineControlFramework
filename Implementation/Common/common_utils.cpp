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


#include <string>
#include <algorithm>
#include <vector>
#include <chrono>
#include <sstream>
#include <exception>
#include <iomanip>
#include <thread>

#include "crossguid/guid.hpp"

#ifdef _WIN32
#include <objbase.h>
#include <iomanip>
#else
#include <ctime>
#endif



#include "common_utils.hpp"

namespace AMCCommon {

#define LIBMC_MAXSTRINGBUFFERSIZE (1024 * 1024 * 1024)

	// Lookup table to convert UTF8 bytes to sequence length
	const unsigned char UTF8DecodeTable[256] = {
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
		2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
		3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
		4,4,4,4,4,4,4,4,5,5,5,5,6,6,0,0
	};

	// Masks to decode highest UTF8 sequence byte
	const unsigned char UTF8DecodeMask[7] = { 0, 0x7f, 0x1f, 0x0f, 0x07, 0x03, 0x01 };


	std::string CUtils::getCurrentISO8601TimeUTC() {

#ifdef _WIN32
		struct tm utc_time;

		__int64 ltime;
		_time64(&ltime);

		errno_t err;
		err = _gmtime64_s(&utc_time, &ltime);		
#else
		std::time_t t = std::time(nullptr);
		std::tm utc_time = *std::localtime(&t);
#endif	

		std::stringstream sstream;
		sstream << std::put_time(&utc_time, "%FT%TZ") << " UTC";
		return sstream.str();

	}

	std::string CUtils::getCurrentTimeFileName() {

#ifdef _WIN32
		struct tm utc_time;
		__int64 ltime;
		_time64(&ltime);

		errno_t err;
		err = _gmtime64_s(&utc_time, &ltime);

#else
		std::time_t t = std::time(nullptr);
		std::tm utc_time = *std::localtime(&t);

#endif	

		std::stringstream sstream;
		sstream << std::put_time(&utc_time, "%Y%m%d_%H%M%S");
		return sstream.str();


	}


	bool fnUTF16CharIsSurrogate(wchar_t cChar)
	{
		uint32_t nSignature = (cChar & 0xfc00);
		return (nSignature == 0xd800) || (nSignature == 0xdc00);
	}

	bool fnUTF16CharIsHighSurrogate(wchar_t cChar)
	{
		uint32_t nSignature = (cChar & 0xfc00);
		return (nSignature == 0xd800);
	}

	bool fnUTF16CharIsLowSurrogate(wchar_t cChar)
	{
		uint32_t nSignature = (cChar & 0xfc00);
		return (nSignature == 0xdc00);
	}

	uint32_t fnUTF16toCharacterID(uint16_t nHighSurrogate, uint16_t nLowSurrogate)
	{
		if ((fnUTF16CharIsHighSurrogate(nLowSurrogate)) && (fnUTF16CharIsLowSurrogate(nHighSurrogate))) {
			std::swap(nLowSurrogate, nHighSurrogate); // UTF16LE
		}

		if ((!fnUTF16CharIsHighSurrogate(nHighSurrogate)) || (!fnUTF16CharIsLowSurrogate(nLowSurrogate)))
			throw std::runtime_error ("could not convert to utf8");

		uint32_t nCode = (((uint32_t)(nHighSurrogate & 0x3ff)) << 10 | ((uint32_t)(nLowSurrogate & 0x3ff)));
		return nCode + 0x10000;
	}


	std::string CUtils::UTF16toUTF8(const std::wstring sString)
	{

		// Check Input Sanity
		size_t nLength = sString.length();
		if (nLength == 0)
			return "";
		if (nLength > LIBMC_MAXSTRINGBUFFERSIZE)
			throw std::runtime_error("invalid buffer size");

		// Reserve UTF8 Buffer
		uint32_t nBufferSize = (uint32_t)nLength * 4 + 1;
		std::vector<char> Buffer;
		Buffer.resize(nBufferSize);

		// nfInt32 nResult;
		// Alternative: Convert via Win API
		// nResult = WideCharToMultiByte(CP_UTF8, 0, sString.c_str(), (nfUint32)nLength, &Buffer[0], nBufferSize, nullptr, nullptr);
		// if (nResult == 0)
		//	throw CNMRException(NMR_ERROR_COULDNOTCONVERTTOUTF8);

		const wchar_t* pChar = sString.c_str();
		char* pOutput = &Buffer[0];

		while (*pChar) {
			wchar_t cChar = *pChar;
			uint32_t nCharacter;
			pChar++;

			if (fnUTF16CharIsSurrogate(cChar)) {
				wchar_t cLowChar = *pChar;
				if (cLowChar == 0)
					throw std::runtime_error("could not convert to utf8");
				pChar++;

				nCharacter = fnUTF16toCharacterID(cChar, cLowChar);
			}
			else {
				nCharacter = cChar;
			}

			if (nCharacter < 0x80) {
				// One Byte Encoding
				*pOutput = nCharacter;
				pOutput++;
			}
			else if (nCharacter < 0x800) {
				// Two Byte Encoding
				*pOutput = (nCharacter >> 6) | 0xC0;
				pOutput++;
				*pOutput = (nCharacter & 0x3f) | 0x80;
				pOutput++;
			}
			else if (nCharacter < 0x10000) {
				// Three Byte Encoding
				*pOutput = (nCharacter >> 12) | 0xE0;
				pOutput++;
				*pOutput = ((nCharacter >> 6) & 0x3f) | 0x80;
				pOutput++;
				*pOutput = (nCharacter & 0x3f) | 0x80;
				pOutput++;
			}
			else if (nCharacter < 0x200000) {
				// Four Byte Encoding
				*pOutput = (nCharacter >> 18) | 0xF0;
				pOutput++;
				*pOutput = ((nCharacter >> 12) & 0x3f) | 0x80;
				pOutput++;
				*pOutput = ((nCharacter >> 6) & 0x3f) | 0x80;
				pOutput++;
				*pOutput = (nCharacter & 0x3f) | 0x80;
				pOutput++;
			}
			else if (nCharacter < 0x4000000) {
				// Five Byte Encoding
				*pOutput = (nCharacter >> 24) | 0xF8;
				pOutput++;
				*pOutput = ((nCharacter >> 18) & 0x3f) | 0x80;
				pOutput++;
				*pOutput = ((nCharacter >> 12) & 0x3f) | 0x80;
				pOutput++;
				*pOutput = ((nCharacter >> 6) & 0x3f) | 0x80;
				pOutput++;
				*pOutput = (nCharacter & 0x3f) | 0x80;
				pOutput++;
			}
			else {
				// Six Byte Encoding
				*pOutput = (nCharacter >> 30) | 0xFC;
				pOutput++;
				*pOutput = ((nCharacter >> 24) & 0x3f) | 0x80;
				pOutput++;
				*pOutput = ((nCharacter >> 18) & 0x3f) | 0x80;
				pOutput++;
				*pOutput = ((nCharacter >> 12) & 0x3f) | 0x80;
				pOutput++;
				*pOutput = ((nCharacter >> 6) & 0x3f) | 0x80;
				pOutput++;
				*pOutput = (nCharacter & 0x3f) | 0x80;
				pOutput++;
			}

		}

		// write end byte
		*pOutput = 0;

		return std::string(&Buffer[0]);

	}

	void CUtils::characterIDToUTF16(uint32_t nCharacterID, uint16_t& nHighSurrogate, uint16_t& nLowSurrogate)
	{
		if ((nCharacterID < 0x10000) || (nCharacterID > 0x10FFFF))
			throw std::runtime_error("could not convert to utf16");
		nCharacterID -= 0x10000;
		nHighSurrogate = (nCharacterID >> 10) | 0xd800;
		nLowSurrogate = (nCharacterID & 0x3ff) | 0xdc00;
	}

	bool CUtils::UTF8StringIsValid(const std::string& sString)
	{
		const char* pChar = sString.c_str();

		while (*pChar) {
			unsigned char cChar = (unsigned char)*pChar;
			uint32_t nLength = UTF8DecodeTable[(uint32_t)cChar];
			pChar++;

			if (nLength == 0)
				return false;
			if (nLength > 6)
				return false;

			// Check for BOM (0xEF,0xBB,0xBF), this also checks for #0 characters at the end,
			// so it does not read over the string end!
			bool bIsBOM = false;
			if (cChar == 0xef) {
				if (*((const unsigned char*)pChar) == 0xbb) {
					if (*((const unsigned char*)(pChar + 1)) == 0xbf) {
						bIsBOM = true;
					}
				}
			};


			if (!bIsBOM) {
				uint32_t nCode = cChar & UTF8DecodeMask[nLength];

				while (nLength > 1) {
					cChar = *pChar;
					if ((cChar & 0xc0) != 0x80)
						return false;
					pChar++;

					// Map UTF8 sequence to code
					nCode = (nCode << 6) | (cChar & 0x3f);
					nLength--;
				}

				// Map Code to UTF16
				if ((nCode < 0xd800) || ((nCode >= 0xe000) && (nCode <= 0xffff))) {
					// everything is good in this case

				}
				else {
					if ((nCode < 0x10000) || (nCode > 0x10FFFF))
						return false;

					// everything is good in this case
				}
			}
			else {
				// If we find a UTF8 bom, we just ignore it.
				if (nLength != 3)
					return false;
				pChar += 2;
			}
		}


		// success
		return true;
	}

	
	std::string CUtils::trimString(const std::string& sString)
	{
		std::string s = sString;
		// left trim
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
			return !std::isspace(ch);
		}));

		// right trim
		s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
			return !std::isspace(ch);
		}).base(), s.end());

		return s;
	}

	std::string CUtils::toLowerString(const std::string& sString)
	{
		std::string s = sString;
		std::transform(s.begin(), s.end(), s.begin(),
			[](unsigned char c) { return std::tolower(c); } // correct
		);
		return s;
	}


    std::wstring CUtils::UTF8toUTF16(const std::string sString)
	{

		// Check Input Sanity
		size_t nLength = sString.length();
		if (nLength == 0)
			return L"";
		if (nLength > LIBMC_MAXSTRINGBUFFERSIZE)
			throw std::runtime_error("invalid buffer size");

		// Reserve UTF8 Buffer
		uint32_t nBufferSize = (uint32_t)nLength;
		std::vector<wchar_t> Buffer;
		Buffer.resize(nBufferSize * 2 + 2);

		const char * pChar = sString.c_str();
		wchar_t* pOutput = &Buffer[0];

		while (*pChar) {
			unsigned char cChar = (unsigned char)*pChar;
			uint32_t nLength = UTF8DecodeTable[(uint32_t)cChar];
			pChar++;

			if (nLength == 0)
				throw std::runtime_error("could not convert to utf16");
			if (nLength > 6)
				throw std::runtime_error("could not convert to utf16");

			// Check for BOM (0xEF,0xBB,0xBF), this also checks for #0 characters at the end,
			// so it does not read over the string end!
			bool bIsBOM = false;
			if (cChar == 0xef) {
				if (*((const unsigned char*)pChar) == 0xbb) {
					if (*((const unsigned char*)(pChar + 1)) == 0xbf) {
						bIsBOM = true;
					}
				}
			};


			if (!bIsBOM) {
				uint32_t nCode = cChar & UTF8DecodeMask[nLength];

				while (nLength > 1) {
					cChar = *pChar;
					if ((cChar & 0xc0) != 0x80)
						throw std::runtime_error("could not convert to utf16");
					pChar++;

					// Map UTF8 sequence to code
					nCode = (nCode << 6) | (cChar & 0x3f);
					nLength--;
				}

				// Map Code to UTF16
				if ((nCode < 0xd800) || ((nCode >= 0xe000) && (nCode <= 0xffff))) {
					*pOutput = nCode;
					pOutput++;
				}
				else {
					uint16_t nHighSurrogate, nLowSurrogate;
					characterIDToUTF16(nCode, nHighSurrogate, nLowSurrogate);
					*pOutput = nHighSurrogate;
					pOutput++;
					*pOutput = nLowSurrogate;
					pOutput++;
				}
			}
			else {
				// If we find a UTF8 bom, we just ignore it.
				if (nLength != 3)
					throw std::runtime_error("could not convert to utf16");
				pChar += 2;
			}
		}

		// write end byte
		*pOutput = 0;

		return std::wstring(&Buffer[0]);

	}


	bool UUIDInValid(char c)
	{
		return !(((c >= '0') && (c <= '9'))
			|| ((c >= 'a') && (c <= 'f'))
			// || (c == '-')
			);
	}

	std::string CUtils::normalizeUUIDString(std::string sRawString)
	{
		
		// to lowercase, remove all non hex characters, insert dashes again
		std::transform(sRawString.begin(), sRawString.end(), sRawString.begin(), ::tolower);
		sRawString.erase(std::remove_if(sRawString.begin(), sRawString.end(), &UUIDInValid), sRawString.end());
		if (sRawString.length() != 32) {
			throw std::runtime_error("invalid uuid string " + sRawString);
		}
		return sRawString.substr(0, 8) + '-' + sRawString.substr(8, 4) + '-' + sRawString.substr(12, 4) + '-' + sRawString.substr(16, 4) + '-' + sRawString.substr(20, 12);
	}


	std::string CUtils::normalizeSHA256String(std::string sRawString)
	{
		// to lowercase, remove all non hex characters, insert dashes again
		std::transform(sRawString.begin(), sRawString.end(), sRawString.begin(), ::tolower);
		sRawString.erase(std::remove_if(sRawString.begin(), sRawString.end(), &UUIDInValid), sRawString.end());
		if (sRawString.length() != 64) {
			throw std::runtime_error("invalid sha256 string " + sRawString);
		}

		return sRawString;
	}



	std::string CUtils::createUUID()
	{
		auto guid = xg::newGuid ();		
		return normalizeUUIDString (guid.str());
	}

	void CUtils::sleepMilliseconds(const uint32_t milliSeconds)
	{
#ifdef _WIN32
			std::this_thread::sleep_for(std::chrono::milliseconds(milliSeconds));
#else
		struct timespec tim, tim2;
		tim.tv_sec = milliSeconds / 1000;
		tim.tv_nsec = (milliSeconds % 1000) * 1000000;

		if (nanosleep(&tim, &tim2) < 0) {
			throw std::runtime_error("sleep failed");
		}

#endif

	}


	void CUtils::deleteFileFromDisk(const std::string& sFileName, bool bMustSucceed)
	{
#ifdef _WIN32
		std::wstring sFileNameUTF16 = UTF8toUTF16(sFileName);
		if (!DeleteFileW(sFileNameUTF16.c_str())) {
			if (bMustSucceed)
				throw std::runtime_error("could not delete file: " + sFileName);		
		}
#else
		if (std::remove(sFileName)) {
			throw std::runtime_error("could not delete file: " + sFileName);
		}
#endif
	}



}
