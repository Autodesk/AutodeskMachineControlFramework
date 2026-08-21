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


export default class AMCApplicationModule_ConfigurationList extends Common.AMCApplicationModule {

	constructor (page, moduleJSON)
	{
		Assert.ObjectValue (moduleJSON);
		super (page, moduleJSON.uuid, moduleJSON.type, moduleJSON.name || moduleJSON.uuid, moduleJSON.caption || "");
		this.registerClass ("amcModule_ConfigurationList");

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
					align:    header.align,
				});
			}
		} else {
			this.headers = [
				{ text: "Active",      value: "configurationActive",    sortable: true,  width: "5vw",  align: "center" },
				{ text: "Version",     value: "configurationVersion",   sortable: true,  width: "5vw",  align: "center" },
				{ text: "Upload time", value: "configurationTimestamp", sortable: true,  width: "10vw", align: "center" },
				{ text: "User",        value: "userName",               sortable: true,  width: "5vw",  align: "center" },
				{ text: "Actions",     value: "configurationActions",   sortable: true,  width: "20vw", align: "center" },
			];
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

		this.loadingtext                  = "";
		this.selectevent                  = "";
		this.selectionvalueuuid           = Common.nullUUID ();
		this.buttonvalueuuid              = Common.nullUUID ();
		this.thumbnailaspectratio         = 1.8;
		this.thumbnailheight              = "150pt";
		this.thumbnailwidth               = "";
		this.entriesperpage               = 5;
		this.configurationSchema          = "com.scanlab.ocmsmc";
		this.lastKnownHeadID              = -1;
		this.hasFetchedEntries            = false;
		this.configurationFetchInFlight   = false;

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

		if (!updateJSON.entries)
			return;

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
			return true;

		if (attrs.loadingtext !== undefined)
			this.loadingtext = attrs.loadingtext;
		if (attrs.selectevent !== undefined)
			this.selectevent = attrs.selectevent;
		if (attrs.entriesperpage !== undefined)
			this.entriesperpage = parseInt(attrs.entriesperpage) || this.entriesperpage;
		if (attrs.selectionvalueuuid !== undefined)
			this.selectionvalueuuid = attrs.selectionvalueuuid;
		if (attrs.buttonvalueuuid !== undefined)
			this.buttonvalueuuid = attrs.buttonvalueuuid;
		if (attrs.schema !== undefined)
			this.configurationSchema = attrs.schema;
		if (attrs.caption !== undefined)
			this.caption = attrs.caption;
		if (attrs.visible !== undefined)
			this.visible = (attrs.visible === "1" || attrs.visible === true || attrs.visible === "true");

		let shouldFetch = !this.hasFetchedEntries;

		if (attrs.configurationlistheadid !== undefined) {
			let headID = parseInt(attrs.configurationlistheadid);
			// The head id is a content hash (see frontendWriteItemToJSON) that also encodes
			// which version is active, so it can change in either direction. Refetch whenever
			// it differs - not just when it grows - otherwise activating a version does not
			// refresh the active-marker / list.
			if (!isNaN(headID) && headID !== this.lastKnownHeadID) {
				this.lastKnownHeadID = headID;
				shouldFetch = true;
			}
		}

		if (!shouldFetch)
			return true;
		if (this.configurationFetchInFlight)
			return true;

		this.configurationFetchInFlight = true;

		let app = this.page.application;
		let requestURL = "/configurations";
		if (this.configurationSchema && this.configurationSchema.length > 0)
			requestURL += "?schema=" + encodeURIComponent(this.configurationSchema);

		app.axiosGetRequest(requestURL)
		.then(resultJSON => {
			this.configurationFetchInFlight = false;
			this.hasFetchedEntries = true;

			if (resultJSON.data && resultJSON.data.configurations) {
				let newEntries = [];
				for (let entry of resultJSON.data.configurations) {
					newEntries.push({
						configurationActive:    !!entry.configurationactive,
						configurationVersion:   entry.configurationversion   || 0,
						userName:               entry.username               || "",
						configurationUUID:      entry.configurationuuid      || Common.nullUUID (),
						configurationTimestamp: entry.configurationtimestamp || "",
					});
				}

				// Show the most recent version first.
				newEntries.sort((a, b) => b.configurationVersion - a.configurationVersion);

				let oldCount = this.entries.length;
				for (let i = 0; i < oldCount; i++) this.entries.pop();
				for (let newEntry of newEntries) this.entries.push(newEntry);
			}
		})
		.catch(() => {
			this.configurationFetchInFlight = false;
		});

		return true;
	}

}
