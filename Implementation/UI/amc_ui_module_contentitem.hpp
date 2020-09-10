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


#ifndef __AMC_UI_MODULE_CONTENTITEM
#define __AMC_UI_MODULE_CONTENTITEM

#include "header_protection.hpp"

#ifndef __AMCIMPL_UI_MODULE
#error this header is protected and should only be included in the corresponding implementation CPP files.
#endif

#include "Core/amc_jsonwriter.hpp"

namespace AMC {

	amcDeclareDependingClass(CUIModule, PUIModule);
	amcDeclareDependingClass(CUIModule_ContentItem, PUIModule_ContentItem);
	amcDeclareDependingClass(CUIModule_ContentParagraph, PUIModule_ContentParagraph);
	amcDeclareDependingClass(CUIModule_ContentImage, PUIModule_ContentImage);
	amcDeclareDependingClass(CUIModule_ContentUpload, PUIModule_ContentUpload);

	class CUIModule_ContentItem {
	protected:		

		std::string m_sUUID;

	public:

		CUIModule_ContentItem(const std::string & sUUID);
		
		virtual ~CUIModule_ContentItem();

		std::string getUUID ();

		virtual void addToJSON (CJSONWriter & writer, CJSONWriterObject & object) = 0;
		
	};

	class CUIModule_ContentParagraph : public CUIModule_ContentItem {
	protected:		

		std::string m_sText;

	public:

		CUIModule_ContentParagraph(const std::string & sText);
		
		virtual ~CUIModule_ContentParagraph();

		std::string getText ();

		void addToJSON(CJSONWriter& writer, CJSONWriterObject& object) override;

	};


	class CUIModule_ContentImage : public CUIModule_ContentItem {
	protected:		

	public:

		CUIModule_ContentImage(const std::string & sUUID);
		
		virtual ~CUIModule_ContentImage();

		void addToJSON(CJSONWriter& writer, CJSONWriterObject& object) override;

	};

	class CUIModule_ContentUpload : public CUIModule_ContentItem {
	protected:
		std::string m_sUploadClass;
		std::string m_sUploadCaption;

	public:

		CUIModule_ContentUpload(const std::string& sUploadClass, const std::string& sUploadCaption);

		virtual ~CUIModule_ContentUpload();

		void addToJSON(CJSONWriter& writer, CJSONWriterObject& object) override;

	};
	

	class CUIModule_ContentParameterList : public CUIModule_ContentItem {
	protected:

	public:

		CUIModule_ContentParameterList();

		virtual ~CUIModule_ContentParameterList();

		void addToJSON(CJSONWriter& writer, CJSONWriterObject& object) override;

	};
}


#endif //__AMC_UI_MODULE_CONTENTITEM

