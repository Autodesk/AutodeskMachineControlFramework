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


export default class AMCApplicationModule_PartList extends Common.AMCApplicationModule {

	constructor (page, moduleJSON)
	{
		Assert.ObjectValue (moduleJSON);
		super (page, moduleJSON.uuid, moduleJSON.type, moduleJSON.name || moduleJSON.uuid, moduleJSON.caption || "");
		this.registerClass ("amcModule_PartList");

		this.usesV2Frontend = true;

		this.builduuid    = Common.nullUUID ();
		this.loadingtext  = "Loading build details...";
		this.showdetails  = true;
		this.selectevent  = "";
		this.selectionvalueuuid = "";

		// Changes whenever parts of the build are disabled or re-enabled; triggers a re-fetch.
		this.partstateversion = 0;

		// Build details (fetched from api/build/<uuid>) and derived part list. The fetch happens
		// in this model class so both the Svelte and the Vue2 frontends share the same logic.
		this.details      = null;
		this.parts        = [];
		this.loading      = false;
		this.errorMessage = "";

		this.fetchedUUID    = "";
		this.fetchedVersion = 0;
		this.fetchInFlight  = false;

		this.updateFromJSON (moduleJSON);
	}


	updateFromJSON (updateJSON)
	{
		Assert.ObjectValue (updateJSON);

		if (updateJSON.loadingtext)
			this.loadingtext = Assert.StringValue (updateJSON.loadingtext);
		if (updateJSON.builduuid)
			this.builduuid = updateJSON.builduuid;
		if (updateJSON.selectevent !== undefined)
			this.selectevent = Assert.StringValue (updateJSON.selectevent);
		if (updateJSON.selectionvalueuuid !== undefined)
			this.selectionvalueuuid = Assert.StringValue (updateJSON.selectionvalueuuid);
		if (updateJSON.partstateversion !== undefined)
			this.partstateversion = Number (updateJSON.partstateversion) || 0;

		this.maybeFetchDetails ();
	}


	updateFromV2Attributes (attrs)
	{
		if (!attrs)
			return true;

		if (attrs.loadingtext !== undefined)
			this.loadingtext = attrs.loadingtext;
		if (attrs.showdetails !== undefined)
			this.showdetails = !(attrs.showdetails === false || attrs.showdetails === "0" || attrs.showdetails === "false");
		if (attrs.caption !== undefined)
			this.caption = attrs.caption;
		if (attrs.visible !== undefined)
			this.visible = (attrs.visible === "1" || attrs.visible === true || attrs.visible === "true");
		if (attrs.builduuid !== undefined)
			this.builduuid = attrs.builduuid;
		if (attrs.selectevent !== undefined)
			this.selectevent = attrs.selectevent;
		if (attrs.selectionvalueuuid !== undefined)
			this.selectionvalueuuid = attrs.selectionvalueuuid;
		if (attrs.partstateversion !== undefined)
			this.partstateversion = Number (attrs.partstateversion) || 0;

		this.maybeFetchDetails ();

		return true;
	}


	// Fetches the build details (name, layer count, thickness, size, thumbnail and part list)
	// whenever the selected build or its part state version changes. A null/empty UUID clears
	// the current details. A version-only change refreshes silently without a loading state.
	maybeFetchDetails ()
	{
		let uuid = this.builduuid || "";
		let nullUUID = Common.nullUUID ();

		if (!uuid || uuid === nullUUID) {
			this.details      = null;
			this.parts        = [];
			this.errorMessage = "";
			this.fetchedUUID  = "";
			this.fetchedVersion = 0;
			return;
		}

		let version = this.partstateversion;
		if ((uuid === this.fetchedUUID) && (version === this.fetchedVersion))
			return;
		if (this.fetchInFlight)
			return;

		let isNewBuild = (uuid !== this.fetchedUUID);

		this.fetchedUUID    = uuid;
		this.fetchedVersion = version;
		this.fetchInFlight  = true;
		if (isNewBuild) {
			this.loading      = true;
			this.errorMessage = "";
		}

		let app = this.page.application;

		app.axiosGetRequest ("/build/" + uuid)
		.then (resultJSON => {
			this.fetchInFlight = false;
			this.loading = false;

			// Ignore stale responses if the selection changed while loading.
			if (this.fetchedUUID === uuid) {
				let data = (resultJSON && resultJSON.data) ? resultJSON.data : null;
				this.details = data;
				this.errorMessage = "";

				let newParts = (data && Array.isArray (data.parts)) ? data.parts : [];
				let oldCount = this.parts.length;
				for (let i = 0; i < oldCount; i++) this.parts.pop ();
				for (let part of newParts) this.parts.push (part);
			}

			// Catch up with selection or version changes that arrived while loading.
			this.maybeFetchDetails ();
		})
		.catch (err => {
			this.fetchInFlight = false;
			this.loading = false;
			if (this.fetchedUUID !== uuid) {
				this.maybeFetchDetails ();
				return;
			}
			console.warn ("[PartList] build details load error:", (err && err.response) || err);
			this.details = null;
			let oldCount = this.parts.length;
			for (let i = 0; i < oldCount; i++) this.parts.pop ();
			this.errorMessage = "Could not load build details.";
			this.maybeFetchDetails ();
		});
	}

}
