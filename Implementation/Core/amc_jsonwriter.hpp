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


#ifndef __AMC_JSONWRITER
#define __AMC_JSONWRITER

#include "RapidJSON/document.h"
#include "RapidJSON/stringbuffer.h"
#include "RapidJSON/writer.h"

#include <string>

namespace AMC {


	class CJSONWriter;
	class CJSONWriterObject;
	class CJSONWriterArray;

	class CJSONWriterObject {
	private:
		rapidjson::Value m_Value;
		rapidjson::Document::AllocatorType& m_allocator;		
	public:

		friend class CJSONWriter;
		friend class CJSONWriterArray;

		CJSONWriterObject(CJSONWriter & writer);

		void addString(const std::string& sName, const std::string& sValue);

		void addInteger(const std::string& sName, int64_t nValue);

		void addBool(const std::string& sName, bool bValue);

		void addDouble(const std::string& sName, double dValue);

		void addObject(const std::string& sName, CJSONWriterObject & object);

		void addArray(const std::string& sName, CJSONWriterArray & array);

		bool isEmpty();

	};




	class CJSONWriterArray {
	private:
		rapidjson::Value m_Value;
		rapidjson::Document::AllocatorType& m_allocator;
	public:

		friend class CJSONWriter;
		friend class CJSONWriterObject;

		CJSONWriterArray(CJSONWriter& writer);

		void addString(const std::string& sValue);

		void addInteger(int64_t nValue);

		void addDouble(const std::string& sName, double dValue);

		void addObject(CJSONWriterObject& object);

		void addArray(CJSONWriterArray& array);

		bool isEmpty();

	};


	class CJSONWriter {
	private:
	
		rapidjson::Document m_document;		
		rapidjson::Document::AllocatorType& m_allocator;

	public:
	
		friend class CJSONWriterObject;
		friend class CJSONWriterArray;

		CJSONWriter();

		std::string saveToString();

		void addString(const std::string& sName, const std::string& sValue);

		void addInteger(const std::string& sName, int64_t nValue);

		void addDouble(const std::string& sName, double dValue);

		void addObject(const std::string& sName, CJSONWriterObject& object);

		void addArray(const std::string& sName, CJSONWriterArray& array);

	};

	
}


#endif //__AMC_JSONWRITER

