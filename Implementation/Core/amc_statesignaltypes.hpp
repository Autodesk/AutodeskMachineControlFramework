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


#ifndef __AMC_STATESIGNALTYPES
#define __AMC_STATESIGNALTYPES

#include <memory>
#include <string>

#define AMC_SIGNAL_DEFAULT_WAITFOR_SLEEP_MS 1

#define AMC_SIGNAL_MINQUEUESIZE 1
#define AMC_SIGNAL_MAXQUEUESIZE 1024

#define AMC_SIGNAL_MINREACTIONTIMEINMS 1
#define AMC_SIGNAL_MAXREACTIONTIMEINMS 3600000 // 1 hour reaction timeout is maximum

#define AMC_SIGNAL_MINARCHIVETIMEINMS 1
#define AMC_SIGNAL_MAXARCHIVETIMEINMS (3600000 * 24 * 14) // 14 days to stay within Uint32 range

namespace AMC {

	enum class eAMCSignalPhase : int32_t {
		Invalid = 0, 
		InPreparation = 10,
		InQueue = 20, 
		InProcess = 30, 
		Handled = 40, 
		Failed = 50, 
		TimedOut = 60, 
		Cleared = 70, 
		Archived = 80 
	};

}


#endif //__AMC_STATESIGNALTYPES

