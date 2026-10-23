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

Abstract: This is the class declaration of CVideoStreamInstance.
A thread-safe in-memory video stream that accepts RGB24 frames,
encodes them to JPEG, and provides them for MJPEG streaming.
Inherits from CStreamInstance to participate in the generic stream registry.

*/


#ifndef __AMC_VIDEOSTREAMINSTANCE
#define __AMC_VIDEOSTREAMINSTANCE

#include "amc_streaminstance.hpp"

#include <memory>
#include <mutex>
#include <vector>
#include <string>
#include <chrono>
#include <cstdint>

namespace AMC {

	class CVideoStreamInstance : public CStreamInstance {
	private:
		uint32_t m_nWidth;
		uint32_t m_nHeight;

		// Timing parameters (in microseconds)
		uint32_t m_nDesiredFrameDurationInMicroseconds;
		uint32_t m_nPauseToleranceInMicroseconds;
		uint32_t m_nFrameCacheDurationInMicroseconds;

		// Frame counters
		uint32_t m_nFrameCount;
		uint32_t m_nDroppedFrameCount;

		// Timing state
		std::chrono::steady_clock::time_point m_StreamStartTime;
		std::chrono::steady_clock::time_point m_LastSourceTime;

		// The latest JPEG-encoded frame (double-buffered for lock-free reads)
		std::vector<uint8_t> m_CurrentJPEGFrame;

		// The latest raw RGB24 frame data (for GetLastSourceFrame)
		std::vector<uint8_t> m_CurrentRGB24Frame;

		// Frame version counter for detecting new frames
		uint64_t m_nCurrentFrameVersion;

		// Thread safety
		mutable std::mutex m_Mutex;

	public:
		CVideoStreamInstance(
			const std::string& sUUID,
			uint32_t nWidth,
			uint32_t nHeight,
			uint32_t nDesiredFrameDurationInMicroseconds,
			uint32_t nPauseToleranceInMicroseconds,
			uint32_t nFrameCacheDurationInMicroseconds
		);

		virtual ~CVideoStreamInstance();

		// CStreamInstance overrides
		eStreamType getStreamType() const override;
		bool isActive() const override;

		uint32_t getWidth() const;
		uint32_t getHeight() const;

		uint32_t getDesiredFrameDurationInMicroseconds() const;
		double getDesiredFramerate() const;
		uint32_t getPauseToleranceInMicroseconds() const;
		uint32_t getFrameCacheDurationInMicroseconds() const;

		uint32_t getFrameCount() const;
		uint32_t getDroppedFrameCount() const;

		uint64_t getStreamStartTimeMicroseconds1970() const;
		uint64_t getLastSourceTimeMicroseconds() const;

		// Push a new RGB24 frame. Encodes to JPEG internally.
		// Fails if image dimensions don't match or format is not RGB24.
		void pushFrame(const uint8_t* pRGB24Data, uint32_t nDataSize);

		// Returns the latest JPEG frame data and the frame version.
		// Returns true if a new frame is available since lastKnownVersion.
		bool getLatestJPEGFrame(std::vector<uint8_t>& jpegBuffer, uint64_t lastKnownVersion, uint64_t& newVersion) const;

		// Returns the latest JPEG frame unconditionally.
		bool getLatestJPEGFrame(std::vector<uint8_t>& jpegBuffer) const;

		// Returns a copy of the current RGB24 frame data.
		std::vector<uint8_t> getLastRGB24Frame() const;

		// Returns current frame version (for polling)
		uint64_t getCurrentFrameVersion() const;
	};

	typedef std::shared_ptr<CVideoStreamInstance> PVideoStreamInstance;

}


#endif //__AMC_VIDEOSTREAMINSTANCE
