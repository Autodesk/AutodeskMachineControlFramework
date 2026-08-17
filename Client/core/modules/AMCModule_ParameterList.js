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


export default class AMCApplicationModule_ParameterList extends Common.AMCApplicationModule {

	constructor (page, moduleJSON)
	{
		Assert.ObjectValue (moduleJSON);
		super (page, moduleJSON.uuid, moduleJSON.type, moduleJSON.name || moduleJSON.uuid, moduleJSON.caption || "");
		this.registerClass ("amcModule_ParameterList");

		this.usesV2Frontend = true;

		this.entries = [];

		// Per-column configuration (identifier -> {visible, width, sizeable}).
		// Defaults: all visible, flexible width, not resizable. Overridable via
		// <column> subnodes in config.xml.
		this.columns = this.defaultColumns ();

		this.headers = [];
		this.rebuildHeaders ();

		this.stateid = 1;
		this.loadingtext = "";
		this.entriesperpage = 25;
		this.editevent = "";

		this.updateFromJSON (moduleJSON);
	}


	// Coerces a server flag (boolean or "1"/"0"/"true"/"false") to a boolean.
	parseFlag (value, fallback)
	{
		if (value === undefined || value === null)
			return fallback;
		return (value === true || value === "1" || value === "true");
	}


	// The canonical column set in fixed order, all visible and flexible.
	defaultColumns ()
	{
		return [
			{ identifier: 'parameter', value: 'paramDescription', text: 'Parameter', visible: true, width: '', sizeable: false },
			{ identifier: 'value',     value: 'paramValue',       text: 'Value',     visible: true, width: '', sizeable: false },
			{ identifier: 'group',     value: 'paramGroup',       text: 'Group',     visible: true, width: '', sizeable: false },
			{ identifier: 'system',    value: 'paramSystem',      text: 'System',    visible: true, width: '', sizeable: false },
		];
	}


	// Replaces this.columns from a server-provided column array (if any). Unknown
	// or missing entries fall back to the canonical defaults, preserving order.
	applyColumns (incoming)
	{
		if (!Array.isArray (incoming) || incoming.length === 0)
			return;

		const byIdentifier = {};
		for (let column of incoming) {
			if (column && column.identifier)
				byIdentifier[column.identifier] = column;
		}

		this.columns = this.defaultColumns ().map ((base) => {
			const override = byIdentifier[base.identifier];
			if (!override)
				return base;
			return {
				identifier: base.identifier,
				value: base.value,
				text: (override.text !== undefined && override.text !== null && override.text !== '') ? override.text : base.text,
				visible: this.parseFlag (override.visible, true),
				width: (override.width !== undefined && override.width !== null) ? String (override.width) : '',
				sizeable: this.parseFlag (override.sizeable, false),
			};
		});
	}


	// Assembles the visible headers, carrying width and resize flags to clients.
	rebuildHeaders ()
	{
		const visibleColumns = this.columns
			.filter ((column) => column.visible)
			.map ((column) => ({ text: column.text, value: column.value, width: column.width, sizeable: column.sizeable }));

		while (this.headers.length > 0) this.headers.pop ();
		for (let column of visibleColumns) this.headers.push (column);
	}


	updateFromJSON (updateJSON)
	{
		Assert.ObjectValue (updateJSON);

		if (!updateJSON.entries)
			return;

		if (updateJSON.loadingtext)
			this.loadingtext = Assert.StringValue (updateJSON.loadingtext);
		if (updateJSON.entriesperpage)
			this.entriesperpage = Assert.IntegerValue (updateJSON.entriesperpage);
		if (updateJSON.editevent !== undefined)
			this.editevent = Assert.StringValue (updateJSON.editevent);

		this.applyColumns (updateJSON.columns);
		this.rebuildHeaders ();

		let oldEntryCount = this.entries.length;
		for (let index = 0; index < oldEntryCount; index++) {
			this.entries.pop();
		}
		for (let entry of updateJSON.entries) {
			this.entries.push(entry);
		}
	}


	updateFromV2Attributes (attrs)
	{
		if (!attrs)
			return;

		if (attrs.loadingtext !== undefined)
			this.loadingtext = attrs.loadingtext;
		if (attrs.entriesperpage !== undefined)
			this.entriesperpage = attrs.entriesperpage;
		if (attrs.editevent !== undefined)
			this.editevent = attrs.editevent;

		this.applyColumns (attrs.columns);
		this.rebuildHeaders ();
		if (attrs.caption !== undefined)
			this.caption = attrs.caption;
		if (attrs.visible !== undefined)
			this.visible = (attrs.visible === "1" || attrs.visible === true || attrs.visible === "true");

		const incoming = Array.isArray(attrs.entries) ? attrs.entries : [];
		while (this.entries.length > 0) this.entries.pop();
		for (let entry of incoming) this.entries.push(entry);
	}

}
