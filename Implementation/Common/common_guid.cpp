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
#include <vector>


#ifdef _WIN32
#define _WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <wincrypt.h>
#endif

#ifdef _WIN32
#include <ShlObj_core.h>
#endif


#include "crossguid/guid.hpp"
#include "PicoSHA2/picosha2.h"
#include "common_utils.hpp"

#define LIBMC_MAXRANDOMSTRINGITERATIONS 1024

namespace AMCCommon {


#ifdef _WASM
	xg::Guid xg::newGuid()
	{
		std::array<unsigned char, 16> bytes;
		for (int index = 0; index < 16; index++) {
			bytes[index] = rand() % 256;
		}
		return xg::Guid{ std::move(bytes) };

	}
#endif //_WASM

	std::string CUtils::createUUID()
	{
		auto guid = xg::newGuid();
		return normalizeUUIDString(guid.str());
	}


	std::string CUtils::calculateRandomSHA256String(const uint32_t nIterations)
	{
		if ((nIterations == 0) || (nIterations > LIBMC_MAXRANDOMSTRINGITERATIONS))
			throw std::runtime_error("invalid random string iterations");

#ifdef _WIN32

		// Check if Win32 System Crypto generated enough random data..
		HCRYPTPROV hProv = 0;
		if (CryptAcquireContext(&hProv, nullptr, nullptr, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {

			std::vector<uint8_t> salt;
			salt.resize(nIterations * 32);

			if (CryptGenRandom(hProv, (uint32_t) salt.size (), salt.data())) {

				CryptReleaseContext(hProv, 0);
				return AMCCommon::CUtils::calculateSHA256FromData (salt.data (), salt.size ());
			}
			else {
				CryptReleaseContext(hProv, 0);
			}

		}

#endif //_WIN32


		std::string sRandomString;
		uint32_t nCount = nIterations + (((uint32_t)rand()) % nIterations);
		for (uint32_t nIndex = 0; nIndex < nCount; nIndex++)
			sRandomString += createUUID();
		return calculateSHA256FromString(sRandomString);
	}


	// Returns a UUID, such that sBasePath/sPrefixUUID.extension does not exist.
	// Tries out maximum nMaxIterations different random uuids.
	std::string CUtils::findTemporaryFileName(const std::string& sBasePath, const std::string& sPrefix, const std::string& sExtension, const uint32_t nMaxIterations)
	{
		if (sBasePath.empty())
			throw std::runtime_error("empty temporary file base path");

		std::string sBasePathWithDelimiter = sBasePath;
		std::string sExtensionWithPoint;

		if (!sExtension.empty()) {
			if (sExtension.at(0) != '.') {
				sExtensionWithPoint = "." + sExtension;
			}
			else {
				sExtensionWithPoint = sExtension;
			}

		}

		char lastChar = sBasePathWithDelimiter.at(sBasePathWithDelimiter.length() - 1);
		if ((lastChar != '/') && (lastChar != '\\'))
			sBasePathWithDelimiter += "/";

		for (uint32_t nIndex = 0; nIndex < nMaxIterations; nIndex++) {
			std::string sUUID = createUUID();
			std::string sFullPath = sBasePathWithDelimiter + sPrefix + sUUID + sExtensionWithPoint;

			if (!fileOrPathExistsOnDisk(sFullPath))
				return sFullPath;

		}

		throw std::runtime_error("could not create temporary file path");

	}


	std::string CUtils::getCurrentUserHomeDirectory()
	{
#ifdef _WIN32
		PWSTR roamingAppDataPath = nullptr;
		if (SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_DEFAULT, 0, &roamingAppDataPath) == S_OK) {

			if (roamingAppDataPath == nullptr)
				throw std::runtime_error("could not return user home directory");

			std::wstring sHomeDirectoryW(roamingAppDataPath);
			CoTaskMemFree(roamingAppDataPath);

			return UTF16toUTF8(sHomeDirectoryW);
		}
		else
			throw std::runtime_error("could not return user home directory");

#else
		throw std::runtime_error("getCurrentUserHomeDirectory not implemented");

#endif

	}

}