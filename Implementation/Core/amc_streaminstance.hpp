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

Abstract: This is the class declaration of CStreamInstance.
An abstract base class for all stream types (video, JSON events, etc.).
Provides a common identity (UUID) and activity interface for the stream registry.

*/


#ifndef __AMC_STREAMINSTANCE
#define __AMC_STREAMINSTANCE

#include <memory>
#include <string>
#include <cstdint>

namespace AMC {

	enum class eStreamType : uint32_t
	{
		Unknown = 0,
		VideoStream = 1,
		JSONEventStream = 2
	};

	class CStreamInstance {
	private:
		std::string m_sUUID;

	public:
		CStreamInstance(const std::string& sUUID);

		virtual ~CStreamInstance();

		std::string getUUID() const;

		// Returns the type of the stream.
		virtual eStreamType getStreamType() const = 0;

		// Returns true if the stream is currently active (receiving data).
		virtual bool isActive() const = 0;
	};

	typedef std::shared_ptr<CStreamInstance> PStreamInstance;

}


#endif //__AMC_STREAMINSTANCE
