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
import AMCListDeltaSync, { compareEntriesBy } from "../common/AMCListDeltaSync.js";


export default class AMCApplicationModule_BuildList extends Common.AMCApplicationModule {

	constructor (page, moduleJSON)
	{
		Assert.ObjectValue (moduleJSON);
		super (page, moduleJSON.uuid, moduleJSON.type, moduleJSON.name || moduleJSON.uuid, moduleJSON.caption || "");
		this.registerClass ("amcModule_BuildList");

		this.usesV2Frontend = true;
		this.entries = [];

		this.headers = [];
		if (moduleJSON.headers && moduleJSON.headers.length > 0) {
			for (let header of moduleJSON.headers) {
				this.headers.push({
					text:     header.text,
					value:    header.value,
					sortable: header.sortable,
					width:    header.width,
				});
			}
		}

		this.entrybuttons = [];
		if (moduleJSON.entrybuttons) {
			for (let entrybutton of moduleJSON.entrybuttons) {
				this.entrybuttons.push({
					uuid:        entrybutton.uuid,
					caption:     entrybutton.caption,
					color:       entrybutton.color,
					cursor:      entrybutton.cursor,
					selectevent: entrybutton.selectevent,
				});
			}
		}

		this.loadingtext            = "";
		this.selectevent            = "";
		this.selectionvalueuuid     = Common.nullUUID ();
		this.buttonvalueuuid        = Common.nullUUID ();
		this.thumbnailaspectratio   = 1.8;
		this.thumbnailheight        = "150pt";
		this.thumbnailwidth         = "";
		this.entriesperpage         = 25;
		this.loaded                 = false;
		this.defaultThumbnailUUID   = Common.nullUUID ();
		this.listSync               = new AMCListDeltaSync (this.entries, "buildUUID", compareEntriesBy ("buildTimestamp", true),
			(sinceHeadID) => this.requestBuilds (sinceHeadID));

		this.updateFromJSON (moduleJSON);
	}


	updateFromJSON (updateJSON)
	{
		Assert.ObjectValue (updateJSON);

		if (updateJSON.loadingtext)
			this.loadingtext = Assert.StringValue (updateJSON.loadingtext);
		if (updateJSON.selectevent)
			this.selectevent = Assert.IdentifierString (updateJSON.selectevent);
		if (updateJSON.selectionvalueuuid)
			this.selectionvalueuuid = Assert.IdentifierString (updateJSON.selectionvalueuuid);
		if (updateJSON.buttonvalueuuid)
			this.buttonvalueuuid = Assert.IdentifierString (updateJSON.buttonvalueuuid);
		if (updateJSON.entriesperpage)
			this.entriesperpage = Assert.IntegerValue (updateJSON.entriesperpage);

		if (updateJSON.entries) {
			let oldEntryCount = this.entries.length;
			for (let index = 0; index < oldEntryCount; index++) {
				this.entries.pop();
			}
			for (let entry of updateJSON.entries) {
				this.entries.push(entry);
			}
		}
	}


	updateFromV2Attributes (attrs)
	{
		if (!attrs)
			return true;

		if (attrs.loadingtext !== undefined)
			this.loadingtext = attrs.loadingtext;
		if (attrs.selectevent !== undefined)
			this.selectevent = attrs.selectevent;
		if (attrs.entriesperpage !== undefined)
			this.entriesperpage = parseInt(attrs.entriesperpage) || this.entriesperpage;
		if (attrs.caption !== undefined)
			this.caption = attrs.caption;
		if (attrs.visible !== undefined)
			this.visible = (attrs.visible === "1" || attrs.visible === true || attrs.visible === "true");
		if (attrs.defaultthumbnail !== undefined)
			this.defaultThumbnailUUID = attrs.defaultthumbnail;
		if (attrs.selectionvalueuuid !== undefined)
			this.selectionvalueuuid = attrs.selectionvalueuuid;
		if (attrs.buttonvalueuuid !== undefined)
			this.buttonvalueuuid = attrs.buttonvalueuuid;

		if (attrs.buildlistheadid !== undefined)
			this.listSync.notifyHeadID (parseInt(attrs.buildlistheadid));

		return true;
	}


	requestBuilds (sinceHeadID)
	{
		let url = "/build?status=validated";
		if (sinceHeadID !== null)
			url += "&since=" + sinceHeadID;

		return this.page.application.axiosGetRequest(url)
		.then(resultJSON => {
			this.loaded = true;

			let data = resultJSON.data;
			if (!data || !Array.isArray(data.buildjobs))
				throw "invalid build list response";

			let rows = [];
			for (let job of data.buildjobs) {
				rows.push({
					buildUUID:           job.uuid           || "",
					buildName:           job.name           || "",
					buildLayers:         job.layercount     || 0,
					buildTimestamp:      job.timestamp      || "",
					buildUser:           job.user           || "",
					buildExecutionCount: job.executioncount || 0,
					buildThumbnail:      job.thumbnail      || this.defaultThumbnailUUID || "00000000-0000-0000-0000-000000000000",
				});
			}

			let removedKeys = Array.isArray(data.removed) ? data.removed : [];

			return { headID: parseInt(data.buildlistheadid), delta: (data.delta === true), rows: rows, removedKeys: removedKeys };
		}, error => {
			this.loaded = true;
			throw error;
		});
	}

}
