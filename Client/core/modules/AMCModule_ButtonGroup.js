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


export default class AMCApplicationModule_ButtonGroup extends Common.AMCApplicationModule {

	constructor (page, moduleJSON)
	{
		Assert.ObjectValue (moduleJSON);
		super (page, moduleJSON.uuid, moduleJSON.type, moduleJSON.name || moduleJSON.uuid, moduleJSON.caption || "");
		this.registerClass ("amcModule_ButtonGroup");

		this.usesV2Frontend = true;

		this.buttons = Array.isArray(moduleJSON.buttons) ? moduleJSON.buttons : [];

		this.buttoncssstyle = "";
		this.cssstyle = "";

		let buttondistribution = moduleJSON.buttondistribution || "";

		if (buttondistribution === "rightaligned")
			this.cssstyle += "text-align: right;";
		if (buttondistribution === "leftaligned")
			this.cssstyle += "text-align: left;";
		if (buttondistribution === "centered")
			this.cssstyle += "text-align: centered;";
		if (buttondistribution === "equal") {
			let buttoncount = this.buttons.length;
			if (buttoncount > 0)
				this.buttoncssstyle += "min-width: calc(" + (100.0 / buttoncount) + "% - 10px);";
			this.cssstyle += "text-align: center;";
		}
	}


	updateFromJSON (updateJSON)
	{
		Assert.ObjectValue (updateJSON);
		if (updateJSON.buttons)
			this.buttons = updateJSON.buttons;
	}


	updateFromV2Attributes (attrs)
	{
		let v2Entry = this.page.application.getV2Entry(this.uuid);
		if (v2Entry && v2Entry.submodules && v2Entry.submodules.length > 0) {
			// ContentLeaf wraps the ButtonGroup item as submodules[0].
			// The individual buttons are one level deeper: submodules[0].submodules.
			let itemEntry = v2Entry.submodules[0];
			let buttonEntries = (itemEntry && itemEntry.submodules) ? itemEntry.submodules : [];
			this.buttons = buttonEntries.map(sub => {
				let a = sub.attributes || {};
				let formValues = [];
				if (a.eventformvalues && a.eventformvalues.trim() !== "") {
					formValues = a.eventformvalues.split(" ");
				}
				return {
					uuid:            sub.uuid,
					name:            sub.uuid,
					caption:         a.caption  || "",
					disabled:        (a.disabled === true || a.disabled === "1" || a.disabled === "true"),
					event:           a.event     || "",
					targetpage:      a.targetpage || "",
					icon:            a.icon      || "",
					eventformvalues: formValues,
				};
			});
		}
		if (attrs && attrs.caption !== undefined)
			this.caption = attrs.caption;
		if (attrs && attrs.visible !== undefined)
			this.visible = (attrs.visible === "1" || attrs.visible === true || attrs.visible === "true");
		return true;
	}

}
