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

import * as Assert from "./AMCAsserts.js";
import * as Common from "./AMCCommon.js"

export default class AMCUploadState extends Common.AMCObject {
	
	constructor (itemUUID)
	{
		super ();		
		this.registerClass ("amcUploadState");
		
		Assert.UUIDValue (itemUUID);
		
		this.chosenFile = null;
		this.uploadID = 0;
		this.IDCounter = 1;
		this.messages = [];
		this.itemUUID = itemUUID;		
		this.cancelMessage = "Upload canceled";
		this.finishMessage = "Upload finished";
		this.startMessage = "Starting upload";
		this.progressMessage = "Uploading...";
		this.waitMessage = "Waiting for upload to finish..";
		this.buildMessage = "Processing build..";
	}
	
	generateUploadID ()
	{
		this.uploadID = this.IDCounter;
		this.IDCounter = this.IDCounter + 1;
		return this.uploadID;
	}
	
	cancelUpload ()
	{
		if (!(this.uploadID === 0)) {
			this.uploadID = 0;
		}
		
		this.chosenFile = null;
		this.setMessage (this.cancelMessage);
	}
	
	getChosenFile ()
	{
		if (!this.chosenFile)
			throw "no file selected"
		
		Assert.ObjectValue (this.chosenFile);
		
		return this.chosenFile;		
	}
	
	
	finishUpload ()
	{
		this.uploadID = 0;
		this.chosenFile = null;
		this.setMessage (this.finishMessage);
	}
	
	setMessage (messageString)
	{
		this.messages = [ Assert.StringValue (messageString) ];
		
	}
	
	getUploadID ()
	{
		return Assert.IntegerValue (this.uploadID);
	}
	
	getItemUUID ()
	{
		return Assert.UUIDValue (this.itemUUID);
	}
	
}

