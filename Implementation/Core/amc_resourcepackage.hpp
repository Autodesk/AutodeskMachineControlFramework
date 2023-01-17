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


#ifndef __AMC_RESOURCEPACKAGE
#define __AMC_RESOURCEPACKAGE
#include <memory>
#include <mutex>
#include <map>
#include <vector>

#include "common_importstream.hpp"

namespace AMC {

	class CResourcePackageZIP;
	typedef std::shared_ptr<CResourcePackageZIP> PResourcePackageZIP;

	class CResourcePackageEntry;
	typedef std::shared_ptr<CResourcePackageEntry> PResourcePackageEntry;

	class CResourcePackage;
	typedef std::shared_ptr<CResourcePackage> PResourcePackage;


	class CResourcePackageEntry {
	private:
		std::string m_sName;
		std::string m_sFileName;
		std::string m_sExtension;
		std::string m_sContentType;
		std::string m_sUUID;
		uint32_t m_nSize;
	public:
		CResourcePackageEntry(const std::string& sUUID, const std::string& sName, const std::string& sFileName, const std::string& sExtension, const std::string& sContentType, uint32_t nSize);

		std::string getName();
		std::string getFileName();
		std::string getExtension();
		std::string getContentType();
		std::string getUUID();
		uint32_t getSize();
	};

	class CResourcePackage {
	private:
		std::mutex m_Mutex;
		std::vector<uint8_t> m_ZIPBuffer;

		std::map<std::string, PResourcePackageEntry> m_UUIDMap;
		std::map<std::string, PResourcePackageEntry> m_NameMap;
		std::vector<PResourcePackageEntry> m_Entries;

		PResourcePackageZIP m_pResourcePackageZIP;
		std::string m_sPackageDebugName;
		
	protected:

	public:

		static PResourcePackage makeFromStream (AMCCommon::CImportStream * pStream, const std::string & sPackageDebugName, const std::string & sSchemaNamespace);
		static PResourcePackage makeFromStream (AMCCommon::PImportStream pStream, const std::string& sPackageDebugName, const std::string& sSchemaNamespace);
		static PResourcePackage makeEmpty (const std::string& sPackageDebugName);

		CResourcePackage(AMCCommon::CImportStream* pStream, const std::string& sPackageDebugName, const std::string & sSchemaNamespace);
		CResourcePackage(const std::string& sPackageDebugName);
		virtual ~CResourcePackage();


		uint64_t getEntryCount();
		PResourcePackageEntry getEntry(const uint64_t nIndex);
		PResourcePackageEntry findEntryByUUID(const std::string& sUUID, const bool bHasToExist);
		PResourcePackageEntry findEntryByName(const std::string& sName, const bool bHasToExist);

		// Resizes Buffer array to right size
		void readEntry(const std::string& sName, std::vector<uint8_t>& Buffer);
		std::string readEntryUTF8String (const std::string& sName);

		// Needs allocated memory passed
		void readEntryEx(const std::string& sName, uint8_t * pBuffer, const uint64_t nBufferSize);

	};

	
}


#endif //__AMC_RESOURCEPACKAGE

