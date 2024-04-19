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
#include <vector>
#include <list>
#include <set>

namespace AMCCommon {

	enum class eBase64Type : int {
		Unknown = 0,
		RFC4648 = 1,	
		URL = 2
	} ;


	class CUtils {
	private:

		static void characterIDToUTF16(uint32_t nCharacterID, uint16_t& nHighSurrogate, uint16_t& nLowSurrogate);

	public:

		static std::string createUUID();
		static std::string normalizeUUIDString(std::string sRawString);
		static std::string normalizeSHA256String(std::string sRawString);		
		static std::string createEmptyUUID();

		static std::string UTF16toUTF8(std::wstring sString);
		static std::wstring UTF8toUTF16(std::string sString);
		static bool UTF8StringIsValid (const std::string & sString);
		static std::string trimString (const std::string& sString);
		static std::string toLowerString(const std::string& sString);
		static void splitString (const std::string& sString, const std::string & sDelimiter, std::vector<std::string> & stringVector);
		static int64_t stringToInteger(const std::string& sString);
		static double stringToDouble(const std::string& sString);

		static std::string calculateSHA256FromFile(const std::string& sFileNameUTF8);
		static std::string calculateSHA256FromString(const std::string& sString);
		static std::string calculateSHA256FromData(const uint8_t * pData, uint64_t nDataSize);
		static std::string calculateRandomSHA256String(const uint32_t nIterations);
		static std::string calculateBlockwiseSHA256FromFile(const std::string& sFileNameUTF8, uint32_t nBlockSize);

		static std::string encodeBase64 (const std::string& sString, eBase64Type eType);
		static void decodeBase64(const std::string& sString, eBase64Type eType, std::vector<uint8_t> & byteBuffer);
		static std::string decodeBase64ToASCIIString(const std::string& sString, eBase64Type eType);

		static bool fileOrPathExistsOnDisk(const std::string& sPathName);
		static void deleteFileFromDisk(const std::string & sFileName, bool MustSucceed);
		static void deleteDirectoryFromDisk(const std::string& sPath, bool MustSucceed);
		static void createDirectoryOnDisk(const std::string& sPath);
		static bool pathIsDirectory(const std::string& sPathName);

		// ATTENTION: On Unix, this will try to create the path name if it does not exist!
		static std::string getFullPathName(const std::string& sRelativePath, bool bMustExist);

		static char getPathDelimiter();
		static std::string includeTrailingPathDelimiter(const std::string& sPathName);
		static std::string removeLeadingPathDelimiter(const std::string& sPathName);

		static std::string findTemporaryFileName(const std::string& sBasePath, const std::string& sPrefix, const std::string & sExtension, const uint32_t nMaxIterations);
		static std::set<std::string> findContentOfDirectory(const std::string & sDirectory, bool bReturnFiles, bool bReturnDirectories);

		static bool stringIsValidAlphanumericNameString(const std::string& sString); // Only alphanumeric characters and underscore, underscore not as first character!
		static bool stringIsValidAlphanumericPathString(const std::string& sString); // Only alphanumeric name strings separated by dots

	};

	
}


#endif //__AMC_UTILS

