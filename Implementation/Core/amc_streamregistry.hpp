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

Abstract: This is the class declaration of CStreamRegistry.
A thread-safe registry that maps stream UUIDs to CStreamInstance objects.
Supports multiple stream types (video, JSON events, etc.).

*/


#ifndef __AMC_STREAMREGISTRY
#define __AMC_STREAMREGISTRY

#include <memory>
#include <mutex>
#include <map>
#include <string>

#include "amc_streaminstance.hpp"
#include "amc_videostreaminstance.hpp"

namespace AMC {

	class CStreamRegistry {
	private:
		std::map<std::string, PStreamInstance> m_StreamMap;
		mutable std::mutex m_Mutex;

	public:
		CStreamRegistry();
		virtual ~CStreamRegistry();

		// Registers an existing stream instance.
		void registerStream(PStreamInstance pStreamInstance);

		// Creates a new video stream and registers it.
		PVideoStreamInstance createVideoStream(
			uint32_t nWidth,
			uint32_t nHeight,
			uint32_t nDesiredFrameDurationInMicroseconds,
			uint32_t nPauseToleranceInMicroseconds,
			uint32_t nFrameCacheDurationInMicroseconds
		);

		// Finds a stream by UUID. Returns nullptr if not found.
		PStreamInstance findStream(const std::string& sUUID) const;

		// Finds a video stream by UUID. Returns nullptr if not found or not a video stream.
		PVideoStreamInstance findVideoStream(const std::string& sUUID) const;

		// Removes a stream by UUID.
		void removeStream(const std::string& sUUID);

		// Returns true if a stream with the given UUID exists.
		bool hasStream(const std::string& sUUID) const;
	};

	typedef std::shared_ptr<CStreamRegistry> PStreamRegistry;

}


#endif //__AMC_STREAMREGISTRY
