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


import * as Assert from "./AMCAsserts.js";

export function nullToken ()
{
	return "0000000000000000000000000000000000000000000000000000000000000000";
}

export function nullUUID ()
{
	return "00000000-0000-0000-0000-000000000000";
}


export class AMCObject {
		
	constructor ()
	{
		this.classSet = new Set ();
		this.classSet.add ("amcObject");	
	}
	
	registerClass (className)
	{
		this.classSet.add (Assert.IdentifierString (className));
	}
	
	isClass (className)
	{
		return this.classSet.has (className);
	}
		
}



export class AMCApplicationItem extends AMCObject {
	
	constructor (moduleInstance, uuid, itemtype) 
	{
		Assert.ObjectInstance (moduleInstance, "amcModule");
		Assert.UUIDValue (uuid);
		Assert.IdentifierString (itemtype);
		
		super ();			
		this.registerClass ("amcItem");
			
		this.moduleInstance = moduleInstance;
		this.type = itemtype;
		this.uuid = uuid;
		this.refresh = false;
	}
	
	setRefreshFlag ()
	{
		this.refresh = true;
	}
	
	updateFromJSON (updateJSON)
	{
		Assert.ObjectValue (updateJSON);
	}

}




export class AMCApplicationModule extends AMCObject {
	
	constructor (page, uuid, moduletype, name, caption) 
	{		
	
		super ();
		this.registerClass ("amcModule");

		this.page = Assert.ObjectInstance (page, "amcPage");
		this.uuid = Assert.UUIDValue (uuid);
		this.type = Assert.IdentifierString (moduletype);
		this.name = Assert.IdentifierString (name);
		this.caption = Assert.StringValue (caption);
		this.onDataHasChanged = null;
		this.stateid = 0;
	}
	
	callDataHasChanged ()
	{
		if (this.onDataHasChanged) {
			this.onDataHasChanged (this);
		}
	}

}