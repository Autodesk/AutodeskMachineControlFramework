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


#ifndef __AMC_STATEJOURNAL
#define __AMC_STATEJOURNAL

#include <memory>
#include <string>

#include "amc_statejournalstream.hpp"

namespace AMC {

	enum class eStateJournalVariableType {
		vtUnknown = 0,
		vtIntegerParameter = 1,
		vtStringParameter = 2,
		vtDoubleParameter = 3,
		vtBoolParameter = 4,
	};


	enum class eStateJournalMode {
		sjmInitialising = 1,
		sjmRecording = 2,
		sjmFinished = 3
	};


	class CStateJournal;
	typedef std::shared_ptr<CStateJournal> PStateJournal;

	class CStateJournalImpl;
	typedef std::shared_ptr<CStateJournalImpl> PStateJournalImpl;


	class CStateJournalVariable
	{
		private:
			uint32_t m_nVariableID;
			PStateJournalImpl m_pStateJournalImpl;

		public:

			CStateJournalVariable();
			CStateJournalVariable(PStateJournalImpl pImpl, uint32_t nVariableID);

			void updateValue(const bool bValue);
			void updateValue(const int64_t nValue);
			void updateValue(const std::string& sValue);
			void updateValue(const double dValue);
	};


	
	class CStateJournal {		
	protected:
		PStateJournalImpl m_pImpl;

	public:

		CStateJournal(PStateJournalStream pStream);
		virtual ~CStateJournal();

		void startRecording ();
		void finishRecording ();

		CStateJournalVariable registerBooleanValue (const std::string & sName, const bool bInitialValue);
		CStateJournalVariable registerIntegerValue (const std::string& sName, const int64_t bInitialValue);
		CStateJournalVariable registerStringValue (const std::string& sName, const std::string & bInitialValue);
		CStateJournalVariable registerDoubleValue (const std::string& sName, const double dInitialValue, double dUnits);

	};

	
}


#endif //__AMC_STATEJOURNAL

