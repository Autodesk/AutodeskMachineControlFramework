/*++

Copyright (C) 2019 3MF Consortium

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

--*/

#ifndef __COMMON_PORTABLEZIPWRITERTYPES
#define __COMMON_PORTABLEZIPWRITERTYPES


#define ZIPFILEHEADERSIGNATURE 0x04034b50
#define ZIPFILECENTRALHEADERSIGNATURE 0x02014b50
#define ZIPFILEENDOFCENTRALDIRSIGNATURE 0x06054b50
#define ZIPFILEDATADESCRIPTORSIGNATURE 0x08074b50
#define ZIPFILEDESCRIPTOROFFSET 14
#define ZIPFILEVERSIONNEEDED 0x0A
#define ZIPFILEVERSIONNEEDEDZIP64 0x2D
#define ZIP64FILEENDOFCENTRALDIRRECORDSIGNATURE 0x06064b50
#define ZIP64FILEENDOFCENTRALDIRLOCATORSIGNATURE 0x07064b50

#define ZIPFILEEXTERNALFILEATTRIBUTES 0x80 // faArchive
#define ZIPFILEMAXENTRIES 2147483647 // 2^31 - 1

#define ZIPFILEDATAZIP64EXTENDEDINFORMATIONEXTRAFIELD 0x0001

#define ZIPFILECOMPRESSION_UNCOMPRESSED 0
#define ZIPFILECOMPRESSION_DEFLATED 8
#define ZIPFILEMAXFILENAMELENGTH 32000

#define ZIPFILEMAXIMUMSIZENON64 0xFFFFFFFF

namespace AMCCommon {

#pragma pack (1)
	typedef struct ZIPLOCALFILEHEADER {
		uint32_t m_nSignature;
		uint16_t m_nVersion;
		uint16_t m_nGeneralPurposeFlags;
		uint16_t m_nCompressionMethod;
		uint16_t m_nLastModTime;
		uint16_t m_nLastModDate;
		uint32_t m_nCRC32;
		uint32_t m_nCompressedSize;
		uint32_t m_nUnCompressedSize;
		uint16_t m_nFileNameLength;
		uint16_t m_nExtraFieldLength;
	} ZIPLOCALFILEHEADER;

	typedef struct ZIPLOCALFILEDESCRIPTOR {
		uint32_t m_nCRC32;
		uint32_t m_nCompressedSize;
		uint32_t m_nUnCompressedSize;
	} ZIPLOCALFILEDESCRIPTOR;

	typedef struct ZIP64EXTRAINFORMATIONFIELD {
		uint16_t m_nTag;
		uint16_t m_nFieldSize;
		uint64_t m_nUncompressedSize;
		uint64_t m_nCompressedSize;
	} ZIP64EXTRAINFORMATIONFIELD;

	typedef struct ZIPCENTRALDIRECTORYFILEHEADER {
		uint32_t m_nSignature;
		uint16_t m_nVersionMade;
		uint16_t m_nVersionNeeded;
		uint16_t m_nGeneralPurposeFlags;
		uint16_t m_nCompressionMethod;
		uint16_t m_nLastModTime;
		uint16_t m_nLastModDate;
		uint32_t m_nCRC32;
		uint32_t m_nCompressedSize;
		uint32_t m_nUnCompressedSize;
		uint16_t m_nFileNameLength;
		uint16_t m_nExtraFieldLength;
		uint16_t m_nFileCommentLength;
		uint16_t m_nDiskNumberStart;
		uint16_t m_nInternalFileAttributes;
		uint32_t m_nExternalFileAttributes;
		uint32_t m_nRelativeOffsetOfLocalHeader;
	} ZIPCENTRALDIRECTORYFILEHEADER;

	typedef struct ZIPENDOFCENTRALDIRHEADER {
		uint32_t m_nSignature;
		uint16_t m_nNumberOfDisk;
		uint16_t m_nRelativeNumberOfDisk;
		uint16_t m_nNumberOfEntriesOfDisk;
		uint16_t m_nNumberOfEntriesOfDirectory;
		uint32_t m_nSizeOfCentralDirectory;
		uint32_t m_nOffsetOfCentralDirectory;
		uint16_t m_nCommentLength;
	} ZIPENDOFCENTRALDIRHEADER;

	typedef struct ZIP64ENDOFCENTRALDIRHEADER {
		uint32_t m_nSignature;
		uint64_t m_nEndOfCentralDirHeaderRecord;
		uint16_t m_nVersionMade;
		uint16_t m_nVersionNeeded;
		uint32_t m_nNumberOfDisk;
		uint32_t m_nNumberOfDiskOfCentralDirectory;
		uint64_t m_nTotalNumberOfEntriesOnThisDisk;
		uint64_t m_nTotalNumberOfEntriesInCentralDirectory;
		uint64_t m_nSizeOfCentralDirectory;
		uint64_t m_nOffsetOfCentralDirectoryWithRespectToDisk;
	} ZIP64ENDOFCENTRALDIRHEADER;

	typedef struct ZIP64ENDOFCENTRALDIRLOCATOR {
		uint32_t m_nSignature;
		uint32_t m_nNumberOfDiskWithStartOfZIP64EOCentralDir;
		uint64_t m_nRelativeOffset;
		uint32_t m_nTotalNumberOfDisk;
	} ZIP64ENDOFCENTRALDIRLOCATOR;

#pragma pack()

}

#endif //__COMMON_PORTABLEZIPWRITERTYPES	
