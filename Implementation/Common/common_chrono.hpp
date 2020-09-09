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


#ifndef __AMC_CHRONO
#define __AMC_CHRONO

#include <memory>
#include <string>
#include <vector>

namespace AMCCommon {

	class CChrono_Impl;

	class CChrono {
		private:
			std::unique_ptr<CChrono_Impl> m_pChronoImpl;
		
		public:
		
			CChrono ();
			~CChrono ();
			
			// Returns instance creation time in ISO8601 time format
			std::string getStartTimeISO8601TimeUTC ();			
			
			// Returns instance creation time in a filename compatible format
			std::string getStartTimeFileName();

			// Returns Duration in seconds since instance creation
			// Subsequent calls of this method will never decrease.
			uint64_t getExistenceTimeInSeconds ();

			// Returns Duration in milliseconds since instance creation
			// Subsequent calls of this method will never decrease.
			uint64_t getExistenceTimeInMilliseconds ();

			// Returns Duration in microseconds since instance creation
			// Subsequent calls of this method will never decrease.
			uint64_t getExistenceTimeInMicroseconds ();

			// Returns Duration in seconds since the time passed
			// Passed timestamp MUST have been created with getExistenceTimeInSeconds
			uint64_t getDurationTimeInSeconds(const uint64_t nStartTimeInSeconds);

			// Returns Duration in seconds since the time passed
			// Passed timestamp MUST have been created with getExistenceTimeInSeconds
			uint64_t getDurationTimeInMilliseconds(const uint64_t nStartTimeInMilliseconds);

			// Returns Duration in seconds since the time passed
			// Passed timestamp MUST have been created with getExistenceTimeInSeconds
			uint64_t getDurationTimeInMicroseconds(const uint64_t nStartTimeInMicroseconds);


			// Sleeps for some seconds
			void sleepSeconds(const uint64_t seconds);

			// Sleeps for some milliseconds
			void sleepMilliseconds(const uint64_t milliseconds);

			// Sleeps for some microseconds
			void sleepMicroseconds(const uint64_t microseconds);
	};

	typedef std::shared_ptr<CChrono> PChrono;
	
}


#endif //__AMC_CHRONO

