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

export default class AMCApplicationModule_Content extends Common.AMCApplicationModule {
	
	constructor (page, moduleJSON) 
	{		
		Assert.ObjectValue (moduleJSON);
		
		super (page, moduleJSON.uuid, moduleJSON.type, moduleJSON.name, moduleJSON.caption);		
		this.registerClass ("amcModule_Content");

		// Phase 2: content module can read from v2 frontend attributes
		this.usesV2Frontend = true;
		
		this.headline = Assert.StringValue (moduleJSON.headline);
		this.title = Assert.StringValue (moduleJSON.title);
		this.subtitle = Assert.StringValue (moduleJSON.subtitle);

		this.visible = Assert.BoolValue (moduleJSON.visible);

		this.cardstyle  = moduleJSON.cardstyle  || "none";
		this.spacing    = parseInt (moduleJSON.spacing)   || 0;
		this.elevation  = parseInt (moduleJSON.elevation) || 2;
		this.cardcolor  = moduleJSON.cardcolor  || "";

		this.modules = [];

		// The legacy backend serialises child modules under the "modules" key via
		// writeLegacyDefinitionToJSON; each child is a ContentLeaf shell:
		//   { type, uuid (ContentLeaf UUID), name, caption, visible, items: [innerJSON] }
		// The actual payload (entities, buttons, imageresource …) lives in items[0].
		// Both the "modules" and legacy "items" key are handled with the same
		// ContentLeaf-unwrapping logic so the leaf constructors receive clean data.
		const childList =
			(moduleJSON.modules && Array.isArray(moduleJSON.modules)) ? moduleJSON.modules :
			(moduleJSON.items   && Array.isArray(moduleJSON.items))   ? moduleJSON.items   :
			[];

		for (let childJSON of childList) {
			// Unwrap ContentLeaf shell when present: real payload is in items[0].
			let innerJSON = (childJSON.items && childJSON.items.length > 0) ? childJSON.items[0] : childJSON;

			let wrappedModuleJSON = Object.assign({}, innerJSON);
			wrappedModuleJSON.type    = childJSON.type    || innerJSON.type;
			wrappedModuleJSON.uuid    = childJSON.uuid;
			wrappedModuleJSON.name    = childJSON.name    || innerJSON.name    || childJSON.uuid || (this.name + "_" + (childJSON.type || ""));
			wrappedModuleJSON.caption = (childJSON.caption !== undefined) ? childJSON.caption : (innerJSON.caption || "");
			wrappedModuleJSON.visible = (childJSON.visible !== undefined) ? childJSON.visible : true;

			let childModule = this.page.application.createModuleInstance(this.page, wrappedModuleJSON);
			if (!childModule)
				throw "Child module type not found: " + (childJSON.type || innerJSON.type);
			this.modules.push(childModule);
			this.page.application.addModule(childModule);
		}
				
	}

	updateFromJSON(updateJSON) {

		Assert.ObjectValue(updateJSON);

		if(updateJSON.visible !== undefined)
			this.visible = Assert.BoolValue(updateJSON.visible);
	}

	updateFromV2Attributes (attrs) {
		if (attrs.headline !== undefined)
			this.headline = attrs.headline;
		if (attrs.title !== undefined)
			this.title = attrs.title;
		if (attrs.subtitle !== undefined)
			this.subtitle = attrs.subtitle;
		if (attrs.caption !== undefined)
			this.caption = attrs.caption;
		if (attrs.visible !== undefined)
			this.visible = (attrs.visible === "1" || attrs.visible === true || attrs.visible === "true");
		if (attrs.cardstyle !== undefined)
			this.cardstyle = attrs.cardstyle;
		if (attrs.cardcolor !== undefined)
			this.cardcolor = attrs.cardcolor;
		if (attrs.spacing !== undefined)
			this.spacing = parseInt (attrs.spacing) || 0;
		if (attrs.elevation !== undefined)
			this.elevation = parseInt (attrs.elevation) || 2;
		return true;
	}
	
}
