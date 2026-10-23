/*++

Copyright (C) 2022 Autodesk Inc.

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

#ifndef __AMCTEST_UNITTEST_IO
#define __AMCTEST_UNITTEST_IO


#include <cstdint>
#include <string>
#include <memory>



namespace AMCUnitTest {
	
	class CUnitTestIO {		
		public:
			virtual void logMessageString (const std::string & sMessage) = 0;		
			virtual void writeTestResult(const std::string& sGroupName, const std::string& sTestName, uint32_t nTestIndex, uint32_t nTestCount, bool bTestSuccess) = 0;
			virtual void writeTestInfo(const std::string& sMessage) = 0;
			virtual void writeTestWarning(const std::string& sMessage) = 0;
			virtual void writeTestError(const std::string& sMessage) = 0;
	};


	class CUnitTestStdIO : public CUnitTestIO {
	public:

		CUnitTestStdIO();
		virtual ~CUnitTestStdIO();

		void writeFixedLengthStringLeftAligned(const std::string& sString, size_t nLength);
		void writeFixedLengthStringRightAligned(const std::string& sString, size_t nLength);

		virtual void logMessageString(const std::string& sMessage) override;
		virtual void writeTestResult(const std::string& sGroupName, const std::string& sTestName, uint32_t nTestIndex, uint32_t nTestCount, bool bTestSuccess) override;
		virtual void writeTestInfo (const std::string& sMessage) override;
		virtual void writeTestWarning (const std::string& sMessage) override;
		virtual void writeTestError (const std::string& sMessage) override;
	};
	
	typedef std::shared_ptr<CUnitTestIO> PUnitTestIO;
	
	
}

#endif //__AMCTEST_UNITTEST_IO