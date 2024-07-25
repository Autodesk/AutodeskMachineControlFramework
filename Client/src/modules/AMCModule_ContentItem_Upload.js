/*++

Copyright (C) 2021 Autodesk Inc.

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


import * as Assert from "../common/AMCAsserts.js";
import * as Common from "../common/AMCCommon.js"


import AMCUploadState from "../common/AMCImplementation_UploadState.js"

export default class AMCApplicationItem_Content_Upload extends Common.AMCApplicationItem {
	
	constructor (moduleInstance, itemJSON) 
	{
		Assert.ObjectValue (itemJSON);		
		super (moduleInstance, itemJSON.uuid, itemJSON.type);		
		this.registerClass ("amcItem_Upload");
		
		this.uploadclass = Assert.IdentifierString (itemJSON.uploadclass);
		this.uploadsuccessevent = Assert.OptionalIdentifierString (itemJSON.uploadsuccessevent);
		this.uploadfailureevent = Assert.OptionalIdentifierString (itemJSON.uploadfailureevent);
		this.uploadcaption = Assert.StringValue (itemJSON.uploadcaption);
		this.uploadfilename = "";
		this.uploadisinitial = 1;
		this.uploadissaving = 0;
		this.acceptedtypes = "";
				
		if (this.uploadclass == "build") {
			this.acceptedtypes = ".3mf";
		}

		if (this.uploadclass == "image") {
			this.acceptedtypes = ".png,.jpg";
		}
				
		this.state = new AMCUploadState (this.uuid);
		
		this.setRefreshFlag ();
		
	}
		
}