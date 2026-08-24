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


export default class AMCApplicationModule_Paragraph extends Common.AMCApplicationModule {

	constructor (page, moduleJSON)
	{
		Assert.ObjectValue (moduleJSON);
		super (page, moduleJSON.uuid, moduleJSON.type, moduleJSON.name || moduleJSON.uuid, moduleJSON.caption || "");
		this.registerClass ("amcModule_Paragraph");

		this.usesV2Frontend = true;

		this.text = moduleJSON.text || "";
		this.variant = moduleJSON.variant || "body";
	}


	updateFromJSON (updateJSON)
	{
		Assert.ObjectValue (updateJSON);
		if (updateJSON.text !== undefined)
			this.text = updateJSON.text;
		if (updateJSON.variant !== undefined)
			this.variant = updateJSON.variant;
		if (updateJSON.visible !== undefined)
			this.visible = Assert.BoolValue (updateJSON.visible);
	}


	updateFromV2Attributes (attrs)
	{
		if (!attrs)
			return true;
		if (attrs.text !== undefined)
			this.text = attrs.text;
		if (attrs.variant !== undefined)
			this.variant = attrs.variant;
		if (attrs.caption !== undefined)
			this.caption = attrs.caption;
		if (attrs.visible !== undefined)
			this.visible = (attrs.visible === "1" || attrs.visible === true || attrs.visible === "true");
		return true;
	}

}
