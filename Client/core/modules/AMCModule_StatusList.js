/*++

Copyright (C) 2026 Autodesk Inc.

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


export default class AMCApplicationModule_StatusList extends Common.AMCApplicationModule {

	constructor (page, moduleJSON)
	{
		Assert.ObjectValue (moduleJSON);
		super (page, moduleJSON.uuid, moduleJSON.type, moduleJSON.name || moduleJSON.uuid, moduleJSON.caption || "");
		this.registerClass ("amcModule_StatusList");

		this.usesV2Frontend = true;

		this.title = moduleJSON.title || "";
		this.loadingtext = moduleJSON.loadingtext || "";
		this.rows = [];

		this.updateFromJSON (moduleJSON);
	}


	updateFromJSON (updateJSON)
	{
		Assert.ObjectValue (updateJSON);

		if (updateJSON.title !== undefined)
			this.title = updateJSON.title;
		if (updateJSON.loadingtext !== undefined)
			this.loadingtext = updateJSON.loadingtext;
		if (updateJSON.visible !== undefined)
			this.visible = Assert.BoolValue (updateJSON.visible);

		if (!updateJSON.rows)
			return;

		while (this.rows.length > 0) this.rows.pop();
		for (let row of updateJSON.rows) this.rows.push(row);
	}


	updateFromV2Attributes (attrs)
	{
		if (!attrs)
			return true;

		if (attrs.title !== undefined)
			this.title = attrs.title;
		if (attrs.loadingtext !== undefined)
			this.loadingtext = attrs.loadingtext;
		if (attrs.caption !== undefined)
			this.caption = attrs.caption;
		if (attrs.visible !== undefined)
			this.visible = (attrs.visible === "1" || attrs.visible === true || attrs.visible === "true");

		const incoming = Array.isArray(attrs.rows) ? attrs.rows : [];
		while (this.rows.length > 0) this.rows.pop();
		for (let row of incoming) this.rows.push(row);

		return true;
	}

}
