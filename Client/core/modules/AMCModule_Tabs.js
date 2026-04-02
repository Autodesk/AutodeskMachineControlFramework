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


export default class AMCApplicationModule_Tabs extends Common.AMCApplicationModule {
	
	constructor (page, moduleJSON) 
	{		
		Assert.ObjectValue (moduleJSON);				
		super (page, moduleJSON.uuid, moduleJSON.type, moduleJSON.name, moduleJSON.caption);		
		this.registerClass ("amcModule_Tabs");
		this.usesV2Frontend = true;
		
		this.tabs = [];

		Assert.ArrayValue (moduleJSON.tabs);
		for (let tabJSON of moduleJSON.tabs) {
			
			let tab = this.page.application.createModuleInstance (this.page, tabJSON);
			
			if (tab) {				
				this.tabs.push (tab);
				this.page.application.addModule (tab);
			} else {
				throw "Module type not found: " + tabJSON.type;
			}
		}
				
	}

	updateFromJSON(updateJSON) {
		Assert.ObjectValue(updateJSON);
	
		if (updateJSON.tabs && Array.isArray(updateJSON.tabs)) {
			
			for (let tabUpdate of updateJSON.tabs) {
		
				let tab = this.tabs.find(t => t.uuid === tabUpdate.uuid);
				if (tab && typeof tab.updateFromJSON === "function") {
					
					tab.updateFromJSON(tabUpdate);
				}
			}
		}
	}

	// Phase 3: v2 frontend – propagate "visible" and "caption" from v2
	// submodules to child tabs, mirroring the legacy updateFromJSON behaviour.
	// Modules that do not register a "visible" attribute on the backend
	// (e.g. Logs, GLScene) keep the base-class default of true.
	updateFromV2Attributes (attrs) {
		if (attrs) {
			if (attrs.caption !== undefined)
				this.caption = attrs.caption;
			if (attrs.visible !== undefined)
				this.visible = (attrs.visible === true || attrs.visible === "1" || attrs.visible === "true");
		}

		let v2Entry = this.page.application.getV2Entry(this.uuid);
		if (v2Entry && v2Entry.submodules) {
			for (let submod of v2Entry.submodules) {
				let tab = this.tabs.find(t => t.uuid === submod.uuid);
				if (tab && submod.attributes) {
					if (submod.attributes.visible !== undefined)
						tab.visible = (submod.attributes.visible === "1" || submod.attributes.visible === true || submod.attributes.visible === "true");
					if (submod.attributes.caption !== undefined)
						tab.caption = submod.attributes.caption;
				}
			}
		}
		return true;
	}

}
