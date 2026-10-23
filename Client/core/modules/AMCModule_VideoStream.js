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


export default class AMCApplicationModule_VideoStream extends Common.AMCApplicationModule {

	constructor (page, moduleJSON)
	{
		Assert.ObjectValue (moduleJSON);
		super (page, moduleJSON.uuid, moduleJSON.type, moduleJSON.name || moduleJSON.uuid, moduleJSON.caption || "");
		this.registerClass ("amcModule_VideoStream");

		this.usesV2Frontend = true;

		this.streamresource = Common.nullUUID ();
		this.aspectratio = 1.0;
		this.maxwidth  = 0;
		this.maxheight = 0;

		this.updateFromJSON (moduleJSON);
	}


	updateFromJSON (updateJSON)
	{
		if (updateJSON.streamresource)
			this.streamresource = Assert.UUIDValue (updateJSON.streamresource);
		if (updateJSON.aspectratio !== undefined)
			this.aspectratio = Assert.NumberValue (updateJSON.aspectratio);
		if (updateJSON.maxwidth !== undefined)
			this.maxwidth = Assert.NumberValue (updateJSON.maxwidth);
		if (updateJSON.maxheight !== undefined)
			this.maxheight = Assert.NumberValue (updateJSON.maxheight);
	}


	updateFromV2Attributes (attrs)
	{
		if (!attrs)
			return true;
		if (attrs.streamresource !== undefined)
			this.streamresource = attrs.streamresource;
		if (attrs.aspectratio !== undefined)
			this.aspectratio = parseFloat(attrs.aspectratio);
		if (attrs.maxwidth !== undefined)
			this.maxwidth = parseFloat(attrs.maxwidth);
		if (attrs.maxheight !== undefined)
			this.maxheight = parseFloat(attrs.maxheight);
		if (attrs.caption !== undefined)
			this.caption = attrs.caption;
		if (attrs.visible !== undefined)
			this.visible = (attrs.visible === "1" || attrs.visible === true || attrs.visible === "true");
		return true;
	}

}
